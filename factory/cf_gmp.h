#ifndef CF_GMP_H
#define CF_GMP_H


#ifdef __cplusplus
  #define __cplusplus_backup __cplusplus
  #undef __cplusplus
  extern "C" {
  #include <gmp.h>
  }
  #define __cplusplus __cplusplus_backup
#else
  #include <gmp.h>
#endif


#endif /* CF_GMP_H */
