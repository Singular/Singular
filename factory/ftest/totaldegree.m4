/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: totaldegree.m4,v 1.1 1997-09-29 12:57:26 schmidt Exp $ */

ftestSetNameOfGame( totaldegree,
        `"Usage: totaldegree [<options>] [<envSpec>] <f> [<x> <y>]\n"
        "  calculates total degree of canonical form f.\n"
	"  If the variables x and y are specified, calculates total\n"
	"  degree of f with respect to the polynomials variables between\n"
        "  x and y (inclusively).\n"' )

//{{{ docu
//
// ftestAlgorithm.m4 - ftestAlgorithm test program.
//
// To create ftestAlgorithm.cc, run m4 using the ftest_util.m4 library in
// the following way:
//
// m4 ftest_util.m4 ftestAlgorithm.m4 > ftestAlgorithm.cc'
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
    ftestInVar( Variable, y );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( x, Variable() );
    ftestGetInVar( y, Variable() );

    // do the test!
    ftestRun(
	if ( ftestArgGiven( x ) )
	    result = totaldegree( f, x, y );
	else
	    result = totaldegree( f ); );

    // print results
    if ( ftestArgGiven( x ) ) {
	ftestOutput( "totaldegree(f, x, y)", result );
    } else {
	ftestOutput( "totaldegree(f)", result );
    }

    // clean up
    ftestMainExit();
}
