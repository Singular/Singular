/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( bgcd, `"
Usage: bgcd [<options>] [<envSpec>] <f> <g> [<realResult>]
  calculates the positive greatest common divider of canonical
  forms f and g.  f and g should be from a base domain.

  If the gcd of f and g is already known, the optional canonical
  form realResult may be used to check the result of the gcd
  computation.
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
// - functions.
//

//{{{ ftestStatusT bgcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & result, const CanonicalForm & realResult )
//{{{ docu
//
// bgcdCheck() - check result of bgcd().
//
//}}}
ftestStatusT
bgcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & result, const CanonicalForm & realResult )
{
    // use old gcd to compare results
    if ( result != gcd( f, g ) ) {
	ftestError( CheckError, "new and old gcd differ\n" );
	return Failed;
    }

    // if realResult is given, use it to compare with result
    if ( ! realResult.isZero() )
	if ( realResult == result )
	    return Passed;
	else if ( -realResult == result )
	    return Passed;
	else {
	    ftestError( CheckError, "result and real result differ\n" );
	    return Failed;
	}

    if ( result.isZero() )
	if ( f.isZero() && g.isZero() )
	    return Passed;
	else {
	    ftestError( CheckError, "result is zero but f and g are not\n" );
	    return Failed;
	}

    if ( divides( result, f ) && divides( result, g ) )
	if ( bgcd( f/result, g/result ).isOne() )
	    return Passed;
	else {
	    ftestError( CheckError, "result is not greatest common divisor\n" );
	    return Failed;
	}
    else {
	ftestError( CheckError, "result is not a common divisor\n" );
	return Failed;
    }
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
	result = bgcd( f, g ); );

    // do the check
    ftestCheck(
	bgcdCheck( f, g, result, realResult ) );

    // print results
    ftestOutput( "bgcd(f, g)", result );

    // clean up
    ftestMainExit();
}
