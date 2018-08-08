
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: output system
*/

#include "misc/auxiliary.h"

#include "reporter/reporter.h"
#include "resources/feResource.h"
#include "resources/feFopen.h"
//#include "options.h"
#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include "misc/mylimits.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif


#define fePutChar(c) fputc((unsigned char)(c),stdout)
/*0 implementation */

// output/print buffer:
#define INITIAL_PRINT_BUFFER 24*1024L
// line buffer for reading:
// minimal value for MAX_FILE_BUFFER: 4*4096 - see Tst/Long/gcd0_l.tst
// this is an upper limit for the size of monomials/numbers read via the interpreter
#define MAX_FILE_BUFFER 4*4096
static long feBufferLength=0;
static char * feBuffer=NULL;
static long feBufferLength_save[8];
static char * feBuffer_save[8];
static int feBuffer_cnt=0;
static char * feBufferStart_save[8];


char *  feErrors=NULL;
int     feErrorsLen=0;
BOOLEAN feWarn = TRUE;
BOOLEAN feOut = TRUE;

//void (*WerrorS_callback)(const char *s) = NULL;

const char feNotImplemented[]="not implemented";

int feProt = FALSE;
FILE*   feProtFile;

static char * feBufferStart;
  /* only used in StringSet(S)/StringAppend(S)*/
void StringAppend(const char *fmt, ...)
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
#if (!defined(SING_NDEBUG)) && (!defined(OM_NDEBUG))
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
}

void StringAppendS(const char *st)
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
      feBuffer=(char *)omreallocSize((void *)feBuffer,feBufferLength,
                                                       more);
      feBufferLength=more;
      feBufferStart=feBuffer+ll;
    }
    strcat(feBufferStart, st);
    feBufferStart +=l;
  }
}

void StringSetS(const char *st)
{
  feBuffer_save[feBuffer_cnt]=feBuffer;
  feBuffer=(char*)omAlloc0(INITIAL_PRINT_BUFFER);
  feBufferLength_save[feBuffer_cnt]=feBufferLength;
  feBufferLength=INITIAL_PRINT_BUFFER;
  feBufferStart_save[feBuffer_cnt]=feBufferStart;
  feBufferStart=feBuffer;
  feBuffer_cnt++;
  assume(feBuffer_cnt<8);
  int l;
  long more;
  if ((l=strlen(st))>feBufferLength)
  {
    more = ((l + (4*1024-1))/(4*1024))*(4*1024);
    feBuffer=(char *)omReallocSize((ADDRESS)feBuffer,feBufferLength,
                                                     more);
    feBufferLength=more;
  }
  strcpy(feBuffer,st);
  feBufferStart=feBuffer+l;
}

char * StringEndS()
{
  char *r=feBuffer;
  feBuffer_cnt--;
  assume(feBuffer_cnt >=0);
  feBuffer=feBuffer_save[feBuffer_cnt];
  feBufferLength=feBufferLength_save[feBuffer_cnt];
  feBufferStart=feBufferStart_save[feBuffer_cnt];
  if (strlen(r)<1024)
  {
    // if the used buffer is a "small block",
    // substitue the "large" initial block by a small one
    char *s=omStrDup(r); omFree(r); r=s;
  }
  return r;
}

void WerrorS_batch(const char *s)
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
  errorreported = TRUE;
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

void (*WarnS_callback)(const char *s) = NULL;

void WarnS(const char *s)
{
  #define warn_str "// ** "
  if (feWarn) /* ignore warnings if option --no-warn was given */
  {
    if (WarnS_callback==NULL)
    {
      fwrite(warn_str,1,6,stdout);
      fwrite(s,1,strlen(s),stdout);
      fwrite("\n",1,1,stdout);
      fflush(stdout);
      if (feProt&SI_PROT_O)
      {
        fwrite(warn_str,1,6,feProtFile);
        fwrite(s,1,strlen(s),feProtFile);
        fwrite("\n",1,1,feProtFile);
      }
    }
    else
    {
      WarnS_callback(s);
    }
  }
}

void Warn(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *s=(char *)omAlloc(256);
#ifdef HAVE_VSNPRINTF
  vsnprintf(s, 256, fmt, ap);
#else
  vsprintf(s, fmt, ap);
#endif
  WarnS(s);
  omFreeSize(s,256);
  va_end(ap);
}


// some routines which redirect the output of print to a string
static char* sprint = NULL;
static char* sprint_backup = NULL;
void SPrintStart()
{
  if (sprint!=NULL)
  {
    if (sprint_backup!=NULL) WerrorS("internal error: SPrintStart");
    else sprint_backup=sprint;
  }
  sprint = omStrDup("");
}

static inline void SPrintS(const char* s)
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
  sprint = sprint_backup;
  sprint_backup=NULL;
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

    if (PrintS_callback!=NULL)
    {
      PrintS_callback(s);
    }
    else
    {
      fwrite(s,1,strlen(s),stdout);
      fflush(stdout);
      if (feProt&SI_PROT_O)
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
    if ((l==-1)||(s[l]!='\0')||(l!=(int)strlen(s)))
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

const char* eati(const char *s, int *i)
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

void feStringAppendResources(int warn)
{
  int i = 0;
  char* r;
  StringAppend("%-10s:\t%s\n", "argv[0]", feArgv0);
  while (feResourceConfigs[i].key != NULL)
  {
    r = feResource(feResourceConfigs[i].key, warn);
    StringAppend("%-10s:\t%s\n", feResourceConfigs[i].key,
                 (r != NULL ? r : ""));
    i++;
  }
}
