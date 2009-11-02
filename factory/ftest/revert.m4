/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( revert, `"
Usage: revert [<options>] [<envSpec>] <f>
  reverts the canonical form <f>.
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

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );

    // do the test!
    ftestRun( result = revert( f ); );

    ftestOutput( "revert(f)", result );

    // clean up
    ftestMainExit();
}
