#ifndef MOD_RAW_H
#define MOD_RAW_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mod_raw.h,v 1.3 1999-11-15 17:20:28 obachman Exp $ */
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dunl_close()
*/

#include "mod2.h"
#if defined(HAVE_DYNAMIC_LOADING) || defined(HAVE_DYN_RL)

void *       dynl_open(char *filename);
void *       dynl_sym(void *handle, char *symbol);
int          dynl_close (void *handle);
const char * dynl_error();


#endif /* HAVE_DYNAMIC_LOADING  || HAVE_DYN_RL */
#endif /* MOD_RAW_H */
