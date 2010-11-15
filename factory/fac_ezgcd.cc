/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

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
    CanonicalForm F, G, f, g, d, Fb, Gb, Db, Fbt, Gbt, Dbt, B0, B, D0, lcF, lcG, lcD;
    CFArray DD( 1, 2 ), lcDD( 1, 2 );
    int degF, degG, delta, t;
    REvaluation bt;
    bool gcdfound = false;
    Variable x = Variable(1);
    modpk bound;

    DEBINCLEVEL( cerr, "ezgcd" );
    DEBOUTLN( cerr, "FF = " << FF );
    DEBOUTLN( cerr, "GG = " << GG );
    f = content( FF, x ); g = content( GG, x ); d = gcd( f, g );
    DEBOUTLN( cerr, "f = " << f );
    DEBOUTLN( cerr, "g = " << g );
    F = FF / f; G = GG / g;
    if ( F.isUnivariate() && G.isUnivariate() )
    {
        DEBDECLEVEL( cerr, "ezgcd" );
        if(F.mvar()==G.mvar())
          d*=gcd(F,G);
        return d;
    }
    else  if ( gcd_test_one( F, G, false ) )
    {
        DEBDECLEVEL( cerr, "ezgcd" );
        return d;
    }
    lcF = LC( F, x ); lcG = LC( G, x );
    lcD = gcd( lcF, lcG );
    delta = 0;
    degF = degree( F, x ); degG = degree( G, x );
    t = tmax( F.level(), G.level() );
    bound = findBound( F, G, lcF, lcG, degF, degG );
    if ( ! internal )
        b = REvaluation( 2, t, IntRandom( 25 ) );
    while ( ! gcdfound ) {
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
          return d;
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
            DEBDECLEVEL( cerr, "ezgcd" );
            return d*F;
        }
        if ( degG < degF && delta == degG && fdivides( G, F ) )
        {
            DEBDECLEVEL( cerr, "ezgcd" );
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
                CanonicalForm dummyres = d * ezgcd_specialcase( F, G, b, bound );
                DEBDECLEVEL( cerr, "ezgcd" );
                return dummyres;
#else
                return d * ezgcd_specialcase( F, G, b, bound );
#endif
              }
            }
            /// ---> A7
            DD[2] = DD[2] * ( b( lcDD[2] ) / lc( DD[2] ) );
            DD[1] = DD[1] * ( b( lcDD[1] ) / lc( DD[1] ) );
            bound = enlargeBound( B, DD[2], DD[1], bound );
            DEBOUTLN( cerr, "(hensel) B    = " << B );
            DEBOUTLN( cerr, "(hensel) lcB  = " << LC( B, Variable(1) ) );
            DEBOUTLN( cerr, "(hensel) b(B) = " << b(B) );
            DEBOUTLN( cerr, "(hensel) DD   = " << DD );
            DEBOUTLN( cerr, "(hensel) lcDD = " << lcDD );
            gcdfound = Hensel( B, DD, lcDD, b, bound, x );
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
    DEBDECLEVEL( cerr, "ezgcd" );
    return d * DD[2] / content(DD[2],Variable(1));
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
        // gcd.debug -ocr /+USE_EZGCD/@12/CB \
        //     '(16*B^8-208*B^6*C+927*B^4*C^2-1512*B^2*C^3+432*C^4)' \
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
