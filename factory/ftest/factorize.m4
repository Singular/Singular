/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: factorize.m4,v 1.1 1997-11-05 16:11:06 schmidt Exp $ */

ftestSetNameOfGame( factorize, `"
Usage: factorize [<options>] [<envSpec>] <f>
  factorizes canonical form f.
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
    ftestOutVar( CFFList, result );
    ftestInVar( CanonicalForm, f );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );

    // do the test!
    ftestRun(
	result = factorize( f ); );

    // print results
    ftestOutput( "factorize(f)", result );

    // clean up
    ftestMainExit();
}
