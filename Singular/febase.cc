/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.cc,v 1.77 1999-08-03 16:33:40 obachman Exp $ */
/*
* ABSTRACT: i/o system
*/

#include "mod2.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#ifndef __MWERKS__
#include <unistd.h>
#endif
#ifdef NeXT
#include <sys/file.h>
#endif

#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "subexpr.h"
#include "ipshell.h"
#include "version.h"


#define fePutChar(c) fputc((uchar)(c),stdout)
/*0 implementation */

char fe_promptstr[]
#ifdef macintosh
                   =" \n";
#else
                   ="  ";
#endif

#define INITIAL_PRINT_BUFFER 24*1024
static int feBufferLength=INITIAL_PRINT_BUFFER;
static char * feBuffer=(char *)Alloc(INITIAL_PRINT_BUFFER);

int     si_echo = 0;
int     printlevel = 0;
#ifndef macintosh
int     pagelength = 24;
#else
int     pagelength = -1;
#endif
int     colmax = 80;
char    prompt_char = '>'; /*1 either '>' or '.'*/
extern "C" {
BITSET  verbose = 1
                  | Sy_bit(V_REDEFINE)
                  | Sy_bit(V_LOAD_LIB)
                  | Sy_bit(V_SHOW_USE)
                  | Sy_bit(V_PROMPT)
/*                  | Sy_bit(V_DEBUG_MEM) */
;}
BOOLEAN errorreported = FALSE;
BOOLEAN feBatch = FALSE;
char *  feErrors=NULL;
int     feErrorsLen=0;
BOOLEAN feWarn = TRUE;
BOOLEAN feOut = TRUE;

#ifdef macintosh
static  int lines = 0;
static  int cols = 0;
#endif

const char feNotImplemented[]="not implemented";

BOOLEAN feProt = FALSE;
FILE*   feProtFile;
BOOLEAN tclmode=FALSE;
/* TCL-Protocoll (Singular -x): <char type>:<int length>:<string> \n
*  E:l:s  error
*  W:l:s  warning
*  N:l:s  stdout
*  Q:0:   quit
*  P:l:   prompt > (ring defined)
*  U:l:   prompt > (no ring defined)
*  P:l:   prompt .
*  R:l:<ring-name> ring change
*  L:l:<lib name> library loaded
*  O:l:<list of options(space seperated)> option change
*  M:l:<mem-usage> output from "option(mem)"
*/

#include "febase.inc"

/*****************************************************************
 *
 * File handling
 *
 *****************************************************************/

FILE * feFopen(char *path, char *mode, char *where,int useWerror)
{
  char longpath[MAXPATHLEN];
  if (path[0]=='~')
  {
    char* home = getenv("HOME");
    if (home != NULL)
    {
      strcpy(longpath, home);
      strcat(longpath, &(path[1]));
      path = longpath;
    }
  }
  FILE * f=myfopen(path,mode);
  if (where!=NULL) strcpy(where,path);
  if ((*mode=='r') && (path[0]!=DIR_SEP)&&(path[0]!='.')
  &&(f==NULL))
  {
    char found = 0;
    char* spath = feResource('s');
    char *s;

    if (where==NULL) s=(char *)AllocL(250);
    else             s=where;

    if (spath!=NULL)
    {
      char *p,*q;
      p = spath;
      while( (q=strchr(p, fePathSep)) != NULL)
      {
        *q = '\0';
        strcpy(s,p);
        *q = fePathSep;
        strcat(s, DIR_SEPP);
        strcat(s, path);
        #ifndef macintosh
          if(!access(s, R_OK)) { found++; break; }
        #else
          f=fopen(s,mode); /* do not need myfopen: we test only the access */
          if (f!=NULL)  { found++; fclose(f); break; }
        #endif
        p = q+1;
      }
      if(!found)
      {
        strcpy(s,p);
        strcat(s, DIR_SEPP);
        strcat(s, path);
      }
      f=myfopen(s,mode);
      if (f!=NULL)
      {
        if (where==NULL) FreeL((ADDRESS)s);
        return f;
      }
    }
    else
    {
      if (where!=NULL) strcpy(s/*where*/,path);
      f=myfopen(path,mode);
    }
    if (where==NULL) FreeL((ADDRESS)s);
  }
  if ((f==NULL)&&(useWerror))
    Werror("cannot open `%s`",path);
  return f;
}

static char * feBufferStart;
  /* only used in StringSet(S)/StringAppend(S)*/
char * StringAppend(char *fmt, ...)
{
  va_list ap;
  char *s = feBufferStart; /*feBuffer + strlen(feBuffer);*/
  int more, vs;
  va_start(ap, fmt);
  if ((more=feBufferStart-feBuffer+strlen(fmt)+100)>feBufferLength)
  {
    more = ((more + (4*1024-1))/(4*1024))*(4*1024);
    int l=s-feBuffer;
    feBuffer=(char *)ReAlloc((ADDRESS)feBuffer,feBufferLength,
                                                     more);
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
  mmTest(feBuffer, feBufferLength);
  va_end(ap);
  return feBuffer;
}

char * StringAppendS(char *st)
{
  /* feBufferStart is feBuffer + strlen(feBuffer);*/
  int more,l;
  int ll=feBufferStart-feBuffer;
  if ((more=ll+2+(l=strlen(st)))>feBufferLength)
  {
    more = ((more + (4*1024-1))/(4*1024))*(4*1024);
    feBuffer=(char *)ReAlloc((ADDRESS)feBuffer,feBufferLength,
                                                     more);
    feBufferLength=more;
    feBufferStart=feBuffer+ll;
  }
  strcat(feBufferStart, st);
  feBufferStart +=l;
  return feBuffer;
}

char * StringSetS(char *st)
{
  int more,l;
  if ((l=strlen(st))>feBufferLength)
  {
    more = ((l + (4*1024-1))/(4*1024))*(4*1024);
    feBuffer=(char *)ReAlloc((ADDRESS)feBuffer,feBufferLength,
                                                     more);
    feBufferLength=more;
  }
  strcpy(feBuffer,st);
  feBufferStart=feBuffer+l;
  return feBuffer;
}

#ifndef __MWERKS__
#ifdef HAVE_TCL
extern "C" {
void PrintTCLS(const char c, const char *s)
{
  int l=strlen(s);
  if (l>0) PrintTCL(c,l,s);
}
}
#endif
#endif

extern "C" {
void WerrorS(const char *s)
{
#ifdef HAVE_MPSR
  if (feBatch)
  {
    if (feErrors==NULL)
    {
      feErrors=(char *)Alloc(256);
      feErrorsLen=256;
      *feErrors = '\0';
    }
    else
    {
      if (((int)(strlen((char *)s)+ 20 +strlen(feErrors)))>=feErrorsLen)
      {
        feErrors=(char *)ReAlloc(feErrors,feErrorsLen,feErrorsLen+256);
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
}

void Werror(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *s=(char *)Alloc(256);
  vsprintf(s, fmt, ap);
  WerrorS(s);
  Free(s,256);
  va_end(ap);
}
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
  if (feWarn) /* ignore warnings in when optin --no-warn was given */
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

void Warn(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *s=(char *)Alloc(256);
  vsprintf(s, fmt, ap);
  WarnS(s);
  Free(s,256);
  va_end(ap);
}

void fePrintReportBug(char* msg, char* file, int line)
{
  WarnS("YOU HAVE FOUND A BUG IN SINGULAR.");
  WarnS("Please, email the following output to singular@mathematik.uni-kl.de");
  Warn("Bug occured at %s:%d", file, line);
  Warn("Message: %s", msg);
  Warn("Version: %s %s (%d) %s %s", S_UNAME, S_VERSION1,
       SINGULAR_VERSION_ID,__DATE__,__TIME__);
}

extern "C" {
void assume_violation(char* file, int line)
{
  fprintf(stderr, "Internal assume violation: file %s line %d\n", file, line);
}
}

#ifdef macintosh
static  int lines = 0;
static  int cols = 0;

void mwrite(uchar c)
{
  if (c == '\n')
  {
    cols = 0;
    if (lines == pagelength)
    {
      lines = 0;
      fputs("pause>\n",stderr);
      uchar c = fgetc(stdin);
    }
    else
    {
      lines++;
      fePutChar(c);
    }
  }
  else
  {
    fePutChar(c);
    cols++;
    if (cols == colmax)
    {
      // cols = 0;   //will be done by mwrite('\n');
      mwrite('\n');
    }
  }
}
#endif

// some routines which redirect the output of print to a string
static char* sprint = NULL;
void SPrintStart()
{
  sprint = mstrdup("");
}

static void SPrintS(char* s)
{
  mmTestL(sprint);
  if (s == NULL) return;
  int ls = strlen(s);
  if (ls == 0) return;

  char* ns;
  int l = strlen(sprint);
  ns = (char*) AllocL((l + ls + 1)*sizeof(char));
  if (l > 0) strcpy(ns, sprint);

  strcpy(&(ns[l]), s);
  FreeL(sprint);
  sprint = ns;
  mmTestL(sprint);
}

char* SPrintEnd()
{
  char* ns = sprint;
  sprint = NULL;
  mmTestL(ns);
  return ns;
}

// Print routines
extern "C" {
void PrintS(char *s)
{
  if (sprint != NULL)
  {
    SPrintS(s);
    return;
  }

  if (feOut) /* do not print when option --no-out was given */
  {

#ifdef macintosh
    char c;
    while ('\0' != (c = *s++))
    {
      mwrite(c);
    }
#else
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
#endif
  }
}

void PrintLn()
{
  PrintS("\n");
}

void Print(char *fmt, ...)
{
  if (sprint != NULL)
  {
    int ls = strlen(fmt);
    va_list ap;
    va_start(ap, fmt);
    mmTestL(sprint);
    if (fmt != NULL && ls > 0)
    {
      char* ns;
      int l = strlen(sprint);
      ns = (char*) AllocL(sizeof(char)*(ls + l + 256));
      if (l > 0)  strcpy(ns, sprint);

#ifdef HAVE_VSNPRINTF
      l = vsnprintf(&(ns[l]), ls+255, fmt, ap);
      assume(l != -1);
#else
      vsprintf(&(ns[l]), fmt, ap);
#endif
      mmTestL(ns);
      FreeL(sprint);
      sprint = ns;
    }
    va_end(ap);
    return;
  }
  if (feOut)
  {
    va_list ap;
    va_start(ap, fmt);
#ifdef HAVE_TCL
    if(tclmode)
#endif
#if (defined(HAVE_TCL) || defined(macintosh))
    {
      char *s=(char *)Alloc(strlen(fmt)+256);
      vsprintf(s,fmt, ap);
#ifdef HAVE_TCL
      PrintTCLS('N',s);
#endif
#ifdef macintosh
      char c;
      while ('\0' != (c = *s++))
      {
        mwrite(c);
      }
      if (feProt&PROT_O)
      {
        vfprintf(feProtFile,fmt,ap);
      }
#endif
    }
#endif
#if !defined(macintosh) || defined(HAVE_TCL)
#ifdef HAVE_TCL
    else
#endif
    {
      vfprintf(stdout, fmt, ap);
      fflush(stdout);
      if (feProt&PROT_O)
      {
        vfprintf(feProtFile,fmt,ap);
      }
    }
#endif
    va_end(ap);
  }
}

/* end extern "C" */
}

void monitor(char* s, int mode)
{
  if (feProt)
  {
    fclose(feProtFile);
  }
  if ((s!=NULL) && (*s!='\0'))
  {
    feProtFile = myfopen(s,"w");
    if (feProtFile==NULL)
    {
      Werror("cannot open %s",s);
    }
    else
      feProt = (BOOLEAN)mode;
  }
}


char* eati(char *s, int *i)
{
  int l=0;

  if    (*s >= '0' && *s <= '9')
  {
    *i = 0;
    while (*s >= '0' && *s <= '9')
    {
      *i *= 10;
      *i += *s++ - '0';
      l++;
      if ((l>=MAX_INT_LEN)||((*i) <0))
      {
        s-=l;
        Werror("`%s` greater than %d(max. integer representation)",
                s,MAX_INT_VAL);
        return s;
      }
    }
  }
  else *i = 1;
  return s;
}

#ifndef unix
// Make sure that mode contains binary option
FILE *myfopen(char *path, char *mode)
{
  char mmode[4];
  int i;
  BOOLEAN done = FALSE;

  for (i=0;;i++)
  {
    mmode[i] = mode[i];
    if (mode[i] == '\0') break;
    if (mode[i] == 'b') done = TRUE;
  }

  if (! done)
  {
    mmode[i] = 'b';
    mmode[i+1] = '\0';
  }
  return fopen(path, mmode);
}
#endif

// replace "\r\n" by " \n" and "\r" by "\n"

size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t got = fread(ptr, size, nmemb, stream) * size;
  size_t i;

  for (i=0; i<got; i++)
  {
    if ( ((char*) ptr)[i] == '\r')
    {
      if (i+1 < got && ((char*) ptr)[i+1] == '\n')
        ((char*) ptr)[i] = ' ';
      else
        ((char*) ptr)[i] = '\n';
    }
  }
  return got;
}
