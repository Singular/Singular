/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: feread.cc,v 1.48 2003-03-05 18:02:09 Singular Exp $ */
/*
* ABSTRACT: input from ttys, simulating fgets
*/

#include "mod2.h"

// ----------------------------------------
// system settings:
#undef USE_GCC3
#undef USE_READLINE4
//----------------------------------------
#ifdef ix86_Win
#define READLINE_STATIC
#endif
#include "tok.h"
#include "febase.h"
#ifdef ix86_Win
#define OM_NO_MALLOC_MACROS
#endif
#ifdef ppcMac_darwin
#define OM_NO_MALLOC_MACROS
#endif
#include "omalloc.h"

#include "static.h"

#ifdef HAVE_STATIC
#undef HAVE_DYN_RL
#endif

#ifndef USE_READLINE4
#define rl_filename_completion_function filename_completion_function
#define rl_completion_matches           completion_matches
#endif


#ifdef HAVE_TCL
#include "ipid.h"
#endif

static char * fe_fgets_stdin_init(char *pr,char *s, int size);
char * (*fe_fgets_stdin)(char *pr,char *s, int size)
 = fe_fgets_stdin_init;

/* ===================================================================*/
/* =                   static/dymanic readline                      = */
/* ===================================================================*/
#if defined(HAVE_READLINE) || defined(HAVE_DYN_RL)

#include "ipshell.h"

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

/* Generator function for command completion.  STATE lets us know whether
*   to start from scratch; without any state (i.e. STATE == 0), then we
*   start at the top of the list.
*/
char *command_generator (char *text, int state)
{
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (state==0)
  {
    list_index = 1;
    len = strlen (text);
  }

  /* Return the next name which partially matches from the command list. */
  while ((name = cmds[list_index].name)!=NULL)
  {
    list_index++;

    if (strncmp (name, text, len) == 0)
      return (strdup(name));
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}
#endif

/* ===================================================================*/
/* =                      static readline                           = */
/* ===================================================================*/
/* some procedure are shared with "dynamic readline" */
#if defined(HAVE_READLINE) && !defined(HAVE_FEREAD)
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>

// #undef READLINE_READLINE_H_OK

extern "C" {
  typedef char * (*RL_PROC)(const char*,int);
 #ifdef READLINE_READLINE_H_OK
  #include <readline/readline.h>
  #ifdef HAVE_READLINE_HISTORY_H
   #include <readline/history.h>
  #endif
 #else /* declare everything we need explicitely and do not rely on includes */
  extern char * rl_readline_name;
  extern char *rl_line_buffer;
  char *rl_filename_completion_function(const char*, int);
  typedef char **CPPFunction ();

  extern char ** rl_completion_matches (const char*, RL_PROC);
  extern CPPFunction * rl_attempted_completion_function;
  extern FILE * rl_outstream;
  extern char * readline ();
  extern void add_history ();
  extern int write_history ();
  extern void using_history();
  extern int read_history();
  extern int history_total_bytes();
 #endif /* READLINE_READLINE_H_OK */
 typedef char * (*PROC)();

  typedef char **RL_CPPFunction (const char*, int,int);
}


char * fe_fgets_stdin_rl(char *pr,char *s, int size);

/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names  or on filenames if it is preceded by " */

/* Attempt to complete on the contents of TEXT.  START and END show the
*   region of TEXT that contains the word to complete.  We can use the
*   entire line in case we want to do some simple parsing.  Return the
*   array of matches, or NULL if there aren't any.
*/
char ** singular_completion (char *text, int start, int end)
{
  /* If this word is not in a string, then it may be a command
     to complete.  Otherwise it may be the name of a file in the current
     directory. */
  if (rl_line_buffer[start-1]=='"')
    return rl_completion_matches (text, (RL_PROC)rl_filename_completion_function);
  char **m=rl_completion_matches (text, (RL_PROC)command_generator);
  if (m==NULL)
  {
    m=(char **)malloc(2*sizeof(char*));
    m[0]=(char *)malloc(end-start+2);
    strncpy(m[0],text,end-start+1);
    m[1]=NULL;
  }
  return m;
}

char * fe_fgets_stdin_rl(char *pr,char *s, int size)
{
  if (!BVERBOSE(V_PROMPT))
  {
    pr="";
  }
  mflush();

  char *line;
  line = readline (pr);

  if (line==NULL)
    return NULL;

  if (*line!='\0')
  {
    add_history (line);
  }
  int l=strlen(line);
  if (l>=size-1)
  {
    strncpy(s,line,size);
  }
  else
  {
    strncpy(s,line,l);
    s[l]='\n';
    s[l+1]='\0';
  }
  free (line);

  return s;
}
#endif

/* ===================================================================*/
/* =                    emulated readline                           = */
/* ===================================================================*/
#if !defined(HAVE_READLINE) && defined(HAVE_FEREAD)
extern "C" {
char * fe_fgets_stdin_fe(char *pr,char *s, int size);
}
char * fe_fgets_stdin_emu(char *pr,char *s, int size)
{
  if (!BVERBOSE(V_PROMPT))
  {
    pr="";
  }
  mflush();
  return fe_fgets_stdin_fe(pr,s,size);
}
#endif

/* ===================================================================*/
/* =                     dynamic readline                           = */
/* ===================================================================*/
/* some procedure are shared with "static readline" */

#if defined(HAVE_DYN_RL)
#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/file.h>
//#include <sys/stat.h>
//#include <sys/errno.h>
//#include <dlfcn.h>
#include "mod_raw.h"

extern "C" {
  typedef char* (*String_Func)();
  typedef void  (*Void_Func)();
  typedef int  (*Int_Func)();
  typedef char** (*CharStarStar_Func)();
  char *(*fe_filename_completion_function)(); /* 3 */
  char *(* fe_readline) ();                   /* 4 */
  void (*fe_add_history) ();                  /* 5 */
  char ** fe_rl_readline_name;                /* 6 */
  char **fe_rl_line_buffer;                   /* 7 */
  char **(*fe_completion_matches)();          /* 8 */
  typedef char **CPPFunction ();
  CPPFunction **fe_rl_attempted_completion_function; /* 9 */
  FILE ** fe_rl_outstream;                    /* 10 */
  int (*fe_write_history) ();                 /* 11 */
  int (*fe_history_total_bytes) ();           /* 12 */
  void (*fe_using_history) ();                /* 13 */
  int (*fe_read_history) ();                  /* 14 */
}

void * fe_rl_hdl=NULL;

char * fe_fgets_stdin_rl(char *pr,char *s, int size);
static int fe_init_dyn_rl()
{
  int res=0;
  loop
  {
    #if defined(HPUX_9) || defined(HPUX_10)
    fe_rl_hdl=dynl_open("libreadline.sl");
    if (fe_rl_hdl==NULL)
      fe_rl_hdl=dynl_open("/lib/libreadline.sl");
    if (fe_rl_hdl==NULL)
      fe_rl_hdl=dynl_open("/usr/lib/libreadline.sl");
    #else
    fe_rl_hdl=dynl_open("libreadline.so");
    #endif
    if (fe_rl_hdl==NULL) { return 1;}

    fe_filename_completion_function= (String_Func)
      dynl_sym(fe_rl_hdl, "filename_completion_function");
    if (fe_filename_completion_function==NULL) { res=3; break; }
    fe_readline=(String_Func)dynl_sym(fe_rl_hdl,"readline");
    if (fe_readline==NULL) { res=4; break; }
    fe_add_history=(Void_Func)dynl_sym(fe_rl_hdl,"add_history");
    if (fe_add_history==NULL) { res=5; break; }
    fe_rl_readline_name=(char**)dynl_sym(fe_rl_hdl,"rl_readline_name");
    if (fe_rl_readline_name==NULL) { res=6; break; }
    fe_rl_line_buffer=(char**)dynl_sym(fe_rl_hdl,"rl_line_buffer");
    if (fe_rl_line_buffer==NULL) { res=7; break; }
    fe_completion_matches=(CharStarStar_Func)dynl_sym(fe_rl_hdl,"completion_matches");
    if (fe_completion_matches==NULL) { res=8; break; }
    fe_rl_attempted_completion_function=
      (CPPFunction**) dynl_sym(fe_rl_hdl,"rl_attempted_completion_function");
    if (fe_rl_attempted_completion_function==NULL) { res=9; break; }
    fe_rl_outstream=(FILE**)dynl_sym(fe_rl_hdl,"rl_outstream");
    if (fe_rl_outstream==NULL) { res=10; break; }
    fe_write_history=(Int_Func)dynl_sym(fe_rl_hdl,"write_history");
    if (fe_write_history==NULL) { res=11; break; }
    fe_history_total_bytes=(Int_Func)dynl_sym(fe_rl_hdl,"history_total_bytes");
    if (fe_history_total_bytes==NULL) { res=12; break; }
    fe_using_history=(Void_Func)dynl_sym(fe_rl_hdl,"using_history");
    if (fe_using_history==NULL) { res=13; break; }
    fe_read_history=(Int_Func)dynl_sym(fe_rl_hdl,"read_history");
    if (fe_read_history==NULL) { res=14; break; }
    return 0;
  }
  dynl_close(fe_rl_hdl);
  return res;
}

/* Attempt to complete on the contents of TEXT.  START and END show the
*   region of TEXT that contains the word to complete.  We can use the
*   entire line in case we want to do some simple parsing.  Return the
*   array of matches, or NULL if there aren't any.
*/
char ** singular_completion (char *text, int start, int end)
{
  /* If this word is not in a string, then it may be a command
     to complete.  Otherwise it may be the name of a file in the current
     directory. */
  if ((*fe_rl_line_buffer)[start-1]=='"')
    return (*fe_completion_matches) (text, *fe_filename_completion_function);
  char **m=(*fe_completion_matches) (text, command_generator);
  if (m==NULL)
  {
    m=(char **)malloc(2*sizeof(char*));
    m[0]=(char *)malloc(end-start+2);
    strncpy(m[0],text,end-start+1);
    m[1]=NULL;
  }
  return m;
}

char * fe_fgets_stdin_drl(char *pr,char *s, int size)
{
  if (!BVERBOSE(V_PROMPT))
  {
    pr="";
  }
  mflush();

  char *line;
  line = (*fe_readline) (pr);

  if (line==NULL)
    return NULL;

  if (*line!='\0')
  {
    (*fe_add_history) (line);
  }
  int l=strlen(line);
  if (l>=size-1)
  {
    strncpy(s,line,size);
  }
  else
  {
    strncpy(s,line,l);
    s[l]='\n';
    s[l+1]='\0';
  }
  free (line);

  return s;
}
#endif

/* ===================================================================*/
/* =                        fgets                                   = */
/* ===================================================================*/
char * fe_fgets(char *pr,char *s, int size)
{
  if (BVERBOSE(V_PROMPT))
  {
    fprintf(stdout,pr);
  }
  mflush();
  return fgets(s,size,stdin);
}

/* ===================================================================*/
/* =       init for static rl, dyn. rl, emu. rl                     = */
/* ===================================================================*/
static char * fe_fgets_stdin_init(char *pr,char *s, int size)
{
#if defined(HAVE_READLINE) && !defined(HAVE_FEREAD)
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "Singular";
  /* Tell the completer that we want a crack first. */
#ifdef USE_GCC3
  rl_attempted_completion_function = (RL_CPPFunction *)singular_completion;
#else
  rl_attempted_completion_function = (CPPFunction *)singular_completion;
#endif

  /* set the output stream */
  if(!isatty(STDOUT_FILENO))
  {
    #ifdef atarist
      rl_outstream = fopen( "/dev/tty", "w" );
    #else
      rl_outstream = fopen( ttyname(fileno(stdin)), "w" );
    #endif
  }

  /* try to read a history */
  using_history();
  char *p = getenv("SINGULARHIST");
  if (p != NULL)
  {
    read_history (p);
  }
  fe_fgets_stdin=fe_fgets_stdin_rl;
  return(fe_fgets_stdin_rl(pr,s,size));
#endif
#ifdef HAVE_DYN_RL
  /* do dynamic loading */
  int res=fe_init_dyn_rl();
  if (res!=0)
  {
    //if (res==1)
    //  WarnS("dynamic loading of libreadline failed");
    //else
    //  Warn("dynamic loading failed: %d\n",res);
    if (res!=1)
      Warn("dynamic loading failed: %d\n",res);
    fe_fgets_stdin=fe_fgets_stdin_emu;
    return fe_fgets_stdin_emu(pr,s,size);
  }
  /* Allow conditional parsing of the ~/.inputrc file. */
  (*fe_rl_readline_name) = "Singular";
  /* Tell the completer that we want a crack first. */
  (*fe_rl_attempted_completion_function) = (CPPFunction *)singular_completion;

  /* set the output stream */
  if(!isatty(STDOUT_FILENO))
  {
    #ifdef atarist
      *fe_rl_outstream = fopen( "/dev/tty", "w" );
    #else
      *fe_rl_outstream = fopen( ttyname(fileno(stdin)), "w" );
    #endif
  }

  /* try to read a history */
  (*fe_using_history)();
  char *p = getenv("SINGULARHIST");
  if (p != NULL)
  {
    (*fe_read_history) (p);
  }
  fe_fgets_stdin=fe_fgets_stdin_drl;
  return fe_fgets_stdin_drl(pr,s,size);
#else
  #if !defined(HAVE_READLINE) && defined(HAVE_FEREAD)
    fe_fgets_stdin=fe_fgets_stdin_emu;
    return(fe_fgets_stdin_emu(pr,s,size));
  #else
    fe_fgets_stdin=fe_fgets;
    return(fe_fgets(pr,s,size));
  #endif
#endif
}

/* ===================================================================*/
/* =                          TCL                                   = */
/* ===================================================================*/
#ifdef HAVE_TCL
/* tcl: */
char * fe_fgets_tcl(char *pr,char *s, int size)
{
  if(currRing!=NULL) PrintTCLS('P',pr);
  else               PrintTCLS('U',pr);
  mflush();
  return fgets(s,size,stdin);
}

#endif

/* ===================================================================*/
/* =                      batch mode                                = */
/* ===================================================================*/
/* dummy (for batch mode): */
char * fe_fgets_dummy(char *pr,char *s, int size)
{
  return NULL;
}

/* ===================================================================*/
/* =          fe_reset_input_mode (all possibilities)               = */
/* ===================================================================*/
void fe_reset_input_mode ()
{
#if defined(HAVE_DYN_RL)
  char *p = getenv("SINGULARHIST");
  if ((p != NULL) && (fe_history_total_bytes != NULL))
  {
    if((*fe_history_total_bytes)()!=0)
      (*fe_write_history) (p);
  }
#endif
#if defined(HAVE_READLINE) && !defined(HAVE_FEREAD) && !defined(HAVE_DYN_RL)
  char *p = getenv("SINGULARHIST");
  if (p != NULL)
  {
    if(history_total_bytes()!=0)
      write_history (p);
  }
#endif
#if !defined(MSDOS) && (defined(HAVE_FEREAD) || defined(HAVE_DYN_RL))
  #ifndef HAVE_ATEXIT
  fe_reset_fe(NULL,NULL);
  #else
  fe_reset_fe();
  #endif
#endif
}
