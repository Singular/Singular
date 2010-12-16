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

#ifdef HAVE_NTL
#include <NTL/lzz_pEX.h>
#include "NTLconvert.h"

static inline
CanonicalForm
mulNTL (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() || G.inCoeffDomain())
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

static inline
CanonicalForm
modNTL (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() && G.isUnivariate())
    return F;
  else if (F.inCoeffDomain() && G.inCoeffDomain())
    return mod (F, G);
  else if (F.isUnivariate() && G.inCoeffDomain())
    return mod (F,G);

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

static inline
CanonicalForm
divNTL (const CanonicalForm& F, const CanonicalForm& G)
{
  if (F.inCoeffDomain() && G.isUnivariate())
    return F;
  else if (F.inCoeffDomain() && G.inCoeffDomain())
    return div (F, G);
  else if (F.isUnivariate() && G.inCoeffDomain())
    return div (F,G);

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

/*static inline
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
  Variable y= F.mvar();
  Variable x= Variable (1);
  A= swapvar (A, x, y);
  CanonicalForm result= 0;
  CFIterator i= A;
  while (d - i.exp() < 0)
    i++;

  for (; i.hasTerms() && (d - i.exp() >= 0); i++)
    result += swapvar (i.coeff(),x,y)*power (x, d - i.exp());
  return result;
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
  if (degB <= 1)
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

  if (degB <= 1)
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
  CFList splitB= split (B, m, x);
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

  if (degB == 0)
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
        if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0] [k] + one.coeff()), (bufFactors[1] [k] +
                     two.coeff())) - M (k + 1, 1) - M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0] [k] + one.coeff()), bufFactors[1] [k]) -
                     M (k + 1, 1);
          one++;
        }
        else if (two.exp() == j - k + 1)
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
    if (two.exp() == j + 1)
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
          if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l + 1] [k] + one.coeff()), (Pi[l - 1] [k] +
                       two.coeff())) - M (k + 1, l + 1) - M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l + 1] [k] + one.coeff()), Pi[l - 1] [k]) -
                       M (k + 1, l + 1);
            one++;
          }
          else if (two.exp() == j - k + 1)
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
              CFList& diophant, CFMatrix& M)
{
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
  for (j; j.hasItem(); j++, i++)
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
    CanonicalForm b;
    for (k= 0; k < factors.length(); k++) //TODO compute b's faster
    {
      b= 1;
      if (fdivides (bufFactors[k], F))
        b= F/bufFactors[k];
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
  CanonicalForm b;
  CFList buf= M;
  buf.removeLast();
  buf.append (yToD);
  for (k= 0; k < factors.length(); k++) //TODO compute b's faster
  {
    b= 1;
    if (fdivides (bufFactors[k], F))
      b= F/bufFactors[k];
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
        if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    (bufFactors[1] [k] + two.coeff()), MOD) - M (k + 1, 1) -
                    M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.exp() == j - k + 1)
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    bufFactors[1] [k], MOD) - M (k + 1, 1);
          one++;
        }
        else if (two.exp() == j - k + 1)
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
    if (two.exp() == j + 1)
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
          if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      (Pi[l - 1] [k] + two.coeff()), MOD) - M (k + 1, l + 1) -
                      M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.exp() == j - k + 1)
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      Pi[l - 1] [k], MOD) - M (k + 1, l + 1);
            one++;
          }
          else if (two.exp() == j - k + 1)
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
  int liftBound;
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
  for (i; i.hasItem(); i++, k++)
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
  for (i; i.hasItem(); i++, k++)
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
            lLength)
{
  CFList diophant;
  CFList buf= factors;
  buf.insert (LC (eval.getFirst(), 1));
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
    while (one.exp() > j) one++;
    while (two.exp() > j) two++;
    for (k= 1; k <= (int) ceil (j/2.0); k++)
    {
      if (k != j - k + 1)
      {
        if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0][k]+one.coeff()),(bufFactors[1][k] +
                      two.coeff())) - M (k + 1, 1) - M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.exp() == j - k + 1)
        {
          tmp[0] += mulNTL ((bufFactors[0][k]+one.coeff()), bufFactors[1] [k]) -
                      M (k + 1, 1);
          one++;
        }
        else if (two.exp() == j - k + 1)
        {
          tmp[0] += mulNTL (bufFactors[0][k],(bufFactors[1][k] + two.coeff())) -
                    M (k + 1, 1);
          two++;
        }
      }
      else
        tmp[0] += M (k + 1, 1);
    }

    if (j + 2 <= M.rows())
    {
      if (degBuf0 >= j + 1 && degBuf1 >= j + 1)
        tmp [0] += mulNTL ((bufFactors [0] [j + 1]+ bufFactors [0] [0]),
                           (bufFactors [1] [j + 1] + bufFactors [1] [0]))
                           - M(1,1) - M (j + 2,1);
      else if (degBuf0 >= j + 1)
        tmp[0] += mulNTL (bufFactors [0] [j+1], bufFactors [1] [0]);
      else if (degBuf1 >= j + 1)
        tmp[0] += mulNTL (bufFactors [0] [0], bufFactors [1] [j + 1]);
    }
  }
  Pi [0] += tmp[0]*xToJ*F.mvar();

  /*// update Pi [l]
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
    if (two.exp() == j + 1)
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
          if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l + 1][k]+one.coeff()),(Pi[l-1] [k] +
                        two.coeff())) - M (k + 1, l + 1) - M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.exp() == j - k + 1)
          {
            tmp[l] += mulNTL ((bufFactors[l+1][k]+one.coeff()),Pi[l - 1] [k]) -
                        M (k + 1, l + 1);
            one++;
          }
          else if (two.exp() == j - k + 1)
          {
            tmp[l] += mulNTL (bufFactors[l+1][k],(Pi[l-1] [k] + two.coeff())) -
                      M (k + 1, l + 1);
            two++;
          }
        }
        else
          tmp[l] += M (k + 1, l + 1);
      }
    }
    Pi[l] += tmp[l]*xToJ*F.mvar();
  }*/
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
  diophant.removeFirst();
  CFListIterator iter= diophant;
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
             const CFList& products, const CFList& M, const CanonicalForm& E)
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
                                      bufE);

  CanonicalForm e= E;
  CFListIterator j= products;
  for (CFListIterator i= recDiophantine; i.hasItem(); i++, j++)
    e -= mulMod (i.getItem(), j.getItem(), M);

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
                                   coeffE);
      CFListIterator l= products;
      for (j= recDiophantine; j.hasItem(); j++, k++, l++)
      {
        k.getItem() += j.getItem()*power (y, i);
        e -= mulMod (j.getItem()*power (y, i), l.getItem(), M);
      }
    }
    if (e.isZero())
      break;
  }
  return result;
}

// non monic case
static inline
CFList
multiRecDiophantine2 (const CanonicalForm& F, const CFList& factors,
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
  CanonicalForm b;
  CFList buf= M;
  buf.removeLast();
  buf.append (yToD);
  for (k= 0; k < factors.length(); k++) //TODO compute b's faster
  {
    b= 1;
    if (fdivides (bufFactors[k], F))
      b= F/bufFactors[k];
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
  result= recResult;
  CanonicalForm g;
  buf.removeLast();
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
      CFList recDiophantine;
      CFList buf2, buf3;
      buf2= factors;
      buf3= p;
      for (CFListIterator iii= buf2; iii.hasItem(); iii++)
        iii.getItem()= mod (iii.getItem(), y);
      for (CFListIterator iii= buf3; iii.hasItem(); iii++)
        iii.getItem()= mod (iii.getItem(), y);
      recDiophantine= diophantine (recResult, buf2, buf3, buf, coeffE);
      CFListIterator iter= recDiophantine;
      for (; j.hasItem(); j++, k++, l++, ii++, iter++)
      {
        k.getItem() += iter.getItem()*power (y, i);
        e -= mulMod (iter.getItem()*power (y, i), l.getItem(), M);
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

// so far only tested for two! factor Hensel lifting
static inline
void
henselStep2 (const CanonicalForm& F, const CFList& factors, CFArray& bufFactors,
            const CFList& diophant, CFMatrix& M, CFArray& Pi,
            const CFList& products, int j, const CFList& MOD)
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
  CFList diophantine2= diophantine (diophant, factors, products, MOD, E);

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
    while (one.exp() > j) one++;
    while (two.exp() > j) two++;
    for (k= 1; k <= (int) ceil (j/2.0); k++)
    {
      if (k != j - k + 1)
      {
        if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    (bufFactors[1] [k] + two.coeff()), MOD) - M (k + 1, 1) -
                    M (j - k + 2, 1);
          one++;
          two++;
        }
        else if (one.exp() == j - k + 1)
        {
          tmp[0] += mulMod ((bufFactors[0] [k] + one.coeff()),
                    bufFactors[1] [k], MOD) - M (k + 1, 1);
          one++;
        }
        else if (two.exp() == j - k + 1)
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

    if (j + 2 <= M.rows())
    {
      if (degBuf0 >= j + 1 && degBuf1 >= j + 1)
        tmp [0] += mulMod ((bufFactors [0] [j + 1]+ bufFactors [0] [0]),
                           (bufFactors [1] [j + 1] + bufFactors [1] [0]), MOD)
                   - M(1,1) - M (j + 2,1);
      else if (degBuf0 >= j + 1)
        tmp[0] += mulMod (bufFactors [0] [j+1], bufFactors [1] [0], MOD);
      else if (degBuf1 >= j + 1)
        tmp[0] += mulMod (bufFactors [0] [0], bufFactors [1] [j + 1], MOD);
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
    if (two.exp() == j + 1)
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
          if (one.exp() == j - k + 1 && two.exp() == j - k + 1)
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      (Pi[l - 1] [k] + two.coeff()), MOD) - M (k + 1, l + 1) -
                      M (j - k + 2, l + 1);
            one++;
            two++;
          }
          else if (one.exp() == j - k + 1)
          {
            tmp[l] += mulMod ((bufFactors[l + 1] [k] + one.coeff()),
                      Pi[l - 1] [k], MOD) - M (k + 1, l + 1);
            one++;
          }
          else if (two.exp() == j - k + 1)
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

// so far only for two factor Hensel lifting
CFList
henselLift232 (const CFList& eval, const CFList& factors, int* l, CFList&
              diophant, CFArray& Pi, CFMatrix& M, const CFList& LCs1,
              const CFList& LCs2)
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
      products.append (M (1, 1)/bufFactors[i] [0]);
    else
      products.append (M (1, 1)/bufFactors[i]);
  }

  for (int d= 1; d < l[1]; d++)
    henselStep2 (j.getItem(), buf, bufFactors, diophant, M, Pi, products, d, MOD);
  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}


CFList
henselLift2 (const CFList& F, const CFList& factors, const CFList& MOD, CFList&
            diophant, CFArray& Pi, CFMatrix& M, const int lOld, int&
            lNew, const CFList& LCs1, const CFList& LCs2)
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
  for (int i= 0; i < bufFactors.size(); i++)
  {
    if (degree (bufFactors[i], y) > 0)
    {
      ASSERT (fdivides (bufFactors[i][0], M (1, 1)), "expected exact division");
      products.append (M (1, 1)/bufFactors[i] [0]);
    }
    else
    {
      ASSERT (fdivides (bufFactors[i], M (1, 1)), "expected exact division");
      products.append (M (1, 1)/bufFactors[i]);
    }
  }

  for (int d= 1; d < lNew; d++)
    henselStep2 (F.getLast(),buf,bufFactors, diophant, M, Pi, products, d, MOD);

  CFList result;
  for (k= 0; k < factors.length(); k++)
    result.append (bufFactors[k]);
  return result;
}

// so far only for two! factor Hensel lifting
CFList
henselLift2 (const CFList& eval, const CFList& factors, int* l, const int
             lLength, bool sort, const CFList& LCs1, const CFList& LCs2,
             const CFArray& Pi, const CFList& diophant)
{
  CFList bufDiophant= diophant;
  CFList buf= factors;
  if (sort)
    sortList (buf, Variable (1));
  CFArray bufPi= Pi;
  CFMatrix M= CFMatrix (l[1], factors.length());
  CFList result= henselLift232(eval, buf, l, bufDiophant, bufPi, M, LCs1, LCs2);
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
                         l[i], bufLCs1, bufLCs2);
    MOD.append (power (Variable (i + 2), l[i]));
    bufEval.removeFirst();
    bufLCs1.removeFirst();
    bufLCs2.removeFirst();
  }
  return result;
}

#endif
/* HAVE_NTL */

