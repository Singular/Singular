/*
 */

#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

#ifndef REGISTER
#ifdef __cplusplus
  #if __cplusplus >= 201402L
  /* clang 3.7, gcc 5.1 sets 201402L */
  #define REGISTER
  #elif defined(__clang__)
  #define REGISTER
  #else
  #define REGISTER register
  #endif
#else
  #define REGISTER register
#endif
#endif
