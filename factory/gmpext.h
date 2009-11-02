/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_GMPEXT_H
#define INCL_GMPEXT_H

#include <config.h>

#include "cf_gmp.h"

#include "cf_defs.h"
#include "imm.h"


inline bool
mpz_is_imm( const MP_INT * mpi )
{
    return ( mpz_cmp_si( mpi, MINIMMEDIATE ) >= 0 ) &&
	( mpz_cmp_si( mpi, MAXIMMEDIATE ) <= 0 );
}

inline void
mpz_mypow_ui( MP_INT * result, int p, int n )
{
    if ( p == 0 )
	mpz_set_ui( result, 0 );
    else  if ( n == 0 )
	mpz_set_ui( result, 1 );
    else {
	mpz_set_ui( result, p );
	n--;
	while ( n > 0 ) {
	    mpz_mul_ui( result, result, p );
	    n--;
	}
    }
}

inline void
mpz_mypow( MP_INT * result, MP_INT * p, int n )
{
    // p == 0 && n == 0 should not happen since this function is only used
    // in the factorizing algorithm
    mpz_set( result, p );
    n--;
    while ( n > 0 ) {
	mpz_mul( result, result, p );
	n--;
    }
}

#endif /* ! INCL_GMPEXT_H */
