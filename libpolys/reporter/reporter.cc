/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: output system
*/

#include <reporter/config.h>

#include <stdlib.h>
#include <stdio.h>
#include <misc/mylimits.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#include <reporter/reporter.h>
#include <omalloc/omalloc.h>
//#include "options.h"

#define fePutChar(c) fputc((unsigned char)(c),stdout)
/*0 implementation */

// output/print buffer:
#define INITIAL_PRINT_BUFFER 24*1024L
// line buffer for reading:
// minimal value for MAX_FILE_BUFFER: 4*4096 - see Tst/Long/gcd0_l.tst
// this is an upper limit for the size of monomials/numbers read via the interpreter
#define MAX_FILE_BUFFER 4*4096
static long feBufferLength=INITIAL_PRINT_BUFFER;
static char * feBuffer=(char *)omAlloc(INITIAL_PRINT_BUFFER);

BOOLEAN errorreported = FALSE;
char *  feErrors=NULL;
int     feErrorsLen=0;
BOOLEAN feWarn = TRUE;
BOOLEAN feOut = TRUE;

const char feNotImplemented[]="not implemented";

int feProt = FALSE;
FILE*   feProtFile;

static char * feBufferStart;
  /* only used in StringSet(S)/StringAppend(S)*/
char * StringAppend(const char *fmt, ...)
{
  va_list ap;
  char *s = feBufferStart; /*feBuffer + strlen(feBuffer);*/
  int vs;
  long more;
  va_start(ap, fmt);
  if ((more=feBufferStart-feBuffer+strlen(fmt)+100)>feBufferLength)
  {
    more = ((more + (8*1024-1))/(8*1024))*(8*1024);
    int l=s-feBuffer;
    feBuffer=(char *)omReallocSize((void *)feBuffer,feBufferLength,
                                                     more);
#if (!defined(NDEBUG)) && (!defined(OM_NDEBUG)) && defined(HAVE_CONFIG_H)
    omMarkAsStaticAddr(feBuffer);
#endif
    feBufferLength=more;
    s=feBuffer+l;
#ifndef BSD_SPRINTF
    feBufferStart=s;
#endif
  }
#ifdef BSD_SPRINTF
  vsprintf(s, fmt, ap);
  while (*s!='\0') s++;
  feBufferStart =s;
#else
#ifdef HAVE_VSNPRINTF
  vs = vsnprintf(s, feBufferLength - (feBufferStart - feBuffer), fmt, ap);
  if (vs == -1)
  {
    assume(0);
    feBufferStart = feBuffer + feBufferLength -1;
  }
  else
  {
    feBufferStart += vs;
  }
#else
  feBufferStart += vsprintf(s, fmt, ap);
#endif
#endif
  omCheckAddrSize(feBuffer, feBufferLength);
  va_end(ap);
  return feBuffer;
}

char * StringAppendS(const char *st)
{
  if (*st!='\0')
  {
    /* feBufferStart is feBuffer + strlen(feBuffer);*/
    int l;
    long more;
    int ll=feBufferStart-feBuffer;
    if ((more=ll+2+(l=strlen(st)))>feBufferLength)
    {
      more = ((more + (8*1024-1))/(8*1024))*(8*1024);
      feBuffer=(char *)omReallocSize((void *)feBuffer,feBufferLength,
                                                       more);
      feBufferLength=more;
      feBufferStart=feBuffer+ll;
    }
    strcat(feBufferStart, st);
    feBufferStart +=l;
  }
  return feBuffer;
}

char * StringSetS(const char *st)
{
  int l;
  long more;
  if ((l=strlen(st))>feBufferLength)
  {
    more = ((l + (4*1024-1))/(4*1024))*(4*1024);
    feBuffer=(char *)omReallocSize((void *)feBuffer,feBufferLength,
                                                     more);
    feBufferLength=more;
  }
  strcpy(feBuffer,st);
  feBufferStart=feBuffer+l;
  return feBuffer;
}

#ifdef HAVE_TCL
extern "C" {
void PrintTCLS(const char c, const char *s)
{
  int l=strlen(s);
  if (l>0) PrintTCL(c,l,s);
}
}
#endif

extern "C" {
void WerrorS(const char *s)
{
#ifdef HAVE_MPSR
  if (fe_fgets_stdin==fe_fgets_dummy)
  {
    if (feErrors==NULL)
    {
      feErrors=(char *)omAlloc(256);
      feErrorsLen=256;
      *feErrors = '\0';
    }
    else
    {
      if (((int)(strlen((char *)s)+ 20 +strlen(feErrors)))>=feErrorsLen)
      {
        feErrors=(char *)omReallocSize(feErrors,feErrorsLen,feErrorsLen+256);
        feErrorsLen+=256;
      }
    }
    strcat(feErrors, "Singular error: ");
    strcat(feErrors, (char *)s);
  }
  else
#endif
  {
#ifdef HAVE_TCL
    if (tclmode)
    {
      PrintTCLS('E',(char *)s);
      PrintTCLS('E',"\n");
    }
    else
#endif
    {
      fwrite("   ? ",1,5,stderr);
      fwrite((char *)s,1,strlen((char *)s),stderr);
      fwrite("\n",1,1,stderr);
      fflush(stderr);
      if (feProt&PROT_O)
      {
        fwrite("   ? ",1,5,feProtFile);
        fwrite((char *)s,1,strlen((char *)s),feProtFile);
        fwrite("\n",1,1,feProtFile);
      }
    }
  }
  errorreported = TRUE;
#ifdef HAVE_FACTORY
  // libfac:
  //extern int libfac_interruptflag;
  //libfac_interruptflag=1;
#endif
}

void Werror(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *s=(char *)omAlloc(256);
  vsprintf(s, fmt, ap);
  WerrorS(s);
  omFreeSize(s,256);
  va_end(ap);
}

void WarnS(const char *s)
{
  #define warn_str "// ** "
#ifdef HAVE_TCL
  if (tclmode)
  {
    PrintTCLS('W',warn_str);
    PrintTCLS('W',s);
    PrintTCLS('W',"\n");
  }
  else
#endif
  if (feWarn) /* ignore warnings if option --no-warn was given */
  {
    fwrite(warn_str,1,6,stdout);
    fwrite(s,1,strlen(s),stdout);
    fwrite("\n",1,1,stdout);
    fflush(stdout);
    if (feProt&PROT_O)
    {
      fwrite(warn_str,1,6,feProtFile);
      fwrite(s,1,strlen(s),feProtFile);
      fwrite("\n",1,1,feProtFile);
    }
  }
}
} /* end extern "C" */

void Warn(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *s=(char *)omAlloc(256);
  vsprintf(s, fmt, ap);
  WarnS(s);
  omFreeSize(s,256);
  va_end(ap);
}


// some routines which redirect the output of print to a string
static char* sprint = NULL;
void SPrintStart()
{
  sprint = omStrDup("");
}

static void SPrintS(const char* s)
{
  omCheckAddr(sprint);
  if ((s == NULL)||(*s == '\0')) return;
  int ls = strlen(s);

  char* ns;
  int l = strlen(sprint);
  ns = (char*) omAlloc((l + ls + 1)*sizeof(char));
  if (l > 0) strcpy(ns, sprint);

  strcpy(&(ns[l]), s);
  omFree(sprint);
  sprint = ns;
  omCheckAddr(sprint);
}

char* SPrintEnd()
{
  char* ns = sprint;
  sprint = NULL;
  omCheckAddr(ns);
  return ns;
}

// Print routines
extern "C" {
void PrintS(const char *s)
{
  if (sprint != NULL)
  {
    SPrintS(s);
    return;
  }
  else if (feOut) /* do not print when option --no-out was given */
  {

#ifdef HAVE_TCL
    if (tclmode)
    {
      PrintTCLS('N',s);
    }
    else
#endif
    {
      fwrite(s,1,strlen(s),stdout);
      fflush(stdout);
      if (feProt&PROT_O)
      {
        fwrite(s,1,strlen(s),feProtFile);
      }
    }
  }
}

void PrintLn()
{
  PrintS("\n");
}

void Print(const char *fmt, ...)
{
  if (sprint != NULL)
  {
    va_list ap;
    va_start(ap, fmt);
    omCheckAddr(sprint);
    int ls = strlen(fmt);
    if (fmt != NULL && ls > 0)
    {
      char* ns;
      int l = strlen(sprint);
      ns = (char*) omAlloc(sizeof(char)*(ls + l + 512));
      if (l > 0)  strcpy(ns, sprint);

#ifdef HAVE_VSNPRINTF
      l = vsnprintf(&(ns[l]), ls+511, fmt, ap);
      assume(l != -1);
#else
      vsprintf(&(ns[l]), fmt, ap);
#endif
      omCheckAddr(ns);
      omFree(sprint);
      sprint = ns;
    }
    va_end(ap);
    return;
  }
  else if (feOut)
  {
    va_list ap;
    va_start(ap, fmt);
    int l;
    long ls=strlen(fmt);
    char *s=(char *)omAlloc(ls+512);
#ifdef HAVE_VSNPRINTF
    l = vsnprintf(s, ls+511, fmt, ap);
    if ((l==-1)||(s[l]!='\0')||(l!=strlen(s)))
    {
      printf("Print problem: l=%d, fmt=>>%s<<\n",l,fmt);
    }
#else
    vsprintf(s, fmt, ap);
#endif
    PrintS(s);
    omFree(s);
    va_end(ap);
  }
}
void PrintNSpaces(const int n)
{
  int l=n-1;
  while(l>=0) { PrintS(" "); l--; }
}

/* end extern "C" */
}
