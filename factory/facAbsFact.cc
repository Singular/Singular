/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.cc
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "facAbsFact.h"
#include "cf_reval.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#include <flint/fmpz_poly_factor.h>
#endif
#ifdef HAVE_NTL
#include "NTLconvert.h"
#include <NTL/LLL.h>
#endif

#ifdef HAVE_FLINT
#ifdef HAVE_NTL

//TODO optimize choice of p -> choose p as large as possible (better than small p since factorization mod p does not require field extension, also less lifting)
int choosePoint (const CanonicalForm& F, int tdegF, CFArray& eval)
{
  REvaluation E1 (1, 1, IntRandom (25));
  REvaluation E2 (2, 2, IntRandom (25));
  //E1.nextpoint();
  //E2.nextpoint();
  CanonicalForm f, Fp;
  int i;
  eval=CFArray (2);
  while (1)
  {
    f= E1(F);
    if (!f.isZero() && factorize (f).length() == 2)
    {
      Off (SW_RATIONAL);
      f= E2(f);
      if (!f.isZero() && f > cf_getSmallPrime (cf_getNumSmallPrimes()))
      {
        for (i= cf_getNumPrimes()-1; i > 0; i--)
        {
          if (f % CanonicalForm (cf_getPrime (i)) == 0)
          {
            Fp= mod (F,cf_getPrime(i));
            if (totaldegree (Fp) == tdegF)
            {
              eval[0]= E1[1];
              eval[1]= E2[2];
              return cf_getPrime(i);
            }
          }
        }
      }
      else if (!f.isZero())
      {
        for (i= cf_getNumSmallPrimes()-1; i > 0; i--)
        {
          if (f % CanonicalForm (cf_getSmallPrime (i)) == 0)
          {
            Fp= mod (F,cf_getSmallPrime(i));
            if (totaldegree (Fp) == tdegF)
            {
              eval[0]= E1[1];
              eval[1]= E2[2];
              return cf_getSmallPrime(i);
            }
          }
        }
      }
      E2.nextpoint();
      On (SW_RATIONAL);
    }
    E1.nextpoint();
  }
  return 0;
}

CFFList absFactorize (const CanonicalForm& G)
{
  //F is assumed to be bivariate, irreducible over Q, primitive wrt x and y, compressed

  CanonicalForm F= bCommonDen (G)*G;
  Off (SW_RATIONAL);
  F /= icontent (F);
  On (SW_RATIONAL);
  CFArray eval;
  int minTdeg, tdegF= totaldegree (F);
  CanonicalForm Fp, smallestFactor;
  int p;
  while (1)
  {
    p= choosePoint (F, tdegF, eval);

    setCharacteristic (p);
    Fp=F.mapinto();
    CFFList factors= factorize (Fp);

    factors.removeFirst();
    CFFListIterator iter= factors;
    smallestFactor= iter.getItem().factor();
    minTdeg= totaldegree (smallestFactor);
    iter++;
    for (; iter.hasItem(); iter++)
    {
      if (totaldegree (iter.getItem().factor()) < minTdeg)
      {
        smallestFactor= iter.getItem().factor();
        minTdeg= totaldegree (smallestFactor);
      }
    }
    if (tdegF % minTdeg == 0)
      break;
    //TODO else
  }
  CanonicalForm Gp= Fp/smallestFactor;
  Gp= Gp (eval[0].mapinto(), 1);
  Fp= Fp (eval[0].mapinto(), 1);
  CanonicalForm smallestFactorEval= smallestFactor (eval[0].mapinto(),1);
  setCharacteristic (0);
  CanonicalForm F1= F(eval[0],1);
  int s= tdegF/minTdeg + 1;
  CanonicalForm bound= power (maxNorm (F1), 2*(s-1));
  bound *= power (CanonicalForm (s),s-1);
  bound *= power (CanonicalForm (2), ((s-1)*(s-1))/2); //possible int overflow

  CanonicalForm B = p;
  long k = 1L;
  while ( B < bound ) {
    B *= p;
    k++;
  }

  //TODO take floor (log2(k))
  k= k+1;
  fmpz_poly_t FLINTF1;
  convertFacCF2Fmpz_poly_t (FLINTF1, F1);
  setCharacteristic (p);
  nmod_poly_t FLINTFp, FLINTGp;
  convertFacCF2nmod_poly_t (FLINTFp, smallestFactorEval/lc (smallestFactorEval));
  convertFacCF2nmod_poly_t (FLINTGp, Gp/lc (Gp));
  nmod_poly_factor_t nmodFactors;
  nmod_poly_factor_init (nmodFactors);
  nmod_poly_factor_insert (nmodFactors, FLINTFp, 1L);
  nmod_poly_factor_insert (nmodFactors, FLINTGp, 1L);

  long * link= new long [2];
  fmpz_poly_t *v= new fmpz_poly_t[2];
  fmpz_poly_t *w= new fmpz_poly_t[2];
  fmpz_poly_init(v[0]);
  fmpz_poly_init(v[1]);
  fmpz_poly_init(w[0]);
  fmpz_poly_init(w[1]);

  fmpz_poly_factor_t liftedFactors;
  fmpz_poly_factor_init (liftedFactors);
  _fmpz_poly_hensel_start_lift(liftedFactors, link, v, w, FLINTF1, nmodFactors, k); //lift factors up to p^k

  nmod_poly_factor_clear (nmodFactors);
  nmod_poly_clear (FLINTFp);
  nmod_poly_clear (FLINTGp);

  setCharacteristic(0);
  modpk pk= modpk (p,k);
  CanonicalForm liftedSmallestFactor= convertFmpz_poly_t2FacCF ((fmpz_poly_t &)liftedFactors->p[0],Variable (2));

  CanonicalForm otherFactor= convertFmpz_poly_t2FacCF ((fmpz_poly_t &)liftedFactors->p[1],Variable (2));
  CanonicalForm test= pk (otherFactor*liftedSmallestFactor);

  Off (SW_SYMMETRIC_FF);
  liftedSmallestFactor= pk (liftedSmallestFactor);
  liftedSmallestFactor= liftedSmallestFactor (eval[1],2);
  On (SW_SYMMETRIC_FF);
  CFMatrix M= CFMatrix (s, s);
  M(s,s)= power (CanonicalForm (p), k);
  for (int i= 1; i < s; i++)
  {
    M (i,i)= 1;
    M (i+1,i)= -liftedSmallestFactor;
  }

  mat_ZZ NTLM= *convertFacCFMatrix2NTLmat_ZZ (M);

  ZZ det;

  transpose (NTLM, NTLM);
  long r=LLL (det, NTLM, 1L, 1L); //use floating point LLL ?
  transpose (NTLM, NTLM);
  M= *convertNTLmat_ZZ2FacCFMatrix (NTLM);

  CanonicalForm mipo= 0;
  Variable x= Variable (1);
  for (int i= 1; i <= s; i++)
    mipo += M (i,1)*power (x,s-i);

  CFFList mipoFactors= factorize (mipo);
  mipoFactors.removeFirst();
  //TODO check if mipoFactors has length 1 and multiplicity 1 - if not choose a new point!
  On (SW_RATIONAL);
  Variable alpha= rootOf (mipo);
  CFFList QaFactors= factorize (F1, alpha);

  QaFactors.append (CFFactor (mipo, 1)); //last factor is the minimal polynomial that defines the extension
  fmpz_poly_clear (v[0]);
  fmpz_poly_clear (v[1]);
  fmpz_poly_clear (w[0]);
  fmpz_poly_clear (w[1]);
  delete [] v;
  delete [] w;
  delete [] link;
  fmpz_poly_factor_clear (liftedFactors);
  return QaFactors;
}
#endif
#endif


