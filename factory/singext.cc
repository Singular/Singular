/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#include "cf_assert.h"

#include "cf_defs.h"
#include "singext.h"
#include "int_cf.h"
#include "int_int.h"
#include "int_rat.h"
#include "imm.h"
#include "cf_factory.h"

#include "cf_gmp.h"


void gmp_numerator ( const CanonicalForm & f, mpz_ptr result )
{
    InternalCF * ff = f.getval();
    ASSERT( ! is_imm( ff ), "illegal type" );
    if ( ff->levelcoeff() == IntegerDomain )
    {
        mpz_init_set( result, &(InternalInteger::MPI( ff )) );
        ff->deleteObject();
    }
    else  if ( ff->levelcoeff() == RationalDomain )
    {
        mpz_init_set( result, &(InternalRational::MPQNUM( ff )) );
        ff->deleteObject();
    }
    else
    {
        ASSERT( 0, "illegal type" );
    }
}

void gmp_denominator ( const CanonicalForm & f, mpz_ptr result )
{
    InternalCF * ff = f.getval();
    ASSERT( ! is_imm( ff ), "illegal type" );
    if ( ff->levelcoeff() == IntegerDomain )
    {
        mpz_init_set_si( result, 1 );
        ff->deleteObject();
    }
    else  if ( ff->levelcoeff() == RationalDomain )
    {
        mpz_init_set( result, &(InternalRational::MPQDEN( ff )) );
        ff->deleteObject();
    }
    else
    {
        ASSERT( 0, "illegal type" );
    }
}

int gf_value (const CanonicalForm & f )
{
    InternalCF * ff = f.getval();
    return ((long)ff) >>2;
}

CanonicalForm
make_cf ( const MP_INT & n )
{
    return CanonicalForm( CFFactory::basic( n ) );
}

CanonicalForm
make_cf ( const MP_INT & n, const MP_INT & d, bool normalize )
{
    return CanonicalForm( CFFactory::rational( n, d, normalize ) );
}

CanonicalForm make_cf_from_gf ( const int z )
{
    return CanonicalForm(int2imm_gf(z));
}
