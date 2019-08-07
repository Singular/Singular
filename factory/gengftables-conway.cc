/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file gengftables-conway.cc
 *
 * generate addition tables used by Factory
 *   to calculate in GF(q).
 *
 * @note This may take quite a while ...
 *
**/


#include "config.h"


#ifdef HAVE_IOSTREAM
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>
#else
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <string.h>
#endif


#include <stdlib.h>

#include "cf_assert.h"
#include "gf_tabutil.h"
#include "cf_algorithm.h"
#include "cf_iter.h"

using namespace std;

/**
 *
 * constants.
 *
 * maxtable: maximal size of a gf_table
 *
**/
const int maxtable = 65536;

/**
 * primes, primes_len:
 *   used to step through possible extensions
**/
const int primes_len = 54;

/**
 * primes, primes_len:
 *   used to step through possible extensions
**/
THREAD_VAR static unsigned short primes [] =
{
      2,   3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223,
        227, 229, 233, 239, 241, 251
};

/** bool isIrreducible ( const CanonicalForm & f )
 *
 * isIrreducible() - return true iff f is irreducible.
 *
**/
bool
isIrreducible ( const CanonicalForm & f )
{
    CFFList F = factorize( f );
    if (F.getFirst().factor().inCoeffDomain())
      F.removeFirst();
    return F.length() == 1 && F.getFirst().exp() == 1;
}

/** int exponent ( const CanonicalForm & f, int q )
 *
 * exponent() - return e > 0 such that x^e == 1 mod f.
 *
 * q: upper limit for e (?)
 *
**/
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

/** bool findGenRec ( int d, int n, int q, const CanonicalForm & m, const Variable & x, CanonicalForm & result )
 *
 * findGenRec() - find a generator of GF(q).
 *
 * Returns true iff result is a valid generator.
 *
 * d: degree of extension
 * q: the q in GF(q) (q == p^d)
 * x: generator should be a poly in x
 * n, m: used to step recursively through all polys in FF(p)
 *   Initially, n == d and m == 0.  If 0 <= n <= d we are
 *   in the process of building m, if n < 0 we built m and
 *   may test whether it generates GF(q).
 * result: generator found
 *
 * i: used to step through GF(p)
 * p: current characteristic
 *
**/
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

/** CanonicalForm findGen ( int d, int q )
 *
 * findGen() - find and return a generator of GF(q).
 *
 * d: degree of extension
 * q: the q in GF(q)
 *
**/
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

/** void printTable ( int d, int q, CanonicalForm mipo )
 *
 * printTable - print +1 table of field GF(q).
 *
 * d: degree of extension
 * q: the q in GF(q)
 * mipo: the minimal polynomial of the extension
 *
 * p: current characteristic
 *
**/
void
printTable ( int d, int q, CanonicalForm mipo )
{
    int i, p = getCharacteristic();

    // open file to write to
        ostrstream fname;
    fname << "gftables/" << q << '\0';
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

/**
 * The new function for getting the minimal polynomials.
 * It uses the Conway polynomials.
 * It reads the polynomials from a file.
 * The file contains all polynomials with p^k <= 2^16
 * but currently only polynomials with p^k <= 2^14 are used.
**/
static CanonicalForm findGenNew(int n, ///< n is the exponent
                                int q  ///< parameter q is not used. It is added to respect the old version
                               )
{
        CanonicalForm conway = 0;
        Variable x( 1 );
        int p = getCharacteristic();
        int ntmp,ptmp,pos1,pos2,ii;
        string ns, ps;
        string LineSe,coef,PC;
        int flag=1;
        ifstream in("./ConwayList.txt");
        getline(in,LineSe); // For the first line

        string err="END"; //to check if we are at the end of the file
        while((flag) && (err != LineSe))
        {
                getline(in,LineSe); //for the line: allConwayPolynomials := [
                if(LineSe == err){
                        break;
                }
                pos1 = LineSe.find( ",", 0 );
                pos2 = LineSe.find( ",", pos1 + 1);        // we check where are the "," to now p and n of this line
                ps = LineSe.substr(0, pos1);
                ns = LineSe.substr(pos1 + 1,pos2 - pos1);
                ptmp = atoi(ps.c_str());                //we have the value of p and n of these line
                ntmp = atoi(ns.c_str());

        if((ntmp==n)&&(ptmp==p)){flag=0;}        // we check if they are our p and n to stop the search

        }

        if (err==LineSe) // If the Conway Polynomial is not in the list, there is an error.
        {
                //cout << "Error: This Conway polinomial is not in the list" << endl;
                return(0);
        }

        // Read the polynomial from the file
        pos1 = pos2 + 1;
        pos2 = LineSe.find(",", pos1 + 1);
        conway = atoi(LineSe.substr(pos1, pos2 - pos1).c_str()); // value of the constant term in PC=Conway Polynomial
    pos1 = pos2;
        pos2 = LineSe.find(",", pos1 + 1);

        for(ii = 2; ii <= n; ii++)
        {
                coef = LineSe.substr(pos1 + 1,pos2 - pos1 - 1); //Coefficient of the monomial of degree ii-1
        if(coef != "0")
                {
                        conway = conway + atoi(coef.c_str()) * power(x, ii - 1) ; //We add this monomial to the Conway Polynomial
                }
                pos1 = pos2;
                pos2 = LineSe.find( ",", pos1+1);
        }

        pos2 = LineSe.find( ",END", pos1 + 1); // To obtain the last coefficient we search "END" instead of ","
        coef = LineSe.substr(pos1 + 1,pos2 - pos1 - 1);
        conway = conway + atoi(coef.c_str()) * power(x, ii - 1) ; //We add the last monomial to the Conway Polynomial

        in.close();

        return(conway);

}


int
main()
{
    int i, p, q, n;
    for ( i = 0; i < primes_len; i++ ) {
                p = primes[i];
                q = p;
                n = 1;
                setCharacteristic( p );
                while ( q < maxtable ) {
                        CanonicalForm f = findGenNew( n, q );
                        ASSERT( f != 0, "no generator found" );
                        printTable( n, q, f );
                        n++; q *= p;
                }
    }
}

