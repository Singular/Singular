/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgFuncUtil.cc
 *
 * This file provides utility functions to factorize polynomials over alg.
 * function fields
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "cf_assert.h"

#include "canonicalform.h"
#include "facAlgFuncUtil.h"
#include "cfCharSetsUtil.h"
#include "cf_random.h"
#include "cf_irred.h"
#include "cf_algorithm.h"
#include "cf_util.h"
#include "cf_iter.h"

CFFList
append (const CFFList & Inputlist, const CFFactor & TheFactor)
{
  CFFList Outputlist;
  CFFactor copy;
  CFFListIterator i;
  int exp=0;

  for (i= Inputlist; i.hasItem() ; i++)
  {
    copy= i.getItem();
    if (copy.factor() == TheFactor.factor())
      exp += copy.exp();
    else
      Outputlist.append(copy);
  }
  Outputlist.append (CFFactor (TheFactor.factor(), exp + TheFactor.exp()));
  return Outputlist;
}

CFFList
merge (const CFFList & Inputlist1, const CFFList & Inputlist2)
{
  CFFList Outputlist;
  CFFListIterator i;

  for (i= Inputlist1; i.hasItem(); i++)
    Outputlist= append (Outputlist, i.getItem());
  for (i= Inputlist2; i.hasItem() ; i++)
    Outputlist= append (Outputlist, i.getItem());

  return Outputlist;
}

Varlist
varsInAs (const Varlist & uord, const CFList & Astar)
{
  Varlist output;
  CanonicalForm elem;
  Variable x;

  for (VarlistIterator i= uord; i.hasItem(); i++)
  {
    x= i.getItem();
    for (CFListIterator j= Astar; j.hasItem(); j++ )
    {
      elem= j.getItem();
      if (degree (elem, x) > 0) // x actually occures in Astar
      {
        output.append (x);
        break;
      }
    }
  }
  return output;
}

// generate an irreducible poly of degree degOfExt over F_p
CanonicalForm
generateMipo (int degOfExt)
{
#ifndef HAVE_NTL
  FFRandom gen;
  return find_irreducible (degOfExt, gen, Variable (1));
#else
  return randomIrredpoly (degOfExt, Variable (1));
#endif
}

CanonicalForm alg_lc (const CanonicalForm & f)
{
  if (f.level()>0)
  {
    return alg_lc(f.LC());
  }

  return f;
}

CanonicalForm alg_LC (const CanonicalForm& f, int lev)
{
  CanonicalForm result= f;
  while (result.level() > lev)
    result= LC (result);
  return result;
}


CanonicalForm
subst (const CanonicalForm& f, const CFList& a, const CFList& b,
       const CanonicalForm& Rstar, bool isFunctionField)
{
  if (isFunctionField)
    ASSERT ((a.length() - 1)*4 == b.length() || (a.length() == 1 && b.length() == 2), "wrong length of lists");
  else
    ASSERT ((a.length() - 1)*2 == b.length() || (a.length() == 1 && b.length() == 1), "lists of equal length expected");
  CFListIterator j= b;
  CanonicalForm result= f, tmp, powj, tmp3;
  CFListIterator i= a;
  CanonicalForm tmp1= i.getItem();
  i++;
  CanonicalForm tmp2= j.getItem();
  j++;
  for (;i.hasItem() && j.hasItem(); i++, j++)
  {
    if (!isFunctionField)
    {
      result= result (j.getItem(), i.getItem().mvar());
      result= result (tmp2, tmp1.mvar());
      tmp1= i.getItem();
      j++;
      if (j.hasItem())
        tmp2= j.getItem();
    }
    else
    {
        tmp= j.getItem();
        j++;
        tmp3= j.getItem();
        j++;
        powj= power (j.getItem(), degree (result, i.getItem().mvar()));
        result= evaluate (result, tmp3, j.getItem(), powj, i.getItem().mvar());

        if (fdivides (powj, result, tmp3))
          result= tmp3;

        result /= vcontent (result, Variable (i.getItem().level() + 1));

        powj= power (tmp, degree (result, tmp1.mvar()));
        result= evaluate (result, tmp2, tmp, powj, tmp1.mvar());

        if (fdivides (powj, result, tmp))
          result= tmp;

        result /= vcontent (result, Variable (tmp1.level() + 1));
        tmp1= i.getItem();
        j++;
        if (j.hasItem())
          tmp2= j.getItem();
    }
  }
  result= Prem (result, CFList (Rstar));
  result /= vcontent (result, Variable (Rstar.level() + 1));
  return result;
}

CanonicalForm
backSubst (const CanonicalForm& F, const CFList& a, const CFList& b)
{
  ASSERT (a.length() == b.length() - 1, "wrong length of lists in backSubst");
  CanonicalForm result= F;
  Variable tmp;
  CFList tmp2= b;
  tmp= tmp2.getLast().mvar();
  tmp2.removeLast();
  for (CFListIterator iter= a; iter.hasItem(); iter++)
  {
    result= result (tmp+iter.getItem()*tmp2.getLast().mvar(), tmp);
    tmp= tmp2.getLast().mvar();
    tmp2.removeLast();
  }
  return result;
}

void deflateDegree (const CanonicalForm & F, int & pExp, int n)
{
  if (n == 0 || n > F.level())
  {
    pExp= -1;
    return;
  }
  if (F.level() == n)
  {
    ASSERT (F.deriv().isZero(), "derivative of F is not zero");
    CFIterator i= F;
    int g= 0;
    for (; i.hasTerms(); i++)
        g= igcd (g, i.exp());

    int count= 0;
    int p= getCharacteristic();
    while ((g >= p) && (g != 0) && (g % p == 0))
    {
      g /= p;
      count++;
    }
    pExp= count;
  }
  else
  {
    CFIterator i= F;
    deflateDegree (i.coeff(), pExp, n);
    i++;
    int tmp= pExp;
    for (; i.hasTerms(); i++)
    {
      deflateDegree (i.coeff(), pExp, n);
      if (tmp == -1)
        tmp= pExp;
      else if (tmp != -1 && pExp != -1)
        pExp= (pExp < tmp) ? pExp : tmp;
      else
        pExp= tmp;
    }
  }
}

CanonicalForm deflatePoly (const CanonicalForm & F, int exp)
{
  if (exp == 0)
    return F;
  int p= getCharacteristic();
  int pToExp= ipower (p, exp);
  Variable x=F.mvar();
  CanonicalForm result= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
    result += i.coeff()*power (x, i.exp()/pToExp);
  return result;
}

CanonicalForm deflatePoly (const CanonicalForm & F, int exps, int n)
{
  if (n == 0 || exps <= 0 || F.level() < n)
    return F;
  if (F.level() == n)
    return deflatePoly (F, exps);
  else
  {
    CanonicalForm result= 0;
    for (CFIterator i= F; i.hasTerms(); i++)
      result += deflatePoly (i.coeff(), exps, n)*power(F.mvar(), i.exp());
    return result;
  }
}

CanonicalForm inflatePoly (const CanonicalForm & F, int exp)
{
  if (exp == 0)
    return F;
  int p= getCharacteristic();
  int pToExp= ipower (p, exp);
  Variable x=F.mvar();
  CanonicalForm result= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
    result += i.coeff()*power (x, i.exp()*pToExp);
  return result;
}

CanonicalForm inflatePoly (const CanonicalForm & F, int exps, int n)
{
  if (n == 0 || exps <= 0 || F.level() < n)
    return F;
  if (F.level() == n)
    return inflatePoly (F, exps);
  else
  {
    CanonicalForm result= 0;
    for (CFIterator i= F; i.hasTerms(); i++)
      result += inflatePoly (i.coeff(), exps, n)*power(F.mvar(), i.exp());
    return result;
  }
}

void
multiplicity (CFFList& factors, const CanonicalForm& F, const CFList& as)
{
  CanonicalForm G= F;
  Variable x= F.mvar();
  CanonicalForm q, r;
  int count= -1;
  for (CFFListIterator iter=factors; iter.hasItem(); iter++)
  {
    count= -1;
    if (iter.getItem().factor().inCoeffDomain())
      continue;
    while (1)
    {
      psqr (G, iter.getItem().factor(), q, r, x);

      q= Prem (q, as);
      r= Prem (r, as);
      if (!r.isZero())
        break;
      count++;
      G= q;
    }
    iter.getItem()= CFFactor (iter.getItem().factor(),
                              iter.getItem().exp() + count);
  }
}

int hasAlgVar (const CanonicalForm &f, const Variable &v)
{
  if (f.inBaseDomain())
    return 0;
  if (f.inCoeffDomain())
  {
    if (f.mvar() == v)
      return 1;
    return hasAlgVar (f.LC(), v);
  }
  if (f.inPolyDomain())
  {
    if (hasAlgVar(f.LC(), v))
      return 1;
    for (CFIterator i= f; i.hasTerms(); i++)
    {
      if (hasAlgVar (i.coeff(), v))
        return 1;
    }
  }
  return 0;
}

int hasVar (const CanonicalForm &f, const Variable &v)
{
  if (f.inBaseDomain())
    return 0;
  if (f.inCoeffDomain())
  {
    if (f.mvar() == v)
      return 1;
    return hasAlgVar (f.LC(), v);
  }
  if (f.inPolyDomain())
  {
    if (f.mvar() == v)
      return 1;
    if (hasVar (f.LC(), v))
      return 1;
    for (CFIterator i= f; i.hasTerms(); i++)
    {
      if (hasVar (i.coeff(), v))
        return 1;
    }
  }
  return 0;
}

int hasAlgVar (const CanonicalForm &f)
{
  if (f.inBaseDomain())
    return 0;
  if (f.inCoeffDomain())
  {
    if (f.level() != 0)
      return 1;
    return hasAlgVar(f.LC());
  }
  if (f.inPolyDomain())
  {
    for (CFIterator i= f; i.hasTerms(); i++)
    {
      if (hasAlgVar (i.coeff()))
        return 1;
    }
  }
  return 0;
}

/// pseudo division of f and g wrt. x s.t. multiplier*f=q*g+r
void
psqr (const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q,
      CanonicalForm & r, CanonicalForm& multiplier, const Variable& x)
{
    ASSERT( x.level() > 0, "type error: polynomial variable expected" );
    ASSERT( ! g.isZero(), "math error: division by zero" );

    // swap variables such that x's level is larger or equal
    // than both f's and g's levels.
    Variable X;
    if (f.level() > g.level())
      X= f.mvar();
    else
      X= g.mvar();
    if (X.level() < x.level())
      X= x;
    CanonicalForm F= swapvar (f, x, X);
    CanonicalForm G= swapvar (g, x, X);

    // now, we have to calculate the pseudo remainder of F and G
    // w.r.t. X
    int fDegree= degree (F, X);
    int gDegree= degree (G, X);
    if (fDegree < 0 || fDegree < gDegree)
    {
      q= 0;
      r= f;
    }
    else
    {
      CanonicalForm LCG= LC (G, X);
      multiplier= power (LCG, fDegree - gDegree + 1);
      divrem (multiplier*F, G, q, r);
      q= swapvar (q, x, X);
      r= swapvar (r, x, X);
    }
}

CanonicalForm
Sprem (const CanonicalForm &f, const CanonicalForm &g, CanonicalForm & m,
       CanonicalForm & q )
{
  CanonicalForm ff, gg, l, test, retvalue;
  int df, dg,n;
  bool reord;
  Variable vf, vg, v;

  if ((vf = f.mvar()) < (vg = g.mvar()))
  {
    m= 0;
    q= 0;
    return f;
  }
  else
  {
    if ( vf == vg )
    {
      ff= f;
      gg= g;
      reord= false;
      v= vg; // == x
    }
    else
    {
      v= Variable (f.level() + 1);
      ff= swapvar (f, vg, v); // == r
      gg= swapvar (g, vg, v); // == v
      reord=true;
    }
    dg= degree (gg, v); // == dv
    df= degree (ff, v); // == dr
    if (dg <= df)
    {
      l= LC (gg);
      gg= gg - LC(gg)*power(v,dg);
    }
    else
      l = 1;
    n= 0;
    while ((dg <= df) && (!ff.isZero()))
    {
      test= gg*LC (ff)*power (v, df - dg);
      if (df == 0)
        ff= 0;
      else
        ff= ff - LC(ff)*power(v,df);
      ff= l*ff - test;
      df= degree (ff, v);
      n++;
    }

    if (reord)
      retvalue= swapvar (ff, vg, v);
    else
      retvalue= ff;

    m= power (l, n);
    if (fdivides (g, m*f - retvalue))
      q= (m*f - retvalue)/g;
    else
      q= 0;
    return retvalue;
  }
}

CanonicalForm
divide (const CanonicalForm & ff, const CanonicalForm & f, const CFList & as)
{
  CanonicalForm r, m, q;

  if (f.inCoeffDomain())
  {
    bool isRat= isOn(SW_RATIONAL);
    if (getCharacteristic() == 0)
      On(SW_RATIONAL);
    q= ff/f;
    if (!isRat && getCharacteristic() == 0)
      Off(SW_RATIONAL);
  }
  else
    r= Sprem (ff, f, m, q);

  r= Prem (q, as);
  return r;
}

// check if polynomials in Astar define a separable extension
bool
isInseparable (const CFList & Astar)
{
  CanonicalForm elem;

  if (Astar.length() == 0)
    return false;
  for (CFListIterator i= Astar; i.hasItem(); i++)
  {
    elem= i.getItem();
    if (elem.deriv().isZero())
      return true;
  }
  return false;
}

// calculate big enough extension for finite fields
// Idea: first calculate k, such that q^k > S (->thesis)
// Second, search k with gcd(k,m_i)=1, where m_i is the degree of the i'th
// minimal polynomial. Then the minpoly f_i remains irrd. over q^k and we
// have enough elements to plug in.
int
getDegOfExt (IntList & degreelist, int n)
{
  int charac= getCharacteristic();
  setCharacteristic(0); // need it for k !
  int k= 1, m= 1, length= degreelist.length();
  IntListIterator i;

  for (i= degreelist; i.hasItem(); i++)
     m= m*i.getItem();
  int q= charac;
  while (q <= ((n*m)*(n*m)/2))
  {
    k= k+1;
    q= q*charac;
  }
  int l= 0;
  do
  {
    for (i= degreelist; i.hasItem(); i++)
    {
      l= l + 1;
      if (igcd (k, i.getItem()) == 1)
      {
        if (l == length)
        {
          setCharacteristic (charac);
          return k;
        }
      }
      else
        break;
    }
    k= k + 1;
    l= 0;
  }
  while (1);
}

CanonicalForm
QuasiInverse (const CanonicalForm& f, const CanonicalForm& g,
              const Variable& x)
{
  CanonicalForm pi, pi1, q, t0, t1, Hi, bi, pi2;
  bool isRat= isOn (SW_RATIONAL);
  pi= f;
  pi1= g;
  if (isRat)
  {
    pi *= bCommonDen (pi);
    pi1 *= bCommonDen (pi1);
  }
  CanonicalForm m,tmp;
  if (isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);

  pi= pi/content (pi,x);
  pi1= pi1/content (pi1,x);

  t0= 0;
  t1= 1;
  bi= 1;

  int delta= degree (f, x) - degree (g, x);
  Hi= power (LC (pi1, x), delta);
  if ( (delta+1) % 2 )
      bi = 1;
  else
      bi = -1;

  while (degree (pi1,x) > 0)
  {
    psqr (pi, pi1, q, pi2, m, x);
    pi2 /= bi;

    tmp= t1;
    t1= t0*m - t1*q;
    t0= tmp;
    t1 /= bi;
    pi= pi1;
    pi1= pi2;
    if (degree (pi1, x) > 0)
    {
      delta= degree (pi, x) - degree (pi1, x);
      if ((delta + 1) % 2)
        bi= LC (pi, x)*power (Hi, delta);
      else
        bi= -LC (pi, x)*power (Hi, delta);
      Hi= power (LC (pi1, x), delta)/power (Hi, delta - 1);
    }
  }
  t1 /= gcd (pi1, t1);
  if (isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  return t1;
}

CanonicalForm
evaluate (const CanonicalForm& f, const CanonicalForm& g,
          const CanonicalForm& h, const CanonicalForm& powH)
{
  if (f.inCoeffDomain())
    return f;
  CFIterator i= f;
  int lastExp = i.exp();
  CanonicalForm result = i.coeff()*powH;
  i++;
  while (i.hasTerms())
  {
    int i_exp= i.exp();
    if ((lastExp - i_exp) == 1)
    {
      result *= g;
      result /= h;
    }
    else
    {
      result *= power (g, lastExp - i_exp);
      result /= power (h, lastExp - i_exp);
    }
    result += i.coeff()*powH;
    lastExp = i_exp;
    i++;
  }
  if (lastExp != 0)
  {
    result *= power (g, lastExp);
    result /= power (h, lastExp);
  }
  return result;
}


/// evaluate f at g/h at v such that powH*f is integral i.e. powH is assumed to be h^degree(f,v)
CanonicalForm
evaluate (const CanonicalForm& f, const CanonicalForm& g,
          const CanonicalForm& h, const CanonicalForm& powH,
          const Variable& v)
{
  if (f.inCoeffDomain())
  {
    return f*powH;
  }

  Variable x = f.mvar();
  if ( v > x )
    return f*powH;
  else  if ( v == x )
    return evaluate (f, g, h, powH);

  // v is less than main variable of f
  CanonicalForm result= 0;
  for (CFIterator i= f; i.hasTerms(); i++)
    result += evaluate (i.coeff(), g, h, powH, v)*power (x, i.exp());
  return result;
}


