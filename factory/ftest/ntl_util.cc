/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// ntl_util.cc - some utilities to connect Factory and NTL.
//
// To initialize NTL, its characteristic has to bet set using
// `setCharacteristicNTL()'.
//
// Univariate polynomials and elements from base domains are
// mapped between Factory and NTL using appropriate `operator
// <<()'.
//
// Conversion routines which map to Factory polynomials return
// the resulting polynomial in `Variable( 1 )'.
//
//}}}

#include <strstream.h>

#include <cf_assert.h>

#include "ntl_util.h"

// size of chunks for API conversion.  For safety's sake we do
// not use the full size of a long integer.
const int limbSize = 1 << 31;

//{{{ void operator << ( ... ) // Z
//{{{ docu
//
// operator <<() - convert elements from the integers.
//
//}}}
static void
internalConvert ( ZZ & nNTL, const CanonicalForm & nCF )
{
    if ( nCF.isZero() )
	nNTL = 0;
    else {
	CanonicalForm q, r;
	divrem( nCF, limbSize, q, r );
	internalConvert( nNTL, q );
	nNTL *= limbSize;
	nNTL += r.intval();
    }
}

void
operator << ( ZZ & nNTL, const CanonicalForm & nCF )
{
    ASSERT( nCF.inZ(), "type error: integer expected" );

    internalConvert( nNTL, abs( nCF ) );
    nNTL *= sign( nCF );
}


static void
internalConvert ( CanonicalForm & nCF, const ZZ & nNTL )
{
    if ( IsZero( nNTL ) )
	nCF = 0;
    else {
	ZZ q, r;
	DivRem( q, r, nNTL, limbSize );
	internalConvert( nCF, q );
	int rInt;
	rInt << r;
	nCF *= limbSize;
	nCF += rInt;
    }
}

void
operator << ( CanonicalForm & nCF, const ZZ & nNTL )
{
    ZZ absNNTL;
    abs( absNNTL, nNTL );
    internalConvert( nCF, absNNTL );
    nCF *= sign( nNTL );
}
//}}}

//{{{ void operator << ( ... ) // Z[x]
//{{{ docu
//
// operator <<() - convert elements from univariate polynomials
//   over the integers.
//
//}}}
void
operator << ( ZZX & fNTL, const CanonicalForm & fCF )
{
    ASSERT( fCF.lc().inZ() && (fCF.inBaseDomain() || fCF.isUnivariate()),
	    "type error: element from FF(p)[x] expected" );

    int i = fCF.degree();
    if ( i == -1 ) i = 0;			// beware of zero
						// polynomial !!

    // clean up old polynomial and reallocate sufficient memory to store
    // new one
    fNTL.kill();
    fNTL.SetMaxLength( i+1 );

    // fill polynomial with coefficients from fCF
    for ( CFIterator I = fCF; I.hasTerms(); I++ )
    {
	ZZ coeffNTL( 0 );
	int exp = I.exp();

	// fill gaps with zeroes
	while ( i != exp ) {
	    SetCoeff( fNTL, i, coeffNTL );
	    i--;
	}

	// map coefficient
	coeffNTL << I.coeff();
	SetCoeff( fNTL, i, coeffNTL );
	i--;
    }
    // fill trailing gaps
    while ( i >= 0 ) {
	ZZ coeffNTL( 0 );
	SetCoeff( fNTL, i, coeffNTL ); i--;
    }

}

void
operator << ( CanonicalForm & nCF, const ZZX & fNTL )
{
    nCF = 0;

    for ( int i = deg( fNTL ); i >= 0; i-- ) {
	ZZ coeffNTL;
	CanonicalForm coeffCF;

	GetCoeff( coeffNTL, fNTL, i );
	coeffCF << coeffNTL;
	nCF += coeffCF * power( Variable( 1 ), i );
    }
}
//}}}

//{{{ void operator << ( ... ) // FF(p)
//{{{ docu
//
// operator <<() - convert elements from FF(p).
//
//}}}
void
operator << ( zz_p & nNTL, const CanonicalForm & nCF )
{
    ASSERT( nCF.inFF(), "type error: element from FF(p) expected" );
    nNTL = zz_p( nCF.intval() );
}

void
operator << ( CanonicalForm & nCF, const zz_p & nNTL )
{
    nCF = CanonicalForm( rep( nNTL ) );
}
//}}}

//{{{ void operator << ( ... ) // FF(p)[x]
//{{{ docu
//
// operator <<() - convert elements from FF(p)[x].
//
//}}}
void
operator << ( zz_pX & fNTL, const CanonicalForm & fCF )
{
    ASSERT( fCF.lc().inFF() && (fCF.inBaseDomain() || fCF.isUnivariate()),
	    "type error: element from FF(p)[x] expected" );

    int i = fCF.degree();
    if ( i == -1 ) i = 0;			// beware of zero
						// polynomial !!

    // clean up old polynomial and reallocate sufficient memory to store
    // new one
    fNTL.kill();
    fNTL.SetMaxLength( i+1 );

    // fill polynomial with coefficients from fCF
    for ( CFIterator I = fCF; I.hasTerms(); I++ )
    {
	zz_p coeffNTL( 0 );
	int exp = I.exp();

	// fill gaps with zeroes
	while ( i != exp ) {
	    SetCoeff( fNTL, i, coeffNTL );
	    i--;
	}

	// map coefficient
	coeffNTL << I.coeff();
	SetCoeff( fNTL, i, coeffNTL );
	i--;
    }
    // fill trailing gaps
    while ( i >= 0 ) {
	zz_p coeffNTL( 0 );
	SetCoeff( fNTL, i, coeffNTL ); i--;
    }

}

void
operator << ( CanonicalForm & nCF, const zz_pX & fNTL )
{
    nCF = 0;

    for ( int i = deg( fNTL ); i >= 0; i-- ) {
	zz_p coeffNTL;
	CanonicalForm coeffCF;

	GetCoeff( coeffNTL, fNTL, i );
	coeffCF << coeffNTL;
	nCF += coeffCF * power( Variable( 1 ), i );
    }
}
//}}}

//{{{ void setCharacteristicNTL ( int p )
//{{{ docu
//
// setCharacteristicNTL() - set NTL's characteristic to `p'.
//
//}}}
void
setCharacteristicNTL ( int p )
{
    if ( p > 0 ) zz_pInit( p );
}
//}}}
