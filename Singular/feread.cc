/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: feread.cc,v 1.20 1998-10-29 13:15:14 Singular Exp $ */
/*
* ABSTRACT: input from ttys, simulating fgets
*/


#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#ifdef HAVE_TCL
#include "ipid.h"
#endif


#if defined(HAVE_READLINE) && !defined(HAVE_FEREAD)
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>

extern "C" {
 #ifdef READLINE_READLINE_H_OK
  #include <readline/readline.h>
  #ifdef HAVE_READLINE_HISTORY_H
   #include <readline/history.h>
  #endif
 #else /* declare everything we need explicitely and do not rely on includes */
  extern char * rl_readline_name;
  extern char *rl_line_buffer;
  char *filename_completion_function();
  typedef char **CPPFunction ();
  extern char ** completion_matches ();
  extern CPPFunction * rl_attempted_completion_function;
  extern FILE * rl_outstream;
  char * readline ();
  void add_history ();
  int write_history ();
 #endif /* READLINE_READLINE_H_OK */
}

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#include "ipshell.h"

BOOLEAN fe_use_fgets=FALSE;


/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names  or on filenames if it is preceded by " */

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
    return completion_matches (text, filename_completion_function);
  char **m=completion_matches (text, command_generator);
  if (m==NULL)
  {
    m=(char **)malloc(2*sizeof(char*));
    m[0]=(char *)malloc(end-start+2);
    strncpy(m[0],text,end-start+1);
    m[1]=NULL;
  }
  return m;
}

void fe_set_input_mode(void)
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "Singular";
  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = (CPPFunction *)singular_completion;

  if(!fe_use_fgets)
  {

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
  }
}

void fe_reset_input_mode (void)
{
  char *p = getenv("SINGULARHIST");
  if (p != NULL)
  {
    if(!feBatch && !fe_use_fgets && (history_total_bytes()!=0))
      write_history (p);
  }
}

char * fe_fgets_stdin_rl(char *pr,char *s, int size)
{
  if (feBatch)
    return NULL;
  if(fe_use_fgets)
  {
    #ifdef HAVE_TCL
    if (tclmode)
    {
      if(currRing!=NULL) PrintTCLS('P',pr);
      else               PrintTCLS('U',pr);
    }
    else
    #endif
    if (BVERBOSE(V_PROMPT))
    {
      PrintS(pr);
    }
    mflush();
    return fgets(s,size,stdin);
  }

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
/*--------------------------------------------------------------*/
#if !defined(HAVE_READLINE) && defined(HAVE_FEREAD)
extern "C" {
char * fe_fgets_stdin_fe(char *pr,char *s, int size);
}
char * fe_fgets_stdin(char *pr,char *s, int size)
{
  if (feBatch)
    return NULL;
  if(fe_use_fgets)
  {
    #ifdef HAVE_TCL
    if (tclmode)
    {
      if(currRing!=NULL) PrintTCLS('P',pr);
      else               PrintTCLS('U',pr);
    }
    else
    #endif
    if (BVERBOSE(V_PROMPT))
    {
      PrintS(pr);
    }
    mflush();
    return fgets(s,size,stdin);
  }

  if (!BVERBOSE(V_PROMPT))
  {
    pr="";
  }
  mflush();


  return fe_fgets_stdin_fe(pr,s,size);
}
#endif
