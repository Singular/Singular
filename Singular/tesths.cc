/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tesths.cc,v 1.49 1998-06-16 11:03:59 obachman Exp $ */

/*
* ABSTRACT - initialize SINGULARs components, run Script and start SHELL
*/

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "version.h"
#include "mod2.h"
#include "tok.h"
#include "ipshell.h"
#include "sing_dld.h"
#include "febase.h"
#include "cntrlc.h"
#include "mmemory.h"
#include "silink.h"
#include "ipid.h"
#include "timer.h"
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory.h>
#endif
#include "getopt.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

// define the long option names here
#define LON_BATCH           "batch"
#define LON_EXECUTE         "execute"
#define LON_ECHO            "echo"
#define LON_HELP            "help"
#define LON_RANDOM          "random"
#define LON_QUIET           "quiet"
#define LON_NO_TTY          "no-tty"
#define LON_USER_OPTION     "user-option"
#define LON_VERSION         "verbose"
#define LON_MIN_TIME        "min-time"
#define LON_NO_STDLIB       "no-stdlib"
#define LON_NO_RC           "no-rc"
#define LON_TICKS_PER_SEC   "ticks-per-sec"
#define LON_TCLMODE         "tclmode"
#define LON_MP_PORT         "MPport"
#define LON_MP_HOST         "MPhost"
// undocumented options
#ifdef HAVE_MPSR
#define LON_MP_TRANSP       "MPtransp"
#define LON_MP_MODE         "MPmode"
#endif

#define LONG_OPTION_RETURN 13
static struct option longopts[] =
{
#ifdef HAVE_MPSR
  {LON_BATCH,             0,  0,  'b'},
#endif
  {LON_EXECUTE,           1,  0,  'c'},
  {LON_ECHO,              2,  0,  'e'},
  {LON_HELP,              0,  0,  'h'},
  {LON_QUIET,             0,  0,  'q'},
  {LON_RANDOM,            1,  0,  'r'},
  {LON_NO_TTY,            0,  0,  't'},
  {LON_USER_OPTION,       1,  0,  'u'},
  {LON_VERSION,           0,  0,  'v'},
#ifdef HAVE_TCL
  {LON_TCLMODE,           0,  0,  'x'},
#endif
  {LON_NO_STDLIB,         0,  0,  LONG_OPTION_RETURN},
  {LON_NO_RC,             0,  0,  LONG_OPTION_RETURN},
  {LON_MIN_TIME,          1,  0,  LONG_OPTION_RETURN},
#ifdef HAVE_MPSR
  {LON_MP_PORT,           1,  0,  LONG_OPTION_RETURN},
  {LON_MP_HOST,           1,  0,  LONG_OPTION_RETURN},
#endif
  {LON_TICKS_PER_SEC,     1,  0,  LONG_OPTION_RETURN},
// undocumented options
#ifdef HAVE_MPSR
  {LON_MP_TRANSP,         1,  0,  LONG_OPTION_RETURN},
  {LON_MP_MODE,           1,  0,  LONG_OPTION_RETURN},
#endif
// terminator
  { 0, 0, 0, 0 }
};
// #define SHORT_OPTS_STRING "bc:e::hqr:tu:v"
#define SHORT_OPTS_STRING "bhqtvxec:r:u:"

struct sing_option
{
  const char*   name;    // as in option
  const char*   arg_name;// name of argument, if has_arg > 0
  const char*   help;    // (short) help string
  char*         val;     // (default) value of option: 0: not set 1:set
                         // string:"" if has_arg > 0
};

// mention only documented options here
static struct sing_option sing_longopts[] =
{
#ifdef HAVE_MPSR
  {LON_BATCH,       0,          "Run in MP batch mode",                                 0},
#endif
  {LON_HELP,        0,          "Print help message and exit",                          0},
  {LON_QUIET,       0,          "Do not print start-up banner and warnings",            0},
  {LON_NO_TTY,      0,          "Do not redefine the terminal characteristics",         0},
  {LON_VERSION,     0,          "Print extended version and configuration info",        0},
#ifdef HAVE_TCL
  {LON_TCLMODE,     0,          "Run in TCL mode, i.e., with TCL user interface",       0},
#endif
  {LON_ECHO,        "VAL",      "Set value of variable `echo' to (integer) VAL",        ""},
  {LON_EXECUTE,     "STRING",   "Execute STRING on start-up",                           ""},
  {LON_RANDOM,      "SEED",     "Seed random generator with integer (integer) SEED",    ""},
  {LON_USER_OPTION, "STRING",   "Return STRING on `system(\"--user-option\")'",         ""},
  {LON_NO_STDLIB,   0,          "Do not load `standard.lib' on start-up",               0},
  {LON_NO_RC,       0,          "Do not execute `.singularrc' file on start-up",        0},
  {LON_MIN_TIME,    "SECS",     "Do not display times smaller than SECS (in seconds)",  "0.5"},
#ifdef HAVE_MPSR
  {LON_MP_PORT,     "PORT",     "Use PORT number for MP conections",                    ""},
  {LON_MP_HOST,     "HOST",     "Use HOST for MP connections",                          ""},
#endif
  {LON_TICKS_PER_SEC, "TICKS",    "Sets unit of timer to TICKS per second",             "1"},
  { 0, 0, 0, 0 }
};

static struct sing_option* mainGetSingOption(const char* name)
{
  int i = 0;
  while (sing_longopts[i].name != 0)
  {
    if (strcmp(name, sing_longopts[i].name) == 0)
      return &(sing_longopts[i]);
    i++;
  }
  return NULL;
}

static void mainSetSingOptionValue(const char* name, char* value)
{
  sing_option* sopt = mainGetSingOption(name);
  if (sopt != NULL)
  {
    sopt->val = value;
  }
}


static char* mainGetSingOptionValue(const char* name)
{
  sing_option* sopt = mainGetSingOption(name);
  if (sopt!=NULL)
    return sopt->val;
  return NULL;
}


// Prints help message
static void mainHelp(const char* name)
{
  int i = 0;
  struct sing_option *sopt;
  char tmp[20];

  printf("Singular version %s -- a CAS for polynomial computations. Usage:\n", S_VERSION1);
  printf("   %s [options] [file1 [file2 ...]]\n", name);
  printf("Options:\n");

  while (longopts[i].name != 0)
  {
    sopt = mainGetSingOption(longopts[i].name);
    if (sopt != NULL)
    {
      if (longopts[i].has_arg > 0)
      {
        if  (longopts[i].has_arg > 1)
          sprintf(tmp, "%s[=%s]", longopts[i].name, sopt->arg_name);
        else
          sprintf(tmp, "%s=%s", longopts[i].name, sopt->arg_name);

        printf(" %c%c --%-19s %s\n",
               (longopts[i].val != 0 ? '-' : ' '),
               (longopts[i].val != 0 ? longopts[i].val : ' '),
               tmp,
               sopt->help);
      }
      else
      {
        printf(" %c%c --%-19s %s\n",
               (longopts[i].val != 0 ? '-' : ' '),
               (longopts[i].val != 0 ? longopts[i].val : ' '),
               longopts[i].name,
               sopt->help);
      }
    }
#ifndef NDEBUG
    else
      printf("--%s Undocumented\n", longopts[i].name);
#endif
    i++;
  }

  printf("\nFor more information, type `help;' from within Singular or visit\n");
  printf("http://www.mathematik.uni-kl.de/~zca/Singular or consult the\n");
  printf("Singular manual (available as on-line info or printed manual).\n");
}

// Prints usage message
static void mainUsage(char* argv0)
{
  printf("Use `%s --help' for a complete list of options\n", argv0);
}

#ifndef NDEBUG
void mainOptionValues()
{
  int i = 0;
  struct sing_option *sopt;

  while (longopts[i].name != 0)
  {
    sopt = mainGetSingOption(longopts[i].name);
    if (sopt != NULL)
    {
      if (sopt->val == NULL || sopt->val == (char*) 1)
        Print("// --%-10s %d\n", sopt->name, sopt->val);
      else
        Print("// --%-10s %s\n", sopt->name, sopt->val);
    }
    else
      Print("// --%s Undocumented \n", longopts[i].name);
    i++;
  }
}
#endif // NDEBUG

BOOLEAN mainGetSingOptionValue(const char* name, char** val)
{
  sing_option* sopt = mainGetSingOption(name);
  if (sopt == NULL)
  {
    *val = NULL;
    return FALSE;
  }
  else
  {
    *val = sopt->val;
    return TRUE;
  }
}

/*0 implementation*/
int main(          /* main entry to Singular */
    int argc,      /* number of parameter */
    char** argv)   /* parameter array */
{
  char * thisfile;
  /* initialize components */
  siRandomStart=inits();
#ifdef INIT_BUG
  jjInitTab1();
#endif
#ifdef GENTABLE
  extern void ttGen1();
  extern void ttGen2();
  #ifndef __MWERKS__
    extern mpsr_ttGen(); // For initialization of (CMD, MP_COP) tables
    mpsr_ttGen();
  #endif
  ttGen1();
  ttGen2();
#else
  int i;
  thisfile = argv[0];
  BOOLEAN load_std_lib=TRUE;
  BOOLEAN load_rc = TRUE;
  char* execute_string = NULL;
  int optc, option_index;

  // do this first, because -v might print version path
  feInitPaths(thisfile);

  // parse command line options
  while((optc = getopt_long(argc, argv,
                            SHORT_OPTS_STRING, longopts, &option_index))
        != EOF)
  {
    switch(optc)
    {
   
        case 'b':
#ifdef HAVE_MPSR   
          feBatch=TRUE;
          mainSetSingOptionValue(LON_BATCH, (char*) 1);
          break;
#else
          printf("%s: Option `-b' not supported in this configuration\n", argv[0]);
          mainUsage(argv[0]);
          exit(1);
#endif

        case 'h':
          mainHelp(argv[0]);
          exit(0);

        case 'q':
          verbose &= ~(Sy_bit(0)|Sy_bit(V_LOAD_LIB));
          mainSetSingOptionValue(LON_QUIET, (char*) 1);
          break;

        case 't':
#if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
          fe_use_fgets=TRUE;
#endif
          mainSetSingOptionValue(LON_NO_TTY, (char*) 1);
          break;

        case 'v':
          printf("Singular for %s version %s  (%d)  %s %s\n",
                 S_UNAME, S_VERSION1,
                 SINGULAR_VERSION_ID,__DATE__,__TIME__);
          printf("with\n");
          printf(versionString());
          printf("\n\n");
          mainSetSingOptionValue(LON_VERSION, (char*) 1);
          break;

        case 'x':
#ifdef HAVE_TCL
          tclmode = TRUE;
          mainSetSingOptionValue(LON_TCLMODE, (char*) 1);
          break;
#else
          printf("%s: Option `-x' not supported in this configuration\n", argv[0]);
          mainUsage(argv[0]);
          exit(1);
#endif

        case 'e':
          if (optarg != NULL)
          {
            errno = 0;
            si_echo = strtol(optarg, NULL, 10);
            if (errno)
            {
              printf("%s: `%s' invalid integer argument for option `--%s'\n", argv[0], optarg, LON_ECHO);
              mainUsage(argv[0]);
              exit(1);
            }
            if (si_echo < 0 || si_echo > 9)
            {
              printf("%s: `%s' argument for option `--%s' is not in valid range 0..9\n",
                     argv[0], optarg, LON_ECHO);
              mainUsage(argv[0]);
              exit(1);
            }
            mainSetSingOptionValue(LON_ECHO, optarg);
          }
          else
          {
            si_echo = 1;
            mainSetSingOptionValue(LON_ECHO, "1");
          }
          break;

        case 'c':
          execute_string = optarg;
          mainSetSingOptionValue(LON_EXECUTE, optarg);
          break;
   
        case 'r':
          errno = 0;
          siRandomStart = strtol(optarg, NULL, 10);
          if (errno || siRandomStart == 0)
          {
            printf("%s: `%s' invalid integer argument for option `--%s'\n", argv[0], optarg, LON_RANDOM);
            mainUsage(argv[0]);
            exit(1);
          }
          #ifdef buildin_rand
          siSeed=siRandomStart;
          #else
          srand((unsigned int)siRandomStart);
          #endif
          #ifdef HAVE_FACTORY
          factoryseed(siRandomStart);
          #endif
          mainSetSingOptionValue(LON_RANDOM, optarg);
          break;
   
        case 'u':
          mainSetSingOptionValue(LON_USER_OPTION, optarg);
          break;

        case LONG_OPTION_RETURN:
          if (strcmp(longopts[option_index].name, LON_NO_STDLIB) == 0)
          {
            load_std_lib = FALSE;
            mainSetSingOptionValue(LON_NO_STDLIB, (char*) 1);
          }
          else if (strcmp(longopts[option_index].name, LON_NO_RC) == 0)
          {
            load_rc = FALSE;
            mainSetSingOptionValue(LON_NO_RC, (char*) 1);
          }
          else if (strcmp(longopts[option_index].name, LON_MIN_TIME) == 0)
          {
            double mintime = atof(optarg);
            if (mintime <= 0)
            {
              printf("%s: `%s' invalid float argument for option `--%s'\n",
                     argv[0], optarg, LON_MIN_TIME);
              mainUsage(argv[0]);
              exit(1);
            }
            SetMinDisplayTime(mintime);
            mainSetSingOptionValue(LON_MIN_TIME, optarg);
          }
#ifdef HAVE_MPSR
          else if (strcmp(longopts[option_index].name, LON_MP_PORT) == 0)
          {
            mainSetSingOptionValue(LON_MP_PORT, optarg);
          }
          else if (strcmp(longopts[option_index].name, LON_MP_HOST) == 0)
          {
            mainSetSingOptionValue(LON_MP_HOST, optarg);
          }
#endif   
          else if (strcmp(longopts[option_index].name, LON_TICKS_PER_SEC) == 0)
          {
            int ticks = atoi(optarg);
            if (ticks <= 0)
            {
              printf("%s: `%s' invalid integer argument for option `--%s'\n",
                     argv[0], optarg, LON_TICKS_PER_SEC);
              mainUsage(argv[0]);
              exit(1);
            }
            SetTimerResolution(ticks);
            mainSetSingOptionValue(LON_TICKS_PER_SEC, optarg);
          }
          else
            // undocumented options
#ifdef HAVE_MPSR
            if (strcmp(longopts[option_index].name, LON_MP_MODE) == 0 ||
                strcmp(longopts[option_index].name, LON_MP_TRANSP) == 0)
            {
              /* ignore */
            }
          else
#endif
            assume(0);
          break;

        default:
          // Error message already emmited by getopt_long
          mainUsage(argv[0]);
          exit(1);
    }
  }
  /*. say hello */
#ifdef HAVE_NAMESPACES
  namespaceroot->push(NULL, "(ROOT)", TRUE);
#endif /* HAVE_NAMESPACES */
  if (BVERBOSE(0) && !feBatch)
  {
    printf(
"                     SINGULAR                             /\n"
" a Computer Algebra System for Polynomial Computations   /   version %s\n"
"                                                       0<\n"
"     by: G.-M. Greuel, G. Pfister, H. Schoenemann        \\   %s\n"
#ifdef SDRING
"       preliminary experimental version\n"
#endif
"FB Mathematik der Universitaet, D-67653 Kaiserslautern    \\\n"
, S_VERSION1,S_VERSION2);
  }
  else
  if (!feBatch)
  {
#ifdef __MWERKS__
    memcpy(stderr,stdout,sizeof(FILE));
#else
    dup2(1,2);
#endif
  }
  slStandardInit();
  dlInit(thisfile);
  myynest=0;
  if (load_std_lib)
  {
    int vv=verbose;
    verbose &= ~Sy_bit(V_LOAD_LIB);
    iiLibCmd(mstrdup("standard.lib"),TRUE);
    verbose=vv;
  }
  errorreported = 0;
#ifndef macintosh
#if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
  fe_set_input_mode();
#endif
#endif
  setjmp(si_start_jmpbuf);

  // Now, put things on the stack of stuff to do

  // Last thing to do is to execute given scripts
  if (optind < argc)
  {
    int i = argc - 1;
    while (i >= optind)
    {
      newFile(argv[i]);
      i--;
    }
  }
  else
  {
    currentVoice=feInitStdin();
  }

  // before scripts, we execute -c, if it was given
  // now execute -c, if it was given
  if (execute_string != NULL)
    newBuffer(mstrdup(execute_string), BT_execute);

  // first thing, however, is to load .singularrc
  if (load_rc)
  {
    FILE * rc=myfopen(".singularrc","r");
    if (rc!=NULL)
    {
      fclose(rc);
      newFile(".singularrc");
    }
    else
    {
      char buf[MAXPATHLEN];
      char *home = getenv("HOME");
      if (home != NULL)
      {
        strcpy(buf, home);
        strcat(buf, "/.singularrc");
        rc = myfopen(buf, "r");
        if (rc != NULL)
        {
          fclose(rc);
          newFile(buf);
        }
      }
    }
  }

  /* start shell */
  if (feBatch)
  {
#ifdef HAVE_MPSR
    extern int Batch_do(const char* port, const char* host);
    return Batch_do(mainGetSingOptionValue(LON_MP_PORT),
                    mainGetSingOptionValue(LON_MP_HOST));
#else
    assume(0);
#endif
  }
  setjmp(si_start_jmpbuf);
  yyparse();
#endif
  //Print("at very end\n");
  m2_end(0);
  return 0;
}
