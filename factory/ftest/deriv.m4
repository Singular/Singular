/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: deriv.m4,v 1.3 1997-10-15 09:19:06 schmidt Exp $ */

ftestSetNameOfGame( deriv,
        `"Usage: deriv [<options>] [<envSpec>] <f> [<x>]\n"
        "  calculates df/dx or derivation of f with respect to main\n"
	"  variable if x is not present.\n"' )

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
    ftestRun(
	if ( x.level() > 0 )
	    result = f.deriv( x );
	else
	    result = f.deriv(); );

    // print results
    if ( x.level() > 0 ) {
	ftestOutput( "df/dx", result );
    } else {
	ftestOutput( "df", result );
    }

    // clean up
    ftestMainExit();
}
