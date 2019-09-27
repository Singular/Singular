/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file imm.h
 *
 * operations on immediates, that is elements of F_p, GF, Z, Q
 * that fit into intrinsic int, long
**/

#ifndef INCL_IMM_H
#define INCL_IMM_H

#include <stdint.h>

// #include "config.h"

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "cf_assert.h"

#include "cf_defs.h"
#include "cf_globals.h"
#include "ffops.h"
#include "gfops.h"
#include "cf_factory.h"
#include "canonicalform.h"
#include "int_cf.h"

const long INTMARK = 1;
const long FFMARK = 2;
const long GFMARK = 3;

/* define type of your compilers 64 bit integer type */
#ifndef FACTORY_INT64
#if SIZEOF_LONG == 8
#define FACTORY_INT64 long int
#else
#define FACTORY_INT64 long long int
#endif
#endif

#if SIZEOF_LONG == 4
const long MINIMMEDIATE = -268435454; // -2^28+2
const long MAXIMMEDIATE = 268435454;  // 2^28-2
#else
const long MINIMMEDIATE = -(1L<<60)+2L; // -2^60+2
const long MAXIMMEDIATE = (1L<<60)-2L;  // 2^60-2
#endif

#if defined(WINNT) && ! defined(__GNUC__)
const FACTORY_INT64 MINIMMEDIATELL = -268435454i64;
const FACTORY_INT64 MAXIMMEDIATELL = 268435454i64;
#else
const FACTORY_INT64 MINIMMEDIATELL = -268435454LL;
const FACTORY_INT64 MAXIMMEDIATELL = 268435454LL;
#endif

//{{{ conversion functions
//#ifdef HAS_ARITHMETIC_SHIFT
#if 1

static inline long imm2int ( const InternalCF * const imm )
{
    return ((intptr_t)imm) >> 2;
}

static inline InternalCF * int2imm ( long i )
{
    return (InternalCF*)((i << 2) | INTMARK );
}

#else

static inline long imm2int ( const InternalCF * const imm )
{
    // this could be better done by masking the sign bit
    if ( ((long)(intptr_t)imm)) < 0 )
        return -((-(long)(intptr_t)imm) >> 2);
    else
        return (long)(intptr_t)imm >> 2;
}

static inline InternalCF * int2imm ( long i )
{
    if ( i < 0 )
        return (InternalCF*)(-(((-i) << 2) | INTMARK));
    else
        return (InternalCF*)((i << 2) | INTMARK );
}

#endif

inline InternalCF * int2imm_p ( long i )
{
    return (InternalCF*)((i << 2) | FFMARK );
}

inline InternalCF * int2imm_gf ( long i )
{
    return (InternalCF*)((i << 2) | GFMARK );
}
//}}}

// predicates
#if 0
inline int is_imm ( const InternalCF * const ptr )
{
    // returns 0 if ptr is not immediate
    return ( (intptr_t)ptr & 3 );
}
#endif

//{{{ inline int imm_isone, imm_isone_p, imm_isone_gf ( const InternalCF * const ptr )
// docu: see CanonicalForm::isOne()
inline int
imm_isone ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 1;
}

inline int
imm_isone_p ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 1;
}

inline int
imm_isone_gf ( const InternalCF * const ptr )
{
    return gf_isone( imm2int( ptr ) );
}
//}}}

//{{{ inline int imm_iszero, imm_iszero_p, imm_iszero_gf ( const InternalCF * const ptr )
// docu: see CanonicalForm::isZero()
inline int
imm_iszero ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 0;
}

inline int
imm_iszero_p ( const InternalCF * const ptr )
{
    return imm2int( ptr ) == 0;
}

inline int
imm_iszero_gf ( const InternalCF * const ptr )
{
    return gf_iszero( imm2int( ptr ) );
}
//}}}

//{{{ conversion functions
inline long imm_intval ( const InternalCF* const op )
{
    if ( is_imm( op ) == FFMARK )
    {
        if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
            return ff_symmetric( imm2int( op ) );
        else
            return imm2int( op );
    }
    else  if ( is_imm( op ) == GFMARK )
    {
        ASSERT( gf_isff( imm2int( op ) ), "invalid conversion" );
        if ( cf_glob_switches.isOn( SW_SYMMETRIC_FF ) )
            return ff_symmetric( gf_gf2ff( imm2int( op ) ) );
        else
            return gf_gf2ff( imm2int( op ) );
    }
    /*else*/
        return imm2int( op );
}
//}}}

/**
 *
 * imm_sign() - return sign of immediate object.
 *
 * If CO is an immediate integer, the sign is defined as usual.
 * If CO is an element of FF(p) and SW_SYMMETRIC_FF is on the
 * sign of CO is the sign of the symmetric representation of CO.
 * If CO is in GF(q) or in FF(p) and SW_SYMMETRIC_FF is off, the
 * sign of CO is zero iff CO is zero, otherwise the sign is one.
 *
 * @sa CanonicalForm::sign(), gf_sign()
 *
**/
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

/**
 *
 * imm_cmp(), imm_cmp_p(), imm_cmp_gf() - compare immediate objects.
 *
 * For immediate integers, it is clear how this should be done.
 * For objects from finite fields, it is not clear since they
 * are not ordered fields.  However, since we want to have a
 * total well order on polynomials we have to define a total well
 * order on all coefficients, too. We decided to use simply the
 * order on the representation as `int's of such objects.
 *
 * @sa CanonicalForm::operator <(), CanonicalForm::operator ==()
 *
**/
inline int
imm_cmp ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( imm2int( lhs ) == imm2int( rhs ) )
        return 0;
    else  if ( imm2int( lhs ) > imm2int( rhs ) )
        return 1;
    else
        return -1;
}

inline int
imm_cmp_p ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( imm2int( lhs ) == imm2int( rhs ) )
        return 0;
    else if ( imm2int( lhs ) > imm2int( rhs ) )
        return 1;
    else
        return -1;
}

inline int
imm_cmp_gf ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    if ( imm2int( lhs ) == imm2int( rhs ) )
        return 0;
    // check is done in this way because zero should be minimal
    else if ( imm2int( lhs ) > imm2int( rhs ) )
        return -1;
    else
        return 1;
}
//}}}

//{{{ arithmetic operators
inline InternalCF * imm_add ( const InternalCF * const lhs, const InternalCF * const rhs )
{
    long result = imm2int( lhs ) + imm2int( rhs );
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
    long result = imm2int( lhs ) - imm2int( rhs );
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

inline InternalCF *
imm_mul ( InternalCF * lhs, InternalCF * rhs )
{
    long a = imm2int( lhs );
    long b = imm2int( rhs );
    int sa= 1;
    unsigned FACTORY_INT64 aa, bb;
    if (a < 0)
    {
      sa= -1;
      aa= (unsigned FACTORY_INT64) (-a);
    }
    else
      aa= (unsigned FACTORY_INT64) a;
    if (b < 0)
    {
      sa= -sa;
      bb= (unsigned FACTORY_INT64) (-b);
    }
    else
      bb= (unsigned FACTORY_INT64) b;
    unsigned FACTORY_INT64 result = aa*bb;
    #if SIZEOF_LONG == 4
    if (result>(unsigned FACTORY_INT64)MAXIMMEDIATE)
    {
        InternalCF * res = CFFactory::basic( IntegerDomain, a, true );
        return res->mulcoeff( rhs );
    }
    #else
    if ( ( a!=0L ) && ((result/aa!=bb) || (result>(unsigned FACTORY_INT64) MAXIMMEDIATE) ))
    {
        InternalCF * res = CFFactory::basic( IntegerDomain, a, true );
        return res->mulcoeff( rhs );
    }
    #endif
    else
      return int2imm( sa*result );
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
    long a = imm2int( lhs );
    long b = imm2int( rhs );
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
        long a = imm2int( lhs );
        long b = imm2int( rhs );
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
        long a = imm2int( lhs );
        long b = imm2int( rhs );
        if ( a > 0 )
            if ( b > 0 )
                return int2imm( a % b );
            else
                return int2imm( a % (-b) );
        else
            if ( b > 0 ) {
                long r = (-a) % b;
                return int2imm( (r==0) ? r : b-r );
            }
            else {
                long r = (-a) % (-b);
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
        r = CFFactory::basic( 0L );
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

//{{{ inline InternalCF * imm_neg, imm_neg_p, imm_neg_gf ( const InternalCF * const op )
// docu: see CanonicalForm::operator -()
inline InternalCF *
imm_neg ( const InternalCF * const op )
{
    return int2imm( -imm2int( op ) );
}

inline InternalCF *
imm_neg_p ( const InternalCF * const op )
{
    return int2imm_p( ff_neg( imm2int( op ) ) );
}

inline InternalCF *
imm_neg_gf ( const InternalCF * const op )
{
    return int2imm_gf( gf_neg( imm2int( op ) ) );
}
//}}}
//}}}

//{{{ input/output
#ifndef NOSTREAMIO
inline void imm_print ( OSTREAM & os, const InternalCF * const op, const char * const str )
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
