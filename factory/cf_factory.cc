/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "cf_factory.h"
#include "canonicalform.h"
#include "int_cf.h"
#include "int_int.h"
#include "int_rat.h"
#include "int_poly.h"
#include "int_pp.h"
#include "imm.h"

/// For optimizing if-branches
#ifdef __GNUC__
#define LIKELY(expression) (__builtin_expect(!!(expression), 1))
#define UNLIKELY(expression) (__builtin_expect(!!(expression), 0))
#else
#define LIKELY(expression) (expression)
#define UNLIKELY(expression) (expression)
#endif

int CFFactory::currenttype = IntegerDomain;

InternalCF *
CFFactory::basic ( long value )
{
  switch(currenttype)
  {
    case IntegerDomain:
        if (LIKELY( value >= MINIMMEDIATE && value <= MAXIMMEDIATE ))
            return int2imm( value );
        else
            return new InternalInteger( value );
//     else  if ( currenttype == RationalDomain )
//         if ( value >= MINIMMEDIATE && value <= MAXIMMEDIATE )
//             return int2imm( value );
//         else
//             return new InternalRational( value );
    case FiniteFieldDomain:
        return int2imm_p( ff_norm( value ) );
    case GaloisFieldDomain:
        return int2imm_gf( gf_int2gf( value ) );
    case PrimePowerDomain:
        return new InternalPrimePower( value );
    default: {
        ASSERT( 0, "illegal basic domain!" );
        return 0;
    }
  }
}

InternalCF *
CFFactory::basic ( int type, long value )
{
    if ( type == IntegerDomain )
        if (LIKELY( value >= MINIMMEDIATE && value <= MAXIMMEDIATE ))
            return int2imm( value );
        else
            return new InternalInteger( value );
//     else  if ( type == RationalDomain )
//         if ( value >= MINIMMEDIATE && value <= MAXIMMEDIATE )
//             return int2imm( value );
//         else
//             return new InternalRational( value );
    else  if ( type == FiniteFieldDomain )
        return int2imm_p( ff_norm( value ) );
    else  if ( type == GaloisFieldDomain )
        return int2imm_gf( gf_int2gf( value ) );
    else {
        ASSERT1( 0, "illegal basic domain (type = %d)!", type );
        return 0;
    }
}

InternalCF *
CFFactory::basic ( const char * str )
{
    if ( currenttype == IntegerDomain ) {
        InternalInteger * dummy = new InternalInteger( str );
        if ( dummy->is_imm() ) {
            InternalCF * res = int2imm( dummy->intval() );
            delete dummy;
            return res;
        }
        else
            return dummy;
    }
//     else  if ( currenttype == RationalDomain ) {
//         InternalRational * dummy = new InternalRational( str );
//         if ( dummy->is_imm() ) {
//             InternalCF * res = int2imm( dummy->intval() );
//             delete dummy;
//             return res;
//         }
//         else
//             return dummy;
//     }
    else  if ( currenttype == FiniteFieldDomain ) {
        InternalInteger * dummy = new InternalInteger( str );
        InternalCF * res = int2imm_p( dummy->intmod( ff_prime ) );
        delete dummy;
        return res;
    }
    else  if ( currenttype == GaloisFieldDomain ) {
        InternalInteger * dummy = new InternalInteger( str );
        InternalCF * res = int2imm_gf( gf_int2gf( dummy->intmod( ff_prime ) ) );
        delete dummy;
        return res;
    }
    else {
        ASSERT( 0, "illegal basic domain!" );
        return 0;
    }
}

InternalCF *
CFFactory::basic ( const char * str, int base )
{
    if ( currenttype == IntegerDomain ) {
        InternalInteger * dummy = new InternalInteger( str, base );
        if ( dummy->is_imm() ) {
            InternalCF * res = int2imm( dummy->intval() );
            delete dummy;
            return res;
        }
        else
            return dummy;
    }
//     else  if ( currenttype == RationalDomain ) {
//         InternalRational * dummy = new InternalRational( str );
//         if ( dummy->is_imm() ) {
//             InternalCF * res = int2imm( dummy->intval() );
//             delete dummy;
//             return res;
//         }
//         else
//             return dummy;
//     }
    else  if ( currenttype == FiniteFieldDomain ) {
        InternalInteger * dummy = new InternalInteger( str, base );
        InternalCF * res = int2imm_p( dummy->intmod( ff_prime ) );
        delete dummy;
        return res;
    }
    else  if ( currenttype == GaloisFieldDomain ) {
        InternalInteger * dummy = new InternalInteger( str, base );
        InternalCF * res = int2imm_gf( gf_int2gf( dummy->intmod( ff_prime ) ) );
        delete dummy;
        return res;
    }
    else {
        ASSERT( 0, "illegal basic domain!" );
        return 0;
    }
}

InternalCF *
CFFactory::basic ( int type, const char * const str )
{
    if ( type == IntegerDomain ) {
        InternalInteger * dummy = new InternalInteger( str );
        if ( dummy->is_imm() ) {
            InternalCF * res = int2imm( dummy->intval() );
            delete dummy;
            return res;
        }
        else
            return dummy;
    }
//     else  if ( type == RationalDomain ) {
//         InternalRational * dummy = new InternalRational( str );
//         if ( dummy->is_imm() ) {
//             InternalCF * res = int2imm( dummy->intval() );
//             delete dummy;
//             return res;
//         }
//         else
//             return dummy;
//     }
    else  if ( type == FiniteFieldDomain ) {
        InternalInteger * dummy = new InternalInteger( str );
        InternalCF * res = int2imm( dummy->intmod( ff_prime ) );
        delete dummy;
        return res;
    }
    else  if ( type == GaloisFieldDomain ) {
        InternalInteger * dummy = new InternalInteger( str );
        InternalCF * res = int2imm_gf( gf_int2gf( dummy->intmod( ff_prime ) ) );
        delete dummy;
        return res;
    }
    else {
        ASSERT( 0, "illegal basic domain!" );
        return 0;
    }
}

InternalCF *
CFFactory::basic ( int type, long value, bool nonimm )
{
    if ( nonimm )
        if ( type == IntegerDomain )
            return new InternalInteger( value );
         else  if ( type == RationalDomain )
             return new InternalRational( value );
        else {
            ASSERT( 0, "illegal basic domain!" );
            return 0;
        }
    else
        return CFFactory::basic( type, value );
}

InternalCF *
CFFactory::basic ( const mpz_ptr num )
{
  ASSERT (currenttype == IntegerDomain, "Integer domain expected");
  return new InternalInteger( num );
}

InternalCF *
CFFactory::rational ( long num, long den )
{
    InternalRational * res = new InternalRational( num, den );
    return res->normalize_myself();
}

InternalCF *
CFFactory::rational ( const mpz_ptr num, const mpz_ptr den, bool normalize )
{
    if ( normalize ) {
        InternalRational * result = new InternalRational( num, den );
        return result->normalize_myself();
    }
    else
        return new InternalRational( num, den );
}

InternalCF *
CFFactory::poly (  const Variable & v, int exp, const CanonicalForm & c )
{
    if ( v.level() == LEVELBASE )
        return c.getval();
    else
        return new InternalPoly( v, exp, c );
}

InternalCF *
CFFactory::poly ( const Variable & v, int exp )
{
    if ( v.level() == LEVELBASE )
        return CFFactory::basic( 1L );
    else
        return new InternalPoly( v, exp, 1 );
}

void getmpi ( InternalCF * value, mpz_t mpi)
{
    ASSERT( ! is_imm( value ) && (value->levelcoeff() == IntegerDomain || value->levelcoeff() == PrimePowerDomain), "illegal operation" );
    mpz_init_set (mpi, ((InternalInteger*)value)->thempi);
}
