/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: emacs.cc,v 1.4 1999-08-26 09:58:27 obachman Exp $ */
/*
* ABSTRACT: Esingular main file
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "mod2.h"
#include "version.h"

#define ESINGULAR

#define Alloc   malloc
#define AllocL  malloc
#define mstrdup strdup
#define Free    free
#define FreeL   free
#define Alloc   malloc
#define AllocL  malloc
#ifndef BOOLEAN
#define BOOLEAN int
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#  define  DIR_SEP '/'
#  define  DIR_SEPP "/"
#  define  UP_DIR ".."
#define Warn  printf
#define WarnS printf
#define StringAppend printf
#define Print printf

#define feReportBug(s) fePrintReportBug(s, __FILE__, __LINE__)
void fePrintReportBug(char* msg, char* file, int line)
{
  WarnS("YOU HAVE FOUND A BUG IN SINGULAR. ");
  WarnS("Please, email the following output to singular@mathematik.uni-kl.de ");
  Warn("Bug occured at %s:%d ", file, line);
  Warn("Message: %s ", msg);
  Warn("Version: " S_UNAME S_VERSION1 " (%d) " __DATE__ __TIME__,
       SINGULAR_VERSION_ID);
}

void assume_violation(char* file, int line)
{
  fprintf(stderr, "Internal assume violation: file %s line %d\n", file, line);
}
   
#include "find_exec.c"
#include "feResource.cc"
#include "feCmdLineOptions.cc"

int main(int argc, char** argv)
{
  char* singular = NULL;
  char* emacs = NULL;
  char* emacs_dir = NULL;
  char* emacs_load = NULL;
  int no_emacs_call = 0;
  char cwd[MAXPATHLEN];
  
  // parse-cmdline options
  
  feInitResources(argv[0]);
  feResource('S');
  feResource('b');
  feResource('r');
  
  int optc, option_index;
  
  while ((optc = getopt_long(argc, argv, SHORT_OPTS_STRING, 
                             longopts, &option_index))
        != EOF)
  {
    switch(optc)
    {
        case 'h':
          mainHelp(argv[0]);
          exit(0);
          
        case '?':
        case ':':
          mainUsage(argv[0]);
          exit(1);

        case  LONG_OPTION_RETURN:
          if (strcmp(longopts[option_index].name, LON_EMACS) == 0)
          {
            emacs = optarg;
          }
          else if (strcmp(longopts[option_index].name, LON_EMACS_DIR) == 0)
          {
            emacs_dir = optarg;
          }
          else if (strcmp(longopts[option_index].name, LON_EMACS_LOAD) == 0)
          {
            emacs_load = optarg;
          }
          else if (strcmp(longopts[option_index].name, LON_SINGULAR) == 0)
          {
            singular = optarg;
          }
          else if (strcmp(longopts[option_index].name, LON_NO_EMACS_CALL) == 0)
          {
            no_emacs_call = 1;
          }
          else
          {
            break;
          }
          // delete options from option-list
          if (optind > 2 && *argv[optind-1] != '-' && 
              optarg != NULL && longopts[option_index].has_arg)
          {
            argv[optind-2] = NULL;
          }
          argv[optind-1] = NULL;
    }
  } 
  
  // make sure  emacs, singular, emacs_dir, emacs_load are set
  if (emacs == NULL) emacs = feResource("emacs", 0);
  if (emacs == NULL)
  {
    fprintf(stderr, "Error: Can't find emacs executable. \nExpected it at %s\n. Specify alternative with --emacs option,\n or set EMACS environment variable.\n", 
            feResourceDefault("emacs"));
    mainUsage(argv[0]);
    exit(1);
  }
            
  if (singular == NULL) singular = feResource("SingularEmacs", 0);
  if (singular == NULL)
  {
    fprintf(stderr, "Error: Can't find singular executable.\nExpected it at %s\nSpecify with --singular option,\n  or set SINGULAR_EMACS environment variable.\n", 
            feResourceDefault("SingularEmacs"));
    mainUsage(argv[0]);
    exit(1);
  }
    
  if (emacs_dir == NULL) emacs_dir = feResource("EmacsDir", 0);
  if (emacs_dir == NULL)
  {
    fprintf(stderr, "Error: Can't find emacs directory for Singular lisp files. \nExpected it at %s\nSpecify with --emacs_dir option,\n  or set SINGULAR_EMACS_DIR environment variable.\n", 
            feResourceDefault("EmacsDir"));
    mainUsage(argv[0]);
    exit(1);
  }

  if (emacs_load == NULL) 
  {
    // look into env variable
    emacs_load = getenv("SINGULAR_EMACS_LOAD");
    if (access(emacs_load, R_OK))
    {
      // look in home-dir
      emacs_load = getenv("HOME");
      sprintf(cwd, "%s/.emacs-singular", emacs_load);
      if (! access(cwd, R_OK))
      {
        emacs_load = mstrdup(cwd);
      }
      else
      {
        // try witrh resources
        emacs_load = feResource("EmacsLoad", 0);
        if (emacs_load == NULL)
        {
          fprintf(stderr, "Error: Can't find emacs load file for Singular mode. \nExpected it at %s\nSpecify with --emacs_load option,\n or set SINGULAR_EMACS_LOAD environment variable,\n or put file '.emacs-singular' in your home directory.\n", 
                  feResourceDefault("EmacsLoad"));  
          mainUsage(argv[0]);
          exit(1);
        }
      }
    }
  }
  
  // construct options
  int i, length = 0;
  for (i=1; i<argc; i++)
  {
    if (argv[i] != NULL) length += strlen(argv[i]) + 3;
  }
  
  char* syscall = (char*) AllocL(strlen(emacs) + 
                                 strlen(singular) + 
                                 strlen(emacs_dir) + 
                                 strlen(emacs_load) +
                                 length + 300);
  char* prefix = "--";
  if (strstr(emacs, "xemacs") || strstr(emacs, "Xemacs") || strstr(emacs, "XEMACS"))
    prefix = "-";
  getcwd(cwd, MAXPATHLEN);

  // Note: option -no-init-file should be equivalent to -q. Anyhow, 
  // xemacs-20.4 sometimes crashed on startup when using -q. Don´t know why.
  sprintf(syscall, "%s %seval '(setq singular-emacs-home-directory \"%s\")' %sno-init-file %sl %s %seval '(singular-other \"%s\" \"%s\" (list ",
          emacs, prefix, emacs_dir, prefix, prefix, emacs_load, prefix, 
          singular, cwd);

  for (i=1; i<argc; i++)
  {
    if (argv[i] != NULL)
    {
      strcat(syscall, "\"");
      strcat(syscall, argv[i]);
      strcat(syscall, "\" ");
    }
  }
  strcat(syscall, ") \"singular\")'");
  
  if (no_emacs_call)
  {
    printf("%s\n", syscall);
  }
  else
  {
    if (system(syscall) != 0)
    {
      fprintf(stderr, "Error: Executation of\n%s\n");
      mainUsage(argv[0]);
      exit(1);
    }
  }
}


