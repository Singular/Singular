/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgFunc.cc
 *
 * This file provides functions to factorize polynomials over alg. function
 * fields
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 * 
 * ABSTRACT: Descriptions can be found in B. Trager "Algebraic Factoring and
 * Rational Function Integration" and A. Steel "Conquering Inseparability:
 * Primary decomposition and multivariate factorization over algebraic function
 * fields of positive characteristic"
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "cf_assert.h"
#include "debug.h"

#include "algext.h"
#include "cf_generator.h"
#include "cf_iter.h"
#include "cf_util.h"
#include "cf_algorithm.h"
#include "cf_map.h"
#include "cfModResultant.h"
#include "cfCharSets.h"
#include "facAlgFunc.h"
#include "facAlgFuncUtil.h"

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

CanonicalForm
alg_content (const CanonicalForm& f, const CFList& as)
{
  if (!f.inCoeffDomain())
  {
    CFIterator i= f;
    CanonicalForm result= abs (i.coeff());
    i++;
    while (i.hasTerms() && !result.isOne())
    {
      result= alg_gcd (i.coeff(), result, as);
      i++;
    }
    return result;
  }

  return abs (f);
}

CanonicalForm
alg_gcd (const CanonicalForm & fff, const CanonicalForm &ggg, const CFList &as)
{
  if (fff.inCoeffDomain() || ggg.inCoeffDomain())
    return 1;
  CanonicalForm f=fff;
  CanonicalForm g=ggg;
  f=Prem(f,as);
  g=Prem(g,as);
  if ( f.isZero() )
  {
    if ( g.lc().sign() < 0 ) return -g;
    else                     return g;
  }
  else  if ( g.isZero() )
  {
    if ( f.lc().sign() < 0 ) return -f;
    else                     return f;
  }

  int v= as.getLast().level();
  if (f.level() <= v || g.level() <= v)
    return 1;

  CanonicalForm res;

  // does as appear in f and g ?
  bool has_alg_var=false;
  for ( CFListIterator j=as;j.hasItem(); j++ )
  {
    Variable v=j.getItem().mvar();
    if (hasVar (f, v))
      has_alg_var=true;
    if (hasVar (g, v))
      has_alg_var=true;
  }
  if (!has_alg_var)
  {
    /*if ((hasAlgVar(f))
    || (hasAlgVar(g)))
    {
      Varlist ord;
      for ( CFListIterator j=as;j.hasItem(); j++ )
        ord.append(j.getItem().mvar());
      res=algcd(f,g,as,ord);
    }
    else*/
    if (!hasAlgVar (f) && !hasAlgVar (g))
      return res=gcd(f,g);
  }

  int mvf=f.level();
  int mvg=g.level();
  if (mvg > mvf)
  {
    CanonicalForm tmp=f; f=g; g=tmp;
    int tmp2=mvf; mvf=mvg; mvg=tmp2;
  }
  if (g.inBaseDomain() || f.inBaseDomain())
    return CanonicalForm(1);

  CanonicalForm c_f= alg_content (f, as);

  if (mvf != mvg)
  {
    res= alg_gcd (g, c_f, as);
    return res;
  }
  Variable x= f.mvar();

  // now: mvf==mvg, f.level()==g.level()
  CanonicalForm c_g= alg_content (g, as);

  int delta= degree (f) - degree (g);

  f= divide (f, c_f, as);
  g= divide (g, c_g, as);

  // gcd of contents
  CanonicalForm c_gcd= alg_gcd (c_f, c_g, as);
  CanonicalForm tmp;

  if (delta < 0)
  {
    tmp= f;
    f= g;
    g= tmp;
    delta= -delta;
  }

  CanonicalForm r= 1;

  while (degree (g, x) > 0)
  {
    r= Prem (f, g);
    r= Prem (r, as);
    if (!r.isZero())
    {
      r= divide (r, alg_content (r,as), as);
      r /= vcontent (r,Variable (v+1));
    }
    f= g;
    g= r;
  }

  if (degree (g, x) == 0)
    return c_gcd;

  c_f= alg_content (f, as);

  f= divide (f, c_f, as);

  f *= c_gcd;
  f /= vcontent (f, Variable (v+1));

  return f;
}

static CanonicalForm
resultante (const CanonicalForm & f, const CanonicalForm& g, const Variable & v)
{
  bool on_rational = isOn(SW_RATIONAL);
  if (!on_rational && getCharacteristic() == 0)
    On(SW_RATIONAL);
  CanonicalForm cd = bCommonDen( f );
  CanonicalForm fz = f * cd;
  cd = bCommonDen( g );
  CanonicalForm gz = g * cd;
  if (!on_rational && getCharacteristic() == 0)
    Off(SW_RATIONAL);
  CanonicalForm result;
  if (getCharacteristic() == 0)
    result= resultantZ (fz, gz,v);
  else
    result= resultant (fz,gz,v);

  return result;
}

// Trager's square free norm algorithm:
// f a separable polynomial over K (alpha),
// alpha is defined by the minimal polynomial Palpha
// K need to contain more than S elements (S is defined in Messollen's thesis;
// see also getDegOfExt)
static CFFList
sqrf_norm_sub (const CanonicalForm & f, const CanonicalForm & PPalpha,
               CFGenerator & myrandom, CanonicalForm & s,  CanonicalForm & g,
               CanonicalForm & R)
{
  Variable y= PPalpha.mvar(),vf= f.mvar();
  CanonicalForm temp, Palpha= PPalpha, t;
  int sqfreetest= 0;
  CFFList testlist;
  CFFListIterator i;

  myrandom.reset();
  s= myrandom.item();
  g= f;
  R= CanonicalForm(0);

  // Norm, resultante taken with respect to y
  while (!sqfreetest)
  {
    R= resultante(Palpha, g, y);
    R= R* bCommonDen(R);
    R /= content (R);
    // sqfree check ; R is a polynomial in K[x]
    if (getCharacteristic() == 0)
    {
      temp= gcd (R, R.deriv(vf));
      if (degree(temp,vf) != 0 || temp == temp.genZero() )
        sqfreetest= 0;
      else
        sqfreetest= 1;
    }
    else
    {
      Variable X;
      testlist= sqrFree (R);

      if (testlist.getFirst().factor().inCoeffDomain())
        testlist.removeFirst();
      sqfreetest= 1;
      for (i= testlist; i.hasItem(); i++)
      {
        if (i.getItem().exp() > 1 && degree (i.getItem().factor(),R.mvar()) > 0)
        {
          sqfreetest= 0;
          break;
        }
      }
    }
    if (!sqfreetest)
    {
      myrandom.next();
      if (getCharacteristic() == 0)
        t= CanonicalForm (mapinto (myrandom.item()));
      else
        t= CanonicalForm (myrandom.item());
      s= t;
      g= f (f.mvar() - t*Palpha.mvar(), f.mvar());
    }
  }
  return testlist;
}

static CFFList
sqrf_norm( const CanonicalForm & f, const CanonicalForm & PPalpha,
           const Variable & Extension, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R)
{
  CFFList result;
  if (getCharacteristic() == 0)
  {
    IntGenerator myrandom;
    result= sqrf_norm_sub (f, PPalpha, myrandom, s, g, R);
  }
  else if (degree (Extension) > 0)
  {
    AlgExtGenerator myrandom (Extension);
    result= sqrf_norm_sub (f, PPalpha, myrandom, s, g, R);
  }
  else
  {
    FFGenerator myrandom;
    result= sqrf_norm_sub (f, PPalpha, myrandom, s, g, R);
  }
  return result;
}

// calculate a "primitive element"
// K must have more than S elements (-> getDegOfExt)
static CFList
simpleExtension (CFList& backSubst, const CFList & Astar,
                 const Variable & Extension, bool& isFunctionField,
                 CanonicalForm & R)
{
  CFList Returnlist, Bstar= Astar;
  CanonicalForm s, g, ra, rb, oldR, h, denra, denrb= 1;
  Variable alpha;
  CFList tmp;

  bool isRat= isOn (SW_RATIONAL);

  CFListIterator j;
  if (Astar.length() == 1)
  {
    R= Astar.getFirst();
    rb= R.mvar();
  }
  else
  {
    R=Bstar.getFirst();
    Bstar.removeFirst();
    for (CFListIterator i= Bstar; i.hasItem(); i++)
    {
      j= i;
      j++;
      if (getCharacteristic() == 0)
        Off (SW_RATIONAL);
      R /= icontent (R);
      if (getCharacteristic() == 0)
        On (SW_RATIONAL);
      oldR= R;
      //TODO normalize i.getItem over K(R)?
      (void) sqrf_norm (i.getItem(), R, Extension, s, g, R);

      backSubst.insert (s);

      if (getCharacteristic() == 0)
        Off (SW_RATIONAL);
      R /= icontent (R);

      if (getCharacteristic() == 0)
        On (SW_RATIONAL);

      if (!isFunctionField)
      {
        alpha= rootOf (R);
        h= replacevar (g, g.mvar(), alpha);
        if (getCharacteristic() == 0)
          On (SW_RATIONAL); //needed for GCD
        h= gcd (h, oldR);
        h /= Lc (h);
        ra= -h[0];
        ra= replacevar(ra, alpha, g.mvar());
        rb= R.mvar()-s*ra;
        for (; j.hasItem(); j++)
        {
          j.getItem()= j.getItem() (ra, oldR.mvar());
          j.getItem()= j.getItem() (rb, i.getItem().mvar());
        }
      }
      else
      {
        if (getCharacteristic() == 0)
          On (SW_RATIONAL);
        h= swapvar (g, g.mvar(), oldR.mvar());
        tmp= CFList (swapvar (R, g.mvar(), oldR.mvar()));
        h= alg_gcd (h, swapvar (oldR, g.mvar(), oldR.mvar()), tmp);
        CanonicalForm hh= replacevar (h, oldR.mvar(), alpha);

        CanonicalForm numinv, deninv;
        numinv= QuasiInverse (tmp.getFirst(), LC (h), tmp.getFirst().mvar());

        if (getCharacteristic() == 0)
          Off (SW_RATIONAL);
        h *= numinv;
        h= reduce (h, tmp.getFirst());
        deninv= LC(h);

        ra= -h[0];
        denra= gcd (ra, deninv);
        ra /= denra;
        denra= deninv/denra;
        denra= replacevar (denra, ra.mvar(), g.mvar());
        ra= replacevar(ra, ra.mvar(), g.mvar());
        rb= R.mvar()*denra-s*ra;
        denrb= denra;
        for (; j.hasItem(); j++)
        {
          CanonicalForm powdenra= power (denra, degree (j.getItem(),
                                                        oldR.mvar()));
          j.getItem()= evaluate (j.getItem(),ra, denra, powdenra, oldR.mvar());
          powdenra= power (denra, degree (j.getItem(), i.getItem().mvar()));
          j.getItem()= evaluate (j.getItem(), rb, denrb, powdenra,
                                 i.getItem().mvar());
        }
      }

      Returnlist.append (ra);
      if (isFunctionField)
        Returnlist.append (denra);
    }
  }
  Returnlist.append (rb);
  if (isFunctionField)
    Returnlist.append (denrb);

  if (isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  else if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);

  return Returnlist;
}

/// Trager's algorithm, i.e. convert to one field extension and
/// factorize over this field extension
static CFFList
Trager (const CanonicalForm & F, const CFList & Astar,
        const Variable & vminpoly, const CFList & as, bool isFunctionField)
{
  bool isRat= isOn (SW_RATIONAL);
  CFFList L, sqrfFactors, Factorlist, tmp;
  CFFListIterator iter, iter2;
  CanonicalForm R, Rstar, s, g, h, f= F;
  CFList substlist, backSubsts;

  substlist= simpleExtension (backSubsts, Astar, vminpoly, isFunctionField,
                              Rstar);

  f= subst (f, Astar, substlist, Rstar, isFunctionField);

  Variable alpha;
  if (!isFunctionField)
  {
    alpha= rootOf (Rstar);
    g= replacevar (f, Rstar.mvar(), alpha);

    if (!isRat && getCharacteristic() == 0)
      On (SW_RATIONAL);
    Factorlist= factorize (g, alpha);

    for (iter= Factorlist; iter.hasItem(); iter++)
    {
      h= iter.getItem().factor();
      if (!h.inCoeffDomain())
      {
        h= replacevar (h, alpha, Rstar.mvar());
        h *= bCommonDen(h);
        h= backSubst (h, backSubsts, Astar);
        h= Prem (h, as);
        L.append (CFFactor (h, iter.getItem().exp()));
      }
    }
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return L;
  }
  // after here we are over an extension of a function field

  // make quasi monic
  CFList Rstarlist= CFList (Rstar);
  int algExtLevel= Astar.getLast().level(); //highest level of algebraic variables
  CanonicalForm numinv;
  if (!isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  numinv= QuasiInverse (Rstar, alg_LC (f, algExtLevel), Rstar.mvar());

  f *= numinv;
  f= Prem (f, Rstarlist);
  f /= vcontent (f, Rstar.mvar());
  // end quasi monic

  if (degree (f) == 1)
  {
    f= backSubst (f, backSubsts, Astar);
    f *= bCommonDen (f);
    f= Prem (f, as);
    f /= vcontent (f, as.getFirst().mvar());

    return CFFList (CFFactor (f, 1));
  }

  sqrfFactors= sqrf_norm (f, Rstar, vminpoly, s, g, R);

  if (getCharacteristic() > 0)
  {
    if (sqrfFactors.getFirst().factor().inCoeffDomain())
      sqrfFactors.removeFirst();

    Variable X;
    for (iter= sqrfFactors; iter.hasItem(); iter++)
    {
      if (hasFirstAlgVar (iter.getItem().factor(), X))
      {
        // factorize over alg.extension with X
        tmp= factorize (iter.getItem().factor(), X);
      }
      else
      {
        // factorize over k
        tmp= factorize (iter.getItem().factor(), true);
      }
      if (tmp.getFirst().factor().inCoeffDomain())
        tmp.removeFirst();
      for (iter2= tmp; iter2.hasItem(); iter2++)
        Factorlist= append (Factorlist, iter2.getItem());
    }
  }
  else
    Factorlist= factorize (R, true);

  if (!Factorlist.getFirst().factor().inCoeffDomain())
    Factorlist.insert (CFFactor (1, 1));
  if (Factorlist.length() == 2 && Factorlist.getLast().exp() == 1)
  {
    f= backSubst (f, backSubsts, Astar);
    f *= bCommonDen (f);
    f= Prem (f, as);
    f /= vcontent (f, as.getFirst().mvar());

    L.append(CFFactor(f,1));
  }
  else
  {
    g= f;
    for (iter= Factorlist; iter.hasItem(); iter++)
    {
      CanonicalForm fnew= iter.getItem().factor();
      if (fnew.level() < Rstar.level()) //factor is a constant from the function field
        continue;
      else
      {
        fnew= fnew (g.mvar() + s*Rstar.mvar(), g.mvar());
        fnew= reduce (fnew, Rstar);
      }

      h= alg_gcd (g, fnew, Rstarlist);
      numinv= QuasiInverse (Rstar, alg_LC (h, algExtLevel), Rstar.mvar());
      h *= numinv;
      h= Prem (h, Rstarlist);
      h /= vcontent (h, Rstar.mvar());

      if (h.level() >= Rstar.level())
      {
        g= divide (g, h, Rstarlist);
        h= backSubst (h, backSubsts, Astar);
        h= Prem (h, as);
        h *= bCommonDen (h);
        h /= vcontent (h, as.getFirst().mvar());
        L.append (CFFactor (h, 1));
      }
    }
    // we are not interested in a
    // constant (over K_r, which can be a polynomial!)
    if (degree (g, f.mvar()) > 0)
      L.append (CFFactor (g, 1));
  }
  CFFList LL;
  if (getCharacteristic() > 0) //do I really need this part?
  {
    CFFListIterator i=L;
    CanonicalForm c_fac=1;
    CanonicalForm c;
    for(;i.hasItem(); i++ )
    {
      CanonicalForm ff=i.getItem().factor();
      c=alg_lc(ff);
      int e=i.getItem().exp();
      ff/=c;
      if (!ff.isOne()) LL.append(CFFactor(ff,e));
      while (e>0) { c_fac*=c;e--; }
    }
    if (!c_fac.isOne()) LL.insert(CFFactor(c_fac,1));
  }
  else
  {
    LL=L;
  }

  if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);

  return LL;
}

/// algorithm of A. Steel described in "Conquering Inseparability: Primary
/// decomposition and multivariate factorization over algebraic function fields
/// of positive characteristic" with the following modifications: we use
/// characteristic sets in IdealOverSubfield and Trager's primitive element
/// algorithm instead of FGLM
CFFList
SteelTrager (const CanonicalForm & f, const CFList & AS)
{
  CanonicalForm F= f, lcmVars= 1;
  CFList asnew, as= AS;
  CFListIterator i, ii;

  bool derivZeroF= false;
  int j, exp=0, expF= 0, tmpExp;
  CFFList varsMapLevel;
  CFFListIterator iter;

  // check if F is separable
  if (F.deriv().isZero())
  {
    derivZeroF= true;
    deflateDegree (F, expF, F.level());
  }

  CanonicalForm varsF= getVars (F);
  varsF /= F.mvar();

  lcmVars= lcm (varsF, lcmVars);

  if (derivZeroF)
    as.append (F);

  CFFList varsGMapLevel;
  CFFList tmp;
  CFFList * varsGMap= new CFFList [as.length()];
  for (j= 0; j < as.length(); j++)
    varsGMap[j]= CFFList();

  CanonicalForm varsG;
  j= 0;
  bool recurse= false;
  i= as;
  // make minimal polys and F separable
  while (i.hasItem())
  {
    if (i.getItem().deriv() == 0)
    {
      deflateDegree (i.getItem(), exp, i.getItem().level());
      i.getItem()= deflatePoly (i.getItem(), exp, i.getItem().level());
     
      varsG= getVars (i.getItem());
      varsG /= i.getItem().mvar();

      lcmVars= lcm (varsG, lcmVars);

      while (!varsG.isOne())
      {
        if (i.getItem().deriv (varsG.level()).isZero())
        {
          deflateDegree (i.getItem(), tmpExp, varsG.level());
          if (exp >= tmpExp)
          {
            if (exp == tmpExp)
              i.getItem()= deflatePoly (i.getItem(), exp, varsG.level());
            else
            {
              if (j != 0)
                recurse= true;
              i.getItem()= deflatePoly (i.getItem(), tmpExp, varsG.level());
            }
            varsGMapLevel.insert (CFFactor (varsG.mvar(), exp - tmpExp));
          }
          else
          {
            i.getItem()= deflatePoly (i.getItem(), exp, varsG.level());
            varsGMapLevel.insert (CFFactor (varsG.mvar(), 0));
          }
        }
        else
        {
          if (j != 0)
            recurse= true;
          varsGMapLevel.insert (CFFactor (varsG.mvar(),exp));
        }
        varsG /= varsG.mvar();
      }

      if (recurse)
      {
        ii= as;
        for (; ii.hasItem(); ii++)
        {
          if (ii.getItem() == i.getItem())
            continue;
          for (iter= varsGMapLevel; iter.hasItem(); iter++)
            ii.getItem()= inflatePoly (ii.getItem(), iter.getItem().exp(),
                                       iter.getItem().factor().level());
        }
      }
      else
      {
        ii= i;
        ii++;
        for (; ii.hasItem(); ii++)
        {
          for (iter= varsGMapLevel; iter.hasItem(); iter++)
          {
            ii.getItem()= inflatePoly (ii.getItem(), iter.getItem().exp(),
                                       iter.getItem().factor().level());
          }
        }
      }
      if (varsGMap[j].isEmpty())
        varsGMap[j]= varsGMapLevel;
      else
      {
        if (!varsGMapLevel.isEmpty())
        {
          tmp= varsGMap[j];
          CFFListIterator iter2= varsGMapLevel;
          ASSERT (tmp.length() == varsGMapLevel.length(),
                  "wrong length of lists");
          for (iter=tmp; iter.hasItem(); iter++, iter2++)
            iter.getItem()= CFFactor (iter.getItem().factor(),
                                  iter.getItem().exp() + iter2.getItem().exp());
          varsGMap[j]= tmp;
        }
      }
      varsGMapLevel= CFFList();
    }
    asnew.append (i.getItem());
    if (!recurse)
    {
      i++;
      j++;
    }
    else
    {
      i= as;
      j= 0;
      recurse= false;
      asnew= CFList();
    }
  }

  while (!lcmVars.isOne())
  {
    varsMapLevel.insert (CFFactor (lcmVars.mvar(), 0));
    lcmVars /= lcmVars.mvar();
  }

  // compute how to map variables in F
  for (j= 0; j < as.length(); j++)
  {
    if (varsGMap[j].isEmpty())
      continue;

    for (CFFListIterator iter2= varsGMap[j]; iter2.hasItem(); iter2++)
    {
      for (iter= varsMapLevel; iter.hasItem(); iter++)
      {
        if (iter.getItem().factor() == iter2.getItem().factor())
        {
          iter.getItem()= CFFactor (iter.getItem().factor(),
                                  iter.getItem().exp() + iter2.getItem().exp());
        }
      }
    }
  }

  delete [] varsGMap;

  if (derivZeroF)
  {
    as.removeLast();
    asnew.removeLast();
    F= deflatePoly (F, expF, F.level());
  }

  // map variables in F
  for (iter= varsMapLevel; iter.hasItem(); iter++)
  {
    if (expF > 0)
      tmpExp= iter.getItem().exp() - expF;
    else
      tmpExp= iter.getItem().exp();

    if (tmpExp > 0)
      F= inflatePoly (F, tmpExp, iter.getItem().factor().level());
    else if (tmpExp < 0)
      F= deflatePoly (F, -tmpExp, iter.getItem().factor().level());
  }

  // factor F with minimal polys given in asnew
  asnew.append (F);
  asnew= charSetViaModCharSet (asnew, false);

  F= asnew.getLast();
  F /= content (F);

  asnew.removeLast();
  for (i= asnew; i.hasItem(); i++)
    i.getItem() /= content (i.getItem());

  tmp= facAlgFunc (F, asnew);

  // transform back
  j= 0;
  int p= getCharacteristic();
  CFList transBack;
  CFMap M;
  CanonicalForm g;

  for (iter= varsMapLevel; iter.hasItem(); iter++)
  {
    if (iter.getItem().exp() > 0)
    {
      j++;
      g= power (Variable (f.level() + j), ipower (p, iter.getItem().exp())) -
         iter.getItem().factor().mvar();
      transBack.append (g);
      M.newpair (iter.getItem().factor().mvar(), Variable (f.level() + j));
    }
  }

  for (i= asnew; i.hasItem(); i++)
    transBack.insert (M (i.getItem()));

  if (expF > 0)
    tmpExp= ipower (p, expF);

  CFFList result;
  CFList transform;

  for (iter= tmp; iter.hasItem(); iter++)
  {
    transform= transBack;
    CanonicalForm factor= iter.getItem().factor();
    factor= M (factor);
    transform.append (factor);
    transform= charSetViaModCharSet (transform, false);
    for (i= transform; i.hasItem(); i++)
    {
      if (degree (i.getItem(), f.mvar()) > 0)
      {
        factor= i.getItem();
        break;
      }
    }

    factor /= content (factor);

    if (expF > 0)
    {
      int mult= tmpExp/(degree (factor)/degree (iter.getItem().factor()));
      result.append (CFFactor (factor, iter.getItem().exp()*mult));
    }
    else
      result.append (CFFactor (factor, iter.getItem().exp()));
  }

  return result;
}

/// factorize a polynomial that is irreducible over the ground field modulo an
/// extension given by an irreducible characteristic set

// 1) prepares data
// 2) for char=p we distinguish 3 cases:
//           no transcendentals, separable and inseparable extensions
CFFList
facAlgFunc2 (const CanonicalForm & f, const CFList & as)
{
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  Variable vf=f.mvar();
  CFListIterator i;
  CFFListIterator jj;
  CFList reduceresult;
  CFFList result;

// F1: [Test trivial cases]
// 1) first trivial cases:
  if (vf.level() <= as.getLast().level())
  {
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return CFFList(CFFactor(f,1));
  }

// 2) Setup list of those polys in AS having degree > 1
  CFList Astar;
  Variable x;
  CanonicalForm elem;
  Varlist ord, uord;
  for (int ii= 1; ii < level (vf); ii++)
    uord.append (Variable (ii));

  for (i= as; i.hasItem(); i++)
  {
    elem= i.getItem();
    x= elem.mvar();
    if (degree (elem, x) > 1)  // otherwise it's not an extension
    {
      Astar.append (elem);
      ord.append (x);
    }
  }
  uord= Difference (uord, ord);

// 3) second trivial cases: we already proved irr. of f over no extensions
  if (Astar.length() == 0)
  {
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return CFFList (CFFactor (f, 1));
  }

// 4) Look if elements in uord actually occur in any of the minimal
//    polynomials. If no element of uord occures in any of the minimal
//    polynomials the field is an alg. number field not an alg. function field
  Varlist newuord= varsInAs (uord, Astar);

  CFFList Factorlist;
  Varlist gcdord= Union (ord, newuord);
  gcdord.append (f.mvar());
  bool isFunctionField= (newuord.length() > 0);

  // TODO alg_sqrfree?
  CanonicalForm Fgcd= 0;
  if (isFunctionField)
    Fgcd= alg_gcd (f, f.deriv(), Astar);

  bool derivZero= f.deriv().isZero();
  if (isFunctionField && (degree (Fgcd, f.mvar()) > 0) && !derivZero)
  {
    CanonicalForm Ggcd= divide(f, Fgcd,Astar);
    if (getCharacteristic() == 0)
    {
      CFFList result= facAlgFunc2 (Ggcd, as); //Ggcd is the squarefree part of f
      multiplicity (result, f, Astar);
      if (!isRat && getCharacteristic() == 0)
        Off (SW_RATIONAL);
      return result;
    }

    Fgcd= pp (Fgcd);
    Ggcd= pp (Ggcd);
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return merge (facAlgFunc2 (Fgcd, as), facAlgFunc2 (Ggcd, as));
  }

  if (getCharacteristic() > 0)
  {
    IntList degreelist;
    Variable vminpoly;
    for (i= Astar; i.hasItem(); i++)
      degreelist.append (degree (i.getItem()));

    int extdeg= getDegOfExt (degreelist, degree (f));

    if (newuord.length() == 0) // no parameters
    {
      if (extdeg > 1)
      {
        CanonicalForm MIPO= generateMipo (extdeg);
        vminpoly= rootOf(MIPO);
      }
      Factorlist= Trager(f, Astar, vminpoly, as, isFunctionField);
      return Factorlist;
    }
    else if (isInseparable(Astar) || derivZero) // inseparable case
    {
      Factorlist= SteelTrager (f, Astar);
      return Factorlist;
    }
    else // separable case
    {
      if (extdeg > 1)
      {
        CanonicalForm MIPO=generateMipo (extdeg);
        vminpoly= rootOf (MIPO);
      }
      Factorlist= Trager (f, Astar, vminpoly, as, isFunctionField);
      return Factorlist;
    }
  }
  else // char 0
  {
    Variable vminpoly;
    Factorlist= Trager (f, Astar, vminpoly, as, isFunctionField);
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return Factorlist;
  }

  return CFFList (CFFactor(f,1));
}


/// factorize a polynomial modulo an extension given by an irreducible
/// characteristic set
CFFList
facAlgFunc (const CanonicalForm & f, const CFList & as)
{
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  CFFList Output, output, Factors= factorize(f);
  if (Factors.getFirst().factor().inCoeffDomain())
    Factors.removeFirst();

  if (as.length() == 0)
  {
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return Factors;
  }
  if (f.level() <= as.getLast().level())
  {
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return Factors;
  }

  for (CFFListIterator i=Factors; i.hasItem(); i++)
  {
    if (i.getItem().factor().level() > as.getLast().level())
    {
      output= facAlgFunc2 (i.getItem().factor(), as);
      for (CFFListIterator j= output; j.hasItem(); j++)
        Output= append (Output, CFFactor (j.getItem().factor(),
                                          j.getItem().exp()*i.getItem().exp()));
    }
  }

  if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);
  return Output;
}
