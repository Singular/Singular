/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: resultant.m4,v 1.4 1997-10-15 09:19:14 schmidt Exp $ */

ftestSetNameOfGame( resultant,
        `"Usage: resultant [<options>] [<envSpec>] <f> <g> <x>\n"
        "  calculates resultant of canonical forms f and g with\n"
        "  respect to variable x.\n"' )

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
    ftestInVar( Variable, x );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );
    ftestGetInVar( x );

    // do the test!
    ftestRun(
	result = resultant( f, g, x ); );

    // print results
    ftestOutput( "resultant(f, g, x)", result );

    // clean up
    ftestMainExit();
}
