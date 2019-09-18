/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"
#include "debug.h"
#include "timing.h"

#include "cf_defs.h"
#include "cf_algorithm.h"
#include "fac_multivar.h"
#include "fac_univar.h"
#include "cf_reval.h"
#include "cf_map.h"
#include "fac_util.h"
#include "cf_binom.h"
#include "cf_iter.h"
#include "cf_primes.h"
#include "fac_distrib.h"

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);
void out_cff(CFFList &L);

TIMING_DEFINE_PRINT(fac_content)
TIMING_DEFINE_PRINT(fac_findeval)
TIMING_DEFINE_PRINT(fac_distrib)
TIMING_DEFINE_PRINT(fac_hensel)

static CFArray
conv_to_factor_array( const CFFList & L )
{
    int n;
    CFFListIterator I = L;
    bool negate = false;

    if ( ! I.hasItem() )
        n = 0;
    else  if ( I.getItem().factor().inBaseDomain() ) {
        negate = I.getItem().factor().sign() < 0;
        I++;
        n = L.length();
    }
    else
        n = L.length() + 1;
    CFFListIterator J = I;
    while ( J.hasItem() ) {
        n += J.getItem().exp() - 1;
        J++;
    }
    CFArray result( 1, n-1 );
    int i, j, k;
    i = 1;
    while ( I.hasItem() ) {
        k = I.getItem().exp();
        for ( j = 1; j <= k; j++ ) {
            result[i] = I.getItem().factor();
            i++;
        }
        I++;
    }
    if ( negate )
        result[1] = -result[1];
    return result;
}

static modpk
coeffBound ( const CanonicalForm & f, int p )
{
    int * degs = degrees( f );
    int M = 0, i, k = f.level();
    for ( i = 1; i <= k; i++ )
        M += degs[i];
    CanonicalForm b = 2 * maxNorm( f ) * power( CanonicalForm( 3 ), M );
    CanonicalForm B = p;
    k = 1;
    while ( B < b ) {
        B *= p;
        k++;
    }
    return modpk( p, k );
}

// static bool
// nonDivisors ( CanonicalForm omega, CanonicalForm delta, const CFArray & F, CFArray & d )
// {
//     DEBOUTLN( cerr, "nondivisors omega = " << omega );
//     DEBOUTLN( cerr, "nondivisors delta = " << delta );
//     DEBOUTLN( cerr, "nondivisors F = " << F );
//     CanonicalForm q, r;
//     int k = F.size();
//     d = CFArray( 0, k );
//     d[0] = delta * omega;
//     for ( int i = 1; i <= k; i++ ) {
//         q = abs(F[i]);
//         for ( int j = i-1; j >= 0; j-- ) {
//             r = d[j];
//             do {
//                 r = gcd( r, q );
//                 q = q / r;
//             } while ( r != 1 );
//             if ( q == 1 )
//                 return false;
//         }
//         d[i] = q;
//     }
//     return true;
// }

static void
findEvaluation ( const CanonicalForm & U, const CanonicalForm & V, const CanonicalForm & omega, const CFFList & F, Evaluation & A, CanonicalForm & U0, CanonicalForm & delta, CFArray & D, int r )
{
    DEBINCLEVEL( cerr, "findEvaluation" );
    CanonicalForm Vn;
    CFFListIterator I;
    int j;
    bool found = false;
    CFArray FF = CFArray( 1, F.length() );
    if ( r > 0 )
        A.nextpoint();
    while ( ! found )
    {
        Vn = A( V );
        if ( Vn != 0 )
        {
            U0 = A( U );
            DEBOUTLN( cerr, "U0 = " << U0 );
            if ( isSqrFree( U0 ) )
            {
                delta = content( U0 );
                DEBOUTLN( cerr, "content( U0 ) = " << delta );
                for ( I = F, j = 1; I.hasItem(); I++, j++ )
                    FF[j] = A( I.getItem().factor() );
                found = nonDivisors( omega, delta, FF, D );
            }
            else
            {
                DEBOUTLN( cerr, "not sqrfree : " << sqrFree( U0 ) );
            }
        }
        if ( ! found )
            A.nextpoint();
    }
    DEBDECLEVEL( cerr, "findEvaluation" );
}

#ifdef HAVE_NTL
THREAD_VAR int prime_number=0;
void find_good_prime(const CanonicalForm &f, int &start)
{
  if (! f.inBaseDomain() )
  {
    CFIterator i = f;
    for(;;)
    {
      if  ( i.hasTerms() )
      {
        find_good_prime(i.coeff(),start);
        if (0==cf_getSmallPrime(start)) return;
        if((i.exp()!=0) && ((i.exp() % cf_getSmallPrime(start))==0))
        {
          start++;
          i=f;
        }
        else  i++;
      }
      else break;
    }
  }
  else
  {
    if (f.inZ())
    {
      if (0==cf_getSmallPrime(start)) return;
      while((!f.isZero()) && (mod(f,cf_getSmallPrime(start))==0))
      {
        start++;
        if (0==cf_getSmallPrime(start)) return;
      }
    }
/* should not happen!
    else if (f.inQ())
    {
      while((f.den()!=0) && (mod(f.den(),cf_getSmallPrime(start))==0))
      {
        start++;
      }
      while((f.num()!=0) && (mod(f.num(),cf_getSmallPrime(start))==0))
      {
        start++;
      }
    }
    else
          cout <<"??"<< f <<"\n";
*/
  }
}
#endif

static CFArray ZFactorizeMulti ( const CanonicalForm & arg )
{
    DEBINCLEVEL( cerr, "ZFactorizeMulti" );
    CFMap M;
    CanonicalForm UU, U = compress( arg, M );
    CanonicalForm delta, omega, V = LC( U, 1 );
    int t = U.level();
    CFFList F = factorize( V );
    CFFListIterator I, J;
    CFArray G, lcG, D;
    int i, j, r, maxdeg;
    REvaluation A( 2, t, IntRandom( 50 ) );
    CanonicalForm U0;
    CanonicalForm ft, ut, gt, d;
    modpk b;
    bool negate = false;

    DEBOUTLN( cerr, "-----------------------------------------------------" );
    DEBOUTLN( cerr, "trying to factorize U = " << U );
    DEBOUTLN( cerr, "U is a polynomial of level = " << arg.level() );
    DEBOUTLN( cerr, "U will be factorized with respect to variable " << Variable(1) );
    DEBOUTLN( cerr, "the leading coefficient of U with respect to that variable is " << V );
    DEBOUTLN( cerr, "which is factorized as " << F );

    maxdeg = 0;
    for ( i = 2; i <= t; i++ )
    {
        j = U.degree( Variable( i ) );
        if ( j > maxdeg ) maxdeg = j;
    }

    if ( F.getFirst().factor().inCoeffDomain() )
    {
        omega = F.getFirst().factor();
        F.removeFirst();
        if ( omega < 0 )
        {
            negate = true;
            omega = -omega;
            U = -U;
        }
    }
    else
        omega = 1;

    bool goodeval = false;
    r = 0;

//    for ( i = 0; i < 10*t; i++ )
//        A.nextpoint();

    while ( ! goodeval )
    {
        TIMING_START(fac_findeval);
        findEvaluation( U, V, omega, F, A, U0, delta, D, r );
        TIMING_END(fac_findeval);
        DEBOUTLN( cerr, "the evaluation point to reduce to an univariate problem is " << A );
        DEBOUTLN( cerr, "corresponding delta = " << delta );
        DEBOUTLN( cerr, "              omega = " << omega );
        DEBOUTLN( cerr, "              D     = " << D );
        DEBOUTLN( cerr, "now factorize the univariate polynomial " << U0 );
        G = conv_to_factor_array( factorize( U0, false ) );
        DEBOUTLN( cerr, "which factorizes into " << G );
        #ifdef HAVE_NTL
        {
          int i=prime_number;
          find_good_prime(arg,i);
          find_good_prime(U0,i);
          find_good_prime(U,i);
          int p=cf_getSmallPrime(i);
          //printf("found:p=%d (%d)\n",p,i);
          if (p==0)
          {
            return conv_to_factor_array(CFFactor(arg,1));
            //printf("out of primes - switch ot non-NTL\n");
          }
          else if (((i==0)||(i!=prime_number)))
          {
            b = coeffBound( U, p );
            prime_number=i;
          }
          // p!=0:
          modpk bb=coeffBound(U0,p);
          if (bb.getk() > b.getk() ) b=bb;
          bb=coeffBound(arg,p);
          if (bb.getk() > b.getk() ) b=bb;
        }
        #else
        b = coeffBound( U, getZFacModulus().getp() );
        if ( getZFacModulus().getpk() > b.getpk() )
            b = getZFacModulus();
        #endif
        //printf("p=%d, k=%d\n",b.getp(),b.getk());
        DEBOUTLN( cerr, "the coefficient bound of the factors of U is " << b.getpk() );

        r = G.size();
        lcG = CFArray( 1, r );
        UU = U;
        DEBOUTLN( cerr, "now trying to distribute the leading coefficients ..." );
        TIMING_START(fac_distrib);
        goodeval = distributeLeadingCoeffs( UU, G, lcG, F, D, delta, omega, A, r );
        TIMING_END(fac_distrib);
#ifdef DEBUGOUTPUT
        if ( goodeval )
        {
            DEBOUTLN( cerr, "the univariate factors after distribution are " << G );
            DEBOUTLN( cerr, "the distributed leading coeffs are " << lcG );
            DEBOUTLN( cerr, "U may have changed and is now " << UU );
            DEBOUTLN( cerr, "which has leading coefficient " << LC( UU, Variable(1) ) );

            if ( LC( UU, Variable(1) ) != prod( lcG ) || A(UU) != prod( G ) )
            {
                DEBOUTLN( cerr, "!!! distribution was not correct !!!" );
                DEBOUTLN( cerr, "product of leading coeffs is " << prod( lcG ) );
                DEBOUTLN( cerr, "product of univariate factors is " << prod( G ) );
                DEBOUTLN( cerr, "the new U is evaluated as " << A(UU) );
            }
            else
                DEBOUTLN( cerr, "leading coeffs correct" );
        }
        else
        {
            DEBOUTLN( cerr, "we have found a bad evaluation point" );
        }
#endif
        if ( goodeval )
        {
            TIMING_START(fac_hensel);
            goodeval = Hensel( UU, G, lcG, A, b, Variable(1) );
            TIMING_END(fac_hensel);
        }
    }
    for ( i = 1; i <= r; i++ )
    {
        G[i] /= icontent( G[i] );
        G[i] = M(G[i]);
    }
    // negate noch beachten !
    if ( negate )
        G[1] = -G[1];
    DEBDECLEVEL( cerr, "ZFactorMulti" );
    return G;
}

CFFList ZFactorizeMultivariate ( const CanonicalForm & f, bool issqrfree )
{
    CFFList G, F, R;
    CFArray GG;
    CFFListIterator i, j;
    CFMap M;
    CanonicalForm g, cont;
    Variable v1, vm;
    int k, m, n;

    v1 = Variable(1);
    if ( issqrfree )
        F = CFFactor( f, 1 );
    else
        F = sqrFree( f );

    for ( i = F; i.hasItem(); i++ )
    {
        if ( i.getItem().factor().inCoeffDomain() )
            R.append( CFFactor( i.getItem().factor(), i.getItem().exp() ) );
        else
        {
            TIMING_START(fac_content);
            g = compress( i.getItem().factor(), M );
            // now after compress g contains Variable(1)
            vm = g.mvar();
            g = swapvar( g, v1, vm );
            cont = content( g );
            g = swapvar( g / cont, v1, vm );
            cont = swapvar( cont, v1, vm );
            n = i.getItem().exp();
            TIMING_END(fac_content);
            DEBOUTLN( cerr, "now after content ..." );
            if ( g.isUnivariate() )
            {
                G = factorize( g, true );
                for ( j = G; j.hasItem(); j++ )
                    if ( ! j.getItem().factor().isOne() )
                        R.append( CFFactor( M( j.getItem().factor() ), n ) );
            }
            else
            {
                GG = ZFactorizeMulti( g );
                m = GG.max();
                for ( k = GG.min(); k <= m; k++ )
                    if ( ! GG[k].isOne() )
                        R.append( CFFactor( M( GG[k] ), n ) );
            }
            G = factorize( cont, true );
            for ( j = G; j.hasItem(); j++ )
                if ( ! j.getItem().factor().isOne() )
                    R.append( CFFactor( M( j.getItem().factor() ), n ) );
        }
    }
    return R;
}
