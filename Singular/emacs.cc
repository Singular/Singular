/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: emacs.cc,v 1.1 1999-08-25 15:26:01 obachman Exp $ */
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
  WarnS("YOU HAVE FOUND A BUG IN SINGULAR.");
  WarnS("Please, email the following output to singular@mathematik.uni-kl.de");
  Warn("Bug occured at %s:%d", file, line);
  Warn("Message: %s", msg);
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
          break;
          
        default:
          feReportBug("Parsing of Cmd-line options");
    }
  }
  
  // make sure  emacs, singular, emacs_dir, emacs_load are set
  if (emacs == NULL) emacs = feResource("emacs", 0);
  if (emacs == NULL)
  {
    fprintf(stderr, "Error: Can't find emacs executable. \nExpected it at %s\n. Specify alternative with --emacs option, or set EMACS environment variable.\n", 
            feResourceDefault("emacs"));
    mainUsage(argv[0]);
    exit(1);
  }
            
  if (singular == NULL) singular = feResource("SingularEmacs", 0);
  if (singular == NULL)
  {
    fprintf(stderr, "Error: Can't find singular executable.\nExpected it at %s\nSpecify with --singular option, or set SINGULAR_EMACS environment variable.\n", 
            feResourceDefault("SingularEmacs"));
    mainUsage(argv[0]);
    exit(1);
  }
    
  if (emacs_dir == NULL) emacs_dir = feResource("EmacsDir", 0);
  if (emacs_dir == NULL)
  {
    fprintf(stderr, "Error: Can't find emacs directory for Singular lisp files. \nExpected it at %s\nSpecify with --emacs_dir option, or set SINGULAR_EMACS_DIR environment variable.\n", 
            feResourceDefault("EmacsDir"));
    mainUsage(argv[0]);
    exit(1);
  }

  if (emacs_load == NULL) emacs_load = feResource("EmacsLoad", 0);
  if (emacs_load == NULL)
  {
    fprintf(stderr, "Error: Can't find emacs load file for Singular mode. \nExpected it at %s\nSpecify with --emacs_load option, or set SINGULAR_EMACS_LOAD environment variable.\n", 
            feResourceDefault("EmacsLoad"));
    mainUsage(argv[0]);
    exit(1);
  }
  
  
  // construct options
  int i, length = 0;
  for (i=1; i<argc; i++)
  {
    if (strstr(argv[i], "--"LON_EMACS) != argv[i] &&
        strstr(argv[i], "--"LON_SINGULAR) != argv[i] &&
        strstr(argv[i], "--"LON_EMACS_DIR) != argv[i] &&
        strstr(argv[i], "--"LON_EMACS_LOAD) != argv[i] &&
        strstr(argv[i], "--"LON_NO_EMACS_CALL) != argv[i])
    {
      length += strlen(argv[i]) + 3;
    }
  }
  
  char* syscall = (char*) AllocL(strlen(emacs) + 
                                 strlen(singular) + 
                                 strlen(emacs_dir) + 
                                 strlen(emacs_load) +
                                 length + 300);
  char* prefix = "--";
  if (strstr(emacs, "xemacs") || strstr(emacs, "Xemacs") || strstr(emacs, "XEMACS"))
    prefix = "-";
  char cwd[MAXPATHLEN];
  getcwd(cwd, MAXPATHLEN);

  sprintf(syscall, "%s %seval '(setq singular-emacs-home-directory \"%s\")' %sq %sl %s %seval '(singular-other \"%s\" \"%s\" (quote ",
          emacs, prefix, emacs_dir, prefix, prefix, emacs_load, prefix, 
          singular, cwd);

  for (i=1; i<argc; i++)
  {
    if (strstr(argv[i], "--"LON_EMACS) != argv[i] &&
        strstr(argv[i], "--"LON_SINGULAR) != argv[i] &&
        strstr(argv[i], "--"LON_EMACS_DIR) != argv[i] &&
        strstr(argv[i], "--"LON_EMACS_LOAD) != argv[i] &&
        strstr(argv[i], "--"LON_NO_EMACS_CALL) != argv[i])
    {
      strcat(syscall, "\"");
      strcat(syscall, argv[i]);
      strcat(syscall, "\" ");
    }
  }
  strcat(syscall, "\"-t\" \"-emacs\") \"Singular\")'");
  
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


