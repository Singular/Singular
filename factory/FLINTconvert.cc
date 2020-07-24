/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file FLINTconvert.cc
 *
 * This file implements functions for conversion to FLINT (www.flintlib.org)
 * and back.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/



#include <config.h>


#include "canonicalform.h"
#include "fac_util.h"
#include "cf_iter.h"
#include "cf_factory.h"
#include "gmpext.h"
#include "singext.h"
#include "cf_algorithm.h"

#ifdef HAVE_OMALLOC
#define Alloc(L) omAlloc(L)
#define Free(A,L) omFreeSize(A,L)
#else
#define Alloc(L) malloc(L)
#define Free(A,L) free(A)
#endif

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
#ifndef __GMP_BITS_PER_MP_LIMB
#define __GMP_BITS_PER_MP_LIMB GMP_LIMB_BITS
#endif
#include <flint/fmpz.h>
#include <flint/fmpq.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_mod_poly.h>
#include <flint/nmod_poly.h>
#include <flint/fmpq_poly.h>
#include <flint/nmod_mat.h>
#include <flint/fmpz_mat.h>
#if ( __FLINT_RELEASE >= 20400)
#include <flint/fq.h>
#include <flint/fq_poly.h>
#include <flint/fq_nmod.h>
#include <flint/fq_nmod_poly.h>
#include <flint/fq_nmod_mat.h>
#endif
#if ( __FLINT_RELEASE >= 20503)
#include <flint/fmpq_mpoly.h>

// planed, but not yet in FLINT: #if (__FLINT_RELEASE < 20602)
// helper for fq_nmod_t -> nmod_poly_t
static void fq_nmod_get_nmod_poly(nmod_poly_t a, const fq_nmod_t b, const fq_nmod_ctx_t ctx)
{
    FLINT_ASSERT(b->mod.n == ctx->modulus->mod.n);
    a->mod = ctx->modulus->mod;
    nmod_poly_set(a, b);
}

// helper for nmod_poly_t -> fq_nmod_t
static void fq_nmod_set_nmod_poly(fq_nmod_t a, const nmod_poly_t b, const fq_nmod_ctx_t ctx)
{
    FLINT_ASSERT(a->mod.n == b->mod.n);
    FLINT_ASSERT(a->mod.n == ctx->modulus->mod.n);
    nmod_poly_set(a, b);
    fq_nmod_reduce(a, ctx);
}
//#endif


#endif
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
    f.mpzval(gmp_val);
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

CanonicalForm convertFmpz2CF (const fmpz_t coefficient)
{
  if (fmpz_cmp_si (coefficient, MINIMMEDIATE) >= 0 &&
      fmpz_cmp_si (coefficient, MAXIMMEDIATE) <= 0)
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
}

CanonicalForm
convertFmpz_poly_t2FacCF (const fmpz_poly_t poly, const Variable& x)
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

void
convertFacCF2nmod_poly_t (nmod_poly_t result, const CanonicalForm& f)
{
  bool save_sym_ff= isOn (SW_SYMMETRIC_FF);
  if (save_sym_ff) Off (SW_SYMMETRIC_FF);
  nmod_poly_init2 (result, getCharacteristic(), degree (f)+1);
  for (CFIterator i= f; i.hasTerms(); i++)
  {
    CanonicalForm c= i.coeff();
    if (!c.isImm()) c=c.mapinto(); //c%= getCharacteristic();
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

CanonicalForm
convertnmod_poly_t2FacCF (const nmod_poly_t poly, const Variable& x)
{
  CanonicalForm result= 0;
  for (int i= 0; i < nmod_poly_length (poly); i++)
  {
    ulong coeff= nmod_poly_get_coeff_ui (poly, i);
    if (coeff != 0)
      result += CanonicalForm ((long)coeff)*power (x,i);
  }
  return result;
}

void convertCF2Fmpq (fmpq_t result, const CanonicalForm& f)
{
  //ASSERT (isOn (SW_RATIONAL), "expected rational");
  if (f.isImm ())
  {
    fmpq_set_si (result, f.intval(), 1);
  }
  else if(f.inQ())
  {
    mpz_t gmp_val;
    gmp_numerator (f, gmp_val);
    fmpz_set_mpz (fmpq_numref (result), gmp_val);
    mpz_clear (gmp_val);
    gmp_denominator (f, gmp_val);
    fmpz_set_mpz (fmpq_denref (result), gmp_val);
    mpz_clear (gmp_val);
  }
  else if(f.inZ())
  {
    mpz_t gmp_val;
    f.mpzval(gmp_val);
    fmpz_set_mpz (fmpq_numref (result), gmp_val);
    mpz_clear (gmp_val);
    fmpz_one(fmpq_denref(result));
  }
  else
  {
    printf("wrong type\n");
  }
}

CanonicalForm convertFmpq2CF (const fmpq_t q)
{
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat)
    On (SW_RATIONAL);

  CanonicalForm num, den;
  mpz_t nnum, nden;
  mpz_init (nnum);
  mpz_init (nden);
  fmpz_get_mpz (nnum, fmpq_numref (q));
  fmpz_get_mpz (nden, fmpq_denref (q));

  CanonicalForm result;
  if (mpz_is_imm (nden))
  {
    if (mpz_is_imm(nnum))
    {
      num= CanonicalForm (mpz_get_si(nnum));
      den= CanonicalForm (mpz_get_si(nden));
      mpz_clear (nnum);
      mpz_clear (nden);
      result= num/den;
    }
    else if (mpz_cmp_si(nden,1)==0)
    {
      result= CanonicalForm( CFFactory::basic(nnum));
      mpz_clear (nden);
    }
    else
      result= CanonicalForm( CFFactory::rational( nnum, nden, false));
  }
  else
  {
    result= CanonicalForm( CFFactory::rational( nnum, nden, false));
  }
  if (!isRat)
    Off (SW_RATIONAL);
  return result;
}

CanonicalForm
convertFmpq_poly_t2FacCF (const fmpq_poly_t p, const Variable& x)
{
  CanonicalForm result= 0;
  fmpq_t coeff;
  long n= p->length;
  for (long i= 0; i < n; i++)
  {
    fmpq_init (coeff);
    fmpq_poly_get_coeff_fmpq (coeff, p, i);
    if (fmpq_is_zero (coeff))
    {
      fmpq_clear (coeff);
      continue;
    }
    result += convertFmpq2CF (coeff)*power (x, i);
    fmpq_clear (coeff);
  }
  return result;
}

void convertFacCF2Fmpz_array (fmpz* result, const CanonicalForm& f)
{
  for (CFIterator i= f; i.hasTerms(); i++)
    convertCF2Fmpz (&result[i.exp()], i.coeff());
}

void convertFacCF2Fmpq_poly_t (fmpq_poly_t result, const CanonicalForm& f)
{
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat)
    On (SW_RATIONAL);

  fmpq_poly_init2 (result, degree (f)+1);
  _fmpq_poly_set_length (result, degree (f) + 1);
  CanonicalForm den= bCommonDen (f);
  convertFacCF2Fmpz_array (fmpq_poly_numref (result), f*den);
  convertCF2Fmpz (fmpq_poly_denref (result), den);

  if (!isRat)
    Off (SW_RATIONAL);
}

CFFList
convertFLINTnmod_poly_factor2FacCFFList (const nmod_poly_factor_t fac,
                                          const mp_limb_t leadingCoeff,
                                          const Variable& x
                                         )
{
  CFFList result;
  if (leadingCoeff != 1)
    result.insert (CFFactor (CanonicalForm ((long) leadingCoeff), 1));

  long i;

  for (i = 0; i < fac->num; i++)
    result.append (CFFactor (convertnmod_poly_t2FacCF (
                             (nmod_poly_t &)fac->p[i],x),
                             fac->exp[i]));
  return result;
}

#if __FLINT_RELEASE >= 20503
CFFList
convertFLINTfmpz_poly_factor2FacCFFList (
                   const fmpz_poly_factor_t fac, ///< [in] a nmod_poly_factor_t
                   const Variable& x       ///< [in] variable the result should
                                           ///< have
                                        )

{
  CFFList result;
  long i;

  result.append (CFFactor(convertFmpz2CF(&fac->c),1));

  for (i = 0; i < fac->num; i++)
    result.append (CFFactor (convertFmpz_poly_t2FacCF (
                             (fmpz_poly_t &)fac->p[i],x),
                             fac->exp[i]));
  return result;
}
#endif

#if __FLINT_RELEASE >= 20400
CFFList
convertFLINTFq_nmod_poly_factor2FacCFFList (const fq_nmod_poly_factor_t fac,
                                       const Variable& x, const Variable& alpha,
                                       const fq_nmod_ctx_t fq_con
                                         )
{
  CFFList result;

  long i;

  for (i = 0; i < fac->num; i++)
    result.append (CFFactor (convertFq_nmod_poly_t2FacCF (
                             (fq_nmod_poly_t &)fac->poly[i], x, alpha, fq_con),
                             fac->exp[i]));
  return result;
}
#endif

void
convertFacCF2Fmpz_mod_poly_t (fmpz_mod_poly_t result, const CanonicalForm& f,
                              const fmpz_t p)
{
  fmpz_mod_poly_init2 (result, p, degree (f) + 1);
  fmpz_poly_t buf;
  convertFacCF2Fmpz_poly_t (buf, f);
  fmpz_mod_poly_set_fmpz_poly (result, buf);
  fmpz_poly_clear (buf);
}

CanonicalForm
convertFmpz_mod_poly_t2FacCF (const fmpz_mod_poly_t poly, const Variable& x,
                              const modpk& b)
{
  fmpz_poly_t buf;
  fmpz_poly_init (buf);
  fmpz_mod_poly_get_fmpz_poly (buf, poly);
  CanonicalForm result= convertFmpz_poly_t2FacCF (buf, x);
  fmpz_poly_clear (buf);
  return b (result);
}

#if __FLINT_RELEASE >= 20400
void
convertFacCF2Fq_nmod_t (fq_nmod_t result, const CanonicalForm& f,
                        const fq_nmod_ctx_t ctx)
{
  bool save_sym_ff= isOn (SW_SYMMETRIC_FF);
  if (save_sym_ff) Off (SW_SYMMETRIC_FF);
  #if __FLINT_RELEASE >= 20503
  nmod_poly_t res;
  nmod_poly_init(res,getCharacteristic());
  #endif
  for (CFIterator i= f; i.hasTerms(); i++)
  {
    CanonicalForm c= i.coeff();
    if (!c.isImm()) c=c.mapinto(); //c%= getCharacteristic();
    if (!c.isImm())
    {  //This case will never happen if the characteristic is in fact a prime
       // number, since all coefficients are represented as immediates
       printf("convertFacCF2Fq_nmod_t: coefficient not immediate!, char=%d\n",
              getCharacteristic());
    }
    else
    {
      STICKYASSERT (i.exp() <= fq_nmod_ctx_degree(ctx), "convertFacCF2Fq_nmod_t: element is not reduced");
      #if __FLINT_RELEASE >= 20503
      nmod_poly_set_coeff_ui (res, i.exp(), c.intval());
      #else
      nmod_poly_set_coeff_ui (result, i.exp(), c.intval());
      #endif
    }
  }
  #if __FLINT_RELEASE >= 20503
  fq_nmod_init(result,ctx);
  fq_nmod_set_nmod_poly(result,res,ctx);
  #endif
  if (save_sym_ff) On (SW_SYMMETRIC_FF);
}

CanonicalForm
convertFq_nmod_t2FacCF (const fq_nmod_t poly, const Variable& alpha, const fq_nmod_ctx_t ctx)
{
  return convertnmod_poly_t2FacCF (poly, alpha);
}

void
convertFacCF2Fq_t (fq_t result, const CanonicalForm& f, const fq_ctx_t ctx)
{
  fmpz_poly_init2 (result, fq_ctx_degree(ctx));
  ASSERT (degree (f) < fq_ctx_degree (ctx), "input is not reduced");
  _fmpz_poly_set_length(result, degree(f)+1);
  for (CFIterator i= f; i.hasTerms(); i++)
    convertCF2Fmpz (fmpz_poly_get_coeff_ptr(result, i.exp()), i.coeff());
  _fmpz_vec_scalar_mod_fmpz (result->coeffs, result->coeffs, degree (f) + 1,
                             &ctx->p);
  _fmpz_poly_normalise (result);
}

CanonicalForm
convertFq_t2FacCF (const fq_t poly, const Variable& alpha)
{
  return convertFmpz_poly_t2FacCF (poly, alpha);
}

void
convertFacCF2Fq_poly_t (fq_poly_t result, const CanonicalForm& f,
                        const fq_ctx_t ctx)
{
  fq_poly_init2 (result, degree (f)+1, ctx);
  _fq_poly_set_length (result, degree (f) + 1, ctx);
  fmpz_poly_t buf;
  for (CFIterator i= f; i.hasTerms(); i++)
  {
    convertFacCF2Fmpz_poly_t (buf, i.coeff());
    _fmpz_vec_scalar_mod_fmpz (buf->coeffs, buf->coeffs, degree (i.coeff()) + 1,
                               &ctx->p);
    _fmpz_poly_normalise (buf);
    fq_poly_set_coeff (result, i.exp(), buf, ctx);
    fmpz_poly_clear (buf);
  }
}

void
convertFacCF2Fq_nmod_poly_t (fq_nmod_poly_t result, const CanonicalForm& f,
                             const fq_nmod_ctx_t ctx)
{
  fq_nmod_poly_init2 (result, degree (f)+1, ctx);
  _fq_nmod_poly_set_length (result, degree (f) + 1, ctx);
  fq_nmod_t buf;
  fq_nmod_init2 (buf, ctx);
  for (CFIterator i= f; i.hasTerms(); i++)
  {
    convertFacCF2Fq_nmod_t (buf, i.coeff(), ctx);
    fq_nmod_poly_set_coeff (result, i.exp(), buf, ctx);
    fq_nmod_zero (buf, ctx);
  }
  fq_nmod_clear (buf, ctx);
}

CanonicalForm
convertFq_poly_t2FacCF (const fq_poly_t p, const Variable& x,
                        const Variable& alpha, const fq_ctx_t ctx)
{
  CanonicalForm result= 0;
  fq_t coeff;
  long n= fq_poly_length (p, ctx);
  fq_init2 (coeff, ctx);
  for (long i= 0; i < n; i++)
  {
    fq_poly_get_coeff (coeff, p, i, ctx);
    if (fq_is_zero (coeff, ctx))
      continue;
    result += convertFq_t2FacCF (coeff, alpha)*power (x, i);
    fq_zero (coeff, ctx);
  }
  fq_clear (coeff, ctx);

  return result;
}

CanonicalForm
convertFq_nmod_poly_t2FacCF (const fq_nmod_poly_t p, const Variable& x,
                             const Variable& alpha, const fq_nmod_ctx_t ctx)
{
  CanonicalForm result= 0;
  fq_nmod_t coeff;
  long n= fq_nmod_poly_length (p, ctx);
  fq_nmod_init2 (coeff, ctx);
  for (long i= 0; i < n; i++)
  {
    fq_nmod_poly_get_coeff (coeff, p, i, ctx);
    if (fq_nmod_is_zero (coeff, ctx))
      continue;
    result += convertFq_nmod_t2FacCF (coeff, alpha, ctx)*power (x, i);
    fq_nmod_zero (coeff, ctx);
  }
  fq_nmod_clear (coeff, ctx);

  return result;
}
#endif

void convertFacCFMatrix2Fmpz_mat_t (fmpz_mat_t M, const CFMatrix &m)
{
  fmpz_mat_init (M, (long) m.rows(), (long) m.columns());

  int i,j;
  for(i=m.rows();i>0;i--)
  {
    for(j=m.columns();j>0;j--)
    {
      convertCF2Fmpz (fmpz_mat_entry (M,i-1,j-1), m(i,j));
    }
  }
}
CFMatrix* convertFmpz_mat_t2FacCFMatrix(const fmpz_mat_t m)
{
  CFMatrix *res=new CFMatrix(fmpz_mat_nrows (m),fmpz_mat_ncols (m));
  int i,j;
  for(i=res->rows();i>0;i--)
  {
    for(j=res->columns();j>0;j--)
    {
      (*res)(i,j)=convertFmpz2CF(fmpz_mat_entry (m,i-1,j-1));
    }
  }
  return res;
}

void convertFacCFMatrix2nmod_mat_t (nmod_mat_t M, const CFMatrix &m)
{
  nmod_mat_init (M, (long) m.rows(), (long) m.columns(), getCharacteristic());

  bool save_sym_ff= isOn (SW_SYMMETRIC_FF);
  if (save_sym_ff) Off (SW_SYMMETRIC_FF);
  int i,j;
  for(i=m.rows();i>0;i--)
  {
    for(j=m.columns();j>0;j--)
    {
      if(!(m(i,j)).isImm()) printf("convertFacCFMatrix2FLINTmat_zz_p: not imm.\n");
      nmod_mat_entry (M,i-1,j-1)= (m(i,j)).intval();
    }
  }
  if (save_sym_ff) On (SW_SYMMETRIC_FF);
}

CFMatrix* convertNmod_mat_t2FacCFMatrix(const nmod_mat_t m)
{
  CFMatrix *res=new CFMatrix(nmod_mat_nrows (m), nmod_mat_ncols (m));
  int i,j;
  for(i=res->rows();i>0;i--)
  {
    for(j=res->columns();j>0;j--)
    {
      (*res)(i,j)=CanonicalForm((long) nmod_mat_entry (m, i-1, j-1));
    }
  }
  return res;
}

#if __FLINT_RELEASE >= 20400
void
convertFacCFMatrix2Fq_nmod_mat_t (fq_nmod_mat_t M,
                                  const fq_nmod_ctx_t fq_con, const CFMatrix &m)
{
  fq_nmod_mat_init (M, (long) m.rows(), (long) m.columns(), fq_con);
  int i,j;
  for(i=m.rows();i>0;i--)
  {
    for(j=m.columns();j>0;j--)
    {
      convertFacCF2nmod_poly_t (M->rows[i-1]+j-1, m (i,j));
    }
  }
}

CFMatrix*
convertFq_nmod_mat_t2FacCFMatrix(const fq_nmod_mat_t m,
                                 const fq_nmod_ctx_t& fq_con,
                                 const Variable& alpha)
{
  CFMatrix *res=new CFMatrix(fq_nmod_mat_nrows (m, fq_con),
                             fq_nmod_mat_ncols (m, fq_con));
  int i,j;
  for(i=res->rows();i>0;i--)
  {
    for(j=res->columns();j>0;j--)
    {
      (*res)(i,j)=convertFq_nmod_t2FacCF (fq_nmod_mat_entry (m, i-1, j-1),
                                          alpha, fq_con);
    }
  }
  return res;
}
#endif
#if __FLINT_RELEASE >= 20503
static void convFlint_RecPP ( const CanonicalForm & f, ulong * exp, nmod_mpoly_t result, nmod_mpoly_ctx_t ctx, int N )
{
  // assume f!=0
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[N-l] = i.exp();
      convFlint_RecPP( i.coeff(), exp, result, ctx, N );
    }
    exp[N-l] = 0;
  }
  else
  {
    int c=f.intval(); // with Off(SW_SYMMETRIC_FF): 0<=c<p
    nmod_mpoly_push_term_ui_ui(result,c,exp,ctx);
  }
}

static void convFlint_RecPP ( const CanonicalForm & f, ulong * exp, fmpq_mpoly_t result, fmpq_mpoly_ctx_t ctx, int N )
{
  // assume f!=0
  if ( ! f.inBaseDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[N-l] = i.exp();
      convFlint_RecPP( i.coeff(), exp, result, ctx, N );
    }
    exp[N-l] = 0;
  }
  else
  {
    fmpq_t c;
    fmpq_init(c);
    convertCF2Fmpq(c,f);
    fmpq_mpoly_push_term_fmpq_ui(result,c,exp,ctx);
    fmpq_clear(c);
  }
}

void convFactoryPFlintMP ( const CanonicalForm & f, nmod_mpoly_t res, nmod_mpoly_ctx_t ctx, int N )
{
  if (f.isZero()) return;
  ulong * exp = (ulong*)Alloc(N*sizeof(ulong));
  memset(exp,0,N*sizeof(ulong));
  bool save_sym_ff= isOn (SW_SYMMETRIC_FF);
  if (save_sym_ff) Off (SW_SYMMETRIC_FF);
  convFlint_RecPP( f, exp, res, ctx, N );
  if (save_sym_ff) On(SW_SYMMETRIC_FF);
  Free(exp,N*sizeof(ulong));
}

void convFactoryPFlintMP ( const CanonicalForm & f, fmpq_mpoly_t res, fmpq_mpoly_ctx_t ctx, int N )
{
  if (f.isZero()) return;
  ulong * exp = (ulong*)Alloc(N*sizeof(ulong));
  memset(exp,0,N*sizeof(ulong));
  convFlint_RecPP( f, exp, res, ctx, N );
  fmpq_mpoly_reduce(res,ctx);
  Free(exp,N*sizeof(ulong));
}

CanonicalForm convFlintMPFactoryP(nmod_mpoly_t f, nmod_mpoly_ctx_t ctx, int N)
{
  CanonicalForm result;
  int d=nmod_mpoly_length(f,ctx)-1;
  ulong* exp=(ulong*)Alloc(N*sizeof(ulong));
  for(int i=d; i>=0; i--)
  {
    ulong c=nmod_mpoly_get_term_coeff_ui(f,i,ctx);
    nmod_mpoly_get_term_exp_ui(exp,f,i,ctx);
    CanonicalForm term=(int)c;
    for ( int i = 0; i <N; i++ )
    {
      if (exp[i]!=0) term*=CanonicalForm( Variable( N-i ), exp[i] );
    }
    result+=term;
  }
  Free(exp,N*sizeof(ulong));
  return result;
}

CanonicalForm convFlintMPFactoryP(fmpq_mpoly_t f, fmpq_mpoly_ctx_t ctx, int N)
{
  CanonicalForm result;
  int d=fmpq_mpoly_length(f,ctx)-1;
  ulong* exp=(ulong*)Alloc(N*sizeof(ulong));
  fmpq_t c;
  fmpq_init(c);
  for(int i=d; i>=0; i--)
  {
    fmpq_mpoly_get_term_coeff_fmpq(c,f,i,ctx);
    fmpq_mpoly_get_term_exp_ui(exp,f,i,ctx);
    CanonicalForm term=convertFmpq2CF(c);
    for ( int i = 0; i <N; i++ )
    {
      if (exp[i]!=0) term*=CanonicalForm( Variable( N-i ), exp[i] );
    }
    result+=term;
  }
  fmpq_clear(c);
  Free(exp,N*sizeof(ulong));
  return result;
}

CanonicalForm mulFlintMP_Zp(const CanonicalForm& F,int lF, const CanonicalForm& G, int lG,int m)
{
  int bits=SI_LOG2(m)+1;
  int N=F.level();
  nmod_mpoly_ctx_t ctx;
  nmod_mpoly_ctx_init(ctx,N,ORD_LEX,getCharacteristic());
  nmod_mpoly_t f,g,res;
  nmod_mpoly_init3(f,lF,bits,ctx);
  nmod_mpoly_init3(g,lG,bits,ctx);
  convFactoryPFlintMP(F,f,ctx,N);
  convFactoryPFlintMP(G,g,ctx,N);
  nmod_mpoly_init(res,ctx);
  nmod_mpoly_mul(res,f,g,ctx);
  nmod_mpoly_clear(g,ctx);
  nmod_mpoly_clear(f,ctx);
  CanonicalForm RES=convFlintMPFactoryP(res,ctx,N);
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_ctx_clear(ctx);
  return RES;
}

CanonicalForm mulFlintMP_QQ(const CanonicalForm& F,int lF, const CanonicalForm& G, int lG, int m)
{
  int bits=SI_LOG2(m)+1;
  int N=F.level();
  fmpq_mpoly_ctx_t ctx;
  fmpq_mpoly_ctx_init(ctx,N,ORD_LEX);
  fmpq_mpoly_t f,g,res;
  fmpq_mpoly_init3(f,lF,bits,ctx);
  fmpq_mpoly_init3(g,lG,bits,ctx);
  convFactoryPFlintMP(F,f,ctx,N);
  convFactoryPFlintMP(G,g,ctx,N);
  fmpq_mpoly_init(res,ctx);
  fmpq_mpoly_mul(res,f,g,ctx);
  fmpq_mpoly_clear(g,ctx);
  fmpq_mpoly_clear(f,ctx);
  CanonicalForm RES=convFlintMPFactoryP(res,ctx,N);
  fmpq_mpoly_clear(res,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  return RES;
}

CanonicalForm gcdFlintMP_Zp(const CanonicalForm& F, const CanonicalForm& G)
{
  int N=F.level();
  int lf,lg,m=1<<MPOLY_MIN_BITS;
  lf=size_maxexp(F,m);
  lg=size_maxexp(G,m);
  int bits=SI_LOG2(m)+1;
  nmod_mpoly_ctx_t ctx;
  nmod_mpoly_ctx_init(ctx,N,ORD_LEX,getCharacteristic());
  nmod_mpoly_t f,g,res;
  nmod_mpoly_init3(f,lf,bits,ctx);
  nmod_mpoly_init3(g,lg,bits,ctx);
  convFactoryPFlintMP(F,f,ctx,N);
  convFactoryPFlintMP(G,g,ctx,N);
  nmod_mpoly_init(res,ctx);
  int ok=nmod_mpoly_gcd(res,f,g,ctx);
  nmod_mpoly_clear(g,ctx);
  nmod_mpoly_clear(f,ctx);
  CanonicalForm RES=1;
  if (ok)
  {
    RES=convFlintMPFactoryP(res,ctx,N);
  }
  nmod_mpoly_clear(res,ctx);
  nmod_mpoly_ctx_clear(ctx);
  return RES;
}

static CanonicalForm b_content ( const CanonicalForm & f )
{
    if ( f.inCoeffDomain() )
        return f;
    else
    {
        CanonicalForm result = 0;
        CFIterator i;
        for ( i = f; i.hasTerms() && (!result.isOne()); i++ )
            result=bgcd( b_content(i.coeff()) , result );
        return result;
    }
}


CanonicalForm gcdFlintMP_QQ(const CanonicalForm& F, const CanonicalForm& G)
{
  int N=F.level();
  fmpq_mpoly_ctx_t ctx;
  fmpq_mpoly_ctx_init(ctx,N,ORD_LEX);
  fmpq_mpoly_t f,g,res;
  fmpq_mpoly_init(f,ctx);
  fmpq_mpoly_init(g,ctx);
  convFactoryPFlintMP(F,f,ctx,N);
  convFactoryPFlintMP(G,g,ctx,N);
  fmpq_mpoly_init(res,ctx);
  int ok=fmpq_mpoly_gcd(res,f,g,ctx);
  fmpq_mpoly_clear(g,ctx);
  fmpq_mpoly_clear(f,ctx);
  CanonicalForm RES=1;
  if (ok)
  {
    // Flint normalizes the gcd to be monic.
    // Singular wants a gcd defined over ZZ that is primitive and has a positive leading coeff.
    if (!fmpq_mpoly_is_zero(res, ctx))
    {
      fmpq_t content;
      fmpq_init(content);
      fmpq_mpoly_content(content, res, ctx);
      fmpq_mpoly_scalar_div_fmpq(res, res, content, ctx);
      fmpq_clear(content);
    }
    RES=convFlintMPFactoryP(res,ctx,N);
    // gcd(2x,4x) should be 2x, so RES should also have the gcd(lc(F),lc(G))
    RES*=bgcd(b_content(F),b_content(G));
  }
  fmpq_mpoly_clear(res,ctx);
  fmpq_mpoly_ctx_clear(ctx);
  return RES;
}

#endif

#endif


