// -*- c++ -*-
//*****************************************************************************
/** @file cf_gcd_smallp.cc
 *
 * @author Martin Lee
 * @date 22.10.2009
 *
 * This file implements the GCD of two polynomials over \f$ F_{p} \f$ ,
 * \f$ F_{p}(\alpha ) \f$ or GF based on Alg. 7.2. as described in "Algorithms
 * for Computer Algebra" by Geddes, Czapor, Labahn
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
**/
//*****************************************************************************


#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "algext.h"
#include "cf_map.h"
#include "cf_util.h"
#include "templates/ftmpl_functions.h"
#include "cf_random.h"
#include "cf_reval.h"
#include "facHensel.h"
#include "cf_iter.h"
#include "cfNewtonPolygon.h"
#include "cf_algorithm.h"
#include "cf_primes.h"

// iinline helper functions:
#include "cf_map_ext.h"

#ifdef HAVE_NTL
#include <NTLconvert.h>
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#include "cf_gcd_smallp.h"

TIMING_DEFINE_PRINT(gcd_recursion)
TIMING_DEFINE_PRINT(newton_interpolation)
TIMING_DEFINE_PRINT(termination_test)
TIMING_DEFINE_PRINT(ez_p_compress)
TIMING_DEFINE_PRINT(ez_p_hensel_lift)
TIMING_DEFINE_PRINT(ez_p_eval)
TIMING_DEFINE_PRINT(ez_p_content)

bool
terminationTest (const CanonicalForm& F, const CanonicalForm& G,
                 const CanonicalForm& coF, const CanonicalForm& coG,
                 const CanonicalForm& cand)
{
  CanonicalForm LCCand= abs (LC (cand));
  if (LCCand*abs (LC (coF)) == abs (LC (F)))
  {
    if (LCCand*abs (LC (coG)) == abs (LC (G)))
    {
      if (abs (cand)*abs (coF) == abs (F))
      {
        if (abs (cand)*abs (coG) == abs (G))
          return true;
      }
      return false;
    }
    return false;
  }
  return false;
}

#ifdef HAVE_NTL

static const double log2exp= 1.442695041;

/// compressing two polynomials F and G, M is used for compressing,
/// N to reverse the compression
int myCompress (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                CFMap & N, bool topLevel)
{
  int n= tmax (F.level(), G.level());
  int * degsf= new int [n + 1];
  int * degsg= new int [n + 1];

  for (int i = 0; i <= n; i++)
    degsf[i]= degsg[i]= 0;

  degsf= degrees (F, degsf);
  degsg= degrees (G, degsg);

  int both_non_zero= 0;
  int f_zero= 0;
  int g_zero= 0;
  int both_zero= 0;

  if (topLevel)
  {
    for (int i= 1; i <= n; i++)
    {
      if (degsf[i] != 0 && degsg[i] != 0)
      {
        both_non_zero++;
        continue;
      }
      if (degsf[i] == 0 && degsg[i] != 0 && i <= G.level())
      {
        f_zero++;
        continue;
      }
      if (degsg[i] == 0 && degsf[i] && i <= F.level())
      {
        g_zero++;
        continue;
      }
    }

    if (both_non_zero == 0)
    {
      delete [] degsf;
      delete [] degsg;
      return 0;
    }

    // map Variables which do not occur in both polynomials to higher levels
    int k= 1;
    int l= 1;
    for (int i= 1; i <= n; i++)
    {
      if (degsf[i] != 0 && degsg[i] == 0 && i <= F.level())
      {
        if (k + both_non_zero != i)
        {
          M.newpair (Variable (i), Variable (k + both_non_zero));
          N.newpair (Variable (k + both_non_zero), Variable (i));
        }
        k++;
      }
      if (degsf[i] == 0 && degsg[i] != 0 && i <= G.level())
      {
        if (l + g_zero + both_non_zero != i)
        {
          M.newpair (Variable (i), Variable (l + g_zero + both_non_zero));
          N.newpair (Variable (l + g_zero + both_non_zero), Variable (i));
        }
        l++;
      }
    }

    // sort Variables x_{i} in increasing order of
    // min(deg_{x_{i}}(f),deg_{x_{i}}(g))
    int m= tmax (F.level(), G.level());
    int min_max_deg;
    k= both_non_zero;
    l= 0;
    int i= 1;
    while (k > 0)
    {
      if (degsf [i] != 0 && degsg [i] != 0)
        min_max_deg= tmax (degsf[i], degsg[i]);
      else
        min_max_deg= 0;
      while (min_max_deg == 0)
      {
        i++;
        if (degsf [i] != 0 && degsg [i] != 0)
          min_max_deg= tmax (degsf[i], degsg[i]);
        else
          min_max_deg= 0;
      }
      for (int j= i + 1; j <=  m; j++)
      {
        if (degsf[j] != 0 && degsg [j] != 0 &&
            tmax (degsf[j],degsg[j]) <= min_max_deg)
        {
          min_max_deg= tmax (degsf[j], degsg[j]);
          l= j;
        }
      }
      if (l != 0)
      {
        if (l != k)
        {
          M.newpair (Variable (l), Variable(k));
          N.newpair (Variable (k), Variable(l));
          degsf[l]= 0;
          degsg[l]= 0;
          l= 0;
        }
        else
        {
          degsf[l]= 0;
          degsg[l]= 0;
          l= 0;
        }
      }
      else if (l == 0)
      {
        if (i != k)
        {
          M.newpair (Variable (i), Variable (k));
          N.newpair (Variable (k), Variable (i));
          degsf[i]= 0;
          degsg[i]= 0;
        }
        else
        {
          degsf[i]= 0;
          degsg[i]= 0;
        }
        i++;
      }
      k--;
    }
  }
  else
  {
    //arrange Variables such that no gaps occur
    for (int i= 1; i <= n; i++)
    {
      if (degsf[i] == 0 && degsg[i] == 0)
      {
        both_zero++;
        continue;
      }
      else
      {
        if (both_zero != 0)
        {
          M.newpair (Variable (i), Variable (i - both_zero));
          N.newpair (Variable (i - both_zero), Variable (i));
        }
      }
    }
  }

  delete [] degsf;
  delete [] degsg;

  return 1;
}

static inline CanonicalForm
uni_content (const CanonicalForm & F);

CanonicalForm
uni_content (const CanonicalForm& F, const Variable& x)
{
  if (F.inCoeffDomain())
    return F.genOne();
  if (F.level() == x.level() && F.isUnivariate())
    return F;
  if (F.level() != x.level() && F.isUnivariate())
    return F.genOne();

  if (x.level() != 1)
  {
    CanonicalForm f= swapvar (F, x, Variable (1));
    CanonicalForm result= uni_content (f);
    return swapvar (result, x, Variable (1));
  }
  else
    return uni_content (F);
}

/// compute the content of F, where F is considered as an element of
/// \f$ R[x_{1}][x_{2},\ldots ,x_{n}] \f$
static inline CanonicalForm
uni_content (const CanonicalForm & F)
{
  if (F.inBaseDomain())
    return F.genOne();
  if (F.level() == 1 && F.isUnivariate())
    return F;
  if (F.level() != 1 && F.isUnivariate())
    return F.genOne();
  if (degree (F,1) == 0) return F.genOne();

  int l= F.level();
  if (l == 2)
    return content(F);
  else
  {
    CanonicalForm pol, c = 0;
    CFIterator i = F;
    for (; i.hasTerms(); i++)
    {
      pol= i.coeff();
      pol= uni_content (pol);
      c= gcd (c, pol);
      if (c.isOne())
        return c;
    }
    return c;
  }
}

CanonicalForm
extractContents (const CanonicalForm& F, const CanonicalForm& G,
                 CanonicalForm& contentF, CanonicalForm& contentG,
                 CanonicalForm& ppF, CanonicalForm& ppG, const int d)
{
  CanonicalForm uniContentF, uniContentG, gcdcFcG;
  contentF= 1;
  contentG= 1;
  ppF= F;
  ppG= G;
  CanonicalForm result= 1;
  for (int i= 1; i <= d; i++)
  {
    uniContentF= uni_content (F, Variable (i));
    uniContentG= uni_content (G, Variable (i));
    gcdcFcG= gcd (uniContentF, uniContentG);
    contentF *= uniContentF;
    contentG *= uniContentG;
    ppF /= uniContentF;
    ppG /= uniContentG;
    result *= gcdcFcG;
  }
  return result;
}

/// compute the leading coefficient of F, where F is considered as an element
/// of \f$ R[x_{1}][x_{2},\ldots ,x_{n}] \f$, order on Mon (x_{2},\ldots ,x_{n})
/// is dp.
static inline
CanonicalForm uni_lcoeff (const CanonicalForm& F)
{
  if (F.level() > 1)
  {
    Variable x= Variable (2);
    int deg= totaldegree (F, x, F.mvar());
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      if (i.exp() + totaldegree (i.coeff(), x, i.coeff().mvar()) == deg)
        return uni_lcoeff (i.coeff());
    }
  }
  return F;
}

/// Newton interpolation - Incremental algorithm.
/// Given a list of values alpha_i and a list of polynomials u_i, 1 <= i <= n,
/// computes the interpolation polynomial assuming that
/// the polynomials in u are the results of evaluating the variabe x
/// of the unknown polynomial at the alpha values.
/// This incremental version receives only the values of alpha_n and u_n and
/// the previous interpolation polynomial for points 1 <= i <= n-1, and computes
/// the polynomial interpolating in all the points.
/// newtonPoly must be equal to (x - alpha_1) * ... * (x - alpha_{n-1})
static inline CanonicalForm
newtonInterp(const CanonicalForm alpha, const CanonicalForm u,
             const CanonicalForm newtonPoly, const CanonicalForm oldInterPoly,
             const Variable & x)
{
  CanonicalForm interPoly;

  interPoly= oldInterPoly + ((u - oldInterPoly(alpha, x))/newtonPoly(alpha, x))
             *newtonPoly;
  return interPoly;
}

/// compute a random element a of \f$ F_{p}(\alpha ) \f$ ,
/// s.t. F(a) \f$ \neq 0 \f$ , F is a univariate polynomial, returns
/// fail if there are no field elements left which have not been used before
static inline CanonicalForm
randomElement (const CanonicalForm & F, const Variable & alpha, CFList & list,
               bool & fail)
{
  fail= false;
  Variable x= F.mvar();
  AlgExtRandomF genAlgExt (alpha);
  FFRandom genFF;
  CanonicalForm random, mipo;
  mipo= getMipo (alpha);
  int p= getCharacteristic ();
  int d= degree (mipo);
  double bound= pow ((double) p, (double) d);
  do
  {
    if (list.length() == bound)
    {
      fail= true;
      break;
    }
    if (list.length() < p)
    {
      random= genFF.generate();
      while (find (list, random))
        random= genFF.generate();
    }
    else
    {
      random= genAlgExt.generate();
      while (find (list, random))
        random= genAlgExt.generate();
    }
    if (F (random, x) == 0)
    {
      list.append (random);
      continue;
    }
  } while (find (list, random));
  return random;
}

static inline
Variable chooseExtension (const Variable & alpha)
{
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLIrredpoly;
  int i, m;
  // extension of F_p needed
  if (alpha.level() == 1)
  {
    i= 1;
    m= 2;
  } //extension of F_p(alpha)
  if (alpha.level() != 1)
  {
    i= 4;
    m= degree (getMipo (alpha));
  }
  BuildIrred (NTLIrredpoly, i*m);
  CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
  return rootOf (newMipo);
}

CanonicalForm
GCD_Fp_extension (const CanonicalForm& F, const CanonicalForm& G,
                  CanonicalForm& coF, CanonicalForm& coG,
                  Variable & alpha, CFList& l, bool& topLevel);

CanonicalForm
GCD_Fp_extension (const CanonicalForm& F, const CanonicalForm& G,
                  Variable & alpha, CFList& l, bool& topLevel)
{
  CanonicalForm dummy1, dummy2;
  CanonicalForm result= GCD_Fp_extension (F, G, dummy1, dummy2, alpha, l,
                                          topLevel);
  return result;
}

/// GCD of F and G over \f$ F_{p}(\alpha ) \f$ ,
/// l and topLevel are only used internally, output is monic
/// based on Alg. 7.2. as described in "Algorithms for
/// Computer Algebra" by Geddes, Czapor, Labahn
CanonicalForm
GCD_Fp_extension (const CanonicalForm& F, const CanonicalForm& G,
                  CanonicalForm& coF, CanonicalForm& coG,
                  Variable & alpha, CFList& l, bool& topLevel)
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0)
  {
    coF= 0;
    coG= Lc (G);
    return G/Lc(G);
  }
  else if (G.isZero() && degree (F) > 0)
  {
    coF= Lc (F);
    coG= 0;
    return F/Lc(F);
  }
  else if (F.isZero() && G.isZero())
  {
    coF= coG= 0;
    return F.genOne();
  }
  if (F.inBaseDomain() || G.inBaseDomain())
  {
    coF= F;
    coG= G;
    return F.genOne();
  }
  if (F.isUnivariate() && fdivides(F, G, coG))
  {
    coF= Lc (F);
    return F/Lc(F);
  }
  if (G.isUnivariate() && fdivides(G, F, coF))
  {
    coG= Lc (G);
    return G/Lc(G);
  }
  if (F == G)
  {
    coF= coG= Lc (F);
    return F/Lc(F);
  }

  CFMap M,N;
  int best_level= myCompress (A, B, M, N, topLevel);

  if (best_level == 0)
  {
    coF= F;
    coG= G;
    return B.genOne();
  }

  A= M(A);
  B= M(B);

  Variable x= Variable(1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
  {
    CanonicalForm result= gcd (A, B);
    coF= N (A/result);
    coG= N (B/result);
    return N (result);
  }

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;

  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable(2), Variable (ppA.level()));

  if (d == 0)
  {
    coF= N (ppA*(cA/gcdcAcB));
    coG= N (ppB*(cB/gcdcAcB));
    return N(gcdcAcB);
  }

  int d0= totaldegree (ppB, Variable(2), Variable (ppB.level()));
  if (d0 < d)
    d= d0;
  if (d == 0)
  {
    coF= N (ppA*(cA/gcdcAcB));
    coG= N (ppB*(cB/gcdcAcB));
    return N(gcdcAcB);
  }

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly, coF_random_element, coG_random_element, coF_m,
                coG_m, ppCoF, ppCoG;

  newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  coF= 0;
  coG= 0;
  H= 0;
  bool fail= false;
  topLevel= false;
  bool inextension= false;
  Variable V_buf= alpha;
  CanonicalForm prim_elem, im_prim_elem;
  CFList source, dest;
  int bound1= degree (ppA, 1);
  int bound2= degree (ppB, 1);
  do
  {
    random_element= randomElement (m*lcA*lcB, V_buf, l, fail);
    if (fail)
    {
      source= CFList();
      dest= CFList();

      Variable V_buf3= V_buf;
      V_buf= chooseExtension (V_buf);
      bool prim_fail= false;
      Variable V_buf2;
      prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

      if (V_buf3 != alpha)
      {
        m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
        G_m= mapDown (G_m, prim_elem, im_prim_elem, alpha, source, dest);
        coF_m= mapDown (coF_m, prim_elem, im_prim_elem, alpha, source, dest);
        coG_m= mapDown (coG_m, prim_elem, im_prim_elem, alpha, source, dest);
        newtonPoly= mapDown (newtonPoly, prim_elem, im_prim_elem, alpha,
                             source, dest);
        ppA= mapDown (ppA, prim_elem, im_prim_elem, alpha, source, dest);
        ppB= mapDown (ppB, prim_elem, im_prim_elem, alpha, source, dest);
        gcdlcAlcB= mapDown (gcdlcAlcB, prim_elem, im_prim_elem, alpha,
                            source, dest);
        lcA= mapDown (lcA, prim_elem, im_prim_elem, alpha, source, dest);
        lcB= mapDown (lcB, prim_elem, im_prim_elem, alpha, source, dest);
        for (CFListIterator i= l; i.hasItem(); i++)
          i.getItem()= mapDown (i.getItem(), prim_elem, im_prim_elem, alpha,
                                source, dest);
      }

      ASSERT (!prim_fail, "failure in integer factorizer");
      if (prim_fail)
        ; //ERROR
      else
        im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
      DEBOUTLN (cerr, "getMipo (V_buf2)= " << getMipo (V_buf2));
      inextension= true;
      for (CFListIterator i= l; i.hasItem(); i++)
        i.getItem()= mapUp (i.getItem(), alpha, V_buf, prim_elem,
                             im_prim_elem, source, dest);
      m= mapUp (m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      G_m= mapUp (G_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      coF_m= mapUp (coF_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      coG_m= mapUp (coG_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      newtonPoly= mapUp (newtonPoly, alpha, V_buf, prim_elem, im_prim_elem,
                          source, dest);
      ppA= mapUp (ppA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      ppB= mapUp (ppB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      gcdlcAlcB= mapUp (gcdlcAlcB, alpha, V_buf, prim_elem, im_prim_elem,
                         source, dest);
      lcA= mapUp (lcA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      lcB= mapUp (lcB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);

      fail= false;
      random_element= randomElement (m*lcA*lcB, V_buf, l, fail );
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x),
                        coF_random_element, coG_random_element, V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      GCD_Fp_extension (ppA(random_element, x), ppB(random_element, x),
                        coF_random_element, coG_random_element, V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 == 0)
    {
      coF= N (ppA*(cA/gcdcAcB));
      coG= N (ppB*(cB/gcdcAcB));
      return N(gcdcAcB);
    }
    if (d0 >  d)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    G_random_element=
    (gcdlcAlcB(random_element, x)/uni_lcoeff (G_random_element))
    * G_random_element;
    coF_random_element= (lcA(random_element,x)/uni_lcoeff(coF_random_element))
                        *coF_random_element;
    coG_random_element= (lcB(random_element,x)/uni_lcoeff(coG_random_element))
                        *coG_random_element;

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 <  d)
    {
      m= gcdlcAlcB;
      newtonPoly= 1;
      G_m= 0;
      d= d0;
      coF_m= 0;
      coG_m= 0;
    }

    TIMING_START (newton_interpolation);
    H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
    coF= newtonInterp (random_element, coF_random_element, newtonPoly, coF_m,x);
    coG= newtonInterp (random_element, coG_random_element, newtonPoly, coG_m,x);
    TIMING_END_AND_PRINT (newton_interpolation,
                          "time for newton interpolation: ");

    //termination test
    if ((uni_lcoeff (H) == gcdlcAlcB) || (G_m == H))
    {
      TIMING_START (termination_test);
      if (gcdlcAlcB.isOne())
        cH= 1;
      else
        cH= uni_content (H);
      ppH= H/cH;
      ppH /= Lc (ppH);
      CanonicalForm lcppH= gcdlcAlcB/cH;
      CanonicalForm ccoF= lcppH/Lc (lcppH);
      CanonicalForm ccoG= lcppH/Lc (lcppH);
      ppCoF= coF/ccoF;
      ppCoG= coG/ccoG;
      if (inextension)
      {
        if (((degree (ppCoF,1)+degree (ppH,1) == bound1) &&
             (degree (ppCoG,1)+degree (ppH,1) == bound2) &&
             terminationTest (ppA, ppB, ppCoF, ppCoG, ppH)) ||
             (fdivides (ppH, ppA, ppCoF) && fdivides (ppH, ppB, ppCoG)))
        {
          CFList u, v;
          DEBOUTLN (cerr, "ppH before mapDown= " << ppH);
          ppH= mapDown (ppH, prim_elem, im_prim_elem, alpha, u, v);
          ppCoF= mapDown (ppCoF, prim_elem, im_prim_elem, alpha, u, v);
          ppCoG= mapDown (ppCoG, prim_elem, im_prim_elem, alpha, u, v);
          DEBOUTLN (cerr, "ppH after mapDown= " << ppH);
          coF= N ((cA/gcdcAcB)*ppCoF);
          coG= N ((cB/gcdcAcB)*ppCoG);
          TIMING_END_AND_PRINT (termination_test,
                                "time for successful termination test Fq: ");
          return N(gcdcAcB*ppH);
        }
      }
      else if (((degree (ppCoF,1)+degree (ppH,1) == bound1) &&
                (degree (ppCoG,1)+degree (ppH,1) == bound2) &&
                terminationTest (ppA, ppB, ppCoF, ppCoG, ppH)) ||
                (fdivides (ppH, ppA, ppCoF) && fdivides (ppH, ppB, ppCoG)))
      {
        coF= N ((cA/gcdcAcB)*ppCoF);
        coG= N ((cB/gcdcAcB)*ppCoG);
        TIMING_END_AND_PRINT (termination_test,
                              "time for successful termination test Fq: ");
        return N(gcdcAcB*ppH);
      }
      TIMING_END_AND_PRINT (termination_test,
                            "time for unsuccessful termination test Fq: ");
    }

    G_m= H;
    coF_m= coF;
    coG_m= coG;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);
  } while (1);
}

/// compute a random element a of GF, s.t. F(a) \f$ \neq 0 \f$ , F is a
/// univariate polynomial, returns fail if there are no field elements left
/// which have not been used before
static inline
CanonicalForm
GFRandomElement (const CanonicalForm& F, CFList& list, bool& fail)
{
  fail= false;
  Variable x= F.mvar();
  GFRandom genGF;
  CanonicalForm random;
  int p= getCharacteristic();
  int d= getGFDegree();
  int bound= ipower (p, d);
  do
  {
    if (list.length() == bound)
    {
      fail= true;
      break;
    }
    if (list.length() < 1)
      random= 0;
    else
    {
      random= genGF.generate();
      while (find (list, random))
        random= genGF.generate();
    }
    if (F (random, x) == 0)
    {
      list.append (random);
      continue;
    }
  } while (find (list, random));
  return random;
}

CanonicalForm
GCD_GF (const CanonicalForm& F, const CanonicalForm& G,
        CanonicalForm& coF, CanonicalForm& coG,
        CFList& l, bool& topLevel);

CanonicalForm
GCD_GF (const CanonicalForm& F, const CanonicalForm& G, CFList& l,
        bool& topLevel)
{
  CanonicalForm dummy1, dummy2;
  CanonicalForm result= GCD_GF (F, G, dummy1, dummy2, l, topLevel);
  return result;
}

/// GCD of F and G over GF, based on Alg. 7.2. as described in "Algorithms for
/// Computer Algebra" by Geddes, Czapor, Labahn
/// Usually this algorithm will be faster than GCD_Fp_extension since GF has
/// faster field arithmetics, however it might fail if the input is large since
/// the size of the base field is bounded by 2^16, output is monic
CanonicalForm
GCD_GF (const CanonicalForm& F, const CanonicalForm& G,
        CanonicalForm& coF, CanonicalForm& coG,
        CFList& l, bool& topLevel)
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0)
  {
    coF= 0;
    coG= Lc (G);
    return G/Lc(G);
  }
  else if (G.isZero() && degree (F) > 0)
  {
    coF= Lc (F);
    coG= 0;
    return F/Lc(F);
  }
  else if (F.isZero() && G.isZero())
  {
    coF= coG= 0;
    return F.genOne();
  }
  if (F.inBaseDomain() || G.inBaseDomain())
  {
    coF= F;
    coG= G;
    return F.genOne();
  }
  if (F.isUnivariate() && fdivides(F, G, coG))
  {
    coF= Lc (F);
    return F/Lc(F);
  }
  if (G.isUnivariate() && fdivides(G, F, coF))
  {
    coG= Lc (G);
    return G/Lc(G);
  }
  if (F == G)
  {
    coF= coG= Lc (F);
    return F/Lc(F);
  }

  CFMap M,N;
  int best_level= myCompress (A, B, M, N, topLevel);

  if (best_level == 0)
  {
    coF= F;
    coG= G;
    return B.genOne();
  }

  A= M(A);
  B= M(B);

  Variable x= Variable(1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
  {
    CanonicalForm result= gcd (A, B);
    coF= N (A/result);
    coG= N (B/result);
    return N (result);
  }

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;

  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable(2), Variable (ppA.level()));
  if (d == 0)
  {
    coF= N (ppA*(cA/gcdcAcB));
    coG= N (ppB*(cB/gcdcAcB));
    return N(gcdcAcB);
  }
  int d0= totaldegree (ppB, Variable(2), Variable (ppB.level()));
  if (d0 < d)
    d= d0;
  if (d == 0)
  {
    coF= N (ppA*(cA/gcdcAcB));
    coG= N (ppB*(cB/gcdcAcB));
    return N(gcdcAcB);
  }

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly, coF_random_element, coG_random_element, coF_m,
                coG_m, ppCoF, ppCoG;

  newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  coF= 0;
  coG= 0;
  H= 0;
  bool fail= false;
  topLevel= false;
  bool inextension= false;
  int p=-1;
  int k= getGFDegree();
  int kk;
  int expon;
  char gf_name_buf= gf_name;
  int bound1= degree (ppA, 1);
  int bound2= degree (ppB, 1);
  do
  {
    random_element= GFRandomElement (m*lcA*lcB, l, fail);
    if (fail)
    {
      p= getCharacteristic();
      expon= 2;
      kk= getGFDegree();
      if (ipower (p, kk*expon) < (1 << 16))
        setCharacteristic (p, kk*(int)expon, 'b');
      else
      {
        expon= (int) floor((log ((double)((1<<16) - 1)))/(log((double)p)*kk));
        ASSERT (expon >= 2, "not enough points in GCD_GF");
        setCharacteristic (p, (int)(kk*expon), 'b');
      }
      inextension= true;
      fail= false;
      for (CFListIterator i= l; i.hasItem(); i++)
        i.getItem()= GFMapUp (i.getItem(), kk);
      m= GFMapUp (m, kk);
      G_m= GFMapUp (G_m, kk);
      newtonPoly= GFMapUp (newtonPoly, kk);
      coF_m= GFMapUp (coF_m, kk);
      coG_m= GFMapUp (coG_m, kk);
      ppA= GFMapUp (ppA, kk);
      ppB= GFMapUp (ppB, kk);
      gcdlcAlcB= GFMapUp (gcdlcAlcB, kk);
      lcA= GFMapUp (lcA, kk);
      lcB= GFMapUp (lcB, kk);
      random_element= GFRandomElement (m*lcA*lcB, l, fail);
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= GCD_GF (ppA(random_element, x), ppB(random_element, x),
                                coF_random_element, coG_random_element,
                                list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= GCD_GF (ppA(random_element, x), ppB(random_element, x),
                                coF_random_element, coG_random_element,
                                list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 == 0)
    {
      if (inextension)
      {
        ppA= GFMapDown (ppA, k);
        ppB= GFMapDown (ppB, k);
        setCharacteristic (p, k, gf_name_buf);
      }
      coF= N (ppA*(cA/gcdcAcB));
      coG= N (ppB*(cB/gcdcAcB));
      return N(gcdcAcB);
    }
    if (d0 >  d)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    G_random_element=
    (gcdlcAlcB(random_element, x)/uni_lcoeff(G_random_element)) *
     G_random_element;

    coF_random_element= (lcA(random_element,x)/uni_lcoeff(coF_random_element))
                        *coF_random_element;
    coG_random_element= (lcB(random_element,x)/uni_lcoeff(coG_random_element))
                        *coG_random_element;

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 < d)
    {
      m= gcdlcAlcB;
      newtonPoly= 1;
      G_m= 0;
      d= d0;
      coF_m= 0;
      coG_m= 0;
    }

    TIMING_START (newton_interpolation);
    H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
    coF= newtonInterp (random_element, coF_random_element, newtonPoly, coF_m,x);
    coG= newtonInterp (random_element, coG_random_element, newtonPoly, coG_m,x);
    TIMING_END_AND_PRINT (newton_interpolation,
                          "time for newton interpolation: ");

    //termination test
    if ((uni_lcoeff (H) == gcdlcAlcB) || (G_m == H))
    {
      TIMING_START (termination_test);
      if (gcdlcAlcB.isOne())
        cH= 1;
      else
        cH= uni_content (H);
      ppH= H/cH;
      ppH /= Lc (ppH);
      CanonicalForm lcppH= gcdlcAlcB/cH;
      CanonicalForm ccoF= lcppH/Lc (lcppH);
      CanonicalForm ccoG= lcppH/Lc (lcppH);
      ppCoF= coF/ccoF;
      ppCoG= coG/ccoG;
      if (inextension)
      {
        if (((degree (ppCoF,1)+degree (ppH,1) == bound1) &&
             (degree (ppCoG,1)+degree (ppH,1) == bound2) &&
             terminationTest (ppA, ppB, ppCoF, ppCoG, ppH)) ||
             (fdivides (ppH, ppA, ppCoF) && fdivides (ppH, ppB, ppCoG)))
        {
          DEBOUTLN (cerr, "ppH before mapDown= " << ppH);
          ppH= GFMapDown (ppH, k);
          ppCoF= GFMapDown (ppCoF, k);
          ppCoG= GFMapDown (ppCoG, k);
          DEBOUTLN (cerr, "ppH after mapDown= " << ppH);
          coF= N ((cA/gcdcAcB)*ppCoF);
          coG= N ((cB/gcdcAcB)*ppCoG);
          setCharacteristic (p, k, gf_name_buf);
          TIMING_END_AND_PRINT (termination_test,
                                "time for successful termination GF: ");
          return N(gcdcAcB*ppH);
        }
      }
      else
      {
      if (((degree (ppCoF,1)+degree (ppH,1) == bound1) &&
           (degree (ppCoG,1)+degree (ppH,1) == bound2) &&
           terminationTest (ppA, ppB, ppCoF, ppCoG, ppH)) ||
           (fdivides (ppH, ppA, ppCoF) && fdivides (ppH, ppB, ppCoG)))
        {
          coF= N ((cA/gcdcAcB)*ppCoF);
          coG= N ((cB/gcdcAcB)*ppCoG);
          TIMING_END_AND_PRINT (termination_test,
                                "time for successful termination GF: ");
          return N(gcdcAcB*ppH);
        }
      }
      TIMING_END_AND_PRINT (termination_test,
                            "time for unsuccessful termination GF: ");
    }

    G_m= H;
    coF_m= coF;
    coG_m= coG;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);
  } while (1);
}

/// computes a random monic irreducible univariate polynomial in x of random
/// degree < i
CanonicalForm
randomIrredpoly (int i, const Variable & x)
{
  int p= getCharacteristic();
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTLirredpoly;
  CanonicalForm CFirredpoly;
  BuildIrred (NTLirredpoly, i + 1);
  CFirredpoly= convertNTLzzpX2CF (NTLirredpoly, x);
  return CFirredpoly;
}

static inline
CanonicalForm
FpRandomElement (const CanonicalForm& F, CFList& list, bool& fail)
{
  fail= false;
  Variable x= F.mvar();
  FFRandom genFF;
  CanonicalForm random;
  int p= getCharacteristic();
  int bound= p;
  do
  {
    if (list.length() == bound)
    {
      fail= true;
      break;
    }
    if (list.length() < 1)
      random= 0;
    else
    {
      random= genFF.generate();
      while (find (list, random))
        random= genFF.generate();
    }
    if (F (random, x) == 0)
    {
      list.append (random);
      continue;
    }
  } while (find (list, random));
  return random;
}

CanonicalForm
GCD_small_p (const CanonicalForm& F, const CanonicalForm&  G,
             CanonicalForm& coF, CanonicalForm& coG,
             bool& topLevel, CFList& l);

CanonicalForm
GCD_small_p (const CanonicalForm& F, const CanonicalForm& G,
             bool& topLevel, CFList& l)
{
  CanonicalForm dummy1, dummy2;
  CanonicalForm result= GCD_small_p (F, G, dummy1, dummy2, topLevel, l);
  return result;
}

CanonicalForm
GCD_small_p (const CanonicalForm& F, const CanonicalForm&  G,
             CanonicalForm& coF, CanonicalForm& coG,
             bool& topLevel, CFList& l)
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0)
  {
    coF= 0;
    coG= Lc (G);
    return G/Lc(G);
  }
  else if (G.isZero() && degree (F) > 0)
  {
    coF= Lc (F);
    coG= 0;
    return F/Lc(F);
  }
  else if (F.isZero() && G.isZero())
  {
    coF= coG= 0;
    return F.genOne();
  }
  if (F.inBaseDomain() || G.inBaseDomain())
  {
    coF= F;
    coG= G;
    return F.genOne();
  }
  if (F.isUnivariate() && fdivides(F, G, coG))
  {
    coF= Lc (F);
    return F/Lc(F);
  }
  if (G.isUnivariate() && fdivides(G, F, coF))
  {
    coG= Lc (G);
    return G/Lc(G);
  }
  if (F == G)
  {
    coF= coG= Lc (F);
    return F/Lc(F);
  }
  CFMap M,N;
  int best_level= myCompress (A, B, M, N, topLevel);

  if (best_level == 0)
  {
    coF= F;
    coG= G;
    return B.genOne();
  }

  A= M(A);
  B= M(B);

  Variable x= Variable (1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
  {
    CanonicalForm result= gcd (A, B);
    coF= N (A/result);
    coG= N (B/result);
    return N (result);
  }

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;
  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable (2), Variable (ppA.level()));
  int d0;

  if (d == 0)
  {
    coF= N (ppA*(cA/gcdcAcB));
    coG= N (ppB*(cB/gcdcAcB));
    return N(gcdcAcB);
  }

  d0= totaldegree (ppB, Variable (2), Variable (ppB.level()));

  if (d0 < d)
    d= d0;

  if (d == 0)
  {
    coF= N (ppA*(cA/gcdcAcB));
    coG= N (ppB*(cB/gcdcAcB));
    return N(gcdcAcB);
  }

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly, coF_random_element, coG_random_element,
                coF_m, coG_m, ppCoF, ppCoG;

  newtonPoly= 1;
  m= gcdlcAlcB;
  H= 0;
  coF= 0;
  coG= 0;
  G_m= 0;
  Variable alpha, V_buf;
  bool fail= false;
  bool inextension= false;
  topLevel= false;
  CFList source, dest;
  int bound1= degree (ppA, 1);
  int bound2= degree (ppB, 1);
  do
  {
    if (inextension)
      random_element= randomElement (m*lcA*lcB, V_buf, l, fail);
    else
      random_element= FpRandomElement (m*lcA*lcB, l, fail);

    if (!fail && !inextension)
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      GCD_small_p (ppA (random_element,x), ppB (random_element,x),
                   coF_random_element, coG_random_element, topLevel,
                   list);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (!fail && inextension)
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x),
                        coF_random_element, coG_random_element, V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (fail && !inextension)
    {
      source= CFList();
      dest= CFList();
      CFList list;
      CanonicalForm mipo;
      int deg= 2;
      do {
        mipo= randomIrredpoly (deg, x);
        alpha= rootOf (mipo);
        inextension= true;
        fail= false;
        random_element= randomElement (m*lcA*lcB, alpha, l, fail);
        deg++;
      } while (fail);
      list= CFList();
      V_buf= alpha;
      TIMING_START (gcd_recursion);
      G_random_element=
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x),
                        coF_random_element, coG_random_element, alpha,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (fail && inextension)
    {
      source= CFList();
      dest= CFList();

      Variable V_buf3= V_buf;
      V_buf= chooseExtension (V_buf);
      bool prim_fail= false;
      Variable V_buf2;
      CanonicalForm prim_elem, im_prim_elem;
      prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

      if (V_buf3 != alpha)
      {
        m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
        G_m= mapDown (G_m, prim_elem, im_prim_elem, alpha, source, dest);
        coF_m= mapDown (coF_m, prim_elem, im_prim_elem, alpha, source, dest);
        coG_m= mapDown (coG_m, prim_elem, im_prim_elem, alpha, source, dest);
        newtonPoly= mapDown (newtonPoly, prim_elem, im_prim_elem, alpha,
                             source, dest);
        ppA= mapDown (ppA, prim_elem, im_prim_elem, alpha, source, dest);
        ppB= mapDown (ppB, prim_elem, im_prim_elem, alpha, source, dest);
        gcdlcAlcB= mapDown (gcdlcAlcB, prim_elem, im_prim_elem, alpha, source,
                            dest);
        lcA= mapDown (lcA, prim_elem, im_prim_elem, alpha, source, dest);
        lcB= mapDown (lcB, prim_elem, im_prim_elem, alpha, source, dest);
        for (CFListIterator i= l; i.hasItem(); i++)
          i.getItem()= mapDown (i.getItem(), prim_elem, im_prim_elem, alpha,
                                source, dest);
      }

      ASSERT (!prim_fail, "failure in integer factorizer");
      if (prim_fail)
        ; //ERROR
      else
        im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));

      for (CFListIterator i= l; i.hasItem(); i++)
        i.getItem()= mapUp (i.getItem(), alpha, V_buf, prim_elem,
                             im_prim_elem, source, dest);
      m= mapUp (m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      G_m= mapUp (G_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      coF_m= mapUp (coF_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      coG_m= mapUp (coG_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      newtonPoly= mapUp (newtonPoly, alpha, V_buf, prim_elem, im_prim_elem,
                          source, dest);
      ppA= mapUp (ppA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      ppB= mapUp (ppB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      gcdlcAlcB= mapUp (gcdlcAlcB, alpha, V_buf, prim_elem, im_prim_elem,
                         source, dest);
      lcA= mapUp (lcA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      lcB= mapUp (lcB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      fail= false;
      random_element= randomElement (m*lcA*lcB, V_buf, l, fail );
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x),
                        coF_random_element, coG_random_element, V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 == 0)
    {
      coF= N (ppA*(cA/gcdcAcB));
      coG= N (ppB*(cB/gcdcAcB));
      return N(gcdcAcB);
    }

    if (d0 >  d)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    G_random_element= (gcdlcAlcB(random_element,x)/uni_lcoeff(G_random_element))
                       *G_random_element;

    coF_random_element= (lcA(random_element,x)/uni_lcoeff(coF_random_element))
                        *coF_random_element;
    coG_random_element= (lcB(random_element,x)/uni_lcoeff(coG_random_element))
                        *coG_random_element;

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 <  d)
    {
      m= gcdlcAlcB;
      newtonPoly= 1;
      G_m= 0;
      d= d0;
      coF_m= 0;
      coG_m= 0;
    }

    TIMING_START (newton_interpolation);
    H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
    coF= newtonInterp (random_element, coF_random_element, newtonPoly, coF_m,x);
    coG= newtonInterp (random_element, coG_random_element, newtonPoly, coG_m,x);
    TIMING_END_AND_PRINT (newton_interpolation,
                          "time for newton_interpolation: ");

    //termination test
    if ((uni_lcoeff (H) == gcdlcAlcB) || (G_m == H))
    {
      TIMING_START (termination_test);
      if (gcdlcAlcB.isOne())
        cH= 1;
      else
        cH= uni_content (H);
      ppH= H/cH;
      ppH /= Lc (ppH);
      CanonicalForm lcppH= gcdlcAlcB/cH;
      CanonicalForm ccoF= lcppH/Lc (lcppH);
      CanonicalForm ccoG= lcppH/Lc (lcppH);
      ppCoF= coF/ccoF;
      ppCoG= coG/ccoG;
      DEBOUTLN (cerr, "ppH= " << ppH);
      if (((degree (ppCoF,1)+degree (ppH,1) == bound1) &&
           (degree (ppCoG,1)+degree (ppH,1) == bound2) &&
           terminationTest (ppA, ppB, ppCoF, ppCoG, ppH)) ||
           (fdivides (ppH, ppA, ppCoF) && fdivides (ppH, ppB, ppCoG)))
      {
        coF= N ((cA/gcdcAcB)*ppCoF);
        coG= N ((cB/gcdcAcB)*ppCoG);
        TIMING_END_AND_PRINT (termination_test,
                              "time for successful termination Fp: ");
        return N(gcdcAcB*ppH);
      }
      TIMING_END_AND_PRINT (termination_test,
                            "time for unsuccessful termination Fp: ");
    }

    G_m= H;
    coF_m= coF;
    coG_m= coG;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);
  } while (1);
}

CFArray
solveVandermonde (const CFArray& M, const CFArray& A)
{
  int r= M.size();
  ASSERT (A.size() == r, "vector does not have right size");

  if (r == 1)
  {
    CFArray result= CFArray (1);
    result [0]= A [0] / M [0];
    return result;
  }
  // check solvability
  bool notDistinct= false;
  for (int i= 0; i < r - 1; i++)
  {
    for (int j= i + 1; j < r; j++)
    {
      if (M [i] == M [j])
      {
        notDistinct= true;
        break;
      }
    }
  }
  if (notDistinct)
    return CFArray();

  CanonicalForm master= 1;
  Variable x= Variable (1);
  for (int i= 0; i < r; i++)
    master *= x - M [i];
  CFList Pj;
  CanonicalForm tmp;
  for (int i= 0; i < r; i++)
  {
    tmp= master/(x - M [i]);
    tmp /= tmp (M [i], 1);
    Pj.append (tmp);
  }
  CFArray result= CFArray (r);

  CFListIterator j= Pj;
  for (int i= 1; i <= r; i++, j++)
  {
    tmp= 0;
    for (int l= 0; l < A.size(); l++)
      tmp += A[l]*j.getItem()[l];
    result[i - 1]= tmp;
  }
  return result;
}

CFArray
solveGeneralVandermonde (const CFArray& M, const CFArray& A)
{
  int r= M.size();
  ASSERT (A.size() == r, "vector does not have right size");
  if (r == 1)
  {
    CFArray result= CFArray (1);
    result [0]= A[0] / M [0];
    return result;
  }
  // check solvability
  bool notDistinct= false;
  for (int i= 0; i < r - 1; i++)
  {
    for (int j= i + 1; j < r; j++)
    {
      if (M [i] == M [j])
      {
        notDistinct= true;
        break;
      }
    }
  }
  if (notDistinct)
    return CFArray();

  CanonicalForm master= 1;
  Variable x= Variable (1);
  for (int i= 0; i < r; i++)
    master *= x - M [i];
  master *= x;
  CFList Pj;
  CanonicalForm tmp;
  for (int i= 0; i < r; i++)
  {
    tmp= master/(x - M [i]);
    tmp /= tmp (M [i], 1);
    Pj.append (tmp);
  }

  CFArray result= CFArray (r);

  CFListIterator j= Pj;
  for (int i= 1; i <= r; i++, j++)
  {
    tmp= 0;

    for (int l= 1; l <= A.size(); l++)
      tmp += A[l - 1]*j.getItem()[l];
    result[i - 1]= tmp;
  }
  return result;
}

/// M in row echolon form, rk rank of M
CFArray
readOffSolution (const CFMatrix& M, const long rk)
{
  CFArray result= CFArray (rk);
  CanonicalForm tmp1, tmp2, tmp3;
  for (int i= rk; i >= 1; i--)
  {
    tmp3= 0;
    tmp1= M (i, M.columns());
    for (int j= M.columns() - 1; j >= 1; j--)
    {
      tmp2= M (i, j);
      if (j == i)
        break;
      else
        tmp3 += tmp2*result[j - 1];
    }
    result[i - 1]= (tmp1 - tmp3)/tmp2;
  }
  return result;
}

CFArray
readOffSolution (const CFMatrix& M, const CFArray& L, const CFArray& partialSol)
{
  CFArray result= CFArray (M.rows());
  CanonicalForm tmp1, tmp2, tmp3;
  int k;
  for (int i= M.rows(); i >= 1; i--)
  {
    tmp3= 0;
    tmp1= L[i - 1];
    k= 0;
    for (int j= M.columns(); j >= 1; j--, k++)
    {
      tmp2= M (i, j);
      if (j == i)
        break;
      else
      {
        if (k > partialSol.size() - 1)
          tmp3 += tmp2*result[j - 1];
        else
          tmp3 += tmp2*partialSol[partialSol.size() - k - 1];
      }
    }
    result [i - 1]= (tmp1 - tmp3)/tmp2;
  }
  return result;
}

long
gaussianElimFp (CFMatrix& M, CFArray& L)
{
  ASSERT (L.size() <= M.rows(), "dimension exceeded");
  CFMatrix *N;
  N= new CFMatrix (M.rows(), M.columns() + 1);

  for (int i= 1; i <= M.rows(); i++)
    for (int j= 1; j <= M.columns(); j++)
      (*N) (i, j)= M (i, j);

  int j= 1;
  for (int i= 0; i < L.size(); i++, j++)
    (*N) (j, M.columns() + 1)= L[i];
#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  convertFacCFMatrix2nmod_mat_t (FLINTN, *N);
  long rk= nmod_mat_rref (FLINTN);

  delete N;
  N= convertNmod_mat_t2FacCFMatrix (FLINTN);
  nmod_mat_clear (FLINTN);
#else
  int p= getCharacteristic ();
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  mat_zz_p *NTLN= convertFacCFMatrix2NTLmat_zz_p(*N);
  delete N;
  long rk= gauss (*NTLN);

  N= convertNTLmat_zz_p2FacCFMatrix (*NTLN);
  delete NTLN;
#endif

  L= CFArray (M.rows());
  for (int i= 0; i < M.rows(); i++)
    L[i]= (*N) (i + 1, M.columns() + 1);
  M= (*N) (1, M.rows(), 1, M.columns());
  delete N;
  return rk;
}

long
gaussianElimFq (CFMatrix& M, CFArray& L, const Variable& alpha)
{
  ASSERT (L.size() <= M.rows(), "dimension exceeded");
  CFMatrix *N;
  N= new CFMatrix (M.rows(), M.columns() + 1);

  for (int i= 1; i <= M.rows(); i++)
    for (int j= 1; j <= M.columns(); j++)
      (*N) (i, j)= M (i, j);

  int j= 1;
  for (int i= 0; i < L.size(); i++, j++)
    (*N) (j, M.columns() + 1)= L[i];
  int p= getCharacteristic ();
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
  zz_pE::init (NTLMipo);
  mat_zz_pE *NTLN= convertFacCFMatrix2NTLmat_zz_pE(*N);
  long rk= gauss (*NTLN);

  delete N;
  N= convertNTLmat_zz_pE2FacCFMatrix (*NTLN, alpha);

  delete NTLN;

  M= (*N) (1, M.rows(), 1, M.columns());
  L= CFArray (M.rows());
  for (int i= 0; i < M.rows(); i++)
    L[i]= (*N) (i + 1, M.columns() + 1);

  delete N;
  return rk;
}

CFArray
solveSystemFp (const CFMatrix& M, const CFArray& L)
{
  ASSERT (L.size() <= M.rows(), "dimension exceeded");
  CFMatrix *N;
  N= new CFMatrix (M.rows(), M.columns() + 1);

  for (int i= 1; i <= M.rows(); i++)
    for (int j= 1; j <= M.columns(); j++)
      (*N) (i, j)= M (i, j);

  int j= 1;
  for (int i= 0; i < L.size(); i++, j++)
    (*N) (j, M.columns() + 1)= L[i];

#ifdef HAVE_FLINT
  nmod_mat_t FLINTN;
  convertFacCFMatrix2nmod_mat_t (FLINTN, *N);
  long rk= nmod_mat_rref (FLINTN);
#else
  int p= getCharacteristic ();
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  mat_zz_p *NTLN= convertFacCFMatrix2NTLmat_zz_p(*N);
  long rk= gauss (*NTLN);
#endif
  delete N;
  if (rk != M.columns())
  {
#ifdef HAVE_FLINT
    nmod_mat_clear (FLINTN);
#else
    delete NTLN;
#endif
    return CFArray();
  }
#ifdef HAVE_FLINT
  N= convertNmod_mat_t2FacCFMatrix (FLINTN);
  nmod_mat_clear (FLINTN);
#else
  N= convertNTLmat_zz_p2FacCFMatrix (*NTLN);
  delete NTLN;
#endif
  CFArray A= readOffSolution (*N, rk);

  delete N;
  return A;
}

CFArray
solveSystemFq (const CFMatrix& M, const CFArray& L, const Variable& alpha)
{
  ASSERT (L.size() <= M.rows(), "dimension exceeded");
  CFMatrix *N;
  N= new CFMatrix (M.rows(), M.columns() + 1);

  for (int i= 1; i <= M.rows(); i++)
    for (int j= 1; j <= M.columns(); j++)
      (*N) (i, j)= M (i, j);
  int j= 1;
  for (int i= 0; i < L.size(); i++, j++)
    (*N) (j, M.columns() + 1)= L[i];
  int p= getCharacteristic ();
  if (fac_NTL_char != p)
  {
    fac_NTL_char= p;
    zz_p::init (p);
  }
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
  zz_pE::init (NTLMipo);
  mat_zz_pE *NTLN= convertFacCFMatrix2NTLmat_zz_pE(*N);
  long rk= gauss (*NTLN);

  delete N;
  if (rk != M.columns())
  {
    delete NTLN;
    return CFArray();
  }
  N= convertNTLmat_zz_pE2FacCFMatrix (*NTLN, alpha);

  delete NTLN;

  CFArray A= readOffSolution (*N, rk);

  delete N;
  return A;
}
#endif

CFArray
getMonoms (const CanonicalForm& F)
{
  if (F.inCoeffDomain())
  {
    CFArray result= CFArray (1);
    result [0]= 1;
    return result;
  }
  if (F.isUnivariate())
  {
    CFArray result= CFArray (size(F));
    int j= 0;
    for (CFIterator i= F; i.hasTerms(); i++, j++)
      result[j]= power (F.mvar(), i.exp());
    return result;
  }
  int numMon= size (F);
  CFArray result= CFArray (numMon);
  int j= 0;
  CFArray recResult;
  Variable x= F.mvar();
  CanonicalForm powX;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    powX= power (x, i.exp());
    recResult= getMonoms (i.coeff());
    for (int k= 0; k < recResult.size(); k++)
      result[j+k]= powX*recResult[k];
    j += recResult.size();
  }
  return result;
}

#ifdef HAVE_NTL
CFArray
evaluateMonom (const CanonicalForm& F, const CFList& evalPoints)
{
  if (F.inCoeffDomain())
  {
    CFArray result= CFArray (1);
    result [0]= F;
    return result;
  }
  if (F.isUnivariate())
  {
    ASSERT (evalPoints.length() == 1,
            "expected an eval point with only one component");
    CFArray result= CFArray (size(F));
    int j= 0;
    CanonicalForm evalPoint= evalPoints.getLast();
    for (CFIterator i= F; i.hasTerms(); i++, j++)
      result[j]= power (evalPoint, i.exp());
    return result;
  }
  int numMon= size (F);
  CFArray result= CFArray (numMon);
  int j= 0;
  CanonicalForm evalPoint= evalPoints.getLast();
  CFList buf= evalPoints;
  buf.removeLast();
  CFArray recResult;
  CanonicalForm powEvalPoint;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    powEvalPoint= power (evalPoint, i.exp());
    recResult= evaluateMonom (i.coeff(), buf);
    for (int k= 0; k < recResult.size(); k++)
      result[j+k]= powEvalPoint*recResult[k];
    j += recResult.size();
  }
  return result;
}

CFArray
evaluate (const CFArray& A, const CFList& evalPoints)
{
  CFArray result= A.size();
  CanonicalForm tmp;
  int k;
  for (int i= 0; i < A.size(); i++)
  {
    tmp= A[i];
    k= 1;
    for (CFListIterator j= evalPoints; j.hasItem(); j++, k++)
      tmp= tmp (j.getItem(), k);
    result[i]= tmp;
  }
  return result;
}

CFList
evaluationPoints (const CanonicalForm& F, const CanonicalForm& G,
                  CanonicalForm& Feval, CanonicalForm& Geval,
                  const CanonicalForm& LCF, const bool& GF,
                  const Variable& alpha, bool& fail, CFList& list
                 )
{
  int k= tmax (F.level(), G.level()) - 1;
  Variable x= Variable (1);
  CFList result;
  FFRandom genFF;
  GFRandom genGF;
  int p= getCharacteristic ();
  double bound;
  if (alpha != Variable (1))
  {
    bound= pow ((double) p, (double) degree (getMipo(alpha)));
    bound= pow (bound, (double) k);
  }
  else if (GF)
  {
    bound= pow ((double) p, (double) getGFDegree());
    bound= pow ((double) bound, (double) k);
  }
  else
    bound= pow ((double) p, (double) k);

  CanonicalForm random;
  int j;
  bool zeroOneOccured= false;
  bool allEqual= false;
  CanonicalForm buf;
  do
  {
    random= 0;
    // possible overflow if list.length() does not fit into a int
    if (list.length() >= bound)
    {
      fail= true;
      break;
    }
    for (int i= 0; i < k; i++)
    {
      if (GF)
      {
        result.append (genGF.generate());
        random += result.getLast()*power (x, i);
      }
      else if (alpha.level() != 1)
      {
        AlgExtRandomF genAlgExt (alpha);
        result.append (genAlgExt.generate());
        random += result.getLast()*power (x, i);
      }
      else
      {
        result.append (genFF.generate());
        random += result.getLast()*power (x, i);
      }
      if (result.getLast().isOne() || result.getLast().isZero())
        zeroOneOccured= true;
    }
    if (find (list, random))
    {
      zeroOneOccured= false;
      allEqual= false;
      result= CFList();
      continue;
    }
    if (zeroOneOccured)
    {
      list.append (random);
      zeroOneOccured= false;
      allEqual= false;
      result= CFList();
      continue;
    }
    // no zero at this point
    if (k > 1)
    {
      allEqual= true;
      CFIterator iter= random;
      buf= iter.coeff();
      iter++;
      for (; iter.hasTerms(); iter++)
        if (buf != iter.coeff())
          allEqual= false;
    }
    if (allEqual)
    {
      list.append (random);
      allEqual= false;
      zeroOneOccured= false;
      result= CFList();
      continue;
    }

    Feval= F;
    Geval= G;
    CanonicalForm LCeval= LCF;
    j= 1;
    for (CFListIterator i= result; i.hasItem(); i++, j++)
    {
      Feval= Feval (i.getItem(), j);
      Geval= Geval (i.getItem(), j);
      LCeval= LCeval (i.getItem(), j);
    }

    if (LCeval.isZero())
    {
      if (!find (list, random))
        list.append (random);
      zeroOneOccured= false;
      allEqual= false;
      result= CFList();
      continue;
    }

    if (list.length() >= bound)
    {
      fail= true;
      break;
    }
  } while (find (list, random));

  return result;
}

/// multiply two lists componentwise
void mult (CFList& L1, const CFList& L2)
{
  ASSERT (L1.length() == L2.length(), "lists of the same size expected");

  CFListIterator j= L2;
  for (CFListIterator i= L1; i.hasItem(); i++, j++)
    i.getItem() *= j.getItem();
}

void eval (const CanonicalForm& A, const CanonicalForm& B, CanonicalForm& Aeval,
           CanonicalForm& Beval, const CFList& L)
{
  Aeval= A;
  Beval= B;
  int j= 1;
  for (CFListIterator i= L; i.hasItem(); i++, j++)
  {
    Aeval= Aeval (i.getItem(), j);
    Beval= Beval (i.getItem(), j);
  }
}

CanonicalForm
monicSparseInterpol (const CanonicalForm& F, const CanonicalForm& G,
                     const CanonicalForm& skeleton, const Variable& alpha,
                     bool& fail, CFArray*& coeffMonoms, CFArray& Monoms
                    )
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0) return G/Lc(G);
  else if (G.isZero() && degree (F) > 0) return F/Lc(F);
  else if (F.isZero() && G.isZero()) return F.genOne();
  if (F.inBaseDomain() || G.inBaseDomain()) return F.genOne();
  if (F.isUnivariate() && fdivides(F, G)) return F/Lc(F);
  if (G.isUnivariate() && fdivides(G, F)) return G/Lc(G);
  if (F == G) return F/Lc(F);

  ASSERT (degree (A, 1) == 0, "expected degree (F, 1) == 0");
  ASSERT (degree (B, 1) == 0, "expected degree (G, 1) == 0");

  CFMap M,N;
  int best_level= myCompress (A, B, M, N, false);

  if (best_level == 0)
    return B.genOne();

  A= M(A);
  B= M(B);

  Variable x= Variable (1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
    return N (gcd (A, B));

  CanonicalForm skel= M(skeleton);
  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;
  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;
  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  if (fdivides (lcA, lcB))
  {
    if (fdivides (A, B))
      return F/Lc(F);
  }
  if (fdivides (lcB, lcA))
  {
    if (fdivides (B, A))
      return G/Lc(G);
  }

  gcdlcAlcB= gcd (lcA, lcB);
  int skelSize= size (skel, skel.mvar());

  int j= 0;
  int biggestSize= 0;

  for (CFIterator i= skel; i.hasTerms(); i++, j++)
    biggestSize= tmax (biggestSize, size (i.coeff()));

  CanonicalForm g, Aeval, Beval;

  CFList evalPoints;
  bool evalFail= false;
  CFList list;
  bool GF= false;
  CanonicalForm LCA= LC (A);
  CanonicalForm tmp;
  CFArray gcds= CFArray (biggestSize);
  CFList * pEvalPoints= new CFList [biggestSize];
  Variable V_buf= alpha;
  CFList source, dest;
  CanonicalForm prim_elem, im_prim_elem;
  for (int i= 0; i < biggestSize; i++)
  {
    if (i == 0)
      evalPoints= evaluationPoints (A, B, Aeval, Beval, LCA, GF, V_buf, evalFail,
                                    list);
    else
    {
      mult (evalPoints, pEvalPoints [0]);
      eval (A, B, Aeval, Beval, evalPoints);
    }

    if (evalFail)
    {
      if (V_buf.level() != 1)
      {
        do
        {
          Variable V_buf2= chooseExtension (V_buf);
          source= CFList();
          dest= CFList();

          bool prim_fail= false;
          Variable V_buf3;
          prim_elem= primitiveElement (V_buf, V_buf3, prim_fail);

          ASSERT (!prim_fail, "failure in integer factorizer");
          if (prim_fail)
            ; //ERROR
          else
            im_prim_elem= mapPrimElem (prim_elem, V_buf, V_buf2);

          DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf));
          DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));

          for (CFListIterator j= list; j.hasItem(); j++)
            j.getItem()= mapUp (j.getItem(), V_buf, V_buf2, prim_elem,
                                im_prim_elem, source, dest);
          for (int k= 0; k < i; k++)
          {
            for (CFListIterator j= pEvalPoints[k]; j.hasItem(); j++)
              j.getItem()= mapUp (j.getItem(), V_buf, V_buf2, prim_elem,
                                  im_prim_elem, source, dest);
            gcds[k]= mapUp (gcds[k], V_buf, V_buf2, prim_elem, im_prim_elem,
                            source, dest);
          }

          if (alpha.level() != 1)
          {
            A= mapUp (A, V_buf, V_buf2, prim_elem, im_prim_elem, source,dest);
            B= mapUp (B, V_buf, V_buf2, prim_elem, im_prim_elem, source,dest);
          }
          evalFail= false;
          evalPoints= evaluationPoints (A, B, Aeval, Beval, LCA, GF, V_buf,
                                        evalFail, list);
        } while (evalFail);
      }
      else
      {
        CanonicalForm mipo;
        int deg= 2;
        do {
          mipo= randomIrredpoly (deg, x);
          V_buf= rootOf (mipo);
          evalFail= false;
          evalPoints= evaluationPoints (A, B, Aeval, Beval, LCA, GF, V_buf,
                                        evalFail, list);
          deg++;
        } while (evalFail);
      }
    }

    g= gcd (Aeval, Beval);
    g /= Lc (g);

    if (degree (g) != degree (skel) || (skelSize != size (g)))
    {
      delete[] pEvalPoints;
      fail= true;
      return 0;
    }
    CFIterator l= skel;
    for (CFIterator k= g; k.hasTerms(); k++, l++)
    {
      if (k.exp() != l.exp())
      {
        delete[] pEvalPoints;
        fail= true;
        return 0;
      }
    }
    pEvalPoints[i]= evalPoints;
    gcds[i]= g;

    tmp= 0;
    int j= 0;
    for (CFListIterator k= evalPoints; k.hasItem(); k++, j++)
      tmp += k.getItem()*power (x, j);
    list.append (tmp);
  }

  if (Monoms.size() == 0)
    Monoms= getMonoms (skel);

  coeffMonoms= new CFArray [skelSize];
  j= 0;
  for (CFIterator i= skel; i.hasTerms(); i++, j++)
    coeffMonoms[j]= getMonoms (i.coeff());

  CFArray* pL= new CFArray [skelSize];
  CFArray* pM= new CFArray [skelSize];
  for (int i= 0; i < biggestSize; i++)
  {
    CFIterator l= gcds [i];
    evalPoints= pEvalPoints [i];
    for (int k= 0; k < skelSize; k++, l++)
    {
      if (i == 0)
        pL[k]= CFArray (biggestSize);
      pL[k] [i]= l.coeff();

      if (i == 0)
        pM[k]= evaluate (coeffMonoms [k], evalPoints);
    }
  }

  CFArray solution;
  CanonicalForm result= 0;
  int ind= 0;
  CFArray bufArray;
  CFMatrix Mat;
  for (int k= 0; k < skelSize; k++)
  {
    if (biggestSize != coeffMonoms[k].size())
    {
      bufArray= CFArray (coeffMonoms[k].size());
      for (int i= 0; i < coeffMonoms[k].size(); i++)
        bufArray [i]= pL[k] [i];
      solution= solveGeneralVandermonde (pM [k], bufArray);
    }
    else
      solution= solveGeneralVandermonde (pM [k], pL[k]);

    if (solution.size() == 0)
    {
      delete[] pEvalPoints;
      delete[] pM;
      delete[] pL;
      delete[] coeffMonoms;
      fail= true;
      return 0;
    }
    for (int l= 0; l < solution.size(); l++)
      result += solution[l]*Monoms [ind + l];
    ind += solution.size();
  }

  delete[] pEvalPoints;
  delete[] pM;
  delete[] pL;
  delete[] coeffMonoms;

  if (alpha.level() != 1 && V_buf != alpha)
  {
    CFList u, v;
    result= mapDown (result, prim_elem, im_prim_elem, alpha, u, v);
  }

  result= N(result);
  if (fdivides (result, F) && fdivides (result, G))
    return result;
  else
  {
    fail= true;
    return 0;
  }
}

CanonicalForm
nonMonicSparseInterpol (const CanonicalForm& F, const CanonicalForm& G,
                        const CanonicalForm& skeleton, const Variable& alpha,
                        bool& fail, CFArray*& coeffMonoms, CFArray& Monoms
                       )
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0) return G/Lc(G);
  else if (G.isZero() && degree (F) > 0) return F/Lc(F);
  else if (F.isZero() && G.isZero()) return F.genOne();
  if (F.inBaseDomain() || G.inBaseDomain()) return F.genOne();
  if (F.isUnivariate() && fdivides(F, G)) return F/Lc(F);
  if (G.isUnivariate() && fdivides(G, F)) return G/Lc(G);
  if (F == G) return F/Lc(F);

  ASSERT (degree (A, 1) == 0, "expected degree (F, 1) == 0");
  ASSERT (degree (B, 1) == 0, "expected degree (G, 1) == 0");

  CFMap M,N;
  int best_level= myCompress (A, B, M, N, false);

  if (best_level == 0)
    return B.genOne();

  A= M(A);
  B= M(B);

  Variable x= Variable (1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
    return N (gcd (A, B));

  CanonicalForm skel= M(skeleton);

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;
  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;
  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  if (fdivides (lcA, lcB))
  {
    if (fdivides (A, B))
      return F/Lc(F);
  }
  if (fdivides (lcB, lcA))
  {
    if (fdivides (B, A))
      return G/Lc(G);
  }

  gcdlcAlcB= gcd (lcA, lcB);
  int skelSize= size (skel, skel.mvar());

  int j= 0;
  int biggestSize= 0;
  int bufSize;
  int numberUni= 0;
  for (CFIterator i= skel; i.hasTerms(); i++, j++)
  {
    bufSize= size (i.coeff());
    biggestSize= tmax (biggestSize, bufSize);
    numberUni += bufSize;
  }
  numberUni--;
  numberUni= (int) ceil ( (double) numberUni / (skelSize - 1));
  biggestSize= tmax (biggestSize , numberUni);

  numberUni= biggestSize + size (LC(skel)) - 1;
  int biggestSize2= tmax (numberUni, biggestSize);

  CanonicalForm g, Aeval, Beval;

  CFList evalPoints;
  CFArray coeffEval;
  bool evalFail= false;
  CFList list;
  bool GF= false;
  CanonicalForm LCA= LC (A);
  CanonicalForm tmp;
  CFArray gcds= CFArray (biggestSize);
  CFList * pEvalPoints= new CFList [biggestSize];
  Variable V_buf= alpha;
  CFList source, dest;
  CanonicalForm prim_elem, im_prim_elem;
  for (int i= 0; i < biggestSize; i++)
  {
    if (i == 0)
    {
      if (getCharacteristic() > 3)
        evalPoints= evaluationPoints (A, B, Aeval, Beval, LCA, GF, V_buf,
                                    evalFail, list);
      else
        evalFail= true;

      if (evalFail)
      {
        if (V_buf.level() != 1)
        {
          do
          {
            Variable V_buf2= chooseExtension (V_buf);
            source= CFList();
            dest= CFList();

            bool prim_fail= false;
            Variable V_buf3;
            prim_elem= primitiveElement (V_buf, V_buf3, prim_fail);

            ASSERT (!prim_fail, "failure in integer factorizer");
            if (prim_fail)
              ; //ERROR
            else
              im_prim_elem= mapPrimElem (prim_elem, V_buf, V_buf2);

            DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf));
            DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));

            for (CFListIterator i= list; i.hasItem(); i++)
              i.getItem()= mapUp (i.getItem(), V_buf, V_buf2, prim_elem,
                                im_prim_elem, source, dest);
            evalFail= false;
            evalPoints= evaluationPoints (A, B, Aeval, Beval, LCA, GF, V_buf,
                                          evalFail, list);
          } while (evalFail);
        }
        else
        {
          CanonicalForm mipo;
          int deg= 2;
          do {
            mipo= randomIrredpoly (deg, x);
            V_buf= rootOf (mipo);
            evalFail= false;
            evalPoints= evaluationPoints (A, B, Aeval, Beval, LCA, GF, V_buf,
                                          evalFail, list);
            deg++;
          } while (evalFail);
        }
      }
    }
    else
    {
      mult (evalPoints, pEvalPoints[0]);
      eval (A, B, Aeval, Beval, evalPoints);
    }

    g= gcd (Aeval, Beval);
    g /= Lc (g);

    if (degree (g) != degree (skel) || (skelSize != size (g)))
    {
      delete[] pEvalPoints;
      fail= true;
      return 0;
    }
    CFIterator l= skel;
    for (CFIterator k= g; k.hasTerms(); k++, l++)
    {
      if (k.exp() != l.exp())
      {
        delete[] pEvalPoints;
        fail= true;
        return 0;
      }
    }
    pEvalPoints[i]= evalPoints;
    gcds[i]= g;

    tmp= 0;
    int j= 0;
    for (CFListIterator k= evalPoints; k.hasItem(); k++, j++)
      tmp += k.getItem()*power (x, j);
    list.append (tmp);
  }

  if (Monoms.size() == 0)
    Monoms= getMonoms (skel);

  coeffMonoms= new CFArray [skelSize];

  j= 0;
  for (CFIterator i= skel; i.hasTerms(); i++, j++)
    coeffMonoms[j]= getMonoms (i.coeff());

  int minimalColumnsIndex;
  if (skelSize > 1)
    minimalColumnsIndex= 1;
  else
    minimalColumnsIndex= 0;
  int minimalColumns=-1;

  CFArray* pM= new CFArray [skelSize];
  CFMatrix Mat;
  // find the Matrix with minimal number of columns
  for (int i= 0; i < skelSize; i++)
  {
    pM[i]= CFArray (coeffMonoms[i].size());
    if (i == 1)
      minimalColumns= coeffMonoms[i].size();
    if (i > 1)
    {
      if (minimalColumns > coeffMonoms[i].size())
      {
        minimalColumns= coeffMonoms[i].size();
        minimalColumnsIndex= i;
      }
    }
  }
  CFMatrix* pMat= new CFMatrix [2];
  pMat[0]= CFMatrix (biggestSize, coeffMonoms[0].size());
  pMat[1]= CFMatrix (biggestSize, coeffMonoms[minimalColumnsIndex].size());
  CFArray* pL= new CFArray [skelSize];
  for (int i= 0; i < biggestSize; i++)
  {
    CFIterator l= gcds [i];
    evalPoints= pEvalPoints [i];
    for (int k= 0; k < skelSize; k++, l++)
    {
      if (i == 0)
        pL[k]= CFArray (biggestSize);
      pL[k] [i]= l.coeff();

      if (i == 0 && k != 0 && k != minimalColumnsIndex)
        pM[k]= evaluate (coeffMonoms[k], evalPoints);
      else if (i == 0 && (k == 0 || k == minimalColumnsIndex))
        coeffEval= evaluate (coeffMonoms[k], evalPoints);
      if (i > 0 && (k == 0 || k == minimalColumnsIndex))
        coeffEval= evaluate (coeffMonoms[k], evalPoints);

      if (k == 0)
      {
        if (pMat[k].rows() >= i + 1)
        {
          for (int ind= 1; ind < coeffEval.size() + 1; ind++)
            pMat[k] (i + 1, ind)= coeffEval[ind - 1];
        }
      }
      if (k == minimalColumnsIndex)
      {
        if (pMat[1].rows() >= i + 1)
        {
          for (int ind= 1; ind < coeffEval.size() + 1; ind++)
            pMat[1] (i + 1, ind)= coeffEval[ind - 1];
        }
      }
    }
  }

  CFArray solution;
  CanonicalForm result= 0;
  int ind= 1;
  int matRows, matColumns;
  matRows= pMat[1].rows();
  matColumns= pMat[0].columns() - 1;
  matColumns += pMat[1].columns();

  Mat= CFMatrix (matRows, matColumns);
  for (int i= 1; i <= matRows; i++)
    for (int j= 1; j <= pMat[1].columns(); j++)
      Mat (i, j)= pMat[1] (i, j);

  for (int j= pMat[1].columns() + 1; j <= matColumns;
       j++, ind++)
  {
    for (int i= 1; i <= matRows; i++)
      Mat (i, j)= (-pMat [0] (i, ind + 1))*pL[minimalColumnsIndex] [i - 1];
  }

  CFArray firstColumn= CFArray (pMat[0].rows());
  for (int i= 0; i < pMat[0].rows(); i++)
    firstColumn [i]= pMat[0] (i + 1, 1);

  CFArray bufArray= pL[minimalColumnsIndex];

  for (int i= 0; i < biggestSize; i++)
    pL[minimalColumnsIndex] [i] *= firstColumn[i];

  CFMatrix bufMat= pMat[1];
  pMat[1]= Mat;

  if (V_buf.level() != 1)
    solution= solveSystemFq (pMat[1],
                             pL[minimalColumnsIndex], V_buf);
  else
    solution= solveSystemFp (pMat[1],
                             pL[minimalColumnsIndex]);

  if (solution.size() == 0)
  { //Javadi's method failed, try deKleine, Monagan, Wittkopf instead
    CFMatrix bufMat0= pMat[0];
    delete [] pMat;
    pMat= new CFMatrix [skelSize];
    pL[minimalColumnsIndex]= bufArray;
    CFList* bufpEvalPoints= NULL;
    CFArray bufGcds;
    if (biggestSize != biggestSize2)
    {
      bufpEvalPoints= pEvalPoints;
      pEvalPoints= new CFList [biggestSize2];
      bufGcds= gcds;
      gcds= CFArray (biggestSize2);
      for (int i= 0; i < biggestSize; i++)
      {
        pEvalPoints[i]= bufpEvalPoints [i];
        gcds[i]= bufGcds[i];
      }
      for (int i= 0; i < biggestSize2 - biggestSize; i++)
      {
        mult (evalPoints, pEvalPoints[0]);
        eval (A, B, Aeval, Beval, evalPoints);
        g= gcd (Aeval, Beval);
        g /= Lc (g);
        if (degree (g) != degree (skel) || (skelSize != size (g)))
        {
          delete[] pEvalPoints;
          delete[] pMat;
          delete[] pL;
          delete[] coeffMonoms;
          delete[] pM;
          if (bufpEvalPoints != NULL)
            delete [] bufpEvalPoints;
          fail= true;
          return 0;
        }
        CFIterator l= skel;
        for (CFIterator k= g; k.hasTerms(); k++, l++)
        {
          if (k.exp() != l.exp())
          {
            delete[] pEvalPoints;
            delete[] pMat;
            delete[] pL;
            delete[] coeffMonoms;
            delete[] pM;
            if (bufpEvalPoints != NULL)
              delete [] bufpEvalPoints;
            fail= true;
            return 0;
          }
        }
        pEvalPoints[i + biggestSize]= evalPoints;
        gcds[i + biggestSize]= g;
      }
    }
    for (int i= 0; i < biggestSize; i++)
    {
      CFIterator l= gcds [i];
      evalPoints= pEvalPoints [i];
      for (int k= 1; k < skelSize; k++, l++)
      {
        if (i == 0)
          pMat[k]= CFMatrix (biggestSize2,coeffMonoms[k].size()+biggestSize2-1);
        if (k == minimalColumnsIndex)
          continue;
        coeffEval= evaluate (coeffMonoms[k], evalPoints);
        if (pMat[k].rows() >= i + 1)
        {
          for (int ind= 1; ind < coeffEval.size() + 1; ind++)
            pMat[k] (i + 1, ind)= coeffEval[ind - 1];
        }
      }
    }
    Mat= bufMat0;
    pMat[0]= CFMatrix (biggestSize2, coeffMonoms[0].size() + biggestSize2 - 1);
    for (int j= 1; j <= Mat.rows(); j++)
      for (int k= 1; k <= Mat.columns(); k++)
        pMat [0] (j,k)= Mat (j,k);
    Mat= bufMat;
    for (int j= 1; j <= Mat.rows(); j++)
      for (int k= 1; k <= Mat.columns(); k++)
        pMat [minimalColumnsIndex] (j,k)= Mat (j,k);
    // write old matrix entries into new matrices
    for (int i= 0; i < skelSize; i++)
    {
      bufArray= pL[i];
      pL[i]= CFArray (biggestSize2);
      for (int j= 0; j < bufArray.size(); j++)
        pL[i] [j]= bufArray [j];
    }
    //write old vector entries into new and add new entries to old matrices
    for (int i= 0; i < biggestSize2 - biggestSize; i++)
    {
      CFIterator l= gcds [i + biggestSize];
      evalPoints= pEvalPoints [i + biggestSize];
      for (int k= 0; k < skelSize; k++, l++)
      {
        pL[k] [i + biggestSize]= l.coeff();
        coeffEval= evaluate (coeffMonoms[k], evalPoints);
        if (pMat[k].rows() >= i + biggestSize + 1)
        {
          for (int ind= 1; ind < coeffEval.size() + 1; ind++)
            pMat[k] (i + biggestSize + 1, ind)= coeffEval[ind - 1];
        }
      }
    }
    // begin new
    for (int i= 0; i < skelSize; i++)
    {
      if (pL[i].size() > 1)
      {
        for (int j= 2; j <= pMat[i].rows(); j++)
          pMat[i] (j, coeffMonoms[i].size() + j - 1)=
              -pL[i] [j - 1];
      }
    }

    matColumns= biggestSize2 - 1;
    matRows= 0;
    for (int i= 0; i < skelSize; i++)
    {
      if (V_buf.level() == 1)
        (void) gaussianElimFp (pMat[i], pL[i]);
      else
        (void) gaussianElimFq (pMat[i], pL[i], V_buf);

      if (pMat[i] (coeffMonoms[i].size(), coeffMonoms[i].size()) == 0)
      {
        delete[] pEvalPoints;
        delete[] pMat;
        delete[] pL;
        delete[] coeffMonoms;
        delete[] pM;
        if (bufpEvalPoints != NULL)
          delete [] bufpEvalPoints;
        fail= true;
        return 0;
      }
      matRows += pMat[i].rows() - coeffMonoms[i].size();
    }

    CFMatrix bufMat;
    Mat= CFMatrix (matRows, matColumns);
    ind= 0;
    bufArray= CFArray (matRows);
    CFArray bufArray2;
    for (int i= 0; i < skelSize; i++)
    {
      bufMat= pMat[i] (coeffMonoms[i].size() + 1, pMat[i].rows(),
                       coeffMonoms[i].size() + 1, pMat[i].columns());

      for (int j= 1; j <= bufMat.rows(); j++)
        for (int k= 1; k <= bufMat.columns(); k++)
          Mat (j + ind, k)= bufMat(j, k);
      bufArray2= coeffMonoms[i].size();
      for (int j= 1; j <= pMat[i].rows(); j++)
      {
        if (j > coeffMonoms[i].size())
          bufArray [j-coeffMonoms[i].size() + ind - 1]= pL[i] [j - 1];
        else
          bufArray2 [j - 1]= pL[i] [j - 1];
      }
      pL[i]= bufArray2;
      ind += bufMat.rows();
      pMat[i]= pMat[i] (1, coeffMonoms[i].size(), 1, pMat[i].columns());
    }

    if (V_buf.level() != 1)
      solution= solveSystemFq (Mat, bufArray, V_buf);
    else
      solution= solveSystemFp (Mat, bufArray);

    if (solution.size() == 0)
    {
      delete[] pEvalPoints;
      delete[] pMat;
      delete[] pL;
      delete[] coeffMonoms;
      delete[] pM;
      if (bufpEvalPoints != NULL)
        delete [] bufpEvalPoints;
      fail= true;
      return 0;
    }

    ind= 0;
    result= 0;
    CFArray bufSolution;
    for (int i= 0; i < skelSize; i++)
    {
      bufSolution= readOffSolution (pMat[i], pL[i], solution);
      for (int i= 0; i < bufSolution.size(); i++, ind++)
        result += Monoms [ind]*bufSolution[i];
    }
    if (alpha.level() != 1 && V_buf != alpha)
    {
      CFList u, v;
      result= mapDown (result, prim_elem, im_prim_elem, alpha, u, v);
    }
    result= N(result);
    delete[] pEvalPoints;
    delete[] pMat;
    delete[] pL;
    delete[] coeffMonoms;
    delete[] pM;

    if (bufpEvalPoints != NULL)
      delete [] bufpEvalPoints;
    if (fdivides (result, F) && fdivides (result, G))
      return result;
    else
    {
      fail= true;
      return 0;
    }
  } // end of deKleine, Monagan & Wittkopf

  result += Monoms[0];
  int ind2= 0, ind3= 2;
  ind= 0;
  for (int l= 0; l < minimalColumnsIndex; l++)
    ind += coeffMonoms[l].size();
  for (int l= solution.size() - pMat[0].columns() + 1; l < solution.size();
       l++, ind2++, ind3++)
  {
    result += solution[l]*Monoms [1 + ind2];
    for (int i= 0; i < pMat[0].rows(); i++)
      firstColumn[i] += solution[l]*pMat[0] (i + 1, ind3);
  }
  for (int l= 0; l < solution.size() + 1 - pMat[0].columns(); l++)
    result += solution[l]*Monoms [ind + l];
  ind= coeffMonoms[0].size();
  for (int k= 1; k < skelSize; k++)
  {
    if (k == minimalColumnsIndex)
    {
      ind += coeffMonoms[k].size();
      continue;
    }
    if (k != minimalColumnsIndex)
    {
      for (int i= 0; i < biggestSize; i++)
        pL[k] [i] *= firstColumn [i];
    }

    if (biggestSize != coeffMonoms[k].size() && k != minimalColumnsIndex)
    {
      bufArray= CFArray (coeffMonoms[k].size());
      for (int i= 0; i < bufArray.size(); i++)
        bufArray [i]= pL[k] [i];
      solution= solveGeneralVandermonde (pM[k], bufArray);
    }
    else
      solution= solveGeneralVandermonde (pM[k], pL[k]);

    if (solution.size() == 0)
    {
      delete[] pEvalPoints;
      delete[] pMat;
      delete[] pL;
      delete[] coeffMonoms;
      delete[] pM;
      fail= true;
      return 0;
    }
    if (k != minimalColumnsIndex)
    {
      for (int l= 0; l < solution.size(); l++)
        result += solution[l]*Monoms [ind + l];
      ind += solution.size();
    }
  }

  delete[] pEvalPoints;
  delete[] pMat;
  delete[] pL;
  delete[] pM;
  delete[] coeffMonoms;

  if (alpha.level() != 1 && V_buf != alpha)
  {
    CFList u, v;
    result= mapDown (result, prim_elem, im_prim_elem, alpha, u, v);
  }
  result= N(result);

  if (fdivides (result, F) && fdivides (result, G))
    return result;
  else
  {
    fail= true;
    return 0;
  }
}

CanonicalForm sparseGCDFq (const CanonicalForm& F, const CanonicalForm& G,
                           const Variable & alpha, CFList& l, bool& topLevel)
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0) return G/Lc(G);
  else if (G.isZero() && degree (F) > 0) return F/Lc(F);
  else if (F.isZero() && G.isZero()) return F.genOne();
  if (F.inBaseDomain() || G.inBaseDomain()) return F.genOne();
  if (F.isUnivariate() && fdivides(F, G)) return F/Lc(F);
  if (G.isUnivariate() && fdivides(G, F)) return G/Lc(G);
  if (F == G) return F/Lc(F);

  CFMap M,N;
  int best_level= myCompress (A, B, M, N, topLevel);

  if (best_level == 0) return B.genOne();

  A= M(A);
  B= M(B);

  Variable x= Variable (1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
    return N (gcd (A, B));

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;
  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  if (fdivides (lcA, lcB))
  {
    if (fdivides (A, B))
      return F/Lc(F);
  }
  if (fdivides (lcB, lcA))
  {
    if (fdivides (B, A))
      return G/Lc(G);
  }

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable (2), Variable (ppA.level()));
  int d0;

  if (d == 0)
    return N(gcdcAcB);
  d0= totaldegree (ppB, Variable (2), Variable (ppB.level()));

  if (d0 < d)
    d= d0;

  if (d == 0)
    return N(gcdcAcB);

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH, skeleton;
  CanonicalForm newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  H= 0;
  bool fail= false;
  topLevel= false;
  bool inextension= false;
  Variable V_buf= alpha;
  CanonicalForm prim_elem, im_prim_elem;
  CFList source, dest;
  do // first do
  {
    random_element= randomElement (m, V_buf, l, fail);
    if (random_element == 0 && !fail)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }
    if (fail)
    {
      source= CFList();
      dest= CFList();

      Variable V_buf3= V_buf;
      V_buf= chooseExtension (V_buf);
      bool prim_fail= false;
      Variable V_buf2;
      prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

      if (V_buf3 != alpha)
      {
        m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
        G_m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
        newtonPoly= mapDown (newtonPoly, prim_elem, im_prim_elem, alpha,
                             source, dest);
        ppA= mapDown (ppA, prim_elem, im_prim_elem, alpha, source, dest);
        ppB= mapDown (ppB, prim_elem, im_prim_elem, alpha, source, dest);
        gcdlcAlcB= mapDown (gcdlcAlcB, prim_elem, im_prim_elem, alpha, source,
                            dest);
        for (CFListIterator i= l; i.hasItem(); i++)
          i.getItem()= mapDown (i.getItem(), prim_elem, im_prim_elem, alpha,
                                source, dest);
      }

      ASSERT (!prim_fail, "failure in integer factorizer");
      if (prim_fail)
        ; //ERROR
      else
        im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
      DEBOUTLN (cerr, "getMipo (V_buf2)= " << getMipo (V_buf2));
      inextension= true;
      for (CFListIterator i= l; i.hasItem(); i++)
        i.getItem()= mapUp (i.getItem(), alpha, V_buf, prim_elem,
                             im_prim_elem, source, dest);
      m= mapUp (m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      G_m= mapUp (G_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      newtonPoly= mapUp (newtonPoly, alpha, V_buf, prim_elem, im_prim_elem,
                          source, dest);
      ppA= mapUp (ppA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      ppB= mapUp (ppB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      gcdlcAlcB= mapUp (gcdlcAlcB, alpha, V_buf, prim_elem, im_prim_elem,
                         source, dest);

      fail= false;
      random_element= randomElement (m, V_buf, l, fail );
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      sparseGCDFq (ppA (random_element, x), ppB (random_element, x), V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      sparseGCDFq (ppA(random_element, x), ppB(random_element, x), V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 == 0)
      return N(gcdcAcB);
    if (d0 >  d)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    G_random_element=
    (gcdlcAlcB(random_element, x)/uni_lcoeff (G_random_element))
    * G_random_element;

    skeleton= G_random_element;
    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 <  d)
    {
      m= gcdlcAlcB;
      newtonPoly= 1;
      G_m= 0;
      d= d0;
    }

    H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
    if (uni_lcoeff (H) == gcdlcAlcB)
    {
      cH= uni_content (H);
      ppH= H/cH;
      if (inextension)
      {
        CFList u, v;
        //maybe it's better to test if ppH is an element of F(\alpha) before
        //mapping down
        if (fdivides (ppH, ppA) && fdivides (ppH, ppB))
        {
          DEBOUTLN (cerr, "ppH before mapDown= " << ppH);
          ppH= mapDown (ppH, prim_elem, im_prim_elem, alpha, u, v);
          ppH /= Lc(ppH);
          DEBOUTLN (cerr, "ppH after mapDown= " << ppH);
          return N(gcdcAcB*ppH);
        }
      }
      else if (fdivides (ppH, ppA) && fdivides (ppH, ppB))
        return N(gcdcAcB*ppH);
    }
    G_m= H;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);

    if (getCharacteristic () > 3 && size (skeleton) < 100)
    {
      CFArray Monoms;
      CFArray *coeffMonoms;
      do //second do
      {
        random_element= randomElement (m, V_buf, l, fail);
        if (random_element == 0 && !fail)
        {
          if (!find (l, random_element))
            l.append (random_element);
          continue;
        }
        if (fail)
        {
          source= CFList();
          dest= CFList();

          Variable V_buf3= V_buf;
          V_buf= chooseExtension (V_buf);
          bool prim_fail= false;
          Variable V_buf2;
          prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

          if (V_buf3 != alpha)
          {
            m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
            G_m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
            newtonPoly= mapDown (newtonPoly, prim_elem, im_prim_elem, alpha,
                                 source, dest);
            ppA= mapDown (ppA, prim_elem, im_prim_elem, alpha, source, dest);
            ppB= mapDown (ppB, prim_elem, im_prim_elem, alpha, source, dest);
            gcdlcAlcB= mapDown (gcdlcAlcB, prim_elem, im_prim_elem, alpha,
                                source, dest);
            for (CFListIterator i= l; i.hasItem(); i++)
              i.getItem()= mapDown (i.getItem(), prim_elem, im_prim_elem, alpha,
                                    source, dest);
          }

          ASSERT (!prim_fail, "failure in integer factorizer");
          if (prim_fail)
            ; //ERROR
          else
            im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

          DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
          DEBOUTLN (cerr, "getMipo (V_buf2)= " << getMipo (V_buf2));
          inextension= true;
          for (CFListIterator i= l; i.hasItem(); i++)
            i.getItem()= mapUp (i.getItem(), alpha, V_buf, prim_elem,
                                im_prim_elem, source, dest);
          m= mapUp (m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          G_m= mapUp (G_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          newtonPoly= mapUp (newtonPoly, alpha, V_buf, prim_elem, im_prim_elem,
                              source, dest);
          ppA= mapUp (ppA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          ppB= mapUp (ppB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);

          gcdlcAlcB= mapUp (gcdlcAlcB, alpha, V_buf, prim_elem, im_prim_elem,
                            source, dest);

          fail= false;
          random_element= randomElement (m, V_buf, l, fail);
          DEBOUTLN (cerr, "fail= " << fail);
          CFList list;
          TIMING_START (gcd_recursion);

          //sparseInterpolation
          bool sparseFail= false;
          if (LC (skeleton).inCoeffDomain())
            G_random_element=
            monicSparseInterpol (ppA (random_element, x), ppB(random_element,x),
                                skeleton,V_buf, sparseFail, coeffMonoms,Monoms);
          else
            G_random_element=
            nonMonicSparseInterpol (ppA(random_element,x),ppB(random_element,x),
                                    skeleton, V_buf, sparseFail, coeffMonoms,
                                    Monoms);
          if (sparseFail)
            break;

          TIMING_END_AND_PRINT (gcd_recursion,
                                "time for recursive call: ");
          DEBOUTLN (cerr, "G_random_element= " << G_random_element);
        }
        else
        {
          CFList list;
          TIMING_START (gcd_recursion);
          bool sparseFail= false;
          if (LC (skeleton).inCoeffDomain())
            G_random_element=
            monicSparseInterpol (ppA (random_element, x),ppB(random_element, x),
                                skeleton,V_buf, sparseFail,coeffMonoms, Monoms);
          else
            G_random_element=
            nonMonicSparseInterpol (ppA(random_element,x),ppB(random_element,x),
                                    skeleton, V_buf, sparseFail, coeffMonoms,
                                    Monoms);
          if (sparseFail)
            break;

          TIMING_END_AND_PRINT (gcd_recursion,
                                "time for recursive call: ");
          DEBOUTLN (cerr, "G_random_element= " << G_random_element);
        }

        if (!G_random_element.inCoeffDomain())
          d0= totaldegree (G_random_element, Variable(2),
                           Variable (G_random_element.level()));
        else
          d0= 0;

        if (d0 == 0)
          return N(gcdcAcB);
        if (d0 >  d)
        {
          if (!find (l, random_element))
            l.append (random_element);
          continue;
        }

        G_random_element=
        (gcdlcAlcB(random_element, x)/uni_lcoeff (G_random_element))
        * G_random_element;

        if (!G_random_element.inCoeffDomain())
          d0= totaldegree (G_random_element, Variable(2),
                          Variable (G_random_element.level()));
        else
          d0= 0;

        if (d0 <  d)
        {
          m= gcdlcAlcB;
          newtonPoly= 1;
          G_m= 0;
          d= d0;
        }

        TIMING_START (newton_interpolation);
        H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
        TIMING_END_AND_PRINT (newton_interpolation,
                              "time for newton interpolation: ");

        //termination test
        if (uni_lcoeff (H) == gcdlcAlcB)
        {
          cH= uni_content (H);
          ppH= H/cH;
          if (inextension)
          {
            CFList u, v;
            //maybe it's better to test if ppH is an element of F(\alpha) before
            //mapping down
            if (fdivides (ppH, ppA) && fdivides (ppH, ppB))
            {
              DEBOUTLN (cerr, "ppH before mapDown= " << ppH);
              ppH= mapDown (ppH, prim_elem, im_prim_elem, alpha, u, v);
              ppH /= Lc(ppH);
              DEBOUTLN (cerr, "ppH after mapDown= " << ppH);
              return N(gcdcAcB*ppH);
            }
          }
          else if (fdivides (ppH, ppA) && fdivides (ppH, ppB))
          {
            return N(gcdcAcB*ppH);
          }
        }

        G_m= H;
        newtonPoly= newtonPoly*(x - random_element);
        m= m*(x - random_element);
        if (!find (l, random_element))
          l.append (random_element);

      } while (1);
    }
  } while (1);
}

CanonicalForm sparseGCDFp (const CanonicalForm& F, const CanonicalForm& G,
                           bool& topLevel, CFList& l)
{
  CanonicalForm A= F;
  CanonicalForm B= G;
  if (F.isZero() && degree(G) > 0) return G/Lc(G);
  else if (G.isZero() && degree (F) > 0) return F/Lc(F);
  else if (F.isZero() && G.isZero()) return F.genOne();
  if (F.inBaseDomain() || G.inBaseDomain()) return F.genOne();
  if (F.isUnivariate() && fdivides(F, G)) return F/Lc(F);
  if (G.isUnivariate() && fdivides(G, F)) return G/Lc(G);
  if (F == G) return F/Lc(F);

  CFMap M,N;
  int best_level= myCompress (A, B, M, N, topLevel);

  if (best_level == 0) return B.genOne();

  A= M(A);
  B= M(B);

  Variable x= Variable (1);

  //univariate case
  if (A.isUnivariate() && B.isUnivariate())
    return N (gcd (A, B));

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  cA = uni_content (A);
  cB = uni_content (B);
  gcdcAcB= gcd (cA, cB);
  ppA= A/cA;
  ppB= B/cB;

  CanonicalForm lcA, lcB;  // leading coefficients of A and B
  CanonicalForm gcdlcAlcB;
  lcA= uni_lcoeff (ppA);
  lcB= uni_lcoeff (ppB);

  if (fdivides (lcA, lcB))
  {
    if (fdivides (A, B))
      return F/Lc(F);
  }
  if (fdivides (lcB, lcA))
  {
    if (fdivides (B, A))
      return G/Lc(G);
  }

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable (2), Variable (ppA.level()));
  int d0;

  if (d == 0)
    return N(gcdcAcB);

  d0= totaldegree (ppB, Variable (2), Variable (ppB.level()));

  if (d0 < d)
    d= d0;

  if (d == 0)
    return N(gcdcAcB);

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH, skeleton;
  CanonicalForm newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  H= 0;
  bool fail= false;
  topLevel= false;
  bool inextension= false;
  Variable V_buf, alpha;
  CanonicalForm prim_elem, im_prim_elem;
  CFList source, dest;
  do //first do
  {
    if (inextension)
      random_element= randomElement (m, V_buf, l, fail);
    else
      random_element= FpRandomElement (m, l, fail);
    if (random_element == 0 && !fail)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    if (!fail && !inextension)
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      sparseGCDFp (ppA (random_element,x), ppB (random_element,x), topLevel,
                   list);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (!fail && inextension)
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      sparseGCDFq (ppA (random_element, x), ppB (random_element, x), alpha,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (fail && !inextension)
    {
      source= CFList();
      dest= CFList();
      CFList list;
      CanonicalForm mipo;
      int deg= 2;
      do
      {
        mipo= randomIrredpoly (deg, x);
        alpha= rootOf (mipo);
        inextension= true;
        fail= false;
        random_element= randomElement (m, alpha, l, fail);
        deg++;
      } while (fail);
      V_buf= alpha;
      list= CFList();
      TIMING_START (gcd_recursion);
      G_random_element=
      sparseGCDFq (ppA (random_element, x), ppB (random_element, x), alpha,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (fail && inextension)
    {
      source= CFList();
      dest= CFList();

      Variable V_buf3= V_buf;
      V_buf= chooseExtension (V_buf);
      bool prim_fail= false;
      Variable V_buf2;
      CanonicalForm prim_elem, im_prim_elem;
      prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

      if (V_buf3 != alpha)
      {
        m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
        G_m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
        newtonPoly= mapDown (newtonPoly, prim_elem, im_prim_elem, alpha, source,
                             dest);
        ppA= mapDown (ppA, prim_elem, im_prim_elem, alpha, source, dest);
        ppB= mapDown (ppB, prim_elem, im_prim_elem, alpha, source, dest);
        gcdlcAlcB= mapDown (gcdlcAlcB, prim_elem, im_prim_elem, alpha, source,
                            dest);
        for (CFListIterator i= l; i.hasItem(); i++)
          i.getItem()= mapDown (i.getItem(), prim_elem, im_prim_elem, alpha,
                                source, dest);
      }

      ASSERT (!prim_fail, "failure in integer factorizer");
      if (prim_fail)
        ; //ERROR
      else
        im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));

      for (CFListIterator i= l; i.hasItem(); i++)
        i.getItem()= mapUp (i.getItem(), alpha, V_buf, prim_elem,
                             im_prim_elem, source, dest);
      m= mapUp (m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      G_m= mapUp (G_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      newtonPoly= mapUp (newtonPoly, alpha, V_buf, prim_elem, im_prim_elem,
                          source, dest);
      ppA= mapUp (ppA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      ppB= mapUp (ppB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
      gcdlcAlcB= mapUp (gcdlcAlcB, alpha, V_buf, prim_elem, im_prim_elem,
                         source, dest);
      fail= false;
      random_element= randomElement (m, V_buf, l, fail );
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element=
      sparseGCDFq (ppA (random_element, x), ppB (random_element, x), V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion,
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 == 0)
      return N(gcdcAcB);
    if (d0 >  d)
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    G_random_element=
    (gcdlcAlcB(random_element, x)/uni_lcoeff (G_random_element))
    * G_random_element;

    skeleton= G_random_element;

    if (!G_random_element.inCoeffDomain())
      d0= totaldegree (G_random_element, Variable(2),
                       Variable (G_random_element.level()));
    else
      d0= 0;

    if (d0 <  d)
    {
      m= gcdlcAlcB;
      newtonPoly= 1;
      G_m= 0;
      d= d0;
    }

    H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);

    if (uni_lcoeff (H) == gcdlcAlcB)
    {
      cH= uni_content (H);
      ppH= H/cH;
      ppH /= Lc (ppH);
      DEBOUTLN (cerr, "ppH= " << ppH);

      if (fdivides (ppH, ppA) && fdivides (ppH, ppB))
        return N(gcdcAcB*ppH);
    }
    G_m= H;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);

    if ((getCharacteristic() > 3 && size (skeleton) < 200))
    {
      CFArray Monoms;
      CFArray* coeffMonoms;

      do //second do
      {
        if (inextension)
          random_element= randomElement (m, alpha, l, fail);
        else
          random_element= FpRandomElement (m, l, fail);
        if (random_element == 0 && !fail)
        {
          if (!find (l, random_element))
            l.append (random_element);
          continue;
        }

        bool sparseFail= false;
        if (!fail && !inextension)
        {
          CFList list;
          TIMING_START (gcd_recursion);
          if (LC (skeleton).inCoeffDomain())
            G_random_element=
            monicSparseInterpol(ppA(random_element, x), ppB (random_element, x),
                                skeleton, Variable(1), sparseFail, coeffMonoms,
                                Monoms);
          else
            G_random_element=
            nonMonicSparseInterpol(ppA(random_element,x), ppB(random_element,x),
                                    skeleton, Variable (1), sparseFail,
                                    coeffMonoms, Monoms);
          TIMING_END_AND_PRINT (gcd_recursion,
                                "time for recursive call: ");
          DEBOUTLN (cerr, "G_random_element= " << G_random_element);
        }
        else if (!fail && inextension)
        {
          CFList list;
          TIMING_START (gcd_recursion);
          if (LC (skeleton).inCoeffDomain())
            G_random_element=
            monicSparseInterpol(ppA (random_element,x), ppB (random_element, x),
                                skeleton, alpha, sparseFail, coeffMonoms,
                                Monoms);
          else
            G_random_element=
            nonMonicSparseInterpol(ppA(random_element,x), ppB(random_element,x),
                                   skeleton, alpha, sparseFail, coeffMonoms,
                                   Monoms);
          TIMING_END_AND_PRINT (gcd_recursion,
                                "time for recursive call: ");
          DEBOUTLN (cerr, "G_random_element= " << G_random_element);
        }
        else if (fail && !inextension)
        {
          source= CFList();
          dest= CFList();
          CFList list;
          CanonicalForm mipo;
          int deg= 2;
          do
          {
            mipo= randomIrredpoly (deg, x);
            alpha= rootOf (mipo);
            inextension= true;
            fail= false;
            random_element= randomElement (m, alpha, l, fail);
            deg++;
          } while (fail);
          V_buf= alpha;
          list= CFList();
          TIMING_START (gcd_recursion);
          if (LC (skeleton).inCoeffDomain())
            G_random_element=
            monicSparseInterpol (ppA (random_element,x), ppB (random_element,x),
                                skeleton, alpha, sparseFail, coeffMonoms,
                                Monoms);
          else
            G_random_element=
            nonMonicSparseInterpol(ppA(random_element,x), ppB(random_element,x),
                                   skeleton, alpha, sparseFail, coeffMonoms,
                                   Monoms);
          TIMING_END_AND_PRINT (gcd_recursion,
                                "time for recursive call: ");
          DEBOUTLN (cerr, "G_random_element= " << G_random_element);
        }
        else if (fail && inextension)
        {
          source= CFList();
          dest= CFList();

          Variable V_buf3= V_buf;
          V_buf= chooseExtension (V_buf);
          bool prim_fail= false;
          Variable V_buf2;
          CanonicalForm prim_elem, im_prim_elem;
          prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

          if (V_buf3 != alpha)
          {
            m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
            G_m= mapDown (m, prim_elem, im_prim_elem, alpha, source, dest);
            newtonPoly= mapDown (newtonPoly, prim_elem, im_prim_elem, alpha,
                                 source, dest);
            ppA= mapDown (ppA, prim_elem, im_prim_elem, alpha, source, dest);
            ppB= mapDown (ppB, prim_elem, im_prim_elem, alpha, source, dest);
            gcdlcAlcB= mapDown (gcdlcAlcB, prim_elem, im_prim_elem, alpha,
                                source, dest);
            for (CFListIterator i= l; i.hasItem(); i++)
              i.getItem()= mapDown (i.getItem(), prim_elem, im_prim_elem, alpha,
                                    source, dest);
          }

          ASSERT (!prim_fail, "failure in integer factorizer");
          if (prim_fail)
            ; //ERROR
          else
            im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

          DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
          DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));

          for (CFListIterator i= l; i.hasItem(); i++)
            i.getItem()= mapUp (i.getItem(), alpha, V_buf, prim_elem,
                                im_prim_elem, source, dest);
          m= mapUp (m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          G_m= mapUp (G_m, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          newtonPoly= mapUp (newtonPoly, alpha, V_buf, prim_elem, im_prim_elem,
                              source, dest);
          ppA= mapUp (ppA, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          ppB= mapUp (ppB, alpha, V_buf, prim_elem, im_prim_elem, source, dest);
          gcdlcAlcB= mapUp (gcdlcAlcB, alpha, V_buf, prim_elem, im_prim_elem,
                            source, dest);
          fail= false;
          random_element= randomElement (m, V_buf, l, fail );
          DEBOUTLN (cerr, "fail= " << fail);
          CFList list;
          TIMING_START (gcd_recursion);
          if (LC (skeleton).inCoeffDomain())
            G_random_element=
            monicSparseInterpol (ppA (random_element, x), ppB (random_element, x),
                                skeleton, V_buf, sparseFail, coeffMonoms,
                                Monoms);
          else
            G_random_element=
            nonMonicSparseInterpol (ppA(random_element,x), ppB(random_element,x),
                                    skeleton, V_buf, sparseFail,
                                    coeffMonoms, Monoms);
          TIMING_END_AND_PRINT (gcd_recursion,
                                "time for recursive call: ");
          DEBOUTLN (cerr, "G_random_element= " << G_random_element);
        }

        if (sparseFail)
          break;

        if (!G_random_element.inCoeffDomain())
          d0= totaldegree (G_random_element, Variable(2),
                           Variable (G_random_element.level()));
        else
          d0= 0;

        if (d0 == 0)
          return N(gcdcAcB);
        if (d0 >  d)
        {
          if (!find (l, random_element))
            l.append (random_element);
          continue;
        }

        G_random_element=
        (gcdlcAlcB(random_element, x)/uni_lcoeff (G_random_element))
        * G_random_element;

        if (!G_random_element.inCoeffDomain())
          d0= totaldegree (G_random_element, Variable(2),
                           Variable (G_random_element.level()));
        else
          d0= 0;

        if (d0 <  d)
        {
          m= gcdlcAlcB;
          newtonPoly= 1;
          G_m= 0;
          d= d0;
        }

        TIMING_START (newton_interpolation);
        H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
        TIMING_END_AND_PRINT (newton_interpolation,
                              "time for newton interpolation: ");

        //termination test
        if (uni_lcoeff (H) == gcdlcAlcB)
        {
          cH= uni_content (H);
          ppH= H/cH;
          ppH /= Lc (ppH);
          DEBOUTLN (cerr, "ppH= " << ppH);
          if (fdivides (ppH, ppA) && fdivides (ppH, ppB))
            return N(gcdcAcB*ppH);
        }

        G_m= H;
        newtonPoly= newtonPoly*(x - random_element);
        m= m*(x - random_element);
        if (!find (l, random_element))
          l.append (random_element);

      } while (1); //end of second do
    }
    else
      return N(gcdcAcB*GCD_small_p (ppA, ppB));
  } while (1); //end of first do
}

static inline
int compress4EZGCD (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                    CFMap & N, int& both_non_zero)
{
  int n= tmax (F.level(), G.level());
  int * degsf= new int [n + 1];
  int * degsg= new int [n + 1];

  for (int i = 0; i <= n; i++)
    degsf[i]= degsg[i]= 0;

  degsf= degrees (F, degsf);
  degsg= degrees (G, degsg);

  both_non_zero= 0;
  int f_zero= 0;
  int g_zero= 0;

  for (int i= 1; i <= n; i++)
  {
    if (degsf[i] != 0 && degsg[i] != 0)
    {
      both_non_zero++;
      continue;
    }
    if (degsf[i] == 0 && degsg[i] != 0 && i <= G.level())
    {
      f_zero++;
      continue;
    }
    if (degsg[i] == 0 && degsf[i] && i <= F.level())
    {
      g_zero++;
      continue;
    }
  }

  if (both_non_zero == 0)
  {
    delete [] degsf;
    delete [] degsg;
    return 0;
  }

  // map Variables which do not occur in both polynomials to higher levels
  int k= 1;
  int l= 1;
  for (int i= 1; i <= n; i++)
  {
    if (degsf[i] != 0 && degsg[i] == 0 && i <= F.level())
    {
      if (k + both_non_zero != i)
      {
        M.newpair (Variable (i), Variable (k + both_non_zero));
        N.newpair (Variable (k + both_non_zero), Variable (i));
      }
      k++;
    }
    if (degsf[i] == 0 && degsg[i] != 0 && i <= G.level())
    {
      if (l + g_zero + both_non_zero != i)
      {
        M.newpair (Variable (i), Variable (l + g_zero + both_non_zero));
        N.newpair (Variable (l + g_zero + both_non_zero), Variable (i));
      }
      l++;
    }
  }

  // sort Variables x_{i} in decreasing order of
  // min(deg_{x_{i}}(f),deg_{x_{i}}(g))
  int m= tmin (F.level(), G.level());
  int max_min_deg;
  k= both_non_zero;
  l= 0;
  int i= 1;
  while (k > 0)
  {
    max_min_deg= tmin (degsf[i], degsg[i]);
    while (max_min_deg == 0)
    {
      i++;
      max_min_deg= tmin (degsf[i], degsg[i]);
    }
    for (int j= i + 1; j <= m; j++)
    {
      if ((tmin (degsf[j],degsg[j]) < max_min_deg) &&
          (tmin (degsf[j], degsg[j]) != 0))
      {
        max_min_deg= tmin (degsf[j], degsg[j]);
        l= j;
      }
    }

    if (l != 0)
    {
      if (l != k)
      {
        M.newpair (Variable (l), Variable(k));
        N.newpair (Variable (k), Variable(l));
        degsf[l]= 0;
        degsg[l]= 0;
        l= 0;
      }
      else
      {
        degsf[l]= 0;
        degsg[l]= 0;
        l= 0;
      }
    }
    else if (l == 0)
    {
      if (i != k)
      {
        M.newpair (Variable (i), Variable (k));
        N.newpair (Variable (k), Variable (i));
        degsf[i]= 0;
        degsg[i]= 0;
      }
      else
      {
        degsf[i]= 0;
        degsg[i]= 0;
      }
      i++;
    }
    k--;
  }

  delete [] degsf;
  delete [] degsg;

  return both_non_zero;
}

static inline
CanonicalForm myShift2Zero (const CanonicalForm& F, CFList& Feval,
                            const CFList& evaluation)
{
  CanonicalForm A= F;
  int k= 2;
  for (CFListIterator i= evaluation; i.hasItem(); i++, k++)
    A= A (Variable (k) + i.getItem(), k);

  CanonicalForm buf= A;
  Feval= CFList();
  Feval.append (buf);
  for (k= evaluation.length() + 1; k > 2; k--)
  {
    buf= mod (buf, Variable (k));
    Feval.insert (buf);
  }
  return A;
}

static inline
CanonicalForm myReverseShift (const CanonicalForm& F, const CFList& evaluation)
{
  int l= evaluation.length() + 1;
  CanonicalForm result= F;
  CFListIterator j= evaluation;
  for (int i= 2; i < l + 1; i++, j++)
  {
    if (F.level() < i)
      continue;
    result= result (Variable (i) - j.getItem(), i);
  }
  return result;
}

static inline
Evaluation optimize4Lift (const CanonicalForm& F, CFMap & M,
                    CFMap & N, const Evaluation& A)
{
  int n= F.level();
  int * degsf= new int [n + 1];

  for (int i = 0; i <= n; i++)
    degsf[i]= 0;

  degsf= degrees (F, degsf);

  Evaluation result= Evaluation (A.min(), A.max());
  ASSERT (A.min() == 2, "expected A.min() == 2");
  int max_deg;
  int k= n;
  int l= 1;
  int i= 2;
  int pos= 2;
  while (k > 1)
  {
    max_deg= degsf [i];
    while (max_deg == 0)
    {
      i++;
      max_deg= degsf [i];
    }
    l= i;
    for (int j= i + 1; j <=  n; j++)
    {
      if (degsf[j] > max_deg)
      {
        max_deg= degsf[j];
        l= j;
      }
    }

    if (l <= n)
    {
      if (l != pos)
      {
        result.setValue (pos, A [l]);
        M.newpair (Variable (l), Variable (pos));
        N.newpair (Variable (pos), Variable (l));
        degsf[l]= 0;
        l= 2;
        if (k == 2 && n == 3)
        {
          result.setValue (l, A [pos]);
          M.newpair (Variable (pos), Variable (l));
          N.newpair (Variable (l), Variable (pos));
          degsf[pos]= 0;
        }
      }
      else
      {
        result.setValue (l, A [l]);
        degsf [l]= 0;
      }
    }
    pos++;
    k--;
    l= 2;
  }

  delete [] degsf;

  return result;
}

static inline
int Hensel_P (const CanonicalForm & UU, CFArray & G, const Evaluation & AA,
              const CFArray& LeadCoeffs )
{
  CFList factors;
  factors.append (G[1]);
  factors.append (G[2]);

  CFMap NN, MM;
  Evaluation A= optimize4Lift (UU, MM, NN, AA);

  CanonicalForm U= MM (UU);
  CFArray LCs= CFArray (1,2);
  LCs [1]= MM (LeadCoeffs [1]);
  LCs [2]= MM (LeadCoeffs [2]);

  CFList evaluation;
  long termEstimate= size (U);
  for (int i= A.min(); i <= A.max(); i++)
  {
    if (!A[i].isZero() && (getCharacteristic() > degree (U,i))) //TODO find a good estimate for getCharacteristic() <= degree (U,i)
    {
      termEstimate *= degree (U,i)*2;
      termEstimate /= 3;
    }
    evaluation.append (A [i]);
  }
  if (termEstimate/getNumVars(U) > 500)
    return -1;
  CFList UEval;
  CanonicalForm shiftedU= myShift2Zero (U, UEval, evaluation);

  if (size (shiftedU)/getNumVars (U) > 500)
    return -1;

  CFArray shiftedLCs= CFArray (2);
  CFList shiftedLCsEval1, shiftedLCsEval2;
  shiftedLCs[0]= myShift2Zero (LCs[1], shiftedLCsEval1, evaluation);
  shiftedLCs[1]= myShift2Zero (LCs[2], shiftedLCsEval2, evaluation);
  factors.insert (1);
  int liftBound= degree (UEval.getLast(), 2) + 1;
  CFArray Pi;
  CFMatrix M= CFMatrix (liftBound, factors.length() - 1);
  CFList diophant;
  CFArray lcs= CFArray (2);
  lcs [0]= shiftedLCsEval1.getFirst();
  lcs [1]= shiftedLCsEval2.getFirst();
  nonMonicHenselLift12 (UEval.getFirst(), factors, liftBound, Pi, diophant, M,
                        lcs, false);

  for (CFListIterator i= factors; i.hasItem(); i++)
  {
    if (!fdivides (i.getItem(), UEval.getFirst()))
      return 0;
  }

  int * liftBounds;
  bool noOneToOne= false;
  if (U.level() > 2)
  {
    liftBounds= new int [U.level() - 1]; /* index: 0.. U.level()-2 */
    liftBounds[0]= liftBound;
    for (int i= 1; i < U.level() - 1; i++)
      liftBounds[i]= degree (shiftedU, Variable (i + 2)) + 1;
    factors= nonMonicHenselLift2 (UEval, factors, liftBounds, U.level() - 1,
                                  false, shiftedLCsEval1, shiftedLCsEval2, Pi,
                                  diophant, noOneToOne);
    delete [] liftBounds;
    if (noOneToOne)
      return 0;
  }
  G[1]= factors.getFirst();
  G[2]= factors.getLast();
  G[1]= myReverseShift (G[1], evaluation);
  G[2]= myReverseShift (G[2], evaluation);
  G[1]= NN (G[1]);
  G[2]= NN (G[2]);
  return 1;
}

static inline
bool findeval_P (const CanonicalForm & F, const CanonicalForm & G,
                 CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db,
                 REvaluation & b, int delta, int degF, int degG, int maxeval,
                 int & count, int& k, int bound, int& l)
{
  if( count == 0 && delta != 0)
  {
    if( count++ > maxeval )
      return false;
  }
  if (count > 0)
  {
    b.nextpoint(k);
    if (k == 0)
      k++;
    l++;
    if (l > bound)
    {
      l= 1;
      k++;
      if (k > tmax (F.level(), G.level()) - 1)
        return false;
      b.nextpoint (k);
    }
    if (count++ > maxeval)
      return false;
  }
  while( true )
  {
    Fb = b( F );
    if( degree( Fb, 1 ) == degF )
    {
      Gb = b( G );
      if( degree( Gb, 1 ) == degG )
      {
        Db = gcd( Fb, Gb );
        if( delta > 0 )
        {
          if( degree( Db, 1 ) <= delta )
            return true;
        }
        else
          return true;
      }
    }
    if (k == 0)
      k++;
    b.nextpoint(k);
    l++;
    if (l > bound)
    {
      l= 1;
      k++;
      if (k > tmax (F.level(), G.level()) - 1)
        return false;
      b.nextpoint (k);
    }
    if( count++ > maxeval )
      return false;
  }
}

// parameters for heuristic
static int maxNumEval= 200;
static int sizePerVars1= 500; //try dense gcd if size/#variables is bigger

/// Extended Zassenhaus GCD for finite fields
CanonicalForm EZGCD_P( const CanonicalForm & FF, const CanonicalForm & GG )
{
  if (FF.isZero() && degree(GG) > 0) return GG/Lc(GG);
  else if (GG.isZero() && degree (FF) > 0) return FF/Lc(FF);
  else if (FF.isZero() && GG.isZero()) return FF.genOne();
  if (FF.inBaseDomain() || GG.inBaseDomain()) return FF.genOne();
  if (FF.isUnivariate() && fdivides(FF, GG)) return FF/Lc(FF);
  if (GG.isUnivariate() && fdivides(GG, FF)) return GG/Lc(GG);
  if (FF == GG) return FF/Lc(FF);

  int maxNumVars= tmax (getNumVars (FF), getNumVars (GG));
  Variable a, oldA;
  int sizeF= size (FF);
  int sizeG= size (GG);

  if (sizeF/maxNumVars > sizePerVars1 && sizeG/maxNumVars > sizePerVars1)
  {
    if (hasFirstAlgVar (FF, a) || hasFirstAlgVar (GG, a))
      return GCD_Fp_extension (FF, GG, a);
    else if (CFFactory::gettype() == GaloisFieldDomain)
      return GCD_GF (FF, GG);
    else
      return GCD_small_p (FF, GG);
  }

  CanonicalForm F, G, f, g, d, Fb, Gb, Db, Fbt, Gbt, Dbt, B0, B, D0, lcF, lcG,
                lcD;
  CFArray DD( 1, 2 ), lcDD( 1, 2 );
  int degF, degG, delta, count;
  int maxeval;
  maxeval= tmin((getCharacteristic()/
                (int)(ilog2(getCharacteristic())*log2exp))*2, maxNumEval);
  count= 0; // number of eval. used
  REvaluation b, bt;
  int gcdfound = 0;
  Variable x = Variable(1);

  F= FF;
  G= GG;

  CFMap M,N;
  int smallestDegLev;
  TIMING_START (ez_p_compress)
  int best_level= compress4EZGCD (F, G, M, N, smallestDegLev);

  if (best_level == 0) return G.genOne();

  F= M (F);
  G= M (G);
  TIMING_END_AND_PRINT (ez_p_compress, "time for compression in EZ_P: ")

  TIMING_START (ez_p_content)
  f = content( F, x ); g = content( G, x );
  d = gcd( f, g );
  F /= f; G /= g;
  TIMING_END_AND_PRINT (ez_p_content, "time to extract content in EZ_P: ")

  if( F.isUnivariate() && G.isUnivariate() )
  {
    if( F.mvar() == G.mvar() )
      d *= gcd( F, G );
    else
      return N (d);
    return N (d);
  }
  if ( F.isUnivariate())
  {
    g= content (G,G.mvar());
    return N(d*gcd(F,g));
  }
  if ( G.isUnivariate())
  {
    f= content (F,F.mvar());
    return N(d*gcd(G,f));
  }

  maxNumVars= tmax (getNumVars (F), getNumVars (G));
  sizeF= size (F);
  sizeG= size (G);

  if (sizeF/maxNumVars > sizePerVars1 && sizeG/maxNumVars > sizePerVars1)
  {
    if (hasFirstAlgVar (F, a) || hasFirstAlgVar (G, a))
      return N (d*GCD_Fp_extension (F, G, a));
    else if (CFFactory::gettype() == GaloisFieldDomain)
      return N (d*GCD_GF (F, G));
    else
      return N (d*GCD_small_p (F, G));
  }

  int dummy= 0;
  if( gcd_test_one( F, G, false, dummy ) )
  {
    return N (d);
  }

  bool passToGF= false;
  bool extOfExt= false;
  int p= getCharacteristic();
  bool algExtension= (hasFirstAlgVar(F,a) || hasFirstAlgVar(G,a));
  int k= 1;
  CanonicalForm primElem, imPrimElem;
  CFList source, dest;
  if (p < 50 && CFFactory::gettype() != GaloisFieldDomain && !algExtension)
  {
    if (p == 2)
      setCharacteristic (2, 12, 'Z');
    else if (p == 3)
      setCharacteristic (3, 4, 'Z');
    else if (p == 5 || p == 7)
      setCharacteristic (p, 3, 'Z');
    else
      setCharacteristic (p, 2, 'Z');
    passToGF= true;
    F= F.mapinto();
    G= G.mapinto();
    maxeval= 2*ipower (p, getGFDegree());
  }
  else if (CFFactory::gettype() == GaloisFieldDomain &&
           ipower (p , getGFDegree()) < 50)
  {
    k= getGFDegree();
    if (ipower (p, 2*k) > 50)
      setCharacteristic (p, 2*k, gf_name);
    else
      setCharacteristic (p, 3*k, gf_name);
    F= GFMapUp (F, k);
    G= GFMapUp (G, k);
    maxeval= tmin (2*ipower (p, getGFDegree()), maxNumEval);
  }
  else if (p < 50 && algExtension && CFFactory::gettype() != GaloisFieldDomain)
  {
    int d= degree (getMipo (a));
    oldA= a;
    Variable v2;
    if (p == 2 && d < 6)
    {
      if (fac_NTL_char != p)
      {
        fac_NTL_char= p;
        zz_p::init (p);
      }
      bool primFail= false;
      Variable vBuf;
      primElem= primitiveElement (a, vBuf, primFail);
      ASSERT (!primFail, "failure in integer factorizer");
      if (d < 3)
      {
        zz_pX NTLIrredpoly;
        BuildIrred (NTLIrredpoly, d*3);
        CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
        v2= rootOf (newMipo);
      }
      else
      {
        zz_pX NTLIrredpoly;
        BuildIrred (NTLIrredpoly, d*2);
        CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
        v2= rootOf (newMipo);
      }
      imPrimElem= mapPrimElem (primElem, a, v2);
      extOfExt= true;
    }
    else if ((p == 3 && d < 4) || ((p == 5 || p == 7) && d < 3))
    {
      if (fac_NTL_char != p)
      {
        fac_NTL_char= p;
        zz_p::init (p);
      }
      bool primFail= false;
      Variable vBuf;
      primElem= primitiveElement (a, vBuf, primFail);
      ASSERT (!primFail, "failure in integer factorizer");
      zz_pX NTLIrredpoly;
      BuildIrred (NTLIrredpoly, d*2);
      CanonicalForm newMipo= convertNTLzzpX2CF (NTLIrredpoly, Variable (1));
      v2= rootOf (newMipo);
      imPrimElem= mapPrimElem (primElem, a, v2);
      extOfExt= true;
    }
    if (extOfExt)
    {
      maxeval= tmin (2*ipower (p, degree (getMipo (v2))), maxNumEval);
      F= mapUp (F, a, v2, primElem, imPrimElem, source, dest);
      G= mapUp (G, a, v2, primElem, imPrimElem, source, dest);
      a= v2;
    }
  }

  lcF = LC( F, x ); lcG = LC( G, x );
  lcD = gcd( lcF, lcG );

  delta = 0;
  degF = degree( F, x ); degG = degree( G, x );

  if(hasFirstAlgVar(G,a))
    b = REvaluation( 2, tmax(F.level(), G.level()), AlgExtRandomF( a ) );
  else
  { // both not in extension given by algebraic variable
    if (CFFactory::gettype() != GaloisFieldDomain)
      b = REvaluation( 2, tmax(F.level(), G.level()), FFRandom() );
    else
      b = REvaluation( 2, tmax(F.level(), G.level()), GFRandom() );
  }

  CanonicalForm cand, contcand;
  CanonicalForm result;
  int o, t;
  o= 0;
  t= 1;
  int goodPointCount= 0;
  while( !gcdfound )
  {
    TIMING_START (ez_p_eval);
    if( !findeval_P( F, G, Fb, Gb, Db, b, delta, degF, degG, maxeval, count, o,
         maxeval/maxNumVars, t ))
    { // too many eval. used --> try another method
      Off (SW_USE_EZGCD_P);
      result= gcd (F,G);
      On (SW_USE_EZGCD_P);
      if (passToGF)
      {
        CanonicalForm mipo= gf_mipo;
        setCharacteristic (p);
        Variable alpha= rootOf (mipo.mapinto());
        result= GF2FalphaRep (result, alpha);
      }
      if (k > 1)
      {
        result= GFMapDown (result, k);
        setCharacteristic (p, k, gf_name);
      }
      if (extOfExt)
        result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
      return N (d*result);
    }
    TIMING_END_AND_PRINT (ez_p_eval, "time for eval point search in EZ_P1: ");
    delta = degree( Db );
    if (delta == degF)
    {
      if (degF <= degG && fdivides (F, G))
      {
        if (passToGF)
        {
          CanonicalForm mipo= gf_mipo;
          setCharacteristic (p);
          Variable alpha= rootOf (mipo.mapinto());
          F= GF2FalphaRep (F, alpha);
        }
        if (k > 1)
        {
          F= GFMapDown (F, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
          F= mapDown (F, primElem, imPrimElem, oldA, dest, source);
        return N (d*F);
      }
      else
        delta--;
    }
    else if (delta == degG)
    {
      if (degG <= degF && fdivides (G, F))
      {
        if (passToGF)
        {
          CanonicalForm mipo= gf_mipo;
          setCharacteristic (p);
          Variable alpha= rootOf (mipo.mapinto());
          G= GF2FalphaRep (G, alpha);
        }
        if (k > 1)
        {
          G= GFMapDown (G, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
          G= mapDown (G, primElem, imPrimElem, oldA, dest, source);
        return N (d*G);
      }
      else
        delta--;
    }
    if( delta == 0 )
    {
      if (passToGF)
        setCharacteristic (p);
      if (k > 1)
        setCharacteristic (p, k, gf_name);
      return N (d);
    }
    while( true )
    {
      bt = b;
      TIMING_START (ez_p_eval);
      if( !findeval_P(F,G,Fbt,Gbt,Dbt, bt, delta, degF, degG, maxeval, count, o,
           maxeval/maxNumVars, t ))
      { // too many eval. used --> try another method
        Off (SW_USE_EZGCD_P);
        result= gcd (F,G);
        On (SW_USE_EZGCD_P);
        if (passToGF)
        {
          CanonicalForm mipo= gf_mipo;
          setCharacteristic (p);
          Variable alpha= rootOf (mipo.mapinto());
          result= GF2FalphaRep (result, alpha);
        }
        if (k > 1)
        {
          result= GFMapDown (result, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
          result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
        return N (d*result);
      }
      TIMING_END_AND_PRINT (ez_p_eval, "time for eval point search in EZ_P2: ");
      int dd = degree( Dbt );
      if( dd == 0 )
      {
        if (passToGF)
          setCharacteristic (p);
        if (k > 1)
          setCharacteristic (p, k, gf_name);
        return N (d);
      }
      if( dd == delta )
      {
        goodPointCount++;
        if (goodPointCount == 5)
          break;
      }
      if( dd < delta )
      {
        goodPointCount= 0;
        delta = dd;
        b = bt;
        Db = Dbt; Fb = Fbt; Gb = Gbt;
      }
      if (delta == degF)
      {
        if (degF <= degG && fdivides (F, G))
        {
          if (passToGF)
          {
            CanonicalForm mipo= gf_mipo;
            setCharacteristic (p);
            Variable alpha= rootOf (mipo.mapinto());
            F= GF2FalphaRep (F, alpha);
          }
          if (k > 1)
          {
            F= GFMapDown (F, k);
            setCharacteristic (p, k, gf_name);
          }
          if (extOfExt)
            F= mapDown (F, primElem, imPrimElem, oldA, dest, source);
          return N (d*F);
        }
        else
          delta--;
      }
      else if (delta == degG)
      {
        if (degG <= degF && fdivides (G, F))
        {
          if (passToGF)
          {
            CanonicalForm mipo= gf_mipo;
            setCharacteristic (p);
            Variable alpha= rootOf (mipo.mapinto());
            G= GF2FalphaRep (G, alpha);
          }
          if (k > 1)
          {
            G= GFMapDown (G, k);
            setCharacteristic (p, k, gf_name);
          }
          if (extOfExt)
            G= mapDown (G, primElem, imPrimElem, oldA, dest, source);
          return N (d*G);
        }
        else
          delta--;
      }
      if( delta == 0 )
      {
        if (passToGF)
          setCharacteristic (p);
        if (k > 1)
          setCharacteristic (p, k, gf_name);
        return N (d);
      }
    }
    if( delta != degF && delta != degG )
    {
      bool B_is_F;
      CanonicalForm xxx1, xxx2;
      CanonicalForm buf;
      DD[1] = Fb / Db;
      buf= Gb/Db;
      xxx1 = gcd( DD[1], Db );
      xxx2 = gcd( buf, Db );
      if (((xxx1.inCoeffDomain() && xxx2.inCoeffDomain()) &&
          (size (F) <= size (G)))
          || (xxx1.inCoeffDomain() && !xxx2.inCoeffDomain()))
      {
        B = F;
        DD[2] = Db;
        lcDD[1] = lcF;
        lcDD[2] = lcD;
        B_is_F = true;
      }
      else if (((xxx1.inCoeffDomain() && xxx2.inCoeffDomain()) &&
               (size (G) < size (F)))
               || (!xxx1.inCoeffDomain() && xxx2.inCoeffDomain()))
      {
        DD[1] = buf;
        B = G;
        DD[2] = Db;
        lcDD[1] = lcG;
        lcDD[2] = lcD;
        B_is_F = false;
      }
      else // special case handling
      {
        Off (SW_USE_EZGCD_P);
        result= gcd (F,G);
        On (SW_USE_EZGCD_P);
        if (passToGF)
        {
          CanonicalForm mipo= gf_mipo;
          setCharacteristic (p);
          Variable alpha= rootOf (mipo.mapinto());
          result= GF2FalphaRep (result, alpha);
        }
        if (k > 1)
        {
          result= GFMapDown (result, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
          result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
        return N (d*result);
      }
      DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
      DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );

      if (size (B*lcDD[2])/maxNumVars > sizePerVars1)
      {
        if (algExtension)
        {
          result= GCD_Fp_extension (F, G, a);
          if (extOfExt)
            result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
          return N (d*result);
        }
        if (CFFactory::gettype() == GaloisFieldDomain)
        {
          result= GCD_GF (F, G);
          if (passToGF)
          {
            CanonicalForm mipo= gf_mipo;
            setCharacteristic (p);
            Variable alpha= rootOf (mipo.mapinto());
            result= GF2FalphaRep (result, alpha);
          }
          if (k > 1)
          {
            result= GFMapDown (result, k);
            setCharacteristic (p, k, gf_name);
          }
          return N (d*result);
        }
        else
          return N (d*GCD_small_p (F,G));
      }

      TIMING_START (ez_p_hensel_lift);
      gcdfound= Hensel_P (B*lcD, DD, b, lcDD);
      TIMING_END_AND_PRINT (ez_p_hensel_lift, "time for Hensel lift in EZ_P: ");

      if (gcdfound == -1) //things became dense
      {
        if (algExtension)
        {
          result= GCD_Fp_extension (F, G, a);
          if (extOfExt)
            result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
          return N (d*result);
        }
        if (CFFactory::gettype() == GaloisFieldDomain)
        {
          result= GCD_GF (F, G);
          if (passToGF)
          {
            CanonicalForm mipo= gf_mipo;
            setCharacteristic (p);
            Variable alpha= rootOf (mipo.mapinto());
            result= GF2FalphaRep (result, alpha);
          }
          if (k > 1)
          {
            result= GFMapDown (result, k);
            setCharacteristic (p, k, gf_name);
          }
          return N (d*result);
        }
        else
        {
          if (p >= cf_getBigPrime(0))
            return N (d*sparseGCDFp (F,G));
          else
            return N (d*GCD_small_p (F,G));
        }
      }

      if (gcdfound == 1)
      {
        TIMING_START (termination_test);
        contcand= content (DD[2], Variable (1));
        cand = DD[2] / contcand;
        if (B_is_F)
          gcdfound = fdivides( cand, G ) && cand*(DD[1]/(lcD/contcand)) == F;
        else
          gcdfound = fdivides( cand, F ) && cand*(DD[1]/(lcD/contcand)) == G;
        TIMING_END_AND_PRINT (termination_test,
                              "time for termination test EZ_P: ");

        if (passToGF && gcdfound)
        {
          CanonicalForm mipo= gf_mipo;
          setCharacteristic (p);
          Variable alpha= rootOf (mipo.mapinto());
          cand= GF2FalphaRep (cand, alpha);
        }
        if (k > 1 && gcdfound)
        {
          cand= GFMapDown (cand, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt && gcdfound)
          cand= mapDown (cand, primElem, imPrimElem, oldA, dest, source);
      }
    }
    delta--;
    goodPointCount= 0;
  }
  return N (d*cand);
}


#endif
