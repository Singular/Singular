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
#include "cf_map_ext.h"

#ifdef HAVE_NTL
#include <NTL/ZZ_pEX.h>
#include <NTLconvert.h>
#endif

#include "cf_gcd_smallp.h"

#ifdef HAVE_NTL

TIMING_DEFINE_PRINT(gcd_recursion);
TIMING_DEFINE_PRINT(newton_interpolation);

/// compressing two polynomials F and G, M is used for compressing, 
/// N to reverse the compression
static inline
int myCompress (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                CFMap & N, bool& topLevel) 
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

int
substituteCheck (const CanonicalForm& F, const CanonicalForm& G) 
{
  if (F.inCoeffDomain() || G.inCoeffDomain())
    return 0;
  Variable x= Variable (1);
  if (degree (F, x) <= 1 || degree (G, x) <= 1)
    return 0;
  CanonicalForm f= swapvar (F, F.mvar(), x); //TODO swapping seems to be pretty expensive 
  CanonicalForm g= swapvar (G, G.mvar(), x); 
  int sizef= 0;
  int sizeg= 0; 
  for (CFIterator i= f; i.hasTerms(); i++, sizef++)
  {
    if (i.exp() == 1)
      return 0;
  }
  for (CFIterator i= g; i.hasTerms(); i++, sizeg++)
  {
    if (i.exp() == 1)
      return 0;
  }
  int * expf= new int [sizef];
  int * expg= new int [sizeg];
  int j= 0;
  for (CFIterator i= f; i.hasTerms(); i++, j++)
  {
    expf [j]= i.exp();
  }
  j= 0;
  for (CFIterator i= g; i.hasTerms(); i++, j++)
  {
    expg [j]= i.exp();
  }
  
  int indf= sizef - 1;
  int indg= sizeg - 1;
  if (expf[indf] == 0)
    indf--;
  if (expg[indg] == 0)
    indg--;
    
  if (expg[indg] != expf [indf] || (expg[indg] == 1 && expf[indf] == 1))
  {
    delete [] expg;
    delete [] expf;
    return 0;
  }
  // expf[indg] == expf[indf] after here
  int result= expg[indg];
  for (int i= indf - 1; i >= 0; i--)
  {
    if (expf [i]%result != 0)
    {
      delete [] expg;
      delete [] expf;
      return 0;
    }
  }
  
  for (int i= indg - 1; i >= 0; i--)
  {
    if (expg [i]%result != 0)
    {
      delete [] expg;
      delete [] expf;
      return 0;
    }
  }

  delete [] expg;
  delete [] expf;
  return result;
}

// substiution
void 
subst (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& A, 
       CanonicalForm& B, const int d
      )
{
  if (d == 1)
  {
    A= F;
    B= G;
    return;
  }
  
  CanonicalForm C= 0;
  CanonicalForm D= 0;  
  Variable x= Variable (1);
  CanonicalForm f= swapvar (F, x, F.mvar());
  CanonicalForm g= swapvar (G, x, G.mvar());
  for (CFIterator i= f; i.hasTerms(); i++)
    C += i.coeff()*power (f.mvar(), i.exp()/ d);
  for (CFIterator i= g; i.hasTerms(); i++)
    D += i.coeff()*power (g.mvar(), i.exp()/ d);
  A= swapvar (C, x, F.mvar());
  B= swapvar (D, x, G.mvar());
}

CanonicalForm 
reverseSubst (const CanonicalForm& F, const int d)
{
  if (d == 1)
    return F;
  
  Variable x= Variable (1);
  if (degree (F, x) <= 0)
    return F;
  CanonicalForm f= swapvar (F, x, F.mvar());
  CanonicalForm result= 0;
  for (CFIterator i= f; i.hasTerms(); i++)
    result += i.coeff()*power (f.mvar(), d*i.exp());
  return swapvar (result, x, F.mvar());   
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
/// l and topLevel are only used internally, output is monic
/// based on Alg. 7.2. as described in "Algorithms for
/// Computer Algebra" by Geddes, Czapor, Labahn
CanonicalForm 
GCD_Fp_extension (const CanonicalForm& F, const CanonicalForm& G, 
                  Variable & alpha, CFList& l, bool& topLevel) 
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

  Variable x= Variable(1);

  //univariate case 
  if (A.isUnivariate() && B.isUnivariate()) 
    return N (gcd(A,B)); 
  
  int substitute= substituteCheck (A, B);
  
  if (substitute > 1)
    subst (A, B, A, B, substitute);

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  if (topLevel)
  {
    if (best_level <= 2)
      gcdcAcB= extractContents (A, B, cA, cB, ppA, ppB, best_level);
    else 
      gcdcAcB= extractContents (A, B, cA, cB, ppA, ppB, 2);  
  }
  else
  {
    cA = uni_content (A);
    cB = uni_content (B); 
    gcdcAcB= gcd (cA, cB);
    ppA= A/cA;
    ppB= B/cB;
  }

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

  int d= totaldegree (ppA, Variable(2), Variable (ppA.level()));

  if (d == 0)
  {
    if (substitute > 1)
      return N (reverseSubst (gcdcAcB, substitute));
    else 
      return N(gcdcAcB);
  }
  int d0= totaldegree (ppB, Variable(2), Variable (ppB.level()));
  if (d0 < d) 
    d= d0; 
  if (d == 0)
  {
    if (substitute > 1)
      return N (reverseSubst (gcdcAcB, substitute));
    else 
      return N(gcdcAcB);
  }

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly;

  newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  H= 0;
  bool fail= false;
  topLevel= false;
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
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x), V_buf,
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
      GCD_Fp_extension (ppA(random_element, x), ppB(random_element, x), V_buf,
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
    }

    d0= totaldegree (G_random_element, Variable(2), 
                     Variable (G_random_element.level()));
    if (d0 == 0)
    {
      if (substitute > 1)
        return N (reverseSubst (gcdcAcB, substitute));
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
        {
          if (substitute > 1)
          {
            ppH= reverseSubst (ppH, substitute);
            gcdcAcB= reverseSubst (gcdcAcB, substitute);
          }
          return N(gcdcAcB*ppH);
        }
      }
      else if (fdivides (ppH, A) && fdivides (ppH, B)) 
      {
        if (substitute > 1)
        {
          ppH= reverseSubst (ppH, substitute);
          gcdcAcB= reverseSubst (gcdcAcB, substitute);
        }
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

/// GCD of F and G over GF, based on Alg. 7.2. as described in "Algorithms for
/// Computer Algebra" by Geddes, Czapor, Labahn
/// Usually this algorithm will be faster than GCD_Fp_extension since GF has
/// faster field arithmetics, however it might fail if the input is large since
/// the size of the base field is bounded by 2^16, output is monic
CanonicalForm GCD_GF (const CanonicalForm& F, const CanonicalForm& G,
        CFList& l, bool& topLevel) 
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

  Variable x= Variable(1);

  //univariate case 
  if (A.isUnivariate() && B.isUnivariate()) 
    return N (gcd (A, B)); 

  int substitute= substituteCheck (A, B);
  
  if (substitute > 1)
    subst (A, B, A, B, substitute);

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  if (topLevel)
  {
    if (best_level <= 2)
      gcdcAcB= extractContents (A, B, cA, cB, ppA, ppB, best_level);
    else 
      gcdcAcB= extractContents (A, B, cA, cB, ppA, ppB, 2);  
  }
  else
  {
    cA = uni_content (A);
    cB = uni_content (B); 
    gcdcAcB= gcd (cA, cB);
    ppA= A/cA;
    ppB= B/cB;
  }

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
  if (d == 0)
  {
    if (substitute > 1)
      return N (reverseSubst (gcdcAcB, substitute));
    else 
      return N(gcdcAcB);
  }
  int d0= totaldegree (ppB, Variable(2), Variable (ppB.level()));
  if (d0 < d) 
    d= d0; 
  if (d == 0)
  {
    if (substitute > 1)
      return N (reverseSubst (gcdcAcB, substitute));
    else 
      return N(gcdcAcB);
  }

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly;

  newtonPoly= 1;
  m= gcdlcAlcB;
  G_m= 0;
  H= 0;
  bool fail= false;
  topLevel= false;
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
      expon= (int) floor ((log ((double) ipower (d + 1, num_vars))/log ((double) p)));
      if (expon < 2)
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
      ppA= GFMapUp (ppA, kk);
      ppB= GFMapUp (ppB, kk);
      gcdlcAlcB= GFMapUp (gcdlcAlcB, kk);
      random_element= GFRandomElement (m, l, fail);
      DEBOUTLN (cerr, "fail= " << fail);
      CFList list;
      TIMING_START (gcd_recursion);
      G_random_element= GCD_GF (ppA(random_element, x), ppB(random_element, x), 
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
                                list, topLevel);
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
        {
          if (substitute > 1)
            return N (reverseSubst (gcdcAcB, substitute));
          else
            return N(gcdcAcB); 
        } 
      }
      else
      {
        if (substitute > 1)
          return N (reverseSubst (gcdcAcB, substitute));
        else
          return N(gcdcAcB); 
      }
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
          if (substitute > 1)
          {
            ppH= reverseSubst (ppH, substitute);
            gcdcAcB= reverseSubst (gcdcAcB, substitute);
          }
          setCharacteristic (p, k, gf_name_buf);
          return N(gcdcAcB*ppH);
        }
      }
      else 
      {
        if (fdivides (ppH, A) && fdivides (ppH, B)) 
        {
          if (substitute > 1)
          {
            ppH= reverseSubst (ppH, substitute);
            gcdcAcB= reverseSubst (gcdcAcB, substitute);
          }
          return N(gcdcAcB*ppH);
        }
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
randomIrredpoly (int i, const Variable & x) 
{
  int p= getCharacteristic();
  ZZ NTLp= to_ZZ (p);
  ZZ_p::init (NTLp);
  ZZ_pX NTLirredpoly; 
  CanonicalForm CFirredpoly;
  BuildIrred (NTLirredpoly, i + 1);
  CFirredpoly= convertNTLZZpX2CF (NTLirredpoly, x);
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

CanonicalForm GCD_small_p (const CanonicalForm& F, const CanonicalForm&  G, 
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

  int substitute= substituteCheck (A, B);
  
  if (substitute > 1)
    subst (A, B, A, B, substitute);

  CanonicalForm cA, cB;    // content of A and B
  CanonicalForm ppA, ppB;    // primitive part of A and B
  CanonicalForm gcdcAcB;

  if (topLevel)
  {
    if (best_level <= 2)
      gcdcAcB= extractContents (A, B, cA, cB, ppA, ppB, best_level);
    else 
      gcdcAcB= extractContents (A, B, cA, cB, ppA, ppB, 2);  
  }
  else
  {
    cA = uni_content (A);
    cB = uni_content (B); 
    gcdcAcB= gcd (cA, cB);
    ppA= A/cA;
    ppB= B/cB;
  }

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
  {
    if (substitute > 1)
      return N (reverseSubst (gcdcAcB, substitute));
    else 
      return N(gcdcAcB);
  }
  d0= totaldegree (ppB, Variable (2), Variable (ppB.level()));

  if (d0 < d) 
    d= d0;

  if (d == 0) 
  {
    if (substitute > 1)
      return N (reverseSubst (gcdcAcB, substitute));
    else 
      return N(gcdcAcB);
  }

  CanonicalForm m, random_element, G_m, G_random_element, H, cH, ppH;
  CanonicalForm newtonPoly= 1;
  m= gcdlcAlcB;
  H= 0;
  G_m= 0;
  Variable alpha, V_buf;
  bool fail= false;
  bool inextension= false;
  bool inextensionextension= false;
  topLevel= false;
  CFList source, dest;
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
      GCD_small_p (ppA (random_element,x), ppB (random_element,x), topLevel, 
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
        random_element= randomElement (m, alpha, l, fail); 
        deg++;
      } while (fail); 
      list= CFList();
      TIMING_START (gcd_recursion);
      G_random_element= 
      GCD_Fp_extension (ppA (random_element, x), ppB (random_element, x), alpha, 
                        list, topLevel);
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
                        list, topLevel);
      TIMING_END_AND_PRINT (gcd_recursion, 
                            "time for recursive call: ");
      DEBOUTLN (cerr, "G_random_element= " << G_random_element);
      alpha= V_buf;
    } 

    d0= totaldegree (G_random_element, Variable(2), 
                     Variable (G_random_element.level()));

    if (d0 == 0)
    {
      if (substitute > 1)
        return N (reverseSubst (gcdcAcB, substitute));
      else
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
      {
        if (substitute > 1)
        {
          ppH= reverseSubst (ppH, substitute);
          gcdcAcB= reverseSubst (gcdcAcB, substitute);
        }
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

#endif
