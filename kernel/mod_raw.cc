/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
 * ABSTRACT: machine depend code for dynamic modules
 *
 * Provides: dynl_open()
 *           dynl_sym()
 *           dynl_error()
 *           dynl_close()
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>


#include <kernel/mod2.h>
#include <Singular/static.h>

#include <kernel/mod_raw.h>
#include <kernel/febase.h>

#ifdef HAVE_STATIC
#undef HAVE_DL
#endif
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define BYTES_TO_CHECK 7

char* si_bultin_libs[]={ "huhu.so", NULL };

lib_types type_of_LIB(char *newlib, char *libnamebuf)
{
  const char mach_o[]={0xfe,0xed,0xfa,0xce,0};
  const char mach_o_module[]={0xce,0xfa,0xed,0xfe,0};
  int i=0;
  while(si_bultin_libs[i]!=NULL)
  {
    if (strcmp(newlib,si_bultin_libs[i])==0)
    {
      if(libnamebuf!=NULL) strcpy(libnamebuf,newlib);
      return LT_BUILTIN;
    }
    i++;
  }
  char        buf[BYTES_TO_CHECK+1];        /* one extra for terminating '\0' */
  struct stat sb;
  int nbytes = 0;
  int ret;
  lib_types LT=LT_NONE;

  FILE * fp = feFopen( newlib, "r", libnamebuf, FALSE );
  ret = stat(libnamebuf, &sb);

  if (fp==NULL)
  {
    return LT_NOTFOUND;
  }
  if((sb.st_mode & S_IFMT) != S_IFREG)
  {
    goto lib_type_end;
  }
  if ((nbytes = fread((char *)buf, sizeof(char), BYTES_TO_CHECK, fp)) == -1)
  {
    goto lib_type_end;
    /*NOTREACHED*/
  }
  if (nbytes == 0)
    goto lib_type_end;
  else
  {
    buf[nbytes++] = '\0';        /* null-terminate it */
  }
  if( (strncmp(buf, "\177ELF", 4)==0)) /* generic ELF */
  {
    LT = LT_ELF;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }

  if( (strncmp(buf, &mach_o[0], 4)==0)) /* generic Mach-O module */
  {
    LT = LT_MACH_O;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }

  if( (strncmp(buf, &mach_o_module[0], 4)==0)) /* Mach-O bundle */
  {
    LT = LT_MACH_O;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }


  if( (strncmp(buf, "\02\020\01\016\05\022@", 7)==0))
  {
    LT = LT_HPUX;
    //omFree(newlib);
    //newlib = omStrDup(libnamebuf);
    goto lib_type_end;
  }
  if(isprint(buf[0]) || buf[0]=='\n')
  { LT = LT_SINGULAR; goto lib_type_end; }

  lib_type_end:
  fclose(fp);
  return LT;
}
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
#define DL_TAIL "so"
#endif

void* dynl_open_binary_warn(const char* binary_name, const char* msg)
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

void* dynl_sym_warn(void* handle, const char* proc, const char* msg)
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

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * SECTION generic ELF: ix86-linux / alpha-linux / IA64-linux /x86_64_Linux  *
 *                      SunOS-5 / IRIX-6 / ppcMac-Darwin / FreeeBSD          *
 *****************************************************************************/
// relying on gcc to define __ELF__, check with cpp -dM /dev/null
// Mac OsX is an ELF system, but does not define __ELF__
// Solaris is an ELF system, but does not define __ELF__
#if defined(__ELF__)
#define HAVE_ELF_SYSTEM
#endif

#if defined(ppcMac_darwin)
#define HAVE_ELF_SYSTEM
#endif

#if defined(ix86Mac_darwin)
#define HAVE_ELF_SYSTEM
#endif

#if defined(x86_64Mac_darwin)
#define HAVE_ELF_SYSTEM
#endif

#if defined(SunOS_5)
#define HAVE_ELF_SYSTEM
#endif

#if defined(HAVE_ELF_SYSTEM)
#include <dlfcn.h>

static void* kernel_handle = NULL;
void *dynl_open(
  char *filename    /* I: filename to load */
  )
{
// glibc 2.2:
  if ((filename==NULL) || (dlopen(filename,RTLD_NOW|RTLD_NOLOAD)==NULL))
    return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
  else
    Werror("module %s already loaded",filename);
  return NULL;
// alternative
//    return(dlopen(filename, RTLD_NOW|RTLD_GLOBAL));
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
#include <dl.h>

typedef char *((*func_ptr) ());

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
 * SECTION AIX-4                                                             *
 *****************************************************************************/
#ifdef AIX_4
#define DL_NOT_IMPLEMENTED
#endif

/*****************************************************************************
 * SECTION Sun3OS-4                                                          *
 *****************************************************************************/
#ifdef Sun3OS_4
#define DL_NOT_IMPLEMENTED
#endif

/*****************************************************************************
 * SECTION SunOS-4                                                         *
 *****************************************************************************/
#if defined(SunOS_4)
#define DL_NOT_IMPLEMENTED
#endif

/*****************************************************************************
 * SECTION generic: dynamic madules not available
 *****************************************************************************/
#ifdef DL_NOT_IMPLEMEMENTED

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
