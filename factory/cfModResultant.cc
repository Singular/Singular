/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfModResultant.cc
 *
 * modular resultant algorithm
 *
 * @author Martin Lee
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/


#include "assert.h"

#include "cfModResultant.h"
#include "cf_primes.h"
#include "templates/ftmpl_functions.h"
#include "cf_map.h"
#include "cf_algorithm.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

//TODO arrange by bound= deg (F,xlevel)*deg (G,i)+deg (G,xlevel)*deg (F, i)
static inline
void myCompress (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                  CFMap & N, const Variable& x)
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
  int degsfx, degsgx;

  if (x.level() != 1)
  {
    int xlevel= x.level();

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

    M.newpair (Variable (xlevel), Variable (1));
    N.newpair (Variable (1), Variable (xlevel));
    degsfx= degsf [xlevel];
    degsgx= degsg [xlevel];
    degsf [xlevel]= 0;
    degsg [xlevel]= 0;
    if (getNumVars (F) == 2 || getNumVars (G) == 2)
    {
      int pos= 2;
      for (int i= 1; i <= n; i++)
      {
        if (i != xlevel)
        {
          if (i != pos && (degsf[i] != 0 || degsg [i] != 0))
          {
            M.newpair (Variable (i), Variable (pos));
            N.newpair (Variable (pos), Variable (i));
            pos++;
          }
        }
      }
      delete [] degsf;
      delete [] degsg;
      return;
    }

    if (both_non_zero == 0)
    {
      delete [] degsf;
      delete [] degsg;
      return;
    }

    // map Variables which do not occur in both polynomials to higher levels
    int k= 1;
    int l= 1;
    for (int i= 1; i <= n; i++)
    {
      if (i == xlevel)
        continue;
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

    int m= tmax (F.level(), G.level());
    int min_max_deg;
    k= both_non_zero;
    l= 0;
    int i= 1;
    while (k > 0)
    {
      if (degsf [i] != 0 && degsg [i] != 0)
        min_max_deg= degsgx*degsf[i] + degsfx*degsg[i];
      else
        min_max_deg= 0;
      while (min_max_deg == 0)
      {
        i++;
        if (degsf [i] != 0 && degsg [i] != 0)
          min_max_deg= degsgx*degsf[i] + degsfx*degsg[i];
        else
          min_max_deg= 0;
      }
      for (int j= i + 1; j <=  m; j++)
      {
        if (degsgx*degsf[j] + degsfx*degsg[j] <= min_max_deg &&
            degsf[j] != 0 && degsg [j] != 0)
        {
          min_max_deg= degsgx*degsf[j] + degsfx*degsg[j];
          l= j;
        }
      }
      if (l != 0)
      {
        if (l != k && l != xlevel && k != 1)
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
        if (i != k && i != xlevel && k != 1)
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
        if (both_zero != 0 && i != 1)
        {
          M.newpair (Variable (i), Variable (i - both_zero));
          N.newpair (Variable (i - both_zero), Variable (i));
        }
      }
    }
  }

  delete [] degsf;
  delete [] degsg;
}

static inline
CanonicalForm oneNorm (const CanonicalForm& F)
{
  if (F.inZ())
    return abs (F);

  CanonicalForm result= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
    result += oneNorm (i.coeff());
  return result;
}

// if F and G are both non constant, make sure their level is equal
static inline
CanonicalForm uniResultant (const CanonicalForm& F, const CanonicalForm& G)
{
#ifdef HAVE_NTL
  ASSERT (getCharacteristic() > 0, "characteristic > 0 expected");
  if (F.inCoeffDomain() && G.inCoeffDomain())
    return 1;
  if (F.isZero() || G.isZero())
    return 0;

  zz_p::init (getCharacteristic());
  zz_pX NTLF= convertFacCF2NTLzzpX (F);
  zz_pX NTLG= convertFacCF2NTLzzpX (G);

  zz_p NTLResult= resultant (NTLF, NTLG);

  return CanonicalForm (to_long (rep (NTLResult)));
#else
  return resultant (F, G, F.mvar());
#endif
}

static inline
void evalPoint (const CanonicalForm& F, const CanonicalForm& G,
                CanonicalForm& FEval, CanonicalForm& GEval, int& evalPoint)
{
  int degF, degG;
  Variable x= Variable (1);
  degF= degree (F, x);
  degG= degree (G, x);
  do
  {
    evalPoint++;
    if (evalPoint >= getCharacteristic())
      break;
    FEval= F (evalPoint, 2);
    GEval= G (evalPoint, 2);
    if (degree (FEval, 1) < degF || degree (GEval, 1) < degG)
      continue;
    else
      return;
  }
  while (evalPoint < getCharacteristic());
}

static inline CanonicalForm
newtonInterp (const CanonicalForm alpha, const CanonicalForm u,
              const CanonicalForm newtonPoly, const CanonicalForm oldInterPoly,
              const Variable & x)
{
  CanonicalForm interPoly;

  interPoly= oldInterPoly+((u - oldInterPoly (alpha, x))/newtonPoly (alpha, x))
                            *newtonPoly;
  return interPoly;
}

CanonicalForm
resultantFp (const CanonicalForm& A, const CanonicalForm& B, const Variable& x,
             bool prob)
{
  ASSERT (getCharacteristic() > 0, "characteristic > 0 expected");

  if (A.isZero() || B.isZero())
    return 0;

  int degAx= degree (A, x);
  int degBx= degree (B, x);
  if (A.level() < x.level())
    return power (A, degBx);
  if (B.level() < x.level())
    return power (B, degAx);

  if (degAx == 0)
    return power (A, degBx);
  else if (degBx == 0)
    return power (B, degAx);

  CanonicalForm F= A;
  CanonicalForm G= B;

  CFMap M, N;
  myCompress (F, G, M, N, x);

  F= M (F);
  G= M (G);

  Variable y= Variable (2);

  if (F.isUnivariate() && G.isUnivariate() && F.level() == G.level())
    return N(uniResultant (F, G));

  int i= -1;
  CanonicalForm GEval, FEval, recResult, H;
  CanonicalForm newtonPoly= 1;
  CanonicalForm modResult= 0;

  Variable z= Variable (1);
  int bound= degAx*degree (G, 2) + degree (F, 2)*degBx;

  int count= 0;
  int equalCount= 0;
  do
  {
    evalPoint (F, G, FEval, GEval, i);

    ASSERT (i < getCharacteristic(), "ran out of points");

    recResult= resultantFp (FEval, GEval, z);

    H= newtonInterp (i, recResult, newtonPoly, modResult, y);

    if (H == modResult)
      equalCount++;

    count++;
    if (count > bound || (prob && equalCount == 2))
      break;

    modResult= H;
    newtonPoly *= (y - i);
  } while (1);

  return N (H);
}

static inline
CanonicalForm
balanceUni ( const CanonicalForm & f, const CanonicalForm & q )
{
    Variable x = f.mvar();
    CanonicalForm result = 0, qh = q / 2;
    CanonicalForm c;
    CFIterator i;
    for ( i = f; i.hasTerms(); i++ ) {
        c = mod( i.coeff(), q );
        if ( c > qh )
            result += power( x, i.exp() ) * (c - q);
        else
            result += power( x, i.exp() ) * c;
    }
    return result;
}

static inline
CanonicalForm
symmetricRemainder (const CanonicalForm& f, const CanonicalForm& q)
{
  CanonicalForm result= 0;
  if (f.isUnivariate() || f.inCoeffDomain())
    return balanceUni (f, q);
  else
  {
    Variable x= f.mvar();
    for (CFIterator i= f; i.hasTerms(); i++)
      result += power (x, i.exp())*symmetricRemainder (i.coeff(), q);
  }
  return result;
}

CanonicalForm
resultantZ (const CanonicalForm& A, const CanonicalForm& B, const Variable& x,
            bool prob)
{
  ASSERT (getCharacteristic() == 0, "characteristic > 0 expected");

  int degAx= degree (A, x);
  int degBx= degree (B, x);
  if (A.level() < x.level())
    return power (A, degBx);
  if (B.level() < x.level())
    return power (B, degAx);

  if (degAx == 0)
    return power (A, degBx);
  else if (degBx == 0)
    return power (B, degAx);

  CanonicalForm F= A;
  CanonicalForm G= B;

  Variable X= x;
  if (F.level() != x.level() || G.level() != x.level())
  {
    if (F.level() > G.level())
      X= F.mvar();
    else
      X= G.mvar();
    F= swapvar (F, X, x);
    G= swapvar (G, X, x);
  }

  // now X is the main variable

  CanonicalForm d= 0;
  CanonicalForm dd= 0;
  CanonicalForm buf;
  for (CFIterator i= F; i.hasTerms(); i++)
  {
    buf= oneNorm (i.coeff());
    d= (buf > d) ? buf : d;
  }
  CanonicalForm e= 0, ee= 0;
  for (CFIterator i= G; i.hasTerms(); i++)
  {
    buf= oneNorm (i.coeff());
    e= (buf > e) ? buf : e;
  }
  d= power (d, degBx);
  e= power (e, degAx);
  CanonicalForm bound= 1;
  for (int i= degBx + degAx; i > 1; i--)
    bound *= i;
  bound *= d*e;
  bound *= 2;

  bool onRational= isOn (SW_RATIONAL);
  if (onRational)
    Off (SW_RATIONAL);
  int i = cf_getNumBigPrimes() - 1;
  int p;
  CanonicalForm l= lc (F)*lc(G);
  CanonicalForm resultModP, q (0), newResult, newQ;
  CanonicalForm result;
  do
  {
    p = cf_getBigPrime( i );
    i--;
    while ( i >= 0 && mod( l, p ) == 0)
    {
      p = cf_getBigPrime( i );
      i--;
    }

    ASSERT (i >= 0, "ran out of primes"); //sic

    setCharacteristic (p);

    resultModP= resultantFp (mapinto (F), mapinto (G), X, prob);

    setCharacteristic (0);

    if ( q.isZero() )
    {
      result= mapinto(resultModP);
      q= p;
    }
    else
    {
      chineseRemainder( result, q, mapinto (resultModP), p, newResult, newQ );
      q= newQ;
      result= newResult;
      if (newQ > bound)
      {
        result= symmetricRemainder (result, q);
        break;
      }
    }
  } while (1);

  if (onRational)
    On (SW_RATIONAL);
  return swapvar (result, X, x);
}

