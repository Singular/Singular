/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tesths.cc,v 1.69 1999-08-25 15:26:02 obachman Exp $ */

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
#include "sdb.h"
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory.h>
#endif

#include "feCmdLineOptions.cc"
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
  thisfile = argv[0];
  BOOLEAN load_std_lib=TRUE;
  BOOLEAN load_rc = TRUE;
  char* execute_string = NULL;
  int optc, option_index;

  // do this first, because -v might print version path
  feInitResources(thisfile);

  // parse command line options
  while((optc = getopt_long(argc, argv,
                            SHORT_OPTS_STRING, longopts, &option_index))
        != EOF)
  {
    switch(optc)
    {

        case 'b':
#ifdef HAVE_MPSR
          fe_fgets_stdin=fe_fgets_dummy;
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
          fe_fgets_stdin=fe_fgets;
#endif
          mainSetSingOptionValue(LON_NO_TTY, (char*) 1);
          break;

        case 'd':
          sdb_flags = 1;
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
          fe_fgets_stdin=fe_fgets_tcl;
          mainSetSingOptionValue(LON_TCLMODE, (char*) 1);
          verbose|=Sy_bit(V_SHOW_MEM);
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
          else if (strcmp(longopts[option_index].name, LON_BROWSER) == 0)
          {
            mainSetSingOptionValue(LON_BROWSER, optarg);
          }
          else if (strcmp(longopts[option_index].name, LON_EMACS) == 0)
          {
            mainSetSingOptionValue(LON_EMACS, (char*) 1);
            // print EmacsDir and InfoFile so that Emacs
            // mode can pcik it up
            Warn("EmacsDir: %s", (feResource('e' /*"EmacsDir"*/) != NULL ?
                                  feResource('e' /*"EmacsDir"*/) : ""));
            Warn("InfoFile: %s", (feResource('i' /*"InfoFile"*/) != NULL ?
                                  feResource('i' /*"InfoFile"*/) : ""));
          }
          else if (strcmp(longopts[option_index].name, LON_NO_RC) == 0)
          {
            load_rc = FALSE;
            mainSetSingOptionValue(LON_NO_RC, (char*) 1);
          }
          else if (strcmp(longopts[option_index].name, LON_NO_WARN) == 0)
          {
            feWarn = FALSE;
            mainSetSingOptionValue(LON_NO_WARN, (char*) 1);
          }
          else if (strcmp(longopts[option_index].name, LON_NO_OUT) == 0)
          {
            feOut = FALSE;
            mainSetSingOptionValue(LON_NO_OUT, (char*) 1);
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
  /* say hello */
  {
    idhdl h;
    namespaceroot->push(NULL, "Top", 0, TRUE);
#ifdef HAVE_NAMESPACES
    h=enterid(mstrdup("Top"), 0, PACKAGE_CMD, &IDROOT, TRUE);
    IDPACKAGE(h) = NSPACK(namespaceroot);
    IDPACKAGE(h)->language = LANG_TOP;
#endif /* HAVE_NAMESPACES */
  }
  if (BVERBOSE(0))
  {
    printf(
"                     SINGULAR                             /"
#ifndef MAKE_DISTRIBUTION
"  Development"
#endif
"\n"
" A Computer Algebra System for Polynomial Computations   /   version %s\n"
"                                                       0<\n"
"     by: G.-M. Greuel, G. Pfister, H. Schoenemann        \\   %s\n"
#ifdef SDRING
"       preliminary experimental version\n"
#endif
"FB Mathematik der Universitaet, D-67653 Kaiserslautern    \\\n"
, S_VERSION1,S_VERSION2);
  }
  else
  {
    sdb_flags = 0;
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
#ifdef HAVE_NAMESPACES
    iiLibCmd(mstrdup("standard.lib"),TRUE, TRUE);
#else /* HAVE_NAMESPACES */
    iiLibCmd(mstrdup("standard.lib"), TRUE);
#endif /* HAVE_NAMESPACES */
    verbose=vv;
  }
  errorreported = 0;
  mmMarkInitDBMCB();
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
  if (fe_fgets_stdin==fe_fgets_dummy)
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
