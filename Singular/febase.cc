/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.cc,v 1.7 1997-04-24 18:01:30 Singular Exp $ */
/*
* ABSTRACT: i/o system, handling of 'voices'
*/

#include <stdlib.h>
#include <string.h>
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

class Voices
{
  private:
    void Init()
    {
      memset(this,0,sizeof(*this));
      v_lineno = 1;
    }
  public:
    int    v_lineno;        // lineno, to restore in recursion
    int    v_oldlineno;     // lineno, to restore at exit
    int    typ;             // buffer type: see BT_..
    int    v_echo;          // echo, to restore in recursion
    int    v_fileVoice;     // to be restored in recursion
    int    v_inputswitch;   // ??
    FILE * files;           // file handle
    long   fptr;            // file | buffer seek
    char*  filename;        // file name
    char * buffer;          // buffer pointer

  void Next();
  Voices() { Init(); }
  Voices * VFile(char* fname);
  Voices * Buffer(char* buf, int t);
  int Exit();
} ;


extern FILE* yyin;

#define INITIAL_PRINT_BUFFER 24*1024
static int feBufferLength=INITIAL_PRINT_BUFFER;
static char * feBuffer=(char *)Alloc(INITIAL_PRINT_BUFFER);

#define START_LEVMAX 32
int     levmax       = START_LEVMAX;
Voices *currentVoice = NULL;
Voices *FileAttribs  =(Voices *)Alloc(START_LEVMAX*sizeof(Voices));
short  *ifswitch     =(short *)Alloc0(START_LEVMAX*sizeof(short));
        /*1 ifswitch==0: no if statement, else is invalid
        *           ==1: if (0) processed, execute else
        *           ==2: if (1) processed, else allowed but not executed
        */
int     si_echo = 0;
int     printlevel = 0;
int     fileVoice = 0;
#ifndef macintosh
int     pagelength = 24;
#else
int     pagelength = -1;
#endif
int     colmax = 80;
int     voice = 0;
int     inputswitch = 0;
int     blocklineno = 0;
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

/*2
* fopen, but use 'SingularPath' from environment and '/usr/local/Singular'
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
  if (where!=NULL) strcpy(where,path);
  if ((*mode=='a') ||(*mode=='w') || (path[0]==DIR_SEP)||(path[0]=='.'))
    return fopen(path,mode);
  char found = 0;
  FILE * f=NULL;
#ifdef MSDOS
  char *env=getenv("SPATH");
#else
  char *env=getenv("SingularPath");
#endif
  char *s;
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
  if (where!=NULL) strcpy(s/*where*/,path);
  f=fopen(path,mode);
#ifndef macintosh
  if (f==NULL)
  {
    strcpy(s,"/usr/local/Singular/");
    strcat(s,path);
    f=fopen(s,mode);
  }
#endif
  if (where==NULL) FreeL((ADDRESS)s);
  if ((f==NULL)&&(useWerror))
    Werror("cannot open `%s`",path);
  return f;
}

/*2
* the name of the current 'voice': the procname (or filename)
*/
const char * VoiceName()
{
  if (FileAttribs[fileVoice].filename!=NULL)
    return FileAttribs[fileVoice].filename;
  return sNoName;
}

/*2
* the name of the 'voice' number 'i': the procname (or filename)
*/
const char * VoiceName(int i)
{
  if (FileAttribs[i].filename!=NULL)
    return FileAttribs[i].filename;
  return sNoName;
}

/*2
* the type of the current voice:BT_proc, BT_example, BT_file
*/
int VoiceType()
{
  int i=fileVoice;
  while ((FileAttribs[i].typ!=BT_proc)
  &&(FileAttribs[i].typ!=BT_example)
  &&(FileAttribs[i].typ!=BT_file)
  &&(i>0))
    i--;
  return FileAttribs[i].typ;
}
/*2
* start the file 'fname' (STDIN is stdin) in the current voice (cf.newVoice)
*/
Voices * Voices::VFile(char* fname)
{
  if (strcmp(fname,"STDIN") == 0)
  {
    yyin = stdin;
    v_inputswitch = 0;
  }
  else
  {
    yyin = feFopen(fname,"r",NULL,TRUE);
    v_inputswitch = -1;
  }
  files      = yyin;
  filename   = mstrdup(fname);
  v_echo     = si_echo;
  fileVoice  = voice;
  yylineno   = 1;
  if (files==NULL)
  {
    inputswitch = 0;
    exitVoice();
    return NULL;
  }
  inputswitch= v_inputswitch;
  return this;
}

/*3
* increment voice counter, allocate new memory
*/
static inline void inc_voice()
{
  voice++;
  if (voice >= levmax)
  {
    FileAttribs=(Voices *)ReAlloc(FileAttribs,
                          levmax*sizeof(Voices),
                          (levmax+16)*sizeof(Voices));
    ifswitch=(short *)ReAlloc(ifswitch,
                          levmax*sizeof(short),
                          (levmax+16)*sizeof(short));
    memset(&ifswitch[levmax],0,16*sizeof(short));
    levmax+=16;
  }
}

/*2
* init a new voice similiar to the current
*/
void Voices::Next()
{
  v_oldlineno = yylineno;
  v_echo      = si_echo;
  v_fileVoice = fileVoice;
  inc_voice();

  currentVoice = &FileAttribs[voice];
  currentVoice->Init();
}

/*2
* start the file 'fname' (STDIN is stdin) as a new voice (cf.VFile)
*/
int newVoice(char* s)
{
  currentVoice->Next();
  return (int)currentVoice->VFile((char *)s);
}

void newBuffer(char* s, int t, char* pname)
{
  currentVoice->Next();
  currentVoice->Buffer(s,t);
  if (pname) currentVoice->filename = mstrdup(pname);
  //printf("start buffer %d typ %d\n",voice,t);
}

Voices * Voices::Buffer(char* buf, int t)
{
  inputswitch = v_inputswitch = t;
  buffer      = buf;
  typ         = t;
  //si_echo        = 0;
  switch (t)
  {
    case BT_example:
    case BT_proc:    v_lineno = yylineno; ::yylineno = 3;     break;
    case BT_file:    v_lineno = yylineno; ::yylineno = 1;     break;
    case BT_if:
    case BT_else:    ::yylineno = v_lineno = blocklineno - 1; break;
    case BT_break:   ::yylineno = v_lineno = blocklineno - 2; break;
  }
  return this;
}

/*2
* after leaving a voice:
* setup everything from the this level
*/
int Voices::Exit()
{
  if (voice >= 0)
  {
    si_echo          = v_echo;
    fileVoice     = v_fileVoice;
    yyin          = files;
    yylineno      = v_oldlineno;
    inputswitch   = v_inputswitch;
    return 0;
  }
  //Print("Exit:%d\n",voice);
  return 1;
}

/*2
* exit Buffer of type 'typ':
* returns 1 if buffer type could not be found
*/
int exitBuffer(int typ)
{
  //printf("exitBuffer: %d\n",typ);
  if (typ == BT_break)  // valid inside for, while. may skip if, else
  {
    /*4 first check for valid buffer type, skip if/else*/
    for (int i=voice; i>0; i--)
    {
      if ((FileAttribs[i].typ == BT_if)
        ||(FileAttribs[i].typ == BT_else)) continue;
      if (FileAttribs[i].typ == BT_break /*typ*/)
      {
        while ((/*typ*/ BT_break != currentVoice->typ)
        && (voice > 0))
        {
          exitVoice();
        }
        return exitVoice();
      }
      else return 1;
    }
    /*4 break not inside a for/while: return an error*/
    if (/*typ*/ BT_break != currentVoice->typ) return 1;
    return exitVoice();
  }

  if ((typ == BT_proc)
  || (typ == BT_example))
  {
    for (int i=voice; i>0; i--)
    {
      if (FileAttribs[i].typ == 0) break;
      if ((FileAttribs[i].typ == BT_proc)
      || (FileAttribs[i].typ == BT_example))
      {
        while ((BT_proc != currentVoice->typ)
          && (BT_example != currentVoice->typ)
        && (voice > 0))
        {
          exitVoice();
        }
        return exitVoice();
      }
    }
  }
  /*4 return not inside a proc: return an error*/
  return 1;
}

/*2
* jump to the beginning of a buffer
*/
int contBuffer(int typ)
{
  if (typ == BT_break)  // valid inside for, while. may skip if, else
  {
    // first check for valid buffer type
    for (int i=voice; i>0; i--)
    {
      if ((FileAttribs[i].typ == BT_if)
        ||(FileAttribs[i].typ == BT_else)) continue;
      if (FileAttribs[i].typ == BT_break /*typ*/)
      {
        while (/*typ*/ BT_break != currentVoice->typ && (voice > i))
        {
          exitVoice();
        }
        currentVoice->fptr = 0L;
        yylineno = currentVoice->v_lineno;
        return 0;
      }
      else return 1;
    }
  }
  return 1;
}

/*2
* leave a file voice
*/
int exitFile()
{
  int oldswitch;

  while ((voice > 0) && (inputswitch > 0))
  {
    exitVoice();
  }
  // now we have left all if-, else-, while-, for-, proc-levels
  // inside this file;
  // if the file is the terminal (inputswitch == 0) and
  // voice >0, so return 1 else return 0
  // (used for EXIT_CMD in CNTRLC-C-handling)
  oldswitch = inputswitch;
  exitVoice();
  #ifdef SIC
  return 1;
  #else
  if ((oldswitch)||(myynest<0)) return 0;
  else return 1;
  #endif
}

/*2
* leave a voice: kill local variables
* setup everything from the previous level (via Exit)
*/
int exitVoice()
{
  if (voice <= 0)   m2_end(0);
  //printf("exitVoice %d, typ %d\n",voice,FileAttribs[voice].typ);
  if (FileAttribs[voice].typ==BT_if)
  {
    ifswitch[voice-1]=2;
  }
  else
  {
    ifswitch[voice-1]=0;
    //if ((FileAttribs[voice].typ==BT_proc)
    //||(FileAttribs[voice].typ==BT_example)
    //||(FileAttribs[voice].typ==0))
    //{
    //  killlocals(myynest);
    //  printf("killlocals %d\n",myynest);
    //}
  }
  if (inputswitch == -1)
  {
    fclose(yyin);
  }
  else if (inputswitch > 0)
  {
    if (FileAttribs[voice].filename!=NULL)
    {
      FreeL((ADDRESS)FileAttribs[voice].filename);
      FileAttribs[voice].filename=NULL;
    }
    if (FileAttribs[voice].buffer!=NULL)
    {
      FreeL((ADDRESS)FileAttribs[voice].buffer);
      FileAttribs[voice].buffer=NULL;
    }
  }
  currentVoice = &FileAttribs[--voice];
  return currentVoice->Exit();
}

int readbuf(char* buf, int l)
{
  char *s;
  char * t = buf;
  int i = 0;
  long fl = currentVoice->fptr;
  if (fl == -1L)
  {
    t[0] = '\0';
    exitVoice();
    return 0;
  }

  s = currentVoice->buffer + fl;
  while (l > 0)
  {
    fl++;
    i++;
    l--;
    *t++ = *s;
    if (*s == '\n')
    {
      *t = '\0';
      if ((si_echo > voice) || (inputswitch == 0) || (traceit&TRACE_SHOW_LINE)
      || (traceit&TRACE_SHOW_LINE1))
      {
        if (currentVoice->filename==NULL)
          Print("(none) %3d%c ",yylineno,prompt_char);
        else if (VoiceType()!=BT_example)
          Print("%s %3d%c ",currentVoice->filename,yylineno,prompt_char);
        prompt_char = '.';
      }
      if (*(s+1) == '\0')
      {
        currentVoice->fptr = -1;
        FreeL((ADDRESS)(currentVoice->buffer));
        currentVoice->buffer=NULL;
        FileAttribs[voice].buffer = NULL;
        exitVoice();
      }
      else
        currentVoice->fptr = fl;
      return i;
    }
    else if (*s == '\0')
    {
      if ((si_echo > voice) || (inputswitch == 0) || (traceit&TRACE_SHOW_LINE)
      || (traceit&TRACE_SHOW_LINE1))
      {
        if (currentVoice->filename==NULL)
          Print("(none) %3d%c ",yylineno,prompt_char);
        else
          Print("%s %3d%c ",currentVoice->filename,yylineno,prompt_char);
        prompt_char = '.';
      }
      currentVoice->fptr = -1;
      FreeL((ADDRESS)(currentVoice->buffer));
      currentVoice->buffer = NULL;
      exitVoice();
      return i-1;
    }
    s++;
  }
  currentVoice->fptr = fl;
  return i;
}

/*2
* init all data structures
*/
void I_FEbase(void)
{
  FileAttribs[0].files       = yyin = stdin;
  FileAttribs[0].filename    = mstrdup("STDIN");
  yylineno = 1;
  currentVoice = &FileAttribs[0];
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

extern "C" {
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
    vfprintf(feProrFile,fmt,ap);
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

void PrintLn()
{
  PrintS("\n");
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

/*2
* print input lines (si_echo or TRACE), set prompt_char
*/
void showInput(void)
{
  if ((inputswitch <= 0) || (traceit&TRACE_SHOW_LINE)
  || (traceit&TRACE_SHOW_LINE1))
  {
    if ((si_echo > voice) || (inputswitch == 0) || (traceit&TRACE_SHOW_LINE)
    || (traceit&TRACE_SHOW_LINE1))
    {
#ifdef HAVE_TCL
      if (tclmode)
      {
         PrintTCL('P',(prompt_char=='>')? 0 : 1,NULL);
      }
      else
#endif
      if (BVERBOSE(V_PROMPT))
      {
        if (inputswitch == 0)
        {
          fe_promptstr[0]=prompt_char;
#ifndef HAVE_READLINE
          PrintS(fe_promptstr);
#endif
        }
        else
        {
          if (currentVoice->filename==NULL)
            Print("(none) %3d%c ",yylineno,prompt_char);
          else
            Print("%s %3d%c ",currentVoice->filename,yylineno,prompt_char);
        }
        prompt_char = '.';
        mflush();
      }
#ifdef macintosh
      cols = 0;
#endif
    }
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

