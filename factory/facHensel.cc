/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facHensel.cc
 *
 * This file implements functions to lift factors via Hensel lifting.
 *
 * ABSTRACT: Hensel lifting is described in "Efficient Multivariate
 * Factorization over Finite Fields" by L. Bernardin & M. Monagon and
 * "Algorithms for Computer Algebra" by Geddes, Czapor, Labahn
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "cfGcdAlgExt.h"
#include "facHensel.h"
#include "facMul.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "cf_primes.h"
#include "facBivar.h"
#include "cfNTLzzpEXGCD.h"
#include "cfUnivarGcd.h"

#ifdef HAVE_NTL
#include <NTL/lzz_pEX.h>
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

TIMING_DEFINE_PRINT (diotime)
TIMING_DEFINE_PRINT (product1)
TIMING_DEFINE_PRINT (product2)
TIMING_DEFINE_PRINT (hensel23)
TIMING_DEFINE_PRINT (hensel)

#if defined (HAVE_NTL) || defined(HAVE_FLINT)

#if (!(HAVE_FLINT && __FLINT_RELEASE >= 20400))
static
CFList productsNTL (const CFList& factors, const CanonicalForm& M)
{
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLMipo= convertFacCF2NTLzzpX (M);
  zz_pE::init (NTLMipo);
  zz_pEX prod;
  vec_zz_pEX v;
  v.SetLength (factors.length());
  int j= 0;
  for (CFListIterator i= factors; i.hasItem(); i++, j++)
  {
    if (i.getItem().inCoeffDomain())
      v[j]= to_zz_pEX (to_zz_pE (convertFacCF2NTLzzpX (i.getItem())));
    else
      v[j]= convertFacCF2NTLzz_pEX (i.getItem(), NTLMipo);
  }
  CFList result;
  Variable x= Variable (1);
  for (int j= 0; j < factors.length(); j++)
  {
    int k= 0;
    set(prod);
    for (int i= 0; i < factors.length(); i++, k++)
    {
      if (k == j)
        continue;
      prod *= v[i];
    }
    result.append (convertNTLzz_pEX2CF (prod, x, M.mvar()));
  }
  return result;
}
#endif

#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
static
CFList productsFLINT (const CFList& factors, const CanonicalForm& M)
{
  nmod_poly_t FLINTmipo;
  fq_nmod_ctx_t fq_con;
  fq_nmod_poly_t prod;
  fq_nmod_t buf;

  nmod_poly_init (FLINTmipo, getCharacteristic());
  convertFacCF2nmod_poly_t (FLINTmipo, M);

  fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");

  fq_nmod_poly_t * vec=new fq_nmod_poly_t [factors.length()];

  int j= 0;

  for (CFListIterator i= factors; i.hasItem(); i++, j++)
  {
    if (i.getItem().inCoeffDomain())
    {
      fq_nmod_poly_init (vec[j], fq_con);
      fq_nmod_init2 (buf, fq_con);
      convertFacCF2Fq_nmod_t (buf, i.getItem(), fq_con);
      fq_nmod_poly_set_coeff (vec[j], 0, buf, fq_con);
      fq_nmod_clear (buf, fq_con);
    }
    else
      convertFacCF2Fq_nmod_poly_t (vec[j], i.getItem(), fq_con);
  }

  CFList result;
  Variable x= Variable (1);
  fq_nmod_poly_init (prod, fq_con);
  for (j= 0; j < factors.length(); j++)
  {
    int k= 0;
    fq_nmod_poly_one (prod, fq_con);
    for (int i= 0; i < factors.length(); i++, k++)
    {
      if (k == j)
        continue;
      fq_nmod_poly_mul (prod, prod, vec[i], fq_con);
    }
    result.append (convertFq_nmod_poly_t2FacCF (prod, x, M.mvar(), fq_con));
  }
  for (j= 0; j < factors.length(); j++)
    fq_nmod_poly_clear (vec[j], fq_con);

  nmod_poly_clear (FLINTmipo);
  fq_nmod_poly_clear (prod, fq_con);
  fq_nmod_ctx_clear (fq_con);
  delete [] vec;
  return result;
}
#endif

static
void tryDiophantine (CFList& result, const CanonicalForm& F,
                     const CFList& factors, const CanonicalForm& M, bool& fail)
{
  ASSERT (M.isUnivariate(), "expected univariate poly");

  CFList bufFactors= factors;
  bufFactors.removeFirst();
  bufFactors.insert (factors.getFirst () (0,2));
  CanonicalForm inv, leadingCoeff= Lc (F);
  CFListIterator i= bufFactors;

  result = CFList();    // empty the list before writing into it?!

  if (bufFactors.getFirst().inCoeffDomain())
  {
    if (i.hasItem())
      i++;
  }
  for (; i.hasItem(); i++)
  {
    tryInvert (Lc (i.getItem()), M, inv ,fail);
    if (fail)
      return;
    i.getItem()= reduce (i.getItem()*inv, M);
  }
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
  bufFactors= productsFLINT (bufFactors, M);
#else
  bufFactors= productsNTL (bufFactors, M);
#endif

  CanonicalForm buf1, buf2, buf3, S, T;
  i= bufFactors;
  if (i.hasItem())
    i++;
  buf1= bufFactors.getFirst();
  buf2= i.getItem();
#ifdef HAVE_NTL
  Variable x= Variable (1);
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLMipo= convertFacCF2NTLzzpX (M);
  zz_pE::init (NTLMipo);
  zz_pEX NTLbuf1, NTLbuf2, NTLbuf3, NTLS, NTLT;
  NTLbuf1= convertFacCF2NTLzz_pEX (buf1, NTLMipo);
  NTLbuf2= convertFacCF2NTLzz_pEX (buf2, NTLMipo);
  tryNTLXGCD (NTLbuf3, NTLS, NTLT, NTLbuf1, NTLbuf2, fail);
  if (fail)
    return;
  S= convertNTLzz_pEX2CF (NTLS, x, M.mvar());
  T= convertNTLzz_pEX2CF (NTLT, x, M.mvar());
#else
  tryExtgcd (buf1, buf2, M, buf3, S, T, fail);
  if (fail)
    return;
#endif
  result.append (S);
  result.append (T);
  if (i.hasItem())
    i++;
  for (; i.hasItem(); i++)
  {
#ifdef HAVE_NTL
    NTLbuf1= convertFacCF2NTLzz_pEX (i.getItem(), NTLMipo);
    tryNTLXGCD (NTLbuf3, NTLS, NTLT, NTLbuf3, NTLbuf1, fail);
    if (fail)
      return;
    S= convertNTLzz_pEX2CF (NTLS, x, M.mvar());
    T= convertNTLzz_pEX2CF (NTLT, x, M.mvar());
#else
    buf1= i.getItem();
    tryExtgcd (buf3, buf1, M, buf3, S, T, fail);
    if (fail)
      return;
#endif
    CFListIterator k= factors;
    for (CFListIterator j= result; j.hasItem(); j++, k++)
    {
      j.getItem() *= S;
      j.getItem()= mod (j.getItem(), k.getItem());
      j.getItem()= reduce (j.getItem(), M);
    }
    result.append (T);
  }
}

static
CFList mapinto (const CFList& L)
{
  CFList result;
  for (CFListIterator i= L; i.hasItem(); i++)
    result.append (mapinto (i.getItem()));
  return result;
}

static
int mod (const CFList& L, const CanonicalForm& p)
{
  for (CFListIterator i= L; i.hasItem(); i++)
  {
    if (mod (i.getItem(), p) == 0)
      return 0;
  }
  return 1;
}


static void
chineseRemainder (const CFList & x1, const CanonicalForm & q1,
                  const CFList & x2, const CanonicalForm & q2,
                  CFList & xnew, CanonicalForm & qnew)
{
  ASSERT (x1.length() == x2.length(), "expected lists of equal length");
  CanonicalForm tmp1, tmp2;
  CFListIterator j= x2;
  for (CFListIterator i= x1; i.hasItem() && j.hasItem(); i++, j++)
  {
    chineseRemainder (i.getItem(), q1, j.getItem(), q2, tmp1, tmp2);
    xnew.append (tmp1);
  }
  qnew= tmp2;
}

static
CFList Farey (const CFList& L, const CanonicalForm& q)
{
  CFList result;
  for (CFListIterator i= L; i.hasItem(); i++)
    result.append (Farey (i.getItem(), q));
  return result;
}

static
CFList replacevar (const CFList& L, const Variable& a, const Variable& b)
{
  CFList result;
  for (CFListIterator i= L; i.hasItem(); i++)
    result.append (replacevar (i.getItem(), a, b));
  return result;
}

CFList
modularDiophant (const CanonicalForm& f, const CFList& factors,
                 const CanonicalForm& M)
{
  bool save_rat=!isOn (SW_RATIONAL);
  On (SW_RATIONAL);
  CanonicalForm F= f*bCommonDen (f);
  CFList products= factors;
  for (CFListIterator i= products; i.hasItem(); i++)
  {
    if (products.getFirst().level() == 1)
      i.getItem() /= Lc (i.getItem());
    i.getItem() *= bCommonDen (i.getItem());
  }
  if (products.getFirst().level() == 1)
    products.insert (Lc (F));
  CanonicalForm bound= maxNorm (F);
  CFList leadingCoeffs;
  leadingCoeffs.append (lc (F));
  CanonicalForm dummy;
  for (CFListIterator i= products; i.hasItem(); i++)
  {
    leadingCoeffs.append (lc (i.getItem()));
    dummy= maxNorm (i.getItem());
    bound= (dummy > bound) ? dummy : bound;
  }
  bound *= maxNorm (Lc (F))*maxNorm (Lc(F))*bound;
  bound *= bound*bound;
  bound= power (bound, degree (M));
  bound *= power (CanonicalForm (2),degree (f));
  CanonicalForm bufBound= bound;
  int i = cf_getNumBigPrimes() - 1;
  int p;
  CFList resultModP, result, newResult;
  CanonicalForm q (0), newQ;
  bool fail= false;
  Variable a= M.mvar();
  Variable b= Variable (2);
  setReduce (M.mvar(), false);
  CanonicalForm mipo= bCommonDen (M)*M;
  Off (SW_RATIONAL);
  CanonicalForm modMipo;
  leadingCoeffs.append (lc (mipo));
  CFList tmp1, tmp2;
  bool equal= false;
  int count= 0;
  do
  {
    p = cf_getBigPrime( i );
    i--;
    while ( i >= 0 && mod( leadingCoeffs, p ) == 0)
    {
      p = cf_getBigPrime( i );
      i--;
    }

    ASSERT (i >= 0, "ran out of primes"); //sic

    setCharacteristic (p);
    modMipo= mapinto (mipo);
    modMipo /= lc (modMipo);
    resultModP= CFList();
    tryDiophantine (resultModP, mapinto (F), mapinto (products), modMipo, fail);
    setCharacteristic (0);
    if (fail)
    {
      fail= false;
      continue;
    }

    if ( q.isZero() )
    {
      result= replacevar (mapinto(resultModP), a, b);
      q= p;
    }
    else
    {
      result= replacevar (result, a, b);
      newResult= CFList();
      chineseRemainder( result, q, replacevar (mapinto (resultModP), a, b),
                        p, newResult, newQ );
      q= newQ;
      result= newResult;
      if (newQ > bound)
      {
        count++;
        tmp1= replacevar (Farey (result, q), b, a);
        if (tmp2.isEmpty())
          tmp2= tmp1;
        else
        {
          equal= true;
          CFListIterator k= tmp1;
          for (CFListIterator j= tmp2; j.hasItem(); j++, k++)
          {
            if (j.getItem() != k.getItem())
              equal= false;
          }
          if (!equal)
            tmp2= tmp1;
        }
        if (count > 2)
        {
          bound *= bufBound;
          equal= false;
          count= 0;
        }
      }
      if (newQ > bound && equal)
      {
        On( SW_RATIONAL );
        CFList bufResult= result;
        result= tmp2;
        setReduce (M.mvar(), true);
        if (factors.getFirst().level() == 1)
        {
          result.removeFirst();
          CFListIterator j= factors;
          CanonicalForm denf= bCommonDen (f);
          for (CFListIterator i= result; i.hasItem(); i++, j++)
            i.getItem() *= Lc (j.getItem())*denf;
        }
        if (factors.getFirst().level() != 1 &&
            !bCommonDen (factors.getFirst()).isOne())
        {
          CanonicalForm denFirst= bCommonDen (factors.getFirst());
          for (CFListIterator i= result; i.hasItem(); i++)
            i.getItem() *= denFirst;
        }

        CanonicalForm test= 0;
        CFListIterator jj= factors;
        for (CFListIterator ii= result; ii.hasItem(); ii++, jj++)
          test += ii.getItem()*(f/jj.getItem());
        if (!test.isOne())
        {
          bound *= bufBound;
          equal= false;
          count= 0;
          setReduce (M.mvar(), false);
          result= bufResult;
          Off (SW_RATIONAL);
        }
        else
          break;
      }
    }
  } while (1);
  if (save_rat) Off(SW_RATIONAL);
  return result;
}

void sortList (CFList& list, const Variable& x)
{
  int l= 1;
  int k= 1;
  CanonicalForm buf;
  CFListIterator m;
  for (CFListIterator i= list; l <= list.length(); i++, l++)
  {
    for (CFListIterator j= list; k <= list.length() - l; k++)
    {
      m= j;
      m++;
      if (degree (j.getItem(), x) > degree (m.getItem(), x))
      {
        buf= m.getItem();
        m.getItem()= j.getItem();
        j.getItem()= buf;
        j++;
        j.getItem()= m.getItem();
      }
      else
        j++;
    }
    k= 1;
  }
}

CFList
diophantine (const CanonicalForm& F, const CFList& factors);

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantine
CFList
diophantineHensel (const CanonicalForm & F, const CFList& factors,
                   const modpk& b)
{
  int p= b.getp();
  setCharacteristic (p);
  CFList recResult= diophantine (mapinto (F), mapinto (factors));
  setCharacteristic (0);
  recResult= mapinto (recResult);
  CanonicalForm e= 1;
  CFList L;
  CFArray bufFactors= CFArray (factors.length());
  int k= 0;
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
  {
    if (k == 0)
      bufFactors[k]= i.getItem() (0);
    else
      bufFactors [k]= i.getItem();
  }
  CanonicalForm tmp, quot;
  for (k= 0; k < factors.length(); k++) //TODO compute b's faster
  {
    tmp= 1;
    for (int l= 0; l < factors.length(); l++)
    {
      if (l == k)
        continue;
      else
      {
        tmp= mulNTL (tmp, bufFactors[l]);
      }
    }
    L.append (tmp);
  }

  setCharacteristic (p);
  for (k= 0; k < factors.length(); k++)
    bufFactors [k]= bufFactors[k].mapinto();
  setCharacteristic(0);

  CFListIterator j= L;
  for (CFListIterator i= recResult; i.hasItem(); i++, j++)
    e= b (e - mulNTL (i.getItem(),j.getItem(), b));

  if (e.isZero())
    return recResult;
  CanonicalForm coeffE;
  CFList s;
  CFList result= recResult;
  setCharacteristic (p);
  recResult= mapinto (recResult);
  setCharacteristic (0);
  CanonicalForm g;
  CanonicalForm modulus= p;
  int d= b.getk();
  modpk b2;
  for (int i= 1; i < d; i++)
  {
    coeffE= div (e, modulus);
    setCharacteristic (p);
    coeffE= coeffE.mapinto();
    setCharacteristic (0);
    b2= modpk (p, d - i);
    if (!coeffE.isZero())
    {
      CFListIterator k= result;
      CFListIterator l= L;
      int ii= 0;
      j= recResult;
      for (; j.hasItem(); j++, k++, l++, ii++)
      {
        setCharacteristic (p);
        g= modNTL (coeffE, bufFactors[ii]);
        g= mulNTL (g, j.getItem());
        g= modNTL (g, bufFactors[ii]);
        setCharacteristic (0);
        k.getItem() += g.mapinto()*modulus;
        e -= mulNTL (g.mapinto(), b2 (l.getItem()), b2)*modulus;
        e= b(e);
      }
    }
    modulus *= p;
    if (e.isZero())
      break;
  }

  return result;
}
#endif

/// solve \f$ 1=\sum_{i=1}^n{\delta_{i} \prod_{j\neq i}{f_j}} \f$ mod \f$p^k\f$
/// over \f$ Q(\alpha) \f$ by p-adic lifting
CFList
diophantineHenselQa (const CanonicalForm & F, const CanonicalForm& G,
                     const CFList& factors, modpk& b, const Variable& alpha)
{
  bool fail= false;
  CFList recResult;
  CanonicalForm modMipo, mipo;
  //here SW_RATIONAL is off
  On (SW_RATIONAL);
  mipo= getMipo (alpha);
  bool mipoHasDen= false;
  if (!bCommonDen (mipo).isOne())
  {
    mipo *= bCommonDen (mipo);
    mipoHasDen= true;
  }
  Off (SW_RATIONAL);
  int p= b.getp();
  setCharacteristic (p);
  setReduce (alpha, false);
  while (1)
  {
    setCharacteristic (p);
    modMipo= mapinto (mipo);
    modMipo /= lc (modMipo);
    tryDiophantine (recResult, mapinto (F), mapinto (factors), modMipo, fail);
    if (fail)
    {
      int i= 0;
      while (cf_getBigPrime (i) < p)
        i++;
      findGoodPrime (F, i);
      findGoodPrime (G, i);
      p=cf_getBigPrime(i);
      b = coeffBound( G, p, mipo );
      modpk bb= coeffBound (F, p, mipo );
      if (bb.getk() > b.getk() ) b=bb;
      fail= false;
    }
    else
      break;
  }
  setCharacteristic (0);
  recResult= mapinto (recResult);
  setReduce (alpha, true);
  CanonicalForm e= 1;
  CFList L;
  CFArray bufFactors= CFArray (factors.length());
  int k= 0;
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
  {
    if (k == 0)
      bufFactors[k]= i.getItem() (0);
    else
      bufFactors [k]= i.getItem();
  }
  CanonicalForm tmp;
  On (SW_RATIONAL);
  for (k= 0; k < factors.length(); k++) //TODO compute b's faster
  {
    tmp= 1;
    for (int l= 0; l < factors.length(); l++)
    {
      if (l == k)
        continue;
      else
        tmp= mulNTL (tmp, bufFactors[l]);
    }
    L.append (tmp*bCommonDen(tmp));
  }

  Variable gamma;
  CanonicalForm den;
  if (mipoHasDen)
  {
    modMipo= getMipo (alpha);
    den= bCommonDen (modMipo);
    modMipo *= den;
    Off (SW_RATIONAL);
    setReduce (alpha, false);
    gamma= rootOf (b (modMipo*b.inverse (den)));
    setReduce (alpha, true);
  }

  setCharacteristic (p);
  Variable beta;
  Off (SW_RATIONAL);
  setReduce (alpha, false);
  modMipo= modMipo.mapinto();
  modMipo /= lc (modMipo);
  beta= rootOf (modMipo);
  setReduce (alpha, true);

  setReduce (alpha, false);
  for (k= 0; k < factors.length(); k++)
  {
    bufFactors [k]= bufFactors[k].mapinto();
    bufFactors [k]= replacevar (bufFactors[k], alpha, beta);
  }
  setReduce (alpha, true);
  setCharacteristic(0);

  CFListIterator j= L;
  for (;j.hasItem(); j++)
  {
    if (mipoHasDen)
      j.getItem()= replacevar (b(j.getItem()*b.inverse(lc(j.getItem()))),
                               alpha, gamma);
    else
      j.getItem()= b(j.getItem()*b.inverse(lc(j.getItem())));
  }
  j= L;
  for (CFListIterator i= recResult; i.hasItem(); i++, j++)
  {
    if (mipoHasDen)
      e= b (e - mulNTL (replacevar (i.getItem(), alpha, gamma),j.getItem(), b));
    else
      e= b (e - mulNTL (i.getItem(), j.getItem(), b));
  }

  if (e.isZero())
  {
    if (mipoHasDen)
    {
      for (CFListIterator i= recResult; i.hasItem(); i++)
        i.getItem()= replacevar (i.getItem(), alpha, gamma);
    }
    return recResult;
  }
  CanonicalForm coeffE;
  CFList result= recResult;
  if (mipoHasDen)
  {
    for (CFListIterator i= result; i.hasItem(); i++)
      i.getItem()= replacevar (i.getItem(), alpha, gamma);
  }
  setCharacteristic (p);
  setReduce (alpha, false);
  recResult= mapinto (recResult);
  setReduce (alpha, true);

  for (CFListIterator i= recResult; i.hasItem(); i++)
    i.getItem()= replacevar (i.getItem(), alpha, beta);

  setCharacteristic (0);
  CanonicalForm g;
  CanonicalForm modulus= p;
  int d= b.getk();
  modpk b2;
  for (int i= 1; i < d; i++)
  {
    coeffE= div (e, modulus);
    setCharacteristic (p);
    if (mipoHasDen)
      setReduce (gamma, false);
    else
      setReduce (alpha, false);
    coeffE= coeffE.mapinto();
    if (mipoHasDen)
      setReduce (gamma, true);
    else
      setReduce (alpha, true);
    if (mipoHasDen)
      coeffE= replacevar (coeffE, gamma, beta);
    else
      coeffE= replacevar (coeffE, alpha, beta);
    setCharacteristic (0);
    b2= modpk (p, d - i);
    if (!coeffE.isZero())
    {
      CFListIterator k= result;
      CFListIterator l= L;
      int ii= 0;
      j= recResult;
      for (; j.hasItem(); j++, k++, l++, ii++)
      {
        setCharacteristic (p);
        g= modNTL (coeffE, bufFactors[ii]);
        g= mulNTL (g, j.getItem());
        g= modNTL (g, bufFactors[ii]);
        setCharacteristic (0);
        if (mipoHasDen)
        {
          setReduce (beta, false);
          k.getItem() += replacevar (g.mapinto()*modulus, beta, gamma);
          e -= mulNTL (replacevar (g.mapinto(), beta, gamma),
                       b2 (l.getItem()), b2)*modulus;
          setReduce (beta, true);
        }
        else
        {
          setReduce (beta, false);
          k.getItem() += replacevar (g.mapinto()*modulus, beta, alpha);
          e -= mulNTL (replacevar (g.mapinto(), beta, alpha),
                       b2 (l.getItem()), b2)*modulus;
          setReduce (beta, true);
        }
        e= b(e);
      }
    }
    modulus *= p;
    if (e.isZero())
      break;
  }

  return result;
}



/// solve \f$ 1=\sum_{i=1}^n{\delta_{i} \prod_{j\neq i}{f_j}} \f$ mod \f$p^k\f$
/// over \f$ Q(\alpha) \f$ by first computing mod \f$p\f$ and if no zero divisor
/// occurred compute it mod \f$p^k\f$
#if defined(HAVE_NTL) || defined(HAVE_FLINT) // XGCD, zzp_eX
CFList
diophantineQa (const CanonicalForm& F, const CanonicalForm& G,
               const CFList& factors, modpk& b, const Variable& alpha)
{
  bool fail= false;
  CFList recResult;
  CanonicalForm modMipo, mipo;
#if HAVE_NTL
  // no variables for ntl
#else
  fmpz_t bigpk;
  fq_ctx_t fqctx;
  fq_poly_t FLINTS, FLINTT, FLINTbuf3, FLINTbuf1, FLINTbuf2;
  fq_t fcheck;
#endif

  //here SW_RATIONAL is off
  On (SW_RATIONAL);
  mipo= getMipo (alpha);
  bool mipoHasDen= false;
  if (!bCommonDen (mipo).isOne())
  {
    mipo *= bCommonDen (mipo);
    mipoHasDen= true;
  }
  Off (SW_RATIONAL);
  int p= b.getp();
  setCharacteristic (p);
  setReduce (alpha, false);
  while (1)
  {
    setCharacteristic (p);
    modMipo= mapinto (mipo);
    modMipo /= lc (modMipo);
    tryDiophantine (recResult, mapinto (F), mapinto (factors), modMipo, fail);
    if (fail)
    {
next_prime:
      int i= 0;
      while (cf_getBigPrime (i) <= p)
        i++;
      findGoodPrime (F, i);
      findGoodPrime (G, i);
      p=cf_getBigPrime(i);
      b = coeffBound( G, p, mipo );
      modpk bb= coeffBound (F, p, mipo );
      if (bb.getk() > b.getk() ) b=bb;
      fail= false;
    }
    else
      break;
  }
  setReduce (alpha, true);
  setCharacteristic (0);

  Variable gamma= alpha;
  CanonicalForm den;
  if (mipoHasDen)
  {
    On (SW_RATIONAL);
    modMipo= getMipo (alpha);
    den= bCommonDen (modMipo);
    modMipo *= den;
    Off (SW_RATIONAL);
    setReduce (alpha, false);
    gamma= rootOf (b (modMipo*b.inverse (den)));
    setReduce (alpha, true);
  }

  Variable x= Variable (1);
  CanonicalForm buf1, buf2, buf3, S;
  CFList bufFactors= factors;
  CFListIterator i= bufFactors;
  if (mipoHasDen)
  {
    for (; i.hasItem(); i++)
      i.getItem()= replacevar (i.getItem(), alpha, gamma);
  }
  i= bufFactors;
  CFList result;
  if (i.hasItem())
    i++;
  buf1= 0;
  CanonicalForm Freplaced;
  if (mipoHasDen)
  {
    Freplaced= replacevar (F, alpha, gamma);
    buf2= divNTL (Freplaced, replacevar (i.getItem(), alpha, gamma), b);
  }
  else
    buf2= divNTL (F, i.getItem(), b);

#ifdef HAVE_NTL
  ZZ_p::init (convertFacCF2NTLZZ (b.getpk()));
  ZZ_pX NTLmipo= to_ZZ_pX (convertFacCF2NTLZZX (getMipo (gamma)));
  ZZ_pE::init (NTLmipo);
  ZZ_pEX NTLS, NTLT, NTLbuf3;
  ZZ_pEX NTLbuf1= convertFacCF2NTLZZ_pEX (buf1, NTLmipo);
  ZZ_pEX NTLbuf2= convertFacCF2NTLZZ_pEX (buf2, NTLmipo);
  XGCD (NTLbuf3, NTLS, NTLT, NTLbuf1, NTLbuf2);
  result.append (b (convertNTLZZ_pEX2CF (NTLS, x, gamma)));
  result.append (b (convertNTLZZ_pEX2CF (NTLT, x, gamma)));
#else // HAVE_FLINT

  fmpz_init(bigpk); // does convert expect an initalized object?
  convertCF2Fmpz(bigpk, b.getpk());
  fmpz_mod_poly_t FLINTmipo;
  convertFacCF2Fmpz_mod_poly_t(FLINTmipo, getMipo(gamma), bigpk);
#if __FLINT_RELEASE >= 20700
  fmpz_mod_ctx_t bigpk_ctx;
  fmpz_mod_ctx_init(bigpk_ctx, bigpk);
  fq_ctx_init_modulus(fqctx, FLINTmipo, bigpk_ctx, "Z");
  fmpz_mod_ctx_clear(bigpk_ctx);
  fmpz_mod_poly_clear(FLINTmipo, bigpk_ctx);
#else
  fq_ctx_init_modulus(fqctx, FLINTmipo, "Z");
  fmpz_mod_poly_clear(FLINTmipo);
#endif

  fq_init(fcheck, fqctx);
  fq_poly_init(FLINTS, fqctx);
  fq_poly_init(FLINTT, fqctx);
  fq_poly_init(FLINTbuf3, fqctx);
  //fq_poly_init(FLINTbuf1, fqctx); //convert expects uninitialized!
  //fq_poly_init(FLINTbuf2, fqctx); //convert expects uninitialized!
  convertFacCF2Fq_poly_t(FLINTbuf1, buf1, fqctx);
  convertFacCF2Fq_poly_t(FLINTbuf2, buf2, fqctx);

  fq_poly_xgcd_euclidean_f(fcheck, FLINTbuf3, FLINTS, FLINTT,
                                                  FLINTbuf1, FLINTbuf2, fqctx);
  if (!fq_is_one(fcheck, fqctx))
  {
    fmpz_clear(bigpk);
    fq_clear(fcheck, fqctx);
    fq_poly_clear(FLINTS, fqctx);
    fq_poly_clear(FLINTT, fqctx);
    fq_poly_clear(FLINTbuf3, fqctx);
    fq_poly_clear(FLINTbuf1, fqctx);
    fq_poly_clear(FLINTbuf2, fqctx);
    fq_ctx_clear(fqctx);
    setReduce (alpha, false);
    fail = true;
    goto next_prime;
  }

  result.append(b(convertFq_poly_t2FacCF(FLINTS, x, alpha, fqctx)));
  result.append(b(convertFq_poly_t2FacCF(FLINTT, x, alpha, fqctx)));
#endif

  if (i.hasItem())
    i++;
  for (; i.hasItem(); i++)
  {
    if (mipoHasDen)
      buf1= divNTL (Freplaced, i.getItem(), b);
    else
      buf1= divNTL (F, i.getItem(), b);

#ifdef HAVE_NTL
    XGCD (NTLbuf3, NTLS, NTLT, NTLbuf3, convertFacCF2NTLZZ_pEX (buf1, NTLmipo));
    S= convertNTLZZ_pEX2CF (NTLS, x, gamma);
#else
    fq_poly_clear(FLINTbuf1, fqctx); //convert expects uninitialized!
    convertFacCF2Fq_poly_t(FLINTbuf1, buf1, fqctx);
    fq_poly_xgcd_euclidean_f(fcheck, FLINTbuf3, FLINTS, FLINTT,
                                                  FLINTbuf3, FLINTbuf1, fqctx);
    if (!fq_is_one(fcheck, fqctx))
    {
      fmpz_clear(bigpk);
      fq_clear(fcheck, fqctx);
      fq_poly_clear(FLINTS, fqctx);
      fq_poly_clear(FLINTT, fqctx);
      fq_poly_clear(FLINTbuf3, fqctx);
      fq_poly_clear(FLINTbuf1, fqctx);
      fq_poly_clear(FLINTbuf2, fqctx);
      fq_ctx_clear(fqctx);
      setReduce (alpha, false);
      fail = true;
      goto next_prime;
    }

    S= convertFq_poly_t2FacCF(FLINTS, x, alpha, fqctx);
#endif

    CFListIterator k= bufFactors;
    for (CFListIterator j= result; j.hasItem(); j++, k++)
    {
      j.getItem()= mulNTL (j.getItem(), S, b);
      j.getItem()= modNTL (j.getItem(), k.getItem(), b);
    }
#if HAVE_NTL
    result.append (b (convertNTLZZ_pEX2CF (NTLT, x, gamma)));
#else
    result.append (b (convertFq_poly_t2FacCF(FLINTT, x, alpha, fqctx)));
#endif
  }

#if HAVE_NTL
  // no cleanup for ntl
#else
  fmpz_clear(bigpk);
  fq_clear(fcheck, fqctx);
  fq_poly_clear(FLINTS, fqctx);
  fq_poly_clear(FLINTT, fqctx);
  fq_poly_clear(FLINTbuf3, fqctx);
  fq_poly_clear(FLINTbuf1, fqctx);
  fq_poly_clear(FLINTbuf2, fqctx);
  fq_ctx_clear(fqctx);
#endif

  return result;
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantineQa
CFList
diophantine (const CanonicalForm& F, const CanonicalForm& G,
             const CFList& factors, modpk& b)
{
  if (getCharacteristic() == 0)
  {
    Variable v;
    bool hasAlgVar= hasFirstAlgVar (F, v);
    for (CFListIterator i= factors; i.hasItem() && !hasAlgVar; i++)
      hasAlgVar= hasFirstAlgVar (i.getItem(), v);
    if (hasAlgVar)
    {
      if (b.getp() != 0)
      {
        CFList result= diophantineQa (F, G, factors, b, v);
        return result;
      }
      CFList result= modularDiophant (F, factors, getMipo (v));
      return result;
    }
    if (b.getp() != 0)
      return diophantineHensel (F, factors, b);
  }

  CanonicalForm buf1, buf2, buf3, S, T;
  CFListIterator i= factors;
  CFList result;
  if (i.hasItem())
    i++;
  buf1= F/factors.getFirst();
  buf2= divNTL (F, i.getItem());
  buf3= extgcd (buf1, buf2, S, T);
  result.append (S);
  result.append (T);
  if (i.hasItem())
    i++;
  for (; i.hasItem(); i++)
  {
    buf1= divNTL (F, i.getItem());
    buf3= extgcd (buf3, buf1, S, T);
    CFListIterator k= factors;
    for (CFListIterator j= result; j.hasItem(); j++, k++)
    {
      j.getItem()= mulNTL (j.getItem(), S);
      j.getItem()= modNTL (j.getItem(), k.getItem());
    }
    result.append (T);
  }
  return result;
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantineQa
CFList
diophantine (const CanonicalForm& F, const CFList& factors)
{
  modpk b= modpk();
  return diophantine (F, 1, factors, b);
}
#endif

void
henselStep12 (const CanonicalForm& F, const CFList& factors,
              CFArray& bufFactors, const CFList& diophant, CFMatrix& M,
              CFArray& Pi, int j, const modpk& b)
{
  CanonicalForm E;
  CanonicalForm xToJ= power (F.mvar(), j);
  Variable x= F.mvar();
  // compute the error
  if (j == 1)
    E= F[j];
  else
  {
    if (degree (Pi [factors.length() - 2], x) > 0)
      E= F[j] - Pi [factors.length() - 2] [j];
    else
      E= F[j];
  }

  if (b.getp() != 0)
    E= b(E);
  CFArray buf= CFArray (diophant.length());
  bufFactors[0]= mod (factors.getFirst(), power (F.mvar(), j + 1));
  int k= 0;
  CanonicalForm remainder;
  // actual lifting
  for (CFListIterator i= diophant; i.hasItem(); i++, k++)
  {
    if (degree (bufFactors[k], x) > 0)
    {
      if (k > 0)
        remainder= modNTL (E, bufFactors[k] [0], b); //TODO precompute inverses of bufFactors[k][0]
      else
        remainder= E;
    }
    else
      remainder= modNTL (E, bufFactors[k], b);

    buf[k]= mulNTL (i.getItem(), remainder, b);
    if (degree (bufFactors[k], x) > 0)
      buf[k]= modNTL (buf[k], bufFactors[k] [0], b);
    else
      buf[k]= modNTL (buf[k], bufFactors[k], b);
  }
  for (k= 1; k < factors.length(); k++)
  {
    bufFactors[k] += xToJ*buf[k];
    if (b.getp() != 0)
      bufFactors[k]= b(bufFactors[k]);
  }

  // update Pi [0]
  int degBuf0= degree (bufFactors[0], x);
  int degBuf1= degree (bufFactors[1], x);
  if (degBuf0 > 0 && degBuf1 > 0)
    M (j + 1, 1)= mulNTL (bufFactors[0] [j], bufFactors[1] [j], b);
  CanonicalForm uIZeroJ;

  if (degBuf0 > 0 && degBuf1 > 0)
    uIZeroJ= mulNTL ((bufFactors[0] [0] + bufFactors[0] [j]),
                    (bufFactors[1] [0] + buf[1]), b) - M(1, 1) - M(j + 1, 1);
  else if (degBuf0 > 0)
    uIZeroJ= mulNTL (bufFactors[0] [j], bufFactors[1], b);
  else if (degBuf1 > 0)
    uIZeroJ= mulNTL (bufFactors[0], buf[1], b);
  else
    uIZeroJ= 0;
  if (b.getp() != 0)
    uIZeroJ= b (uIZeroJ);
  Pi [0] += xToJ*uIZeroJ;
  if (b.getp() != 0)
    Pi [0]= b (Pi[0]);

  CFArray tmp= CFArray (factors.length() - 1);
  for (k= 0; k < factors.length() - 1; k++)
    tmp[k]= 0;
  CFIterator one, two;
  one= bufFactors [0];
  two= bufFactors [1];
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    for (k= 1; k <= (j+1)/2; k++)
    {
      if (k != j - k + 1)
      {
        if ((one.hasTerms() && one.exp() == j - k + 1)
             && (two.hasTerms() && two.exp() == j - k + 1))
        {
          tmp[0] += mulNTL ((bufFactors[0][k]+one.coeff()), (bufFactors[1][k]+
                     two.coeff()), b) - M (k + 1, 1) - M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.hasTerms() && one.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0][k]+one.coeff()), bufFactors[1][k], b)
                    - M (k + 1, 1);
          one++;
        }
        else if (two.hasTerms() && two.exp() == j - k + 1)
        {
          tmp[0] += mulNTL (bufFactors[0][k], (bufFactors[1][k]+two.coeff()), b)
                    - M (k + 1, 1);
          two++;
        }
      }
      else
      {
        tmp[0] += M (k + 1, 1);
      }
    }
  }
  if (b.getp() != 0)
    tmp[0]= b (tmp[0]);
  Pi [0] += tmp[0]*xToJ*F.mvar();

  // update Pi [l]
  int degPi, degBuf;
  for (int l= 1; l < factors.length() - 1; l++)
  {
    degPi= degree (Pi [l - 1], x);
    degBuf= degree (bufFactors[l + 1], x);
    if (degPi > 0 && degBuf > 0)
      M (j + 1, l + 1)= mulNTL (Pi [l - 1] [j], bufFactors[l + 1] [j], b);
    if (j == 1)
    {
      if (degPi > 0 && degBuf > 0)
        Pi [l] += xToJ*(mulNTL (Pi [l - 1] [0] + Pi [l - 1] [j],
                  bufFactors[l + 1] [0] + buf[l + 1], b) - M (j + 1, l +1) -
                  M (1, l + 1));
      else if (degPi > 0)
        Pi [l] += xToJ*(mulNTL (Pi [l - 1] [j], bufFactors[l + 1], b));
      else if (degBuf > 0)
        Pi [l] += xToJ*(mulNTL (Pi [l - 1], buf[l + 1], b));
    }
    else
    {
      if (degPi > 0 && degBuf > 0)
      {
        uIZeroJ= mulNTL (uIZeroJ, bufFactors [l + 1] [0], b);
        uIZeroJ += mulNTL (Pi [l - 1] [0], buf [l + 1], b);
      }
      else if (degPi > 0)
        uIZeroJ= mulNTL (uIZeroJ, bufFactors [l + 1], b);
      else if (degBuf > 0)
      {
        uIZeroJ= mulNTL (uIZeroJ, bufFactors [l + 1] [0], b);
        uIZeroJ += mulNTL (Pi [l - 1], buf[l + 1], b);
      }
      Pi[l] += xToJ*uIZeroJ;
    }
    one= bufFactors [l + 1];
    two= Pi [l - 1];
    if (two.hasTerms() && two.exp() == j + 1)
    {
      if (degBuf > 0 && degPi > 0)
      {
          tmp[l] += mulNTL (two.coeff(), bufFactors[l + 1][0], b);
          two++;
      }
      else if (degPi > 0)
      {
          tmp[l] += mulNTL (two.coeff(), bufFactors[l + 1], b);
          two++;
      }
    }
    if (degBuf > 0 && degPi > 0)
    {
      for (k= 1; k <= (j+1)/2; k++)
      {
        if (k != j - k + 1)
        {
          if ((one.hasTerms() && one.exp() == j - k + 1) &&
              (two.hasTerms() && two.exp() == j - k + 1))
          {
            tmp[l] += mulNTL ((bufFactors[l+1][k] + one.coeff()), (Pi[l-1][k] +
                      two.coeff()),b) - M (k + 1, l + 1) - M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.hasTerms() && one.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l+1][k]+one.coeff()), Pi[l-1][k], b) -
                       M (k + 1, l + 1);
            one++;
          }
          else if (two.hasTerms() && two.exp() == j - k + 1)
          {
            tmp[l] += mulNTL (bufFactors[l+1][k], (Pi[l-1][k] + two.coeff()), b)
                      - M (k + 1, l + 1);
            two++;
          }
        }
        else
          tmp[l] += M (k + 1, l + 1);
      }
    }
    if (b.getp() != 0)
      tmp[l]= b (tmp[l]);
    Pi[l] += tmp[l]*xToJ*F.mvar();
  }
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diopantineQa
void
henselLift12 (const CanonicalForm& F, CFList& factors, int l, CFArray& Pi,
              CFList& diophant, CFMatrix& M, modpk& b, bool sort)
{
  if (sort)
    sortList (factors, Variable (1));
  Pi= CFArray (factors.length() - 1);
  CFListIterator j= factors;
  diophant= diophantine (F[0], F, factors, b);
  CanonicalForm bufF= F;
  if (getCharacteristic() == 0 && b.getp() != 0)
  {
    Variable v;
    bool hasAlgVar= hasFirstAlgVar (F, v);
    for (CFListIterator i= factors; i.hasItem() && !hasAlgVar; i++)
      hasAlgVar= hasFirstAlgVar (i.getItem(), v);
    Variable w;
    bool hasAlgVar2= false;
    for (CFListIterator i= diophant; i.hasItem() && !hasAlgVar2; i++)
      hasAlgVar2= hasFirstAlgVar (i.getItem(), w);
    if (hasAlgVar && hasAlgVar2 && v!=w)
    {
      bufF= replacevar (bufF, v, w);
      for (CFListIterator i= factors; i.hasItem(); i++)
        i.getItem()= replacevar (i.getItem(), v, w);
    }
  }

  DEBOUTLN (cerr, "diophant= " << diophant);
  j++;
  Pi [0]= mulNTL (j.getItem(), mod (factors.getFirst(), F.mvar()), b);
  M (1, 1)= Pi [0];
  int i= 1;
  if (j.hasItem())
    j++;
  for (; j.hasItem(); j++, i++)
  {
    Pi [i]= mulNTL (Pi [i - 1], j.getItem(), b);
    M (1, i + 1)= Pi [i];
  }
  CFArray bufFactors= CFArray (factors.length());
  i= 0;
  for (CFListIterator k= factors; k.hasItem(); i++, k++)
  {
    if (i == 0)
      bufFactors[i]= mod (k.getItem(), F.mvar());
    else
      bufFactors[i]= k.getItem();
  }
  for (i= 1; i < l; i++)
    henselStep12 (bufF, factors, bufFactors, diophant, M, Pi, i, b);

  CFListIterator k= factors;
  for (i= 0; i < factors.length (); i++, k++)
    k.getItem()= bufFactors[i];
  factors.removeFirst();
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) //henselLift12
void
henselLift12 (const CanonicalForm& F, CFList& factors, int l, CFArray& Pi,
              CFList& diophant, CFMatrix& M, bool sort)
{
  modpk dummy= modpk();
  henselLift12 (F, factors, l, Pi, diophant, M, dummy, sort);
}
#endif

void
henselLiftResume12 (const CanonicalForm& F, CFList& factors, int start, int
                    end, CFArray& Pi, const CFList& diophant, CFMatrix& M,
                    const modpk& b)
{
  CFArray bufFactors= CFArray (factors.length());
  int i= 0;
  CanonicalForm xToStart= power (F.mvar(), start);
  for (CFListIterator k= factors; k.hasItem(); k++, i++)
  {
    if (i == 0)
      bufFactors[i]= mod (k.getItem(), xToStart);
    else
      bufFactors[i]= k.getItem();
  }
  for (i= start; i < end; i++)
    henselStep12 (F, factors, bufFactors, diophant, M, Pi, i, b);

  CFListIterator k= factors;
  for (i= 0; i < factors.length(); k++, i++)
    k.getItem()= bufFactors [i];
  factors.removeFirst();
  return;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantine
CFList
biDiophantine (const CanonicalForm& F, const CFList& factors, int d)
{
  Variable y= F.mvar();
  CFList result;
  if (y.level() == 1)
  {
    result= diophantine (F, factors);
    return result;
  }
  else
  {
    CFList buf= factors;
    for (CFListIterator i= buf; i.hasItem(); i++)
      i.getItem()= mod (i.getItem(), y);
    CanonicalForm A= mod (F, y);
    int bufD= 1;
    CFList recResult= biDiophantine (A, buf, bufD);
    CanonicalForm e= 1;
    CFList p;
    CFArray bufFactors= CFArray (factors.length());
    CanonicalForm yToD= power (y, d);
    int k= 0;
    for (CFListIterator i= factors; i.hasItem(); i++, k++)
    {
      bufFactors [k]= i.getItem();
    }
    CanonicalForm b, quot;
    for (k= 0; k < factors.length(); k++) //TODO compute b's faster
    {
      b= 1;
      if (fdivides (bufFactors[k], F, quot))
        b= quot;
      else
      {
        for (int l= 0; l < factors.length(); l++)
        {
          if (l == k)
            continue;
          else
          {
            b= mulMod2 (b, bufFactors[l], yToD);
          }
        }
      }
      p.append (b);
    }

    CFListIterator j= p;
    for (CFListIterator i= recResult; i.hasItem(); i++, j++)
      e -= i.getItem()*j.getItem();

    if (e.isZero())
      return recResult;
    CanonicalForm coeffE;
    CFList s;
    result= recResult;
    CanonicalForm g;
    for (int i= 1; i < d; i++)
    {
      if (degree (e, y) > 0)
        coeffE= e[i];
      else
        coeffE= 0;
      if (!coeffE.isZero())
      {
        CFListIterator k= result;
        CFListIterator l= p;
        int ii= 0;
        j= recResult;
        for (; j.hasItem(); j++, k++, l++, ii++)
        {
          g= coeffE*j.getItem();
          if (degree (bufFactors[ii], y) <= 0)
            g= mod (g, bufFactors[ii]);
          else
            g= mod (g, bufFactors[ii][0]);
          k.getItem() += g*power (y, i);
          e -= mulMod2 (g*power(y, i), l.getItem(), yToD);
          DEBOUTLN (cerr, "mod (e, power (y, i + 1))= " <<
                    mod (e, power (y, i + 1)));
        }
      }
      if (e.isZero())
        break;
    }

    DEBOUTLN (cerr, "mod (e, y)= " << mod (e, y));

#ifdef DEBUGOUTPUT
    CanonicalForm test= 0;
    j= p;
    for (CFListIterator i= result; i.hasItem(); i++, j++)
      test += mod (i.getItem()*j.getItem(), power (y, d));
    DEBOUTLN (cerr, "test= " << test);
#endif
    return result;
  }
}
#endif

CFList
multiRecDiophantine (const CanonicalForm& F, const CFList& factors,
                     const CFList& recResult, const CFList& M, int d)
{
  Variable y= F.mvar();
  CFList result;
  CFListIterator i;
  CanonicalForm e= 1;
  CFListIterator j= factors;
  CFList p;
  CFArray bufFactors= CFArray (factors.length());
  CanonicalForm yToD= power (y, d);
  int k= 0;
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
    bufFactors [k]= i.getItem();
  CanonicalForm b, quot;
  CFList buf= M;
  buf.removeLast();
  buf.append (yToD);
  for (k= 0; k < factors.length(); k++) //TODO compute b's faster
  {
    b= 1;
    if (fdivides (bufFactors[k], F, quot))
      b= quot;
    else
    {
      for (int l= 0; l < factors.length(); l++)
      {
        if (l == k)
          continue;
        else
        {
          b= mulMod (b, bufFactors[l], buf);
        }
      }
    }
    p.append (b);
  }
  j= p;
  for (CFListIterator i= recResult; i.hasItem(); i++, j++)
    e -= mulMod (i.getItem(), j.getItem(), M);

  if (e.isZero())
    return recResult;
  CanonicalForm coeffE;
  CFList s;
  result= recResult;
  CanonicalForm g;
  for (int i= 1; i < d; i++)
  {
    if (degree (e, y) > 0)
      coeffE= e[i];
    else
      coeffE= 0;
    if (!coeffE.isZero())
    {
      CFListIterator k= result;
      CFListIterator l= p;
      j= recResult;
      int ii= 0;
      CanonicalForm dummy;
      for (; j.hasItem(); j++, k++, l++, ii++)
      {
        g= mulMod (coeffE, j.getItem(), M);
        if (degree (bufFactors[ii], y) <= 0)
          divrem (g, mod (bufFactors[ii], Variable (y.level() - 1)), dummy,
                  g, M);
        else
          divrem (g, bufFactors[ii][0], dummy, g, M);
        k.getItem() += g*power (y, i);
        e -= mulMod (g*power (y, i), l.getItem(), M);
      }
    }

    if (e.isZero())
      break;
  }

#ifdef DEBUGOUTPUT
    CanonicalForm test= 0;
    j= p;
    for (CFListIterator i= result; i.hasItem(); i++, j++)
      test += mod (i.getItem()*j.getItem(), power (y, d));
    DEBOUTLN (cerr, "test in multiRecDiophantine= " << test);
#endif
  return result;
}

static
void
henselStep (const CanonicalForm& F, const CFList& factors, CFArray& bufFactors,
            const CFList& diophant, CFMatrix& M, CFArray& Pi, int j,
            const CFList& MOD)
{
  CanonicalForm E;
  CanonicalForm xToJ= power (F.mvar(), j);
  Variable x= F.mvar();
  // compute the error
  if (j == 1)
  {
    E= F[j];
#ifdef DEBUGOUTPUT
    CanonicalForm test= 1;
    for (int i= 0; i < factors.length(); i++)
    {
      if (i == 0)
        test= mulMod (test, mod (bufFactors [i], xToJ), MOD);
      else
        test= mulMod (test, bufFactors[i], MOD);
    }
    CanonicalForm test2= mod (F-test, xToJ);

    test2= mod (test2, MOD);
    DEBOUTLN (cerr, "test in henselStep= " << test2);
#endif
  }
  else
  {
#ifdef DEBUGOUTPUT
    CanonicalForm test= 1;
    for (int i= 0; i < factors.length(); i++)
    {
      if (i == 0)
        test *= mod (bufFactors [i], power (x, j));
      else
        test *= bufFactors[i];
    }
    test= mod (test, power (x, j));
    test= mod (test, MOD);
    CanonicalForm test2= mod (F, power (x, j - 1)) - mod (test, power (x, j-1));
    DEBOUTLN (cerr, "test in henselStep= " << test2);
#endif

    if (degree (Pi [factors.length() - 2], x) > 0)
      E= F[j] - Pi [factors.length() - 2] [j];
    else
      E= F[j];
  }

  CFArray buf= CFArray (diophant.length());
  bufFactors[0]= mod (factors.getFirst(), power (F.mvar(), j + 1));
  int k= 0;
  // actual lifting
  CanonicalForm dummy, rest1;
  for (CFListIterator i= diophant; i.hasItem(); i++, k++)
  {
    if (degree (bufFactors[k], x) > 0)
    {
      if (k > 0)
        divrem (E, bufFactors[k] [0], dummy, rest1, MOD);
      else
        rest1= E;
    }
    else
      divrem (E, bufFactors[k], dummy, rest1, MOD);

    buf[k]= mulMod (i.getItem(), rest1, MOD);

    if (degree (bufFactors[k], x) > 0)
      divrem (buf[k], bufFactors[k] [0], dummy, buf[k], MOD);
    else
      divrem (buf[k], bufFactors[k], dummy, buf[k], MOD);
  }
  for (k= 1; k < factors.length(); k++)
    bufFactors[k] += xToJ*buf[k];

  // update Pi [0]
  int degBuf0= degree (bufFactors[0], x);
  int degBuf1= degree (bufFactors[1], x);
  if (degBuf0 > 0 && degBuf1 > 0)
    M (j + 1, 1)= mulMod (bufFactors[0] [j], bufFactors[1] [j], MOD);
  CanonicalForm uIZeroJ;

  if (degBuf0 > 0 && degBuf1 > 0)
    uIZeroJ= mulMod ((bufFactors[0] [0] + bufFactors[0] [j]),
                  (bufFactors[1] [0] + buf[1]), MOD) - M(1, 1) - M(j + 1, 1);
  else if (degBuf0 > 0)
    uIZeroJ= mulMod (bufFactors[0] [j], bufFactors[1], MOD);
  else if (degBuf1 > 0)
    uIZeroJ= mulMod (bufFactors[0], buf[1], MOD);
  else
    uIZeroJ= 0;
  Pi [0] += xToJ*uIZeroJ;

  CFArray tmp= CFArray (factors.length() - 1);
  for (k= 0; k < factors.length() - 1; k++)
    tmp[k]= 0;
  CFIterator one, two;
  one= bufFactors [0];
  two= bufFactors [1];
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    for (k= 1; k <= (j+1)/2; k++)
    {
      if (k != j - k + 1)
      {
        if ((one.hasTerms() && one.exp() == j - k + 1) &&
            (two.hasTerms() && two.exp() == j - k + 1))
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    (bufFactors[1] [k] + two.coeff()), MOD) - M (k + 1, 1) -
                    M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.hasTerms() && one.exp() == j - k + 1)
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    bufFactors[1] [k], MOD) - M (k + 1, 1);
          one++;
        }
        else if (two.hasTerms() && two.exp() == j - k + 1)
        {
          tmp[0] += mulMod (bufFactors[0] [k], (bufFactors[1] [k] +
                    two.coeff()), MOD) - M (k + 1, 1);
          two++;
        }
      }
      else
      {
        tmp[0] += M (k + 1, 1);
      }
    }
  }
  Pi [0] += tmp[0]*xToJ*F.mvar();

  // update Pi [l]
  int degPi, degBuf;
  for (int l= 1; l < factors.length() - 1; l++)
  {
    degPi= degree (Pi [l - 1], x);
    degBuf= degree (bufFactors[l + 1], x);
    if (degPi > 0 && degBuf > 0)
      M (j + 1, l + 1)= mulMod (Pi [l - 1] [j], bufFactors[l + 1] [j], MOD);
    if (j == 1)
    {
      if (degPi > 0 && degBuf > 0)
        Pi [l] += xToJ*(mulMod ((Pi [l - 1] [0] + Pi [l - 1] [j]),
                  (bufFactors[l + 1] [0] + buf[l + 1]), MOD) - M (j + 1, l +1)-
                  M (1, l + 1));
      else if (degPi > 0)
        Pi [l] += xToJ*(mulMod (Pi [l - 1] [j], bufFactors[l + 1], MOD));
      else if (degBuf > 0)
        Pi [l] += xToJ*(mulMod (Pi [l - 1], buf[l + 1], MOD));
    }
    else
    {
      if (degPi > 0 && degBuf > 0)
      {
        uIZeroJ= mulMod (uIZeroJ, bufFactors [l + 1] [0], MOD);
        uIZeroJ += mulMod (Pi [l - 1] [0], buf [l + 1], MOD);
      }
      else if (degPi > 0)
        uIZeroJ= mulMod (uIZeroJ, bufFactors [l + 1], MOD);
      else if (degBuf > 0)
      {
        uIZeroJ= mulMod (uIZeroJ, bufFactors [l + 1] [0], MOD);
        uIZeroJ += mulMod (Pi [l - 1], buf[l + 1], MOD);
      }
      Pi[l] += xToJ*uIZeroJ;
    }
    one= bufFactors [l + 1];
    two= Pi [l - 1];
    if (two.hasTerms() && two.exp() == j + 1)
    {
      if (degBuf > 0 && degPi > 0)
      {
          tmp[l] += mulMod (two.coeff(), bufFactors[l + 1][0], MOD);
          two++;
      }
      else if (degPi > 0)
      {
          tmp[l] += mulMod (two.coeff(), bufFactors[l + 1], MOD);
          two++;
      }
    }
    if (degBuf > 0 && degPi > 0)
    {
      for (k= 1; k <= (j+1)/2; k++)
      {
        if (k != j - k + 1)
        {
          if ((one.hasTerms() && one.exp() == j - k + 1) &&
              (two.hasTerms() && two.exp() == j - k + 1))
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      (Pi[l - 1] [k] + two.coeff()), MOD) - M (k + 1, l + 1) -
                      M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.hasTerms() && one.exp() == j - k + 1)
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      Pi[l - 1] [k], MOD) - M (k + 1, l + 1);
            one++;
          }
          else if (two.hasTerms() && two.exp() == j - k + 1)
          {
            tmp[l] += mulMod (bufFactors[l + 1] [k],
                      (Pi[l - 1] [k] + two.coeff()), MOD) - M (k + 1, l + 1);
            two++;
          }
        }
        else
          tmp[l] += M (k + 1, l + 1);
      }
    }
    Pi[l] += tmp[l]*xToJ*F.mvar();
  }

  return;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // biDiophantine
CFList
henselLift23 (const CFList& eval, const CFList& factors, int* l, CFList&
              diophant, CFArray& Pi, CFMatrix& M)
{
  CFList buf= factors;
  int k= 0;
  int liftBoundBivar= l[k];
  diophant= biDiophantine (eval.getFirst(), buf, liftBoundBivar);
  CFList MOD;
  MOD.append (power (Variable (2), liftBoundBivar));
  CFArray bufFactors= CFArray (factors.length());
  k= 0;
  CFListIterator j= eval;
  j++;
  buf.removeFirst();
  buf.insert (LC (j.getItem(), 1));
  for (CFListIterator i= buf; i.hasItem(); i++, k++)
    bufFactors[k]= i.getItem();
  Pi= CFArray (factors.length() - 1);
  CFListIterator i= buf;
  i++;
  Variable y= j.getItem().mvar();
  Pi [0]= mulMod (i.getItem(), mod (buf.getFirst(), y), MOD);
  M (1, 1)= Pi [0];
  k= 1;
  if (i.hasItem())
    i++;
  for (; i.hasItem(); i++, k++)
  {
    Pi [k]= mulMod (Pi [k - 1], i.getItem(), MOD);
    M (1, k + 1)= Pi [k];
  }

  for (int d= 1; d < l[1]; d++)
    henselStep (j.getItem(), buf, bufFactors, diophant, M, Pi, d, MOD);
  CFList result;
  for (k= 1; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}
#endif

void
henselLiftResume (const CanonicalForm& F, CFList& factors, int start, int end,
                  CFArray& Pi, const CFList& diophant, CFMatrix& M,
                  const CFList& MOD)
{
  CFArray bufFactors= CFArray (factors.length());
  int i= 0;
  CanonicalForm xToStart= power (F.mvar(), start);
  for (CFListIterator k= factors; k.hasItem(); k++, i++)
  {
    if (i == 0)
      bufFactors[i]= mod (k.getItem(), xToStart);
    else
      bufFactors[i]= k.getItem();
  }
  for (i= start; i < end; i++)
    henselStep (F, factors, bufFactors, diophant, M, Pi, i, MOD);

  CFListIterator k= factors;
  for (i= 0; i < factors.length(); k++, i++)
    k.getItem()= bufFactors [i];
  factors.removeFirst();
  return;
}

CFList
henselLift (const CFList& F, const CFList& factors, const CFList& MOD, CFList&
            diophant, CFArray& Pi, CFMatrix& M, int lOld, int lNew)
{
  diophant= multiRecDiophantine (F.getFirst(), factors, diophant, MOD, lOld);
  int k= 0;
  CFArray bufFactors= CFArray (factors.length());
  for (CFListIterator i= factors; i.hasItem(); i++, k++)
  {
    if (k == 0)
      bufFactors[k]= LC (F.getLast(), 1);
    else
      bufFactors[k]= i.getItem();
  }
  CFList buf= factors;
  buf.removeFirst();
  buf.insert (LC (F.getLast(), 1));
  CFListIterator i= buf;
  i++;
  Variable y= F.getLast().mvar();
  Variable x= F.getFirst().mvar();
  CanonicalForm xToLOld= power (x, lOld);
  Pi [0]= mod (Pi[0], xToLOld);
  M (1, 1)= Pi [0];
  k= 1;
  if (i.hasItem())
    i++;
  for (; i.hasItem(); i++, k++)
  {
    Pi [k]= mod (Pi [k], xToLOld);
    M (1, k + 1)= Pi [k];
  }

  for (int d= 1; d < lNew; d++)
    henselStep (F.getLast(), buf, bufFactors, diophant, M, Pi, d, MOD);
  CFList result;
  for (k= 1; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // henselLift23
CFList
henselLift (const CFList& eval, const CFList& factors, int* l, int lLength,
            bool sort)
{
  CFList diophant;
  CFList buf= factors;
  buf.insert (LC (eval.getFirst(), 1));
  if (sort)
    sortList (buf, Variable (1));
  CFArray Pi;
  CFMatrix M= CFMatrix (l[1], factors.length());
  CFList result= henselLift23 (eval, buf, l, diophant, Pi, M);
  if (eval.length() == 2)
    return result;
  CFList MOD;
  for (int i= 0; i < 2; i++)
    MOD.append (power (Variable (i + 2), l[i]));
  CFListIterator j= eval;
  j++;
  CFList bufEval;
  bufEval.append (j.getItem());
  j++;

  for (int i= 2; i < lLength && j.hasItem(); i++, j++)
  {
    result.insert (LC (bufEval.getFirst(), 1));
    bufEval.append (j.getItem());
    M= CFMatrix (l[i], factors.length());
    result= henselLift (bufEval, result, MOD, diophant, Pi, M, l[i - 1], l[i]);
    MOD.append (power (Variable (i + 2), l[i]));
    bufEval.removeFirst();
  }
  return result;
}
#endif

// nonmonic

void
nonMonicHenselStep12 (const CanonicalForm& F, const CFList& factors,
                      CFArray& bufFactors, const CFList& diophant, CFMatrix& M,
                      CFArray& Pi, int j, const CFArray& /*LCs*/)
{
  Variable x= F.mvar();
  CanonicalForm xToJ= power (x, j);

  CanonicalForm E;
  // compute the error
  if (degree (Pi [factors.length() - 2], x) > 0)
    E= F[j] - Pi [factors.length() - 2] [j];
  else
    E= F[j];

  CFArray buf= CFArray (diophant.length());

  int k= 0;
  CanonicalForm remainder;
  // actual lifting
  for (CFListIterator i= diophant; i.hasItem(); i++, k++)
  {
    if (degree (bufFactors[k], x) > 0)
      remainder= modNTL (E, bufFactors[k] [0]);
    else
      remainder= modNTL (E, bufFactors[k]);
    buf[k]= mulNTL (i.getItem(), remainder);
    if (degree (bufFactors[k], x) > 0)
      buf[k]= modNTL (buf[k], bufFactors[k] [0]);
    else
      buf[k]= modNTL (buf[k], bufFactors[k]);
  }

  for (k= 0; k < factors.length(); k++)
    bufFactors[k] += xToJ*buf[k];

  // update Pi [0]
  int degBuf0= degree (bufFactors[0], x);
  int degBuf1= degree (bufFactors[1], x);
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    M (j + 1, 1)= mulNTL (bufFactors[0] [j], bufFactors[1] [j]);
    if (j + 2 <= M.rows())
      M (j + 2, 1)= mulNTL (bufFactors[0] [j + 1], bufFactors[1] [j + 1]);
  }
  else
    M (j + 1, 1)= 0;

  CanonicalForm uIZeroJ;
  if (degBuf0 > 0 && degBuf1 > 0)
    uIZeroJ= mulNTL(bufFactors[0][0], buf[1]) +
             mulNTL (bufFactors[1][0], buf[0]);
  else if (degBuf0 > 0)
    uIZeroJ= mulNTL (buf[0], bufFactors[1]) +
             mulNTL (buf[1], bufFactors[0][0]);
  else if (degBuf1 > 0)
    uIZeroJ= mulNTL (bufFactors[0], buf[1]) +
             mulNTL (buf[0], bufFactors[1][0]);
  else
    uIZeroJ= mulNTL (bufFactors[0], buf[1]) +
             mulNTL (buf[0], bufFactors[1]);

  Pi [0] += xToJ*uIZeroJ;

  CFArray tmp= CFArray (factors.length() - 1);
  for (k= 0; k < factors.length() - 1; k++)
    tmp[k]= 0;
  CFIterator one, two;
  one= bufFactors [0];
  two= bufFactors [1];
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    while (one.hasTerms() && one.exp() > j) one++;
    while (two.hasTerms() && two.exp() > j) two++;
    for (k= 1; k <= (j+1)/2; k++)
    {
      if (k != j - k + 1)
      {
        if ((one.hasTerms() && one.exp() == j - k + 1) && +
            (two.hasTerms() && two.exp() == j - k + 1))
        {
          tmp[0] += mulNTL ((bufFactors[0][k]+one.coeff()),(bufFactors[1][k] +
                    two.coeff())) - M (k + 1, 1) - M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.hasTerms() && one.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0][k]+one.coeff()), bufFactors[1] [k]) -
                    M (k + 1, 1);
          one++;
        }
        else if (two.hasTerms() && two.exp() == j - k + 1)
        {
          tmp[0] += mulNTL (bufFactors[0][k],(bufFactors[1][k] + two.coeff())) -
                    M (k + 1, 1);
          two++;
        }
      }
      else
        tmp[0] += M (k + 1, 1);
    }
  }

  if (degBuf0 >= j + 1 && degBuf1 >= j + 1)
  {
    if (j + 2 <= M.rows())
      tmp [0] += mulNTL ((bufFactors [0] [j + 1]+ bufFactors [0] [0]),
                         (bufFactors [1] [j + 1] + bufFactors [1] [0]))
                         - M(1,1) - M (j + 2,1);
  }
  else if (degBuf0 >= j + 1)
  {
    if (degBuf1 > 0)
      tmp[0] += mulNTL (bufFactors [0] [j+1], bufFactors [1] [0]);
    else
      tmp[0] += mulNTL (bufFactors [0] [j+1], bufFactors [1]);
  }
  else if (degBuf1 >= j + 1)
  {
    if (degBuf0 > 0)
      tmp[0] += mulNTL (bufFactors [0] [0], bufFactors [1] [j + 1]);
    else
      tmp[0] += mulNTL (bufFactors [0], bufFactors [1] [j + 1]);
  }

  Pi [0] += tmp[0]*xToJ*F.mvar();

  int degPi, degBuf;
  for (int l= 1; l < factors.length() - 1; l++)
  {
    degPi= degree (Pi [l - 1], x);
    degBuf= degree (bufFactors[l + 1], x);
    if (degPi > 0 && degBuf > 0)
    {
      M (j + 1, l + 1)= mulNTL (Pi [l - 1] [j], bufFactors[l + 1] [j]);
      if (j + 2 <= M.rows())
        M (j + 2, l + 1)= mulNTL (Pi [l - 1][j + 1], bufFactors[l + 1] [j + 1]);
    }
    else
      M (j + 1, l + 1)= 0;

    if (degPi > 0 && degBuf > 0)
      uIZeroJ= mulNTL (Pi[l - 1] [0], buf[l + 1]) +
               mulNTL (uIZeroJ, bufFactors[l+1] [0]);
    else if (degPi > 0)
      uIZeroJ= mulNTL (uIZeroJ, bufFactors[l + 1]) +
               mulNTL (Pi[l - 1][0], buf[l + 1]);
    else if (degBuf > 0)
      uIZeroJ= mulNTL (uIZeroJ, bufFactors[l + 1][0]) +
               mulNTL (Pi[l - 1], buf[l + 1]);
    else
      uIZeroJ= mulNTL (uIZeroJ, bufFactors[l + 1]) +
               mulNTL (Pi[l - 1], buf[l + 1]);

    Pi [l] += xToJ*uIZeroJ;

    one= bufFactors [l + 1];
    two= Pi [l - 1];
    if (degBuf > 0 && degPi > 0)
    {
      while (one.hasTerms() && one.exp() > j) one++;
      while (two.hasTerms() && two.exp() > j) two++;
      for (k= 1; k <= (j+1)/2; k++)
      {
        if (k != j - k + 1)
        {
          if ((one.hasTerms() && one.exp() == j - k + 1) &&
              (two.hasTerms() && two.exp() == j - k + 1))
          {
            tmp[l] += mulNTL ((bufFactors[l + 1] [k] + one.coeff()),
                      (Pi[l - 1] [k] + two.coeff())) - M (k + 1, l + 1) -
                      M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.hasTerms() && one.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l + 1] [k] + one.coeff()),
                               Pi[l - 1] [k]) - M (k + 1, l + 1);
            one++;
          }
          else if (two.hasTerms() && two.exp() == j - k + 1)
          {
            tmp[l] += mulNTL (bufFactors[l + 1] [k],
                      (Pi[l - 1] [k] + two.coeff())) - M (k + 1, l + 1);
            two++;
           }
        }
        else
          tmp[l] += M (k + 1, l + 1);
      }
    }

    if (degPi >= j + 1 && degBuf >= j + 1)
    {
      if (j + 2 <= M.rows())
        tmp [l] += mulNTL ((Pi [l - 1] [j + 1]+ Pi [l - 1] [0]),
                           (bufFactors [l + 1] [j + 1] + bufFactors [l + 1] [0])
                          ) - M(1,l+1) - M (j + 2,l+1);
    }
    else if (degPi >= j + 1)
    {
      if (degBuf > 0)
        tmp[l] += mulNTL (Pi [l - 1] [j+1], bufFactors [l + 1] [0]);
      else
        tmp[l] += mulNTL (Pi [l - 1] [j+1], bufFactors [l + 1]);
    }
    else if (degBuf >= j + 1)
    {
      if (degPi > 0)
        tmp[l] += mulNTL (Pi [l - 1] [0], bufFactors [l + 1] [j + 1]);
      else
        tmp[l] += mulNTL (Pi [l - 1], bufFactors [l + 1] [j + 1]);
    }

    Pi[l] += tmp[l]*xToJ*F.mvar();
  }
  return;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantine
void
nonMonicHenselLift12 (const CanonicalForm& F, CFList& factors, int l,
                      CFArray& Pi, CFList& diophant, CFMatrix& M,
                      const CFArray& LCs, bool sort)
{
  if (sort)
    sortList (factors, Variable (1));
  Pi= CFArray (factors.length() - 2);
  CFList bufFactors2= factors;
  bufFactors2.removeFirst();
  diophant= diophantine (F[0], bufFactors2);
  DEBOUTLN (cerr, "diophant= " << diophant);

  CFArray bufFactors= CFArray (bufFactors2.length());
  int i= 0;
  for (CFListIterator k= bufFactors2; k.hasItem(); i++, k++)
    bufFactors[i]= replaceLc (k.getItem(), LCs [i]);

  Variable x= F.mvar();
  if (degree (bufFactors[0], x) > 0 && degree (bufFactors [1], x) > 0)
  {
    M (1, 1)= mulNTL (bufFactors [0] [0], bufFactors[1] [0]);
    Pi [0]= M (1, 1) + (mulNTL (bufFactors [0] [1], bufFactors[1] [0]) +
                        mulNTL (bufFactors [0] [0], bufFactors [1] [1]))*x;
  }
  else if (degree (bufFactors[0], x) > 0)
  {
    M (1, 1)= mulNTL (bufFactors [0] [0], bufFactors[1]);
    Pi [0]= M (1, 1) +
            mulNTL (bufFactors [0] [1], bufFactors[1])*x;
  }
  else if (degree (bufFactors[1], x) > 0)
  {
    M (1, 1)= mulNTL (bufFactors [0], bufFactors[1] [0]);
    Pi [0]= M (1, 1) +
            mulNTL (bufFactors [0], bufFactors[1] [1])*x;
  }
  else
  {
    M (1, 1)= mulNTL (bufFactors [0], bufFactors[1]);
    Pi [0]= M (1, 1);
  }

  for (i= 1; i < Pi.size(); i++)
  {
    if (degree (Pi[i-1], x) > 0 && degree (bufFactors [i+1], x) > 0)
    {
      M (1,i+1)= mulNTL (Pi[i-1] [0], bufFactors[i+1] [0]);
      Pi [i]= M (1,i+1) + (mulNTL (Pi[i-1] [1], bufFactors[i+1] [0]) +
                       mulNTL (Pi[i-1] [0], bufFactors [i+1] [1]))*x;
    }
    else if (degree (Pi[i-1], x) > 0)
    {
      M (1,i+1)= mulNTL (Pi[i-1] [0], bufFactors [i+1]);
      Pi [i]=  M(1,i+1) + mulNTL (Pi[i-1] [1], bufFactors[i+1])*x;
    }
    else if (degree (bufFactors[i+1], x) > 0)
    {
      M (1,i+1)= mulNTL (Pi[i-1], bufFactors [i+1] [0]);
      Pi [i]= M (1,i+1) + mulNTL (Pi[i-1], bufFactors[i+1] [1])*x;
    }
    else
    {
      M (1,i+1)= mulNTL (Pi [i-1], bufFactors [i+1]);
      Pi [i]= M (1,i+1);
    }
  }

  for (i= 1; i < l; i++)
    nonMonicHenselStep12 (F, bufFactors2, bufFactors, diophant, M, Pi, i, LCs);

  factors= CFList();
  for (i= 0; i < bufFactors.size(); i++)
    factors.append (bufFactors[i]);
  return;
}
#endif

/// solve \f$ E=\sum_{i= 1}^r{\sigma_{i}\prod_{j=1, j\neq i}^{r}{f_{j}}} \f$
/// mod M, @a products contains \f$ \prod_{j=1, j\neq i}^{r}{f_{j}} \f$
CFList
diophantine (const CFList& recResult, const CFList& factors,
             const CFList& products, const CFList& M, const CanonicalForm& E,
             bool& bad)
{
  if (M.isEmpty())
  {
    CFList result;
    CFListIterator j= factors;
    CanonicalForm buf;
    for (CFListIterator i= recResult; i.hasItem(); i++, j++)
    {
      ASSERT (E.isUnivariate() || E.inCoeffDomain(),
              "constant or univariate poly expected");
      ASSERT (i.getItem().isUnivariate() || i.getItem().inCoeffDomain(),
              "constant or univariate poly expected");
      ASSERT (j.getItem().isUnivariate() || j.getItem().inCoeffDomain(),
              "constant or univariate poly expected");
      buf= mulNTL (E, i.getItem());
      result.append (modNTL (buf, j.getItem()));
    }
    return result;
  }
  Variable y= M.getLast().mvar();
  CFList bufFactors= factors;
  for (CFListIterator i= bufFactors; i.hasItem(); i++)
    i.getItem()= mod (i.getItem(), y);
  CFList bufProducts= products;
  for (CFListIterator i= bufProducts; i.hasItem(); i++)
    i.getItem()= mod (i.getItem(), y);
  CFList buf= M;
  buf.removeLast();
  CanonicalForm bufE= mod (E, y);
  CFList recDiophantine= diophantine (recResult, bufFactors, bufProducts, buf,
                                      bufE, bad);

  if (bad)
    return CFList();

  CanonicalForm e= E;
  CFListIterator j= products;
  for (CFListIterator i= recDiophantine; i.hasItem(); i++, j++)
    e -= j.getItem()*i.getItem();

  CFList result= recDiophantine;
  int d= degree (M.getLast());
  CanonicalForm coeffE;
  for (int i= 1; i < d; i++)
  {
    if (degree (e, y) > 0)
      coeffE= e[i];
    else
      coeffE= 0;
    if (!coeffE.isZero())
    {
      CFListIterator k= result;
      recDiophantine= diophantine (recResult, bufFactors, bufProducts, buf,
                                   coeffE, bad);
      if (bad)
        return CFList();
      CFListIterator l= products;
      for (j= recDiophantine; j.hasItem(); j++, k++, l++)
      {
        k.getItem() += j.getItem()*power (y, i);
        e -= l.getItem()*(j.getItem()*power (y, i));
      }
    }
    if (e.isZero())
      break;
  }
  if (!e.isZero())
  {
    bad= true;
    return CFList();
  }
  return result;
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantine
void
nonMonicHenselStep (const CanonicalForm& F, const CFList& factors,
                    CFArray& bufFactors, const CFList& diophant, CFMatrix& M,
                    CFArray& Pi, const CFList& products, int j,
                    const CFList& MOD, bool& noOneToOne)
{
  CanonicalForm E;
  CanonicalForm xToJ= power (F.mvar(), j);
  Variable x= F.mvar();

  // compute the error
#ifdef DEBUGOUTPUT
    CanonicalForm test= 1;
    for (int i= 0; i < factors.length(); i++)
    {
      if (i == 0)
        test *= mod (bufFactors [i], power (x, j));
      else
        test *= bufFactors[i];
    }
    test= mod (test, power (x, j));
    test= mod (test, MOD);
    CanonicalForm test2= mod (F, power (x, j - 1)) - mod (test, power (x, j-1));
    DEBOUTLN (cerr, "test in nonMonicHenselStep= " << test2);
#endif

  if (degree (Pi [factors.length() - 2], x) > 0)
    E= F[j] - Pi [factors.length() - 2] [j];
  else
    E= F[j];

  CFArray buf= CFArray (diophant.length());

  // actual lifting
  TIMING_START (diotime);
  CFList diophantine2= diophantine (diophant, factors, products, MOD, E,
                                    noOneToOne);

  if (noOneToOne)
    return;

  int k= 0;
  for (CFListIterator i= diophantine2; k < factors.length(); k++, i++)
  {
    buf[k]= i.getItem();
    bufFactors[k] += xToJ*i.getItem();
  }
  TIMING_END_AND_PRINT (diotime, "time for dio: ");

  // update Pi [0]
  TIMING_START (product2);
  int degBuf0= degree (bufFactors[0], x);
  int degBuf1= degree (bufFactors[1], x);
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    M (j + 1, 1)= mulMod (bufFactors[0] [j], bufFactors[1] [j], MOD);
    if (j + 2 <= M.rows())
      M (j + 2, 1)= mulMod (bufFactors[0] [j + 1], bufFactors[1] [j + 1], MOD);
  }
  else
    M (j + 1, 1)= 0;

  CanonicalForm uIZeroJ;
  if (degBuf0 > 0 && degBuf1 > 0)
    uIZeroJ= mulMod (bufFactors[0] [0], buf[1], MOD) +
             mulMod (bufFactors[1] [0], buf[0], MOD);
  else if (degBuf0 > 0)
    uIZeroJ= mulMod (buf[0], bufFactors[1], MOD) +
             mulMod (buf[1], bufFactors[0][0], MOD);
  else if (degBuf1 > 0)
    uIZeroJ= mulMod (bufFactors[0], buf[1], MOD) +
             mulMod (buf[0], bufFactors[1][0], MOD);
  else
    uIZeroJ= mulMod (bufFactors[0], buf[1], MOD) +
             mulMod (buf[0], bufFactors[1], MOD);
  Pi [0] += xToJ*uIZeroJ;

  CFArray tmp= CFArray (factors.length() - 1);
  for (k= 0; k < factors.length() - 1; k++)
    tmp[k]= 0;
  CFIterator one, two;
  one= bufFactors [0];
  two= bufFactors [1];
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    while (one.hasTerms() && one.exp() > j) one++;
    while (two.hasTerms() && two.exp() > j) two++;
    for (k= 1; k <= (j+1)/2; k++)
    {
      if (k != j - k + 1)
      {
        if ((one.hasTerms() && one.exp() == j - k + 1) &&
            (two.hasTerms() && two.exp() == j - k + 1))
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    (bufFactors[1] [k] + two.coeff()), MOD) - M (k + 1, 1) -
                    M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.hasTerms() && one.exp() == j - k + 1)
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    bufFactors[1] [k], MOD) - M (k + 1, 1);
          one++;
        }
        else if (two.hasTerms() && two.exp() == j - k + 1)
        {
          tmp[0] += mulMod (bufFactors[0] [k], (bufFactors[1] [k] +
                    two.coeff()), MOD) - M (k + 1, 1);
          two++;
        }
      }
      else
      {
        tmp[0] += M (k + 1, 1);
      }
    }
  }

  if (degBuf0 >= j + 1 && degBuf1 >= j + 1)
  {
    if (j + 2 <= M.rows())
      tmp [0] += mulMod ((bufFactors [0] [j + 1]+ bufFactors [0] [0]),
                         (bufFactors [1] [j + 1] + bufFactors [1] [0]), MOD)
                         - M(1,1) - M (j + 2,1);
  }
  else if (degBuf0 >= j + 1)
  {
    if (degBuf1 > 0)
      tmp[0] += mulMod (bufFactors [0] [j+1], bufFactors [1] [0], MOD);
    else
      tmp[0] += mulMod (bufFactors [0] [j+1], bufFactors [1], MOD);
  }
  else if (degBuf1 >= j + 1)
  {
    if (degBuf0 > 0)
      tmp[0] += mulMod (bufFactors [0] [0], bufFactors [1] [j + 1], MOD);
    else
      tmp[0] += mulMod (bufFactors [0], bufFactors [1] [j + 1], MOD);
  }
  Pi [0] += tmp[0]*xToJ*F.mvar();

  // update Pi [l]
  int degPi, degBuf;
  for (int l= 1; l < factors.length() - 1; l++)
  {
    degPi= degree (Pi [l - 1], x);
    degBuf= degree (bufFactors[l + 1], x);
    if (degPi > 0 && degBuf > 0)
    {
      M (j + 1, l + 1)= mulMod (Pi [l - 1] [j], bufFactors[l + 1] [j], MOD);
      if (j + 2 <= M.rows())
        M (j + 2, l + 1)= mulMod (Pi [l - 1] [j + 1], bufFactors[l + 1] [j + 1],
                                  MOD);
    }
    else
      M (j + 1, l + 1)= 0;

    if (degPi > 0 && degBuf > 0)
      uIZeroJ= mulMod (Pi[l - 1] [0], buf[l + 1], MOD) +
               mulMod (uIZeroJ, bufFactors[l + 1] [0], MOD);
    else if (degPi > 0)
      uIZeroJ= mulMod (uIZeroJ, bufFactors[l + 1], MOD) +
               mulMod (Pi[l - 1][0], buf[l + 1], MOD);
    else if (degBuf > 0)
      uIZeroJ= mulMod (Pi[l - 1], buf[l + 1], MOD) +
               mulMod (uIZeroJ, bufFactors[l + 1][0], MOD);
    else
      uIZeroJ= mulMod (Pi[l - 1], buf[l + 1], MOD) +
               mulMod (uIZeroJ, bufFactors[l + 1], MOD);

    Pi [l] += xToJ*uIZeroJ;

    one= bufFactors [l + 1];
    two= Pi [l - 1];
    if (degBuf > 0 && degPi > 0)
    {
      while (one.hasTerms() && one.exp() > j) one++;
      while (two.hasTerms() && two.exp() > j) two++;
      for (k= 1; k <= (j+1)/2; k++)
      {
        if (k != j - k + 1)
        {
          if ((one.hasTerms() && one.exp() == j - k + 1) &&
              (two.hasTerms() && two.exp() == j - k + 1))
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      (Pi[l - 1] [k] + two.coeff()), MOD) - M (k + 1, l + 1) -
                      M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.hasTerms() && one.exp() == j - k + 1)
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      Pi[l - 1] [k], MOD) - M (k + 1, l + 1);
            one++;
          }
          else if (two.hasTerms() && two.exp() == j - k + 1)
          {
            tmp[l] += mulMod (bufFactors[l + 1] [k],
                      (Pi[l - 1] [k] + two.coeff()), MOD) - M (k + 1, l + 1);
            two++;
           }
        }
        else
          tmp[l] += M (k + 1, l + 1);
      }
    }

    if (degPi >= j + 1 && degBuf >= j + 1)
    {
      if (j + 2 <= M.rows())
        tmp [l] += mulMod ((Pi [l - 1] [j + 1]+ Pi [l - 1] [0]),
                           (bufFactors [l + 1] [j + 1] + bufFactors [l + 1] [0])
                            , MOD) - M(1,l+1) - M (j + 2,l+1);
    }
    else if (degPi >= j + 1)
    {
      if (degBuf > 0)
        tmp[l] += mulMod (Pi [l - 1] [j+1], bufFactors [l + 1] [0], MOD);
      else
        tmp[l] += mulMod (Pi [l - 1] [j+1], bufFactors [l + 1], MOD);
    }
    else if (degBuf >= j + 1)
    {
      if (degPi > 0)
        tmp[l] += mulMod (Pi [l - 1] [0], bufFactors [l + 1] [j + 1], MOD);
      else
        tmp[l] += mulMod (Pi [l - 1], bufFactors [l + 1] [j + 1], MOD);
    }

    Pi[l] += tmp[l]*xToJ*F.mvar();
  }
  TIMING_END_AND_PRINT (product2, "time for product in hensel step: ");
  return;
}
#endif

// wrt. Variable (1)
CanonicalForm replaceLC (const CanonicalForm& F, const CanonicalForm& c)
{
  if (degree (F, 1) <= 0)
   return c;
  else
  {
    CanonicalForm result= swapvar (F, Variable (F.level() + 1), Variable (1));
    result += (swapvar (c, Variable (F.level() + 1), Variable (1))
              - LC (result))*power (result.mvar(), degree (result));
    return swapvar (result, Variable (F.level() + 1), Variable (1));
  }
}

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // nonMonicHenselStep
CFList
nonMonicHenselLift232(const CFList& eval, const CFList& factors, int* l, CFList&
                      diophant, CFArray& Pi, CFMatrix& M, const CFList& LCs1,
                      const CFList& LCs2, bool& bad)
{
  CFList buf= factors;
  int k= 0;
  int liftBoundBivar= l[k];
  CFList bufbuf= factors;
  Variable v= Variable (2);

  CFList MOD;
  MOD.append (power (Variable (2), liftBoundBivar));
  CFArray bufFactors= CFArray (factors.length());
  k= 0;
  CFListIterator j= eval;
  j++;
  CFListIterator iter1= LCs1;
  CFListIterator iter2= LCs2;
  iter1++;
  iter2++;
  bufFactors[0]= replaceLC (buf.getFirst(), iter1.getItem());
  bufFactors[1]= replaceLC (buf.getLast(), iter2.getItem());

  CFListIterator i= buf;
  i++;
  Variable y= j.getItem().mvar();
  if (y.level() != 3)
    y= Variable (3);

  Pi[0]= mod (Pi[0], power (v, liftBoundBivar));
  M (1, 1)= Pi[0];
  if (degree (bufFactors[0], y) > 0 && degree (bufFactors [1], y) > 0)
    Pi [0] += (mulMod (bufFactors [0] [1], bufFactors[1] [0], MOD) +
                        mulMod (bufFactors [0] [0], bufFactors [1] [1], MOD))*y;
  else if (degree (bufFactors[0], y) > 0)
    Pi [0] += mulMod (bufFactors [0] [1], bufFactors[1], MOD)*y;
  else if (degree (bufFactors[1], y) > 0)
    Pi [0] += mulMod (bufFactors [0], bufFactors[1] [1], MOD)*y;

  CFList products;
  for (int i= 0; i < bufFactors.size(); i++)
  {
    if (degree (bufFactors[i], y) > 0)
      products.append (eval.getFirst()/bufFactors[i] [0]);
    else
      products.append (eval.getFirst()/bufFactors[i]);
  }

  for (int d= 1; d < l[1]; d++)
  {
    nonMonicHenselStep (j.getItem(), buf, bufFactors, diophant, M, Pi, products,
                        d, MOD, bad);
    if (bad)
      return CFList();
  }
  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // nonMonicHenselStep
CFList
nonMonicHenselLift2 (const CFList& F, const CFList& factors, const CFList& MOD,
                    CFList& diophant, CFArray& Pi, CFMatrix& M, int lOld,
                    int& lNew, const CFList& LCs1, const CFList& LCs2, bool& bad
                   )
{
  int k= 0;
  CFArray bufFactors= CFArray (factors.length());
  bufFactors[0]= replaceLC (factors.getFirst(), LCs1.getLast());
  bufFactors[1]= replaceLC (factors.getLast(), LCs2.getLast());
  CFList buf= factors;
  Variable y= F.getLast().mvar();
  Variable x= F.getFirst().mvar();
  CanonicalForm xToLOld= power (x, lOld);
  Pi [0]= mod (Pi[0], xToLOld);
  M (1, 1)= Pi [0];

  if (degree (bufFactors[0], y) > 0 && degree (bufFactors [1], y) > 0)
    Pi [0] += (mulMod (bufFactors [0] [1], bufFactors[1] [0], MOD) +
                        mulMod (bufFactors [0] [0], bufFactors [1] [1], MOD))*y;
  else if (degree (bufFactors[0], y) > 0)
    Pi [0] += mulMod (bufFactors [0] [1], bufFactors[1], MOD)*y;
  else if (degree (bufFactors[1], y) > 0)
    Pi [0] += mulMod (bufFactors [0], bufFactors[1] [1], MOD)*y;

  CFList products;
  CanonicalForm quot;
  for (int i= 0; i < bufFactors.size(); i++)
  {
    if (degree (bufFactors[i], y) > 0)
    {
      if (!fdivides (bufFactors[i] [0], F.getFirst(), quot))
      {
        bad= true;
        return CFList();
      }
      products.append (quot);
    }
    else
    {
      if (!fdivides (bufFactors[i], F.getFirst(), quot))
      {
        bad= true;
        return CFList();
      }
      products.append (quot);
    }
  }

  for (int d= 1; d < lNew; d++)
  {
    nonMonicHenselStep (F.getLast(), buf, bufFactors, diophant, M, Pi, products,
                        d, MOD, bad);
    if (bad)
      return CFList();
  }

  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // nonMonicHenselStep
CFList
nonMonicHenselLift2 (const CFList& eval, const CFList& factors, int* l, int
                    lLength, bool sort, const CFList& LCs1, const CFList& LCs2,
                    const CFArray& Pi, const CFList& diophant, bool& bad)
{
  CFList bufDiophant= diophant;
  CFList buf= factors;
  if (sort)
    sortList (buf, Variable (1));
  CFArray bufPi= Pi;
  CFMatrix M= CFMatrix (l[1], factors.length());
  CFList result=
    nonMonicHenselLift232(eval, buf, l, bufDiophant, bufPi, M, LCs1, LCs2, bad);
  if (bad)
    return CFList();

  if (eval.length() == 2)
    return result;
  CFList MOD;
  for (int i= 0; i < 2; i++)
    MOD.append (power (Variable (i + 2), l[i]));
  CFListIterator j= eval;
  j++;
  CFList bufEval;
  bufEval.append (j.getItem());
  j++;
  CFListIterator jj= LCs1;
  CFListIterator jjj= LCs2;
  CFList bufLCs1, bufLCs2;
  jj++, jjj++;
  bufLCs1.append (jj.getItem());
  bufLCs2.append (jjj.getItem());
  jj++, jjj++;

  for (int i= 2; i < lLength && j.hasItem(); i++, j++, jj++, jjj++)
  {
    bufEval.append (j.getItem());
    bufLCs1.append (jj.getItem());
    bufLCs2.append (jjj.getItem());
    M= CFMatrix (l[i], factors.length());
    result= nonMonicHenselLift2 (bufEval, result, MOD, bufDiophant, bufPi, M,
                                 l[i - 1], l[i], bufLCs1, bufLCs2, bad);
    if (bad)
      return CFList();
    MOD.append (power (Variable (i + 2), l[i]));
    bufEval.removeFirst();
    bufLCs1.removeFirst();
    bufLCs2.removeFirst();
  }
  return result;
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // diophantine
CFList
nonMonicHenselLift23 (const CanonicalForm& F, const CFList& factors, const
                      CFList& LCs, CFList& diophant, CFArray& Pi, int liftBound,
                      int bivarLiftBound, bool& bad)
{
  CFList bufFactors2= factors;

  Variable y= Variable (2);
  for (CFListIterator i= bufFactors2; i.hasItem(); i++)
    i.getItem()= mod (i.getItem(), y);

  CanonicalForm bufF= F;
  bufF= mod (bufF, y);
  bufF= mod (bufF, Variable (3));

  TIMING_START (diotime);
  diophant= diophantine (bufF, bufFactors2);
  TIMING_END_AND_PRINT (diotime, "time for dio in 23: ");

  CFMatrix M= CFMatrix (liftBound, bufFactors2.length() - 1);

  Pi= CFArray (bufFactors2.length() - 1);

  CFArray bufFactors= CFArray (bufFactors2.length());
  CFListIterator j= LCs;
  int i= 0;
  for (CFListIterator k= factors; k.hasItem(); j++, k++, i++)
    bufFactors[i]= replaceLC (k.getItem(), j.getItem());

  //initialise Pi
  Variable v= Variable (3);
  CanonicalForm yToL= power (y, bivarLiftBound);
  if (degree (bufFactors[0], v) > 0 && degree (bufFactors [1], v) > 0)
  {
    M (1, 1)= mulMod2 (bufFactors [0] [0], bufFactors[1] [0], yToL);
    Pi [0]= M (1,1) + (mulMod2 (bufFactors [0] [1], bufFactors[1] [0], yToL) +
                       mulMod2 (bufFactors [0] [0], bufFactors [1] [1], yToL))*v;
  }
  else if (degree (bufFactors[0], v) > 0)
  {
    M (1,1)= mulMod2 (bufFactors [0] [0], bufFactors [1], yToL);
    Pi [0]=  M(1,1) + mulMod2 (bufFactors [0] [1], bufFactors[1], yToL)*v;
  }
  else if (degree (bufFactors[1], v) > 0)
  {
    M (1,1)= mulMod2 (bufFactors [0], bufFactors [1] [0], yToL);
    Pi [0]= M (1,1) + mulMod2 (bufFactors [0], bufFactors[1] [1], yToL)*v;
  }
  else
  {
    M (1,1)= mulMod2 (bufFactors [0], bufFactors [1], yToL);
    Pi [0]= M (1,1);
  }

  for (i= 1; i < Pi.size(); i++)
  {
    if (degree (Pi[i-1], v) > 0 && degree (bufFactors [i+1], v) > 0)
    {
      M (1,i+1)= mulMod2 (Pi[i-1] [0], bufFactors[i+1] [0], yToL);
      Pi [i]= M (1,i+1) + (mulMod2 (Pi[i-1] [1], bufFactors[i+1] [0], yToL) +
                       mulMod2 (Pi[i-1] [0], bufFactors [i+1] [1], yToL))*v;
    }
    else if (degree (Pi[i-1], v) > 0)
    {
      M (1,i+1)= mulMod2 (Pi[i-1] [0], bufFactors [i+1], yToL);
      Pi [i]=  M(1,i+1) + mulMod2 (Pi[i-1] [1], bufFactors[i+1], yToL)*v;
    }
    else if (degree (bufFactors[i+1], v) > 0)
    {
      M (1,i+1)= mulMod2 (Pi[i-1], bufFactors [i+1] [0], yToL);
      Pi [i]= M (1,i+1) + mulMod2 (Pi[i-1], bufFactors[i+1] [1], yToL)*v;
    }
    else
    {
      M (1,i+1)= mulMod2 (Pi [i-1], bufFactors [i+1], yToL);
      Pi [i]= M (1,i+1);
    }
  }

  CFList products;
  bufF= mod (F, Variable (3));
  TIMING_START (product1);
  for (CFListIterator k= factors; k.hasItem(); k++)
    products.append (bufF/k.getItem());
  TIMING_END_AND_PRINT (product1, "time for product1 in 23: ");

  CFList MOD= CFList (power (v, liftBound));
  MOD.insert (yToL);
  for (int d= 1; d < liftBound; d++)
  {
    nonMonicHenselStep (F, factors, bufFactors, diophant, M, Pi, products, d,
                        MOD, bad);
    if (bad)
      return CFList();
  }

  CFList result;
  for (i= 0; i < factors.length(); i++)
    result.append (bufFactors[i]);
  return result;
}
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT) // nonMonicHenselStep
CFList
nonMonicHenselLift (const CFList& F, const CFList& factors, const CFList& LCs,
                    CFList& diophant, CFArray& Pi, CFMatrix& M, int lOld,
                    int& lNew, const CFList& MOD, bool& noOneToOne
                   )
{

  int k= 0;
  CFArray bufFactors= CFArray (factors.length());
  CFListIterator j= LCs;
  for (CFListIterator i= factors; i.hasItem(); i++, j++, k++)
    bufFactors [k]= replaceLC (i.getItem(), j.getItem());

  Variable y= F.getLast().mvar();
  Variable x= F.getFirst().mvar();
  CanonicalForm xToLOld= power (x, lOld);

  Pi [0]= mod (Pi[0], xToLOld);
  M (1, 1)= Pi [0];

  if (degree (bufFactors[0], y) > 0 && degree (bufFactors [1], y) > 0)
    Pi [0] += (mulMod (bufFactors [0] [1], bufFactors[1] [0], MOD) +
                        mulMod (bufFactors [0] [0], bufFactors [1] [1], MOD))*y;
  else if (degree (bufFactors[0], y) > 0)
    Pi [0] += mulMod (bufFactors [0] [1], bufFactors[1], MOD)*y;
  else if (degree (bufFactors[1], y) > 0)
    Pi [0] += mulMod (bufFactors [0], bufFactors[1] [1], MOD)*y;

  for (int i= 1; i < Pi.size(); i++)
  {
    Pi [i]= mod (Pi [i], xToLOld);
    M (1, i + 1)= Pi [i];

    if (degree (Pi[i-1], y) > 0 && degree (bufFactors [i+1], y) > 0)
      Pi [i] += (mulMod (Pi[i-1] [1], bufFactors[i+1] [0], MOD) +
                 mulMod (Pi[i-1] [0], bufFactors [i+1] [1], MOD))*y;
    else if (degree (Pi[i-1], y) > 0)
      Pi [i] += mulMod (Pi[i-1] [1], bufFactors[i+1], MOD)*y;
    else if (degree (bufFactors[i+1], y) > 0)
      Pi [i] += mulMod (Pi[i-1], bufFactors[i+1] [1], MOD)*y;
  }

  CFList products;
  CanonicalForm quot, bufF= F.getFirst();

  TIMING_START (product1);
  for (int i= 0; i < bufFactors.size(); i++)
  {
    if (degree (bufFactors[i], y) > 0)
    {
      if (!fdivides (bufFactors[i] [0], bufF, quot))
      {
        noOneToOne= true;
        return factors;
      }
      products.append (quot);
    }
    else
    {
      if (!fdivides (bufFactors[i], bufF, quot))
      {
        noOneToOne= true;
        return factors;
      }
      products.append (quot);
    }
  }
  TIMING_END_AND_PRINT (product1, "time for product1 in further hensel:" );

  for (int d= 1; d < lNew; d++)
  {
    nonMonicHenselStep (F.getLast(), factors, bufFactors, diophant, M, Pi,
                        products, d, MOD, noOneToOne);
    if (noOneToOne)
      return CFList();
  }

  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}
#endif

#ifdef HAVE_NTL // nonMonicHenselLift23
CFList
nonMonicHenselLift (const CFList& eval, const CFList& factors,
                    CFList* const& LCs, CFList& diophant, CFArray& Pi,
                    int* liftBound, int length, bool& noOneToOne
                   )
{
  CFList bufDiophant= diophant;
  CFList buf= factors;
  CFArray bufPi= Pi;
  CFMatrix M= CFMatrix (liftBound[1], factors.length() - 1);
  int k= 0;

  TIMING_START (hensel23);
  CFList result=
  nonMonicHenselLift23 (eval.getFirst(), factors, LCs [0], diophant, bufPi,
                        liftBound[1], liftBound[0], noOneToOne);
  TIMING_END_AND_PRINT (hensel23, "time for 23: ");

  if (noOneToOne)
    return CFList();

  if (eval.length() == 1)
    return result;

  k++;
  CFList MOD;
  for (int i= 0; i < 2; i++)
    MOD.append (power (Variable (i + 2), liftBound[i]));

  CFListIterator j= eval;
  CFList bufEval;
  bufEval.append (j.getItem());
  j++;

  for (int i= 2; i <= length && j.hasItem(); i++, j++, k++)
  {
    bufEval.append (j.getItem());
    M= CFMatrix (liftBound[i], factors.length() - 1);
    TIMING_START (hensel);
    result= nonMonicHenselLift (bufEval, result, LCs [i-1], diophant, bufPi, M,
                                liftBound[i-1], liftBound[i], MOD, noOneToOne);
    TIMING_END_AND_PRINT (hensel, "time for further hensel: ");
    if (noOneToOne)
      return result;
    MOD.append (power (Variable (i + 2), liftBound[i]));
    bufEval.removeFirst();
  }

  return result;
}
#endif
#endif
