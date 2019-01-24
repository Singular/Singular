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
 *           dynl_close()
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "misc/auxiliary.h"
#include "reporter/reporter.h"
#include "resources/feResource.h"
#include "omalloc/omalloc.h"
#include "mod_raw.h"

#ifdef HAVE_STATIC
#undef HAVE_DL
#endif
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#if defined(HAVE_DL)

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
#define DL_TAIL ".so"
//#define DL_TAIL "_g.so"
#endif

void* dynl_open_binary_warn(const char* binary_name, const char* msg)
{
  void* handle = NULL;
  char* binary_name_so=NULL;
  BOOLEAN found=FALSE;

  // try P_PROCS_DIR (%P)
  char* proc_path = feGetResource('P');
  if (proc_path != NULL)
  {
    char *p;
    char *q;
    p=proc_path;
    int binary_name_so_length = 3 + strlen(DL_TAIL) + strlen(binary_name) + strlen(DIR_SEPP) + strlen(proc_path);
    binary_name_so = (char *)omAlloc0( binary_name_so_length * sizeof(char) );
    while((p!=NULL)&&(*p!='\0'))
    {
      q=strchr(p,fePathSep);
      if (q!=NULL) *q='\0';
      strcpy(binary_name_so,p);
      if (q!=NULL) *q=fePathSep;
      strcat(binary_name_so,DIR_SEPP);
      strcat(binary_name_so,binary_name);
      strcat(binary_name_so,DL_TAIL);
      if(!access(binary_name_so, R_OK)) { found=TRUE; break; }
      if (q!=NULL) p=q+1; else p=NULL;
    }
    if (found) handle = dynl_open(binary_name_so);
  }

  if (handle == NULL && ! warn_handle)
  {
    Warn("Could not find dynamic library: %s%s (path %s)",
            binary_name, DL_TAIL,proc_path);
    if (found) Warn("Error message from system: %s", dynl_error());
    if (msg != NULL) Warn("%s", msg);
    WarnS("See the INSTALL section in the Singular manual for details.");
    warn_handle = TRUE;
  }
  omfree((ADDRESS)binary_name_so );

  return  handle;
}

void* dynl_sym_warn(void* handle, const char* proc, const char* msg)
{
  void *proc_ptr = NULL;
  if (handle != NULL)
  {
    proc_ptr = dynl_sym(handle, proc);
    if (proc_ptr == NULL && ! warn_proc)
    {
      WarnS("Could load a procedure from a dynamic library");
      Warn("Error message from system: %s", dynl_error());
      if (msg != NULL) Warn("%s", msg);
      WarnS("See the INSTALL section in the Singular manual for details.");
      warn_proc = TRUE;
    }
  }
  return proc_ptr;
}

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * SECTION generic ELF: ix86-linux / alpha-linux / IA64-linux /x86_64_Linux  *
 *                      SunOS-5 / IRIX-6 / ppcMac-Darwin / FreeeBSD          *
 *****************************************************************************/
// relying on gcc to define __ELF__, check with cpp -dM /dev/null
#if defined(__ELF__)
#define HAVE_ELF_SYSTEM
#endif

// Mac OsX is an ELF system, but does not define __ELF__
#if (defined(__APPLE__) && defined(__MACH__)) && (!defined(HAVE_ELF_SYSTEM))
#define HAVE_ELF_SYSTEM
#endif

// Solaris is an ELF system, but does not define __ELF__
#if defined(__sun) && defined(__SVR4)
#define HAVE_ELF_SYSTEM
#endif

#if defined(HAVE_ELF_SYSTEM)
#include <dlfcn.h>
#define DL_IMPLEMENTED

static void* kernel_handle = NULL;
int dynl_check_opened(
  char *filename    /* I: filename to check */
  )
{
  return dlopen(filename,RTLD_NOW|RTLD_NOLOAD) != NULL;
}

void *dynl_open(
  char *filename    /* I: filename to load */
  )
{
  return dlopen(filename, RTLD_NOW|RTLD_GLOBAL);
#if 0
// glibc 2.2:
  if ((filename==NULL) || (dlopen(filename,RTLD_NOW|RTLD_NOLOAD)==NULL))
    return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
  else
    Werror("module %s already loaded",filename);
  return NULL;
// alternative
//    return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
#endif
}

void *dynl_sym(void *handle, const char *symbol)
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
#endif /* ELF_SYSTEM */

/*****************************************************************************
 * SECTION HPUX-9/10                                                         *
 *****************************************************************************/
#if defined(HPUX_9) || defined(HPUX_10)
#define DL_IMPLEMENTED
#include <dl.h>

typedef char *((*func_ptr) ());

int dynl_check_opened(    /* NOTE: untested */
  char *filename    /* I: filename to check */
  )
{
  struct shl_descriptor *desc;
  for (int idx = 0; shl_get(idx, &desc) != -1; ++idx)
  {
    if (strcmp(filename, desc->filename) == 0) return TRUE;
  }
  return FALSE;
}

void *dynl_open(char *filename)
{
  shl_t           handle = shl_load(filename, BIND_DEFERRED, 0);

  return ((void *) handle);
}

void *dynl_sym(void *handle, const char *symbol)
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
 * SECTION generic: dynamic madules not available
 *****************************************************************************/
#ifndef DL_IMPLEMENTED

int dynl_check_opened(char *filename)
{
  return FALSE;
}

void *dynl_open(char *filename)
{
  return(NULL);
}

void *dynl_sym(void *handle, const char *symbol)
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
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAVE_DL */
