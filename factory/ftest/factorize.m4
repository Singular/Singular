/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( factorize, `"
Usage: factorize [<options>] [<envSpec>] <f> [<v>]
  factorizes canonical form <f>.
  If algebraic variable <v> is specified, factorizes in
  algebraic extension by <v>.
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
    ftestInVar( Variable, v );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( v, Variable() );

    // do the test!
    if ( ftestArgGiven( v ) ) {
	ftestRun( result = factorize( f, v ); );
    } else {
	ftestRun( result = factorize( f ); );
    }

    // print results
    if ( ftestArgGiven( v ) ) {
	ftestOutput( "factorize(f, v)", result );
    } else {
	ftestOutput( "factorize(f)", result );
    }

    // clean up
    ftestMainExit();
}
