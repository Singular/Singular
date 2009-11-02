/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( degree, `"
Usage: degree [<options>] [<envSpec>] <f> [<x>]
  returns degree of canonical form <f> with respect to its main
  variable.  If variable <x> is specified, returns degree of <f>
  with respect to <x>.
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
    ftestOutVar( int, result );
    ftestInVar( CanonicalForm, f );
    ftestInVar( Variable, x );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( x, Variable() );

    // do the test!
    if ( ftestArgGiven( x ) ) {
	ftestRun( result = degree( f, x ); );
    } else {
	ftestRun( result = degree( f ); );
    }

    // print results
    if ( ftestArgGiven( x ) ) {
	ftestOutput( "degree(f, x)", result );
    } else {
	ftestOutput( "degree(f)", result );
    }

    // clean up
    ftestMainExit();
}
