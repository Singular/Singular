/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.cc,v 1.41 1998-06-04 15:31:17 obachman Exp $ */
/*
* ABSTRACT: i/o system
*/

#include "mod2.h"

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

#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "subexpr.h"
#include "ipshell.h"

// Define or pass as argument to compiler, if you are building a distribution
// #define MAKE_DISTRIBUTION

#if defined(MAKE_DISTRIBUTION)
#undef SINGULAR_ROOT_DIR
#undef SINGULAR_BIN_DIR

#ifdef WINNT
#define SINGULAR_ROOT_DIR "/Singular"
#define SINGULAR_BIN_DIR "/Singular/Intel-Win"
#else
#define SINGULAR_ROOT_DIR "/usr/local"
#define SINGULAR_BIN_DIR "/usr/local/bin"
#endif

#endif // defined(MAKE_DISTRIBUTION)


#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

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

#ifdef macintosh
#  define  DIR_SEP ':'
#  define  DIR_SEPP ":"
#else
#ifdef MSDOS
#  define  DIR_SEP '\\'
#  define  DIR_SEPP "\\"
#else
#ifdef atarist
#  define  DIR_SEP '\\'
#  define  DIR_SEPP "\\"
#else  /* unix */
#  define  FS_SEP ':'
#  define  DIR_SEP '/'
#  define  DIR_SEPP "/"
#endif  /* atarist */
#endif  /* MSDOS */
#endif  /* macintosh */

#if defined(WINNT)
#  define  FS_SEP ';'
#elsif defined(macintosh)
#define FS_SEP ','
#else
#define FS_SEP ':'
#endif


/*****************************************************************
 *
 * PATH STUFF
 *
 *****************************************************************/

// Define to chatter about path stuff
// #define PATH_DEBUG
static char* feArgv0 = NULL;
static char* feExpandedExecutable = NULL;
static char* feBinDir = NULL;
static char* feSearchPath = NULL;
static char* feInfoProgram = NULL;
static char* feInfoFile = NULL;
static char* feInfoCall = NULL;

extern "C" char* find_executable(const char* argv0);
static char* feRemovePathnameHead(const char* expanded_executable);
static char* CleanUpPath(char* path);
static char* CleanUpName(char* filename);

inline char* feGetExpandedExecutable(const char* argv0)
{
  return (argv0 != NULL ? find_executable(argv0) : (char* ) NULL);
}

inline char* feGetBinDir(const char* expanded_executable)
{
  return feRemovePathnameHead(expanded_executable);
}

// Return the file search path for singular w.r.t. the following steps:
// 1.) SINGULARPATH Env Variable 
// 2.) bindir/LIB
// 3.) bindir/LIB/VERSION
// 4.) bindir/../../Singular/LIB
// 5.) bindir/../../Singular/LIB/VERSION
// 6.) ROOT_DIR/Singular/LIB/
// 7.) ROOT_DIR/Singular/LIB/VERSION
// 8.) Go through all dirs and remove duplicates dirs resp. 
//     those which do not exist
static char* feGetSearchPath(const char* bindir)
{   
  char *env = NULL, *path, *opath;
  int plength = 0, tmp;
  
#ifdef MSDOS
    env=getenv("SPATH");
#else
    env=getenv("SINGULARPATH");
#endif
#ifdef PATH_DEBUG
    printf("I'm going to chatter about the Search path:\n");
#endif
    if (env != NULL)
      plength = strlen(env);

    if (bindir != NULL)
      plength += 4*strlen(bindir);

    plength += 2*strlen(SINGULAR_ROOT_DIR)  
      + 3*(strlen(S_VERSION1) + 1)
      + 24         + 36          + 12       + 6          + 7; 
      // == 6*/LIB + 4*/Singular + 2*/../.. + for colons + some room to breath
  
    opath = (char*) AllocL(plength*sizeof(char));
    path = opath;
  
    if (env != NULL)
    {
      strcpy(path, env);
      path += strlen(path);
      *path=FS_SEP;
      path++;
#ifdef PATH_DEBUG
      *(path +1) = '\0';
      printf("Got from env var: %s\n", opath);
#endif      
    }
  
    if (bindir != NULL)
    {
      sprintf(
        path,
        "%s/LIB%c%s/LIB/%s%c%s/../../Singular/LIB%c%s/../../Singular/LIB/%s%c",
        bindir, FS_SEP,
        bindir, S_VERSION1, FS_SEP,
        bindir, FS_SEP,
        bindir, S_VERSION1, FS_SEP);
#ifdef PATH_DEBUG
      printf("From bindir: %s\n", path);
#endif
      path += strlen(path);
    }
    
    sprintf(path, "%s/Singular/LIB%c%sSingular/LIB/%s",
            SINGULAR_ROOT_DIR, FS_SEP,
            SINGULAR_ROOT_DIR, S_VERSION1);
#ifdef PATH_DEBUG
    printf("From rootdir: %s\n", path);
#endif    
    return CleanUpPath(opath);
}

// Return location of file singular.hlp. Search for it as follows:
// bindir/../doc/singular.hlp
// bindir/../info/singular.hlp
// bindir/../../doc/singular.hlp
// bindir/../../info/singular.hlp
// ROOTDIR/doc/singular.hlp
// ROOTDIR/info/singular.hlp

static char* feGetInfoFile(const char* bindir)
{
  char* hlpfile = (char*) AllocL(max((bindir != NULL ? strlen(bindir) : 0),
                                     strlen(SINGULAR_ROOT_DIR))
                                 + 30);
  if (bindir != NULL)
  {
    sprintf(hlpfile,"%s/../doc/singular.hlp", bindir);
    if (! access(hlpfile, R_OK)) return hlpfile;

    sprintf(hlpfile,"%s/../info/singular.hlp", bindir);
    if (! access(hlpfile, R_OK)) return hlpfile;

    sprintf(hlpfile,"%s/../../doc/singular.hlp", bindir);
    if (! access(hlpfile, R_OK)) return hlpfile;

    sprintf(hlpfile,"%s/../../info/singular.hlp", bindir);
    if (! access(hlpfile, R_OK)) return hlpfile;

    sprintf(hlpfile,"%s/doc/singular.hlp", SINGULAR_ROOT_DIR);
    if (! access(hlpfile, R_OK)) return hlpfile;

    sprintf(hlpfile,"%s/info/singular.hlp", SINGULAR_ROOT_DIR);
    if (! access(hlpfile, R_OK)) return hlpfile;
  }
  *hlpfile = '\0';
  return hlpfile;
}

#ifdef WINNT
#define INFOPROG "info.exe"
#else
#define INFOPROG "info"
#endif

// we first look into bindir, if nothing found there, we use HAVE_INFO
static char* feGetInfoProgram(const char* bindir)
{
  char infoprog[MAXPATHLEN];
  if (bindir != NULL)
  {
    sprintf(infoprog, "%s/%s", bindir, INFOPROG);
    if (! access(infoprog, X_OK)) return mstrdup(infoprog);
  }

  sprintf(infoprog, "%s/%s", SINGULAR_BIN_DIR, INFOPROG);
  if (! access(infoprog, X_OK)) return mstrdup(infoprog);
  
#ifdef HAVE_INFO
  sprintf(infoprog, "%s", HAVE_INFO);
  if (! access(infoprog, X_OK)) return mstrdup(infoprog);
#endif
  // nothing found, let's try "info"
  sprintf(infoprog, "info");
  return mstrdup(infoprog);
}

//
// public routines
//
void feInitPaths(const char* argv0)
{
  feArgv0 = mstrdup(argv0);
}

char* feGetExpandedExecutable()
{
  if (feExpandedExecutable == NULL)
    feExpandedExecutable = feGetExpandedExecutable(feArgv0);
  return feExpandedExecutable;
}

char* feGetBinDir()
{
  if (feBinDir == NULL)
    feBinDir = feGetBinDir(feGetExpandedExecutable());
  return feBinDir;
}

char* feGetSearchPath()
{
  if (feSearchPath == NULL)
    feSearchPath = feGetSearchPath(feGetBinDir());
  return feSearchPath;
}

char* feGetInfoProgram()
{
  if (feInfoProgram == NULL)
    feInfoProgram = feGetInfoProgram(feGetBinDir());
  return feInfoProgram;
}

char* feGetInfoFile()
{
  if (feInfoFile == NULL)
    feInfoFile = feGetInfoFile(feGetBinDir());
  return feInfoFile;
}

char* feGetInfoCall(const char* what)
{
  if (feInfoCall == NULL)
    feInfoCall = (char*) AllocL(strlen(feGetInfoProgram())
                                + strlen(feGetInfoFile())
                                + 100);
  char *infofile = feGetInfoFile();
  
  if (what != NULL && strcmp(what, "index") != 0)
    sprintf(feInfoCall,
            "%s %s %s Index %s", 
            feGetInfoProgram(), 
            (*infofile != '\0' ? "-f" : ""),
            (*infofile != '\0' ? infofile : "Singular"), 
            what);
  else
    sprintf(feInfoCall,
            "%s %s %s", 
            feGetInfoProgram(), 
            (*infofile != '\0' ? "-f" : ""),
            (*infofile != '\0' ? infofile : "Singular"));
  return feInfoCall;
}

//
// auxillary routines
//
static char* feRemovePathnameHead(const char* ef)
{
  if (ef != NULL)
  {
    char* ret = mstrdup(ef);
    char* p = strrchr(ret, DIR_SEP);
    if (p != NULL) *p = '\0';
    return ret;
  }
  return NULL;
}

// remove duplicates dir resp. those which do not exist
static char* CleanUpPath(char* path)
{
#ifdef PATH_DEBUG
  printf("Entered CleanUpPath with: %s\n", path);
#endif  
  if (path == NULL) return path;

  int n_comps = 1, i, j;
  char* opath = path;
  char** path_comps;
  
  for (; *path != '\0'; path++)
  {
    if (*path == FS_SEP) n_comps++;
  }
  

  path_comps = (char**) AllocL(n_comps*sizeof(char*));
  path_comps[0]=opath;
  path=opath;
  i = 1;
  
  if (i < n_comps)
  {
    while (1)
    {
      if (*path == FS_SEP)
      {
        *path = '\0';
        path_comps[i] = path+1;
        i++;
        if (i == n_comps) break;
      }
      path++;
    }
  }
  
  for (i=0; i<n_comps; i++)
    path_comps[i] = CleanUpName(path_comps[i]);
#ifdef PATH_DEBUG
  printf("After CleanUpName: ");
  for (i=0; i<n_comps; i++)
    printf("%s:", path_comps[i]);
  printf("\n");
#endif
  
  for (i=0; i<n_comps;)
  {
#ifdef PATH_DEBUG
    if (access(path_comps[i], X_OK))
      printf("remove %d:%s -- can not access\n", i, path_comps[i]);
#endif    
    if ( ! access(path_comps[i], X_OK))
    {
      // x- permission is granted -- we assume that it is a dir
      for (j=0; j<i; j++)
      {
        if (strcmp(path_comps[j], path_comps[i]) == 0)
        {
          // found a duplicate
#ifdef PATH_DEBUG
          printf("remove %d:%s -- equal to %d:%s\n", j, path_comps[j], i, path_comps[i]);
#endif          
          j = i+1;
          break;
        }
      }
      if (j == i) 
      {
        i++;
        continue;
      }
    }
    // now we can either not access or found a duplicate
    path_comps[i] = NULL;
    for (j=i+1; j<n_comps; j++)
        path_comps[j-1] = path_comps[j];
    n_comps--;
  }

  // assemble everything again
  for (path=opath, i=0;i<n_comps-1;i++)
  {
    strcpy(path, path_comps[i]);
    path += strlen(path);
    *path = FS_SEP;
    path++;
  }
  if (n_comps) strcpy(path, path_comps[i]);
  FreeL(path_comps);
#ifdef PATH_DEBUG
  printf("SearchPath is: %s\n", opath);
#endif
  return opath;
}

static void mystrcpy(char* d, char* s)
{
  assume(d != NULL && s != NULL);
  while (*s != '\0')
  {
    *d = *s;
    d++;
    s++;
  }
  *d = '\0';
}

static char* CleanUpName(char* fname)
{
  char* fn, *s;
  
  for (fn = fname; *fn != '\0'; fn++)
  {
    if (*fn == '/')
    {
      if (*(fn+1) == '\0')
      {
        if (fname != fn) *fn = '\0';
        break;
      }
      if (*(fn + 1) == '/' && (fname != fn))
      {
        mystrcpy(fn, fn+1);
        fn--;
      }
      else if (*(fn+1) == '.')
      {
        if (*(fn+2) == '.' && (*(fn + 3) == '/' || *(fn + 3) == '\0'))
        {
          *fn = '\0';
          s = strrchr(fname, '/');
          if (s != NULL)
          {
            mystrcpy(s+1, fn+3);
            fn = s-1;
          }
          else
          {
            *fn = '/';
          }
        }
        else if (*(fn+2) == '/' || *(fn+2) == '\0')
        {
          mystrcpy(fn+1, fn+3);
          fn--;
        }
      }
    }
  }
  return fname;
}

/*****************************************************************
 *
 * File handling
 *
 *****************************************************************/

FILE * feFopen(char *path, char *mode, char *where,int useWerror)
{
#ifdef __MWERKS__
  FILE * f=myfopen(path,mode);
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
  BOOLEAN tilde = FALSE;
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
