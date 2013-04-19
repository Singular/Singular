/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.cc
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "timing.h"
#include "debug.h"

#include "facAbsFact.h"
#include "facBivar.h"
#include "facFqBivar.h"
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

TIMING_DEFINE_PRINT(fac_Qa_factorize)
TIMING_DEFINE_PRINT(fac_evalpoint)

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

//TODO optimize choice of p -> choose p as large as possible (better than small p since factorization mod p does not require field extension, also less lifting)
int choosePoint (const CanonicalForm& F, int tdegF, CFArray& eval, bool rec)
{
  REvaluation E1 (1, 1, IntRandom (25));
  REvaluation E2 (2, 2, IntRandom (25));
  if (rec)
  {
    E1.nextpoint();
    E2.nextpoint();
  }
  CanonicalForm f, f1, f2, Fp;
  int i, p;
  eval=CFArray (2);
  printf ("tdegF= %d\n", tdegF);
  out_cf ("F= ", F, "\n");
  printf ("getCharacteristic()= %d\n", getCharacteristic());
  while (1)
  {
    f1= E1(F);
    if (!f1.isZero() && factorize (f1).length() == 2)
    {
      Off (SW_RATIONAL);
      f= E2(f1);
      f2= E2 (F);
      out_cf ("f= ", f, "\n");
      printf ("isOn (SW_RATIONAL)= %d\n", isOn (SW_RATIONAL));
      printf ("cf_getSmallPrime (cf_getNumSmallPrimes())= %d\n", cf_getSmallPrime (cf_getNumSmallPrimes()-1));
      if ((!f.isZero()) && (abs(f) > cf_getSmallPrime (cf_getNumSmallPrimes()-1)))
      {
        printf ("hier0\n");
        for (i= cf_getNumPrimes()-1; i >= 0; i--)
        {
          if (f % CanonicalForm (cf_getPrime (i)) == 0)
          {
            p= cf_getPrime(i);
            Fp= mod (F,p);
            out_cf ("Fp0= ", Fp, "\n");
            if (totaldegree (Fp) == tdegF && degree (mod (f2,p), 1) == degree (F,1) && degree (mod (f1, p),2) == degree (F,2))
            {
              eval[0]= E1[1];
              eval[1]= E2[2];
              return p;
            }
          }
        }
      }
      else if (!f.isZero())
      {
        printf ("hier\n");
        for (i= cf_getNumSmallPrimes()-1; i >= 0; i--)
        {
          if (f % CanonicalForm (cf_getSmallPrime (i)) == 0)
          {
            p= cf_getSmallPrime (i);
            Fp= mod (F,p);
            out_cf ("Fp1= ", Fp, "\n");
            if (totaldegree (Fp) == tdegF && degree (mod (f2, p),1) == degree (F,1) && degree (mod (f1,p),2) == degree (F,2))
            {
              eval[0]= E1[1];
              eval[1]= E2[2];
              return p;
            }
          }
        }
      }
      E2.nextpoint();
      On (SW_RATIONAL);
      out_cf ("E2= ", E2[2], "\n");
    }
    E1.nextpoint();
    out_cf ("E1= ", E1[1], "\n");
  }
  return 0;
}

//TODO sowohl bzgl. x als auch y auswerten und minpoly berechnen
CFAFList absFactorizeMain (const CanonicalForm& G)
{
  //F is assumed to be bivariate, irreducible over Q, primitive wrt x and y, compressed

  out_cf ("F= ", G, "\n");
  CanonicalForm F= bCommonDen (G)*G;
  Off (SW_RATIONAL);
  F /= icontent (F);
  out_cf ("F after icontent= ", F, "\n");
  On (SW_RATIONAL);
  CFArray eval;
  int minTdeg, tdegF= totaldegree (F);
  CanonicalForm Fp, smallestFactor;
  int p;
  CFFList factors;
  Variable alpha;
  bool rec= false;
  Variable x= Variable (1);
  Variable y= Variable (2);
differentevalpoint:
  while (1)
  {
    TIMING_START (fac_evalpoint);
    p= choosePoint (F, tdegF, eval, rec);
    TIMING_END_AND_PRINT (fac_evalpoint, "time to find eval point: ");

    setCharacteristic (p);
    Fp=F.mapinto();
    factors= factorize (Fp);

    for (CFFListIterator iter= factors; iter.hasItem(); iter++)
    {
      out_cf ("factors= ", iter.getItem().factor(), "\n");
      printf ("exp= %d\n", iter.getItem().exp());
    }
    printf ("p= %d\n", p);
    if (factors.getFirst().factor().inCoeffDomain())
      factors.removeFirst();
    printf ("factors.length()= %d\n", factors.length());
    printf ("factors.getFirst().exp()= %d\n", factors.getFirst().exp());
    if (factors.length() == 1 && factors.getFirst().exp() == 1)
    {
      if (absIrredTest (Fp)) //TODO absIrredTest mit shift, modular absIrredTest
      {
        printf ("irred after test\n");
        printf ("absIrred\n");
        setCharacteristic(0);
        alpha= rootOf (x);
    out_cf ("G= ", G, "\n");
    out_cf ("getMipo (alpha)= ", getMipo (alpha), "\n");

        return CFAFList (CFAFactor (G, getMipo (alpha), 1));
      }
      else
      {
        setCharacteristic (0);
        if (modularIrredTestWithShift (F))
        {
          printf ("irred after modular test\n");
          alpha= rootOf (x);
          return CFAFList (CFAFactor (G, getMipo (alpha), 1));
        }
        rec= true;
        continue;
      }
    }
    CFFListIterator iter= factors;
    smallestFactor= iter.getItem().factor();
    out_cf ("smallestFactor before= ", smallestFactor, "\n");
    while (smallestFactor.isUnivariate() && iter.hasItem())
    {
      iter++;
      if (!iter.hasItem())
        break;
      out_cf ("factors= ", iter.getItem().factor(), "\n");
      printf ("exp= %d\n", iter.getItem().exp());
      smallestFactor= iter.getItem().factor();
    }
    //TODO univariate Faktoren rausschmeißen!
    minTdeg= totaldegree (smallestFactor);
    if (iter.hasItem())
      iter++;
    for (; iter.hasItem(); iter++)
    {
      out_cf ("factors= ", iter.getItem().factor(), "\n");
      printf ("exp= %d\n", iter.getItem().exp());
      if (!iter.getItem().factor().isUnivariate() && (totaldegree (iter.getItem().factor()) < minTdeg))
      {
        smallestFactor= iter.getItem().factor();
        minTdeg= totaldegree (smallestFactor);
      }
    }
    if (tdegF % minTdeg == 0)
      break;
    setCharacteristic(0);
    rec=true;
  }
  CanonicalForm Gp= Fp/smallestFactor;
  out_cf ("Gp before= ", Gp, "\n");
  out_cf ("smallestFactor= ", smallestFactor, "\n");
  printf ("degree (Gp,1)= %d\n", degree (Gp, 1));
  printf ("degree smallestFactor= %d\n", degree (smallestFactor, 1));
  printf ("degree (Fp,1)= %d\n", degree (Fp,1));
  printf ("degree (F,1)= %d\n", degree (F,1));
  out_cf ("eval[1]= ", eval[1], "\n");
  out_cf ("eval[0]= ", eval[0], "\n");
  //printf ("Gp*smallestFactor==Fp ? %d\n", Gp*smallestFactor == Fp);
  Gp= Gp /Lc (Gp);

  CanonicalForm Gpy= Gp (eval[0].mapinto(), 1);
  CanonicalForm smallestFactorEvaly= smallestFactor (eval[0].mapinto(),1);
  CanonicalForm Gpx= Gp (eval[1].mapinto(), 2);
  out_cf ("Gp eval= ", Gp, "\n");
  CanonicalForm smallestFactorEvalx= smallestFactor (eval[1].mapinto(),2);

  out_cf ("smallestFactorEvalx= ", smallestFactorEvalx, "\n");
  out_cf ("gcd (Gpx, smallestFactorEvalx)= ", gcd (Gpx, smallestFactorEvalx), "\n");
  bool xValid= !(Gpx.inCoeffDomain() || smallestFactorEvalx.inCoeffDomain() || !gcd (Gpx, smallestFactorEvalx).inCoeffDomain());
  bool yValid= !(Gpy.inCoeffDomain() || smallestFactorEvaly.inCoeffDomain() || !gcd (Gpy, smallestFactorEvaly).inCoeffDomain());
  if (!xValid && !yValid)
  {
    rec= true;
    setCharacteristic (0);
    printf ("goto1\n");
    goto differentevalpoint;
  }

  setCharacteristic (0);

  CanonicalForm mipo;

  int loop, i;
  if (xValid && yValid)
  {
    loop= 3;
    i=1;
  }
  else if (xValid)
  {
    loop= 3;
    i=2;
  }
  else
  {
    loop= 2;
    i=1;
  }

  CFArray mipos= CFArray (loop-i);
  printf ("loop= %d\n", loop);
  printf ("xValid= %d\n", xValid);
  printf ("yValid= %d\n", yValid);

  for (; i < loop; i++)
  {
    CanonicalForm Fi= F(eval[i-1],i);
    //CanonicalForm Fx= F(eval[0],1);
    //CanonicalForm Fy= F(eval[1],2);

    int s= tdegF/minTdeg + 1;
    CanonicalForm bound= power (maxNorm (Fi), 2*(s-1));
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
    fmpz_poly_t FLINTFi;
    out_cf ("Fi= ", Fi, "\n");
    convertFacCF2Fmpz_poly_t (FLINTFi, Fi);
    setCharacteristic (p);
    printf ("p= %d\n", p);
    nmod_poly_t FLINTFpi, FLINTGpi;
    if (i == 2)
    {
      convertFacCF2nmod_poly_t (FLINTFpi, smallestFactorEvalx/lc (smallestFactorEvalx));
      convertFacCF2nmod_poly_t (FLINTGpi, Gpx/lc (Gpx));
    }
    else
    {
      convertFacCF2nmod_poly_t (FLINTFpi, smallestFactorEvaly/lc (smallestFactorEvaly));
      convertFacCF2nmod_poly_t (FLINTGpi, Gpy/lc (Gpy));
    }
    nmod_poly_factor_t nmodFactors;
    nmod_poly_factor_init (nmodFactors);
    nmod_poly_factor_insert (nmodFactors, FLINTFpi, 1L);
    nmod_poly_factor_insert (nmodFactors, FLINTGpi, 1L);

    //out_cf ("Gpx= ", Gpx, "\n");
    //out_cf ("smallestFactorEvalx= ", smallestFactorEvalx, "\n");
    long * link= new long [2];
    fmpz_poly_t *v= new fmpz_poly_t[2];
    fmpz_poly_t *w= new fmpz_poly_t[2];
    fmpz_poly_init(v[0]);
    fmpz_poly_init(v[1]);
    fmpz_poly_init(w[0]);
    fmpz_poly_init(w[1]);

    printf ("k= %ld\n", k);
    fmpz_poly_factor_t liftedFactors;
    fmpz_poly_factor_init (liftedFactors);
    _fmpz_poly_hensel_start_lift(liftedFactors, link, v, w, FLINTFi, nmodFactors, k); //lift factors up to p^k

    nmod_poly_factor_clear (nmodFactors);
    nmod_poly_clear (FLINTFpi);
    nmod_poly_clear (FLINTGpi);

    setCharacteristic(0);
    modpk pk= modpk (p,k);
    CanonicalForm liftedSmallestFactor= convertFmpz_poly_t2FacCF ((fmpz_poly_t &)liftedFactors->p[0],Variable (1));

    CanonicalForm otherFactor= convertFmpz_poly_t2FacCF ((fmpz_poly_t &)liftedFactors->p[1],Variable (1));
    CanonicalForm test= pk (otherFactor*liftedSmallestFactor);

    Off (SW_SYMMETRIC_FF);
    liftedSmallestFactor= pk (liftedSmallestFactor);
    if (i == 2)
      liftedSmallestFactor= liftedSmallestFactor (eval[0],1);
    else
      liftedSmallestFactor= liftedSmallestFactor (eval[1],1);

    On (SW_SYMMETRIC_FF);
    CFMatrix M= CFMatrix (s, s);
    M(s,s)= power (CanonicalForm (p), k);
    for (int j= 1; j < s; j++)
    {
      M (j,j)= 1;
      M (j+1,j)= -liftedSmallestFactor;
    }

    mat_ZZ NTLM= *convertFacCFMatrix2NTLmat_ZZ (M);

    ZZ det;

    transpose (NTLM, NTLM);
    (void) LLL (det, NTLM, 1L, 1L); //use floating point LLL ?
    transpose (NTLM, NTLM);
    M= *convertNTLmat_ZZ2FacCFMatrix (NTLM);

    mipo= 0;
    for (int j= 1; j <= s; j++)
      mipo += M (j,1)*power (x,s-j);

    CFFList mipoFactors= factorize (mipo);
    mipoFactors.removeFirst();

    fmpz_poly_clear (v[0]);
    fmpz_poly_clear (v[1]);
    fmpz_poly_clear (w[0]);
    fmpz_poly_clear (w[1]);
    delete [] v;
    delete [] w;
    delete [] link;
    fmpz_poly_factor_clear (liftedFactors);

    if (mipoFactors.length() > 1 ||
        (mipoFactors.length() == 1 && mipoFactors.getFirst().exp() > 1))
    {
      if (i+1 >= loop && ((loop-i == 1) || (loop-i==2 && mipos[0].isZero())))
      {
        rec=true;
        printf ("goto2\n");
        goto differentevalpoint;
        //TODO check if mipoFactors has length 1 and multiplicity 1 - if not choose a new point!
      }
    }
    else
      mipos[loop-i-1]= mipo;
  }

  On (SW_RATIONAL);
  if (xValid && yValid && !mipos[0].isZero() && !mipos[1].isZero())
  {
    if (maxNorm (mipos[0]) < maxNorm (mipos[1]))
      alpha= rootOf (mipos[0]);
    else
      alpha= rootOf (mipos[1]);
  }
  else
    alpha= rootOf (mipo);

  for (i= 0; i < mipos.size(); i++)
  {
    out_cf ("mipos= ", mipos [i], "\n");
    out_cf ("maxNorm mipo= ", maxNorm (mipos[i]), "\n");
  }

  CanonicalForm F1;
  CFFList QaF1Factors;
  int wrongMipo= 0;
  if (xValid && yValid)
  {
    if (degree (F,1) > minTdeg)
      F1= F (eval[1], 2);
    else
      F1= F (eval[0], 1);
  }
  else if (xValid)
    F1= F (eval[1], 2);
  else
    F1= F (eval[0], 1);

  QaF1Factors= factorize (F1, alpha);
  if (QaF1Factors.getFirst().factor().inCoeffDomain())
    QaF1Factors.removeFirst();
  out_cf ("mipo0= ", getMipo (alpha), "\n");
  for (CFFListIterator iter= QaF1Factors; iter.hasItem(); iter++)
  {
    out_cf ("QaF1Factors= ", iter.getItem().factor(), "\n");
    if (degree (iter.getItem().factor()) > minTdeg)
      wrongMipo++;
  }

  if (wrongMipo == QaF1Factors.length())
  {
    if (xValid && yValid)
    {
      if (mipo==mipos[0])
        alpha= rootOf (mipos[1]);
      else
        alpha= rootOf (mipos[0]);
    }

    wrongMipo= 0;
    out_cf ("mipo1= ", getMipo (alpha), "\n");
    QaF1Factors= factorize (F1, alpha);
    if (QaF1Factors.getFirst().factor().inCoeffDomain())
      QaF1Factors.removeFirst();
    for (CFFListIterator iter= QaF1Factors; iter.hasItem(); iter++)
    {
      out_cf ("QaF1Factors= ", iter.getItem().factor(), "\n");
      if (degree (iter.getItem().factor()) > minTdeg)
        wrongMipo++;
    }
    if (wrongMipo == QaF1Factors.length())
    {
      rec= true;
      printf ("goto30\n");
      goto differentevalpoint;
    }
  }

  CanonicalForm A= F;
  CanonicalForm Aeval= F1;

  out_cf ("F1= ", F1, "\n");
  A *= bCommonDen (A);
  A= A (y + eval[1], y); //TODO find right evaluation point and swap if necessary

  out_cf ("A= ", A, "\n");
  out_cf ("A[0]= ", A(0,y), "\n");
  int liftBound= degree (A,y) + 1;

  modpk b= modpk();

  //bool mipoHasDen= false;
  CanonicalForm den= 1;

  mipo= getMipo (alpha);

  CFList uniFactors;
  for (CFFListIterator iter=QaF1Factors; iter.hasItem(); iter++)
  {
    uniFactors.append (iter.getItem().factor());
    out_cf ("uniFactors.getLast()= ", uniFactors.getLast(), "\n");
  }


    A /= Lc (Aeval);
    //mipoHasDen= !bCommonDen(mipo).isOne();
    //mipo *= bCommonDen (mipo);
    ZZX NTLmipo= convertFacCF2NTLZZX (mipo);
    ZZX NTLLcf= convertFacCF2NTLZZX (Lc (A*bCommonDen (A)));
    ZZ NTLf= resultant (NTLmipo, NTLLcf);
    ZZ NTLD= discriminant (NTLmipo);
    den= abs (convertZZ2CF (NTLD*NTLf));

    // make factors elements of Z(a)[x] disable for modularDiophant
    CanonicalForm multiplier= 1;
    for (CFListIterator i= uniFactors; i.hasItem(); i++)
    {
      multiplier *= bCommonDen (i.getItem());
      i.getItem()= i.getItem()*bCommonDen(i.getItem());
    }
    A *= multiplier;
    A *= bCommonDen (A);

    Off (SW_RATIONAL);
    int ii= 0;
    CanonicalForm discMipo= convertZZ2CF (NTLD);
    findGoodPrime (F*discMipo,ii);
    findGoodPrime (Aeval*discMipo,ii);
    findGoodPrime (A*discMipo,ii);

    int pp=cf_getBigPrime(ii);
    b = coeffBound( A, pp, mipo );
    modpk bb= coeffBound (Aeval, pp, mipo);
    if (bb.getk() > b.getk() ) b=bb;
      bb= coeffBound (F, pp, mipo);
    if (bb.getk() > b.getk() ) b=bb;

  ExtensionInfo dummy= ExtensionInfo (alpha, false);
  DegreePattern degs= DegreePattern (uniFactors);

  bool earlySuccess= false;
  CFList earlyFactors;
  TIMING_START (fac_bi_hensel_lift);
  uniFactors= henselLiftAndEarly
              (A, earlySuccess, earlyFactors, degs, liftBound,
               uniFactors, dummy, eval[1], b, den);
  TIMING_END_AND_PRINT (fac_bi_hensel_lift,
                        "time for bivariate hensel lifting over Q: ");
  DEBOUTLN (cerr, "lifted factors= " << uniFactors);

  CanonicalForm MODl= power (y, liftBound); //TODO

  On (SW_RATIONAL);
  A *= bCommonDen (A);
  Off (SW_RATIONAL);

  printf ("earlyFactors.length()= %d\n", earlyFactors.length());
  CFList biFactors;

  TIMING_START (fac_bi_factor_recombination);
  biFactors= factorRecombination (uniFactors, A, MODl, degs, 1,
                                uniFactors.length()/2, b, den);
  TIMING_END_AND_PRINT (fac_bi_factor_recombination,
                        "time for bivariate factor recombination over Q: ");

  On (SW_RATIONAL);

  if (earlySuccess)
    biFactors= Union (earlyFactors, biFactors);
  else if (!earlySuccess && degs.getLength() == 1)
    biFactors= earlyFactors;

  for (CFListIterator i= biFactors; i.hasItem(); i++)
    i.getItem()= i.getItem() (y - eval[1], y); //TODO

  bool swap= false;
  bool swap2= false;
  appendSwapDecompress (biFactors, CFList(), CFList(), swap, swap2, CFMap());
  if (isOn (SW_RATIONAL))
    normalize (biFactors);

  CFAFList result;
  bool found= false;

  out_cf ("mipo= ", mipo, "\n");
  printf ("minTdeg= %d\n", minTdeg);
  for (CFListIterator iter= biFactors; iter.hasItem(); iter++)
  {
    out_cf ("biFactors= ", iter.getItem(), "\n");
    printf ("totaldegree ()= %d\n", totaldegree (iter.getItem()));
    if (totaldegree (iter.getItem()) == minTdeg)
    {
      result= CFAFList (CFAFactor (iter.getItem(), getMipo (alpha), 1));
      found= true;
      break;
    }
  }

  if (found)
  {
    printf ("thisexitexit\n\n");
    return result;
  }

  /*  A *= bCommonDen (A);
  A= A (y + evaluation, y);

  int liftBound= degree (A, y) + 1;

  modpk b= modpk();
  bool mipoHasDen= false;
  CanonicalForm den= 1;

  if (!extension)
  {
    Off (SW_RATIONAL);
    int i= 0;
    findGoodPrime(F,i);
    findGoodPrime(Aeval,i);
    findGoodPrime(A,i);
    if (i >= cf_getNumBigPrimes())
      printf ("out of primes\n"); //TODO exit

    int p=cf_getBigPrime(i);
    b = coeffBound( A, p );
    modpk bb= coeffBound (Aeval, p);
    if (bb.getk() > b.getk() ) b=bb;
      bb= coeffBound (F, p);
    if (bb.getk() > b.getk() ) b=bb;
  }
  else
  {
    A /= Lc (Aeval);
    mipoHasDen= !bCommonDen(mipo).isOne();
    mipo *= bCommonDen (mipo);
    ZZX NTLmipo= convertFacCF2NTLZZX (mipo);
    ZZX NTLLcf= convertFacCF2NTLZZX (Lc (A*bCommonDen (A)));
    ZZ NTLf= resultant (NTLmipo, NTLLcf);
    ZZ NTLD= discriminant (NTLmipo);
    den= abs (convertZZ2CF (NTLD*NTLf));

    // make factors elements of Z(a)[x] disable for modularDiophant
    CanonicalForm multiplier= 1;
    for (CFListIterator i= uniFactors; i.hasItem(); i++)
    {
      multiplier *= bCommonDen (i.getItem());
      i.getItem()= i.getItem()*bCommonDen(i.getItem());
    }
    A *= multiplier;
    A *= bCommonDen (A);

    Off (SW_RATIONAL);
    int i= 0;
    CanonicalForm discMipo= convertZZ2CF (NTLD);
    findGoodPrime (F*discMipo,i);
    findGoodPrime (Aeval*discMipo,i);
    findGoodPrime (A*discMipo,i);

    int p=cf_getBigPrime(i);
    b = coeffBound( A, p, mipo );
    modpk bb= coeffBound (Aeval, p, mipo);
    if (bb.getk() > b.getk() ) b=bb;
      bb= coeffBound (F, p, mipo);
    if (bb.getk() > b.getk() ) b=bb;
  }

  ExtensionInfo dummy= ExtensionInfo (false);
  if (extension)
    dummy= ExtensionInfo (v, false);
  bool earlySuccess= false;
  CFList earlyFactors;
  TIMING_START (fac_bi_hensel_lift);
  uniFactors= henselLiftAndEarly
              (A, earlySuccess, earlyFactors, degs, liftBound,
               uniFactors, dummy, evaluation, b, den);
  TIMING_END_AND_PRINT (fac_bi_hensel_lift,
                        "time for bivariate hensel lifting over Q: ");
  DEBOUTLN (cerr, "lifted factors= " << uniFactors);

  CanonicalForm MODl= power (y, liftBound);

  if (mipoHasDen)
  {
    Variable vv;
    for (CFListIterator iter= uniFactors; iter.hasItem(); iter++)
      if (hasFirstAlgVar (iter.getItem(), vv))
        break;
    for (CFListIterator iter= uniFactors; iter.hasItem(); iter++)
      iter.getItem()= replacevar (iter.getItem(), vv, v);
  }

  On (SW_RATIONAL);
  A *= bCommonDen (A);
  Off (SW_RATIONAL);

  TIMING_START (fac_bi_factor_recombination);
  factors= factorRecombination (uniFactors, A, MODl, degs, 1,
                                uniFactors.length()/2, b, den);
  TIMING_END_AND_PRINT (fac_bi_factor_recombination,
                        "time for bivariate factor recombination over Q: ");

  On (SW_RATIONAL);

  if (earlySuccess)
    factors= Union (earlyFactors, factors);
  else if (!earlySuccess && degs.getLength() == 1)
    factors= earlyFactors;

  for (CFListIterator i= factors; i.hasItem(); i++)
    i.getItem()= i.getItem() (y - evaluation, y);

  appendSwapDecompress (factors, conv (contentAxFactors),
                        conv (contentAyFactors), swap, swap2, N);
  if (isOn (SW_RATIONAL))
    normalize (factors);*/

  TIMING_START (fac_Qa_factorize);
  CFFList QaFactors= factorize (F, alpha); //TODO lift these factors
  TIMING_END_AND_PRINT (fac_Qa_factorize, "time to factorize over Qa: ");

  /*mipo= getMipo (alpha);
  out_cf ("maxNorm (mipo)= ", maxNorm (mipo), "\n");
  QaFactors.append (CFFactor (mipo, 1)); //last factor is the minimal polynomial that defines the extension
  if (degree (mipo) < 3)
    printf ("scheissescheissescheissescheisse\n");*/
  printf ("minTdeg= %d\n", minTdeg);
  //CFAFList result;
  //bool found= false;
  out_cf ("mipo= ", getMipo (alpha), "\n");
  for (CFFListIterator iter= QaFactors; iter.hasItem(); iter++)
  {
    out_cf ("QaFactors= ", iter.getItem().factor(), "\n");
    printf ("totaldegree ()= %d\n", totaldegree (iter.getItem().factor()));
    if (totaldegree (iter.getItem().factor()) == minTdeg)
    {
      result= CFAFList (CFAFactor (iter.getItem().factor(), getMipo (alpha), 1));
      found= true;
      break;
    }
  }
  if (!found && xValid && yValid)
  {
    if (mipo == mipos [0])
      mipo= mipos[1];
    else
      mipo= mipos[0];
    alpha= rootOf (mipo);

    QaFactors= factorize (F, alpha);

    for (CFFListIterator iter= QaFactors; iter.hasItem(); iter++)
    {
      out_cf ("QaFactors= ", iter.getItem().factor(), "\n");
      printf ("totaldegree ()= %d\n", totaldegree (iter.getItem().factor()));
      if (totaldegree (iter.getItem().factor()) == minTdeg)
      {
        result= CFAFList (CFAFactor (iter.getItem().factor(), getMipo (alpha), 1));
        found= true;
        break;
      }
    }
    if (!found)
    {
      rec= true;
      printf ("goto31\n");
      goto differentevalpoint;
    }
  }
  else if (!found)
  {
    rec= true;
    printf ("goto32\n");
    goto differentevalpoint;
  }

  return result;
}
#endif
#endif


