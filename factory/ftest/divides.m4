/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( divides, `"
Usage: divides [<options>] [<envSpec>] <f> <g>
  returns true iff canonical form <f> divides canonical form <g>.
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

//{{{ static ftestStatusT dividesCheck ( const CanonicalForm & f, const CanonicalForm & g, bool result )
//{{{ docu
//
// dividesCheck() - check result of divides().
//
//}}}
static ftestStatusT
dividesCheck ( const CanonicalForm & f, const CanonicalForm & g, bool result )
{
    // check for f == 0
    if ( f.isZero() )
	if ( (! g.isZero() || result) && (! result || g.isZero()) )
	    return Passed;
	else {
	    ftestError( CheckError, "result differs from result of check" );
	    return Failed;
	}

    // we check the result without any heuristics
    CanonicalForm q, r;
    bool checkResult = divremt( g, f, q, r );

    // by the way, check divremt(), too
    if ( checkResult ) {
	if ( q != g / f ) {
	    ftestError( CheckError, "q != g/f\n" );
	    return Failed;
	} else if ( r != g % f ) {
	    ftestError( CheckError, "r != g%f\n" );
	    return Failed;
	} else if ( g.inBaseDomain() && f.inBaseDomain() && getCharacteristic() == 0
		    && (r < 0 || r > abs( f )) ) {
	    // check euclidean division in Z
	    ftestError( CheckError, "!(0 <= g%f < abs(f))\n" );
	    return Failed;
	} else if ( g != f*q+r ) {
	    ftestError( CheckError, "g != f*q+r\n" );
	    return Failed;
	} else if ( g.inPolyDomain() || f.inPolyDomain() ) {
	    // check euclidean division in R[x]
	    Variable x = (mvar( g ) >= mvar( f )) ? mvar( g ) : mvar( f );
	    if ( r.isZero() || degree( r, x ) < degree( f, x ) )
		return Passed;
	    else {
		ftestError( CheckError, "degree(r) >= degree(f)\n" );
		return Failed;
	    }
	}
    }

    // check result now
    checkResult = checkResult && r.isZero();
    if ( (! checkResult || result) && (! result || checkResult) )
	return Passed;
    else {
	ftestError( CheckError, "result differs from result of check" );
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
    ftestOutVar( bool, result );
    ftestInVar( CanonicalForm, f );
    ftestInVar( CanonicalForm, g );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );

    // do the test!
    ftestRun( result = divides( f, g ); );

    // do the check
    ftestCheck( dividesCheck( f, g, result ); );

    // print results
    ftestOutput( "divides(f, g)", result );

    // clean up
    ftestMainExit();
}
