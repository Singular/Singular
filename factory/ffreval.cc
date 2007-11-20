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
#include "ftmpl_functions.h"
#include "ffreval.h"

/* forward declarations:*/
static CanonicalForm fin_ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG, FFREvaluation & b, bool internal );
static bool fin_findeval( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db, FFREvaluation & b, int delta, int degF, int degG );
static CanonicalForm fin_ezgcd_specialcase ( const CanonicalForm & F, const CanonicalForm & G, FFREvaluation & b, const modpk & bound );

CanonicalForm fin_ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG )
{
    FFREvaluation b;
    return fin_ezgcd( FF, GG, b, false );
}

static CanonicalForm fin_ezgcd ( const CanonicalForm & FF, const CanonicalForm & GG, FFREvaluation & b, bool internal )
{
    CanonicalForm F, G, f, g, d, Fb, Gb, Db, Fbt, Gbt, Dbt, B0, B, D0, lcF, lcG, lcD;
    CFArray DD( 1, 2 ), lcDD( 1, 2 );
    int degF, degG, delta, t;
    FFREvaluation bt;
    bool gcdfound = false;
    Variable x = Variable(1);
    modpk p = modpk(getCharacteristic(), 1); // k = 1
    DEBINCLEVEL( cerr, "fin_ezgcd" );
    DEBOUTLN( cerr, "FF = " << FF );
    DEBOUTLN( cerr, "GG = " << GG );
    f = content( FF, x ); g = content( GG, x ); d = gcd( f, g );
    DEBOUTLN( cerr, "f = " << f );
    DEBOUTLN( cerr, "g = " << g );
    F = FF / f; G = GG / g;
    if ( F.isUnivariate() && G.isUnivariate() )
    {
        DEBDECLEVEL( cerr, "fin_ezgcd" );
        if(F.mvar()==G.mvar())
          d*=gcd(F,G);
        return d;
    }
    else  if ( gcd_test_one( F, G, false ) )
    {
        DEBDECLEVEL( cerr, "fin_ezgcd" );
        return d;
    }
    lcF = LC( F, x ); lcG = LC( G, x );
    lcD = gcd( lcF, lcG );
    delta = 0;
    degF = degree( F, x ); degG = degree( G, x );
    t = tmax( F.level(), G.level() );
    if ( ! internal )
    {
        b = FFREvaluation( 2, t, FFRandom() );
        b.init(); // choose random point
    }
    while ( ! gcdfound ) {
        /// ---> A2
        DEBOUTLN( cerr, "search for evaluation, delta = " << delta );
        DEBOUTLN( cerr, "F = " << F );
        DEBOUTLN( cerr, "G = " << G );
        if( ! fin_findeval( F, G, Fb, Gb, Db, b, delta, degF, degG ) )
           return 0;

        DEBOUTLN( cerr, "found evaluation b = " << b );
        DEBOUTLN( cerr, "F(b) = " << Fb );
        DEBOUTLN( cerr, "G(b) = " << Gb );
        DEBOUTLN( cerr, "D(b) = " << Db );
        delta = degree( Db );
        /// ---> A3
        if ( delta == 0 )
        {
          DEBDECLEVEL( cerr, "fin_ezgcd" );
          return d;
        }
        /// ---> A4
        //deltaold = delta;
        while ( 1 ) {
            bt = b;
            if( ! fin_findeval( F, G, Fbt, Gbt, Dbt, bt, delta + 1, degF, degG ) )
               return 0;

            int dd=degree( Dbt );
            if ( dd /*degree( Dbt )*/ == 0 )
            {
                DEBDECLEVEL( cerr, "fin_ezgcd" );
                return d;
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
        if ( degF <= degG && delta == degF && fdivides( F, G ) )
        {
            DEBDECLEVEL( cerr, "fin_ezgcd" );
            return d*F;
        }
        if ( degG < degF && delta == degG && fdivides( G, F ) )
        {
            DEBDECLEVEL( cerr, "fin_ezgcd" );
            return d*G;
        }
        if ( delta != degF && delta != degG ) {
            bool B_is_F;
            /// ---> A6
            CanonicalForm xxx;
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
                CanonicalForm dummyres = d * fin_ezgcd_specialcase( F, G, b, p );
                DEBDECLEVEL( cerr, "fin_ezgcd" );
                return dummyres;
#else
                return d * fin_ezgcd_specialcase( F, G, b, p );
#endif
              }
            }
            /// ---> A7
            DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
            DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );
            DEBOUTLN( cerr, "(hensel) B    = " << B );
            DEBOUTLN( cerr, "(hensel) lcB  = " << LC( B, Variable(1) ) );
            DEBOUTLN( cerr, "(hensel) b(B) = " << b(B) );
            DEBOUTLN( cerr, "(hensel) DD   = " << DD );
            DEBOUTLN( cerr, "(hensel) lcDD = " << lcDD );
            gcdfound = Hensel( B, DD, lcDD, b, p, x );
            DEBOUTLN( cerr, "(hensel finished) DD   = " << DD );
            if (gcdfound)
            {
              CanonicalForm xxx=content(DD[2],Variable(1));
              CanonicalForm cand=DD[2] / xxx; //content(DD[2],Variable(1));
#if 0
              gcdfound= fdivides(cand,G) &&  fdivides(cand,F);
#else
              if (B_is_F /*B==F*lcF*/)
              {
                DEBOUTLN( cerr, "(test) G: "<<G<<" % gcd:"<<cand<<" -> " << G%cand );
                gcdfound= fdivides(cand,G);
              }
              else
              {
                DEBOUTLN( cerr, "(test) F: "<<F<<" % gcd:"<<cand<<" -> " << F%cand);
                gcdfound= fdivides(cand,F);
              }
#endif
            }
            /// ---> A8 (gcdfound)
        }
        delta++;
    }
    /// ---> A9
    DEBDECLEVEL( cerr, "fin_ezgcd" );
    return d * DD[2] / content(DD[2],Variable(1));
}

static CanonicalForm fin_ezgcd_specialcase ( const CanonicalForm & F, const CanonicalForm & G, FFREvaluation & b, const modpk & bound )
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
    DEBOUTLN( cerr, "fin_ezgcd: special case" );
    CanonicalForm Ft, Gt, L, LL, Fb, Gb, Db, Lb, D, Ds, Dt;
    CFArray DD( 1, 2 ), lcDD( 1, 2 );
    Variable x = Variable( 1 );
    bool gcdfound;
    Dt = 1;
    /// ---> S1
    DEBOUTLN( cerr, "fin_ezgcdspec: (S1)" );
    Ft = fin_ezgcd( F, F.deriv( x ) );
    if ( Ft.isOne() ) {
        // In this case F is squarefree and we came here by bad chance
        // (means: bad evaluation point).  Try again with another
        // evaluation point.  Bug fix (?) by JS.  The bad example was
        // gcd.debug -ocr /+USE_EZGCD/@12/CB \
        //     '(16*B^8-208*B^6*C+927*B^4*C^2-1512*B^2*C^3+432*C^4)' \
        //     '(4*B^7*C^2-50*B^5*C^3+208*B^3*C^4-288*B*C^5)'
        if( ! b.step() ) // Error: run out of points
           return 0;

        return fin_ezgcd( F, G, b, true );
    }
    DEBOUTLN( cerr, "fin_ezgcdspec: (S1) done, Ft = " << Ft );
    L = F / Ft;
    /// ---> S2
    DEBOUTLN( cerr, "fin_ezgcdspec: (S2)" );
    L = fin_ezgcd( L, G, b, true );
    DEBOUTLN( cerr, "fin_ezgcdspec: (S2) done, Ds = " << Ds );
    Gt = G / L;
    Ds = L; Dt = L;
    Lb = b( L ); Gb = b( Gt ); Fb = b( Ft );
    d = gcd( LC( L, x ), gcd( LC( Ft, x ), LC( Gt, x ) ) );
    while ( true ) {
        /// ---> S3
        DEBOUTLN( cerr, "fin_ezgcdspec: (S3)" );
        DEBOUTLN( cerr, "fin_ezgcdspec: Fb = " << Fb );
        DEBOUTLN( cerr, "fin_ezgcdspec: Gb = " << Gb );
        DD[1] = gcd( Lb, gcd( Fb, Gb ) );
        /// ---> S4
        DEBOUTLN( cerr, "fin_ezgcdspec: (S4)" );
        if ( degree( DD[1] ) == 0 )
            return Ds;
        /// ---> S5
        DEBOUTLN( cerr, "fin_ezgcdspec: (S5)" );
        DEBOUTLN( cerr, "fin_ezgcdspec: Lb = " << Lb );
        DEBOUTLN( cerr, "fin_ezgcdspec: Db = " << DD[1] );
        Db = DD[1];
        if ( ! (DD[2] = Lb/DD[1]).isOne() ) {
            DEBOUTLN( cerr, "fin_ezgcdspec: (S55)" );
            lcDD[2] = LC( L, x );
            lcDD[1] = d;
            DD[1] = ( DD[1] * b( d ) ) / lc( DD[1] );
            DD[2] = ( DD[2] * b( lcDD[2] ) ) / lc( DD[2] );
            LL = L * d;
            DEBOUTLN( cerr, "fin_ezgcdspec: begin with lift." );
            DEBOUTLN( cerr, "fin_ezgcdspec: B     = " << LL );
            DEBOUTLN( cerr, "fin_ezgcdspec: DD    = " << DD );
            DEBOUTLN( cerr, "fin_ezgcdspec: lcDD  = " << lcDD );
            DEBOUTLN( cerr, "fin_ezgcdspec: b     = " << b );
            DEBOUTLN( cerr, "fin_ezgcdspec: lc(B) = " << LC( LL, x ) );
            DEBOUTLN( cerr, "fin_ezgcdspec: test  = " << b( LL ) - DD[1] * DD[2] );
            gcdfound = Hensel( LL, DD, lcDD, b, p, x );
            ASSERT( gcdfound, "fatal error in algorithm" );
            Dt = pp( DD[1] );
        }
        DEBOUTLN( cerr, "fin_ezgcdspec: (S7)" );
        Ds = Ds * Dt;
        Fb = Fb / Db;
        Gb = Gb / Db;
    }
    // this point is never reached, but to avoid compiler warnings let's return a value
    return 0;
#endif
}

static bool fin_findeval( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & Fb, CanonicalForm & Gb, CanonicalForm & Db, FFREvaluation & b, int delta, int degF, int degG )
{
    int i;
    bool ok;
    DEBOUTLN( cerr, "fin_ezgcd: (fin_findeval) F = " << F  <<", G="<< G);
    DEBOUTLN( cerr, "fin_ezgcd: (fin_findeval) degF = " << degF << ", degG="<<degG );
    while(1) {
        DEBOUTLN( cerr, "fin_ezgcd: (fin_findeval) b = " << b );
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
        if ( ok )
            break;

        if( b.step() ) // can choose valid point
           continue;

        return false;
    }
    return true;
}
