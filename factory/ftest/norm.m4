/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( norm, `"
Usage: norm [<options>] [<envSpec>] <f> [<euclideanFlag>]
  returns maximum norm of canonical form <f>.  If integer
  <euclideanFlag> is specified and non-zero, the euclidean
  norm of <f> is returned instead.
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
    ftestInVar( int, euclideanFlag );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( euclideanFlag, 0 );

    // do the test!
    if ( euclideanFlag ) {
	ftestRun( result = euclideanNorm( f ); );
    } else {
	ftestRun( result = maxNorm( f ); );
    }

    // print results
    if ( euclideanFlag ) {
	ftestOutput( "euclideanNorm(f)", result );
    } else {
	ftestOutput( "maxNorm(f)", result );
    }

    // clean up
    ftestMainExit();
}
