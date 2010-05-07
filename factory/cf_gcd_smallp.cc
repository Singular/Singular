// -*- c++ -*-
//*****************************************************************************
/** @file cf_gcd_smallp.cc
 *
 * @author Martin Lee
 * @date 22.10.2009
 *
 * This file implements the GCD of two polynomials over \f$ F_{p} \f$ , 
 * \f$ F_{p}(\alpha ) \f$ or GF based on Alg. 7.2. as described in "Algorithms 
 * for Computer Algebra" by Geddes, Czapor, Labahnn
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal 
 * @version \$Id$
 *
**/
//*****************************************************************************

#include <config.h>

#include "assert.h"
#include "debug.h"
#include "timing.h"

#include "canonicalform.h"
#include "cf_map.h"
#include "cf_util.h"
#include "ftmpl_functions.h"
#include "cf_random.h"

// iinline helper functions:
#include "cf_map_ext.inc"

#ifdef HAVE_NTL
#include <NTL/ZZ_pEX.h>
#endif

#ifdef HAVE_NTL

TIMING_DEFINE_PRINT(gcd_recursion);
TIMING_DEFINE_PRINT(newton_interpolation);

/// compressing two polynomials F and G, M is used for compressing, 
/// N to reverse the compression
int myCompress (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                CFMap & N, bool& top_level) 
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

  if (top_level) 
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

    if (both_non_zero == 0) return 0;

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
      for (int j= i + 1; j <=  m; j++) 
      {
        if (tmin (degsf[j],degsg[j]) >= max_min_deg) 
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

/// compute the leading coefficient of F, where F is considered as an element
/// of \f$ R[x_{1}][x_{2},\ldots ,x_{n}] \f$, order on Mon (x_{2},\ldots ,x_{n})
/// is dp.
static inline
CanonicalForm uni_lcoeff (const CanonicalForm& F) 
{
  if (F.level() <= 1)
    return F; 
  else
  {
    Variable x= Variable (2);
    int deg= totaldegree (F, x, F.mvar());
    for (CFIterator i= F; i.hasTerms(); i++)
    {
      if (i.exp() + totaldegree (i.coeff(), x, i.coeff().mvar()) == deg)
        return uni_lcoeff (i.coeff()); 
    }
  }
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
newtonInterp(const CanonicalForm alpha, const CanonicalForm u, const CanonicalForm newtonPoly, const CanonicalForm oldInterPoly, const Variable & x)
{
  CanonicalForm interPoly;

  interPoly = oldInterPoly + ((u - oldInterPoly(alpha, x)) / newtonPoly(alpha, x)) * newtonPoly;
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
  int bound= ipower (p, d);
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

/// chooses a suitable extension of \f$ F_{p}(\alpha ) \f$ 
/// we do not extend \f$ F_{p}(\alpha ) \f$ itself but extend \f$ F_{p} \f$ ,
/// s.t. \f$ F_{p}(\alpha ) \subset F_{p}(\beta ) \f$ 
static inline
void choose_extension (const int& d, const int& num_vars, Variable& beta) 
{
  int p= getCharacteristic();
  ZZ NTLp= to_ZZ (p);
  ZZ_p::init (NTLp);
  ZZ_pX NTLirredpoly;
  //TODO: replace d by max_{i} (deg_x{i}(f))
  int i= (int) (log ((double) ipower (d + 1, num_vars))/log ((double) p)); 
  int m= degree (getMipo (beta));
  if (i <= 1)
    i= 2;
  BuildIrred (NTLirredpoly, i*m); 
  CanonicalForm mipo= convertNTLZZpX2CF (NTLirredpoly, Variable(1)); 
  beta= rootOf (mipo); 
}

/// GCD of F and G over \f$ F_{p}(\alpha ) \f$ , 
/// l and top_level are only used internally, output is monic
/// based on Alg. 7.2. as described in "Algorithms for
/// Computer Algebra" by Geddes, Czapor, Labahn
CanonicalForm 
GCD_Fp_extension (const CanonicalForm& F, const CanonicalForm& G, 
                  Variable & alpha, CFList& l, bool& top_level) 
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
  int best_level= myCompress (A, B, M, N, top_level);

  if (best_level == 0) return B.genOne();

  A= M(A);
  B= M(B);

  Variable x= Variable(1);

  //univariate case 
  if (A.isUnivariate() && B.isUnivariate()) 
    return N (gcd(A,B)); 
  
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
  
  if (fdivides (lcA, lcB)) { 
    if (fdivides (A, B))
      return F/Lc(F);    
  }
  if (fdivides (lcB, lcA))
  { 
    if (fdivides (B, A)) 
      return G/Lc(G);
  }

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable(2), Variable (ppA.level()));

  if (d == 0) return N(gcdcAcB);
  int d0= totaldegree (ppB, Variable(2), Variable (ppB.level()));
  if (d0 < d) 
    d= d0; 
  if (d == 0) return N(gcdcAcB);

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly;

  newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  H= 0;
  bool fail= false;
  top_level= false;
  bool inextension= false;
  Variable V_buf= alpha;
  CanonicalForm prim_elem, im_prim_elem;
  CFList source, dest;
  do 
  {
    random_element= randomElement (m, V_buf, l, fail);
    if (fail) 
    {
      source= CFList();
      dest= CFList();
      int num_vars= tmin (getNumVars(A), getNumVars(B));
      num_vars--;

      choose_extension (d, num_vars, V_buf);
      bool prim_fail= false;
      Variable V_buf2;
      prim_elem= primitiveElement (alpha, V_buf2, prim_fail);

      ASSERT (!prim_fail, "failure in integer factorizer");
      if (prim_fail)
        ; //ERROR
      else
        im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));
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
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x), V_buf,
                        list, top_level);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else 
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= 
      GCD_Fp_extension (ppA(random_element, x), ppB(random_element, x), V_buf,
                        list, top_level);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    d0= totaldegree (G_random_element, Variable(2), 
                     Variable (G_random_element.level()));
    if (d0 == 0) return N(gcdcAcB); 
    if (d0 >  d) 
    {
      if (!find (l, random_element))
        l.append (random_element);
      continue;
    }

    G_random_element= 
    (gcdlcAlcB(random_element, x)/uni_lcoeff (G_random_element))
    * G_random_element;

    d0= totaldegree (G_random_element, Variable(2), 
                     Variable(G_random_element.level()));
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
        DEBOUTLN (cerr, "ppH before mapDown= " << ppH);
        ppH= mapDown (ppH, prim_elem, im_prim_elem, alpha, u, v); 
        ppH /= Lc(ppH);
        DEBOUTLN (cerr, "ppH after mapDown= " << ppH);
        if (fdivides (ppH, A) && fdivides (ppH, B))         
          return N(gcdcAcB*ppH);
      }
      else if (fdivides (ppH, A) && fdivides (ppH, B)) 
        return N(gcdcAcB*(ppH/Lc(ppH)));
    }

    G_m= H;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);
  } while (1);
}

/// compute a random element a of GF, s.t. F(a) \f$ \neq 0 \f$ , F is a 
/// univariate polynomial, returns fail if there are no field elements left
/// which have not been used before
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

/// GCD of F and G over GF, based on Alg. 7.2. as described in "Algorithms for
/// Computer Algebra" by Geddes, Czapor, Labahn
/// Usually this algorithm will be faster than GCD_Fp_extension since GF has
/// faster field arithmetics, however it might fail if the input is large since
/// the size of the base field is bounded by 2^16, output is monic
CanonicalForm 
GCD_GF (const CanonicalForm& F, const CanonicalForm& G, CFList& l, 
        bool& top_level) 
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
  int best_level= myCompress (A, B, M, N, top_level);

  if (best_level == 0) return B.genOne();

  A= M(A);
  B= M(B);

  Variable x= Variable(1);

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
      return F; 
  }   
  if (fdivides (lcB, lcA)) 
  { 
    if (fdivides (B, A)) 
      return G;
  }

  gcdlcAlcB= gcd (lcA, lcB);

  int d= totaldegree (ppA, Variable(2), Variable (ppA.level()));
  if (d == 0) return N(gcdcAcB);
  int d0= totaldegree (ppB, Variable(2), Variable (ppB.level()));
  if (d0 < d) 
    d= d0; 
  if (d == 0) return N(gcdcAcB);

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly;

  newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  H= 0;
  bool fail= false;
  top_level= false;
  bool inextension= false;
  int p;
  int k= getGFDegree();
  int kk;
  int expon;
  char gf_name_buf= gf_name;
  do 
  {
    random_element= GFRandomElement (m, l, fail);
    if (fail) 
    { 
      int num_vars= tmin (getNumVars(A), getNumVars(B));
      num_vars--;
      p= getCharacteristic();
      expon= floor ((log ((double) ipower (d + 1, num_vars))/log ((double) p)));
      if (expon < 2)
        expon= 2;
      kk= getGFDegree(); 
      if (ipower (p, kk*expon) < (1 << 16)) 
        setCharacteristic (p, kk*(int)expon, 'b');
      else 
      {
        expon= floor((log ((double)((1<<16) - 1)))/(log((double)p)*kk));
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
      ppA= GFMapUp (ppA, kk);
      ppB= GFMapUp (ppB, kk);
      gcdlcAlcB= GFMapUp (gcdlcAlcB, kk);
      random_element= GFRandomElement (m, l, fail);
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= GCD_GF (ppA(random_element, x), ppB(random_element, x), 
                                list, top_level);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else 
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= GCD_GF (ppA(random_element, x), ppB(random_element, x), 
                                list, top_level);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    d0= totaldegree (G_random_element, Variable(2), 
                     Variable (G_random_element.level()));
    if (d0 == 0) 
    {
      if (inextension) 
      {
        setCharacteristic (p, k, gf_name_buf);
        return N(gcdcAcB); 
      }
      else
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
    d0= totaldegree (G_random_element, Variable(2), 
                     Variable (G_random_element.level()));

    if (d0 < d) 
    {
      m= gcdlcAlcB;
      newtonPoly= 1;
      G_m= 0;
      d= d0;
    }

    TIMING_START (newton_interpolation);
    H= newtonInterp (random_element, G_random_element, newtonPoly, G_m, x);
    TIMING_END_AND_PRINT (newton_interpolation, "time for newton interpolation: ");

    //termination test 
    if (uni_lcoeff (H) == gcdlcAlcB) 
    {
      cH= uni_content (H);
      ppH= H/cH;
      if (inextension) 
      {
        if (fdivides(ppH, GFMapUp(A, k)) && fdivides(ppH, GFMapUp(B,k))) 
        {
          DEBOUTLN (cerr, "ppH before mapDown= " << ppH);
          ppH= GFMapDown (ppH, k);
          DEBOUTLN (cerr, "ppH after mapDown= " << ppH);
          setCharacteristic (p, k, gf_name_buf);
          return N(gcdcAcB*ppH);
        }
      }
      else 
      {
        if (fdivides (ppH, A) && fdivides (ppH, B)) 
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

/// F is assumed to be an univariate polynomial in x,
/// computes a random monic irreducible univariate polynomial of random 
/// degree < i in x which does not divide F
CanonicalForm 
randomIrredpoly (int i, const Variable & x, 
                 CFList & list) 
{
  int random;
  int p= getCharacteristic();
  ZZ NTLp= to_ZZ (p);
  ZZ_p::init (NTLp);
  ZZ_pX NTLirredpoly; 
  CanonicalForm CFirredpoly;
  int buf= ceil((log((double) i) / log((double) p)));
  buf= buf + 1;
  int bound= 0;
  //lower bound on the number of monic irreducibles of degree less than buf
  for (int j= 2; j < buf; j++) 
    bound += ((ipower (p, j) - 2*ipower(p, j/2)) / j);
  if (list.length() > bound) 
  {
    if (buf > 1)
      buf--;
    buf *= 2;
    buf++;
  }
  for (int j=  ((buf - 1)/2) + 1; j < buf; j++) 
    bound += ((ipower (p, j) - 2*ipower(p, j/2)) / j);
  do 
  {
    if (list.length() <= bound) 
    {
      random= factoryrandom ((int) buf);  
      if (random <= 1)
        random= 2;
    }
    else 
    {
      if (buf > 1) 
        buf--;
      buf *= 2;
      buf++;
      for (int j= ((int) (buf - 1)/2) + 1; j < buf; j++) 
        bound += ((ipower (p, j) - 2*ipower(p, j/2)) / j);
      random= factoryrandom ((int) buf);
      if (random <= 1)
        random= 2;
    } 
    BuildIrred (NTLirredpoly, random);
    CFirredpoly= convertNTLZZpX2CF (NTLirredpoly, x);
  } while (find (list, CFirredpoly));
  return CFirredpoly;
} 

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

CanonicalForm GCD_small_p (const CanonicalForm& F, const CanonicalForm&  G, 
                           bool& top_level, CFList& l) 
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
  int best_level= myCompress (A, B, M, N, top_level);

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

  if (d == 0) return N(gcdcAcB);
  d0= totaldegree (ppB, Variable (2), Variable (ppB.level()));

  if (d0 < d) 
    d= d0;

  if (d == 0) return N(gcdcAcB);

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly= 1;
  m= gcdlcAlcB;
  H= 0;
  G_m= 0;
  Variable alpha, V_buf;
  int p, k;
  bool fail= false;
  bool inextension= false;
  bool inextensionextension= false;
  top_level= false;
  CanonicalForm CF_buf;
  CFList source, dest;
  CanonicalForm gcdcheck;
  do 
  {
    if (inextension)
      random_element= randomElement (m, alpha, l, fail);
    else
      random_element= FpRandomElement (m, l, fail);

    if (!fail && !inextension)
    {
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= 
      GCD_small_p (ppA (random_element,x), ppB (random_element,x), top_level, 
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
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x), alpha, 
                        list, top_level);
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
      int deg= 3;
      do {
        mipo= randomIrredpoly (deg, x, list); 
        alpha= rootOf (mipo);
        inextension= true;
        fail= false;
        random_element= randomElement (m, alpha, l, fail); 
        deg++;
      } while (fail); 
      list= CFList();
      TIMING_START (gcd_recursion);
      G_random_element= 
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x), alpha, 
                        list, top_level);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }
    else if (fail && inextension)
    {
      source= CFList();
      dest= CFList();
      int num_vars= tmin (getNumVars(A), getNumVars(B));
      num_vars--;
      V_buf= alpha;
      choose_extension (d, num_vars, V_buf);
      bool prim_fail= false;
      Variable V_buf2;
      CanonicalForm prim_elem, im_prim_elem;
      prim_elem= primitiveElement (alpha, V_buf2, prim_fail);
       
      ASSERT (!prim_fail, "failure in integer factorizer");
      if (prim_fail)
        ; //ERROR
      else
        im_prim_elem= mapPrimElem (prim_elem, alpha, V_buf);

      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (alpha));
      DEBOUTLN (cerr, "getMipo (alpha)= " << getMipo (V_buf2));

      inextensionextension= true;
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
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x), V_buf,
                        list, top_level);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
      alpha= V_buf;
    } 

    d0= totaldegree (G_random_element, Variable(2), 
                     Variable (G_random_element.level()));

    if (d0 == 0)
    {
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

    
    d0= totaldegree (G_random_element, Variable(2), 
                     Variable(G_random_element.level()));

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
                          "time for newton_interpolation: ");

    //termination test
    if (uni_lcoeff (H) == gcdlcAlcB) 
    {
      cH= uni_content (H);
      ppH= H/cH;
      ppH /= Lc (ppH);
      DEBOUTLN (cerr, "ppH= " << ppH);
      if (fdivides (ppH, A) && fdivides (ppH, B)) 
        return N(gcdcAcB*ppH);
    }

    G_m= H;
    newtonPoly= newtonPoly*(x - random_element);
    m= m*(x - random_element);
    if (!find (l, random_element))
      l.append (random_element);
  } while (1);
}

#endif
