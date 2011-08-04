#ifndef FEBASE_H
#define FEBASE_H
/****************************************
 * *  Computer Algebra System SINGULAR     *
 * ****************************************/
/* $Id: febase.h 14137 2011-04-11 16:39:34Z hannes $ */
/*
 * * ABSTRACT: basic i/o
 * */

#include <reporter/reporter.h>
#include <resources/feFopen.h>
#include <resources/feResource.h>
#include <kernel/structs.h>

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

extern int myynest;
extern int yylineno;
extern char     my_yylinebuf[80];
extern char    prompt_char; /*1 either '>' or '.'*/
extern int    si_echo;

#ifdef __cplusplus

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
extern "C"
{
  /* readline, linked in: */
  char * fe_fgets_stdin_rl(const char *pr,char *s, int size);

  /* emulated readline: */
  char * fe_fgets_stdin_emu(const char *pr,char *s, int size);

  /* fgets: */
  char * fe_fgets(const char *pr,char *s, int size);

  /* dummy (for batch mode): */
  char * fe_fgets_dummy(const char *pr,char *s, int size);

}
const  char * VoiceName();
void    VoiceBackTrack();
BOOLEAN contBuffer(feBufferTypes typ);
const char *  eati(const char *s, int *i);
BOOLEAN exitBuffer(feBufferTypes typ);
BOOLEAN exitVoice();
void    monitor(void *F, int mode); /* FILE*, int */
BOOLEAN newFile(char* fname, FILE *f=NULL);
void    newBuffer(char* s, feBufferTypes t, procinfo *pname = NULL, int start_lineno = 0);
void *  myynewbuffer();
void    myyoldbuffer(void * oldb);

#endif
#endif
  
