#ifndef FEBASE_H
#define FEBASE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: febase.h,v 1.3 1997-04-09 12:19:42 Singular Exp $ */
/*
* ABSTRACT
*/
#include <stdio.h>
#include "structs.h"

extern int    si_echo, printlevel;
extern int    pagelength, colmax;
extern int    voice;
extern int    fileVoice;
extern char   prompt_char;
extern BOOLEAN tclmode;
extern int    blocklineno;
extern BOOLEAN errorreported;
extern short*  ifswitch;
extern BOOLEAN fe_use_fgets;
extern BOOLEAN feBatch;
extern char*   feErrors;
extern BOOLEAN feProt;
extern FILE*   feProtFile;
extern char    fe_promptstr[];

#define PROT_NONE 0
#define PROT_I    1
#define PROT_O    2
#define PROT_IO   3

/* the C-part: */
#ifdef __cplusplus
extern "C" {
#endif

void   Werror(char *fmt, ...);

#ifdef __cplusplus
}
/* the C++-part: */

enum   /* BufferTypes */
{
  BT_break = 1,  // while, for
  BT_proc,       // proc, script
  BT_example,    // example
  BT_file,       // <"file"
  BT_execute,    // execute
  BT_if,         // if
  BT_else        // else
};

void   I_FEbase(void);
FILE * feFopen(char *path, char *mode, char *where=NULL, int useWerror=FALSE);
void   fePause(void);
void   Print(char* fmt, ...);
void   PrintS(char* s);
void   PrintLn();
void   PrintTCLS(char c, char * s);
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

char * StringAppend(char *fmt, ...);
char * StringAppendS(char *s);
char * StringSet(char *fmt, ...);
char * StringSetS(char* s);
const  char * VoiceName();
const  char * VoiceName(int v);
int    VoiceType();
void   WerrorS(char *s);
void   WarnS(char *s);
void   Warn(char *fmt, ...);
int    contBuffer(int typ);
char * eati(char *s, int *i);
int    exitBuffer(int typ);
int    exitFile(void);
int    exitVoice(void);
#define mflush() fflush(stdout)
void   monitor(char* s,int mode);
void   newBuffer(char* s, int t, char *pname = NULL);
int    newVoice(char* fn);
int    readbuf(char* b, int l);
void   showInput(void);
void * myynewbuffer();
void   myyoldbuffer(void * oldb);

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
    char * fe_fgets_stdin_rl(char *pr,char *s, int size);
    #define fe_fgets_stdin(A,B) fe_fgets_stdin_rl(fe_promptstr,A,B)
  #else
    #define fe_fgets_stdin(A,B) fgets(A,B,stdin)
  #endif
#endif
#endif
#endif
