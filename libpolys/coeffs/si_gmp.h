/* $Id$ */

#ifndef SI_GMP_H
#define SI_GMP_H

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

#endif /* ! SI_GMP_H */
