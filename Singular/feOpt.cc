/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: feOpt.cc,v 1.1 1999-09-20 18:03:45 obachman Exp $ */
/*
* ABSTRACT: Implementation of option buisness
*/

#include "mod2.h"
#include "feOpt.h"

// Define here which cmd-line options are recognized
struct fe_option feOptSpec[] =
{
//
//  Has to be of the form 
//    {name, has_arg, flag, val,
//     arg_name, help, type, value, set} 
//  where:
//
//        name   is the name of the long option.
// 
//        has_arg
//               is:  no_argument (or 0) if the option does not take
//               an argument, required_argument (or 1) if the option
//               requires  an  argument, or optional_argument (or 2)
//               if the option takes an optional argument.
// 
//        flag   specifies how  results  are  returned  for  a  long
//               option.    If  flag  is  NULL,  then  getopt_long()
//               returns val.  (For example, the calling program may
//               set  val to the equivalent short option character.)
//               Otherwise, getopt_long() returns 0, and flag points
//               to  a variable which is set to val if the option is
//               found, but left unchanged  if  the  option  is  not
//               found.
//               SHOULD ALWAYS BE ZERO FOR SINGULAR
// 
//        val    is  the  value  to  return,  or  to  load  into the
//               variable pointed to by flag.
//               NEEDS TO BE LONG_OPTION_RETURN, for long option
//                           short option char,  for short option
//
//        arg_name if set, uses this value as name for argument in 
//                display of help
//      
//        help  one-line description of option
//
//        type  one of feOptBool, feOptInt, feOptString
//
//        value (default) value of option
//         
//       set   only relevant for feOptString: 
//             1: if value different from default value
//             0: otherwise      
//
// The order in which options are specified is the order in which 
// their help is printed on -h
//
// Options whose hel starts with an "//" are considered undocumented,
// i.e., their help is not printed on -h
// 
#ifdef ESINGULAR
// options only relevant for ESINGULAR
  {"emacs",         required_argument,  0,  LONG_OPTION_RETURN,
   "EMACS",     "Use EMACS as emacs program to run Singular",          feOptString, 0,   0},

  {"emacs-dir",         required_argument,  0,  LONG_OPTION_RETURN,
   "DIR",       "Use DIR as directory to look for emacs lisp files",   feOptString, 0,   0},

  {"emacs-load",        required_argument,  0,  LONG_OPTION_RETURN,
   "FILE",      "Load FILE on emacs start-up, instead of default",     feOptString, 0,   0},
  
  {"singular",          required_argument,  0,  LONG_OPTION_RETURN,
   "PROG",      "Start PROG as Singular program within emacs",         feOptString, 0,   0},

  {"no-emacs-call",     no_argument,  0,  LONG_OPTION_RETURN,
   0,          "Do not start emacs. Print emacs-call to stdout",       feOptBool,   0,   0},
#endif

#ifdef HAVE_MPSR
  {"batch",             no_argument,  0,  'b',
   0,          "Run in MP batch mode",                                 feOptBool,    0,     0},
#endif

  {"execute",           required_argument,  0,  'c',
   "STRING",   "Execute STRING on start-up",                           feOptString, 0,   0},
   
  {"sdb",               no_argument,  0,  'd',
   0,          "Enable source code debugger (experimental)",           feOptBool,    0,      0},

  {"echo",              optional_argument, 0,  'e',
   "VAL",       "Set value of variable `echo' to (integer) VAL",        feOptInt,    0,      0},
   
  {"help",              no_argument,  0,  'h',
   0,          "Print help message and exit",                          feOptBool,    0,      0},

  {"quiet",             no_argument,  0,  'q',
   0,          "Do not print start-up banner and lib load messages",   feOptBool,    0,      0},

  {"random",            required_argument,  0,  'r',
   "SEED",     "Seed random generator with integer (integer) SEED",    feOptInt,    0,      0},

  {"no-tty",            no_argument,  0,  't',
   0,          "Do not redefine the terminal characteristics",         feOptBool,    0,      0},

  {"user-option",       required_argument,  0,  'u',
   "STRING",   "Return STRING on `system(\"--user-option\")'",         feOptString, 0,   0},

  {"version",           no_argument,  0,  'v',
   0,          "Print extended version and configuration info",        feOptBool,    0,      0},

#ifdef HAVE_TCL
  {"tclmode",           no_argument,  0,  'x',
   0,          "Run in TCL mode, i.e., with TCL user interface",       feOptBool,    0,      0},
#endif

  {"browser",           required_argument,  0,  LONG_OPTION_RETURN,
   "BROWSER",  "Display help in BROWSER ([x,tk]info, netscape)",       feOptString, 0,   0},

#ifndef ESINGULAR
  {"emacs",             no_argument,  0,  LONG_OPTION_RETURN,
   0,          "Set defaults for running within emacs",                feOptBool,    0,      0},
#endif

  {"no-stdlib",         no_argument,  0,  LONG_OPTION_RETURN,
   0,          "Do not load `standard.lib' on start-up",               feOptBool,    0,      0},
  
  {"no-rc",             no_argument,  0,  LONG_OPTION_RETURN,
   0,          "Do not execute `.singularrc' file on start-up",        feOptBool,    0,      0},

  {"no-warn",           no_argument,  0,  LONG_OPTION_RETURN,
   0,          "Do not display warning messages",                      feOptBool,    0,      0},

  {"no-out",            no_argument,  0,  LONG_OPTION_RETURN,
   0,          "Suppress all output",                                  feOptBool,    0,      0},

  {"min-time",          required_argument,  0,  LONG_OPTION_RETURN,
  "SECS",     "Do not display times smaller than SECS (in seconds)",   feOptString, "0.5",  0}, 

#ifdef HAVE_MPSR
  {"mp-port",           required_argument,  0,  LONG_OPTION_RETURN,
   "PORT",     "Use PORT number for MP conections",                    feOptString,    0,      0},

  {"mp-host",           required_argument,  0,  LONG_OPTION_RETURN,
   "HOST",     "Use HOST for MP connections",                          feOptString,    0,   0},
#endif

  {"ticks-per-sec",     required_argument,  0,  LONG_OPTION_RETURN,
   "TICKS",     "Sets unit of timer to TICKS per second",               feOptInt,    (void*)1,      0},

// undocumented options
#ifdef HAVE_MPSR
  {"mp-transp",         required_argument,  0,  LONG_OPTION_RETURN,
   "TRANSP",    "// Use TRANSP for MP connections",                     feOptString,    0,   0},
   
  {"mp-mode",           required_argument,  0,  LONG_OPTION_RETURN,
   "MODE",      "// Use MODE for MP connections",                       feOptString,    0,   0},
#endif
    
// terminator -- do NOT remove
  { 0, 0, 0, 0 , 0, 0, feOptInt, 0, 0}
};

const char SHORT_OPTS_STRING[] = "bdhqtvxec:r:u:";

//////////////////////////////////////////////////////////////
//
// Generation of feOptIndex
//
#ifdef GENERATE_OPTION_INDEX

#include <stdio.h>
main()
{
  FILE* fd;
#ifdef ESINGULAR
  fd = fopen("feOptES.inc", "w");
#else  
  fd = fopen("feOpt.inc", "w");
#endif

  if (fd == NULL) exit(1);

  int i = 0;
  
  fputs("typedef enum\n{\n  ", fd);
  
  while (feOptSpec[i].name != NULL)
  {
    const char* name = feOptSpec[i].name;
    fputs("FE_OPT_", fd);
    while (*name != 0)
    {
      if (*name == '-') 
      {
        putc('_', fd);
      }
      else if (*name >= 97 && *name <= 122)
      {
        putc(*name - 32, fd);
      }
      else
      {
        putc(*name, fd);
      }
      name++;
    }
    if (i == 0)
    {
      fputs("=0", fd);
    }
    i++;
    fputs(",\n  ", fd);
  }
  
  fprintf(fd, "FE_OPT_UNDEF\n} feOptIndex;\n");
  fclose(fd);
  exit(0);
}

#else // ! GENERATE_OPTION_INDEX

///////////////////////////////////////////////////////////////
//
// Getting Values
//

feOptIndex feGetOptIndex(const char* name)
{
  int opt = 0;
  
  while (opt != (int) FE_OPT_UNDEF)
  {
    if (strcmp(feOptSpec[opt].name, name) == 0) 
      return (feOptIndex) opt;
    opt = opt + 1;
  }
  return FE_OPT_UNDEF;
}

feOptIndex feGetOptIndex(int optc)
{
  int opt = 0;
  
  if (optc == LONG_OPTION_RETURN) return FE_OPT_UNDEF;
  
  while (opt != (int) FE_OPT_UNDEF)
  {
    if (feOptSpec[opt].val == optc)
      return (feOptIndex) opt;
    opt = opt + 1;
  }
  return FE_OPT_UNDEF;
}

///////////////////////////////////////////////////////////////
//
// Setting Values
//
static void feOptHelp(const char* name);
//
// Return: NULL -- everything ok
//         "error-string" on error
#ifndef ESINGULAR
#include "mmemory.h"
#include "febase.h"
#include "ipshell.h"
#include "tok.h"
#include "sdb.h"
#include "cntrlc.h"
#include "timer.h"

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory.h>
#endif
#include <errno.h>

static char* feOptAction(feOptIndex opt);
char* feSetOptValue(feOptIndex opt, char* optarg)
{
  if (opt == FE_OPT_UNDEF) return "option undefined";
  
  if (feOptSpec[opt].type != feOptString)
  {
    if (optarg != NULL)
    {
      errno = 0;
      feOptSpec[opt].value = (void*) strtol(optarg, NULL, 10);
      if (errno) return "invalid integer argument";
    }
    else
    {
      feOptSpec[opt].value = (void*) 0;
    }
  }
  else
  {
    assume(feOptSpec[opt].type == feOptString);
    if (feOptSpec[opt].set && feOptSpec[opt].value != NULL)
      FreeL(feOptSpec[opt].value);
    if (optarg != NULL)
      feOptSpec[opt].value = mstrdup(optarg);
    else
      feOptSpec[opt].value = NULL;
    feOptSpec[opt].set = 1;
  }
  
  return feOptAction(opt);
}

char* feSetOptValue(feOptIndex opt, int optarg)
{
  if (opt == FE_OPT_UNDEF) return "option undefined";
  
  if (feOptSpec[opt].type == feOptString)
    return "option value needs to be a string";
  
  feOptSpec[opt].value = (void*) optarg;
  return feOptAction(opt);
}

static char* feOptAction(feOptIndex opt)
{
  // do some special actions
  switch(opt)
  {
#ifdef HAVE_MPSR
      case FE_OPT_BATCH:
        fe_fgets_stdin=fe_fgets_dummy;
        return NULL;
#endif

      case FE_OPT_HELP:
        feOptHelp(feArgv0);
        return NULL;

      case FE_OPT_QUIET:
        verbose &= ~(Sy_bit(0)|Sy_bit(V_LOAD_LIB));
        return NULL;

      case FE_OPT_NO_TTY:
#if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
        fe_fgets_stdin=fe_fgets;
#endif
        return NULL;

      case FE_OPT_SDB:
        sdb_flags = 1;
        return NULL;

      case FE_OPT_VERSION:
        printf("Singular for %s version %s  (%lu)  %s %s\n",
               S_UNAME, S_VERSION1,
               feVersionId,__DATE__,__TIME__);
        printf("with\n");
        printf(versionString());
        printf("\n\n");
        return NULL;

#ifdef HAVE_TCL
      case FE_OPT_TCLMODE:
        tclmode = TRUE;
        fe_fgets_stdin=fe_fgets_tcl;
        verbose|=Sy_bit(V_SHOW_MEM);
        return NULL;
#endif

      case FE_OPT_ECHO:
        si_echo = (int) feOptSpec[opt].value;
        if (si_echo < 0 || si_echo > 9)
          return "argument of option is not in valid range 0..9";
        return NULL;
        
      case 'r':
        siRandomStart = (unsigned int) feOptSpec[opt].value;
#ifdef buildin_rand
        siSeed=siRandomStart;
#else
        srand((unsigned int)siRandomStart);
#endif
#ifdef HAVE_FACTORY
        factoryseed(siRandomStart);
#endif
        return NULL;

      case FE_OPT_EMACS:
        // print EmacsDir and InfoFile so that Emacs
        // mode can pcik it up
        Warn("EmacsDir: %s", (feResource('e' /*"EmacsDir"*/) != NULL ?
                              feResource('e' /*"EmacsDir"*/) : ""));
        Warn("InfoFile: %s", (feResource('i' /*"InfoFile"*/) != NULL ?
                              feResource('i' /*"InfoFile"*/) : ""));
        return NULL;

      case FE_OPT_NO_WARN:
        feWarn = FALSE;
        return NULL;
        
      case FE_OPT_NO_OUT:
        feOut = FALSE;
        return NULL;

      case FE_OPT_MIN_TIME:
      {
        double mintime = atof((char*) feOptSpec[FE_OPT_MIN_TIME].value);
        if (mintime <= 0) return "invalid float argument";
        SetMinDisplayTime(mintime);
        return NULL;
      }

      case FE_OPT_BROWSER:
        feHelpBrowser(feOptSpec[FE_OPT_BROWSER].value, 1);

      case FE_OPT_TICKS_PER_SEC:
      {
        int ticks = (int) feOptSpec[opt].value;
        if (ticks <= 0)
          return "integer argument must be larger than 0";
        SetTimerResolution(ticks);
        return NULL;
      }
      default:
        return NULL;
  }
}

// Prints usage message
void fePrintOptValues()
{
  int i = 0;

  while (feOptSpec[i].name != 0)
  {
    if (feOptSpec[i].help != NULL
#ifndef NDEBUG
        && *(feOptSpec[i].help) != '/'
#endif
        )
    {
      if (feOptSpec[i].type == feOptString)
      {
        if (feOptSpec[i].value == NULL)
        {
          Print("// --%-15s\n", feOptSpec[i].name);
        }
        else
        {
          Print("// --%-15s \"%s\"\n", feOptSpec[i].name, (char*) feOptSpec[i].value);
        }
      }
      else
      {
        Print("// --%-15s %d\n", feOptSpec[i].name, feOptSpec[i].value);
      }
    }
    i++;
  }
}

#endif // ! ESingular

// Prints help message
static void feOptHelp(const char* name)
{
  int i = 0;
  char tmp[20];
#ifdef ESINGULAR
  printf("ESingular: A Program that starts-up Singular within emacs, for\n");
#endif  
  printf("Singular version %s -- a CAS for polynomial computations. Usage:\n", S_VERSION1);
  printf("   %s [options] [file1 [file2 ...]]\n", name);
  printf("Options:\n");

  while (feOptSpec[i].name != 0)
  {
    if (feOptSpec[i].help != NULL 
#ifdef NDEBUG
        && *(feOptSpec[i].help) != '/'
#endif
        )
    {
      if (feOptSpec[i].has_arg > 0)
      {
        if  (feOptSpec[i].has_arg > 1)
          sprintf(tmp, "%s[=%s]", feOptSpec[i].name, feOptSpec[i].arg_name);
        else
          sprintf(tmp, "%s=%s", feOptSpec[i].name, feOptSpec[i].arg_name);

        printf(" %c%c --%-19s %s\n",
               (feOptSpec[i].val != 0 ? '-' : ' '),
               (feOptSpec[i].val != 0 ? feOptSpec[i].val : ' '),
               tmp,
               feOptSpec[i].help);
      }
      else
      {
        printf(" %c%c --%-19s %s\n",
               (feOptSpec[i].val != 0 ? '-' : ' '),
               (feOptSpec[i].val != 0 ? feOptSpec[i].val : ' '),
               feOptSpec[i].name,
               feOptSpec[i].help);
      }
    }
    i++;
  }

  printf("\nFor more information, type `help;' from within Singular or visit\n");
  printf("http://www.mathematik.uni-kl.de/~zca/Singular or consult the\n");
  printf("Singular manual (available as on-line info or printed manual).\n");
}



#endif // GENERATE_OPTION_INDEX
