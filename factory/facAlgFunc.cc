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

#include "cf_generator.h"
#include "cf_iter.h"
#include "cf_util.h"
#include "cf_algorithm.h"
#include "templates/ftmpl_functions.h"
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
#ifdef HAVE_NTL
  if (getCharacteristic() == 0)
    result= resultantZ (fz, gz,v);
  else
    result= resultantFp (fz,gz,v);
#else
    result= resultant (fz,gz,v);
#endif

  return result;
}

/// compute the norm R of f over PPalpha, g= f (x-s*alpha)
/// if proof==true, R is squarefree and if in addition getCharacteristic() > 0
/// the squarefree factors of R are returned.
/// Based on Trager's sqrf_norm algorithm.
static CFFList
norm (const CanonicalForm & f, const CanonicalForm & PPalpha,
      CFGenerator & myrandom, CanonicalForm & s, CanonicalForm & g,
      CanonicalForm & R, bool proof)
{
  Variable y= PPalpha.mvar(), vf= f.mvar();
  CanonicalForm temp, Palpha= PPalpha, t;
  int sqfreetest= 0;
  CFFList testlist;
  CFFListIterator i;

  if (proof)
  {
    myrandom.reset();
    s= myrandom.item();
    g= f;
    R= CanonicalForm(0);
  }
  else
  {
    if (getCharacteristic() == 0)
      t= CanonicalForm (mapinto (myrandom.item()));
    else
      t= CanonicalForm (myrandom.item());
    s= t;
    g= f (vf - t*Palpha.mvar(), vf);
  }

  // Norm, resultante taken with respect to y
  while (!sqfreetest)
  {
    R= resultante (Palpha, g, y);
    R= R* bCommonDen(R);
    R /= content (R);
    if (proof)
    {
      // sqfree check ; R is a polynomial in K[x]
      if (getCharacteristic() == 0)
      {
        temp= gcd (R, R.deriv (vf));
        if (degree(temp,vf) != 0 || temp == temp.genZero())
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
        g= f (vf - t*Palpha.mvar(), vf);
      }
    }
    else
      break;
  }
  return testlist;
}

/// see @a norm, R is guaranteed to be squarefree
/// Based on Trager's sqrf_norm algorithm.
static CFFList
sqrfNorm (const CanonicalForm & f, const CanonicalForm & PPalpha,
          const Variable & Extension, CanonicalForm & s,  CanonicalForm & g,
          CanonicalForm & R)
{
  CFFList result;
  if (getCharacteristic() == 0)
  {
    IntGenerator myrandom;
    result= norm (f, PPalpha, myrandom, s, g, R, true);
  }
  else if (degree (Extension) > 0)
  {
    AlgExtGenerator myrandom (Extension);
    result= norm (f, PPalpha, myrandom, s, g, R, true);
  }
  else
  {
    FFGenerator myrandom;
    result= norm (f, PPalpha, myrandom, s, g, R, true);
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
    Returnlist.append (rb);
    if (isFunctionField)
      Returnlist.append (denrb);
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
      (void) sqrfNorm (i.getItem(), R, Extension, s, g, R);

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
          j.getItem()= j.getItem() (rb, i.getItem().mvar());
          j.getItem()= j.getItem() (ra, oldR.mvar());
        }
        prune (alpha);
      }
      else
      {
        if (getCharacteristic() == 0)
          On (SW_RATIONAL);
        Variable v= Variable (tmax (g.level(), oldR.level()) + 1);
        h= swapvar (g, oldR.mvar(), v);
        tmp= CFList (R);
        h= alg_gcd (h, swapvar (oldR, oldR.mvar(), v), tmp);

        CanonicalForm numinv, deninv;
        numinv= QuasiInverse (tmp.getFirst(), LC (h), tmp.getFirst().mvar());
        h *= numinv;
        h= Prem (h, tmp);
        deninv= LC(h);

        ra= -h[0];
        denra= gcd (ra, deninv);
        ra /= denra;
        denra= deninv/denra;
        rb= R.mvar()*denra-s*ra;
        denrb= denra;
        for (; j.hasItem(); j++)
        {
          CanonicalForm powdenra= power (denra, degree (j.getItem(),
                                         i.getItem().mvar()));
          j.getItem()= evaluate (j.getItem(), rb, denrb, powdenra,
                                 i.getItem().mvar());
          powdenra= power (denra, degree (j.getItem(), oldR.mvar()));
          j.getItem()= evaluate (j.getItem(),ra, denra, powdenra, oldR.mvar());

        }
      }

      Returnlist.append (ra);
      if (isFunctionField)
        Returnlist.append (denra);
      Returnlist.append (rb);
      if (isFunctionField)
        Returnlist.append (denrb);
    }
  }

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
  CFFList L, normFactors, tmp;
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
    tmp= factorize (g, alpha); // factorization over number field

    for (iter= tmp; iter.hasItem(); iter++)
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
    prune (alpha);
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

  CFGenerator * Gen;
  if (getCharacteristic() == 0)
    Gen= CFGenFactory::generate();
  else if (degree (vminpoly) > 0)
    Gen= AlgExtGenerator (vminpoly).clone();
  else
    Gen= CFGenFactory::generate();

  CFFList LL= CFFList (CFFactor (f, 1));

  Variable X;
  do
  {
    tmp= CFFList();
    for (iter= LL; iter.hasItem(); iter++)
    {
      f= iter.getItem().factor();
      (void) norm (f, Rstar, *Gen, s, g, R, false);

      if (hasFirstAlgVar (R, X))
        normFactors= factorize (R, X);
      else
        normFactors= factorize (R);

      if (normFactors.getFirst().factor().inCoeffDomain())
        normFactors.removeFirst();
      if (normFactors.length() < 1 || (normFactors.length() == 1 && normFactors.getLast().exp() == 1))
      {
        f= backSubst (f, backSubsts, Astar);
        f *= bCommonDen (f);
        f= Prem (f, as);
        f /= vcontent (f, as.getFirst().mvar());

        L.append (CFFactor (f, 1));
      }
      else
      {
        g= f;
        int count= 0;
        for (iter2= normFactors; iter2.hasItem(); iter2++)
        {
          CanonicalForm fnew= iter2.getItem().factor();
          if (fnew.level() <= Rstar.level()) //factor is a constant from the function field
            continue;
          else
          {
            fnew= fnew (g.mvar() + s*Rstar.mvar(), g.mvar());
            fnew= Prem (fnew, CFList (Rstar));
          }

          h= alg_gcd (g, fnew, Rstarlist);
          numinv= QuasiInverse (Rstar, alg_LC (h, algExtLevel), Rstar.mvar());
          h *= numinv;
          h= Prem (h, Rstarlist);
          h /= vcontent (h, Rstar.mvar());

          if (h.level() > Rstar.level())
          {
            g= divide (g, h, Rstarlist);
            if (degree (h) == 1 || iter2.getItem().exp() == 1)
            {
              h= backSubst (h, backSubsts, Astar);
              h= Prem (h, as);
              h *= bCommonDen (h);
              h /= vcontent (h, as.getFirst().mvar());
              L.append (CFFactor (h, 1));
            }
            else
              tmp.append (CFFactor (h, iter2.getItem().exp()));
          }
          count++;
          if (g.level() <= Rstar.level())
            break;
          if (count == normFactors.length() - 1)
          {
            if (normFactors.getLast().exp() == 1 && g.level() > Rstar.level())
            {
              g= backSubst (g, backSubsts, Astar);
              g= Prem (g, as);
              g *= bCommonDen (g);
              g /= vcontent (g, as.getFirst().mvar());
              L.append (CFFactor (g, 1));
            }
            else if (normFactors.getLast().exp() > 1 &&
                     g.level() > Rstar.level())
            {
              g /= vcontent (g, Rstar.mvar());
              tmp.append (CFFactor (g, normFactors.getLast().exp()));
            }
            break;
          }
        }
      }
    }
    LL= tmp;
    (*Gen).next();
  }
  while (!LL.isEmpty());

  if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);

  delete Gen;

  return L;
}


/// map elements in @a AS into a PIE \f$ L \f$ and record where the variables
/// are mapped to in @a varsMapLevel, i.e @a varsMapLevel contains a list of
/// pairs of variables \f$ v_i \f$ and integers \f$ e_i \f$ such that
/// \f$ L=K(\sqrt[p^{e_1}]{v_1}, \ldots, \sqrt[p^{e_n}]{v_n}) \f$
CFList
mapIntoPIE (CFFList& varsMapLevel, CanonicalForm& lcmVars, const CFList & AS)
{
  CanonicalForm varsG;
  int j, exp= 0, tmpExp;
  bool recurse= false;
  CFList asnew, as= AS;
  CFListIterator i= as, ii;
  CFFList varsGMapLevel, tmp;
  CFFListIterator iter;
  CFFList * varsGMap= new CFFList [as.length()];
  for (j= 0; j < as.length(); j++)
    varsGMap[j]= CFFList();
  j= 0;
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

  return asnew;
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
  int j, expF= 0, tmpExp;
  CFFList varsMapLevel, tmp;
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

  asnew= mapIntoPIE (varsMapLevel, lcmVars, as);

  if (derivZeroF)
  {
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

  bool found;
  for (iter= tmp; iter.hasItem(); iter++)
  {
    found= false;
    transform= transBack;
    CanonicalForm factor= iter.getItem().factor();
    factor= M (factor);
    transform.append (factor);
    transform= modCharSet (transform, false);

retry:
    if (transform.isEmpty())
    {
      transform= transBack;
      transform.append (factor);
      transform= charSetViaCharSetN (transform);
    }
    for (i= transform; i.hasItem(); i++)
    {
      if (degree (i.getItem(), f.mvar()) > 0)
      {
        if (i.getItem().level() > f.level())
          break;
        found= true;
        factor= i.getItem();
        break;
      }
    }

    if (!found)
    {
      found= false;
      transform= CFList();
      goto retry;
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
      if (extdeg > 1)
        prune (vminpoly);
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
      if (extdeg > 1)
        prune (vminpoly);
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
