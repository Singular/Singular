/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( size, `"
Usage: size [<options>] [<envSpec>] <f> [<x>]
  calculates number of monomials of canonical form <f>.  If
  variable <x> is specified, calculates number of monomials of
  <f> with level higher or equal than level of <x>.
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
	ftestRun( result = size( f, x ); );
    } else {
	ftestRun( result = size( f ); );
    }

    // print results
    if ( ftestArgGiven( x ) ) {
	ftestOutput( "size(f, x)", result );
    } else {
	ftestOutput( "size(f)", result );
    }

    // clean up
    ftestMainExit();
}
