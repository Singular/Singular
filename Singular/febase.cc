/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.cc,v 1.35 1998-04-28 13:12:09 Singular Exp $ */
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
extern "C" {
BITSET  verbose = 1
                  | Sy_bit(V_REDEFINE)
                  | Sy_bit(V_LOAD_LIB)
                  | Sy_bit(V_SHOW_USE)
                  | Sy_bit(V_PROMPT)
/*                  | Sy_bit(V_DEBUG_MEM) */
;}
BOOLEAN errorreported = FALSE;
BOOLEAN feBatch;
char *  feErrors=NULL;
int     feErrorsLen=0;

#ifdef macintosh
static  int lines = 0;
static  int cols = 0;
#endif

const char feNotImplemented[]="not implemented";

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

extern "C" char* find_executable(const char* argv0);

#define SINGULAR_RELATIVE_DATA_DIR "LIB"

static char* SearchPath = NULL;
static char* ExpandedExecutable = NULL;

char* feGetExpandedExecutable(const char* argv0)
{
  if (ExpandedExecutable == NULL)
  {
    if (argv0 != NULL)
      ExpandedExecutable = find_executable(argv0);
  }
  return ExpandedExecutable;
}

static char* feRemovePathnameHead(const char* ef)
{
  if (ef != NULL)
  {
    char *temp, *temp2, *temp3;

    temp2 = mstrdup(ef);
    temp3 = temp2;
    temp = NULL;
    while (*temp3 != '\0')
    {
      if (*temp3 == DIR_SEP) temp = temp3;
      temp3++;
    }
    if (temp != NULL)
    {
      *temp = '\0';
    }
    return temp2;
  }
  else 
    return NULL;
}


// Return the file search path for singular w.r.t. the following priorities:
// Env-variables + Relative Data Dir + Burned-in data dir
char* feGetSearchPath(const char* argv0)
{   
  if (SearchPath == NULL) 
  {
    char *env = NULL, *sibbling = NULL, *path;
    int plength = 0, tmp;
  
#ifdef MSDOS
    env=getenv("SPATH");
#else
    env=getenv("SINGULARPATH");
#endif
  
    if (argv0 != NULL)
      sibbling = feRemovePathnameHead(feGetExpandedExecutable(argv0));
  
    if (env != NULL)
      plength = strlen(env) + 1;

    if (sibbling != NULL)
      plength += strlen(sibbling) + strlen(SINGULAR_RELATIVE_DATA_DIR) + 2;
  
    plength += strlen(SINGULAR_DATADIR) + 2;
  
    path = (char*) AllocL(plength*sizeof(char));
    SearchPath = path;
  
    if (env != NULL)
    {
      tmp = strlen(env);
      memcpy(path, env, tmp);
      path = &(path[tmp]);
      *path = FS_SEP;
      path++;
    }
  
    if (sibbling != NULL)
    {
      tmp = strlen(sibbling);
      memcpy(path, sibbling, tmp);
      path = &(path[tmp]);
      *path = DIR_SEP;
      path++;
      tmp = strlen(SINGULAR_RELATIVE_DATA_DIR);
      memcpy(path, SINGULAR_RELATIVE_DATA_DIR, tmp);
      path = &(path[tmp]);
      *path = FS_SEP;
      path++;
      FreeL(sibbling);
    }
  
    tmp = strlen(SINGULAR_DATADIR);
    memcpy(path,SINGULAR_DATADIR, tmp);
    path = &(path[tmp]);
    *path = '\0';
  }
  return SearchPath;
}


FILE * feFopen(char *path, char *mode, char *where,int useWerror)
{
  FILE * f=myfopen(path,mode);
#ifdef __MWERKS__
  if (f!=NULL)
  {
    if (where!=NULL) strcpy(where,path);
    return f;
  }
  char *res;
  int idat=strlen(SINGULAR_DATADIR),ipath=strlen(path);
  char *env=getenv("SINGULARPATH");
  int ienv=0, ii=0;
  if (env!=NULL)
  {
    ienv=strlen(env);
    ii=ienv;
  }
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
    f=myfopen(res,mode);
  }
  if ((f==NULL)&&(idat!=0))
  {
    memcpy(res,SINGULAR_DATADIR,idat);
    memcpy(res+idat,path,ipath);
    res[idat+ipath]='\0';
    f=myfopen(res,mode);
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
    char* spath = feGetSearchPath();
    char *s;

    if (where==NULL) s=(char *)AllocL(250);
    else             s=where;

    if (spath!=NULL)
    {
      char *p,*q;
      p = spath;
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
          f=myfopen(s,mode);
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
void WerrorS(const char *s)
{
#ifdef HAVE_MPSR
  if (feBatch)
  {
    if (feErrors==NULL)
    {
      feErrors=(char *)Alloc(256);
      feErrorsLen=256;
      strcpy(feErrors,(char *)s);
    }
    else
    {
      if (((int)(strlen((char *)s)+strlen(feErrors)))>=feErrorsLen)
      {
        feErrors=(char *)ReAlloc(feErrors,feErrorsLen,feErrorsLen+256);
        feErrorsLen+=256;
      }
      strcat(feErrors,(char *)s);
    }
    strcat(feErrors,"\n");
  }
  else
#endif
  {
#ifdef HAVE_TCL
    if (tclmode)
    {
      //PrintTCLS('E',(char *)s);
      //PrintTCLS('E',"\n");
      PrintTCLS('N',(char *)s);
      PrintTCLS('N',"\n");
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

#ifdef macintosh
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


      
