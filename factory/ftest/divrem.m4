/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

ftestSetNameOfGame( divrem, `"
Usage: divrem [<options>] [<envSpec>] <f> <g> [<divisionCheck>]
  calculates quotient q and remainder r of division from
  canonical form <f> by canonical form <g> such that f=g*q+r.
  <g> should not equal zero.  The result is not defined if such q
  and r do not exist.
  If the optional integer <divisionCheck> is specified and non-
  zero additional checks are done whether q and r exist.  The
  result of the check is printed as a boolean.
  The results are printed in the order q - r [ - checkResult ].
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

//{{{ static ftestStatusT divremCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & q, const CanonicalForm & r, bool checkResult )
//{{{ docu
//
// divremCheck() - check result of divrem().
//
//}}}
static ftestStatusT
divremCheck ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm & q, const CanonicalForm & r, bool checkResult )
{
    if ( checkResult )
	if ( q != f / g ) {
	    ftestError( CheckError, "q != f/g\n" );
	    return Failed;
	} else if ( r != f % g ) {
	    ftestError( CheckError, "r != f%%g\n" );
	    return Failed;
	} else if ( f.inBaseDomain() && g.inBaseDomain() && getCharacteristic() == 0
		    && (r < 0 || r > abs( g )) ) {
	    // check euclidean division in Z
	    ftestError( CheckError, "!(0 <= f%%g < abs(g))\n" );
	    return Failed;
	} else if ( f != g*q+r ) {
	    ftestError( CheckError, "f != g*q+r\n" );
	    return Failed;
	} else if ( f.inPolyDomain() || g.inPolyDomain() ) {
	    // check euclidean division in R[x]
	    Variable x = (mvar( f ) >= mvar( g )) ? mvar( f ) : mvar( g );
	    if ( r.isZero() || degree( r, x ) < degree( g, x ) )
		return Passed;
	    else {
		ftestError( CheckError, "degree(r) >= degree(g)\n" );
		return Failed;
	    }
	} else
	    return Passed;
    else
	return UndefinedResult;
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
    ftestOutVar( CanonicalForm, q );
    ftestOutVar( CanonicalForm, r );
    ftestOutVar( bool, checkResult );
    ftestInVar( CanonicalForm, f );
    ftestInVar( CanonicalForm, g );
    ftestInVar( int, divisionCheck );

    // process argument list and set environment
    ftestGetOpts();
    ftestGetEnv();
    ftestGetInVar( f );
    ftestGetInVar( g );
    ftestGetInVar( divisionCheck, 0 );

    // do the test!
    if ( divisionCheck ) {
	ftestRun( checkResult = divremt( f, g, q, r ); );
    } else {
	checkResult = true;
	ftestRun( divrem( f, g, q, r ); );
    }

    // do the check
    ftestCheck( divremCheck( f, g, q, r, checkResult ); );

    // print results
    if ( divisionCheck ) {
	ftestOutput( "q", q, "r", r, "divremt(f, g, q, r)", checkResult );
    } else {
	ftestOutput( "q", q, "r", r );
    }

    // clean up
    ftestMainExit();
}
