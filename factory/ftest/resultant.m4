/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: resultant.m4,v 1.1 1997-09-12 12:26:17 schmidt Exp $ */

//{{{ docu
//
// resultant.m4 - resultant test program.
//
// Syntax: type `resultant -?' for more information
//
//}}}

ftestPreprocInit();

#include <factory.h>

#include "ftest_util.h"

ftestGlobalInit();

//
// - functions.
//

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
    ftestInVar( Variable, x );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );
    ftestGetInVar( x );

    ftestSetEnv();

    // do the test!
    ftestRun(
	result = resultant( f, g, x ); );

    // print results
    ftestOutput( "resultant", "resultant( f, g, x ) =", result );

    // clean up
    ftestMainExit();
}
