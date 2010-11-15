/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_factory.h"
#include "canonicalform.h"
#include "int_cf.h"
#include "int_int.h"
#include "int_rat.h"
#include "int_poly.h"
#include "int_pp.h"
#include "imm.h"

int CFFactory::currenttype = IntegerDomain;

void
CFFactory::settype ( int type )
{
    ASSERT( type==FiniteFieldDomain || type==GaloisFieldDomain || type==IntegerDomain || type==RationalDomain || type==PrimePowerDomain, "illegal basic domain!" );
    currenttype = type;
}

InternalCF *
CFFactory::basic ( int value )
{
    if ( currenttype == IntegerDomain )
        if ( value >= MINIMMEDIATE && value <= MAXIMMEDIATE )
            return int2imm( value );
        else
            return new InternalInteger( value );
//     else  if ( currenttype == RationalDomain )
//         if ( value >= MINIMMEDIATE && value <= MAXIMMEDIATE )
//             return int2imm( value );
//         else
//             return new InternalRational( value );
    else  if ( currenttype == FiniteFieldDomain )
        return int2imm_p( ff_norm( value ) );
    else  if ( currenttype == GaloisFieldDomain )
        return int2imm_gf( gf_int2gf( value ) );
    else  if ( currenttype == PrimePowerDomain )
        return new InternalPrimePower( value );
    else {
        ASSERT( 0, "illegal basic domain!" );
        return 0;
    }
}

InternalCF *
CFFactory::basic ( int type, int value )
{
    if ( type == IntegerDomain )
        if ( value >= MINIMMEDIATE && value <= MAXIMMEDIATE )
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
    else  if ( type == PrimePowerDomain )
        return new InternalPrimePower( value );
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
    else  if ( currenttype == PrimePowerDomain )
        return new InternalPrimePower( str );
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
    else  if ( currenttype == PrimePowerDomain )
        return new InternalPrimePower( str, base );
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
    else  if ( type == PrimePowerDomain )
        return new InternalPrimePower( str );
    else {
        ASSERT( 0, "illegal basic domain!" );
        return 0;
    }
}

InternalCF *
CFFactory::basic ( int type, int value, bool nonimm )
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
CFFactory::basic ( const MP_INT & num )
{
    if ( currenttype != IntegerDomain ) {
        InternalPrimePower * dummy = new InternalPrimePower( num );
        return (InternalCF*)(dummy->normalize_myself());
    }
    else
        return new InternalInteger( num );
}

InternalCF *
CFFactory::rational ( int num, int den )
{
    InternalRational * res = new InternalRational( num, den );
    return res->normalize_myself();
}

InternalCF *
CFFactory::rational ( const MP_INT & num, const MP_INT & den, bool normalize )
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
        return CFFactory::basic( 1 );
    else
        return new InternalPoly( v, exp, 1 );
}

MP_INT getmpi ( InternalCF * value, bool symmetric )
{
    ASSERT( ! is_imm( value ) && ( value->levelcoeff() == PrimePowerDomain || value->levelcoeff() == IntegerDomain ), "illegal operation" );
    MP_INT dummy;
    if ( value->levelcoeff() == IntegerDomain )
        mpz_init_set( &dummy, &InternalInteger::MPI( value ) );
    else  if ( symmetric ) {
        mpz_init( &dummy );
        if ( mpz_cmp( &InternalPrimePower::primepowhalf, &InternalPrimePower::MPI( value ) ) < 0 )
            mpz_sub( &dummy, &InternalPrimePower::MPI( value ), &InternalPrimePower::primepow );
        else
            mpz_set( &dummy, &InternalPrimePower::MPI( value ) );
    }
    else
        mpz_init_set( &dummy, &InternalPrimePower::MPI( value ) );
    return dummy;
}
