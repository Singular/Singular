/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mod_raw.cc,v 1.8 2000-05-23 13:10:10 Singular Exp $ */
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dunl_close()
*/

#include "mod2.h"
#include "static.h"
#ifdef HAVE_STATIC
#undef HAVE_DYNAMIC_LOADING
#undef HAVE_DYN_RL
#endif

#if defined(HAVE_DYNAMIC_LOADING) || defined(HAVE_DYN_RL)
#include "mod_raw.h"


/*****************************************************************************
 * SECTION ix86-linux / alpha-linux                                          *
 *****************************************************************************/
#if defined(ix86_Linux) || defined(DecAlpha_Linux) || defined(ix86_Linux_libc5)
#include <dlfcn.h>

void *dynl_open(
  char *filename    /* I: filename to load */
  )
{
  return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
}

void *dynl_sym(void *handle, char *symbol)
{
  return(dlsym(handle, symbol));
}

int dynl_close (void *handle)
{
  return(dlclose (handle));
}

const char *dynl_error()
{
  return(dlerror());
}
#endif /* ix86_Linux */

/*****************************************************************************
 * SECTION HPUX-9/10                                                         *
 *****************************************************************************/
#if defined(HPUX_9) || defined(HPUX_10)
#include <dl.h>

typedef char *((*func_ptr) ());

void *dynl_open(char *filename)
{
  shl_t           handle = shl_load(filename, BIND_DEFERRED, 0);

  return ((void *) handle);
}

void *dynl_sym(void *handle, char *symbol)
{
  func_ptr        f;

  if (shl_findsym((shl_t *) & handle, symbol, TYPE_PROCEDURE, &f) == -1)
  {
    if (shl_findsym((shl_t *) & handle, symbol, TYPE_UNDEFINED, &f) == -1)
    {
      f = (func_ptr) NULL;
    }
  }
  return (f);
}

int dynl_close (void *handle)
{
  shl_unload((shl_t) handle);
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "shl_load failed";

  return errmsg;
}
#endif /* HPUX_9  or HPUX_10 */

/*****************************************************************************
 * SECTION 68k-MPW                                                           *
 *****************************************************************************/
#ifdef 68k_MPW
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";

  return errmsg;
}
#  endif /* 68k_MPW */


/*****************************************************************************
 * SECTION AIX-4                                                             *
 *****************************************************************************/
#ifdef AIX_4
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";

  return errmsg;
}
#  endif /* AIX_4 */

/*****************************************************************************
 * SECTION IRIX-6                                                            *
 *****************************************************************************/
#ifdef IRIX_6
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";

  return errmsg;
}
#  endif /* IRIX_6 */

/*****************************************************************************
 * SECTION Sun3OS-4                                                          *
 *****************************************************************************/
#ifdef Sun3OS_4
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";

  return errmsg;
}
#endif /* Sun3OS_4 */

/*****************************************************************************
 * SECTION SunOS-4/5                                                         *
 *****************************************************************************/
#if defined(SunOS_4) || defined(SunOS_5)
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";

  return errmsg;
}
#endif /* SunOS_4 or SunOS_5 */

/*****************************************************************************
 * SECTION ix86-win                                                          *
 *****************************************************************************/
#ifdef ix86_win
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";

  return errmsg;
}
#endif /* ix86_win */

/*****************************************************************************
 * SECTION ppc-MPW                                                           *
 *****************************************************************************/
#ifdef ppc_MPW
/* #    include <> */

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, char *symbol)
{
  return(NULL);
}

int dynl_close (void *handle)
{
  return(0);
}

const char *dynl_error()
{
  static char errmsg[] = "support for dynamic loading not implemented";
  return errmsg;
}
#  endif /* ppc_MPW */


#endif /* HAVE_DYNAMIC_LOADING */
