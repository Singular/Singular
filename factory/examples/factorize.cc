/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: factorize.cc,v 1.1 1997-10-27 16:23:41 schmidt Exp $ */

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

    cout << "Do not forget to terminate canonical forms by `;' in input!" << endl;

    setCharacteristic( 0 );
    cout << "Please enter a multivariate polynomial over Z." << endl;
    cout << "f(x, y, z) = ";
    cin >> f;

    cout << "You entered:" << endl;
    cout << "f                  = " << f << endl;

    cout << "Factoring f in charactersitic 0 ..." << endl;
    cout << "factorize(f)       = " << factorize( f ) << endl;

    setCharacteristic( 29 );
    cout << "Please enter an univariate polynomial over GF(29)." << endl;
    cout << "f(x) = ";
    cin >> f;

    cout << "You entered:" << endl;
    cout << "f (mod 29)         = " << f << endl;

    cout << "Factoring f in charactersitic 29 ..." << endl;
    cout << "factorize(f)       = " << factorize( f ) << endl;
}
