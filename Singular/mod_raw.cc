/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mod_raw.cc,v 1.18 2003-02-18 17:44:31 Singular Exp $ */
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dunl_close()
*/

#include <stdio.h>

#include "mod2.h"
#include "static.h"
#ifdef HAVE_STATIC
#undef HAVE_DL
#endif

#if defined(HAVE_DL)
#include "mod_raw.h"
#include "febase.h"

/*****************************************************************************
 *
 * General section
 * These are just wrappers around the repsective dynl_* calls 
 * which look for the binary in the bin_dir of Singular and ommit warnings if
 * somethings goes wrong
 *
 *****************************************************************************/
static BOOLEAN warn_handle = FALSE;
static BOOLEAN warn_proc = FALSE;
#ifndef DL_TAIL
#define DL_TAIL "so"
#endif

void* dynl_open_binary_warn(char* binary_name, const char* msg)
{
  void* handle = NULL;
  const char* bin_dir = feGetResource('b');
  if (bin_dir != NULL)
  {
    char path_name[MAXPATHLEN];
    sprintf(path_name, "%s%s%s.%s", bin_dir, DIR_SEPP, binary_name, 
            DL_TAIL);
    handle = dynl_open(path_name);
    if (handle == NULL && ! warn_handle)
    {
      Warn("Could not open dynamic library: %s", path_name);
      Warn("Error message from system: %s", dynl_error());
      if (msg != NULL) Warn("%s", msg);
      Warn("See the INSTALL section in the Singular manual for details.");
      warn_handle = TRUE;
    }
  }
  return  handle;
}

void* dynl_sym_warn(void* handle, char* proc, const char* msg)
{
  void *proc_ptr = NULL;
  if (handle != NULL)
  {
    proc_ptr = dynl_sym(handle, proc);
    if (proc_ptr == NULL && ! warn_proc)
    {
      Warn("Could load a procedure from a dynamic library");
      Warn("Error message from system: %s", dynl_error());
      if (msg != NULL) Warn("%s", msg);
      Warn("See the INSTALL section in the Singular manual for details.");
      warn_proc = TRUE;
    }
  }
  return proc_ptr;
}


/*****************************************************************************
 * SECTION ix86-linux / alpha-linux                                          *
 *****************************************************************************/
#if defined(ix86_Linux) || defined(DecAlpha_Linux) || defined(ix86_Linux_libc5)
#include <dlfcn.h>

static void* kernel_handle = NULL;
void *dynl_open(
  char *filename    /* I: filename to load */
  )
{
  return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
}

void *dynl_sym(void *handle, char *symbol)
{
  if (handle == DYNL_KERNEL_HANDLE)
  {
    if (kernel_handle == NULL)
      kernel_handle = dynl_open(NULL);
    handle = kernel_handle;
  }
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
  
  if (handle == DYNL_KERNEL_HANDLE)
    handle = PROG_HANDLE;
  
  if (shl_findsym((shl_t *) & handle, symbol, TYPE_PROCEDURE, &f) == -1)
  {
    if (shl_findsym((shl_t *) & handle, symbol, TYPE_UNDEFINED, &f) == -1)
    {
      f = (func_ptr) NULL;
    }
  }
  return ((void *)f);
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
#ifdef MPW
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
#include <dlfcn.h>
static void* kernel_handle = NULL;
void *dynl_open( char *filename)
{
  return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
}

void *dynl_sym(void *handle, char *symbol)
{
  if (handle == DYNL_KERNEL_HANDLE)
  {
    if (kernel_handle == NULL)
      kernel_handle = dynl_open(NULL);
    handle = kernel_handle;
  }
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
 * SECTION SunOS-4                                                         *
 *****************************************************************************/
#if defined(SunOS_4)
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
#endif /* SunOS_4 */
 
/*****************************************************************************
 * SECTION SunOs-5
 *****************************************************************************/
#if defined(SunOS_5)
#include <dlfcn.h>

static void* kernel_handle = NULL;
void *dynl_open(
  char *filename    /* I: filename to load */
  )
{
  return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
}

void *dynl_sym(void *handle, char *symbol)
{
  if (handle == DYNL_KERNEL_HANDLE)
  {
    if (kernel_handle == NULL)
      kernel_handle = dynl_open(NULL);
    handle = kernel_handle;
  }
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
#endif /* SunOS_5 */

/*****************************************************************************
 * SECTION ix86-Win                                                          *
 *****************************************************************************/
// is defined externally in sing_win.cc

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


#endif /* HAVE_DL */
