
#include "config.h"


#ifndef NOSTREAMIO
#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif
#ifdef HAVE_IOSTREAM_H
#include <iostream.h>
#elif defined(HAVE_IOSTREAM)
#include <iostream>
#endif
#endif

#include "cf_assert.h"
#include "timing.h"

#include "templates/ftmpl_functions.h"
#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "cf_algorithm.h"
#include "cfGcdAlgExt.h"
#include "cfUnivarGcd.h"
#include "cf_map.h"
#include "cf_generator.h"
#include "facMul.h"
#include "cfNTLzzpEXGCD.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

TIMING_DEFINE_PRINT(alg_content_p)
TIMING_DEFINE_PRINT(alg_content)
TIMING_DEFINE_PRINT(alg_compress)
TIMING_DEFINE_PRINT(alg_termination)
TIMING_DEFINE_PRINT(alg_termination_p)
TIMING_DEFINE_PRINT(alg_reconstruction)
TIMING_DEFINE_PRINT(alg_newton_p)
TIMING_DEFINE_PRINT(alg_recursion_p)
TIMING_DEFINE_PRINT(alg_gcd_p)
TIMING_DEFINE_PRINT(alg_euclid_p)

/// compressing two polynomials F and G, M is used for compressing,
/// N to reverse the compression
static int myCompress (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                CFMap & N, bool topLevel)
{
  int n= tmax (F.level(), G.level());
  int * degsf= NEW_ARRAY(int,n + 1);
  int * degsg= NEW_ARRAY(int,n + 1);

  for (int i = 0; i <= n; i++)
    degsf[i]= degsg[i]= 0;

  degsf= degrees (F, degsf);
  degsg= degrees (G, degsg);

  int both_non_zero= 0;
  int f_zero= 0;
  int g_zero= 0;
  int both_zero= 0;
  int Flevel=F.level();
  int Glevel=G.level();

  if (topLevel)
  {
    for (int i= 1; i <= n; i++)
    {
      if (degsf[i] != 0 && degsg[i] != 0)
      {
        both_non_zero++;
        continue;
      }
      if (degsf[i] == 0 && degsg[i] != 0 && i <= Glevel)
      {
        f_zero++;
        continue;
      }
      if (degsg[i] == 0 && degsf[i] && i <= Flevel)
      {
        g_zero++;
        continue;
      }
    }

    if (both_non_zero == 0)
    {
      DELETE_ARRAY(degsf);
      DELETE_ARRAY(degsg);
      return 0;
    }

    // map Variables which do not occur in both polynomials to higher levels
    int k= 1;
    int l= 1;
    for (int i= 1; i <= n; i++)
    {
      if (degsf[i] != 0 && degsg[i] == 0 && i <= Flevel)
      {
        if (k + both_non_zero != i)
        {
          M.newpair (Variable (i), Variable (k + both_non_zero));
          N.newpair (Variable (k + both_non_zero), Variable (i));
        }
        k++;
      }
      if (degsf[i] == 0 && degsg[i] != 0 && i <= Glevel)
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
    int m= tmax (Flevel, Glevel);
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
        min_max_deg= tmax (degsf[i], degsg[i]);
        if (degsf [i] != 0 && degsg [i] != 0)
          min_max_deg= tmax (degsf[i], degsg[i]);
        else
          min_max_deg= 0;
      }
      for (int j= i + 1; j <=  m; j++)
      {
        if (tmax (degsf[j],degsg[j]) <= min_max_deg && degsf[j] != 0 && degsg [j] != 0)
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

  DELETE_ARRAY(degsf);
  DELETE_ARRAY(degsg);

  return 1;
}

void tryInvert( const CanonicalForm & F, const CanonicalForm & M, CanonicalForm & inv, bool & fail )
{ // F, M are required to be "univariate" polynomials in an algebraic variable
  // we try to invert F modulo M
  if(F.inBaseDomain())
  {
    if(F.isZero())
    {
      fail = true;
      return;
    }
    inv = 1/F;
    return;
  }
  CanonicalForm b;
  Variable a = M.mvar();
  Variable x = Variable(1);
  if(!extgcd( replacevar( F, a, x ), replacevar( M, a, x ), inv, b ).isOne())
    fail = true;
  else
    inv = replacevar( inv, x, a ); // change back to alg var
}

#ifndef HAVE_NTL
void tryDivrem (const CanonicalForm& F, const CanonicalForm& G, CanonicalForm& Q,
                CanonicalForm& R, CanonicalForm& inv, const CanonicalForm& mipo,
                bool& fail)
{
  if (F.inCoeffDomain())
  {
    Q= 0;
    R= F;
    return;
  }

  CanonicalForm A, B;
  Variable x= F.mvar();
  A= F;
  B= G;
  int degA= degree (A, x);
  int degB= degree (B, x);

  if (degA < degB)
  {
    R= A;
    Q= 0;
    return;
  }

  tryInvert (Lc (B), mipo, inv, fail);
  if (fail)
    return;

  R= A;
  Q= 0;
  CanonicalForm Qi;
  for (int i= degA -degB; i >= 0; i--)
  {
    if (degree (R, x) == i + degB)
    {
      Qi= Lc (R)*inv*power (x, i);
      Qi= reduce (Qi, mipo);
      R -= Qi*B;
      R= reduce (R, mipo);
      Q += Qi;
    }
  }
}

void tryEuclid( const CanonicalForm & A, const CanonicalForm & B, const CanonicalForm & M, CanonicalForm & result, bool & fail )
{
  CanonicalForm P;
  if(A.inCoeffDomain())
  {
    tryInvert( A, M, P, fail );
    if(fail)
      return;
    result = 1;
    return;
  }
  if(B.inCoeffDomain())
  {
    tryInvert( B, M, P, fail );
    if(fail)
      return;
    result = 1;
    return;
  }
  // here: both not inCoeffDomain
  if( A.degree() > B.degree() )
  {
    P = A; result = B;
  }
  else
  {
    P = B; result = A;
  }
  CanonicalForm inv;
  if( result.isZero() )
  {
    tryInvert( Lc(P), M, inv, fail );
    if(fail)
      return;
    result = inv*P; // monify result (not reduced, yet)
    result= reduce (result, M);
    return;
  }
  Variable x = P.mvar();
  CanonicalForm rem, Q;
  // here: degree(P) >= degree(result)
  while(true)
  {
    tryDivrem (P, result, Q, rem, inv, M, fail);
    if (fail)
      return;
    if( rem.isZero() )
    {
      result *= inv;
      result= reduce (result, M);
      return;
    }
    if(result.degree(x) >= rem.degree(x))
    {
      P = result;
      result = rem;
    }
    else
      P = rem;
  }
}
#endif

static CanonicalForm trycontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail );
static CanonicalForm tryvcontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail );
static CanonicalForm trycf_content ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & M, bool & fail );

static inline CanonicalForm
tryNewtonInterp (const CanonicalForm & alpha, const CanonicalForm & u,
              const CanonicalForm & newtonPoly, const CanonicalForm & oldInterPoly,
              const Variable & x, const CanonicalForm& M, bool& fail)
{
  CanonicalForm interPoly;

  CanonicalForm inv;
  tryInvert (newtonPoly (alpha, x), M, inv, fail);
  if (fail)
    return 0;

  interPoly= oldInterPoly+reduce ((u - oldInterPoly (alpha, x))*inv*newtonPoly, M);
  return interPoly;
}

void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail, bool topLevel )
{ // assume F,G are multivariate polys over Z/p(a) for big prime p, M "univariate" polynomial in an algebraic variable
  // M is assumed to be monic
  if(F.isZero())
  {
    if(G.isZero())
    {
      result = G; // G is zero
      return;
    }
    if(G.inCoeffDomain())
    {
      tryInvert(G,M,result,fail);
      if(fail)
        return;
      result = 1;
      return;
    }
    // try to make G monic modulo M
    CanonicalForm inv;
    tryInvert(Lc(G),M,inv,fail);
    if(fail)
      return;
    result = inv*G;
    result= reduce (result, M);
    return;
  }
  if(G.isZero()) // F is non-zero
  {
    if(F.inCoeffDomain())
    {
      tryInvert(F,M,result,fail);
      if(fail)
        return;
      result = 1;
      return;
    }
    // try to make F monic modulo M
    CanonicalForm inv;
    tryInvert(Lc(F),M,inv,fail);
    if(fail)
      return;
    result = inv*F;
    result= reduce (result, M);
    return;
  }
  // here: F,G both nonzero
  if(F.inCoeffDomain())
  {
    tryInvert(F,M,result,fail);
    if(fail)
      return;
    result = 1;
    return;
  }
  if(G.inCoeffDomain())
  {
    tryInvert(G,M,result,fail);
    if(fail)
      return;
    result = 1;
    return;
  }
  TIMING_START (alg_compress)
  CFMap MM,NN;
  int lev= myCompress (F, G, MM, NN, topLevel);
  if (lev == 0)
  {
    result= 1;
    return;
  }
  CanonicalForm f=MM(F);
  CanonicalForm g=MM(G);
  TIMING_END_AND_PRINT (alg_compress, "time to compress in alg gcd: ")
  // here: f,g are compressed
  // compute largest variable in f or g (least one is Variable(1))
  int mv = f.level();
  if(g.level() > mv)
    mv = g.level();
  // here: mv is level of the largest variable in f, g
  Variable v1= Variable (1);
#ifdef HAVE_NTL
  Variable v= M.mvar();
  int ch=getCharacteristic();
  if (fac_NTL_char != ch)
  {
    fac_NTL_char= ch;
    zz_p::init (ch);
  }
  zz_pX NTLMipo= convertFacCF2NTLzzpX (M);
  zz_pE::init (NTLMipo);
  zz_pEX NTLResult;
  zz_pEX NTLF;
  zz_pEX NTLG;
#endif
  if(mv == 1) // f,g univariate
  {
    TIMING_START (alg_euclid_p)
#ifdef HAVE_NTL
    NTLF= convertFacCF2NTLzz_pEX (f, NTLMipo);
    NTLG= convertFacCF2NTLzz_pEX (g, NTLMipo);
    tryNTLGCD (NTLResult, NTLF, NTLG, fail);
    if (fail)
      return;
    result= convertNTLzz_pEX2CF (NTLResult, f.mvar(), v);
#else
    tryEuclid(f,g,M,result,fail);
    if(fail)
      return;
#endif
    TIMING_END_AND_PRINT (alg_euclid_p, "time for euclidean alg mod p: ")
    result= NN (reduce (result, M)); // do not forget to map back
    return;
  }
  TIMING_START (alg_content_p)
  // here: mv > 1
  CanonicalForm cf = tryvcontent(f, Variable(2), M, fail); // cf is univariate poly in var(1)
  if(fail)
    return;
  CanonicalForm cg = tryvcontent(g, Variable(2), M, fail);
  if(fail)
    return;
  CanonicalForm c;
#ifdef HAVE_NTL
  NTLF= convertFacCF2NTLzz_pEX (cf, NTLMipo);
  NTLG= convertFacCF2NTLzz_pEX (cg, NTLMipo);
  tryNTLGCD (NTLResult, NTLF, NTLG, fail);
  if (fail)
    return;
  c= convertNTLzz_pEX2CF (NTLResult, v1, v);
#else
  tryEuclid(cf,cg,M,c,fail);
  if(fail)
    return;
#endif
  // f /= cf
  f.tryDiv (cf, M, fail);
  if(fail)
    return;
  // g /= cg
  g.tryDiv (cg, M, fail);
  if(fail)
    return;
  TIMING_END_AND_PRINT (alg_content_p, "time for content in alg gcd mod p: ")
  if(f.inCoeffDomain())
  {
    tryInvert(f,M,result,fail);
    if(fail)
      return;
    result = NN(c);
    return;
  }
  if(g.inCoeffDomain())
  {
    tryInvert(g,M,result,fail);
    if(fail)
      return;
    result = NN(c);
    return;
  }
  int *L = new int[mv+1]; // L is addressed by i from 2 to mv
  int *N = new int[mv+1];
  for(int i=2; i<=mv; i++)
    L[i] = N[i] = 0;
  L = leadDeg(f, L);
  N = leadDeg(g, N);
  CanonicalForm gamma;
  TIMING_START (alg_euclid_p)
#ifdef HAVE_NTL
  NTLF= convertFacCF2NTLzz_pEX (firstLC (f), NTLMipo);
  NTLG= convertFacCF2NTLzz_pEX (firstLC (g), NTLMipo);
  tryNTLGCD (NTLResult, NTLF, NTLG, fail);
  if (fail)
    return;
  gamma= convertNTLzz_pEX2CF (NTLResult, v1, v);
#else
  tryEuclid( firstLC(f), firstLC(g), M, gamma, fail );
  if(fail)
    return;
#endif
  TIMING_END_AND_PRINT (alg_euclid_p, "time for gcd of lcs in alg mod p: ")
  for(int i=2; i<=mv; i++) // entries at i=0,1 not visited
    if(N[i] < L[i])
      L[i] = N[i];
  // L is now upper bound for degrees of gcd
  int *dg_im = new int[mv+1]; // for the degree vector of the image we don't need any entry at i=1
  for(int i=2; i<=mv; i++)
    dg_im[i] = 0; // initialize
  CanonicalForm gamma_image, m=1;
  CanonicalForm gm=0;
  CanonicalForm g_image, alpha, gnew;
  FFGenerator gen = FFGenerator();
  Variable x= Variable (1);
  bool divides= true;
  for(FFGenerator gen = FFGenerator(); gen.hasItems(); gen.next())
  {
    alpha = gen.item();
    gamma_image = reduce(gamma(alpha, x),M); // plug in alpha for var(1)
    if(gamma_image.isZero()) // skip lc-bad points var(1)-alpha
      continue;
    TIMING_START (alg_recursion_p)
    tryBrownGCD( f(alpha, x), g(alpha, x), M, g_image, fail, false ); // recursive call with one var less
    TIMING_END_AND_PRINT (alg_recursion_p,
                         "time for recursive calls in alg gcd mod p: ")
    if(fail)
      return;
    g_image = reduce(g_image, M);
    if(g_image.inCoeffDomain()) // early termination
    {
      tryInvert(g_image,M,result,fail);
      if(fail)
        return;
      result = NN(c);
      return;
    }
    for(int i=2; i<=mv; i++)
      dg_im[i] = 0; // reset (this is necessary, because some entries may not be updated by call to leadDeg)
    dg_im = leadDeg(g_image, dg_im); // dg_im cannot be NIL-pointer
    if(isEqual(dg_im, L, 2, mv))
    {
      CanonicalForm inv;
      tryInvert (firstLC (g_image), M, inv, fail);
      if (fail)
        return;
      g_image *= inv;
      g_image *= gamma_image; // multiply by multiple of image lc(gcd)
      g_image= reduce (g_image, M);
      TIMING_START (alg_newton_p)
      gnew= tryNewtonInterp (alpha, g_image, m, gm, x, M, fail);
      TIMING_END_AND_PRINT (alg_newton_p,
                            "time for Newton interpolation in alg gcd mod p: ")
      // gnew = gm mod m
      // gnew = g_image mod var(1)-alpha
      // mnew = m * (var(1)-alpha)
      if(fail)
        return;
      m *= (x - alpha);
      if((firstLC(gnew) == gamma) || (gnew == gm)) // gnew did not change
      {
        TIMING_START (alg_termination_p)
        cf = tryvcontent(gnew, Variable(2), M, fail);
        if(fail)
          return;
        divides = true;
        g_image= gnew;
        g_image.tryDiv (cf, M, fail);
        if(fail)
          return;
        divides= tryFdivides (g_image,f, M, fail); // trial division (f)
        if(fail)
          return;
        if(divides)
        {
          bool divides2= tryFdivides (g_image,g, M, fail); // trial division (g)
          if(fail)
            return;
          if(divides2)
          {
            result = NN(reduce (c*g_image, M));
            TIMING_END_AND_PRINT (alg_termination_p,
                      "time for successful termination test in alg gcd mod p: ")
            return;
          }
        }
        TIMING_END_AND_PRINT (alg_termination_p,
                    "time for unsuccessful termination test in alg gcd mod p: ")
      }
      gm = gnew;
      continue;
    }

    if(isLess(L, dg_im, 2, mv)) // dg_im > L --> current point unlucky
      continue;

    // here: isLess(dg_im, L, 2, mv) --> all previous points were unlucky
    m = CanonicalForm(1); // reset
    gm = 0; // reset
    for(int i=2; i<=mv; i++) // tighten bound
      L[i] = dg_im[i];
  }
  // we are out of evaluation points
  fail = true;
}

static CanonicalForm
myicontent ( const CanonicalForm & f, const CanonicalForm & c )
{
#if defined(HAVE_NTL) || defined(HAVE_FLINT)
    if (f.isOne() || c.isOne())
      return 1;
    if ( f.inBaseDomain() && c.inBaseDomain())
    {
      if (c.isZero()) return abs(f);
      return bgcd( f, c );
    }
    else if ( (f.inCoeffDomain() && c.inCoeffDomain()) ||
              (f.inCoeffDomain() && c.inBaseDomain()) ||
              (f.inBaseDomain() && c.inCoeffDomain()))
    {
      if (c.isZero()) return abs (f);
#ifdef HAVE_FLINT
      fmpz_poly_t FLINTf, FLINTc;
      convertFacCF2Fmpz_poly_t (FLINTf, f);
      convertFacCF2Fmpz_poly_t (FLINTc, c);
      fmpz_poly_gcd (FLINTc, FLINTc, FLINTf);
      CanonicalForm result;
      if (f.inCoeffDomain())
        result= convertFmpz_poly_t2FacCF (FLINTc, f.mvar());
      else
        result= convertFmpz_poly_t2FacCF (FLINTc, c.mvar());
      fmpz_poly_clear (FLINTc);
      fmpz_poly_clear (FLINTf);
      return result;
#else
      ZZX NTLf= convertFacCF2NTLZZX (f);
      ZZX NTLc= convertFacCF2NTLZZX (c);
      NTLc= GCD (NTLc, NTLf);
      if (f.inCoeffDomain())
        return convertNTLZZX2CF(NTLc,f.mvar());
      else
        return convertNTLZZX2CF(NTLc,c.mvar());
#endif
    }
    else
    {
        CanonicalForm g = c;
        for ( CFIterator i = f; i.hasTerms() && ! g.isOne(); i++ )
            g = myicontent( i.coeff(), g );
        return g;
    }
#else
    return 1;
#endif
}

static CanonicalForm myicontent ( const CanonicalForm & f )
{
#if defined(HAVE_NTL) || defined(HAVE_FLINT)
    return myicontent( f, 0 );
#else
    return 1;
#endif
}

CanonicalForm QGCD( const CanonicalForm & F, const CanonicalForm & G )
{ // f,g in Q(a)[x1,...,xn]
  if(F.isZero())
  {
    if(G.isZero())
      return G; // G is zero
    if(G.inCoeffDomain())
      return CanonicalForm(1);
    CanonicalForm lcinv= 1/Lc (G);
    return G*lcinv; // return monic G
  }
  if(G.isZero()) // F is non-zero
  {
    if(F.inCoeffDomain())
      return CanonicalForm(1);
    CanonicalForm lcinv= 1/Lc (F);
    return F*lcinv; // return monic F
  }
  if(F.inCoeffDomain() || G.inCoeffDomain())
    return CanonicalForm(1);
  // here: both NOT inCoeffDomain
  CanonicalForm f, g, tmp, M, q, D, Dp, cl, newq, mipo;
  int p, i;
  int *bound, *other; // degree vectors
  bool fail;
  bool off_rational=!isOn(SW_RATIONAL);
  On( SW_RATIONAL ); // needed by bCommonDen
  f = F * bCommonDen(F);
  g = G * bCommonDen(G);
  TIMING_START (alg_content)
  CanonicalForm contf= myicontent (f);
  CanonicalForm contg= myicontent (g);
  f /= contf;
  g /= contg;
  CanonicalForm gcdcfcg= myicontent (contf, contg);
  TIMING_END_AND_PRINT (alg_content, "time for content in alg gcd: ")
  Variable a, b;
  if(hasFirstAlgVar(f,a))
  {
    if(hasFirstAlgVar(g,b))
    {
      if(b.level() > a.level())
        a = b;
    }
  }
  else
  {
    if(!hasFirstAlgVar(g,a))// both not in extension
    {
      Off( SW_RATIONAL );
      Off( SW_USE_QGCD );
      tmp = gcdcfcg*gcd( f, g );
      On( SW_USE_QGCD );
      if (off_rational) Off(SW_RATIONAL);
      return tmp;
    }
  }
  // here: a is the biggest alg. var in f and g AND some of f,g is in extension
  setReduce(a,false); // do not reduce expressions modulo mipo
  tmp = getMipo(a);
  M = tmp * bCommonDen(tmp);
  // here: f, g in Z[a][x1,...,xn], M in Z[a] not necessarily monic
  Off( SW_RATIONAL ); // needed by mod
  // calculate upper bound for degree vector of gcd
  int mv = f.level(); i = g.level();
  if(i > mv)
    mv = i;
  // here: mv is level of the largest variable in f, g
  bound = new int[mv+1]; // 'bound' could be indexed from 0 to mv, but we will only use from 1 to mv
  other = new int[mv+1];
  for(int i=1; i<=mv; i++) // initialize 'bound', 'other' with zeros
    bound[i] = other[i] = 0;
  bound = leadDeg(f,bound); // 'bound' is set the leading degree vector of f
  other = leadDeg(g,other);
  for(int i=1; i<=mv; i++) // entry at i=0 not visited
    if(other[i] < bound[i])
      bound[i] = other[i];
  // now 'bound' is the smaller vector
  cl = lc(M) * lc(f) * lc(g);
  q = 1;
  D = 0;
  CanonicalForm test= 0;
  bool equal= false;
  for( i=cf_getNumBigPrimes()-1; i>-1; i-- )
  {
    p = cf_getBigPrime(i);
    if( mod( cl, p ).isZero() ) // skip lc-bad primes
      continue;
    fail = false;
    setCharacteristic(p);
    mipo = mapinto(M);
    mipo /= mipo.lc();
    // here: mipo is monic
    TIMING_START (alg_gcd_p)
    tryBrownGCD( mapinto(f), mapinto(g), mipo, Dp, fail );
    TIMING_END_AND_PRINT (alg_gcd_p, "time for alg gcd mod p: ")
    if( fail ) // mipo splits in char p
    {
      setCharacteristic(0);
      continue;
    }
    if( Dp.inCoeffDomain() ) // early termination
    {
      tryInvert(Dp,mipo,tmp,fail); // check if zero divisor
      setCharacteristic(0);
      if(fail)
        continue;
      setReduce(a,true);
      if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
      delete[] other;
      delete[] bound;
      return gcdcfcg;
    }
    setCharacteristic(0);
    // here: Dp NOT inCoeffDomain
    for(int i=1; i<=mv; i++)
      other[i] = 0; // reset (this is necessary, because some entries may not be updated by call to leadDeg)
    other = leadDeg(Dp,other);

    if(isEqual(bound, other, 1, mv)) // equal
    {
      chineseRemainder( D, q, mapinto(Dp), p, tmp, newq );
      // tmp = Dp mod p
      // tmp = D mod q
      // newq = p*q
      q = newq;
      if( D != tmp )
        D = tmp;
      On( SW_RATIONAL );
      TIMING_START (alg_reconstruction)
      tmp = Farey( D, q ); // Farey
      tmp *= bCommonDen (tmp);
      TIMING_END_AND_PRINT (alg_reconstruction,
                            "time for rational reconstruction in alg gcd: ")
      setReduce(a,true); // reduce expressions modulo mipo
      On( SW_RATIONAL ); // needed by fdivides
      if (test != tmp)
        test= tmp;
      else
        equal= true; // modular image did not add any new information
      TIMING_START (alg_termination)
#ifdef HAVE_NTL
#ifdef HAVE_FLINT
      if (equal && tmp.isUnivariate() && f.isUnivariate() && g.isUnivariate()
          && f.level() == tmp.level() && tmp.level() == g.level())
      {
        CanonicalForm Q, R;
        newtonDivrem (f, tmp, Q, R);
        if (R.isZero())
        {
          newtonDivrem (g, tmp, Q, R);
          if (R.isZero())
          {
            Off (SW_RATIONAL);
            setReduce (a,true);
            if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
            TIMING_END_AND_PRINT (alg_termination,
                                 "time for successful termination test in alg gcd: ")
            delete[] other;
            delete[] bound;
            return tmp*gcdcfcg;
          }
        }
      }
      else
#endif
#endif
      if(equal && fdivides( tmp, f ) && fdivides( tmp, g )) // trial division
      {
        Off( SW_RATIONAL );
        setReduce(a,true);
        if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
        TIMING_END_AND_PRINT (alg_termination,
                            "time for successful termination test in alg gcd: ")
        delete[] other;
        delete[] bound;
        return tmp*gcdcfcg;
      }
      TIMING_END_AND_PRINT (alg_termination,
                          "time for unsuccessful termination test in alg gcd: ")
      Off( SW_RATIONAL );
      setReduce(a,false); // do not reduce expressions modulo mipo
      continue;
    }
    if( isLess(bound, other, 1, mv) ) // current prime unlucky
      continue;
    // here: isLess(other, bound, 1, mv) ) ==> all previous primes unlucky
    q = p;
    D = mapinto(Dp); // shortcut CRA // shortcut CRA
    for(int i=1; i<=mv; i++) // tighten bound
      bound[i] = other[i];
  }
  // hopefully, we never reach this point
  setReduce(a,true);
  delete[] other;
  delete[] bound;
  Off( SW_USE_QGCD );
  D = gcdcfcg*gcd( f, g );
  On( SW_USE_QGCD );
  if (off_rational) Off(SW_RATIONAL); else On(SW_RATIONAL);
  return D;
}


int * leadDeg(const CanonicalForm & f, int *degs)
{ // leading degree vector w.r.t. lex. monomial order x(i+1) > x(i)
  // if f is in a coeff domain, the zero pointer is returned
  // 'a' should point to an array of sufficient size level(f)+1
  if(f.inCoeffDomain())
    return 0;
  CanonicalForm tmp = f;
  do
  {
    degs[tmp.level()] = tmp.degree();
    tmp = LC(tmp);
  }
  while(!tmp.inCoeffDomain());
  return degs;
}


bool isLess(int *a, int *b, int lower, int upper)
{ // compares the degree vectors a,b on the specified part. Note: x(i+1) > x(i)
  for(int i=upper; i>=lower; i--)
    if(a[i] == b[i])
      continue;
    else
      return a[i] < b[i];
  return true;
}


bool isEqual(int *a, int *b, int lower, int upper)
{ // compares the degree vectors a,b on the specified part. Note: x(i+1) > x(i)
  for(int i=lower; i<=upper; i++)
    if(a[i] != b[i])
      return false;
  return true;
}


CanonicalForm firstLC(const CanonicalForm & f)
{ // returns the leading coefficient (LC) of level <= 1
  CanonicalForm ret = f;
  while(ret.level() > 1)
    ret = LC(ret);
  return ret;
}

#ifndef HAVE_NTL
void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail )
{ // F, G are univariate polynomials (i.e. they have exactly one polynomial variable)
  // F and G must have the same level AND level > 0
  // we try to calculate gcd(F,G) = s*F + t*G
  // if a zero divisor is encountered, 'fail' is set to one
  // M is assumed to be monic
  CanonicalForm P;
  if(F.inCoeffDomain())
  {
    tryInvert( F, M, P, fail );
    if(fail)
      return;
    result = 1;
    s = P; t = 0;
    return;
  }
  if(G.inCoeffDomain())
  {
    tryInvert( G, M, P, fail );
    if(fail)
      return;
    result = 1;
    s = 0; t = P;
    return;
  }
  // here: both not inCoeffDomain
  CanonicalForm inv, rem, tmp, u, v, q, sum=0;
  if( F.degree() > G.degree() )
  {
    P = F; result = G;  s=v=0; t=u=1;
  }
  else
  {
    P = G; result = F; s=v=1; t=u=0;
  }
  Variable x = P.mvar();
  // here: degree(P) >= degree(result)
  while(true)
  {
    tryDivrem (P, result, q, rem, inv, M, fail);
    if(fail)
      return;
    if( rem.isZero() )
    {
      s*=inv;
      s= reduce (s, M);
      t*=inv;
      t= reduce (t, M);
      result *= inv; // monify result
      result= reduce (result, M);
      return;
    }
    sum += q;
    if(result.degree(x) >= rem.degree(x))
    {
      P=result;
      result=rem;
      tmp=u-sum*s;
      u=s;
      s=tmp;
      tmp=v-sum*t;
      v=t;
      t=tmp;
      sum = 0; // reset
    }
    else
      P = rem;
  }
}
#endif

static CanonicalForm trycontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail )
{ // as 'content', but takes care of zero divisors
  ASSERT( x.level() > 0, "cannot calculate content with respect to algebraic variable" );
  Variable y = f.mvar();
  if ( y == x )
    return trycf_content( f, 0, M, fail );
  if ( y < x )
     return f;
  return swapvar( trycontent( swapvar( f, y, x ), y, M, fail ), y, x );
}


static CanonicalForm tryvcontent ( const CanonicalForm & f, const Variable & x, const CanonicalForm & M, bool & fail )
{ // as vcontent, but takes care of zero divisors
  ASSERT( x.level() > 0, "cannot calculate vcontent with respect to algebraic variable" );
  if ( f.mvar() <= x )
    return trycontent( f, x, M, fail );
  CFIterator i;
  CanonicalForm d = 0, e, ret;
  for ( i = f; i.hasTerms() && ! d.isOne() && ! fail; i++ )
  {
    e = tryvcontent( i.coeff(), x, M, fail );
    if(fail)
      break;
    tryBrownGCD( d, e, M, ret, fail );
    d = ret;
  }
  return d;
}


static CanonicalForm trycf_content ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & M, bool & fail )
{ // as cf_content, but takes care of zero divisors
  if ( f.inPolyDomain() || ( f.inExtension() && ! getReduce( f.mvar() ) ) )
  {
    CFIterator i = f;
    CanonicalForm tmp = g, result;
    while ( i.hasTerms() && ! tmp.isOne() && ! fail )
    {
      tryBrownGCD( i.coeff(), tmp, M, result, fail );
      tmp = result;
      i++;
    }
    return result;
  }
  return abs( f );
}

