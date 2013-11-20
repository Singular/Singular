#ifndef MOD_LIB_H
#define MOD_LIB_H

// #include config.h
#include <kernel/mod2.h> /* for EMBED_PYTHON, HAVE_MATHICGB */

#include <polys/mod_raw.h>  /* for lib_types */

lib_types type_of_LIB(const char *newlib, char *fullname);


#if HAVE_GFANLIB
#define SI_BUILTIN_GFANLIB(add) add(gfanlib)
#else
#define SI_BUILTIN_GFANLIB(add)
#endif


#ifdef EMBED_PYTHON
#define SI_BUILTIN_PYOBJECT(add) add(pyobject)  add(syzextra)      SI_BUILTIN_GFANLIB(add)
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

