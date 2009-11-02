/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// gcd.cc - sample Factory application.
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
    CanonicalForm g;

    cout << "Do not forget to terminate canonical forms with `;' in input!" << endl;

    setCharacteristic( 0 );
    On( SW_USE_EZGCD );
    On( SW_RATIONAL );
    cout << "Multivariate gcd calculation in characteristic 0." << endl;
    cout << "Please enter two multivariate polynomials over Q." << endl;
    cout << "f(x, y, z) = ";
    cin >> f;
    cout << "g(x, y, z) = ";
    cin >> g;
    cout << "f                  = " << f << endl;
    cout << "g                  = " << g << endl;
    cout << "gcd(f, g)          = " << gcd( f, g ) << endl << endl;

    setCharacteristic( 29 );
    cout << "Multivariate gcd calculation in GF(29)." << endl;
    cout << "Please enter two multivariate polynomials over GF(29)." << endl;
    cout << "f(x, y, z) = ";
    cin >> f;
    cout << "g(x, y, z) = ";
    cin >> g;
    cout << "f (mod 29)         = " << f << endl;
    cout << "g (mod 29)         = " << g << endl;
    cout << "gcd(f, g) (mod 29) = " << gcd( f, g ) << endl;
}
