/* emacs edit mode for this file is -*- C++ -*- */

//{{{ docu
//
// gengftables.cc - generate addition tables used by Factory
//   to calculate in GF(q).
//
// Note: This may take quite a while ...
//
//}}}


#include "config.h"


#ifdef HAVE_IOSTREAM
#include <iostream>
#include <fstream>
#include <strstream>
using namespace std;
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#endif

#include "cf_assert.h"
#include "gf_tabutil.h"
#include "cf_algorithm.h"
#include "cf_iter.h"

//{{{ constants
//{{{ docu
//
// - constants.
//
// maxtable: maximal size of a gf_table
// primes, primes_len:
//   used to step through possible extensions
//
//}}}
const int maxtable = 32767;

const int primes_len = 42;
THREAD_VAR static unsigned short primes [] =
{
      2,   3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181
};
//}}}

//{{{ bool isIrreducible ( const CanonicalForm & f )
//{{{ docu
//
// isIrreducible() - return true iff f is irreducible.
//
//}}}
bool
isIrreducible ( const CanonicalForm & f )
{
    CFFList F = factorize( f );
    return F.length() == 1 && F.getFirst().exp() == 1;
}
//}}}

//{{{ int exponent ( const CanonicalForm & f, int q )
//{{{ docu
//
// exponent() - return e > 0 such that x^e == 1 mod f.
//
// q: upper limit for e (?)
//
//}}}
int
exponent ( const CanonicalForm & f, int q )
{
    Variable x = f.mvar();
    int e = 1;
    CanonicalForm prod = x;
    while ( e <= q && ! prod.isOne() ) {
        e++;
        prod = ( prod * x ) % f;
    }
    return e;
}
//}}}

//{{{ bool findGenRec ( int d, int n, int q, const CanonicalForm & m, const Variable & x, CanonicalForm & result )
//{{{ docu
//
// findGenRec() - find a generator of GF(q).
//
// Returns true iff result is a valid generator.
//
// d: degree of extension
// q: the q in GF(q) (q == p^d)
// x: generator should be a poly in x
// n, m: used to step recursively through all polys in FF(p)
//   Initially, n == d and m == 0.  If 0 <= n <= d we are
//   in the process of building m, if n < 0 we built m and
//   may test whether it generates GF(q).
// result: generator found
//
// i: used to step through GF(p)
// p: current characteristic
//
//}}}
bool
findGenRec ( int d, int n, int q,
             const CanonicalForm & m, const Variable & x,
             CanonicalForm & result )
{
    int i, p = getCharacteristic();
    if ( n < 0 ) {
        cerr << "."; cerr.flush();
        // check whether m is irreducible
        if ( isIrreducible( m ) ) {
            cerr << "*"; cerr.flush();
            // check whether m generates multiplicative group
            if ( exponent( m, q ) == q - 1 ) {
                result = m;
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    // for each monomial x^0, ..., x^n, ..., x^d, try all possible coefficients
    else  if ( n == d || n == 0 ) {
        // we want to have a leading coefficient and a constant term,
        // so start with coefficient >= 1
        for ( i = 1; i < p; i++ )
            if ( findGenRec( d, n-1, q, m + i * power( x, n ), x, result ) )
                return true;
    }
    else {
        for ( i = 0; i < p; i++ )
            if ( findGenRec( d, n-1, q, m + i * power( x, n ), x, result ) )
                return true;
    }
    return false;
}
//}}}

//{{{ CanonicalForm findGen ( int d, int q )
//{{{ docu
//
// findGen() - find and return a generator of GF(q).
//
// d: degree of extension
// q: the q in GF(q)
//
//}}}
CanonicalForm
findGen ( int d, int q )
{
    Variable x( 1 );
    CanonicalForm result;
    cerr << "testing p = " << getCharacteristic() << ", d = " << d << " ... ";
    cerr.flush();
    bool ok = findGenRec( d, d, q, 0, x, result );
    cerr << endl;
    if ( ! ok )
        return 0;
    else
        return result;
}
//}}}

//{{{ void printTable ( int d, int q, CanonicalForm mipo )
//{{{ docu
//
// printTable - print +1 table of field GF(q).
//
// d: degree of extension
// q: the q in GF(q)
// mipo: the minimal polynomial of the extension
//
// p: current characteristic
//
//}}}
void
printTable ( int d, int q, CanonicalForm mipo )
{
    int i, p = getCharacteristic();

    // open file to write to
    ostrstream fname;
    fname << "gftables/gftable." << p << "." << d << '\0';
    char * fn = fname.str();
    ofstream outfile;
    outfile.open( fn, ios::out );
    STICKYASSERT1( outfile, "can not open GF(q) table %s for writing", fn );
    delete fn;

    cerr << "mipo = " << mipo << ": ";
    cerr << "generating multiplicative group ... ";
    cerr.flush();

    CanonicalForm * T = new CanonicalForm[maxtable];
    Variable x( 1 );

    // fill T with powers of x
    T[0] = 1;
    for ( i = 1; i < q; i++ )
        T[i] = ( T[i-1] * x ) % mipo;

    cerr << "generating addition table ... ";
    cerr.flush();

    // brute force method
    int * table = new int[maxtable];
    CanonicalForm f;

    for ( i = 0; i < q; i++ ) {
        f = T[i] + 1;
        int j = 0;
        while ( j < q && T[j] != f ) j++;
        table[i] = j;
    }

    cerr << "writing table ... ";
    cerr.flush();

    outfile << "@@ factory GF(q) table @@" << endl;
    outfile << p << " " << d << " " << mipo << "; ";

    // print simple reprenstation of mipo
    outfile << d;
    CFIterator MiPo = mipo;
    for ( i = d; MiPo.hasTerms(); i--, MiPo++ ) {
        int exp;
        for ( exp = MiPo.exp(); exp < i; i-- )
            outfile << " 0";
        outfile << " " << MiPo.coeff();
    }
    // since mipo is irreducible, it has a constant term,
    // so i == 0 at this point
    outfile << endl;

    int m = gf_tab_numdigits62( q );
    char * outstr = new char[30*m+1];
    outstr[30*m] = '\0';
    i = 1;
    while ( i < q ) {
        int k = 0;
        char * sptr = outstr;
        while ( i < q && k < 30 ) {
            convert62( table[i], m, sptr );
            sptr += m;
            k++; i++;
        }
        while ( k < 30 ) {
            convert62( 0, m, sptr );
            sptr += m;
            k++;
        }
        outfile << outstr << endl;
    }
    outfile.close();

    delete [] outstr;
    delete [] T;
    delete [] table;

    cerr << endl;
}
//}}}

int
main()
{
    int i, p, q, n;
    for ( i = 0; i < primes_len; i++ ) {
        p = primes[i];
        q = p*p;
        n = 2;
        setCharacteristic( p );
        while ( q < maxtable ) {
            CanonicalForm f = findGen( n, q );
            ASSERT( f != 0, "no generator found" );
            printTable( n, q, f );
            n++; q *= p;
        }
    }
}
