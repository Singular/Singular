#ifndef MOD_RAW_H
#define MOD_RAW_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mod_raw.h,v 1.5 2000-12-07 15:03:57 obachman Exp $ */
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dunl_close()
*/

#if defined(HAVE_DL)
void* dynl_open_binary_warn(char* binary_name);
void* dynl_sym_warn(void* handle, char* proc);

void *       dynl_open(char *filename);
void *       dynl_sym(void *handle, char *symbol);
int          dynl_close (void *handle);
const char * dynl_error();


#endif /* HAVE_DL */
#endif /* MOD_RAW_H */
