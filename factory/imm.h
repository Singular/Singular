/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: imm.h,v 1.7 1997-11-19 11:37:29 schmidt Exp $ */

#ifndef INCL_IMM_H
#define INCL_IMM_H

#include <config.h>

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

#include "assert.h"

#include "cf_defs.h"
#include "cf_globals.h"
#include "ffops.h"
#include "gfops.h"
#include "cf_factory.h"
#include "canonicalform.h"
#include "int_cf.h"

const int INTMARK = 1;
const int FFMARK = 2;
const int GFMARK = 3;

const int MINIMMEDIATE = -268435454; // -2^28-2
const int MAXIMMEDIATE = 268435454;  // 2^28-2
#ifdef macintosh
const long long int MINIMMEDIATELL = -268435454;
const long long int MAXIMMEDIATELL = 268435454;
#else
const long long int MINIMMEDIATELL = -268435454LL;
const long long int MAXIMMEDIATELL = 268435454LL;
#endif

//{{{ predicates
inline int is_imm ( const InternalCF * const ptr )
{
    // returns 0 if ptr is not immediate
    return ( (int)ptr & 3 );
}

inline int imm_iszero ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 0;
}

inline int imm_isone ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 1;
}

inline int imm_iszero_p ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 0;
}

inline int imm_isone_p ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 1;
}

inline int imm_iszero_gf ( const InternalCF * const ptr )
{
    return gf_iszero( imm2int( ptr ) );
}

inline int imm_isone_gf ( const InternalCF * const ptr )
{
    return gf_isone( imm2int( ptr ) );
}
//}}}

//{{{ conversion functions
#ifdef HAS_ARITHMETIC_SHIFT

inline int imm2int ( const InternalCF * const imm )
{
    return (int)imm >> 2;
}

inline InternalCF * int2imm ( int i )
{
    return (InternalCF*)((i << 2) | INTMARK );
}

#else

inline int imm2int ( const InternalCF * const imm )
{
    // this could be better done by masking the sign bit
    if ( (int)imm < 0 )
	return -((-(int)imm) >> 2);
    else
	return (int)imm >> 2;
}

inline InternalCF * int2imm ( int i )
{
    if ( i < 0 )
	return (InternalCF*)(-(((-i) << 2) | INTMARK));
    else
	return (InternalCF*)((i << 2) | INTMARK );
}

#endif

inline InternalCF * int2imm_p ( int i )
{
    return (InternalCF*)((i << 2) | FFMARK );
}

inline InternalCF * int2imm_gf ( int i )
{
    return (InternalCF*)((i << 2) | GFMARK );
}

inline int imm_intval ( const InternalCF* const op )
{
    if ( is_imm( op ) == FFMARK )
	if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
	    return ff_symmetric( imm2int( op ) );
	else
	    return imm2int( op );
    else  if ( is_imm( op ) == GFMARK ) {
	ASSERT( gf_isff( imm2int( op ) ), "invalid conversion" );
	if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
	    return ff_symmetric( gf_gf2ff( imm2int( op ) ) );
	else
	    return gf_gf2ff( imm2int( op ) );
    }
    else
	return imm2int( op );
}
//}}}

//{{{ inline int imm_sign ( const InternalCF * const op )
//{{{ docu
//
// imm_sign() - return sign of immediate object.
//
// If CO is an immediate integer, the sign is defined as usual.
// If CO is an element of a finite field and SW_SYMMETRIC_FF is
// on the sign of CO is the sign of the symmetric representation
// of CO.  If CO is in GF(q) or in a finite field and
// SW_SYMMETRIC_FF is off, the sign of CO is zero iff CO is zero,
// otherwise the sign is one.
//
// See alse: CanonicalForm::sign(), gf_sign()
//
//}}}
inline int
imm_sign ( const InternalCF * const op )
{
    if ( is_imm( op ) == FFMARK )
	if ( imm2int( op ) == 0 )
	    return 0;
	else  if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
	    if ( ff_symmetric( imm2int( op ) ) > 0 )
		return 1;
	    else
		return -1;
	else
	    return 1;
    else  if ( is_imm( op ) == GFMARK )
	return gf_sign( imm2int( op ) );
    else  if ( imm2int( op ) == 0 )
	return 0;
    else  if ( imm2int( op ) > 0 )
	return 1;
    else
	return -1;
}
//}}}

//{{{ comparison operators
inline int imm_cmp ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( imm2int( lhs ) == imm2int( rhs ) )
	return 0;
    else  if ( imm2int( lhs ) > imm2int( rhs ) )
	return 1;
    else
	return -1;
}

inline int imm_cmp_p ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( imm2int( lhs ) == imm2int( rhs ) )
	return 0;
    else
	return 1;
}

inline int imm_cmp_gf ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( imm2int( lhs ) == imm2int( rhs ) )
	return 0;
    else
	return 1;
}
//}}}

//{{{ arithmetic operators
inline InternalCF * imm_add ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    int result = imm2int( lhs ) + imm2int( rhs );
    if ( ( result > MAXIMMEDIATE ) || ( result < MINIMMEDIATE ) )
	return CFFactory::basic( result );
    else
	return int2imm( result );
}

inline InternalCF * imm_add_p ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_p( ff_add( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_add_gf ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_gf( gf_add( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_sub ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    int result = imm2int( lhs ) - imm2int( rhs );
    if ( ( result > MAXIMMEDIATE ) || ( result < MINIMMEDIATE ) )
	return CFFactory::basic( result );
    else
	return int2imm( result );
}

inline InternalCF * imm_sub_p ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_p( ff_sub( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_sub_gf ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_gf( gf_sub( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_mul ( InternalCF * lhs, InternalCF * rhs )
{
    long long int result = (long long int)imm2int( lhs ) * imm2int( rhs );
    if ( ( result > MAXIMMEDIATELL ) || ( result < MINIMMEDIATELL ) ) {
	InternalCF * res = CFFactory::basic( IntegerDomain, imm2int( lhs ), true );
	return res->mulcoeff( rhs );
    }
    else
	return int2imm( (int)result );
}

inline InternalCF * imm_mul_p ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_p( ff_mul( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_mul_gf ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_gf( gf_mul( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_div ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    int a = imm2int( lhs );
    int b = imm2int( rhs );
    if ( a > 0 )
	return int2imm( a / b );
    else  if ( b > 0 )
	return int2imm( -((b-a-1)/b) );
    else
	return int2imm( (-a-b-1)/(-b) );
}

inline InternalCF * imm_divrat ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( cf_glob_switches.isOn( SW_RATIONAL ) )
	return CFFactory::rational( imm2int( lhs ), imm2int( rhs ) );
    else {
	int a = imm2int( lhs );
	int b = imm2int( rhs );
	if ( a > 0 )
	    return int2imm( a / b );
	else  if ( b > 0 )
	    return int2imm( -((b-a-1)/b) );
	else
	    return int2imm( (-a-b-1)/(-b) );
    }
}

inline InternalCF * imm_div_p ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_p( ff_div( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_div_gf ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    return int2imm_gf( gf_div( imm2int( lhs ), imm2int( rhs ) ) );
}

inline InternalCF * imm_mod ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( cf_glob_switches.isOn( SW_RATIONAL ) )
	return int2imm( 0 );
    else {
	int a = imm2int( lhs );
	int b = imm2int( rhs );
	if ( a > 0 )
	    if ( b > 0 )
		return int2imm( a % b );
	    else
		return int2imm( a % (-b) );
	else
	    if ( b > 0 ) {
		int r = (-a) % b;
		return int2imm( (r==0) ? r : b-r );
	    }
	    else {
		int r = (-a) % (-b);
		return int2imm( (r==0) ? r : -b-r );
	    }
    }
}

inline InternalCF * imm_mod_p ( const InternalCF * const, const InternalCF * const )
{
    return int2imm_p( 0 );
}

inline InternalCF * imm_mod_gf ( const InternalCF * const, const InternalCF * const )
{
    return int2imm_gf( gf_q );
}

inline void imm_divrem ( const InternalCF * const lhs, const InternalCF * const rhs, InternalCF * & q, InternalCF * & r )
{
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	q = imm_divrat( lhs, rhs );
	r = CFFactory::basic( 0 );
    }
    else {
	q = imm_div( lhs, rhs );
	r = imm_mod( lhs, rhs );
    }
}

inline void imm_divrem_p ( const InternalCF * const lhs, const InternalCF * const rhs, InternalCF * & q, InternalCF * & r )
{
    q = int2imm_p( ff_div( imm2int( lhs ), imm2int( rhs ) ) );
    r = int2imm_p( 0 );
}

inline void imm_divrem_gf ( const InternalCF * const lhs, const InternalCF * const rhs, InternalCF * & q, InternalCF * & r )
{
    q = int2imm_gf( gf_div( imm2int( lhs ), imm2int( rhs ) ) );
    r = int2imm_gf( gf_q );
}

inline InternalCF * imm_neg ( const InternalCF * const op )
{
    return int2imm( -imm2int( op ) );
}

inline InternalCF * imm_neg_p ( const InternalCF * const op )
{
    return int2imm_p( ff_neg( imm2int( op ) ) );
}

inline InternalCF * imm_neg_gf ( const InternalCF * const op )
{
    return int2imm_gf( gf_neg( imm2int( op ) ) );
}
//}}}

//{{{ input/output
#ifndef NOSTREAMIO
inline void imm_print ( ostream & os, const InternalCF * const op, const char * const str )
{
    if ( is_imm( op ) == FFMARK )
	if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
	    os << ff_symmetric( imm2int( op ) ) << str;
	else
	    os << imm2int( op ) << str;
    else  if ( is_imm( op ) == GFMARK ) {
	gf_print( os, imm2int( op ) );
	os << str;
    }
    else
	os << imm2int( op ) << str;
}
#endif /* NOSTREAMIO */
//}}}

#endif /* ! INCL_IMM_H */
