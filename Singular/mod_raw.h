/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mod_raw.h,v 1.1 1999-04-01 10:10:08 krueger Exp $ */
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dunl_close()
*/

#ifndef MOD_RAW_H
#  define MOD_RAW_H
#  include "mod2.h"
#  ifdef HAVE_DYNAMIC_LOADING

void *dynl_open(char *filename);
void *dynl_sym(void *handle, char *symbol);
int dynl_close (void *handle);
const char *dynl_error();


#  endif /* HAVE_DYNAMIC_LOADING */
#endif /* MOD_RAW_H */
