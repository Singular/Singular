/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: gcd.m4,v 1.1 1997-09-12 12:25:31 schmidt Exp $ */

//{{{ docu
//
// gcd.m4 - gcd test program.
//
// Syntax: type `gcd -?' for more information
//
//}}}

ftestPreprocInit();

#include <factory.h>

#include "ftest_util.h"

ftestGlobalInit();

//
// - functions.
//

//{{{ int gcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & result )
//{{{ docu
//
// gcdCheck() - check result of gcd().
//
//}}}
int
gcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & result )
{
    return Passed;
}
//}}}

//
// - main program.
//
int
main ( int argc, char ** argv )
{
    ftestMainInit();

    // declare input and output variables
    ftestOutVar( CanonicalForm, result );
    ftestInVar( CanonicalForm, f );
    ftestInVar( CanonicalForm, g );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );

    ftestSetEnv();

    // do the test!
    ftestRun(
	result = gcd( f, g ); );

    // do the check
    ftestCheck( gcdCheck( f, g, result ) );

    // print results
    ftestOutput( "gcd", "gcd(f, g) =", result );

    // clean up
    ftestMainExit();
}
