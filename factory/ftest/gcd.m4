/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: gcd.m4,v 1.3 1997-09-29 13:00:17 schmidt Exp $ */

ftestSetNameOfGame( gcd,
        `"Usage: gcd [<options>] [<envSpec>] <f> <g> [<realResult>]\n"
        "  calculates greatest common divider of canonical forms f and g.\n"
    	"  If the gcd of f and g is already known, the optional canonical\n"
    	"  form <realResult> may be used to check the result of the gcd\n"
    	"  computation.\n"' )

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
// - functions.
//

//{{{ ftestStatusT gcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & result, const CanonicalForm & realResult )
//{{{ docu
//
// gcdCheck() - check result of gcd().
//
//}}}
ftestStatusT
gcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & result, const CanonicalForm & realResult )
{
    // if realResult is given, use it to compare with result
    if ( ! realResult.isZero() )
	if ( realResult == result )
	    return Passed;
	else if ( -realResult == result )
	    return Passed;
	else
	    return Failed;

    if ( result.isZero() )
	if ( f.isZero() && g.isZero() )
	    return Passed;
	else
	    return Failed;

    if ( divides( result, f ) && divides( result, g ) )
	if ( gcd( f/result, g/result ).isOne() )
	    return Passed;
	else
	    return Failed;
    else
	return Failed;
}
//}}}

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
    ftestInVar( CanonicalForm, realResult );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );
    ftestGetInVar( realResult, 0 );

    // do the test!
    ftestRun(
	result = gcd( f, g ); );

    // do the check
    ftestCheck( gcdCheck( f, g, result, realResult ) );

    // print results
    ftestOutput( "gcd(f, g)", result );

    // clean up
    ftestMainExit();
}
