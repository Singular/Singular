/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// factorize.cc - sample Factory application.
//
//}}}

#include <factory.h>

int
main()
{
    Variable x( 'x' );
    Variable y( 'y' );
    Variable z( 'z' );
    CanonicalForm f;

    cout << "Do not forget to terminate canonical forms with `;' in input!" << endl;

    setCharacteristic( 0 );
    cout << "Multivariate factorization in characteristic 0." << endl;
    cout << "Please enter a multivariate polynomial over Z." << endl;
    cout << "f(x, y, z) = ";
    cin >> f;
    cout << "f                  = " << f << endl;
    cout << "factorize(f)       = " << factorize( f ) << endl << endl;

    setCharacteristic( 29 );
    cout << "Univariate factorization in GF(29)." << endl;
    cout << "Please enter an univariate polynomial over GF(29)." << endl;
    cout << "f(x) = ";
    cin >> f;
    cout << "f (mod 29)         = " << f << endl;
    cout << "factorize(f)       = " << factorize( f ) << endl;
}
