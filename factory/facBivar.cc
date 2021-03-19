/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facBivar.cc
 *
 * bivariate factorization over Q(a)
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "cf_assert.h"
#include "cf_util.h"
#include "debug.h"
#include "timing.h"

#include "cf_algorithm.h"
#include "facFqBivar.h"
#include "facBivar.h"
#include "facHensel.h"
#include "facMul.h"
#include "cf_primes.h"

TIMING_DEFINE_PRINT(fac_uni_factorizer)
TIMING_DEFINE_PRINT(fac_bi_hensel_lift)
TIMING_DEFINE_PRINT(fac_bi_factor_recombination)
TIMING_DEFINE_PRINT(fac_bi_evaluation)
TIMING_DEFINE_PRINT(fac_bi_shift_to_zero)

// bound on coeffs of f (cf. Musser: Multivariate Polynomial Factorization,
//                          Gelfond: Transcendental and Algebraic Numbers)
modpk
coeffBound ( const CanonicalForm & f, int p )
{
    int * degs = degrees( f );
    int M = 0, i, k = f.level();
    CanonicalForm b= 1;
    for ( i = 1; i <= k; i++ )
    {
      M += degs[i];
      b *= degs[i] + 1;
    }
    DELETE_ARRAY(degs);
    b /= power (CanonicalForm (2), k);
    b= b.sqrt() + 1;
    b *= 2 * maxNorm( f ) * power( CanonicalForm( 2 ), M );
    CanonicalForm B = p;
    k = 1;
    while ( B < b ) {
        B *= p;
        k++;
    }
    return modpk( p, k );
}

void findGoodPrime(const CanonicalForm &f, int &start)
{
  if (! f.inBaseDomain() )
  {
    CFIterator i = f;
    for(;;)
    {
      if  ( i.hasTerms() )
      {
        findGoodPrime(i.coeff(),start);
        if (0==cf_getBigPrime(start)) return;
        if((i.exp()!=0) && ((i.exp() % cf_getBigPrime(start))==0))
        {
          start++;
          i=f;
        }
        else  i++;
      }
      else break;
    }
  }
  else
  {
    if (f.inZ())
    {
      if (0==cf_getBigPrime(start)) return;
      while((!f.isZero()) && (mod(f,cf_getBigPrime(start))==0))
      {
        start++;
        if (0==cf_getBigPrime(start)) return;
      }
    }
  }
}

modpk
coeffBound ( const CanonicalForm & f, int p, const CanonicalForm& mipo )
{
    int * degs = degrees( f );
    int M = 0, i, k = f.level();
    CanonicalForm K= 1;
    for ( i = 1; i <= k; i++ )
    {
        M += degs[i];
        K *= degs[i] + 1;
    }
    DELETE_ARRAY(degs);
    K /= power (CanonicalForm (2), k/2);
    K *= power (CanonicalForm (2), M);
    int N= degree (mipo);
    CanonicalForm b;
    b= 2*power (maxNorm (f), N)*power (maxNorm (mipo), 4*N)*K*
       power (CanonicalForm (2), N)*
       power (CanonicalForm (N+1), 4*N);
    b /= power (abs (lc (mipo)), N);

    CanonicalForm B = p;
    k = 1;
    while ( B < b ) {
        B *= p;
        k++;
    }
    return modpk( p, k );
}

CFList conv (const CFFList& L)
{
  CFList result;
  for (CFFListIterator i= L; i.hasItem(); i++)
    result.append (i.getItem().factor());
  return result;
}

bool testPoint (const CanonicalForm& F, CanonicalForm& G, int i)
{
  G= F (i, 2);
  if (G.inCoeffDomain() || degree (F, 1) > degree (G, 1))
    return false;

  if (degree (gcd (deriv (G, G.mvar()), G)) > 0)
    return false;
  return true;
}

CanonicalForm evalPoint (const CanonicalForm& F, int& i)
{
  Variable x= Variable (1);
  Variable y= Variable (2);
  CanonicalForm result;

  int k;

  if (i == 0)
  {
    if (testPoint (F, result, i))
      return result;
  }
  do
  {
    if (i > 0)
      k= 1;
    else
      k= 2;
    while (k < 3)
    {
      if (k == 1)
      {
        if (testPoint (F, result, i))
          return result;
      }
      else
      {
        if (testPoint (F, result, -i))
        {
          i= -i;
          return result;
        }
        else if (i < 0)
          i= -i;
      }
      k++;
    }
    i++;
  } while (1);
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // henselLiftAndEarly
CFList biFactorize (const CanonicalForm& F, const Variable& v)
{
  if (F.inCoeffDomain())
    return CFList(F);

  bool extension= (v.level() != 1);
  CanonicalForm A;
  if (isOn (SW_RATIONAL))
    A= F*bCommonDen (F);
  else
    A= F;

  CanonicalForm mipo;
  if (extension)
    mipo= getMipo (v);

  if (A.isUnivariate())
  {
    CFFList buf;
    if (extension)
      buf= factorize (A, v);
    else
      buf= factorize (A, true);
    CFList result= conv (buf);
    if (result.getFirst().inCoeffDomain())
      result.removeFirst();
    return result;
  }

  CFMap N;
  A= compress (A, N);
  Variable y= A.mvar();

  if (y.level() > 2) return CFList (F);
  Variable x= Variable (1);

  CanonicalForm contentAx= content (A, x);
  CanonicalForm contentAy= content (A);

  A= A/(contentAx*contentAy);
  CFFList contentAxFactors, contentAyFactors;

  if (extension)
  {
    if (!contentAx.inCoeffDomain())
    {
      contentAxFactors= factorize (contentAx, v);
      if (contentAxFactors.getFirst().factor().inCoeffDomain())
        contentAxFactors.removeFirst();
    }
    if (!contentAy.inCoeffDomain())
    {
      contentAyFactors= factorize (contentAy, v);
      if (contentAyFactors.getFirst().factor().inCoeffDomain())
        contentAyFactors.removeFirst();
    }
  }
  else
  {
    if (!contentAx.inCoeffDomain())
    {
      contentAxFactors= factorize (contentAx, true);
      if (contentAxFactors.getFirst().factor().inCoeffDomain())
        contentAxFactors.removeFirst();
    }
    if (!contentAy.inCoeffDomain())
    {
      contentAyFactors= factorize (contentAy, true);
      if (contentAyFactors.getFirst().factor().inCoeffDomain())
        contentAyFactors.removeFirst();
    }
  }

  //check trivial case
  if (degree (A) == 1 || degree (A, 1) == 1 ||
      (size (A) == 2 && igcd (degree (A), degree (A,1)) == 1))
  {
    CFList factors;
    factors.append (A);

    appendSwapDecompress (factors, conv (contentAxFactors),
                          conv (contentAyFactors), false, false, N);

    normalize (factors);
    return factors;
  }

  //trivial case
  CFList factors;
  if (A.inCoeffDomain())
  {
    append (factors, conv (contentAxFactors));
    append (factors, conv (contentAyFactors));
    decompress (factors, N);
    return factors;
  }
  else if (A.isUnivariate())
  {
    if (extension)
      factors= conv (factorize (A, v));
    else
      factors= conv (factorize (A, true));
    append (factors, conv (contentAxFactors));
    append (factors, conv (contentAyFactors));
    decompress (factors, N);
    return factors;
  }

  if (irreducibilityTest (A))
  {
    CFList factors;
    factors.append (A);

    appendSwapDecompress (factors, conv (contentAxFactors),
                          conv (contentAyFactors), false, false, N);

    normalize (factors);
    return factors;
  }
  bool swap= false;
  if (degree (A) > degree (A, x))
  {
    A= swapvar (A, y, x);
    swap= true;
  }

  CanonicalForm Aeval, bufAeval, buf;
  CFList uniFactors, list, bufUniFactors;
  DegreePattern degs;
  DegreePattern bufDegs;

  CanonicalForm Aeval2, bufAeval2;
  CFList bufUniFactors2, list2, uniFactors2;
  DegreePattern degs2;
  DegreePattern bufDegs2;
  bool swap2= false;

  // several univariate factorizations to obtain more information about the
  // degree pattern therefore usually less combinations have to be tried during
  // the recombination process
  int factorNums= 2;
  int subCheck1= substituteCheck (A, x);
  int subCheck2= substituteCheck (A, y);
  buf= swapvar (A,x,y);
  int evaluation, evaluation2, bufEvaluation= 0, bufEvaluation2= 0;
  for (int i= 0; i < factorNums; i++)
  {
    bufAeval= A;
    TIMING_START (fac_bi_evaluation);
    bufAeval= evalPoint (A, bufEvaluation);
    TIMING_END_AND_PRINT (fac_bi_evaluation, "time for eval point over Q: ");

    bufAeval2= buf;
    TIMING_START (fac_bi_evaluation);
    bufAeval2= evalPoint (buf, bufEvaluation2);
    TIMING_END_AND_PRINT (fac_bi_evaluation,
                          "time for eval point over Q in y: ");

    // univariate factorization
    TIMING_START (fac_uni_factorizer);
    if (extension)
      bufUniFactors= conv (factorize (bufAeval, v));
    else
      bufUniFactors= conv (factorize (bufAeval, true));
    TIMING_END_AND_PRINT (fac_uni_factorizer,
                          "time for univariate factorization over Q: ");
    DEBOUTLN (cerr, "prod (bufUniFactors)== bufAeval " <<
              (prod (bufUniFactors) == bufAeval));

    if (bufUniFactors.getFirst().inCoeffDomain())
      bufUniFactors.removeFirst();

    if (bufUniFactors.length() == 1)
    {
      factors.append (A);

      appendSwapDecompress (factors, conv (contentAxFactors),
                            conv (contentAyFactors), swap, swap2, N);

      if (isOn (SW_RATIONAL))
        normalize (factors);
      return factors;
    }

    TIMING_START (fac_uni_factorizer);
    if (extension)
      bufUniFactors2= conv (factorize (bufAeval2, v));
    else
      bufUniFactors2= conv (factorize (bufAeval2, true));
    TIMING_END_AND_PRINT (fac_uni_factorizer,
                          "time for univariate factorization in y over Q: ");
    DEBOUTLN (cerr, "prod (bufuniFactors2)== bufAeval2 " <<
              (prod (bufUniFactors2) == bufAeval2));

    if (bufUniFactors2.getFirst().inCoeffDomain())
      bufUniFactors2.removeFirst();
    if (bufUniFactors2.length() == 1)
    {
      factors.append (A);

      appendSwapDecompress (factors, conv (contentAxFactors),
                            conv (contentAyFactors), swap, swap2, N);

      if (isOn (SW_RATIONAL))
        normalize (factors);
      return factors;
    }

    if (i == 0)
    {
      if (subCheck1 > 0)
      {
        int subCheck= substituteCheck (bufUniFactors);

        if (subCheck > 1 && (subCheck1%subCheck == 0))
        {
          CanonicalForm bufA= A;
          subst (bufA, bufA, subCheck, x);
          factors= biFactorize (bufA, v);
          reverseSubst (factors, subCheck, x);
          appendSwapDecompress (factors, conv (contentAxFactors),
                                conv (contentAyFactors), swap, swap2, N);
          if (isOn (SW_RATIONAL))
            normalize (factors);
          return factors;
        }
      }

      if (subCheck2 > 0)
      {
        int subCheck= substituteCheck (bufUniFactors2);

        if (subCheck > 1 && (subCheck2%subCheck == 0))
        {
          CanonicalForm bufA= A;
          subst (bufA, bufA, subCheck, y);
          factors= biFactorize (bufA, v);
          reverseSubst (factors, subCheck, y);
          appendSwapDecompress (factors, conv (contentAxFactors),
                                conv (contentAyFactors), swap, swap2, N);
          if (isOn (SW_RATIONAL))
            normalize (factors);
          return factors;
        }
      }
    }

    // degree analysis
    bufDegs = DegreePattern (bufUniFactors);
    bufDegs2= DegreePattern (bufUniFactors2);

    if (i == 0)
    {
      Aeval= bufAeval;
      evaluation= bufEvaluation;
      uniFactors= bufUniFactors;
      degs= bufDegs;
      Aeval2= bufAeval2;
      evaluation2= bufEvaluation2;
      uniFactors2= bufUniFactors2;
      degs2= bufDegs2;
    }
    else
    {
      degs.intersect (bufDegs);
      degs2.intersect (bufDegs2);
      if (bufUniFactors2.length() < uniFactors2.length())
      {
        uniFactors2= bufUniFactors2;
        Aeval2= bufAeval2;
        evaluation2= bufEvaluation2;
      }
      if (bufUniFactors.length() < uniFactors.length())
      {
        uniFactors= bufUniFactors;
        Aeval= bufAeval;
        evaluation= bufEvaluation;
      }
    }
    if (bufEvaluation > 0)
      bufEvaluation++;
    else
      bufEvaluation= -bufEvaluation + 1;
    if (bufEvaluation > 0)
      bufEvaluation2++;
    else
      bufEvaluation2= -bufEvaluation2 + 1;
  }

  if (uniFactors.length() > uniFactors2.length() ||
      (uniFactors.length() == uniFactors2.length()
       && degs.getLength() > degs2.getLength()))
  {
    degs= degs2;
    uniFactors= uniFactors2;
    evaluation= evaluation2;
    Aeval= Aeval2;
    A= buf;
    swap2= true;
  }

  if (degs.getLength() == 1) // A is irreducible
  {
    factors.append (A);
    appendSwapDecompress (factors, conv (contentAxFactors),
                          conv (contentAyFactors), swap, swap2, N);
    if (isOn (SW_RATIONAL))
      normalize (factors);
    return factors;
  }

  A *= bCommonDen (A);
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
    #ifdef HAVE_FLINT
    // init
    fmpz_t FLINTf,FLINTD;
    fmpz_init(FLINTf);
    fmpz_init(FLINTD);
    fmpz_poly_t FLINTmipo;
    fmpz_poly_t FLINTLcf;
    //conversion
    convertFacCF2Fmpz_poly_t(FLINTmipo,mipo);
    convertFacCF2Fmpz_poly_t(FLINTLcf,Lc (A*bCommonDen (A)));
    // resultant, discriminant
    fmpz_poly_resultant(FLINTf,FLINTmipo,FLINTLcf);
    fmpz_poly_discriminant(FLINTD,FLINTmipo);
    fmpz_mul(FLINTf,FLINTD,FLINTf);
    // conversion
    den= abs (convertFmpz2CF(FLINTf));
    // clean up
    fmpz_clear(FLINTf);
    // FLINTD is used below
    fmpz_poly_clear(FLINTLcf);
    fmpz_poly_clear(FLINTmipo);
    #elif defined(HAVE_NTL)
    ZZX NTLmipo= convertFacCF2NTLZZX (mipo);
    ZZX NTLLcf= convertFacCF2NTLZZX (Lc (A*bCommonDen (A)));
    ZZ NTLf= resultant (NTLmipo, NTLLcf);
    ZZ NTLD= discriminant (NTLmipo);
    den= abs (convertZZ2CF (NTLD*NTLf));
    #else
    factoryError("NTL/FLINT missing: biFactorize");
    #endif

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
    #ifdef HAVE_FLINT
    CanonicalForm discMipo= convertFmpz2CF(FLINTD);
    fmpz_clear(FLINTD);
    #elif defined(HAVE_NTL)
    CanonicalForm discMipo= convertZZ2CF (NTLD);
    #else
    factoryError("NTL/FLINT missing: biFactorize");
    #endif
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
    //prune (vv);
  }

  On (SW_RATIONAL);
  A *= bCommonDen (A);
  Off (SW_RATIONAL);

  TIMING_START (fac_bi_factor_recombination);
  factors= factorRecombination (uniFactors, A, MODl, degs, evaluation, 1,
                                uniFactors.length()/2, b, den);
  TIMING_END_AND_PRINT (fac_bi_factor_recombination,
                        "time for bivariate factor recombination over Q: ");

  On (SW_RATIONAL);

  if (earlySuccess)
    factors= Union (earlyFactors, factors);
  else if (!earlySuccess && degs.getLength() == 1)
    factors= earlyFactors;

  appendSwapDecompress (factors, conv (contentAxFactors),
                        conv (contentAyFactors), swap, swap2, N);
  if (isOn (SW_RATIONAL))
    normalize (factors);

  return factors;
}
#endif

