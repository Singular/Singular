/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: application.cc,v 1.2 1997-10-27 16:50:59 schmidt Exp $ */

//{{{ docu
//
// application.cc - sample Factory application.
//
//}}}

#include <factory.h>

int
main()
{
    // test for existence of GF(q) tables
    setCharacteristic( 3, 2, 'z' );
    cout << factoryVersion << endl;
    cout << factoryConfiguration << endl;
    cout << "Factory lives!" << endl;

    // now for something completely different
    setCharacteristic( 0 );
    On( SW_RATIONAL );

    Variable x( 'x' );
    Variable y( 'y' );
    Variable z( 'z' );
    CanonicalForm f;
    CanonicalForm g;

    // call some of Factory's functions and methods and print
    // their results
    cout << "Do not forget to terminate canonical forms with `;' in input!" << endl;
    cout << "Simple polynomial operations in characteristic 0." << endl;
    cout << "Please enter two multivariate polynomials over Q." << endl;

    cout << "f(x, y, z) = ";
    cin >> f;
    cout << "g(x, y, z) = ";
    cin >> g;
    cout << "f                = " << f << endl;
    cout << "g                = " << g << endl;

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
