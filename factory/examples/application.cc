/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: application.cc,v 1.5 2009-10-28 14:43:02 Singular Exp $ */

//{{{ docu
//
// application.cc - sample Factory application.
//
//}}}

#include <factory.h>
#include <iostream.h>

int
main()
{
    Variable x( 'x' );
    Variable y( 'y' );
    Variable z( 'z' );
    CanonicalForm f;
    CanonicalForm g;

    cout << "Do not forget to terminate canonical forms with `;' in input!" << endl;

    // set our ring
    setCharacteristic( 0 );
    On( SW_RATIONAL );

    cout << "Simple polynomial operations in characteristic 0." << endl;
    cout << "Please enter two multivariate polynomials over Q." << endl;
    cout << "f(x, y, z) = ";
    cin >> f;
    cout << "g(x, y, z) = ";
    cin >> g;
    cout << "f                = " << f << endl;
    cout << "g                = " << g << endl;

    // call some of Factory's functions and methods and print
    // their results
    cout << "Polynomial information on f:" << endl;
    cout << "mvar(f)          = " << mvar( f ) << endl;
    cout << "degree(f)        = " << degree( f ) << endl;
    cout << "degree(f, x)     = " << degree( f, x ) << endl;
    cout << "degree(f, y)     = " << degree( f, y ) << endl;
    cout << "degree(f, z)     = " << degree( f, z ) << endl;
    cout << "LC(f, x)         = " << LC( f, x ) << endl;
    cout << "LC(f, y)         = " << LC( f, y ) << endl;
    cout << "LC(f, z)         = " << LC( f, z ) << endl;

    cout << "Arithmetic operators:" << endl;
    cout << "f+g              = " << f + g << endl;
    cout << "f-g              = " << f - g << endl;
    cout << "f*g              = " << f * g << endl;
    cout << "f/g              = " << f / g << endl;
    cout << "f%g              = " << f % g << endl;
    cout << "f(g, x)          = " << f(g, x) << endl;
}
