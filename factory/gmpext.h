/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_GMPEXT_H
#define INCL_GMPEXT_H

/**
 * @file gmpext.h
 *
 * utility functions for gmp
**/

// #include "config.h"

#include "cf_defs.h"
#include "imm.h"


inline bool
mpz_is_imm( const mpz_t mpi )
{
  if (mpi->_mp_size==0)
    return true;
  if ((mpi->_mp_size>1)||(mpi->_mp_size< -1))
    return false;
  return ( mpz_cmp_si( mpi, MINIMMEDIATE ) >= 0 ) &&
        ( mpz_cmp_si( mpi, MAXIMMEDIATE ) <= 0 );
}

#endif /* ! INCL_GMPEXT_H */
