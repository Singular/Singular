#ifndef FEBASE_H
#define FEBASE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.h,v 1.12 1998-04-15 16:44:57 Singular Exp $ */
/*
* ABSTRACT
*/
#include <stdio.h>
#include <string.h>
#include "structs.h"

extern char*  feErrors;
extern FILE*  feProtFile;
extern FILE*  feFilePending; /*temp. storage for grammar.y */
extern char   fe_promptstr[];
extern int    si_echo, printlevel;
extern int    pagelength, colmax;
extern int    yy_blocklineno;
extern int    yy_noeof;
extern char   prompt_char;
extern const char feNotImplemented[];
#ifdef HAVE_TCL
extern BOOLEAN tclmode;
#endif
extern BOOLEAN errorreported;
extern BOOLEAN fe_use_fgets;
extern BOOLEAN feBatch;
extern BOOLEAN feProt;

#define PROT_NONE 0
#define PROT_I    1
#define PROT_O    2
#define PROT_IO   3

/* the C-part: */
#ifdef __cplusplus
extern "C" {
#endif

void   Werror(char *fmt, ...);
void   WerrorS(char *s);

#ifdef __cplusplus
}
/* the C++-part: */

enum   feBufferTypes 
{
  BT_none  = 0,  // entry level
  BT_break = 1,  // while, for
  BT_proc,       // proc
  BT_example,    // example
  BT_file,       // <"file"
  BT_execute,    // execute
  BT_if,         // if
  BT_else        // else
};

enum   feBufferInputs 
{
  BI_stdin = 1,
  BI_buffer,
  BI_file
};

char*   feGetSearchPath(const char* argv0 = NULL);
char*   feGetExpandedExecutable(const char* argv0 = NULL);
FILE *  feFopen(char *path, char *mode, char *where=NULL, int useWerror=FALSE);
void    fePause(void);
void    Print(char* fmt, ...);
void    PrintS(char* s);
void     PrintLn();
void    PrintTCLS(char c, char * s);
#ifndef macintosh
inline void PrintTCL(char c, int l,char *s)
{
#ifdef HAVE_TCL
  if (s!=NULL) printf("%c:%d:%s\n",c,l,s);
  else         printf("%c:%d:\n",c,l);
  fflush(stdout);
#endif
}
#endif

char *  StringAppend(char *fmt, ...);
char *  StringAppendS(char *s);
char *  StringSet(char *fmt, ...);
char *  StringSetS(char* s);
const  char * VoiceName();
void    VoiceBackTrack();
void    WarnS(char *s);
void    Warn(char *fmt, ...);
BOOLEAN contBuffer(feBufferTypes typ);
char *  eati(char *s, int *i);
BOOLEAN exitBuffer(feBufferTypes typ);
BOOLEAN exitVoice();
#define mflush() fflush(stdout)
void    monitor(char* s,int mode);
BOOLEAN newFile(char* fname, FILE *f=NULL);
void    newBuffer(char* s, feBufferTypes t, procinfo *pname = NULL, int start_lineno = 0);
void *  myynewbuffer();
void    myyoldbuffer(void * oldb);

/* assume(x) -- a handy macro for assumptions */
#ifdef NDEBUG
/* empty macro, if NDEBUG */
#define assume(x) ((void*) 0)
#else /* ! NDEBUG */
#define assume(x) _assume(x, __FILE__, __LINE__)
#define _assume(x, f, l)                                        \
do                                                              \
{                                                               \
  if (! (x))                                                    \
  {                                                             \
    Warn("Internal assume violation: file %s line %d\n", f, l); \
  }                                                             \
}                                                               \
while (0)
#endif /* NDEBUG */

class Voice
{
  public:
    Voice  * next;
    Voice  * prev;
    char   * filename;    // file name or proc name
    procinfo * pi;        // proc info
    void   * oldb;        // internal scanner buffer
    int    start_lineno;  // lineno, to restore in recursion
    int    curr_lineno;   // current lineno
    feBufferInputs   sw;  // BI_stdin: read from STDIN
                          // BI_buffer: buffer
                          // BI_file: files
    char   ifsw;          // if-switch:
            /*1 ifsw==0: no if statement, else is invalid
            *       ==1: if (0) processed, execute else
            *       ==2: if (1) processed, else allowed but not executed
            */
    feBufferTypes   typ;  // buffer type: see BT_..
    // for files only:
    FILE * files;         // file handle
    // for buffers only:
    char * buffer;        // buffer pointer
    long   fptr;          // current position in buffer

  Voice() { memset(this,0,sizeof(*this));}
  feBufferTypes Typ();
  void Next();
} ;

extern Voice  *currentVoice;

Voice * feInitStdin();

/* feread.cc: */
#ifdef HAVE_FEREAD
  void fe_set_input_mode (void);
  void fe_temp_set (void);
  void fe_temp_reset (void);
  char * fe_fgets_stdin(char *s, int size);
  #ifndef MSDOS
    #ifdef HAVE_ATEXIT
      void fe_reset_input_mode (void);
    #else
      void fe_reset_input_mode (int i, void *v);
    #endif
  #else
    #define fe_reset_input_mode()
  #endif
#else
  #ifdef HAVE_READLINE
    void fe_set_input_mode (void);
    void fe_reset_input_mode (void);
    char * fe_fgets_stdin_rl(char *pr,char *s, int size);
    #define fe_fgets_stdin(A,B) fe_fgets_stdin_rl(fe_promptstr,A,B)
  #else
    #define fe_fgets_stdin(A,B) fgets(A,B,stdin)
  #endif
#endif
#endif
#endif
