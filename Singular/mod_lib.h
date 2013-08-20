#ifndef MOD_LIB_H
#define MOD_LIB_H

// #include <config.h>         /* for EMBED_PYTHON, HAVE_MATHICGB */

#include <polys/mod_raw.h>  /* for lib_types */

lib_types type_of_LIB(char *newlib, char *fullname);

#ifdef EMBED_PYTHON
#define SI_BUILTIN_PYOBJECT(add) add(pyobject) add(syzextra)
//TODO: the line above means that syzextra should be staticly embedded IFF pyobjects do so :(
#else
#define SI_BUILTIN_PYOBJECT(add) 
#endif

#ifdef HAVE_MATHICGB
# define SI_BUILTIN_MATHIC(add) add(singmathic)
#else
# define SI_BUILTIN_MATHIC(add) 
#endif

/// Data for @c type_of_LIB to determine built-in modules,
/// use @c add(name) to add built-in library to macro
#define SI_FOREACH_BUILTIN(add)\
    add(staticdemo)\
    SI_BUILTIN_MATHIC(add)\
    SI_BUILTIN_PYOBJECT(add)

#define SI_MOD_INIT(name) name##_mod_init

#endif

