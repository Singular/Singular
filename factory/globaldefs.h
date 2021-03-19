#ifndef _SINGULAR_GLOBALDEFS_H
#define _SINGULAR_GLOBALDEFS_H

#ifndef PSINGULAR
#define VAR
#define EXTERN_VAR extern
#define STATIC_VAR static
#define INST_VAR
#define EXTERN_INST_VAR extern
#define STATIC_INST_VAR static
#define GLOBAL_VAR
#define THREAD_VAR
#else
#define GLOBAL_VAR
#define THREAD_VAR __thread
#endif

#if defined(__CYGWIN__)
  #ifdef FACTORY_BUILDING_DLL
    #define FACTORY_PUBLIC __declspec(dllexport)
  #else
    #define FACTORY_PUBLIC __declspec(dllimport)
  #endif
#else
  #define FACTORY_PUBLIC
#endif

#endif // _SINGULAR_GLOBALDEFS_H
