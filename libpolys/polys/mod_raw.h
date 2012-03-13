#ifndef MOD_RAW_H
#define MOD_RAW_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
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
#endif /* MOD_RAW_H */
