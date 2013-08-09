
#ifndef MOD_RAW_H
#define MOD_RAW_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_check_opened()
 *           dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dunl_close()
*/

typedef enum { LT_NONE, LT_NOTFOUND, LT_SINGULAR, LT_ELF, LT_HPUX, LT_MACH_O, LT_BUILTIN} lib_types;

lib_types type_of_LIB(char *newlib, char *fullname);

#if defined(HAVE_DL)
#ifdef __cplusplus
void* dynl_open_binary_warn(const char* binary_name, const char* msg = NULL );
void* dynl_sym_warn(void* handle, const char* proc, const char* msg = NULL );
#endif

#ifdef __cplusplus
extern "C" {
#endif
int          dynl_check_opened(char* filename);
void *       dynl_open(char *filename);
// if handle == DYNL_KERNEL_HANDLE, then symbol is searched for
// in kernel of program
#define DYNL_KERNEL_HANDLE ((void*) 0x1)
void *       dynl_sym(void *handle, const char *symbol);
int          dynl_close (void *handle);
const char * dynl_error();

#ifdef __cplusplus
}
#endif

#endif /* HAVE_DL */


#ifdef EMBED_PYTHON
#define SI_BUILTIN_PYOBJECT(add) add(pyobject)
#else
#define SI_BUILTIN_PYOBJECT(add) 
#endif

/// Data for @c type_of_LIB to determine built-in modules,
/// use @c add(name) to add built-in library to macro
#define SI_FOREACH_BUILTIN(add)\
  add(staticdemo)\
  add(singmathic)\
  SI_BUILTIN_PYOBJECT(add)


#endif /* MOD_RAW_H */
