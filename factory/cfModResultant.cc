/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfModResultant.cc
 *
 * modular resultant algorithm
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"


#include "cf_assert.h"
#include "timing.h"

#include "cfModResultant.h"
#include "cf_primes.h"
#include "templates/ftmpl_functions.h"
#include "cf_map.h"
#include "cf_algorithm.h"
#include "cf_iter.h"
#include "cf_irred.h"
#include "cf_generator.h"
#include "cf_random.h"
#include "cf_map_ext.h"
#include "facFqBivarUtil.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#if defined(HAVE_NTL) || defined(HAVE_FLINT)
TIMING_DEFINE_PRINT(fac_resultant_p)

//TODO arrange by bound= deg (F,xlevel)*deg (G,i)+deg (G,xlevel)*deg (F, i)
static inline
void myCompress (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                  CFMap & N, const Variable& x)
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
  int degsfx, degsgx;
  int Flevel=F.level();
  int Glevel=G.level();

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

    M.newpair (Variable (xlevel), Variable (1));
    N.newpair (Variable (1), Variable (xlevel));
    degsfx= degsf [xlevel];
    degsgx= degsg [xlevel];
    degsf [xlevel]= 0;
    degsg [xlevel]= 0;
    if ((getNumVars (F) == 2 && getNumVars (G) == 1) ||
        (getNumVars (G) == 2 && getNumVars (F) == 1) ||
        (getNumVars (F) == 2 && getNumVars (F) == getNumVars (G)
         && getVars (F) == getVars (G)))
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
      DELETE_ARRAY(degsf);
      DELETE_ARRAY(degsg);
      return;
    }

    if (both_non_zero == 0)
    {
      DELETE_ARRAY(degsf);
      DELETE_ARRAY(degsg);
      return;
    }

    // map Variables which do not occur in both polynomials to higher levels
    int k= 1;
    int l= 1;
    for (int i= 1; i <= n; i++)
    {
      if (i == xlevel)
        continue;
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

    int m= n;
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
      while (min_max_deg == 0 && i < m)
      {
        i++;
        if (degsf [i] != 0 && degsg [i] != 0)
          min_max_deg= degsgx*degsf[i] + degsfx*degsg[i];
        else
          min_max_deg= 0;
      }
      for (int j= i + 1; j <= m; j++)
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
        else if (l < m + 1)
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
        else if (i < m + 1)
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

  DELETE_ARRAY(degsf);
  DELETE_ARRAY(degsg);
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
  ASSERT (getCharacteristic() > 0, "characteristic > 0 expected");
  if (F.inCoeffDomain() && G.inCoeffDomain())
    return 1;
  if (F.isZero() || G.isZero())
    return 0;
  Variable alpha;

#ifdef HAVE_FLINT
  if (!hasFirstAlgVar (F, alpha) && !hasFirstAlgVar (G,alpha))
  {
    nmod_poly_t FLINTF, FLINTG;
    convertFacCF2nmod_poly_t (FLINTF, F);
    convertFacCF2nmod_poly_t (FLINTG, G);
    mp_limb_t FLINTresult= nmod_poly_resultant (FLINTF, FLINTG);
    nmod_poly_clear (FLINTF);
    nmod_poly_clear (FLINTG);
    return CanonicalForm ((long) FLINTresult);
  }
  return resultant (F, G, F.mvar());
#elif defined(HAVE_NTL)
  if (!hasFirstAlgVar (F, alpha) && !hasFirstAlgVar (G,alpha))
  {
    if (fac_NTL_char != getCharacteristic())
    {
      fac_NTL_char= getCharacteristic();
      zz_p::init (getCharacteristic());
    }
    zz_pX NTLF= convertFacCF2NTLzzpX (F);
    zz_pX NTLG= convertFacCF2NTLzzpX (G);

    zz_p NTLResult= resultant (NTLF, NTLG);

    return CanonicalForm (to_long (rep (NTLResult)));
  }
  //at this point F or G has an algebraic var.
  if (fac_NTL_char != getCharacteristic())
  {
    fac_NTL_char= getCharacteristic();
    zz_p::init (getCharacteristic());
  }
  zz_pX NTLMipo= convertFacCF2NTLzzpX (getMipo (alpha));
  zz_pE::init (NTLMipo);
  zz_pEX NTLF= convertFacCF2NTLzz_pEX (F, NTLMipo);
  zz_pEX NTLG= convertFacCF2NTLzz_pEX (G, NTLMipo);
  zz_pE NTLResult= resultant (NTLF, NTLG);

  return convertNTLzzpE2CF (NTLResult, alpha);
#else
  return resultant (F, G, F.mvar());
#endif
}

static inline
void evalPoint (const CanonicalForm& F, const CanonicalForm& G,
                CanonicalForm& FEval, CanonicalForm& GEval,
                CFGenerator& evalPoint)
{
  int degF, degG;
  Variable x= Variable (1);
  degF= degree (F, x);
  degG= degree (G, x);
  do
  {
    if (!evalPoint.hasItems())
      break;
    FEval= F (evalPoint.item(), 2);
    GEval= G (evalPoint.item(), 2);
    if (degree (FEval, 1) < degF || degree (GEval, 1) < degG)
    {
      evalPoint.next();
      continue;
    }
    else
      return;
  }
  while (evalPoint.hasItems());
}

static inline CanonicalForm
newtonInterp (const CanonicalForm & alpha, const CanonicalForm & u,
              const CanonicalForm & newtonPoly, const CanonicalForm & oldInterPoly,
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

  if (A.isUnivariate() && B.isUnivariate() && A.level() == B.level())
    return uniResultant (A, B);

  CanonicalForm F= A;
  CanonicalForm G= B;

  CFMap M, N;
  myCompress (F, G, M, N, x);

  F= M (F);
  G= M (G);

  Variable y= Variable (2);

  CanonicalForm GEval, FEval, recResult, H;
  CanonicalForm newtonPoly= 1;
  CanonicalForm modResult= 0;

  Variable z= Variable (1);
  int bound= degAx*degree (G, 2) + degree (F, 2)*degBx;

  int p= getCharacteristic();
  CanonicalForm minpoly;
  Variable alpha= Variable (tmax (F.level(), G.level()) + 1);
  bool algExt= hasFirstAlgVar (F, alpha) || hasFirstAlgVar (G, alpha);
  CFGenerator * gen;
  bool extOfExt= false;
  Variable v= alpha;
  CanonicalForm primElemAlpha, imPrimElemAlpha;
  CFList source,dest;
  if (!algExt && (p < (1 << 28)))
  {
    // pass to an extension of size at least 2^29
    // for very very large input that is maybe too small though
    int deg= ceil (29.0*((double) log (2)/log (p)))+1;
    minpoly= randomIrredpoly (deg, z);
    alpha= rootOf (minpoly);
    AlgExtGenerator AlgExtGen (alpha);
    gen= AlgExtGen.clone();
    for (int i= 0; i < p; i++) // skip values from the prime field
      (*gen).next();
  }
  else if (!algExt)
  {
    FFGenerator FFGen;
    gen= FFGen.clone();
  }
  else
  {
    int deg= ceil (29.0*((double) log (2)/log (p)));
    if (degree (getMipo (alpha)) < deg)
    {
      mpz_t field_size;
      mpz_init (field_size);
      mpz_ui_pow_ui (field_size, p,
                 deg + degree (getMipo (alpha)) - deg%degree (getMipo (alpha)));

      // field_size needs to fit in an int because of mapUp, mapDown, length of lists etc.
      if (mpz_fits_sint_p (field_size))
      {
        minpoly= randomIrredpoly (deg + degree (getMipo (alpha))
                                  - deg%degree (getMipo (alpha)), z);
        v= rootOf (minpoly);
        Variable V_buf2;
        bool primFail= false;
        extOfExt= true;
        primElemAlpha= primitiveElement (alpha, V_buf2, primFail);
        ASSERT (!primFail, "failure in integer factorizer");
        if (primFail)
          ; //ERROR
        else
          imPrimElemAlpha= mapPrimElem (primElemAlpha, alpha, v);
        F= mapUp (F, alpha, v, primElemAlpha, imPrimElemAlpha, source, dest);
        G= mapUp (G, alpha, v, primElemAlpha, imPrimElemAlpha, source, dest);
      }
      else
      {
        deg= deg - deg % degree (getMipo (alpha));
        mpz_ui_pow_ui (field_size, p, deg);
        while (deg / degree (getMipo (alpha)) >= 2 && !mpz_fits_sint_p (field_size))
        {
          deg -= degree (getMipo (alpha));
          mpz_ui_pow_ui (field_size, p, deg);
        }
        if (deg != degree (getMipo (alpha)))
        {
           minpoly= randomIrredpoly (deg, z);
           v= rootOf (minpoly);
           Variable V_buf2;
           bool primFail= false;
           extOfExt= true;
           primElemAlpha= primitiveElement (alpha, V_buf2, primFail);
           ASSERT (!primFail, "failure in integer factorizer");
           if (primFail)
             ; //ERROR
           else
             imPrimElemAlpha= mapPrimElem (primElemAlpha, alpha, v);
           F= mapUp (F, alpha, v, primElemAlpha, imPrimElemAlpha, source, dest);
           G= mapUp (G, alpha, v, primElemAlpha, imPrimElemAlpha, source, dest);
        }
      }
      mpz_clear (field_size);
    }
    AlgExtGenerator AlgExtGen (v);
    gen= AlgExtGen.clone();
    for (int i= 0; i < p; i++)
      (*gen).next();
  }
  int count= 0;
  int equalCount= 0;
  CanonicalForm point;
  do
  {
    evalPoint (F, G, FEval, GEval, *gen);

    recResult= resultantFp (FEval, GEval, z, prob);

    H= newtonInterp ((*gen).item(), recResult, newtonPoly, modResult, y);

    if (H == modResult)
      equalCount++;
    else
      equalCount= 0;

    count++;
    if (count > bound || (prob && equalCount == 2 && !H.inCoeffDomain()))
    {
      if (!algExt && degree (H, alpha) <= 0)
        break;
      else if (algExt)
      {
        if (extOfExt && !isInExtension (H, imPrimElemAlpha, 1, primElemAlpha,
                                        dest, source))
        {
          H= mapDown (H, primElemAlpha, imPrimElemAlpha, alpha, dest, source);
          prune (v);
          break;
        }
        else if (!extOfExt)
          break;
      }
    }

    modResult= H;
    newtonPoly *= (y - (*gen).item());
    if ((*gen).hasItems())
        (*gen).next();
    else
      STICKYASSERT (0, "out of evaluation points");
  } while (1);

  delete gen;

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
#ifdef HAVE_NTL // mapPrimitiveElem
CanonicalForm
resultantZ (const CanonicalForm& A, const CanonicalForm& B, const Variable& x,
            bool prob)
{
  ASSERT (getCharacteristic() == 0, "characteristic > 0 expected");
#ifndef NOASSERT
  bool isRat= isOn (SW_RATIONAL);
  On (SW_RATIONAL);
  ASSERT (bCommonDen (A).isOne(), "input A is rational");
  ASSERT (bCommonDen (B).isOne(), "input B is rational");
  if (!isRat)
    Off (SW_RATIONAL);
#endif

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
  int equalCount= 0;
  CanonicalForm test, newTest;
  int count= 0;
  do
  {
    p = cf_getBigPrime( i );
    i--;
    while ( i >= 0 && mod( l, p ) == 0)
    {
      p = cf_getBigPrime( i );
      i--;
    }

    if (i <= 0)
      return resultant (A, B, x);

    setCharacteristic (p);

    TIMING_START (fac_resultant_p);
    resultModP= resultantFp (mapinto (F), mapinto (G), X, prob);
    TIMING_END_AND_PRINT (fac_resultant_p, "time to compute resultant mod p: ");

    setCharacteristic (0);

    count++;
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
      test= symmetricRemainder (result,q);
      if (test != newTest)
      {
        newTest= test;
        equalCount= 0;
      }
      else
        equalCount++;
      if (newQ > bound || (prob && equalCount == 2))
      {
        result= test;
        break;
      }
    }
  } while (1);

  if (onRational)
    On (SW_RATIONAL);
  return swapvar (result, X, x);
}
#endif
#endif

