// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_factor.cc,v 1.0 1996-05-17 10:59:43 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "cf_gmp.h"

#include "assert.h"
#include "cf_defs.h"
#include "cf_globals.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "cf_factor.h"
#include "fac_berlekamp.h"
#include "fac_cantzass.h"
#include "fac_univar.h"
#ifdef MULTIFACTOR
#include "fac_multivar.h"
#endif
#include "fac_sqrfree.h"


static bool isUnivariateBaseDomain( const CanonicalForm & f )
{
    CFIterator i = f;
    bool ok = i.coeff().inBaseDomain();
    i++;
    while ( i.hasTerms() && ( ok = ok && i.coeff().inBaseDomain() ) ) i++;
    return ok;
}

#ifdef MULTIFACTOR

CFFList factorize ( const CanonicalForm & f, bool issqrfree )
{
    if ( f.inCoeffDomain() )
	return CFFList( f );
    if ( getCharacteristic() > 0 ) {
	ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
	if ( cf_glob_switches.isOn( SW_BERLEKAMP ) )
	    return FpFactorizeUnivariateB( f, issqrfree );
	else
	    return FpFactorizeUnivariateCZ( f, issqrfree );
    }
    else {
	if ( f.isUnivariate() )
	    return ZFactorizeUnivariate( f, issqrfree );
	else
	    return ZFactorizeMultivariate( f, issqrfree );
    }
}

#else

CFFList factorize ( const CanonicalForm & f, bool issqrfree )
{
    if ( f.inCoeffDomain() )
	return CFFList( f );
    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    if ( getCharacteristic() > 0 ) {
	if ( cf_glob_switches.isOn( SW_BERLEKAMP ) )
	    return FpFactorizeUnivariateB( f, issqrfree );
	else
	    return FpFactorizeUnivariateCZ( f, issqrfree );
    }
    else {
	ASSERT( isUnivariateBaseDomain( f ), "factorization in char 0 over algebraic extensions not implemented" );
	return ZFactorizeUnivariate( f, issqrfree );
    }
}

#endif

CFFList factorize ( const CanonicalForm & f, const Variable & alpha )
{
    ASSERT( alpha.level() < 0, "not an algebraic extension" );
    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    ASSERT( getCharacteristic() > 0, "char 0 factorization not implemented" );
    return FpFactorizeUnivariateCZ( f, false, 1, alpha );
}

CFFList sqrFree ( const CanonicalForm & f )
{
//    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    if ( getCharacteristic() == 0 )
	return sqrFreeZ( f );
    else
	return sqrFreeFp( f );
}

bool isSqrFree ( const CanonicalForm & f )
{
//    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    if ( getCharacteristic() == 0 )
	return isSqrFreeZ( f );
    else
	return isSqrFreeFp( f );
}
