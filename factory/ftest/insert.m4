/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: insert.m4,v 1.1 1997-11-05 16:10:15 schmidt Exp $ */

ftestSetNameOfGame( insert, `"
Usage: insert [<options>] [<envSpec>] <f> <g> [<v>]
  inserts canonical form g for variable v in canonical form f.
  v defaults to main variable of f if not explicitely specified.
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
    ftestInVar( CanonicalForm, g );
    ftestInVar( Variable, v );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );
    ftestGetInVar( v, f.mvar() );

    // do the test!
    ftestRun(
	if ( ftestArgGiven( v ) )
	    result = f( g, v );
	else
	    result = f( g ); );

    // print results
    if ( ftestArgGiven( v ) ) {
	ftestOutput( "f(g, v)", result );
    } else {
	ftestOutput( "f(g)", result );
    }

    // clean up
    ftestMainExit();
}
