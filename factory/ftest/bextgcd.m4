/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( bextgcd, `"
Usage: bextgcd [<options>] [<envSpec>] <f> <g> [<realResult>]
  calculates the positive greatest common divider c of canonical
  forms f and g and CanonicalForms a and b such that f*a + b*c = c.
  f and g should be from a base domain.
  The results ar printed in the order a - b - c.

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

//{{{ ftestStatusT bextgcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & a, const CanonicalForm & b, const CanonicalForm & result, const CanonicalForm & realResult )
//{{{ docu
//
// bextgcdCheck() - check result of bextgcd().
//
//}}}
ftestStatusT
bextgcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & a, const CanonicalForm & b, const CanonicalForm & result, const CanonicalForm & realResult )
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

    if ( f*a + g*b != result ) {
	ftestError( CheckError, "f*a + g*b != c\n" );
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
    ftestOutVar( CanonicalForm, a );
    ftestOutVar( CanonicalForm, b );
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
	result = bextgcd( f, g, a, b ); );

    // do the check
    ftestCheck(
	bextgcdCheck( f, g, a, b, result, realResult ) );

    // print results
    ftestOutput( "a", a, "b", b, "bextgcd(f, g, a, b)", result );

    // clean up
    ftestMainExit();
}
