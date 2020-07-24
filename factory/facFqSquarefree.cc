/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facFqSquarefree.cc
 *
 * This file provides functions for squarefrees factorizing over
 * \f$ F_{p} \f$ , \f$ F_{p}(\alpha ) \f$ or GF, as decribed in "Factoring
 * multivariate polynomials over a finite field" by L. Bernardin
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "canonicalform.h"

#include "cf_iter.h"
#include "cf_map.h"
#include "cf_util.h"
#include "cf_factory.h"
#include "facFqSquarefree.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

static inline
CanonicalForm
pthRoot (const CanonicalForm & F, int q)
{
  CanonicalForm A= F;
  int p= getCharacteristic ();
  if (A.inCoeffDomain())
  {
    A= power (A, q/p);
    return A;
  }
  else
  {
    CanonicalForm buf= 0;
    for (CFIterator i= A; i.hasTerms(); i++)
      buf= buf + power(A.mvar(), i.exp()/p)*pthRoot (i.coeff(), q);
    return buf;
  }
}

#ifdef HAVE_NTL
CanonicalForm
pthRoot (const CanonicalForm & F, const ZZ& q, const Variable& alpha)
{
  CanonicalForm A= F;
  int p= getCharacteristic ();
  if (A.inCoeffDomain())
  {
    zz_p::init (p);
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pX NTLA= convertFacCF2NTLzzpX (A);
    zz_pE NTLA2= to_zz_pE (NTLA);
    power (NTLA2, NTLA2, q/p);
    A= convertNTLzzpE2CF (NTLA2, alpha);
    return A;
  }
  else
  {
    CanonicalForm buf= 0;
    for (CFIterator i= A; i.hasTerms(); i++)
      buf= buf + power(A.mvar(), i.exp()/p)*pthRoot (i.coeff(), q, alpha);
    return buf;
  }
}
#endif

#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
CanonicalForm
pthRoot (const CanonicalForm & F, const fmpz_t& q, const Variable& alpha)
{
  CanonicalForm A= F;
  int p= getCharacteristic ();
  if (A.inCoeffDomain())
  {
    nmod_poly_t FLINTmipo;
    fq_nmod_ctx_t fq_con;
    fmpz_t qp;
    fq_nmod_t FLINTA;

    nmod_poly_init (FLINTmipo, p);
    convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

    fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

    fq_nmod_init2 (FLINTA, fq_con);

    convertFacCF2Fq_nmod_t (FLINTA, A, fq_con);

    fmpz_init_set (qp, q);
    fmpz_divexact_si (qp, qp, p);

    fq_nmod_pow (FLINTA, FLINTA, qp, fq_con);
    A= convertFq_nmod_t2FacCF (FLINTA, alpha, fq_con);

    fmpz_clear (qp);
    nmod_poly_clear (FLINTmipo);
    fq_nmod_clear (FLINTA, fq_con);
    fq_nmod_ctx_clear (fq_con);
    return A;
  }
  else
  {
    CanonicalForm buf= 0;
    for (CFIterator i= A; i.hasTerms(); i++)
      buf= buf + power(A.mvar(), i.exp()/p)*pthRoot (i.coeff(), q, alpha);
    return buf;
  }
}
#endif

CanonicalForm
maxpthRoot (const CanonicalForm & F, int q, int& l)
{
  CanonicalForm result= F;
  bool derivZero= true;
  l= 0;
  while (derivZero)
  {
    for (int i= 1; i <= result.level(); i++)
    {
      if (!deriv (result, Variable (i)).isZero())
      {
        derivZero= false;
        break;
      }
    }
    if (!derivZero)
      break;
    result= pthRoot (result, q);
    l++;
  }
  return result;
}

static inline
CFFList
sqrfPosDer (const CanonicalForm & F, const Variable & x,
            CanonicalForm & c)
{
  CanonicalForm b= deriv (F, x);
  c= gcd (F, b);
  CanonicalForm w= F/c;
  CanonicalForm v= b/c;
  CanonicalForm u= v - deriv (w, x);
  int j= 1;
  int p= getCharacteristic();
  CanonicalForm g;
  CFFList result;
  while (j < p - 1 && degree(u) >= 0)
  {
    g= gcd (w, u);
    if (!g.inCoeffDomain())
      result.append (CFFactor (g, j));
    w= w/g;
    c= c/w;
    v= u/g;
    u= v - deriv (w, x);
    j++;
  }
  if (!w.inCoeffDomain())
    result.append (CFFactor (w, j));
  return result;
}

CFFList
squarefreeFactorization (const CanonicalForm & F, const Variable & alpha)
{
  int p= getCharacteristic();
  CanonicalForm A= F;
  CFMap M;
  A= compress (A, M);
  Variable x= A.mvar();
  int l= x.level();
  int k;
  if (CFFactory::gettype() == GaloisFieldDomain)
    k= getGFDegree();
  else if (alpha.level() != 1)
    k= degree (getMipo (alpha));
  else
    k= 1;
  Variable buf, buf2;
  CanonicalForm tmp;

  CFFList tmp1, tmp2;
  bool found;
  for (int i= l; i > 0; i--)
  {
    buf= Variable (i);
    if (degree (deriv (A, buf)) >= 0)
    {
      tmp1= sqrfPosDer (A, buf, tmp);
      A= tmp;
      for (CFFListIterator j= tmp1; j.hasItem(); j++)
      {
        found= false;
        CFFListIterator k= tmp2;
        if (!k.hasItem() && !j.getItem().factor().inCoeffDomain()) tmp2.append (j.getItem());
        else
        {
          for (; k.hasItem(); k++)
          {
            if (k.getItem().exp() == j.getItem().exp())
            {
              k.getItem()= CFFactor (k.getItem().factor()*j.getItem().factor(),
                                     j.getItem().exp());
              found= true;
            }
          }
          if (found == false && !j.getItem().factor().inCoeffDomain())
            tmp2.append(j.getItem());
        }
      }
    }
  }

  bool degcheck= false;;
  for (int i= l; i > 0; i--)
  if (degree (A, Variable(i)) >= p)
    degcheck= true;

  if (degcheck == false && tmp1.isEmpty() && tmp2.isEmpty())
    return CFFList (CFFactor (F/Lc(F), 1));

  CanonicalForm buffer;
#if defined(HAVE_NTL) || (HAVE_FLINT && __FLINT_RELEASE >= 20400)
  if (alpha.level() == 1)
#endif
    buffer= pthRoot (A, ipower (p, k));
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
  else
  {
    fmpz_t qq;
    fmpz_init_set_ui (qq, p);
    fmpz_pow_ui (qq, qq, k);
    buffer= pthRoot (A, qq, alpha);
    fmpz_clear (qq);
  }
#elif defined(HAVE_NTL)
  else
  {
    ZZ q;
    power (q, p, k);
    buffer= pthRoot (A, q, alpha);
  }
#else
  factoryError("NTL/FLINT missing: squarefreeFactorization");
#endif

  tmp1= squarefreeFactorization (buffer, alpha);

  CFFList result;
  buf= alpha;
  for (CFFListIterator i= tmp2; i.hasItem(); i++)
  {
    for (CFFListIterator j= tmp1; j.hasItem(); j++)
    {
      tmp= gcd (i.getItem().factor(), j.getItem().factor());
      i.getItem()= CFFactor (i.getItem().factor()/tmp, i.getItem().exp());
      j.getItem()= CFFactor (j.getItem().factor()/tmp, j.getItem().exp());
      if (!tmp.inCoeffDomain())
      {
        tmp= M (tmp);
        result.append (CFFactor (tmp/Lc(tmp),
                       j.getItem().exp()*p + i.getItem().exp()));
      }
    }
  }
  for (CFFListIterator i= tmp2; i.hasItem(); i++)
  {
    if (!i.getItem().factor().inCoeffDomain())
    {
      tmp= M (i.getItem().factor());
      result.append (CFFactor (tmp/Lc(tmp), i.getItem().exp()));
    }
  }
  for (CFFListIterator j= tmp1; j.hasItem(); j++)
  {
    if (!j.getItem().factor().inCoeffDomain())
    {
      tmp= M (j.getItem().factor());
      result.append (CFFactor (tmp/Lc(tmp), j.getItem().exp()*p));
    }
  }
  return result;
}

CanonicalForm
sqrfPart (const CanonicalForm& F, CanonicalForm& pthPower,
          const Variable& alpha)
{
  if (F.inCoeffDomain())
  {
    pthPower= 1;
    return F;
  }
  CFMap M;
  CanonicalForm A= compress (F, M);
  Variable vBuf= alpha;
  CanonicalForm w, v, b;
  pthPower= 1;
  CanonicalForm result;
  int i= 1;
  bool allZero= true;
  for (; i <= A.level(); i++)
  {
    if (!deriv (A, Variable (i)).isZero())
    {
      allZero= false;
      break;
    }
  }
  if (allZero)
  {
    pthPower= F;
    return 1;
  }
  w= gcd (A, deriv (A, Variable (i)));

  b= A/w;
  result= b;
  if (degree (w) < 1)
    return M (result);
  i++;
  for (; i <= A.level(); i++)
  {
    if (!deriv (w, Variable (i)).isZero())
    {
      b= w;
      w= gcd (w, deriv (w, Variable (i)));
      b /= w;
      if (degree (b) < 1)
        break;
      CanonicalForm g;
      g= gcd (b, result);
      if (degree (g) > 0)
        result *= b/g;
      if (degree (g) <= 0)
        result *= b;
    }
  }
  result= M (result);
  return result;
}

