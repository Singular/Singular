#ifndef SI_GMP_H
#define SI_GMP_H


#ifdef __cplusplus
//  #define __cplusplus_backup __cplusplus

# ifdef DISABLE_GMP_CPP
#  undef __cplusplus
extern "C" 
{
#  include <gmp.h>
}
  #include <factory/cplusplus.h>
#else
  #include <gmp.h>
#endif
//  #define __cplusplus __cplusplus_backup
#else
  #include <gmp.h>
#endif


#endif /* SI_GMP_H */
