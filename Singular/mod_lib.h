#ifndef MOD_LIB_H
#define MOD_LIB_H

#define SI_MOD_INIT0(name) name##_mod_init

#ifdef STATIC_VERSION
#  define SI_MOD_INIT(name) SI_MOD_INIT0(name)
#elif defined(DYNAMIC_VERSION)
#  define SI_MOD_INIT(name) mod_init
#endif

// Note that STATIC_VERSION and DYNAMIC_VERSION should not be defined in the following config header mod2.h!
#include "kernel/mod2.h" /*!
!
 for SI_BUILTINMODULES_ADD 

*/

/// Data for @c type_of_LIB to determine built-in modules,
/// use @c add(name) to add built-in library to macro
#define SI_FOREACH_BUILTIN(add) SI_BUILTINMODULES_ADD(add)

#include "polys/mod_raw.h"  /*!
!
 for lib_types 

*/
lib_types type_of_LIB(const char *newlib, char *fullname);

#endif

/*!
!

#if HAVE_GFANLIB
#define SI_BUILTIN_GFANLIB(add) add(gfanlib)
#endif
#ifdef HAVE_MATHICGB
# define SI_BUILTIN_MATHIC(add) add(singmathic)
#endif
#ifdef EMBED_PYTHON
//TODO: the line above means that syzextra should be staticly embedded IFF pyobjects do so :(((((
#define SI_BUILTIN_PYOBJECT(add) add(pyobject) add(syzextra) SI_BUILTIN_GFANLIB(add) SI_BUILTIN_MATHIC(add)
#endif


*/
