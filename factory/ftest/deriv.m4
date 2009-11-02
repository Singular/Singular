/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( deriv, `"
Usage: deriv [<options>] [<envSpec>] <f> [<x>]
  calculates df/dx or derivation of canonical form <f> with
  respect to main variable if variable <x> is not specified.
"'`' )

//{{{ docu
//
// ftestAlgorithm.m4 - ftestAlgorithm test program.
//
// To create ftestAlgorithm.cc, run m4 using the ftest_util.m4 library in
// the following way:
//
// m4 ftest_util.m4 ftestAlgorithm.m4 > ftestAlgorithm.cc
//
//}}}

ftestPreprocInit();

ftestGlobalInit();

//
// - main program.
//
int
main ( int argc, char ** argv )
{
    // initialization
    ftestMainInit();

    // declare input and output variables
    ftestOutVar( CanonicalForm, result );
    ftestInVar( CanonicalForm, f );
    ftestInVar( Variable, x );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( x, Variable() );

    // do the test!
    if ( ftestArgGiven( x ) ) {
	ftestRun( result = f.deriv( x ); );
    } else {
	ftestRun( result = f.deriv(); );
    }

    // print results
    if ( ftestArgGiven( x ) ) {
	ftestOutput( "df/dx", result );
    } else {
	ftestOutput( "df", result );
    }

    // clean up
    ftestMainExit();
}
