/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.cc,v 1.25 1998-02-27 14:06:10 Singular Exp $ */
/*
* ABSTRACT: i/o system
*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#ifndef macintosh
#include <unistd.h>
#endif
#ifdef NeXT
#include <sys/file.h>
#endif
#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "subexpr.h"
#include "ipshell.h"

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
BITSET  verbose = 1
                  | Sy_bit(V_REDEFINE)
                  | Sy_bit(V_LOAD_LIB)
                  | Sy_bit(V_SHOW_USE)
                  | Sy_bit(V_PROMPT)
//                  | Sy_bit(V_DEBUG_MEM)
                  ;
BOOLEAN errorreported = FALSE;
BOOLEAN feBatch;
char *  feErrors=NULL;
int     feErrorsLen=0;

BOOLEAN feProt = FALSE;
FILE*   feProtFile;
BOOLEAN tclmode=FALSE;
/* TCL-Protocoll (Singular -x): <char type>:<int length>:<string> \n
*  E:l:s  error - not implemented yet (use N)
*  W:l:s  warning
*  N:l:s  stdout
*  Q:0:   quit
*  P:0:   prompt >
*  P:1:   prompt .
*  R:l:<ring-name> ring change
* plan:
*  O:l:<option/no-option> option change (option)
*  V:l:<option/no-option> option change (verbose)
*/

#include "febase.inc"

/*2
* fopen, but use 'SINGULARPATH' from environment and SINGULARDATADIR
* as set by configure
*/
#ifdef macintosh
#  define  FS_SEP ','
#  define  DIR_SEP ':'
#  define  DIR_SEPP ":"
#else
#ifdef MSDOS
#  define  FS_SEP ';'
#  define  DIR_SEP '\\'
#  define  DIR_SEPP "\\"
#else
#ifdef atarist
#  define  FS_SEP ';'
#  define  DIR_SEP '\\'
#  define  DIR_SEPP "\\"
#else  /* unix */
#  define  FS_SEP ':'
#  define  DIR_SEP '/'
#  define  DIR_SEPP "/"
#endif  /* atarist */
#endif  /* MSDOS */
#endif  /* macintosh */

FILE * feFopen(char *path, char *mode, char *where,int useWerror)
{
  FILE * f=fopen(path,mode);
#ifdef macintosh
  if (f!=NULL)
  {
    if (where!=NULL) strcpy(where,path);
    return f;
  }
  char *res;
  int idat=strlen(SINGULAR_DATADIR),ipath=strlen(path);
  char *env=getenv("SINGULARPATH");
  int ienv=strlen(env), ii=ienv;
  if (ii<idat) ii = idat;
  if (ii==0)
  {
    if (useWerror)
      Werror("cannot open `%s`",path);
    return f;
  }
  res=(char*) AllocL(ii+ipath+1);
  if (ienv!=0)
  {
    memcpy(res,env,ienv);
    memcpy(res+ienv,path,ipath);
    res[ienv+ipath]='\0';
    f=fopen(res,mode);
  }
  if ((f==NULL)&&(idat!=0))
  {
    memcpy(res,SINGULAR_DATADIR,idat);
    memcpy(res+idat,path,ipath);
    res[idat+ipath]='\0';
    f=fopen(res,mode);
  }
  if (f==NULL)
  {
    if (useWerror)
      Werror("cannot open `%s`",res);
  }
  else if (where!=NULL)
    strcpy(where,res);
  FreeL(res);
#else
  if (where!=NULL) strcpy(where,path);
  if ((*mode=='r') && (path[0]!=DIR_SEP)&&(path[0]!='.')
  &&(f==NULL))
  {
    char found = 0;
    #ifdef MSDOS
      char *env=getenv("SPATH");
    #else
      char *env=getenv("SINGULARPATH");
    #endif
    char *s;
    #ifndef macintosh
    // extend path by SINGULAR_DATADIR
    s = (char*) AllocL((env != NULL ? strlen(env) : 0)
                       +strlen(SINGULAR_DATADIR)+2);
    if (env != NULL)
    {
      strcpy(s, env);
      s[strlen(env)] = FS_SEP;
      s[strlen(env)+1] = '\0';
      strcat(s, SINGULAR_DATADIR);
    }
    else strcpy(s, SINGULAR_DATADIR);
    env = s;
    #endif
    if (where==NULL) s=(char *)AllocL(250);
    else             s=where;
    if (env!=NULL)
    {
      char *p,*q;
      p = env;
      while( (q=strchr(p, FS_SEP)) != NULL)
      {
        *q = '\0';
        strcpy(s,p);
        *q = FS_SEP;
        strcat(s, DIR_SEPP);
        strcat(s, path);
        #ifndef macintosh
          if(!access(s, R_OK)) { found++; break; }
        #else
          f=fopen(s,mode);
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
      f=fopen(s,mode);
      if (f!=NULL)
      {
        if (where==NULL) FreeL((ADDRESS)s);
        return f;
      }
    }
    else
    {
      if (where!=NULL) strcpy(s/*where*/,path);
      f=fopen(path,mode);
    }
    #ifndef macintosh
    FreeL(env);
    #endif
    if (where==NULL) FreeL((ADDRESS)s);
  }
  if ((f==NULL)&&(useWerror))
    Werror("cannot open `%s`",path);
#endif
  return f;
}

static char * feBufferStart;
  /* only used in StringSet(S)/StringAppend(S)*/
char * StringAppend(char *fmt, ...)
{
  va_list ap;
  char *s = feBufferStart; /*feBuffer + strlen(feBuffer);*/
  int more;
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
  feBufferStart += vsprintf(s, fmt, ap);
#endif
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

char * StringSet(char *fmt, ...)
{
  va_list ap;
  char *s = feBuffer;
  va_start(ap, fmt);
#ifdef BSD_SPRINTF
  vsprintf(s, fmt, ap);
  while (*s!='\0') s++;
  feBufferStart = s;
#else
  feBufferStart = feBuffer + vsprintf(s, fmt, ap);
#endif
  va_end(ap);
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

void PrintTCLS(char c, char *s)
{
#ifndef macintosh
  int l=strlen(s);
  if (l>0) PrintTCL(c,l,s);
#endif
}

extern "C" {
void WerrorS(char *s)
{
#ifdef HAVE_MPSR
  if (feBatch)
  {
    if (feErrors==NULL)
    {
      feErrors=(char *)Alloc(256);
      feErrorsLen=256;
      strcpy(feErrors,s);
    }
    else
    {
      if (((int)(strlen(s)+strlen(feErrors)))>=feErrorsLen)
      {
        feErrors=(char *)ReAlloc(feErrors,feErrorsLen,feErrorsLen+256);
        feErrorsLen+=256;
      }
      strcat(feErrors,s);
    }
    strcat(feErrors,"\n");
  }
  else
#endif
  {
#ifdef HAVE_TCL
    if (tclmode)
    {
      //PrintTCLS('E',s);
      //PrintTCLS('E',"\n");
      PrintTCLS('N',s);
      PrintTCLS('N',"\n");
    }
    else
#endif
    {
      fwrite("   ? ",1,5,stderr);
      fwrite(s,1,strlen(s),stderr);
      fwrite("\n",1,1,stderr);
      fflush(stderr);
      if (feProt&PROT_O)
      {
        fwrite("   ? ",1,5,feProtFile);
        fwrite(s,1,strlen(s),feProtFile);
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

void WarnS(char *s)
{
#ifdef HAVE_TCL
  if (tclmode)
  {
    PrintTCLS('W',s);
  }
  else
#endif
  {
    fwrite("// ** ",1,6,stdout);
    fwrite(s,1,strlen(s),stdout);
    fwrite("\n",1,1,stdout);
    fflush(stdout);
    if (feProt&PROT_O)
    {
      fwrite("// ** ",1,6,feProtFile);
      fwrite(s,1,strlen(s),feProtFile);
      fwrite("\n",1,1,feProtFile);
    }
  }
}

void Warn(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *s=(char *)Alloc(256);
  vsprintf(s, fmt, ap);
  WarnS(s);
  Free(s,256);
  va_end(ap);
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
      fePause();
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
//      cols = 0;   //will be done by mwrite('\n');
      mwrite('\n');
    }
  }
}
#endif

void PrintS(char *s)
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

void PrintLn()
{
  PrintS("\n");
}

void Print(char *fmt, ...)
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

void fePause()
{
  uchar c;
  mflush();
#ifndef macintosh
  fputs("pause>",stderr);
#else
  fputs("pause>\n",stderr);
#endif
  c = fgetc(stdin);
  if (((c == '\003') || (c == 'C')) || (c == 'c'))
  {
    m2_end(1);
  }
}

void monitor(char* s, int mode)
{
  if (feProt)
  {
    fclose(feProtFile);
  }
  if ((s!=NULL) && (*s!='\0'))
  {
    feProtFile = fopen(s,"w");
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
      if ((l>MAX_INT_LEN)||((*i) <0))
      {
        s-=l;
        Werror("`%s` greater than %d(max. integer representation)",
                s,INT_MAX);
        return s;
      }
    }
  }
  else *i = 1;
  return s;
}

