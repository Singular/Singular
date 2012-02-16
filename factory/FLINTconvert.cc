
#include <config.h>

#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_factory.h"
#include "gmpext.h"
#include "singext.h"
#include "cf_algorithm.h"

#ifdef HAVE_FLINT
#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#include <fmpz.h>
#include <fmpq.h>
#include <fmpz_poly.h>
#include <nmod_poly.h>
#include <fmpq_poly.h>
#ifdef __cplusplus
}
#endif

#include "FLINTconvert.h"

void convertCF2Fmpz (fmpz_t result, const CanonicalForm& f)
{
  if (f.isImm())
    fmpz_set_si (result, f.intval());
  else
  {
    mpz_t gmp_val;
    gmp_val[0]= *getmpi(f.getval());
    fmpz_set_mpz (result, gmp_val);
    mpz_clear (gmp_val);
  }
}

void convertFacCF2Fmpz_poly_t (fmpz_poly_t result, const CanonicalForm& f)
{
  fmpz_poly_init2 (result, degree (f)+1);
  _fmpz_poly_set_length(result, degree(f)+1);
  for (CFIterator i= f; i.hasTerms(); i++)
    convertCF2Fmpz (fmpz_poly_get_coeff_ptr(result, i.exp()), i.coeff());
}

CanonicalForm convertFmpz2CF (fmpz_t coefficient)
{
  if (fmpz_cmp_si (coefficient, MINIMMEDIATE) >= 0 && fmpz_cmp_si (coefficient, MAXIMMEDIATE) <= 0) //this should work with flint 2.3 now
  {
    long coeff= fmpz_get_si (coefficient);
    return CanonicalForm (coeff);
  }
  else
  {
    mpz_t gmp_val;
    mpz_init (gmp_val);
    fmpz_get_mpz (gmp_val, coefficient);
    CanonicalForm result= CanonicalForm (CFFactory::basic (gmp_val));
    return result;
  }

  /*mpz_t gmp_val;
  mpz_init (gmp_val);
  fmpz_get_mpz (gmp_val, coefficient); //TODO fmpz_fits_si
  if (mpz_is_imm (gmp_val)) //TODO for long
  {
    long coeff= mpz_get_si (gmp_val);
    mpz_clear (gmp_val);
    return CanonicalForm (coeff);
  }

  CanonicalForm result= CanonicalForm (CFFactory::basic (gmp_val));
  return result;*/
}

CanonicalForm convertFmpz_poly_t2FacCF (fmpz_poly_t poly, const Variable& x)
{
  CanonicalForm result= 0;
  fmpz* coeff;
  for (int i= 0; i < fmpz_poly_length (poly); i++)
  {
    coeff= fmpz_poly_get_coeff_ptr (poly, i);
    if (!fmpz_is_zero (coeff))
      result += convertFmpz2CF (coeff)*power (x,i);
  }
  return result;
}

void convertFacCF2nmod_poly_t (nmod_poly_t result, const CanonicalForm& f)
{
  bool save_sym_ff= isOn (SW_SYMMETRIC_FF);
  if (save_sym_ff) Off (SW_SYMMETRIC_FF);
  nmod_poly_init2 (result, getCharacteristic(), degree (f)+1);
  for (CFIterator i= f; i.hasTerms(); i++)
  {
    CanonicalForm c= i.coeff();
    if (!c.isImm()) c.mapinto(); //c%= getCharacteristic();
    if (!c.isImm())
    {  //This case will never happen if the characteristic is in fact a prime
       // number, since all coefficients are represented as immediates
       printf("convertCF2nmod_poly_t: coefficient not immediate!, char=%d\n",
              getCharacteristic());
    }
    else
      nmod_poly_set_coeff_ui (result, i.exp(), c.intval());
  }
  if (save_sym_ff) On (SW_SYMMETRIC_FF);
}

CanonicalForm convertnmod_poly_t2FacCF (nmod_poly_t poly, const Variable& x)
{
  CanonicalForm result= 0;
  for (int i= 0; i < nmod_poly_length (poly); i++)
  {
    ulong coeff= nmod_poly_get_coeff_ui (poly, i);
    if (!coeff == 0)
      result += CanonicalForm (coeff)*power (x,i);
  }
  return result;
}

void convertCF2Fmpq (fmpq_t result, const CanonicalForm& f) //TODO wie oben bei CF2Fmpz
{
  ASSERT (isOn (SW_RATIONAL), "expected rational");
  fmpz_t tmp1, tmp2;
  fmpz_init (tmp1);
  fmpz_init (tmp2);
  if (f.isImm ())
  {
    fmpz_set_si (tmp1, f.num().intval());
    fmpz_set_si (tmp2, f.den().intval());
  }
  else
  {
    mpz_t gmp_val;
    gmp_numerator (f, gmp_val);
    fmpz_set_mpz (tmp1, gmp_val);
    mpz_clear (gmp_val);
    gmp_denominator (f, gmp_val);
    fmpz_set_mpz (tmp2, gmp_val);
    mpz_clear (gmp_val);
  }

  fmpz_set (fmpq_numref (result), tmp1);
  fmpz_set (fmpq_denref (result), tmp2);
  fmpz_clear (tmp1);
  fmpz_clear (tmp2);
}

CanonicalForm convertFmpq_t2CF (const fmpq_t q)
{
  ASSERT (isOn (SW_RATIONAL), "expected rational");
  //TODO as for Fmpz check first if num and den are immediate

  CanonicalForm num, den;
  mpz_t nnum, nden;
  mpz_init (nnum);
  mpz_init (nden);
  fmpz_get_mpz (nnum, fmpq_numref (q));
  fmpz_get_mpz (nden, fmpq_denref (q));

  if (mpz_is_imm (nnum) && mpz_is_imm (nden))
  {
    num= CanonicalForm (mpz_get_si(nnum));
    den= CanonicalForm (mpz_get_si(nden));
    mpz_clear (nnum);
    mpz_clear (nden);
    return num/den;
  }
  else
    return make_cf (nnum, nden, false);
}

CanonicalForm convertFmpq_poly_t2FacCF (fmpq_poly_t p, const Variable& x)
{
#if 0
  ASSERT (isOn (SW_RATIONAL), "expected poly over Q");
  CanonicalForm den= convertFmpz2CF (fmpq_poly_denref (p));
  fmpz_poly_t FLINTnum;
  long n= fmpq_poly_length (p);
  fmpz_poly_init2 (FLINTnum, fmpq_poly_length (p));

  for (long i= 0; i < n; i++)
    fmpz_set (FLINTnum->coeffs + i,fmpq_poly_numref (p) + i);
  _fmpz_poly_set_length (FLINTnum, n);
  CanonicalForm result= convertFmpz_poly_t2FacCF (FLINTnum, x);
  fmpz_poly_clear (FLINTnum);
  return result/den;
#else
  CanonicalForm result= 0;
  fmpq_t coeff;
  long n= fmpq_poly_length (p);
  for (long i= 0; i < n; i++)
  {
    fmpq_init (coeff);
    fmpq_poly_get_coeff_fmpq (coeff, p, i);
    if (fmpq_is_zero (coeff))
    {
      fmpq_clear (coeff);
      continue;
    }
    result += convertFmpq_t2CF (coeff)*power (x, i);
    fmpq_clear (coeff);
  }
  return result;
#endif
}

void convertFacCF2Fmpz_array (fmpz* result, const CanonicalForm& f)
{
  for (CFIterator i= f; i.hasTerms(); i++)
    convertCF2Fmpz (&result[i.exp()], i.coeff());
}

//TODO multiply by bCommonDen and convertFacCF2Fmpz_poly_t
void convertFacCF2Fmpq_poly_t (fmpq_poly_t result, const CanonicalForm& f)
{
  ASSERT (isOn (SW_RATIONAL), "expected poly over Q");

  fmpq_poly_init2 (result, degree (f)+1);
  _fmpq_poly_set_length (result, degree (f) + 1);
  CanonicalForm den= bCommonDen (f);
  convertFacCF2Fmpz_array (fmpq_poly_numref (result), f*den);
  convertCF2Fmpz (fmpq_poly_denref (result), den);
  /*fmpq_t coeff;
  for (CFIterator i= f; i.hasTerms(); i++)
  {
    fmpq_init (coeff);
    convertCF2Fmpq (coeff, i.coeff());
    fmpq_poly_set_coeff_fmpq (result, i.exp(), coeff);
    fmpq_clear (coeff);
  }*/
}

CFFList
convertFLINTnmod_poly_factor2FacCFFList (nmod_poly_factor_t fac,
                                          mp_limb_t leadingCoeff,
                                          const Variable& x
                                         )
{
  CFFList result;
  result.insert (CFFactor (CanonicalForm ((long) leadingCoeff), 1));

  long i;

  for (i = 0; i < fac->num; i++)
    result.append (CFFactor (convertnmod_poly_t2FacCF ((nmod_poly_t &)fac->p[i],x),
                             fac->exp[i]));
  return result;
}

#endif


