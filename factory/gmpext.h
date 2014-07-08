/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_GMPEXT_H
#define INCL_GMPEXT_H

/**
 * @file gmpext.h
 *
 * utility functions for gmp
**/

// #include "config.h"
// #include <factory/cf_gmp.h>

#include "cf_defs.h"
#include "imm.h"


inline bool
mpz_is_imm( const mpz_t mpi )
{
    return ( mpz_cmp_si( mpi, MINIMMEDIATE ) >= 0 ) &&
	( mpz_cmp_si( mpi, MAXIMMEDIATE ) <= 0 );
}

#endif /* ! INCL_GMPEXT_H */
