#ifndef COEFFS_SI_GMP_H
#define COEFFS_SI_GMP_H


// just assume that factory have been built (cplusplus.h!)
// instead of duplicating cf_gmp.h here for now...
#include <factory/cf_gmp.h>

/*
#if defined(__cplusplus) && defined(__GNUC__)
  #define __cplusplus_backup __cplusplus
  #undef __cplusplus
  extern "C" {
  #include <gmp.h>
  }
  #define __cplusplus __cplusplus_backup
#else
  #include <gmp.h>
#endif
*/


#ifndef mpz_size1
// This is taken from longrat.h: it seems to be generally GMP-related
#define mpz_size1(A) (ABS((A)->_mp_size))
//#define mpz_size1(A) mpz_size(A)
#endif // mpz_size1


#endif /* ! COEFFS_SI_GMP_H */
