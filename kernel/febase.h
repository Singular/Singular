#ifndef FEBASE_H
#define FEBASE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: basic i/o
*/
#include <stdio.h>
#include <string.h>
#include <kernel/structs.h>

/* define DIR_SEPP, etc */
/* unix , WINNT */
#  define  DIR_SEP '/'
#  define  DIR_SEPP "/"
#  define  UP_DIR ".."
// this might still get reset by feInitResources
extern char fePathSep;

/* define MAXPATHLEN */
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

/* the are versions of limits.h with incorrect values (IRIX_6)
* let's include our own */

#include <omalloc/mylimits.h>

/* OSF/1 and AIX_4 are missing the header for setenv, but the proc exists */
#if defined(DecAlpha_OSF1) || defined(AIX_4)
#ifdef __cplusplus
extern "C"
{
#endif
  int setenv(const char *name, const char *value, int overwrite);
#ifdef __cplusplus
}
#endif
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#ifdef HAVE_LIBREADLINE
  #define HAVE_READLINE 1
#endif

#ifdef HAVE_DYN_RL
  #undef HAVE_READLINE
  #define HAVE_FEREAD 1
#elif ! defined(HAVE_READLINE)
#ifndef ix86_Win
#ifdef HAVE_TERM_H
  #define HAVE_FEREAD 1
#endif
#endif
#endif
#if defined(ix86_Linux) || defined(x86_64_Linux)
  // FEREAD stuff does not work with glibc2
  #undef HAVE_FEREAD
#endif

/*
// These are our versions of fopen and fread They are very similar to
// the usual fopen and fread, except that on reading, they always
// convert "\r\n" into " \n" and "\r" into "\n".
//
// IMPORTANT: do only use myfopen and myfread when reading text,
// do never use fopen and fread
*/
#ifdef ix86_Win
FILE *myfopen(const char *path, const char *mode);
#else
#define myfopen fopen
#endif

size_t myfread(void *ptr, size_t size, size_t nmemb, FILE *stream);

enum noeof_t
{
  noeof_brace = 1,
  noeof_asstring,
  noeof_block,
  noeof_bracket,
  noeof_comment,
  noeof_procname,
  noeof_string
};  /* for scanner.l */

extern char*  feErrors;
extern char*  feArgv0;
extern int    feErrorsLen;
extern FILE*  feProtFile;
extern FILE*  feFilePending; /*temp. storage for grammar.y */
extern char   fe_promptstr[];
extern int    si_echo, printlevel;
extern int    colmax;
extern int    yy_blocklineno;
extern int    yy_noeof;
extern char   prompt_char;
extern const char feNotImplemented[];
extern BOOLEAN errorreported;
extern int     feProt;
extern BOOLEAN feWarn;
extern BOOLEAN feOut;
extern int  traceit ;
#define TRACE_SHOW_PROC   1
#define TRACE_SHOW_LINENO 2
#define TRACE_SHOW_LINE   4
#define TRACE_SHOW_RINGS  8
#define TRACE_SHOW_LINE1  16
#define TRACE_BREAKPOINT  32
#define TRACE_TMP_BREAKPOINT  64
extern int myynest;
extern int yylineno;
extern char     my_yylinebuf[80];

#define PROT_NONE 0
#define PROT_I    1
#define PROT_O    2
#define PROT_IO   3

/* the C-part: */
#define mflush() fflush(stdout)

#ifdef __cplusplus
extern "C" {
#endif
#ifndef __GNUC__
#undef __attribute__
#define __attribute__(A)
#endif

extern void (*WerrorS_callback)(const char *s);

void    Werror(const char *fmt, ...) __attribute__((format(printf,1,2)));
void    WerrorS(const char *s);
void    WarnS(const char *s);
void    Print(const char* fmt, ...) __attribute__((format(printf,1,2)));
/* Print should not produce more than strlen(fmt)+510 characters! */

void    PrintNSpaces(const int n);
void    PrintLn();
void    PrintS(const char* s);
char*   feGetResource(const char id);

#ifdef HAVE_FEREAD
extern BOOLEAN fe_is_raw_tty;
void           fe_temp_reset (void);
#endif

#ifdef __cplusplus
}
/* the C++-part: */

typedef enum { LANG_NONE, LANG_TOP, LANG_SINGULAR, LANG_C, LANG_MAX} language_defs;
// LANG_TOP     : Toplevel package only
// LANG_SINGULAR:
// LANG_C       :
//

class proc_singular
{
public:
  long   proc_start;       // position where proc is starting
  long   def_end;          // position where proc header is ending
  long   help_start;       // position where help is starting
  long   help_end;         // position where help is starting
  long   body_start;       // position where proc-body is starting
  long   body_end;         // position where proc-body is ending
  long   example_start;    // position where example is starting
  long   proc_end;         // position where proc is ending
  int    proc_lineno;
  int    body_lineno;
  int    example_lineno;
  char   *body;
  long help_chksum;
};

struct proc_object
{
//public:
  BOOLEAN (*function)(leftv res, leftv v);
};
union uprocinfodata
{
public:
  proc_singular  s;        // data of Singular-procedure
  struct proc_object    o; // pointer to binary-function
};

typedef union uprocinfodata procinfodata;

class procinfo;
typedef procinfo *         procinfov;

class procinfo
{
public:
  char          *libname;
  char          *procname;
  package       pack;
  language_defs language;
  short         ref;
  char          is_static;        // if set, proc not accessible for user
  char          trace_flag;
  procinfodata  data;
};
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

/*****************************************************************
 *
 * Resource management (feResources.cc)
 *
 *****************************************************************/
// returns value of Resource as read-only string, or NULL
// if Resource not found
// issues warning, if explicitely requested (warn > 0), or
// if warn < 0 and Resource is gotten for the first time
// Always quiet if warn == 0
char* feResource(const char id, int warn = -1);
char* feResource(const char* key, int warn = -1);
// This needs to be called before the first call to feResource
// Initializes Resources, SearchPath, and extends PATH
void feInitResources(char* argv0);
// Re-inits resources, should be called after changing env. variables
void feReInitResources();
// Prints resources into string with StringAppend, etc
void feStringAppendResources(int warn = -1);

/*****************************************************************
 *
 * help system (fehelp.cc)
 *
 *****************************************************************/
// if str != NULL display help for str
// display general help, otherwise
void feHelp(char* str = NULL);
// if browser != NULL or feOpt("browser") != NULL
//    set HelpBrowser to browser
// otherwise, if browser was already set, leave as is,
//            if not, choose first available browser
// return string identifying current browser
// keeps feOpt("browser") up-to-date
// Optional warn argument is as in feResource
const char* feHelpBrowser(char* browser = NULL, int warn = -1);
void feStringAppendBrowsers(int warn = -1);

/*****************************************************************
 *
 * version Id
 *
 *****************************************************************/
/* extern unsigned long feVersionId; */

/*****************************************************************
 *
 * File Stuff
 *
 *****************************************************************/
FILE *  feFopen(const char *path, const char *mode, char *where=NULL,
                int useWerror=FALSE, int path_only=FALSE);

char *  StringAppend(const char *fmt, ...);
char *  StringAppendS(const char *s);
char *  StringSetS(const char* s);
const  char * VoiceName();
void    VoiceBackTrack();
void    Warn(const char *fmt, ...);
BOOLEAN contBuffer(feBufferTypes typ);
const char *  eati(const char *s, int *i);
BOOLEAN exitBuffer(feBufferTypes typ);
BOOLEAN exitVoice();
void    monitor(void *F, int mode); /* FILE*, int */
BOOLEAN newFile(char* fname, FILE *f=NULL);
void    newBuffer(char* s, feBufferTypes t, procinfo *pname = NULL, int start_lineno = 0);
void *  myynewbuffer();
void    myyoldbuffer(void * oldb);

class Voice
{
  public:
    Voice  * next;
    Voice  * prev;
    char   * filename;    // file name or proc name
    procinfo * pi;        // proc info
    void   * oldb;        // internal scanner buffer
    // for files only:
    FILE * files;         // file handle
    // for buffers only:
    char * buffer;        // buffer pointer
    long   fptr;          // current position in buffer

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

  Voice() { memset(this,0,sizeof(*this));}
  feBufferTypes Typ();
  void Next();
} ;

extern Voice  *currentVoice;

Voice * feInitStdin(Voice *pp);

/* feread.cc: */

/* the interface for reading: */
extern  char * (*fe_fgets_stdin)(const char *pr,char *s, int size);

#ifdef HAVE_DYN_RL
char * fe_fgets_stdin_drl(const char *pr,char *s, int size);
#endif

extern "C" void fe_reset_input_mode();

extern "C" {
#ifndef HAVE_ATEXIT
void fe_reset_fe (int i, void *v);
#else
void fe_reset_fe (void);
#endif
}


/* possible implementations: */
#ifdef __cplusplus
extern "C"
{
#endif
  /* readline, linked in: */
  char * fe_fgets_stdin_rl(const char *pr,char *s, int size);

  /* emulated readline: */
  char * fe_fgets_stdin_emu(const char *pr,char *s, int size);

  /* fgets: */
  char * fe_fgets(const char *pr,char *s, int size);

  /* dummy (for batch mode): */
  char * fe_fgets_dummy(const char *pr,char *s, int size);

#ifdef __cplusplus
}
#endif
#endif /* c++ only */

/* everything in between calls to these procedures is printed into a string
 * which is returned by SprintEnd()
 */
void SPrintStart();
char* SPrintEnd();


#endif /* ifndef FEBASE_H */
