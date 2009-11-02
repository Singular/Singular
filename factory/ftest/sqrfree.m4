/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( sqrfree, `"
Usage: sqrfree [<options>] [<envSpec>] [sort=<flag>] <f>
  computes squarefree decomposition of canonical form <f>.
  If optional <flag> is set, squarefree factors are returned
  sorted by multiplicity.
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
    ftestInVar( bool, flag );
    ftestInVar( CanonicalForm, f );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( flag, false, "sort" );
    ftestGetInVar( f );

    // do the test!
    ftestRun( result = sqrFree( f, flag ); );

    // print results
    if ( ftestArgGiven( flag ) && flag ) {
	ftestOutput( "sqrfree(f)", result );
    } else {
	ftestOutput( "sqrfree(f) (unsorted)", result );
    }

    // clean up
    ftestMainExit();
}
