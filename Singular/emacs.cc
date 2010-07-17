/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: Esingular main file
*/

#include <stdio.h>
#include <unistd.h>
#ifdef DecAlpha_OSF1
#define _BSD
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef ix86_Win
#include <windows.h>
#endif
#include <Singular/mod2.h>
#include <Singular/version.h>
#include <omalloc.h>

#if !defined(TSINGULAR) && !defined(ESINGULAR)
#define ESINGULAR
#endif

#ifdef system
#undef system
#endif

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

#ifndef ix86_Win
void error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
}
#else
void error(char* fmt, ...)
{
   char buf[4096];
   int j =0;
   va_list args;
   va_start(args, fmt);
   j =   sprintf(buf,    "");
   j += vsprintf(buf + j,fmt,args);
   j +=  sprintf(buf + j,"\n");
   va_end(args);
   MessageBox(NULL, buf, "ESingular.exe", MB_ICONSTOP);
   exit(1);
}
#endif

#define Warn  error
#define WarnS error
#define StringAppend printf
#define Print error

#define feReportBug(s) fePrintReportBug(s, __FILE__, __LINE__)
void fePrintReportBug(char* msg, char* file, int line)
{
  error("YOU HAVE FOUND A BUG IN SINGULAR.\n"
"Please, email the following output to singular@mathematik.uni-kl.de\n"
"Bug occured at %s:%d\n"
"Message: %s\n"
"Version: " S_UNAME S_VERSION1 " (%lu) " __DATE__ __TIME__,
        file, line, msg, feVersionId);

}

#include <Singular/../kernel/feResource.cc>
#include <Singular/feOpt.cc>

void mainUsage()
{
  error( "Use `%s --help' for a complete list of options\n", feArgv0);
}

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

  while ((optc = fe_getopt_long(argc, argv, SHORT_OPTS_STRING,
                                feOptSpec, &option_index))
        != EOF)
  {
    switch(optc)
    {
        case 'h':
          feOptHelp(feArgv0);
          exit(0);

        case '?':
        case ':':
        case '\0':
          mainUsage();
          exit(1);

        case  LONG_OPTION_RETURN:
        {
          switch(option_index)
          {
#ifdef TSINGULAR
              case FE_OPT_XTERM:
                emacs = fe_optarg;
              break;
#else
              case FE_OPT_EMACS:
                emacs = fe_optarg;
                break;

              case FE_OPT_EMACS_DIR:
                emacs_dir = fe_optarg;
                break;

              case FE_OPT_EMACS_LOAD:
                emacs_load = fe_optarg;
                break;
#endif
              case FE_OPT_SINGULAR:
                singular = fe_optarg;
                break;

              case FE_OPT_NO_CALL:
                no_emacs_call = 1;
                break;

              default:
                goto NEXT;
          }
          // delete options from option-list
          if (fe_optind > 2 && *argv[fe_optind-1] != '-' &&
              fe_optarg != NULL && feOptSpec[option_index].has_arg)
          {
            argv[fe_optind-2] = NULL;
          }
          argv[fe_optind-1] = NULL;
        }
    }
    NEXT:{}
  }

  int i, length = 0;
  char* syscall;
  for (i=1; i<argc; i++)
  {
    if (argv[i] != NULL) length += strlen(argv[i]) + 3;
  }

#ifdef TSINGULAR
  if (emacs == NULL) emacs = feResource('X', 0);
  if (emacs == NULL)
  {
    error( "Error: Can't find emacs xterm program. \n Expected it at %s or %s\n Specify alternative with --xterm=PROGRAM option,\n or set ESINGULAR_EMACS environment variable to the name of the program to use as xterm.\n",
           feResourceDefault('X'));
    mainUsage();
    exit(1);
  }

  if (singular == NULL) singular = feResource("SingularXterm", 0);
  if (singular == NULL)
  {
    error( "Error: Can't find singular executable.\n Expected it at %s\n Specify with --singular option,\n or set TSINGULAR_SINGULAR environment variable.\n",
            feResourceDefault("SingularXterm"));
    mainUsage();
    exit(1);
  }

#ifdef ix86_Win
#define EXTRA_XTERM_ARGS "+vb -sl 2000 -fb Courier-bold-12 -tn xterm -cr Red3"
#else
#define EXTRA_XTERM_ARGS ""
#endif

  syscall = (char*) omAlloc(strlen(emacs) +
                                 strlen(singular) +
                                 length + 300);
  sprintf(syscall, "%s %s -e %s ", emacs, EXTRA_XTERM_ARGS, singular);

  for (i=1; i<argc; i++)
  {
    if (argv[i] != NULL)
    {
      strcat(syscall, " ");
      strcat(syscall, argv[i]);
    }
  }
#else
  // make sure  emacs, singular, emacs_dir, emacs_load are set
  if (emacs == NULL) emacs = feResource("xemacs", 0);
  if (emacs == NULL) emacs = feResource("emacs", 0);
  if (emacs == NULL)
  {
    error( "Error: Can't find emacs or xemacs executable. \n Expected it at %s or %s\n Specify alternative with --emacs option,\n or set ESINGULAR_EMACS environment variable.\n",
            feResourceDefault("emacs"), feResourceDefault("xemacs"));
    mainUsage();
    exit(1);
  }

  if (singular == NULL) singular = feResource("SingularEmacs", 0);
  if (singular == NULL)
  {
    error( "Error: Can't find singular executable.\n Expected it at %s\n Specify with --singular option,\n or set ESINGULAR_SINGULAR environment variable.\n",
            feResourceDefault("SingularEmacs"));
    mainUsage();
    exit(1);
  }

  if (emacs_dir == NULL) emacs_dir = feResource("EmacsDir", 0);
  if (emacs_dir == NULL)
  {
    error( "Error: Can't find emacs directory for Singular lisp files. \n Expected it at %s\n Specify with --emacs_dir option,\n or set ESINGULAR_EMACS_DIR environment variable.\n",
            feResourceDefault("EmacsDir"));
    mainUsage();
    exit(1);
  }

  if (emacs_load == NULL)
  {
    // look into env variable
    emacs_load = getenv("ESINGULAR_EMACS_LOAD");
    if (access(emacs_load, R_OK))
    {
      // look in home-dir
      emacs_load = getenv("HOME");
#ifdef ix86_Win
      if ((emacs_load==NULL)||(!access(emacs_load,X_OK)))
        emacs_load = getenv("SINGHOME");
#endif
      sprintf(cwd, "%s/.emacs-singular", emacs_load);
      if (! access(cwd, R_OK))
      {
        emacs_load = omStrDup(cwd);
      }
      else
      {
        // try with resources
        emacs_load = feResource("EmacsLoad", 0);
        if (emacs_load == NULL)
        {
          error( "Error: Can't find emacs load file for Singular mode. \n Expected it at %s\n Specify with --emacs_load option,\n or set ESINGULAR_EMACS_LOAD environment variable,\n or put file '.emacs-singular' in your home directory.\n",
                  feResourceDefault("EmacsLoad"));
          mainUsage();
          exit(1);
        }
      }
    }
  }

  syscall = (char*) omAlloc(strlen(emacs) +
                           strlen(singular) +
                           strlen(emacs_dir) +
                           strlen(emacs_load) +
                           length + 300);
  const char* prefix = "--";
  if (strstr(emacs, "xemacs") || strstr(emacs, "Xemacs") || strstr(emacs, "XEMACS"))
    prefix = "-";
  getcwd(cwd, MAXPATHLEN);
  // append / at the end of cwd
  if (cwd[strlen(cwd)-1] != '/') strcat(cwd, "/");

  // Note: option -no-init-file should be equivalent to -q. Anyhow,
  // xemacs-20.4 sometimes crashed on startup when using -q. Don´t know why.
  sprintf(syscall, "%s %sno-init-file %seval '(progn (setq singular-emacs-home-directory \"%s\") (load-file \"%s\") (singular-other \"%s\" \"%s\" (list ",
          emacs, prefix, prefix, emacs_dir, emacs_load, 
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
  strcat(syscall, ") \"singular\"))'");
#endif

  if (no_emacs_call)
  {
    printf("%s\n", syscall);
  }
  else
  {
    if (system(syscall) != 0)
    {
      error( "Error: Execution of\n%s\n", syscall);
      mainUsage();
      exit(1);
    }
  }
}


