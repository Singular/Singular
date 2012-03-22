/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: input from ttys, simulating fgets
*/

#include <kernel/mod2.h>

// ----------------------------------------
// system settings:

#undef USE_READLINE4

//----------------------------------------
#ifdef ix86_Win
#define READLINE_STATIC
#endif
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/options.h>

#include <Singular/static.h>

#ifdef HAVE_STATIC
#undef HAVE_DYN_RL
#endif

#if defined(HAVE_DYN_RL)
#include <unistd.h>
#endif

static char * fe_fgets_stdin_init(const char *pr,char *s, int size);
char * (*fe_fgets_stdin)(const char *pr,char *s, int size)
 = fe_fgets_stdin_init;

extern char *iiArithGetCmd(int);

/* ===================================================================*/
/* =                   static/dymanic readline                      = */
/* ===================================================================*/
#if defined(HAVE_READLINE) || defined(HAVE_DYN_RL) || defined(HAVE_LIBREADLINE)

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

/* Generator function for command completion.  STATE lets us know whether
*   to start from scratch; without any state (i.e. STATE == 0), then we
*   start at the top of the list.
*/
extern "C"
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
  while ((name = iiArithGetCmd(list_index))!=NULL)
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
#if (defined(HAVE_READLINE) || defined(HAVE_LIBREADLINE) || defined(HAVE_DYN_RL))
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
  #endif

  #ifdef RL_VERSION_MAJOR
    #if (RL_VERSION_MAJOR >= 4)
      #define USE_READLINE4
    #endif
  #endif

  #ifndef USE_READLINE4
    #define rl_filename_completion_function filename_completion_function
    #define rl_completion_matches           completion_matches
  #endif
  #ifndef READLINE_READLINE_H_OK  
    /* declare everything we need explicitely and do not rely on includes */
    extern char * rl_readline_name;
    extern char *rl_line_buffer;
    char *rl_filename_completion_function(const char*, int);
    typedef char **CPPFunction ();

    extern char ** rl_completion_matches (const char*, RL_PROC);
    extern CPPFunction * rl_attempted_completion_function;
    extern FILE * rl_outstream;
    extern char * readline (const char *);
    extern void add_history (char *);
    extern int write_history ();
    extern void using_history();
    extern int read_history(char *);
    extern int history_total_bytes();
  #endif /* READLINE_READLINE_H_OK */

  typedef char * (*PROC)();

  typedef char **RL_CPPFunction (const char*, int,int);
}


char * fe_fgets_stdin_rl(const char *pr,char *s, int size);

/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names  or on filenames if it is preceded by " */

/* Attempt to complete on the contents of TEXT.  START and END show the
*   region of TEXT that contains the word to complete.  We can use the
*   entire line in case we want to do some simple parsing.  Return the
*   array of matches, or NULL if there aren't any.
*/
#if defined(HAVE_DYN_RL)
extern "C" 
{
  int fe_init_dyn_rl();
  char *(*fe_filename_completion_function)(); /* 3 */
  char *(* fe_readline) (char *);             /* 4 */
  void (*fe_add_history) (char *);            /* 5 */
  char ** fe_rl_readline_name;                /* 6 */
  char **fe_rl_line_buffer;                   /* 7 */
  char **(*fe_completion_matches)(...);          /* 8 */
  CPPFunction **fe_rl_attempted_completion_function; /* 9 */
  FILE ** fe_rl_outstream;                    /* 10 */
  int (*fe_write_history) ();                 /* 11 */
  int (*fe_history_total_bytes) ();           /* 12 */
  void (*fe_using_history) ();                /* 13 */
  int (*fe_read_history) (char *);            /* 14 */

}
#endif
char ** singular_completion (char *text, int start, int end)
{
  /* If this word is not in a string, then it may be a command
     to complete.  Otherwise it may be the name of a file in the current
     directory. */
#ifdef HAVE_DYN_RL
  #define x_rl_line_buffer (*fe_rl_line_buffer)
  #define x_rl_completion_matches (*fe_completion_matches)
  #define x_rl_filename_completion_function (*fe_filename_completion_function)
#else
  #define x_rl_line_buffer rl_line_buffer
  #define x_rl_completion_matches rl_completion_matches
  #define x_rl_filename_completion_function rl_filename_completion_function
#endif
  if (x_rl_line_buffer[start-1]=='"')
    return x_rl_completion_matches (text, (RL_PROC)x_rl_filename_completion_function);
  char **m=x_rl_completion_matches (text, (RL_PROC)command_generator);
#undef x_rl_line_buffer
#undef x_rl_completion_matches
  if (m==NULL)
  {
    m=(char **)malloc(2*sizeof(char*));
    m[0]=(char *)malloc(end-start+2);
    strncpy(m[0],text,end-start+1);
    m[1]=NULL;
  }
  return m;
}

#ifndef HAVE_DYN_RL
char * fe_fgets_stdin_rl(const char *pr,char *s, int size)
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
#endif

/* ===================================================================*/
/* =                    emulated readline                           = */
/* ===================================================================*/
#if !defined(HAVE_READLINE) && defined(HAVE_FEREAD)
extern "C" {
char * fe_fgets_stdin_fe(const char *pr,char *s, int size);
}
char * fe_fgets_stdin_emu(const char *pr,char *s, int size)
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
char * fe_fgets_stdin_drl(const char *pr,char *s, int size)
{
  if (!BVERBOSE(V_PROMPT))
  {
    pr="";
  }
  mflush();

  char *line;
  line = (*fe_readline) ((char*)pr);

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
char * fe_fgets(const char *pr,char *s, int size)
{
  if (BVERBOSE(V_PROMPT))
  {
    fprintf(stdout,"%s",pr);
  }
  mflush();
  return fgets(s,size,stdin);
}

/* ===================================================================*/
/* =       init for static rl, dyn. rl, emu. rl                     = */
/* ===================================================================*/
static char * fe_fgets_stdin_init(const char *pr,char *s, int size)
{
#if (defined(HAVE_READLINE) || defined(HAVE_LIBREADLINE)) && !defined(HAVE_DYN_RL) && !defined(HAVE_FEREAD)
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "Singular";
  /* Tell the completer that we want a crack first. */
#ifdef USE_READLINE4
  rl_attempted_completion_function = (rl_completion_func_t *)singular_completion;
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
    #ifdef HAVE_FEREAD
    fe_fgets_stdin=fe_fgets_stdin_emu;
    #else
    fe_fgets_stdin=fe_fgets;
    #endif
    return fe_fgets_stdin(pr,s,size);
  }
  else /* could load libreadline: */
  {
    /* Allow conditional parsing of the ~/.inputrc file. */
    *fe_rl_readline_name = "Singular";
    /* Tell the completer that we want a crack first. */
    *fe_rl_attempted_completion_function = (CPPFunction *)singular_completion;
    /* try to read a history */
    (*fe_using_history)();
    char *p = getenv("SINGULARHIST");
    if (p != NULL)
    {
      (*fe_read_history) (p);
    }
  }

  /* set the output stream */
  if(!isatty(STDOUT_FILENO))
  {
    #ifdef atarist
      *fe_rl_outstream = fopen( "/dev/tty", "w" );
    #else
      *fe_rl_outstream = fopen( ttyname(fileno(stdin)), "w" );
    #endif
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
/* =                      batch mode                                = */
/* ===================================================================*/
/* dummy (for batch mode): */
char * fe_fgets_dummy(const char *pr,char *s, int size)
{
  return NULL;
}

