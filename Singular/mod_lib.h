#ifndef MOD_LIB_H
#define MOD_LIB_H

#include <config.h>         /* for EMBED_PYTHON */
#include <polys/mod_raw.h>  /* for lib_types */


#ifdef EMBED_PYTHON
#define SI_BUILTIN_PYOBJECT(add) add(pyobject)
#else
#define SI_BUILTIN_PYOBJECT(add) 
#endif
lib_types type_of_LIB(char *newlib, char *fullname);

/// Data for @c type_of_LIB to determine built-in modules,
/// use @c add(name) to add built-in library to macro
#define SI_FOREACH_BUILTIN(add)\
  add(staticdemo)\
  SI_BUILTIN_PYOBJECT(add)

#define SI_MOD_INIT(name) name##_mod_init

#endif
