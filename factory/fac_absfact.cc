/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facMul.cc
 *
 * This file implements functions for fast multiplication and division with
 * remainder
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "fac_absfact.h"
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
void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

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
      out_cf ("f= ", f, "\n");
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

CFList absFactorize (const CanonicalForm& G)
{
  out_cf ("maxNorm (G)= ", maxNorm (G), "\n");
  //F is assumed to be bivariate, irreducible over Q, primitive wrt x and y, compressed

  CanonicalForm F= bCommonDen (G)*G;
  Off (SW_RATIONAL);
  //out_cf ("icontent (F)= ", icontent (F), "\n");
  F /= icontent (F);
  out_cf ("LC (F,1)= ", LC (F,1), "\n");
  out_cf ("LC (F,2)= ", LC (F,2), "\n");
  On (SW_RATIONAL);
  CFArray eval;
  int minTdeg, tdegF= totaldegree (F);
  CanonicalForm Fp, smallestFactor;
  int p;
  while (1)
  {
    printf ("enter while\n");
    p= choosePoint (F, tdegF, eval);

    printf ("p= %d\n", p);
    setCharacteristic (p);
    Fp=F.mapinto();
    CFFList factors= factorize (Fp);

    printf ("after factorize\n");
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
    else
      printf ("fail");
  }
  out_cf ("eval[0]= ", eval[0], "\n");
  out_cf ("eval[1]= ", eval[1], "\n");
  //out_cf ("Fp= ", Fp, "\n");
  CanonicalForm Gp= Fp/smallestFactor;
  printf ("Fp==Gp*smallestFactor? %d\n",Fp==Gp*smallestFactor);
  Gp= Gp (eval[0].mapinto(), 1);
  Fp= Fp (eval[0].mapinto(), 1);
  CanonicalForm smallestFactorEval= smallestFactor (eval[0].mapinto(),1);
  out_cf ("Fp= ", Fp, "\n");
  out_cf ("Gp= ", Gp, "\n");
  out_cf ("smallestFactorEval= ", smallestFactorEval, "\n");
  //out_cf ("Gp*smallestFactorEval= ", Gp*smallestFactorEval, "\n");
  printf ("Gp*smallestFactorEval==Fp? %d\n", Gp*smallestFactorEval == Fp);
  setCharacteristic (0);
  out_cf ("eval[0]= ", eval[0], "\n");
  CanonicalForm F1= F(eval[0],1);
  setCharacteristic (p);
  printf ("F1.mapinto()==Fp? %d\n", Fp==F1.mapinto());
  setCharacteristic (0);
  //out_cf ("Fp= ", Fp, "\n");
  //out_cf ("F1= ", F1, "\n");
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

  printf ("k= %ld\n", k);
  //k= 73L; //reicht für bertoneinput1
  //k= 92L;
  //k= 128L;
  //k= 141L;
  //k=256L;
  //k= 512L;
  //k= 1024L;
  printf ("k= %ld\n", k);
  k= k+1;
  fmpz_poly_t FLINTF1;
  convertFacCF2Fmpz_poly_t (FLINTF1, F1);
  /*fmpz_poly_print_pretty (FLINTF1, "x");
  printf ("\n");*/
  setCharacteristic (p);
  nmod_poly_t FLINTFp, FLINTGp;
  convertFacCF2nmod_poly_t (FLINTFp, smallestFactorEval/lc (smallestFactorEval));
  convertFacCF2nmod_poly_t (FLINTGp, Gp/lc (Gp));
  printf ("test= %d\n", (smallestFactorEval/lc (smallestFactorEval))*(Gp/lc (Gp))*lc (Fp) == Fp);
  nmod_poly_factor_t nmodFactors;
  nmod_poly_factor_init (nmodFactors);
  nmod_poly_factor_insert (nmodFactors, FLINTFp, 1L); //wird da vorne drangehangen oder hinten?
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
  //fmpz_poly_hensel_lift_once (liftedFactors, FLINTF1, nmodFactors, k);

  nmod_poly_factor_clear (nmodFactors);
  nmod_poly_clear (FLINTFp);
  nmod_poly_clear (FLINTGp);

  //out_cf ("smallestFactorEval= ", smallestFactorEval, "\n");
  printf ("after lift\n");
  setCharacteristic(0);
  modpk pk= modpk (p,k);
  printf ("after modpk\n");
  /*printf ("liftedSmallestFactor= ");
  fmpz_poly_print_pretty ((fmpz_poly_t &) liftedFactors->p[0], "x");
  printf ("\n");*/
  CanonicalForm liftedSmallestFactor= convertFmpz_poly_t2FacCF ((fmpz_poly_t &)liftedFactors->p[0],Variable (2));
  out_cf ("liftedSmallestFactor= ", liftedSmallestFactor, "\n");
  //liftedSmallestFactor *= lc (F1);
  
  //liftedSmallestFactor= pk (liftedSmallestFactor);
  //out_cf ("icontent (liftedSmallestFactor)= ", icontent (liftedSmallestFactor), "\n");
  //liftedSmallestFactor /= icontent (liftedSmallestFactor);
  CanonicalForm otherFactor= convertFmpz_poly_t2FacCF ((fmpz_poly_t &)liftedFactors->p[1],Variable (2));
  //otherFactor *= lc (F1);
  //otherFactor= pk (otherFactor);
  //out_cf ("icontent (otherFactor)= ", icontent (otherFactor), "\n");
  printf ("isOn (SW_RATIONAL)= %d\n", isOn (SW_RATIONAL));
  //out_cf ("F1= ", F1, "\n");
  CanonicalForm test= pk (otherFactor*liftedSmallestFactor);
  printf ("test == F1 %d\n", test == F1);
  //out_cf ("otherFactor*smallestFactor= ", test, "\n");

  setCharacteristic (p);
  //out_cf ("F1.mapinto()= ", F1.mapinto(), "\n");
  //out_cf ("test.mapinto()= ", test.mapinto(), "\n");
  printf ("test.mapinto() == F1.mapinto() ? %d\n", test.mapinto() == F1.mapinto());
  setCharacteristic (0);
  //out_cf ("liftedSmallestFactor= ", liftedSmallestFactor, "\n");
  //out_cf ("otherFactor= ", otherFactor, "\n");

  Off (SW_SYMMETRIC_FF);
  liftedSmallestFactor= pk (liftedSmallestFactor);
  //out_cf ("pk.getpk()= ", pk.getpk(), "\n");
  //out_cf ("liftedSmallestFactor= ", liftedSmallestFactor, "\n");
  liftedSmallestFactor= liftedSmallestFactor (eval[1],2);
  On (SW_SYMMETRIC_FF);
  CFMatrix M= CFMatrix (s, s);
  M(s,s)= power (CanonicalForm (p), k);
  for (int i= 1; i < s; i++)
  {
    M (i,i)= 1;
    M (i+1,i)= -liftedSmallestFactor;
  }

  printf ("before LLL\n");
  /*for (int i= 1; i <= s; i++)
  {
    printf ("M (i)= ");
    for (int j= 1; j <=s; j++)
      out_cf (" ", M(i,j), "");
    printf ("\n");
  }*/

  mat_ZZ NTLM= *convertFacCFMatrix2NTLmat_ZZ (M);

  ZZ det;

  transpose (NTLM, NTLM);
  long r=LLL (det, NTLM, 1L, 1L);
  transpose (NTLM, NTLM);
  M= *convertNTLmat_ZZ2FacCFMatrix (NTLM);

  printf ("r= %ld\n", r);
  printf ("s= %d\n", s);
  /*for (int i= 1; i <= s; i++)
  {
    printf ("M (i)= ");
    for (int j= 1; j <=s; j++)
      out_cf (" ", M(i,j), "");
    printf ("\n");
  }*/
  CanonicalForm mipo= 0;
  Variable x= Variable (1);
  for (int i= 1; i <= s; i++)
    mipo += M (i,1)*power (x,s-i);

  out_cf ("/= ", F1 (eval[1],2)*lc(mipo), "\n");
  out_cf ("mipo/= ", mipo (0,1)*lc (F1), "\n");
  CFFList mipoFactors= factorize (mipo);
  printf ("mipoFactors.length()= %d\n", mipoFactors.length());
  mipoFactors.removeFirst();
  out_cf ("mipoFactors.getFirst()= ", mipoFactors.getFirst().factor(), "\n");
  //out_cf ("lc (F1)= ", lc (F1), "\n");
  //out_cf ("F1eval= ", F1 (eval[1],2), "\n");
  //out_cf ("F1= ", F1, "\n");
  //out_cf ("icontent (mipo)= ", icontent (mipo), "\n");
  On (SW_RATIONAL);
  Variable alpha= rootOf (mipo);
  CFFList QaFactors= factorize (F1, alpha);
  for (CFFListIterator iter= QaFactors; iter.hasItem(); iter++)
    out_cf ("iter.getItem()= ", iter.getItem().factor(), "\n");

  fmpz_poly_clear (v[0]);
  fmpz_poly_clear (v[1]);
  fmpz_poly_clear (w[0]);
  fmpz_poly_clear (w[1]);
  delete [] v;
  delete [] w;
  delete [] link;
  fmpz_poly_factor_clear (liftedFactors);
  //TODO delete v, wclear
  return CFList();
}
#endif


