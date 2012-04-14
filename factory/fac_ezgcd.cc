/* emacs edit mode for this file is -*- C++ -*- */

#include <config.h>

#include "assert.h"
#include "debug.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "fac_util.h"
#include "cf_algorithm.h"
#include "cf_reval.h"
#include "cf_random.h"
#include "cf_primes.h"
#include "fac_distrib.h"
#include "templates/ftmpl_functions.h"
#include "cf_map.h"
#include "facHensel.h"

static
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
  ASSERT (A.max() == n, "expected A.max() == n");
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
  for (int i= A.min(); i <= A.max(); i++)
    evaluation.append (A [i]);
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

static void findeval( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db, REvaluation & b, int delta, int degF, int degG );

static CanonicalForm ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG, REvaluation & b, bool internal );

static CanonicalForm ezgcd_specialcase ( const CanonicalForm & F, const CanonicalForm & G, REvaluation & b, const modpk & bound );

static modpk findBound ( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & lcF, const CanonicalForm & lcG, int degF, int degG );

static modpk enlargeBound ( const CanonicalForm & F, const CanonicalForm & Lb, const CanonicalForm & Db, const modpk & pk );

CanonicalForm
ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG )
{
    REvaluation b;
    return ezgcd( FF, GG, b, false );
}

static CanonicalForm
ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG, REvaluation & b, bool internal )
{
    CanonicalForm F, G, f, g, d, Fb, Gb, Db, Fbt, Gbt, Dbt, B0, B, D0, lcF, lcG, lcD, cand, result;
    CFArray DD( 1, 2 ), lcDD( 1, 2 );
    int degF, degG, delta, t;
    REvaluation bt;
    bool gcdfound = false;
    Variable x = Variable(1);
    modpk bound;

    F= FF;
    G= GG;

    CFMap M,N;
    int smallestDegLev;
    int best_level= compress4EZGCD (F, G, M, N, smallestDegLev);

    if (best_level == 0) return G.genOne();

    F= M (F);
    G= M (G);


    DEBINCLEVEL( cerr, "ezgcd" );
    DEBOUTLN( cerr, "FF = " << FF );
    DEBOUTLN( cerr, "GG = " << GG );
    f = content( F, x ); g = content( G, x ); d = gcd( f, g );
    DEBOUTLN( cerr, "f = " << f );
    DEBOUTLN( cerr, "g = " << g );
    F /= f; G /= g;
    if ( F.isUnivariate() && G.isUnivariate() )
    {
        DEBDECLEVEL( cerr, "ezgcd" );
        if(F.mvar()==G.mvar())
          d*=gcd(F,G);
        return N (d);
    }
    else  if ( gcd_test_one( F, G, false ) )
    {
        DEBDECLEVEL( cerr, "ezgcd" );
        return N (d);
    }
    lcF = LC( F, x ); lcG = LC( G, x );
    lcD = gcd( lcF, lcG );
    delta = 0;
    degF = degree( F, x ); degG = degree( G, x );
    t = tmax( F.level(), G.level() );
    //bound = findBound( F, G, lcF, lcG, degF, degG );
    if ( ! internal )
        b = REvaluation( 2, t, IntRandom( 25 ) );
    while ( ! gcdfound )
    {
        /// ---> A2
        DEBOUTLN( cerr, "search for evaluation, delta = " << delta );
        DEBOUTLN( cerr, "F = " << F );
        DEBOUTLN( cerr, "G = " << G );
        findeval( F, G, Fb, Gb, Db, b, delta, degF, degG );
        DEBOUTLN( cerr, "found evaluation b = " << b );
        DEBOUTLN( cerr, "F(b) = " << Fb );
        DEBOUTLN( cerr, "G(b) = " << Gb );
        DEBOUTLN( cerr, "D(b) = " << Db );
        delta = degree( Db );
        /// ---> A3
        if ( delta == 0 )
        {
          DEBDECLEVEL( cerr, "ezgcd" );
          return N (d);
        }
        /// ---> A4
        //deltaold = delta;
        while ( 1 ) {
            bt = b;
            findeval( F, G, Fbt, Gbt, Dbt, bt, delta + 1, degF, degG );
            int dd=degree( Dbt );
            if ( dd /*degree( Dbt )*/ == 0 )
            {
                DEBDECLEVEL( cerr, "ezgcd" );
                return N (d);
            }
            if ( dd /*degree( Dbt )*/ == delta )
                break;
            else  if ( dd /*degree( Dbt )*/ < delta ) {
                delta = dd /*degree( Dbt )*/;
                b = bt;
                Db = Dbt; Fb = Fbt; Gb = Gbt;
            }
        }
        DEBOUTLN( cerr, "now after A4, delta = " << delta );
        /// ---> A5
        if (delta == degF)
        {
          if (degF <= degG  && fdivides (F, G))
          {
            DEBDECLEVEL( cerr, "ezgcd" );
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
            return N (d*G);
          }
          else
            delta--;
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
              Off(SW_USE_EZGCD);
              result=gcd( F, G );
              On(SW_USE_EZGCD);
              return N (d*result);
            }
            /*CanonicalForm xxx;
            //if ( gcd( (DD[1] = Fb / Db), Db ) == 1 ) {
            DD[1] = Fb / Db;
            xxx= gcd( DD[1], Db );
            DEBOUTLN( cerr, "gcd((Fb/Db),Db) = " << xxx );
            DEBOUTLN( cerr, "Fb/Db = " << DD[1] );
            DEBOUTLN( cerr, "Db = " << Db );
            if (xxx.inCoeffDomain()) {
                B = F;
                DD[2] = Db;
                lcDD[1] = lcF;
                lcDD[2] = lcF;
                B *= lcF;
                B_is_F=true;
            }
            //else  if ( gcd( (DD[1] = Gb / Db), Db ) == 1 ) {
            else
            {
              DD[1] = Gb / Db;
              xxx=gcd( DD[1], Db );
              DEBOUTLN( cerr, "gcd((Gb/Db),Db) = " << xxx );
              DEBOUTLN( cerr, "Gb/Db = " << DD[1] );
              DEBOUTLN( cerr, "Db = " << Db );
              if (xxx.inCoeffDomain())
              {
                B = G;
                DD[2] = Db;
                lcDD[1] = lcG;
                lcDD[2] = lcG;
                B *= lcG;
                B_is_F=false;
              }
              else
              {
#ifdef DEBUGOUTPUT
                CanonicalForm dummyres = d * ezgcd_specialcase( F, G, b, bound );
                DEBDECLEVEL( cerr, "ezgcd" );
                return dummyres;
#else
                return d * ezgcd_specialcase( F, G, b, bound );
#endif
              }
            }*/
            /// ---> A7
            DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
            DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );
            //bound = enlargeBound( B, DD[2], DD[1], bound );
            DEBOUTLN( cerr, "(hensel) B    = " << B );
            DEBOUTLN( cerr, "(hensel) lcB  = " << LC( B, Variable(1) ) );
            DEBOUTLN( cerr, "(hensel) b(B) = " << b(B) );
            DEBOUTLN( cerr, "(hensel) DD   = " << DD );
            DEBOUTLN( cerr, "(hensel) lcDD = " << lcDD );
            gcdfound= Hensel (B*lcD, DD, b, lcDD);
            DEBOUTLN( cerr, "(hensel finished) DD   = " << DD );

            if (gcdfound == -1)
            {
              Off (SW_USE_EZGCD);
              result= gcd (F,G);
              On (SW_USE_EZGCD_P);
              return N (d*result);
            }

            if (gcdfound)
            {
              cand = DD[2] / content( DD[2], Variable(1) );
              gcdfound = fdivides( cand, G ) && fdivides ( cand, F );
            }
            /// ---> A8 (gcdfound)
        }
        delta--;
    }
    /// ---> A9
    DEBDECLEVEL( cerr, "ezgcd" );
    return N (d*cand);
}

static CanonicalForm
ezgcd_specialcase ( const CanonicalForm & F, const CanonicalForm & G, REvaluation & b, const modpk & bound )
{
    CanonicalForm d;
#if 1
    Off(SW_USE_EZGCD);
    //bool ntl0=isOn(SW_USE_NTL_GCD_0);
    //Off(SW_USE_NTL_GCD_0);
    //bool ntlp=isOn(SW_USE_NTL_GCD_P);
    //Off(SW_USE_NTL_GCD_P);
    d=gcd( F, G );
    //if (ntl0) On(SW_USE_NTL_GCD_0);
    //if (ntlp) On(SW_USE_NTL_GCD_P);
    On(SW_USE_EZGCD);
    return d;
#else
    DEBOUTLN( cerr, "ezgcd: special case" );
    CanonicalForm Ft, Gt, L, LL, Fb, Gb, Db, Lb, D, Ds, Dt;
    CFArray DD( 1, 2 ), lcDD( 1, 2 );
    Variable x = Variable( 1 );
    bool gcdfound;

    Dt = 1;
    /// ---> S1
    DEBOUTLN( cerr, "ezgcdspec: (S1)" );
    Ft = ezgcd( F, F.deriv( x ) );
    if ( Ft.isOne() )
    {
        // In this case F is squarefree and we came here by bad chance
        // (means: bad evaluation point).  Try again with another
        // evaluation point.  Bug fix (?) by JS.  The bad example was
        // gcd.debug -ocr /+USE_EZGCD/@12/CB
        //     '(16*B^8-208*B^6*C+927*B^4*C^2-1512*B^2*C^3+432*C^4)'
        //     '(4*B^7*C^2-50*B^5*C^3+208*B^3*C^4-288*B*C^5)'
        b.nextpoint();
        return ezgcd( F, G, b, true );
    }

    DEBOUTLN( cerr, "ezgcdspec: (S1) done, Ft = " << Ft );
    L = F / Ft;
    /// ---> S2
    DEBOUTLN( cerr, "ezgcdspec: (S2)" );

    L = ezgcd( L, G, b, true );
    DEBOUTLN( cerr, "ezgcdspec: (S2) done, Ds = " << Ds );
    Gt = G / L;
    Ds = L; Dt = L;
    Lb = b( L ); Gb = b( Gt ); Fb = b( Ft );
    d = gcd( LC( L, x ), gcd( LC( Ft, x ), LC( Gt, x ) ) );
    while ( true ) {
        /// ---> S3
        DEBOUTLN( cerr, "ezgcdspec: (S3)" );
        DEBOUTLN( cerr, "ezgcdspec: Fb = " << Fb );
        DEBOUTLN( cerr, "ezgcdspec: Gb = " << Gb );
        DD[1] = gcd( Lb, gcd( Fb, Gb ) );
        /// ---> S4
        DEBOUTLN( cerr, "ezgcdspec: (S4)" );
        if ( degree( DD[1] ) == 0 )
            return Ds;
        /// ---> S5
        DEBOUTLN( cerr, "ezgcdspec: (S5)" );
        DEBOUTLN( cerr, "ezgcdspec: Lb = " << Lb );
        DEBOUTLN( cerr, "ezgcdspec: Db = " << DD[1] );
        Db = DD[1];
        if ( ! (DD[2] = Lb/DD[1]).isOne() ) {
            DEBOUTLN( cerr, "ezgcdspec: (S55)" );
            lcDD[2] = LC( L, x );
            lcDD[1] = d;
            DD[1] = ( DD[1] * b( d ) ) / lc( DD[1] );
            DD[2] = ( DD[2] * b( lcDD[2] ) ) / lc( DD[2] );
            LL = L * d;
            modpk newbound = enlargeBound( LL, DD[2], DD[1], bound );
            DEBOUTLN( cerr, "ezgcdspec: begin with lift." );
            DEBOUTLN( cerr, "ezgcdspec: B     = " << LL );
            DEBOUTLN( cerr, "ezgcdspec: DD    = " << DD );
            DEBOUTLN( cerr, "ezgcdspec: lcDD  = " << lcDD );
            DEBOUTLN( cerr, "ezgcdspec: b     = " << b );
            DEBOUTLN( cerr, "ezgcdspec: bound = " << bound.getpk() );
            DEBOUTLN( cerr, "ezgcdspec: lc(B) = " << LC( LL, x ) );
            DEBOUTLN( cerr, "ezgcdspec: test  = " << b( LL ) - DD[1] * DD[2] );
            gcdfound = Hensel( LL, DD, lcDD, b, newbound, x );
            ASSERT( gcdfound, "fatal error in algorithm" );
            Dt = pp( DD[1] );
        }
        DEBOUTLN( cerr, "ezgcdspec: (S7)" );
        Ds = Ds * Dt;
        Fb = Fb / Db;
        Gb = Gb / Db;
    }
    // this point is never reached, but to avoid compiler warnings let's return a value
    return 0;
#endif
}

static void
findeval( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db, REvaluation & b, int delta, int degF, int degG )
{
    bool ok;
    if ( delta != 0 )
        b.nextpoint();
    DEBOUTLN( cerr, "ezgcd: (findeval) F = " << F  <<", G="<< G);
    DEBOUTLN( cerr, "ezgcd: (findeval) degF = " << degF << ", degG="<<degG );
    do {
        DEBOUTLN( cerr, "ezgcd: (findeval) b = " << b );
        Fb = b( F );
        ok = degree( Fb ) == degF;
        if ( ok ) {
            Gb = b( G );
            ok = degree( Gb ) == degG;
        }

        if ( ok )
        {
            Db = gcd( Fb, Gb );
            if ( delta > 0 )
              ok = degree( Db ) < delta;
        }
        if ( ! ok )
        {
            b.nextpoint();
        }
    } while ( ! ok );
}

static modpk
enlargeBound ( const CanonicalForm & F, const CanonicalForm & Lb, const CanonicalForm & Db, const modpk & pk )
{
    DEBOUTLN( cerr, "ezgcd: (enlarge bound) F      = " << F );
    DEBOUTLN( cerr, "ezgcd: (enlarge bound) Lb     = " << Lb );
    DEBOUTLN( cerr, "ezgcd: (enlarge bound) Db     = " << Db );
    DEBOUTLN( cerr, "ezgcd: (enlarge bound) Lb % p = " << mod( Lb, pk.getp() ) );
    DEBOUTLN( cerr, "ezgcd: (enlarge bound) Db % p = " << mod( Db, pk.getp() ) );

    CanonicalForm limit = power( CanonicalForm(2), degree( Db ) ) *
        tmax( maxNorm( Lb ), tmax( maxNorm( Db ), maxNorm( F ) ) );
    int p = pk.getp();
    int k = pk.getk();
    CanonicalForm bound = pk.getpk();
    while ( bound < limit ) {
        k++;
        bound *= p;
    }
    k *= 2;
    DEBOUTLN( cerr, "ezgcd: (enlarge bound) newbound = " << power( CanonicalForm( p ), k ) );
    return modpk( p, k );
}

static modpk
findBound ( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & lcF, const CanonicalForm & lcG, int degF, int degG )
{
    CanonicalForm limit = power( CanonicalForm(2), tmin( degF, degG ) ) *
        gcd( icontent( lcF ), icontent( lcG ) ) * tmin( maxNorm( F ), maxNorm( G ) );
    int p, i = 0;
    do {
        p = cf_getBigPrime( i );
        i++;
    } while ( mod( lcF, p ).isZero() && mod( lcG, p ).isZero() );
    CanonicalForm bound = p;
    i = 1;
    while ( bound < limit ) {
        i++;
        bound *= p;
    }
    return modpk( p, i );
}
