/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( extgcd, `"
Usage: extgcd [<options>] [<envSpec>] <f> <g> [<realResult>]
  calculates greatest common divisor d of canonical forms <f> and
  <g> and canonical forms a and b such that d = a*<f>+b*<g>.  If
  the gcd of <f> and <g> is already known, the optional canonical
  form <realResult> may be used to check the result of the gcd
  computation.
  The extended gcd may be calculated in Euclidean domains only.
  The results are printed in the order a - b - d.
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

//{{{ static CanonicalForm normalizeGcd ( const CanonicalForm & f )
//{{{ docu
//
// normalizeGcd() - normalize result of gcd computation for
//   testing.
//
// Unit normalization is done in case of a field, sign
// normalization otherwise.
//
//}}}
static CanonicalForm
normalizeGcd ( const CanonicalForm & f )
{
    if ( getCharacteristic() > 0 || isOn( SW_RATIONAL ) )
	return f/Lc( f );
    else
	return abs( f );
}
//}}}

//{{{ static ftestStatusT extgcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & a, const CanonicalForm & b, const CanonicalForm & result, const CanonicalForm & realResult )
//{{{ docu
//
// extgcdCheck() - check result of extgcd().
//
//}}}
static ftestStatusT
extgcdCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & a, const CanonicalForm & b, const CanonicalForm & result, const CanonicalForm & realResult )
{
    // check whether result == a*f+b*g
    if ( result != a*f+b*g ) {
	ftestError( CheckError, "gcd != a*f+b*g\n" );
	return Failed;
    }

    // if realResult is given, use it to compare with result
    if ( ! realResult.isZero() )
	if ( normalizeGcd( realResult ) == normalizeGcd( result ) )
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

    if ( ! divides( result, f ) || ! divides( result, g ) ) {
	ftestError( CheckError, "result is not a common divisor\n" );
	return Failed;
    } else if ( ! gcd( f/result, g/result ).isOne() ) {
	ftestError( CheckError, "result is not greatest common divisor\n" );
	return Failed;
    } else
	return Passed;
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
    ftestRun( result = extgcd( f, g, a, b ); );

    // do the check
    ftestCheck( extgcdCheck( f, g, a, b, result, realResult ); );

    // print results
    ftestOutput( "a", a, "b", b, "extgcd(f, g, a, b)", result );

    // clean up
    ftestMainExit();
}
