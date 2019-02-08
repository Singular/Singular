/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfEzgcd.cc
 *
 * This file implements the GCD of two multivariate polynomials over Q or F_q
 * using EZ-GCD as described in "Algorithms for Computer Algebra" by Geddes,
 * Czapor, Labahnn
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#include "config.h"

#include "timing.h"
#include "cf_assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cfEzgcd.h"
#include "cfModGcd.h"
#include "cf_util.h"
#include "cf_map_ext.h"
#include "cf_algorithm.h"
#include "cf_reval.h"
#include "cf_random.h"
#include "cf_primes.h"
#include "templates/ftmpl_functions.h"
#include "cf_map.h"
#include "facHensel.h"

#ifdef HAVE_NTL
#include "NTLconvert.h"

static const double log2exp= 1.442695041;

TIMING_DEFINE_PRINT(ez_eval)
TIMING_DEFINE_PRINT(ez_compress)
TIMING_DEFINE_PRINT(ez_hensel_lift)
TIMING_DEFINE_PRINT(ez_content)
TIMING_DEFINE_PRINT(ez_termination)

static
int compress4EZGCD (const CanonicalForm& F, const CanonicalForm& G, CFMap & M,
                    CFMap & N, int& both_non_zero)
{
  int n= tmax (F.level(), G.level());
  int * degsf= NEW_ARRAY(int,n + 1);
  int * degsg= NEW_ARRAY(int,n + 1);

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
    DELETE_ARRAY(degsf);
    DELETE_ARRAY(degsg);
    return 0;
  }

  // map Variables which do not occur in both polynomials to higher levels
  int k= 1;
  int l= 1;
  int Flevel=F.level();
  int Glevel=G.level();
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

  // sort Variables x_{i} in decreasing order of
  // min(deg_{x_{i}}(f),deg_{x_{i}}(g))
  //int m= tmin (F.level(), G.level());
  int m= tmin (Flevel, Glevel);
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

  DELETE_ARRAY(degsf);
  DELETE_ARRAY(degsg);

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
  int Flevel=F.level();
  for (int i= 2; i < l + 1; i++, j++)
  {
    if (Flevel < i)
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
  int * degsf= NEW_ARRAY(int,n + 1);

  for (int i = n; i >= 0; i--)
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
    max_deg= degsf [i]; // i is always 2 here, n>=2
    while ((i<n) &&(max_deg == 0))
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

  DELETE_ARRAY(degsf);

  return result;
}

static inline
int Hensel (const CanonicalForm & UU, CFArray & G, const Evaluation & AA,
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
    if (!A[i].isZero() &&
        ((getCharacteristic() > degree (U,i)) || getCharacteristic() == 0))
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
    liftBounds= NEW_ARRAY(int,U.level() - 1); /* index: 0.. U.level()-2 */
    liftBounds[0]= liftBound;
    for (int i= 1; i < U.level() - 1; i++)
      liftBounds[i]= degree (shiftedU, Variable (i + 2)) + 1;
    factors= nonMonicHenselLift2 (UEval, factors, liftBounds, U.level() - 1,
                                  false, shiftedLCsEval1, shiftedLCsEval2, Pi,
                                  diophant, noOneToOne);
    DELETE_ARRAY(liftBounds);
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

static
bool findeval (const CanonicalForm & F, const CanonicalForm & G,
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
        {
          k++;
          return true;
        }
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
static void gcd_mon_rec(CanonicalForm G, CanonicalForm &cf,int *exp,int pl)
{  // prevoius level: pl
  if (G.inCoeffDomain())
  {
    for(int i=pl-1;i>0;i--) exp[i]=0;
    cf=gcd(cf,G);
    return;
  }
  int l=G.level();
  for(int i=pl-1;i>l;i--) exp[i]=0;
  for(CFIterator i=G; i.hasTerms(); i++)
  {
    if (i.exp()<exp[l]) exp[l]=i.exp();
    gcd_mon_rec(i.coeff(),cf,exp,l);
  }
}

static CanonicalForm gcd_mon(CanonicalForm F, CanonicalForm G)
{
  // assume: size(F)==1
  CanonicalForm cf=F;
  int ll=tmax(F.level(),G.level());
  int *exp=NEW_ARRAY(int,ll+1);
  for(int i=ll;i>=0;i--) exp[i]=0;
  CanonicalForm c=F;
  while(!c.inCoeffDomain())
  {
    exp[c.level()]=c.degree();
    c=c.LC();
    cf=c;
  }
  gcd_mon_rec(G,cf,exp,G.level()+1);
  CanonicalForm res=cf;
  for(int i=0;i<=ll;i++)
  {
    if (exp[i]>0) res*=power(Variable(i),exp[i]);
  }
  DELETE_ARRAY(exp);
  return res;
}

/// real implementation of EZGCD over Z
static CanonicalForm
ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG, REvaluation & b,
        bool internal )
{
  bool isRat= isOn (SW_RATIONAL);

  int maxNumVars= tmax (getNumVars (FF), getNumVars (GG));
  int sizeF= size (FF);
  int sizeG= size (GG);


  if (sizeF==1)
  {
    Off(SW_USE_EZGCD);
    CanonicalForm result=gcd_mon( FF, GG );
    On(SW_USE_EZGCD);
    return result;
  }
  else if (sizeG==1)
  {
    Off(SW_USE_EZGCD);
    CanonicalForm result=gcd_mon( GG, FF );
    On(SW_USE_EZGCD);
    return result;
  }
  if (!isRat)
    On (SW_RATIONAL);
  if (sizeF/maxNumVars > 500 && sizeG/maxNumVars > 500)
  {
    Off(SW_USE_EZGCD);
    CanonicalForm result=gcd( FF, GG );
    On(SW_USE_EZGCD);
    if (!isRat)
      Off (SW_RATIONAL);
    result /= icontent (result);
    DEBDECLEVEL( cerr, "ezgcd" );
    return result;
  }


  CanonicalForm F, G, f, g, d, Fb, Gb, Db, Fbt, Gbt, Dbt, B0, B, D0, lcF, lcG,
                lcD, cand, contcand, result;
  CFArray DD( 1, 2 ), lcDD( 1, 2 );
  int degF, degG, delta, t, count, maxeval;
  REvaluation bt;
  int gcdfound = 0;
  Variable x = Variable(1);
  count= 0;
  maxeval= 200;
  int o, l;
  o= 0;
  l= 1;

  if (!isRat)
    On (SW_RATIONAL);
  F= FF*bCommonDen (FF);
  G= GG*bCommonDen (GG);
  if (!isRat)
    Off (SW_RATIONAL);

  TIMING_START (ez_compress)
  CFMap M,N;
  int smallestDegLev;
  int best_level= compress4EZGCD (F, G, M, N, smallestDegLev);

  if (best_level == 0)
  {
    DEBDECLEVEL( cerr, "ezgcd" );
    return G.genOne();
  }

  F= M (F);
  G= M (G);
  TIMING_END_AND_PRINT (ez_compress, "time for compression in EZ: ")

  DEBINCLEVEL( cerr, "ezgcd" );
  DEBOUTLN( cerr, "FF = " << FF );
  DEBOUTLN( cerr, "GG = " << GG );
  TIMING_START (ez_content)
  f = content( F, x ); g = content( G, x ); d = gcd( f, g );
  DEBOUTLN( cerr, "f = " << f );
  DEBOUTLN( cerr, "g = " << g );
  F /= f; G /= g;
  TIMING_END_AND_PRINT (ez_content, "time to extract content in EZ: ")
  if ( F.isUnivariate() )
  {
    if ( G.isUnivariate() )
    {
      DEBDECLEVEL( cerr, "ezgcd" );
      if(F.mvar()==G.mvar())
        d*=gcd(F,G);
      else
        return N (d);
      return N (d);
    }
    else
    {
      g= content (G,G.mvar());
      return N(d*gcd(F,g));
    }
  }
  if ( G.isUnivariate())
  {
    f= content (F,F.mvar());
    return N(d*gcd(G,f));
  }

  maxNumVars= tmax (getNumVars (F), getNumVars (G));
  sizeF= size (F);
  sizeG= size (G);

  if (!isRat)
    On (SW_RATIONAL);
  if (sizeF/maxNumVars > 500 && sizeG/maxNumVars > 500)
  {
    Off(SW_USE_EZGCD);
    result=gcd( F, G );
    On(SW_USE_EZGCD);
    if (!isRat)
      Off (SW_RATIONAL);
    result /= icontent (result);
    DEBDECLEVEL( cerr, "ezgcd" );
    return N (d*result);
  }

  int dummy= 0;
  if ( gcd_test_one( F, G, false, dummy ) )
  {
    DEBDECLEVEL( cerr, "ezgcd" );
    if (!isRat)
      Off (SW_RATIONAL);
    return N (d);
  }
  lcF = LC( F, x ); lcG = LC( G, x );
  lcD = gcd( lcF, lcG );
  delta = 0;
  degF = degree( F, x ); degG = degree( G, x );
  t = tmax( F.level(), G.level() );
  if ( ! internal )
    b = REvaluation( 2, t, IntRandom( 25 ) );
  while ( ! gcdfound )
  {
    /// ---> A2
    DEBOUTLN( cerr, "search for evaluation, delta = " << delta );
    DEBOUTLN( cerr, "F = " << F );
    DEBOUTLN( cerr, "G = " << G );
    TIMING_START (ez_eval)
    if (!findeval( F, G, Fb, Gb, Db, b, delta, degF, degG, maxeval, count,
                   o, 25, l))
    {
      Off(SW_USE_EZGCD);
      result=gcd( F, G );
      On(SW_USE_EZGCD);
      if (!isRat)
        Off (SW_RATIONAL);
      DEBDECLEVEL( cerr, "ezgcd" );
      result /= icontent (result);
      return N (d*result);
    }
    TIMING_END_AND_PRINT (ez_eval, "time to find eval point in EZ1: ")
    DEBOUTLN( cerr, "found evaluation b = " << b );
    DEBOUTLN( cerr, "F(b) = " << Fb );
    DEBOUTLN( cerr, "G(b) = " << Gb );
    DEBOUTLN( cerr, "D(b) = " << Db );
    delta = degree( Db );
    /// ---> A3
    if (delta == degF)
    {
      if (degF <= degG  && fdivides (F, G))
      {
        DEBDECLEVEL( cerr, "ezgcd" );
        if (!isRat)
          Off (SW_RATIONAL);
        return N (d*F);
      }
      else
        delta--;
    }
    else if (delta == degG)
    {
      if (degG <= degF && fdivides( G, F ))
      {
        DEBDECLEVEL( cerr, "ezgcd" );
        if (!isRat)
          Off (SW_RATIONAL);
        return N (d*G);
      }
      else
        delta--;
    }
    if ( delta == 0 )
    {
      DEBDECLEVEL( cerr, "ezgcd" );
      if (!isRat)
        Off (SW_RATIONAL);
      return N (d);
    }
    /// ---> A4
    //deltaold = delta;
    while ( 1 )
    {
      bt = b;
      TIMING_START (ez_eval)
      if (!findeval( F, G, Fbt, Gbt, Dbt, bt, delta, degF, degG, maxeval, count,
                     o, 25,l ))
      {
        Off(SW_USE_EZGCD);
        result=gcd( F, G );
        On(SW_USE_EZGCD);
        if (!isRat)
          Off (SW_RATIONAL);
        DEBDECLEVEL( cerr, "ezgcd" );
        result /= icontent (result);
        return N (d*result);
      }
      TIMING_END_AND_PRINT (ez_eval, "time to find eval point in EZ2: ")
      int dd=degree( Dbt );
      if ( dd /*degree( Dbt )*/ == 0 )
      {
        DEBDECLEVEL( cerr, "ezgcd" );
        if (!isRat)
          Off (SW_RATIONAL);
        return N (d);
      }
      if ( dd /*degree( Dbt )*/ == delta )
        break;
      else  if ( dd /*degree( Dbt )*/ < delta )
      {
        delta = dd /*degree( Dbt )*/;
        b = bt;
        Db = Dbt; Fb = Fbt; Gb = Gbt;
      }
      DEBOUTLN( cerr, "now after A4, delta = " << delta );
      /// ---> A5
      if (delta == degF)
      {
        if (degF <= degG  && fdivides (F, G))
        {
          DEBDECLEVEL( cerr, "ezgcd" );
          if (!isRat)
            Off (SW_RATIONAL);
          return N (d*F);
        }
        else
          delta--;
      }
      else if (delta == degG)
      {
        if (degG <= degF && fdivides( G, F ))
        {
          DEBDECLEVEL( cerr, "ezgcd" );
          if (!isRat)
            Off (SW_RATIONAL);
          return N (d*G);
        }
        else
          delta--;
      }
      if ( delta == 0 )
      {
        DEBDECLEVEL( cerr, "ezgcd" );
        if (!isRat)
          Off (SW_RATIONAL);
        return N (d);
      }
    }
    if ( delta != degF && delta != degG )
    {
      /// ---> A6
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
      else
      {
        //special case
        Off(SW_USE_EZGCD);
        result=gcd( F, G );
        On(SW_USE_EZGCD);
        if (!isRat)
          Off (SW_RATIONAL);
        DEBDECLEVEL( cerr, "ezgcd" );
        result /= icontent (result);
        return N (d*result);
      }
      /// ---> A7
      DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
      DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );
      DEBOUTLN( cerr, "(hensel) B    = " << B );
      DEBOUTLN( cerr, "(hensel) lcB  = " << LC( B, Variable(1) ) );
      DEBOUTLN( cerr, "(hensel) b(B) = " << b(B) );
      DEBOUTLN( cerr, "(hensel) DD   = " << DD );
      DEBOUTLN( cerr, "(hensel) lcDD = " << lcDD );
      TIMING_START (ez_hensel_lift)
      gcdfound= Hensel (B*lcD, DD, b, lcDD);
      TIMING_END_AND_PRINT (ez_hensel_lift, "time to hensel lift in EZ: ")
      DEBOUTLN( cerr, "(hensel finished) DD   = " << DD );

      if (gcdfound == -1)
      {
        Off (SW_USE_EZGCD);
        result= gcd (F,G);
        On (SW_USE_EZGCD);
        if (!isRat)
          Off (SW_RATIONAL);
        DEBDECLEVEL( cerr, "ezgcd" );
        result /= icontent (result);
        return N (d*result);
      }

      if (gcdfound)
      {
        TIMING_START (ez_termination)
        contcand= content (DD[2], Variable (1));
        cand = DD[2] / contcand;
        if (B_is_F)
          gcdfound = fdivides( cand, G ) && cand*(DD[1]/(lcD/contcand)) == F;
        else
          gcdfound = fdivides( cand, F ) && cand*(DD[1]/(lcD/contcand)) == G;
        TIMING_END_AND_PRINT (ez_termination,
                              "time for termination test in EZ: ")
      }
      /// ---> A8 (gcdfound)
    }
    delta--;
  }
  /// ---> A9
  DEBDECLEVEL( cerr, "ezgcd" );
  cand *= bCommonDen (cand);
  if (!isRat)
    Off (SW_RATIONAL);
  cand /= icontent (cand);
  return N (d*cand);
}
#endif

/// Extended Zassenhaus GCD over Z.
/// In case things become too dense we switch to a modular algorithm.
CanonicalForm
ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG )
{
#ifdef HAVE_NTL
  REvaluation b;
  return ezgcd( FF, GG, b, false );
#else
  Off (SW_USE_EZGCD);
  return gcd (FF, GG);
  On (SW_USE_EZGCD);
#endif
}

#ifdef HAVE_NTL
// parameters for heuristic
STATIC_VAR int maxNumEval= 200;
STATIC_VAR int sizePerVars1= 500; //try dense gcd if size/#variables is bigger

/// Extended Zassenhaus GCD for finite fields.
/// In case things become too dense we switch to a modular algorithm.
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

  if (sizeF==1)
  {
    return gcd_mon( FF, GG );
  }
  else if (sizeG==1)
  {
    return gcd_mon( GG, FF );
  }

  if (sizeF/maxNumVars > sizePerVars1 && sizeG/maxNumVars > sizePerVars1)
  {
    if (hasFirstAlgVar (FF, a) || hasFirstAlgVar (GG, a))
      return modGCDFq (FF, GG, a);
    else if (CFFactory::gettype() == GaloisFieldDomain)
      return modGCDGF (FF, GG);
    else
      return modGCDFp (FF, GG);
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
  TIMING_DEFINE(ez_p_compress);
  TIMING_START (ez_p_compress);
  int best_level= compress4EZGCD (F, G, M, N, smallestDegLev);

  if (best_level == 0) return G.genOne();

  F= M (F);
  G= M (G);
  TIMING_END_AND_PRINT (ez_p_compress, "time for compression in EZ_P: ")

  TIMING_DEFINE (ez_p_content)
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
      return N (d*modGCDFq (F, G, a));
    else if (CFFactory::gettype() == GaloisFieldDomain)
      return N (d*modGCDGF (F, G));
    else
      return N (d*modGCDFp (F, G));
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

  if (algExtension)
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
  TIMING_DEFINE(ez_p_eval);
  while( !gcdfound )
  {
    TIMING_START (ez_p_eval);
    if( !findeval( F, G, Fb, Gb, Db, b, delta, degF, degG, maxeval, count, o,
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
        prune (alpha);
      }
      if (k > 1)
      {
        result= GFMapDown (result, k);
        setCharacteristic (p, k, gf_name);
      }
      if (extOfExt)
      {
        result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
        prune1 (oldA);
      }
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
          prune (alpha);
        }
        if (k > 1)
        {
          F= GFMapDown (F, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
        {
          F= mapDown (F, primElem, imPrimElem, oldA, dest, source);
          prune1 (oldA);
        }
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
          prune (alpha);
        }
        if (k > 1)
        {
          G= GFMapDown (G, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
        {
          G= mapDown (G, primElem, imPrimElem, oldA, dest, source);
          prune1 (oldA);
        }
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
      if( !findeval(F,G,Fbt,Gbt,Dbt, bt, delta, degF, degG, maxeval, count, o,
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
          prune (alpha);
        }
        if (k > 1)
        {
          result= GFMapDown (result, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
        {
          result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
          prune1 (oldA);
        }
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
            prune (alpha);
          }
          if (k > 1)
          {
            F= GFMapDown (F, k);
            setCharacteristic (p, k, gf_name);
          }
          if (extOfExt)
          {
            F= mapDown (F, primElem, imPrimElem, oldA, dest, source);
            prune1 (oldA);
          }
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
            prune (alpha);
          }
          if (k > 1)
          {
            G= GFMapDown (G, k);
            setCharacteristic (p, k, gf_name);
          }
          if (extOfExt)
          {
            G= mapDown (G, primElem, imPrimElem, oldA, dest, source);
            prune1 (oldA);
          }
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
          prune (alpha);
        }
        if (k > 1)
        {
          result= GFMapDown (result, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt)
        {
          result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
          prune1 (oldA);
        }
        return N (d*result);
      }
      DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
      DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );

      if (size (B*lcDD[2])/maxNumVars > sizePerVars1)
      {
        if (algExtension)
        {
          result= modGCDFq (F, G, a);
          if (extOfExt)
          {
            result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
            prune1 (oldA);
          }
          return N (d*result);
        }
        if (CFFactory::gettype() == GaloisFieldDomain)
        {
          result= modGCDGF (F, G);
          if (passToGF)
          {
            CanonicalForm mipo= gf_mipo;
            setCharacteristic (p);
            Variable alpha= rootOf (mipo.mapinto());
            result= GF2FalphaRep (result, alpha);
            prune (alpha);
          }
          if (k > 1)
          {
            result= GFMapDown (result, k);
            setCharacteristic (p, k, gf_name);
          }
          return N (d*result);
        }
        else
          return N (d*modGCDFp (F,G));
      }

      TIMING_DEFINE(ez_p_hensel_lift);
      TIMING_START (ez_p_hensel_lift);
      gcdfound= Hensel (B*lcD, DD, b, lcDD);
      TIMING_END_AND_PRINT (ez_p_hensel_lift, "time for Hensel lift in EZ_P: ");

      if (gcdfound == -1) //things became dense
      {
        if (algExtension)
        {
          result= modGCDFq (F, G, a);
          if (extOfExt)
          {
            result= mapDown (result, primElem, imPrimElem, oldA, dest, source);
            prune1 (oldA);
          }
          return N (d*result);
        }
        if (CFFactory::gettype() == GaloisFieldDomain)
        {
          result= modGCDGF (F, G);
          if (passToGF)
          {
            CanonicalForm mipo= gf_mipo;
            setCharacteristic (p);
            Variable alpha= rootOf (mipo.mapinto());
            result= GF2FalphaRep (result, alpha);
            prune (alpha);
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
            return N (d*modGCDFp (F,G));
        }
      }

      if (gcdfound == 1)
      {
        TIMING_DEFINE(termination_test);
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
          prune (alpha);
        }
        if (k > 1 && gcdfound)
        {
          cand= GFMapDown (cand, k);
          setCharacteristic (p, k, gf_name);
        }
        if (extOfExt && gcdfound)
        {
          cand= mapDown (cand, primElem, imPrimElem, oldA, dest, source);
          prune1 (oldA);
        }
      }
    }
    delta--;
    goodPointCount= 0;
  }
  return N (d*cand);
}
#endif

