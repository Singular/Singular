/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facHensel.cc
 *
 * This file implements functions to lift factors via Hensel lifting and
 * functions for modular multiplication and division with remainder.
 *
 * ABSTRACT: Hensel lifting is described in "Efficient Multivariate
 * Factorization over Finite Fields" by L. Bernardin & M. Monagon. Division with
 * remainder is described in "Fast Recursive Division" by C. Burnikel and
 * J. Ziegler. Karatsuba multiplication is described in "Modern Computer
 * Algebra" by J. von zur Gathen and J. Gerhard.
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "facHensel.h"
#include "cf_util.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "cf_primes.h"

#ifdef HAVE_NTL
#include <NTL/lzz_pEX.h>
#include "NTLconvert.h"

static
CFList productsNTL (const CFList& factors, const CanonicalForm& M)
{
  zz_p::init (getCharacteristic());
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
  bufFactors= productsNTL (bufFactors, M);

  CanonicalForm buf1, buf2, buf3, S, T;
  i= bufFactors;
  if (i.hasItem())
    i++;
  buf1= bufFactors.getFirst();
  buf2= i.getItem();
  tryExtgcd (buf1, buf2, M, buf3, S, T, fail);
  if (fail)
    return;
  result.append (S);
  result.append (T);
  if (i.hasItem())
    i++;
  for (; i.hasItem(); i++)
  {
    buf1= i.getItem();
    tryExtgcd (buf3, buf1, M, buf3, S, T, fail);
    if (fail)
      return;
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

static inline
CFList mapinto (const CFList& L)
{
  CFList result;
  for (CFListIterator i= L; i.hasItem(); i++)
    result.append (mapinto (i.getItem()));
  return result;
}

static inline
int mod (const CFList& L, const CanonicalForm& p)
{
  for (CFListIterator i= L; i.hasItem(); i++)
  {
    if (mod (i.getItem(), p) == 0)
      return 0;
  }
  return 1;
}


static inline void
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

static inline
CFList Farey (const CFList& L, const CanonicalForm& q)
{
  CFList result;
  for (CFListIterator i= L; i.hasItem(); i++)
    result.append (Farey (i.getItem(), q));
  return result;
}

static inline
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

CanonicalForm
mulNTL (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() || G.inCoeffDomain() || getCharacteristic() == 0)
    return F*G;
  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
  if (CFFactory::gettype() == GaloisFieldDomain)
    return F*G;
  zz_p::init (getCharacteristic());
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    mul (NTLF, NTLF, NTLG);
    result= convertNTLzz_pEX2CF(NTLF, F.mvar(), alpha);
  }
  else
  {
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    mul (NTLF, NTLF, NTLG);
    result= convertNTLzzpX2CF(NTLF, F.mvar());
  }
  return result;
}

CanonicalForm
modNTL (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() && G.isUnivariate())
    return F;
  else if (F.inCoeffDomain() && G.inCoeffDomain())
    return mod (F, G);
  else if (F.isUnivariate() && G.inCoeffDomain())
    return mod (F,G);

  if (getCharacteristic() == 0)
    return mod (F, G);

  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
  if (CFFactory::gettype() == GaloisFieldDomain)
    return mod (F, G);
  zz_p::init (getCharacteristic());
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
    zz_pX NTLMipo= convertFacCF2NTLzzpX(getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    rem (NTLF, NTLF, NTLG);
    result= convertNTLzz_pEX2CF(NTLF, F.mvar(), alpha);
  }
  else
  {
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    rem (NTLF, NTLF, NTLG);
    result= convertNTLzzpX2CF(NTLF, F.mvar());
  }
  return result;
}

CanonicalForm
divNTL (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() && G.isUnivariate())
    return F;
  else if (F.inCoeffDomain() && G.inCoeffDomain())
    return div (F, G);
  else if (F.isUnivariate() && G.inCoeffDomain())
    return div (F,G);

  if (getCharacteristic() == 0)
    return div (F, G);

  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
  if (CFFactory::gettype() == GaloisFieldDomain)
    return div (F, G);
  zz_p::init (getCharacteristic());
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
    zz_pX NTLMipo= convertFacCF2NTLzzpX(getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    div (NTLF, NTLF, NTLG);
    result= convertNTLzz_pEX2CF(NTLF, F.mvar(), alpha);
  }
  else
  {
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    div (NTLF, NTLF, NTLG);
    result= convertNTLzzpX2CF(NTLF, F.mvar());
  }
  return result;
}

/*
void
divremNTL (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
           CanonicalForm& R)
{
  if (F.inCoeffDomain() && G.isUnivariate())
  {
    R= F;
    Q= 0;
  }
  else if (F.inCoeffDomain() && G.inCoeffDomain())
  {
    divrem (F, G, Q, R);
    return;
  }
  else if (F.isUnivariate() && G.inCoeffDomain())
  {
    divrem (F, G, Q, R);
    return;
  }

  ASSERT (F.isUnivariate() && G.isUnivariate(), "expected univariate polys");
  ASSERT (F.level() == G.level(), "expected polys of same level");
  if (CFFactory::gettype() == GaloisFieldDomain)
  {
    divrem (F, G, Q, R);
    return;
  }
  zz_p::init (getCharacteristic());
  Variable alpha;
  CanonicalForm result;
  if (hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha))
  {
    zz_pX NTLMipo= convertFacCF2NTLzzpX(getMipo (alpha));
    zz_pE::init (NTLMipo);
    zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
    zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
    zz_pEX NTLQ;
    zz_pEX NTLR;
    DivRem (NTLQ, NTLR, NTLF, NTLG);
    Q= convertNTLzz_pEX2CF(NTLQ, F.mvar(), alpha);
    R= convertNTLzz_pEX2CF(NTLR, F.mvar(), alpha);
    return;
  }
  else
  {
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);
    zz_pX NTLQ;
    zz_pX NTLR;
    DivRem (NTLQ, NTLR, NTLF, NTLG);
    Q= convertNTLzzpX2CF(NTLQ, F.mvar());
    R= convertNTLzzpX2CF(NTLR, F.mvar());
    return;
  }
}*/

CanonicalForm mod (const CanonicalForm& F, const CFList& M)
{
  CanonicalForm A= F;
  for (CFListIterator i= M; i.hasItem(); i++)
    A= mod (A, i.getItem());
  return A;
}

zz_pX kronSubFp (const CanonicalForm& A, int d)
{
  int degAy= degree (A);
  zz_pX result;
  result.rep.SetLength (d*(degAy + 1));

  zz_p *resultp;
  resultp= result.rep.elts();
  zz_pX buf;
  zz_p *bufp;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
      buf= convertFacCF2NTLzzpX (i.coeff());
    else
      buf= convertFacCF2NTLzzpX (i.coeff());

    int k= i.exp()*d;
    bufp= buf.rep.elts();
    int bufRepLength= (int) buf.rep.length();
    for (int j= 0; j < bufRepLength; j++)
      resultp [j + k]= bufp [j];
  }
  result.normalize();

  return result;
}

zz_pEX kronSub (const CanonicalForm& A, int d, const Variable& alpha)
{
  int degAy= degree (A);
  zz_pEX result;
  result.rep.SetLength (d*(degAy + 1));

  Variable v;
  zz_pE *resultp;
  resultp= result.rep.elts();
  zz_pEX buf1;
  zz_pE *buf1p;
  zz_pX buf2;
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      buf2= convertFacCF2NTLzzpX (i.coeff());
      buf1= to_zz_pEX (to_zz_pE (buf2));
    }
    else
      buf1= convertFacCF2NTLzz_pEX (i.coeff(), NTLMipo);

    int k= i.exp()*d;
    buf1p= buf1.rep.elts();
    int buf1RepLength= (int) buf1.rep.length();
    for (int j= 0; j < buf1RepLength; j++)
      resultp [j + k]= buf1p [j];
  }
  result.normalize();

  return result;
}

void
kronSubRecipro (zz_pEX& subA1, zz_pEX& subA2,const CanonicalForm& A, int d,
                const Variable& alpha)
{
  int degAy= degree (A);
  subA1.rep.SetLength ((long) d*(degAy + 2));
  subA2.rep.SetLength ((long) d*(degAy + 2));

  Variable v;
  zz_pE *subA1p;
  zz_pE *subA2p;
  subA1p= subA1.rep.elts();
  subA2p= subA2.rep.elts();
  zz_pEX buf;
  zz_pE *bufp;
  zz_pX buf2;
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    if (i.coeff().inCoeffDomain())
    {
      buf2= convertFacCF2NTLzzpX (i.coeff());
      buf= to_zz_pEX (to_zz_pE (buf2));
    }
    else
      buf= convertFacCF2NTLzz_pEX (i.coeff(), NTLMipo);

    int k= i.exp()*d;
    int kk= (degAy - i.exp())*d;
    bufp= buf.rep.elts();
    int bufRepLength= (int) buf.rep.length();
    for (int j= 0; j < bufRepLength; j++)
    {
      subA1p [j + k] += bufp [j];
      subA2p [j + kk] += bufp [j];
    }
  }
  subA1.normalize();
  subA2.normalize();
}

void
kronSubRecipro (zz_pX& subA1, zz_pX& subA2,const CanonicalForm& A, int d)
{
  int degAy= degree (A);
  subA1.rep.SetLength ((long) d*(degAy + 2));
  subA2.rep.SetLength ((long) d*(degAy + 2));

  zz_p *subA1p;
  zz_p *subA2p;
  subA1p= subA1.rep.elts();
  subA2p= subA2.rep.elts();
  zz_pX buf;
  zz_p *bufp;

  for (CFIterator i= A; i.hasTerms(); i++)
  {
    buf= convertFacCF2NTLzzpX (i.coeff());

    int k= i.exp()*d;
    int kk= (degAy - i.exp())*d;
    bufp= buf.rep.elts();
    int bufRepLength= (int) buf.rep.length();
    for (int j= 0; j < bufRepLength; j++)
    {
      subA1p [j + k] += bufp [j];
      subA2p [j + kk] += bufp [j];
    }
  }
  subA1.normalize();
  subA2.normalize();
}

CanonicalForm
reverseSubst (const zz_pEX& F, const zz_pEX& G, int d, int k,
              const Variable& alpha)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pEX f= F;
  zz_pEX g= G;
  int degf= deg(f);
  int degg= deg(g);

  zz_pEX buf1;
  zz_pEX buf2;
  zz_pEX buf3;

  zz_pE *buf1p;
  zz_pE *buf2p;
  zz_pE *buf3p;
  if (f.rep.length() < (long) d*(k+1)) //zero padding
    f.rep.SetLength ((long)d*(k+1));

  zz_pE *gp= g.rep.elts();
  zz_pE *fp= f.rep.elts();
  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2;
  int repLengthBuf1;
  zz_pE zzpEZero= zz_pE();

  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;
    buf1.rep.SetLength((long) repLengthBuf1);

    buf1p= buf1.rep.elts();
    for (int ind= 0; ind < repLengthBuf1; ind++)
      buf1p [ind]= fp [ind + lf];
    buf1.normalize();

    repLengthBuf1= buf1.rep.length();

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    buf2.rep.SetLength ((long) repLengthBuf2);
    buf2p= buf2.rep.elts();
    for (int ind= 0; ind < repLengthBuf2; ind++)
    {
      buf2p [ind]= gp [ind + lg];
    }
    buf2.normalize();

    repLengthBuf2= buf2.rep.length();

    buf3.rep.SetLength((long) repLengthBuf2 + d);
    buf3p= buf3.rep.elts();
    buf2p= buf2.rep.elts();
    buf1p= buf1.rep.elts();
    for (int ind= 0; ind < repLengthBuf1; ind++)
      buf3p [ind]= buf1p [ind];
    for (int ind= repLengthBuf1; ind < d; ind++)
      buf3p [ind]= zzpEZero;
    for (int ind= 0; ind < repLengthBuf2; ind++)
      buf3p [ind + d]= buf2p [ind];
    buf3.normalize();

    result += convertNTLzz_pEX2CF (buf3, x, alpha)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k-i);
    deggSubLg= degg - lg;

    buf1p= buf1.rep.elts();

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      int tmp= tmin (repLengthBuf1, deggSubLg + 1);
      for (int ind= 0; ind < tmp; ind++)
        gp [ind + lg] -= buf1p [ind];
    }

    if (lg < 0)
      break;

    buf2p= buf2.rep.elts();
    if (degfSubLf >= 0)
    {
      for (int ind= 0; ind < repLengthBuf2; ind++)
        fp [ind + lf] -= buf2p [ind];
    }
  }

  return result;
}

CanonicalForm
reverseSubst (const zz_pX& F, const zz_pX& G, int d, int k)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pX f= F;
  zz_pX g= G;
  int degf= deg(f);
  int degg= deg(g);

  zz_pX buf1;
  zz_pX buf2;
  zz_pX buf3;

  zz_p *buf1p;
  zz_p *buf2p;
  zz_p *buf3p;

  if (f.rep.length() < (long) d*(k+1)) //zero padding
    f.rep.SetLength ((long)d*(k+1));

  zz_p *gp= g.rep.elts();
  zz_p *fp= f.rep.elts();
  CanonicalForm result= 0;
  int i= 0;
  int lf= 0;
  int lg= d*k;
  int degfSubLf= degf;
  int deggSubLg= degg-lg;
  int repLengthBuf2;
  int repLengthBuf1;
  zz_p zzpZero= zz_p();
  while (degf >= lf || lg >= 0)
  {
    if (degfSubLf >= d)
      repLengthBuf1= d;
    else if (degfSubLf < 0)
      repLengthBuf1= 0;
    else
      repLengthBuf1= degfSubLf + 1;
    buf1.rep.SetLength((long) repLengthBuf1);

    buf1p= buf1.rep.elts();
    for (int ind= 0; ind < repLengthBuf1; ind++)
      buf1p [ind]= fp [ind + lf];
    buf1.normalize();

    repLengthBuf1= buf1.rep.length();

    if (deggSubLg >= d - 1)
      repLengthBuf2= d - 1;
    else if (deggSubLg < 0)
      repLengthBuf2= 0;
    else
      repLengthBuf2= deggSubLg + 1;

    buf2.rep.SetLength ((long) repLengthBuf2);
    buf2p= buf2.rep.elts();
    for (int ind= 0; ind < repLengthBuf2; ind++)
      buf2p [ind]= gp [ind + lg];

    buf2.normalize();

    repLengthBuf2= buf2.rep.length();


    buf3.rep.SetLength((long) repLengthBuf2 + d);
    buf3p= buf3.rep.elts();
    buf2p= buf2.rep.elts();
    buf1p= buf1.rep.elts();
    for (int ind= 0; ind < repLengthBuf1; ind++)
      buf3p [ind]= buf1p [ind];
    for (int ind= repLengthBuf1; ind < d; ind++)
      buf3p [ind]= zzpZero;
    for (int ind= 0; ind < repLengthBuf2; ind++)
      buf3p [ind + d]= buf2p [ind];
    buf3.normalize();

    result += convertNTLzzpX2CF (buf3, x)*power (y, i);
    i++;


    lf= i*d;
    degfSubLf= degf - lf;

    lg= d*(k-i);
    deggSubLg= degg - lg;

    buf1p= buf1.rep.elts();

    if (lg >= 0 && deggSubLg > 0)
    {
      if (repLengthBuf2 > degfSubLf + 1)
        degfSubLf= repLengthBuf2 - 1;
      int tmp= tmin (repLengthBuf1, deggSubLg + 1);
      for (int ind= 0; ind < tmp; ind++)
        gp [ind + lg] -= buf1p [ind];
    }
    if (lg < 0)
      break;

    buf2p= buf2.rep.elts();
    if (degfSubLf >= 0)
    {
      for (int ind= 0; ind < repLengthBuf2; ind++)
        fp [ind + lf] -= buf2p [ind];
    }
  }

  return result;
}

CanonicalForm reverseSubst (const zz_pEX& F, int d, const Variable& alpha)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pEX f= F;
  zz_pE *fp= f.rep.elts();

  zz_pEX buf;
  zz_pE *bufp;
  CanonicalForm result= 0;
  int i= 0;
  int degf= deg(f);
  int k= 0;
  int degfSubK;
  int repLength;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    buf.rep.SetLength ((long) repLength);
    bufp= buf.rep.elts();
    for (int j= 0; j < repLength; j++)
      bufp [j]= fp [j + k];
    buf.normalize();

    result += convertNTLzz_pEX2CF (buf, x, alpha)*power (y, i);
    i++;
    k= d*i;
  }

  return result;
}

CanonicalForm reverseSubstFp (const zz_pX& F, int d)
{
  Variable y= Variable (2);
  Variable x= Variable (1);

  zz_pX f= F;
  zz_p *fp= f.rep.elts();

  zz_pX buf;
  zz_p *bufp;
  CanonicalForm result= 0;
  int i= 0;
  int degf= deg(f);
  int k= 0;
  int degfSubK;
  int repLength;
  while (degf >= k)
  {
    degfSubK= degf - k;
    if (degfSubK >= d)
      repLength= d;
    else
      repLength= degfSubK + 1;

    buf.rep.SetLength ((long) repLength);
    bufp= buf.rep.elts();
    for (int j= 0; j < repLength; j++)
      bufp [j]= fp [j + k];
    buf.normalize();

    result += convertNTLzzpX2CF (buf, x)*power (y, i);
    i++;
    k= d*i;
  }

  return result;
}

// assumes input to be reduced mod M and to be an element of Fq not Fp
CanonicalForm
mulMod2NTLFpReci (const CanonicalForm& F, const CanonicalForm& G, const
                  CanonicalForm& M)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  zz_pX F1, F2;
  kronSubRecipro (F1, F2, F, d1);
  zz_pX G1, G2;
  kronSubRecipro (G1, G2, G, d1);

  int k= d1*degree (M);
  MulTrunc (F1, F1, G1, (long) k);

  int degtailF= degree (tailcoeff (F), 1);
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);
  int b= k + degtailF + degtailG - d1*(2+taildegF+taildegG);

  reverse (F2, F2);
  reverse (G2, G2);
  MulTrunc (F2, F2, G2, b + 1);
  reverse (F2, F2, b);

  int d2= tmax (deg (F2)/d1, deg (F1)/d1);
  return reverseSubst (F1, F2, d1, d2);
}

//Kronecker substitution
CanonicalForm
mulMod2NTLFp (const CanonicalForm& F, const CanonicalForm& G, const
              CanonicalForm& M)
{
  CanonicalForm A= F;
  CanonicalForm B= G;

  int degAx= degree (A, 1);
  int degAy= degree (A, 2);
  int degBx= degree (B, 1);
  int degBy= degree (B, 2);
  int d1= degAx + 1 + degBx;
  int d2= tmax (degAy, degBy);

  if (d1 > 128 && d2 > 160 && (degAy == degBy) && (2*degAy > degree (M)))
    return mulMod2NTLFpReci (A, B, M);

  zz_pX NTLA= kronSubFp (A, d1);
  zz_pX NTLB= kronSubFp (B, d1);

  int k= d1*degree (M);
  MulTrunc (NTLA, NTLA, NTLB, (long) k);

  A= reverseSubstFp (NTLA, d1);

  return A;
}

// assumes input to be reduced mod M and to be an element of Fq not Fp
CanonicalForm
mulMod2NTLFqReci (const CanonicalForm& F, const CanonicalForm& G, const
                  CanonicalForm& M, const Variable& alpha)
{
  int d1= degree (F, 1) + degree (G, 1) + 1;
  d1 /= 2;
  d1 += 1;

  zz_pEX F1, F2;
  kronSubRecipro (F1, F2, F, d1, alpha);
  zz_pEX G1, G2;
  kronSubRecipro (G1, G2, G, d1, alpha);

  int k= d1*degree (M);
  MulTrunc (F1, F1, G1, (long) k);

  int degtailF= degree (tailcoeff (F), 1);
  int degtailG= degree (tailcoeff (G), 1);
  int taildegF= taildegree (F);
  int taildegG= taildegree (G);
  int b= k + degtailF + degtailG - d1*(2+taildegF+taildegG);

  reverse (F2, F2);
  reverse (G2, G2);
  MulTrunc (F2, F2, G2, b + 1);
  reverse (F2, F2, b);

  int d2= tmax (deg (F2)/d1, deg (F1)/d1);
  return reverseSubst (F1, F2, d1, d2, alpha);
}

CanonicalForm
mulMod2NTLFq (const CanonicalForm& F, const CanonicalForm& G, const
              CanonicalForm& M)
{
  Variable alpha;
  CanonicalForm A= F;
  CanonicalForm B= G;

  if (hasFirstAlgVar (A, alpha) || hasFirstAlgVar (B, alpha))
  {
    int degAx= degree (A, 1);
    int degAy= degree (A, 2);
    int degBx= degree (B, 1);
    int degBy= degree (B, 2);
    int d1= degAx + degBx + 1;
    int d2= tmax (degAy, degBy);
    zz_p::init (getCharacteristic());
    zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
    zz_pE::init (NTLMipo);

    int degMipo= degree (getMipo (alpha));
    if ((d1 > 128/degMipo) && (d2 > 160/degMipo) && (degAy == degBy) &&
        (2*degAy > degree (M)))
      return mulMod2NTLFqReci (A, B, M, alpha);

    zz_pEX NTLA= kronSub (A, d1, alpha);
    zz_pEX NTLB= kronSub (B, d1, alpha);

    int k= d1*degree (M);

    MulTrunc (NTLA, NTLA, NTLB, (long) k);

    A= reverseSubst (NTLA, d1, alpha);

    return A;
  }
  else
    return mulMod2NTLFp (A, B, M);
}

CanonicalForm mulMod2 (const CanonicalForm& A, const CanonicalForm& B,
                       const CanonicalForm& M)
{
  if (A.isZero() || B.isZero())
    return 0;

  ASSERT (M.isUnivariate(), "M must be univariate");

  CanonicalForm F= mod (A, M);
  CanonicalForm G= mod (B, M);
  if (F.inCoeffDomain() || G.inCoeffDomain())
    return F*G;
  Variable y= M.mvar();
  int degF= degree (F, y);
  int degG= degree (G, y);

  if ((degF < 1 && degG < 1) && (F.isUnivariate() && G.isUnivariate()) &&
      (F.level() == G.level()))
  {
    CanonicalForm result= mulNTL (F, G);
    return mod (result, M);
  }
  else if (degF <= 1 && degG <= 1)
  {
    CanonicalForm result= F*G;
    return mod (result, M);
  }

  int sizeF= size (F);
  int sizeG= size (G);

  int fallBackToNaive= 50;
  if (sizeF < fallBackToNaive || sizeG < fallBackToNaive)
    return mod (F*G, M);

  if (getCharacteristic() > 0 && CFFactory::gettype() != GaloisFieldDomain &&
      (((degF-degG) < 50 && degF > degG) || ((degG-degF) < 50 && degF <= degG)))
    return mulMod2NTLFq (F, G, M);

  int m= (int) ceil (degree (M)/2.0);
  if (degF >= m || degG >= m)
  {
    CanonicalForm MLo= power (y, m);
    CanonicalForm MHi= power (y, degree (M) - m);
    CanonicalForm F0= mod (F, MLo);
    CanonicalForm F1= div (F, MLo);
    CanonicalForm G0= mod (G, MLo);
    CanonicalForm G1= div (G, MLo);
    CanonicalForm F0G1= mulMod2 (F0, G1, MHi);
    CanonicalForm F1G0= mulMod2 (F1, G0, MHi);
    CanonicalForm F0G0= mulMod2 (F0, G0, M);
    return F0G0 + MLo*(F0G1 + F1G0);
  }
  else
  {
    m= (int) ceil (tmax (degF, degG)/2.0);
    CanonicalForm yToM= power (y, m);
    CanonicalForm F0= mod (F, yToM);
    CanonicalForm F1= div (F, yToM);
    CanonicalForm G0= mod (G, yToM);
    CanonicalForm G1= div (G, yToM);
    CanonicalForm H00= mulMod2 (F0, G0, M);
    CanonicalForm H11= mulMod2 (F1, G1, M);
    CanonicalForm H01= mulMod2 (F0 + F1, G0 + G1, M);
    return H11*yToM*yToM + (H01 - H11 - H00)*yToM + H00;
  }
  DEBOUTLN (cerr, "fatal end in mulMod2");
}

CanonicalForm mulMod (const CanonicalForm& A, const CanonicalForm& B,
                      const CFList& MOD)
{
  if (A.isZero() || B.isZero())
    return 0;

  if (MOD.length() == 1)
    return mulMod2 (A, B, MOD.getLast());

  CanonicalForm M= MOD.getLast();
  CanonicalForm F= mod (A, M);
  CanonicalForm G= mod (B, M);
  if (F.inCoeffDomain() || G.inCoeffDomain())
    return F*G;
  Variable y= M.mvar();
  int degF= degree (F, y);
  int degG= degree (G, y);

  if ((degF <= 1 && F.level() <= M.level()) &&
      (degG <= 1 && G.level() <= M.level()))
  {
    CFList buf= MOD;
    buf.removeLast();
    if (degF == 1 && degG == 1)
    {
      CanonicalForm F0= mod (F, y);
      CanonicalForm F1= div (F, y);
      CanonicalForm G0= mod (G, y);
      CanonicalForm G1= div (G, y);
      if (degree (M) > 2)
      {
        CanonicalForm H00= mulMod (F0, G0, buf);
        CanonicalForm H11= mulMod (F1, G1, buf);
        CanonicalForm H01= mulMod (F0 + F1, G0 + G1, buf);
        return H11*y*y + (H01 - H00 - H11)*y + H00;
      }
      else //here degree (M) == 2
      {
        buf.append (y);
        CanonicalForm F0G1= mulMod (F0, G1, buf);
        CanonicalForm F1G0= mulMod (F1, G0, buf);
        CanonicalForm F0G0= mulMod (F0, G0, MOD);
        CanonicalForm result= F0G0 + y*(F0G1 + F1G0);
        return result;
      }
    }
    else if (degF == 1 && degG == 0)
      return mulMod (div (F, y), G, buf)*y + mulMod (mod (F, y), G, buf);
    else if (degF == 0 && degG == 1)
      return mulMod (div (G, y), F, buf)*y + mulMod (mod (G, y), F, buf);
    else
      return mulMod (F, G, buf);
  }
  int m= (int) ceil (degree (M)/2.0);
  if (degF >= m || degG >= m)
  {
    CanonicalForm MLo= power (y, m);
    CanonicalForm MHi= power (y, degree (M) - m);
    CanonicalForm F0= mod (F, MLo);
    CanonicalForm F1= div (F, MLo);
    CanonicalForm G0= mod (G, MLo);
    CanonicalForm G1= div (G, MLo);
    CFList buf= MOD;
    buf.removeLast();
    buf.append (MHi);
    CanonicalForm F0G1= mulMod (F0, G1, buf);
    CanonicalForm F1G0= mulMod (F1, G0, buf);
    CanonicalForm F0G0= mulMod (F0, G0, MOD);
    return F0G0 + MLo*(F0G1 + F1G0);
  }
  else
  {
    m= (int) ceil (tmax (degF, degG)/2.0);
    CanonicalForm yToM= power (y, m);
    CanonicalForm F0= mod (F, yToM);
    CanonicalForm F1= div (F, yToM);
    CanonicalForm G0= mod (G, yToM);
    CanonicalForm G1= div (G, yToM);
    CanonicalForm H00= mulMod (F0, G0, MOD);
    CanonicalForm H11= mulMod (F1, G1, MOD);
    CanonicalForm H01= mulMod (F0 + F1, G0 + G1, MOD);
    return H11*yToM*yToM + (H01 - H11 - H00)*yToM + H00;
  }
  DEBOUTLN (cerr, "fatal end in mulMod");
}

CanonicalForm prodMod (const CFList& L, const CanonicalForm& M)
{
  if (L.isEmpty())
    return 1;
  int l= L.length();
  if (l == 1)
    return mod (L.getFirst(), M);
  else if (l == 2) {
    CanonicalForm result= mulMod2 (L.getFirst(), L.getLast(), M);
    return result;
  }
  else
  {
    l /= 2;
    CFList tmp1, tmp2;
    CFListIterator i= L;
    CanonicalForm buf1, buf2;
    for (int j= 1; j <= l; j++, i++)
      tmp1.append (i.getItem());
    tmp2= Difference (L, tmp1);
    buf1= prodMod (tmp1, M);
    buf2= prodMod (tmp2, M);
    CanonicalForm result= mulMod2 (buf1, buf2, M);
    return result;
  }
}

CanonicalForm prodMod (const CFList& L, const CFList& M)
{
  if (L.isEmpty())
    return 1;
  else if (L.length() == 1)
    return L.getFirst();
  else if (L.length() == 2)
    return mulMod (L.getFirst(), L.getLast(), M);
  else
  {
    int l= L.length()/2;
    CFListIterator i= L;
    CFList tmp1, tmp2;
    CanonicalForm buf1, buf2;
    for (int j= 1; j <= l; j++, i++)
      tmp1.append (i.getItem());
    tmp2= Difference (L, tmp1);
    buf1= prodMod (tmp1, M);
    buf2= prodMod (tmp2, M);
    return mulMod (buf1, buf2, M);
  }
}


CanonicalForm reverse (const CanonicalForm& F, int d)
{
  if (d == 0)
    return F;
  CanonicalForm A= F;
  Variable y= Variable (2);
  Variable x= Variable (1);
  if (degree (A, x) > 0)
  {
    A= swapvar (A, x, y);
    CanonicalForm result= 0;
    CFIterator i= A;
    while (d - i.exp() < 0)
      i++;

    for (; i.hasTerms() && (d - i.exp() >= 0); i++)
      result += swapvar (i.coeff(),x,y)*power (x, d - i.exp());
    return result;
  }
  else
    return A*power (x, d);
}

CanonicalForm
newtonInverse (const CanonicalForm& F, const int n, const CanonicalForm& M)
{
  int l= ilog2(n);

  CanonicalForm g= mod (F, M)[0] [0];

  ASSERT (!g.isZero(), "expected a unit");

  Variable alpha;

  if (!g.isOne())
    g = 1/g;
  Variable x= Variable (1);
  CanonicalForm result;
  int exp= 0;
  if (n & 1)
  {
    result= g;
    exp= 1;
  }
  CanonicalForm h;

  for (int i= 1; i <= l; i++)
  {
    h= mulMod2 (g, mod (F, power (x, (1 << i))), M);
    h= mod (h, power (x, (1 << i)) - 1);
    h= div (h, power (x, (1 << (i - 1))));
    h= mod (h, M);
    g -= power (x, (1 << (i - 1)))*
         mod (mulMod2 (g, h, M), power (x, (1 << (i - 1))));

    if (n & (1 << i))
    {
      if (exp)
      {
        h= mulMod2 (result, mod (F, power (x, exp + (1 << i))), M);
        h= mod (h, power (x, exp + (1 << i)) - 1);
        h= div (h, power (x, exp));
        h= mod (h, M);
        result -= power(x, exp)*mod (mulMod2 (g, h, M),
                                       power (x, (1 << i)));
        exp += (1 << i);
      }
      else
      {
        exp= (1 << i);
        result= g;
      }
    }
  }

  return result;
}

CanonicalForm
newtonDiv (const CanonicalForm& F, const CanonicalForm& G, const CanonicalForm&
           M)
{
  ASSERT (getCharacteristic() > 0, "positive characteristic expected");
  ASSERT (CFFactory::gettype() != GaloisFieldDomain, "no GF expected");

  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);

  Variable x= Variable (1);
  int degA= degree (A, x);
  int degB= degree (B, x);
  int m= degA - degB;
  if (m < 0)
    return 0;

  CanonicalForm Q;
  if (degB <= 1 || CFFactory::gettype() == GaloisFieldDomain)
  {
    CanonicalForm R;
    divrem2 (A, B, Q, R, M);
  }
  else
  {
    CanonicalForm R= reverse (A, degA);
    CanonicalForm revB= reverse (B, degB);
    revB= newtonInverse (revB, m + 1, M);
    Q= mulMod2 (R, revB, M);
    Q= mod (Q, power (x, m + 1));
    Q= reverse (Q, m);
  }

  return Q;
}

void
newtonDivrem (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
              CanonicalForm& R, const CanonicalForm& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);
  Variable x= Variable (1);
  int degA= degree (A, x);
  int degB= degree (B, x);
  int m= degA - degB;

  if (m < 0)
  {
    R= A;
    Q= 0;
    return;
  }

  if (degB <= 1 || CFFactory::gettype() == GaloisFieldDomain)
  {
     divrem2 (A, B, Q, R, M);
  }
  else
  {
    R= reverse (A, degA);

    CanonicalForm revB= reverse (B, degB);
    revB= newtonInverse (revB, m + 1, M);
    Q= mulMod2 (R, revB, M);

    Q= mod (Q, power (x, m + 1));
    Q= reverse (Q, m);

    R= A - mulMod2 (Q, B, M);
  }
}

static inline
CFList split (const CanonicalForm& F, const int m, const Variable& x)
{
  CanonicalForm A= F;
  CanonicalForm buf= 0;
  bool swap= false;
  if (degree (A, x) <= 0)
    return CFList(A);
  else if (x.level() != A.level())
  {
    swap= true;
    A= swapvar (A, x, A.mvar());
  }

  int j= (int) floor ((double) degree (A)/ m);
  CFList result;
  CFIterator i= A;
  for (; j >= 0; j--)
  {
    while (i.hasTerms() && i.exp() - j*m >= 0)
    {
      if (swap)
        buf += i.coeff()*power (A.mvar(), i.exp() - j*m);
      else
        buf += i.coeff()*power (x, i.exp() - j*m);
      i++;
    }
    if (swap)
      result.append (swapvar (buf, x, F.mvar()));
    else
      result.append (buf);
    buf= 0;
  }
  return result;
}

static inline
void divrem32 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
               CanonicalForm& R, const CFList& M);

static inline
void divrem21 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
               CanonicalForm& R, const CFList& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);
  Variable x= Variable (1);
  int degB= degree (B, x);
  int degA= degree (A, x);
  if (degA < degB)
  {
    Q= 0;
    R= A;
    return;
  }
  ASSERT (2*degB > degA, "expected degree (F, 1) < 2*degree (G, 1)");
  if (degB < 1)
  {
    divrem (A, B, Q, R);
    Q= mod (Q, M);
    R= mod (R, M);
    return;
  }

  int m= (int) ceil ((double) (degB + 1)/2.0) + 1;
  CFList splitA= split (A, m, x);
  if (splitA.length() == 3)
    splitA.insert (0);
  if (splitA.length() == 2)
  {
    splitA.insert (0);
    splitA.insert (0);
  }
  if (splitA.length() == 1)
  {
    splitA.insert (0);
    splitA.insert (0);
    splitA.insert (0);
  }

  CanonicalForm xToM= power (x, m);

  CFListIterator i= splitA;
  CanonicalForm H= i.getItem();
  i++;
  H *= xToM;
  H += i.getItem();
  i++;
  H *= xToM;
  H += i.getItem();
  i++;

  divrem32 (H, B, Q, R, M);

  CFList splitR= split (R, m, x);
  if (splitR.length() == 1)
    splitR.insert (0);

  H= splitR.getFirst();
  H *= xToM;
  H += splitR.getLast();
  H *= xToM;
  H += i.getItem();

  CanonicalForm bufQ;
  divrem32 (H, B, bufQ, R, M);

  Q *= xToM;
  Q += bufQ;
  return;
}

static inline
void divrem32 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
               CanonicalForm& R, const CFList& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);
  Variable x= Variable (1);
  int degB= degree (B, x);
  int degA= degree (A, x);
  if (degA < degB)
  {
    Q= 0;
    R= A;
    return;
  }
  ASSERT (3*(degB/2) > degA, "expected degree (F, 1) < 3*(degree (G, 1)/2)");
  if (degB < 1)
  {
    divrem (A, B, Q, R);
    Q= mod (Q, M);
    R= mod (R, M);
    return;
  }
  int m= (int) ceil ((double) (degB + 1)/ 2.0);

  CFList splitA= split (A, m, x);
  CFList splitB= split (B, m, x);

  if (splitA.length() == 2)
  {
    splitA.insert (0);
  }
  if (splitA.length() == 1)
  {
    splitA.insert (0);
    splitA.insert (0);
  }
  CanonicalForm xToM= power (x, m);

  CanonicalForm H;
  CFListIterator i= splitA;
  i++;

  if (degree (splitA.getFirst(), x) < degree (splitB.getFirst(), x))
  {
    H= splitA.getFirst()*xToM + i.getItem();
    divrem21 (H, splitB.getFirst(), Q, R, M);
  }
  else
  {
    R= splitA.getFirst()*xToM + i.getItem() + splitB.getFirst() -
       splitB.getFirst()*xToM;
    Q= xToM - 1;
  }

  H= mulMod (Q, splitB.getLast(), M);

  R= R*xToM + splitA.getLast() - H;

  while (degree (R, x) >= degB)
  {
    xToM= power (x, degree (R, x) - degB);
    Q += LC (R, x)*xToM;
    R -= mulMod (LC (R, x), B, M)*xToM;
    Q= mod (Q, M);
    R= mod (R, M);
  }

  return;
}

void divrem2 (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
              CanonicalForm& R, const CanonicalForm& M)
{
  CanonicalForm A= mod (F, M);
  CanonicalForm B= mod (G, M);

  if (B.inCoeffDomain())
  {
    divrem (A, B, Q, R);
    return;
  }
  if (A.inCoeffDomain() && !B.inCoeffDomain())
  {
    Q= 0;
    R= A;
    return;
  }

  if (B.level() < A.level())
  {
    divrem (A, B, Q, R);
    return;
  }
  if (A.level() > B.level())
  {
    R= A;
    Q= 0;
    return;
  }
  if (B.level() == 1 && B.isUnivariate())
  {
    divrem (A, B, Q, R);
    return;
  }
  if (!(B.level() == 1 && B.isUnivariate()) && (A.level() == 1 && A.isUnivariate()))
  {
    Q= 0;
    R= A;
    return;
  }

  Variable x= Variable (1);
  int degB= degree (B, x);
  if (degB > degree (A, x))
  {
    Q= 0;
    R= A;
    return;
  }

  CFList splitA= split (A, degB, x);

  CanonicalForm xToDegB= power (x, degB);
  CanonicalForm H, bufQ;
  Q= 0;
  CFListIterator i= splitA;
  H= i.getItem()*xToDegB;
  i++;
  H += i.getItem();
  CFList buf;
  while (i.hasItem())
  {
    buf= CFList (M);
    divrem21 (H, B, bufQ, R, buf);
    i++;
    if (i.hasItem())
      H= R*xToDegB + i.getItem();
    Q *= xToDegB;
    Q += bufQ;
  }
  return;
}

void divrem (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
             CanonicalForm& R, const CFList& MOD)
{
  CanonicalForm A= mod (F, MOD);
  CanonicalForm B= mod (G, MOD);
  Variable x= Variable (1);
  int degB= degree (B, x);
  if (degB > degree (A, x))
  {
    Q= 0;
    R= A;
    return;
  }

  if (degB <= 0)
  {
    divrem (A, B, Q, R);
    Q= mod (Q, MOD);
    R= mod (R, MOD);
    return;
  }
  CFList splitA= split (A, degB, x);

  CanonicalForm xToDegB= power (x, degB);
  CanonicalForm H, bufQ;
  Q= 0;
  CFListIterator i= splitA;
  H= i.getItem()*xToDegB;
  i++;
  H += i.getItem();
  while (i.hasItem())
  {
    divrem21 (H, B, bufQ, R, MOD);
    i++;
    if (i.hasItem())
      H= R*xToDegB + i.getItem();
    Q *= xToDegB;
    Q += bufQ;
  }
  return;
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

static inline
CFList diophantine (const CanonicalForm& F, const CFList& factors)
{
  if (getCharacteristic() == 0)
  {
    Variable v;
    bool hasAlgVar= hasFirstAlgVar (F, v);
    for (CFListIterator i= factors; i.hasItem() && !hasAlgVar; i++)
      hasAlgVar= hasFirstAlgVar (i.getItem(), v);
    if (hasAlgVar)
    {
      CFList result= modularDiophant (F, factors, getMipo (v));
      return result;
    }
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

void
henselStep12 (const CanonicalForm& F, const CFList& factors,
              CFArray& bufFactors, const CFList& diophant, CFMatrix& M,
              CFArray& Pi, int j)
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
        remainder= modNTL (E, bufFactors[k] [0]);
      else
        remainder= E;
    }
    else
      remainder= modNTL (E, bufFactors[k]);

    buf[k]= mulNTL (i.getItem(), remainder);
    if (degree (bufFactors[k], x) > 0)
      buf[k]= modNTL (buf[k], bufFactors[k] [0]);
    else
      buf[k]= modNTL (buf[k], bufFactors[k]);
  }
  for (k= 1; k < factors.length(); k++)
    bufFactors[k] += xToJ*buf[k];

  // update Pi [0]
  int degBuf0= degree (bufFactors[0], x);
  int degBuf1= degree (bufFactors[1], x);
  if (degBuf0 > 0 && degBuf1 > 0)
    M (j + 1, 1)= mulNTL (bufFactors[0] [j], bufFactors[1] [j]);
  CanonicalForm uIZeroJ;
  if (j == 1)
  {
    if (degBuf0 > 0 && degBuf1 > 0)
      uIZeroJ= mulNTL ((bufFactors[0] [0] + bufFactors[0] [j]),
                  (bufFactors[1] [0] + buf[1])) - M(1, 1) - M(j + 1, 1);
    else if (degBuf0 > 0)
      uIZeroJ= mulNTL (bufFactors[0] [j], bufFactors[1]);
    else if (degBuf1 > 0)
      uIZeroJ= mulNTL (bufFactors[0], buf[1]);
    else
      uIZeroJ= 0;
    Pi [0] += xToJ*uIZeroJ;
  }
  else
  {
    if (degBuf0 > 0 && degBuf1 > 0)
      uIZeroJ= mulNTL ((bufFactors[0] [0] + bufFactors[0] [j]),
                  (bufFactors[1] [0] + buf[1])) - M(1, 1) - M(j + 1, 1);
    else if (degBuf0 > 0)
      uIZeroJ= mulNTL (bufFactors[0] [j], bufFactors[1]);
    else if (degBuf1 > 0)
      uIZeroJ= mulNTL (bufFactors[0], buf[1]);
    else
      uIZeroJ= 0;
    Pi [0] += xToJ*uIZeroJ;
  }
  CFArray tmp= CFArray (factors.length() - 1);
  for (k= 0; k < factors.length() - 1; k++)
    tmp[k]= 0;
  CFIterator one, two;
  one= bufFactors [0];
  two= bufFactors [1];
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    for (k= 1; k <= (int) ceil (j/2.0); k++)
    {
      if (k != j - k + 1)
      {
        if ((one.hasTerms() && one.exp() == j - k + 1) && (two.hasTerms() && two.exp() == j - k + 1))
        {
          tmp[0] += mulNTL ((bufFactors[0] [k] + one.coeff()), (bufFactors[1] [k] +
                     two.coeff())) - M (k + 1, 1) - M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.hasTerms() && one.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0] [k] + one.coeff()), bufFactors[1] [k]) -
                     M (k + 1, 1);
          one++;
        }
        else if (two.hasTerms() && two.exp() == j - k + 1)
        {
          tmp[0] += mulNTL (bufFactors[0] [k], (bufFactors[1] [k] + two.coeff())) -
                    M (k + 1, 1);
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
      M (j + 1, l + 1)= mulNTL (Pi [l - 1] [j], bufFactors[l + 1] [j]);
    if (j == 1)
    {
      if (degPi > 0 && degBuf > 0)
        Pi [l] += xToJ*(mulNTL (Pi [l - 1] [0] + Pi [l - 1] [j],
                  bufFactors[l + 1] [0] + buf[l + 1]) - M (j + 1, l +1) -
                  M (1, l + 1));
      else if (degPi > 0)
        Pi [l] += xToJ*(mulNTL (Pi [l - 1] [j], bufFactors[l + 1]));
      else if (degBuf > 0)
        Pi [l] += xToJ*(mulNTL (Pi [l - 1], buf[l + 1]));
    }
    else
    {
      if (degPi > 0 && degBuf > 0)
      {
        uIZeroJ= mulNTL (uIZeroJ, bufFactors [l + 1] [0]);
        uIZeroJ += mulNTL (Pi [l - 1] [0], buf [l + 1]);
      }
      else if (degPi > 0)
        uIZeroJ= mulNTL (uIZeroJ, bufFactors [l + 1]);
      else if (degBuf > 0)
      {
        uIZeroJ= mulNTL (uIZeroJ, bufFactors [l + 1] [0]);
        uIZeroJ += mulNTL (Pi [l - 1], buf[l + 1]);
      }
      Pi[l] += xToJ*uIZeroJ;
    }
    one= bufFactors [l + 1];
    two= Pi [l - 1];
    if (two.hasTerms() && two.exp() == j + 1)
    {
      if (degBuf > 0 && degPi > 0)
      {
          tmp[l] += mulNTL (two.coeff(), bufFactors[l + 1][0]);
          two++;
      }
      else if (degPi > 0)
      {
          tmp[l] += mulNTL (two.coeff(), bufFactors[l + 1]);
          two++;
      }
    }
    if (degBuf > 0 && degPi > 0)
    {
      for (k= 1; k <= (int) ceil (j/2.0); k++)
      {
        if (k != j - k + 1)
        {
          if ((one.hasTerms() && one.exp() == j - k + 1) &&
              (two.hasTerms() && two.exp() == j - k + 1))
          {
            tmp[l] += mulNTL ((bufFactors[l + 1] [k] + one.coeff()), (Pi[l - 1] [k] +
                       two.coeff())) - M (k + 1, l + 1) - M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.hasTerms() && one.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l + 1] [k] + one.coeff()), Pi[l - 1] [k]) -
                       M (k + 1, l + 1);
            one++;
          }
          else if (two.hasTerms() && two.exp() == j - k + 1)
          {
            tmp[l] += mulNTL (bufFactors[l + 1] [k], (Pi[l - 1] [k] + two.coeff())) -
                      M (k + 1, l + 1);
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

void
henselLift12 (const CanonicalForm& F, CFList& factors, int l, CFArray& Pi,
              CFList& diophant, CFMatrix& M, bool sort)
{
  if (sort)
    sortList (factors, Variable (1));
  Pi= CFArray (factors.length() - 1);
  CFListIterator j= factors;
  diophant= diophantine (F[0], factors);
  DEBOUTLN (cerr, "diophant= " << diophant);
  j++;
  Pi [0]= mulNTL (j.getItem(), mod (factors.getFirst(), F.mvar()));
  M (1, 1)= Pi [0];
  int i= 1;
  if (j.hasItem())
    j++;
  for (; j.hasItem(); j++, i++)
  {
    Pi [i]= mulNTL (Pi [i - 1], j.getItem());
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
    henselStep12 (F, factors, bufFactors, diophant, M, Pi, i);

  CFListIterator k= factors;
  for (i= 0; i < factors.length (); i++, k++)
    k.getItem()= bufFactors[i];
  factors.removeFirst();
  return;
}

void
henselLiftResume12 (const CanonicalForm& F, CFList& factors, int start, int
                    end, CFArray& Pi, const CFList& diophant, CFMatrix& M)
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
    henselStep12 (F, factors, bufFactors, diophant, M, Pi, i);

  CFListIterator k= factors;
  for (i= 0; i < factors.length(); k++, i++)
    k.getItem()= bufFactors [i];
  factors.removeFirst();
  return;
}

static inline
CFList
biDiophantine (const CanonicalForm& F, const CFList& factors, const int d)
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

static inline
CFList
multiRecDiophantine (const CanonicalForm& F, const CFList& factors,
                     const CFList& recResult, const CFList& M, const int d)
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
    DEBOUTLN (cerr, "test= " << test);
#endif
  return result;
}

static inline
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
    DEBOUTLN (cerr, "test= " << test2);
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
    DEBOUTLN (cerr, "test= " << test2);
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
  if (j == 1)
  {
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
  }
  else
  {
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
  }

  CFArray tmp= CFArray (factors.length() - 1);
  for (k= 0; k < factors.length() - 1; k++)
    tmp[k]= 0;
  CFIterator one, two;
  one= bufFactors [0];
  two= bufFactors [1];
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    for (k= 1; k <= (int) ceil (j/2.0); k++)
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
      for (k= 1; k <= (int) ceil (j/2.0); k++)
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

CFList
henselLift23 (const CFList& eval, const CFList& factors, const int* l, CFList&
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
            diophant, CFArray& Pi, CFMatrix& M, const int lOld, const int
            lNew)
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

CFList
henselLift (const CFList& eval, const CFList& factors, const int* l, const int
            lLength, bool sort)
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

void
henselStep122 (const CanonicalForm& F, const CFList& factors,
              CFArray& bufFactors, const CFList& diophant, CFMatrix& M,
              CFArray& Pi, int j, const CFArray& LCs)
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
  CanonicalForm uIZeroJ;
  if (degBuf0 > 0 && degBuf1 > 0)
    uIZeroJ= mulNTL(bufFactors[0][0],buf[1])+mulNTL (bufFactors[1][0], buf[0]);
  else if (degBuf0 > 0)
    uIZeroJ= mulNTL (buf[0], bufFactors[1]);
  else if (degBuf1 > 0)
    uIZeroJ= mulNTL (bufFactors[0], buf [1]);
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
    while (one.hasTerms() && one.exp() > j) one++;
    while (two.hasTerms() && two.exp() > j) two++;
    for (k= 1; k <= (int) ceil (j/2.0); k++)
    {
      if (one.hasTerms() && two.hasTerms())
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
  return;
}

void
henselLift122 (const CanonicalForm& F, CFList& factors, int l, CFArray& Pi,
              CFList& diophant, CFMatrix& M, const CFArray& LCs, bool sort)
{
  if (sort)
    sortList (factors, Variable (1));
  Pi= CFArray (factors.length() - 2);
  CFList bufFactors2= factors;
  bufFactors2.removeFirst();
  CanonicalForm s,t;
  extgcd (bufFactors2.getFirst(), bufFactors2.getLast(), s, t);
  diophant= CFList();
  diophant.append (t);
  diophant.append (s);
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

  for (i= 1; i < l; i++)
    henselStep122 (F, bufFactors2, bufFactors, diophant, M, Pi, i, LCs);

  factors= CFList();
  for (i= 0; i < bufFactors.size(); i++)
    factors.append (bufFactors[i]);
  return;
}

/// solve \f$ E=sum_{i= 1}^{r}{\sigma_{i}prod_{j=1, j\neq i}^{r}{f_{i}}}\f$
/// mod M, products contains \f$ prod_{j=1, j\neq i}^{r}{f_{i}}} \f$
static inline
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
    e -= i.getItem()*j.getItem();

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
        e -= j.getItem()*power (y, i)*l.getItem();
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

static inline
void
henselStep2 (const CanonicalForm& F, const CFList& factors, CFArray& bufFactors,
            const CFList& diophant, CFMatrix& M, CFArray& Pi,
            const CFList& products, int j, const CFList& MOD, bool& noOneToOne)
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
    DEBOUTLN (cerr, "test= " << test2);
#endif

  if (degree (Pi [factors.length() - 2], x) > 0)
    E= F[j] - Pi [factors.length() - 2] [j];
  else
    E= F[j];

  CFArray buf= CFArray (diophant.length());

  // actual lifting
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

  // update Pi [0]
  int degBuf0= degree (bufFactors[0], x);
  int degBuf1= degree (bufFactors[1], x);
  if (degBuf0 > 0 && degBuf1 > 0)
  {
    M (j + 1, 1)= mulMod (bufFactors[0] [j], bufFactors[1] [j], MOD);
    if (j + 2 <= M.rows())
      M (j + 2, 1)= mulMod (bufFactors[0] [j + 1], bufFactors[1] [j + 1], MOD);
  }
  CanonicalForm uIZeroJ;

  if (degBuf0 > 0 && degBuf1 > 0)
    uIZeroJ= mulMod (bufFactors[0] [0], buf[1], MOD) +
             mulMod (bufFactors[1] [0], buf[0], MOD);
  else if (degBuf0 > 0)
    uIZeroJ= mulMod (buf[0], bufFactors[1], MOD);
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
    while (one.hasTerms() && one.exp() > j) one++;
    while (two.hasTerms() && two.exp() > j) two++;
    for (k= 1; k <= (int) ceil (j/2.0); k++)
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

    if (degPi > 0 && degBuf > 0)
      uIZeroJ= mulMod (Pi[l -1] [0], buf[l + 1], MOD) +
               mulMod (uIZeroJ, bufFactors[l+1] [0], MOD);
    else if (degPi > 0)
      uIZeroJ= mulMod (uIZeroJ, bufFactors[l + 1], MOD);
    else if (degBuf > 0)
      uIZeroJ= mulMod (Pi[l - 1], buf[1], MOD);
    else
      uIZeroJ= 0;

    Pi [l] += xToJ*uIZeroJ;

    one= bufFactors [l + 1];
    two= Pi [l - 1];
    if (degBuf > 0 && degPi > 0)
    {
      while (one.hasTerms() && one.exp() > j) one++;
      while (two.hasTerms() && two.exp() > j) two++;
      for (k= 1; k <= (int) ceil (j/2.0); k++)
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
  return;
}

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

CFList
henselLift232 (const CFList& eval, const CFList& factors, int* l, CFList&
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
    henselStep2 (j.getItem(), buf, bufFactors, diophant, M, Pi, products, d, MOD, bad);
    if (bad)
      return CFList();
  }
  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}


CFList
henselLift2 (const CFList& F, const CFList& factors, const CFList& MOD, CFList&
            diophant, CFArray& Pi, CFMatrix& M, const int lOld, int&
            lNew, const CFList& LCs1, const CFList& LCs2, bool& bad)
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
    henselStep2 (F.getLast(), buf, bufFactors, diophant, M, Pi, products, d, MOD, bad);
    if (bad)
      return CFList();
  }

  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}

CFList
henselLift2 (const CFList& eval, const CFList& factors, int* l, const int
             lLength, bool sort, const CFList& LCs1, const CFList& LCs2,
             const CFArray& Pi, const CFList& diophant, bool& bad)
{
  CFList bufDiophant= diophant;
  CFList buf= factors;
  if (sort)
    sortList (buf, Variable (1));
  CFArray bufPi= Pi;
  CFMatrix M= CFMatrix (l[1], factors.length());
  CFList result= henselLift232(eval, buf, l, bufDiophant, bufPi, M, LCs1, LCs2,
                               bad);
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
    result= henselLift2 (bufEval, result, MOD, bufDiophant, bufPi, M, l[i - 1],
                         l[i], bufLCs1, bufLCs2, bad);
    if (bad)
      return CFList();
    MOD.append (power (Variable (i + 2), l[i]));
    bufEval.removeFirst();
    bufLCs1.removeFirst();
    bufLCs2.removeFirst();
  }
  return result;
}

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

  diophant= diophantine (bufF, bufFactors2);

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
  for (CFListIterator k= factors; k.hasItem(); k++)
    products.append (bufF/k.getItem());

  CFList MOD= CFList (power (v, liftBound));
  MOD.insert (yToL);
  for (int d= 1; d < liftBound; d++)
  {
    henselStep2 (F, factors, bufFactors, diophant, M, Pi, products, d, MOD, bad);
    if (bad)
      return CFList();
  }

  CFList result;
  for (i= 0; i < factors.length(); i++)
    result.append (bufFactors[i]);
  return result;
}

CFList
nonMonicHenselLift (const CFList& F, const CFList& factors, const CFList& LCs,
                    CFList& diophant, CFArray& Pi, CFMatrix& M, const int lOld,
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

  for (int d= 1; d < lNew; d++)
  {
    henselStep2 (F.getLast(), factors, bufFactors, diophant, M, Pi, products, d,
                 MOD, noOneToOne);
    if (noOneToOne)
      return CFList();
  }

  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}

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

  CFList result=
  nonMonicHenselLift23 (eval.getFirst(), factors, LCs [0], diophant, bufPi,
                        liftBound[1], liftBound[0], noOneToOne);

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
    result= nonMonicHenselLift (bufEval, result, LCs [i-1], diophant, bufPi, M,
                                liftBound[i-1], liftBound[i], MOD, noOneToOne);
    if (noOneToOne)
      return result;
    MOD.append (power (Variable (i + 2), liftBound[i]));
    bufEval.removeFirst();
  }

  return result;
}

#endif
/* HAVE_NTL */

