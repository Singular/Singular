/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT - initialize SINGULARs components, run Script and start SHELL
*/


#include "kernel/mod2.h"
#include "omalloc/omalloc.h"
#include "misc/options.h"
#include "factory/factory.h"
#include "kernel/oswrapper/feread.h"
#include "Singular/fevoices.h"
#include "kernel/oswrapper/timer.h"

#include "ipshell.h"
#include "cntrlc.h"
#include "links/silink.h"
#include "ipid.h"
#include "sdb.h"
#include "feOpt.h"
#include "distrib.h"
#include "mmalloc.h"
#include "tok.h"
#include "resources/fegetopt.h"

#include "Singular/countedref.h"
#include "Singular/pyobject_setup.h"

#include <unistd.h>
#ifdef HAVE_NTL
#include <NTL/config.h>
#endif


extern void siInit(char *);
#ifdef PSINGULAR
GLOBAL_VAR char *global_argv0;
#endif
EXTERN_VAR FILE* File_Log;

int mmInit( void )
{
#ifndef X_OMALLOC
#if defined(OMALLOC_USES_MALLOC)
    /* in mmstd.c, for some architectures freeSize() unconditionally uses the *system* free() */
    /* sage ticket 5344: http://trac.sagemath.org/sage_trac/ticket/5344 */
    /* do not rely on the default in Singular as libsingular may be different */
    mp_set_memory_functions(omMallocFunc,omReallocSizeFunc,omFreeSizeFunc);
#else
    mp_set_memory_functions(malloc,reallocSize,freeSize);
#endif
#endif
  return 1;
}

/*0 implementation*/
int main(          /* main entry to Singular */
    int argc,      /* number of parameter */
    char** argv)   /* parameter array */
{
  mmInit();
  // Don't worry: ifdef OM_NDEBUG, then all these calls are undef'ed
  omInitRet_2_Info(argv[0]);
  omInitGetBackTrace();

#ifdef PSINGULAR
  global_argv0 = argv[0];
#endif
  siInit(argv[0]);
  init_signals();
  #ifdef HAVE_NTL
  #if NTL_MAJOR_VERSION>=10
  #ifdef NTL_THREAD_BOOST
  SetNumThreads(feOptValue(FE_OPT_CPUS));
  #endif
  #endif
  #endif

  // parse command line options
  int optc, option_index;
  const char* errormsg;
  while((optc = fe_getopt_long(argc, argv,
                               SHORT_OPTS_STRING, feOptSpec, &option_index))
        != EOF)
  {
    if (optc == '?' || optc == 0)
    {
      fprintf(stderr, "Use '%s --help' for a complete list of options\n", feArgv0);
      exit(1);
    }

    if (optc != LONG_OPTION_RETURN)
      option_index = feGetOptIndex(optc);

    assume(option_index >= 0 && option_index < (int) FE_OPT_UNDEF);

    if (fe_optarg == NULL &&
        (feOptSpec[option_index].type == feOptBool ||
         feOptSpec[option_index].has_arg == optional_argument))
      errormsg = feSetOptValue((feOptIndex) option_index, (int) 1);
    else
      errormsg = feSetOptValue((feOptIndex) option_index, fe_optarg);

    if (errormsg)
    {
      if (fe_optarg == NULL)
        fprintf(stderr, "Error: Option '--%s' %s\n",
               feOptSpec[option_index].name, errormsg);
      else
        fprintf(stderr, "Error: Option '--%s=%s' %s\n",
               feOptSpec[option_index].name, fe_optarg, errormsg);
      fprintf(stderr, "Use '%s --help' for a complete list of options\n", feArgv0);
      exit(1);
    }
    if (optc == 'h') exit(0);
    switch(option_index)
    {
      case FE_OPT_DUMP_VERSIONTUPLE:
        exit(0);
        break;
      default: ;
    }
  }

  /* say hello */

  if (TEST_V_QUIET)
  {
    (printf)(
"                     SINGULAR                                 /"
#ifndef MAKE_DISTRIBUTION
"  Development"
#endif
"\n"
" A Computer Algebra System for Polynomial Computations       /   version %s\n"
"                                                           0<\n"
" by: W. Decker, G.-M. Greuel, G. Pfister, H. Schoenemann     \\   %s\n"
"FB Mathematik der Universitaet, D-67653 Kaiserslautern        \\\n"
, VERSION, VERSION_DATE);
  if (feOptValue(FE_OPT_NO_SHELL)) WarnS("running in restricted mode:"
    " shell invocation and links are disallowed");
  }
  else
  {
    if (feOptValue(FE_OPT_SORT)) On(SW_USE_NTL_SORT);
    dup2(1,2);
    /* alternative:
    *    memcpy(stderr,stdout,sizeof(FILE));
    */
  }
  if (feOptValue(FE_OPT_LOG))
  {
    int pid=getpid();
    char buf[20];
    sprintf(buf,"sing_log.%d",pid);
    File_Log=fopen(buf,"w");
  }

#ifdef SINGULAR_PYOBJECT_SETUP_H
   pyobject_setup();
#endif
#ifdef SI_COUNTEDREF_AUTOLOAD
  countedref_init();
#endif
  errorreported = 0;

  // -- example for "static" modules ------
  //load_builtin("huhu.so",FALSE,(SModulFunc_t)huhu_mod_init);
  //module_help_main("huhu.so","Help for huhu\nhaha\n");
  //module_help_proc("huhu.so","p","Help for huhu::p\nhaha\n");
  setjmp(si_start_jmpbuf);

  // Now, put things on the stack of stuff to do
  // Last thing to do is to execute given scripts
  if (fe_optind < argc)
  {
    int i = argc - 1;
    FILE *fd;
    while (i >= fe_optind)
    {
      if ((fd = feFopen(argv[i], "r")) == NULL)
      {
        Warn("Can not open %s", argv[i]);
      }
      else
      {
        fclose(fd);
        newFile(argv[i]);
      }
      i--;
    }
  }
  else
  {
    currentVoice=feInitStdin(NULL);
  }

  // before scripts, we execute -c, if it was given
  if (feOptValue(FE_OPT_EXECUTE) != NULL)
    newBuffer(omStrDup((char*) feOptValue(FE_OPT_EXECUTE)), BT_execute);

  // first thing, however, is to load .singularrc from Singularpath
  // and cwd/$HOME (in that order).
  if (! feOptValue(FE_OPT_NO_RC))
  {
    char buf[MAXPATHLEN];
    FILE * rc = feFopen("." DIR_SEPP ".singularrc", "r", buf);
    if (rc == NULL) rc = feFopen("~" DIR_SEPP ".singularrc", "r", buf);
    if (rc == NULL) rc = feFopen(".singularrc", "r", buf);

    if (rc != NULL)
    {
      if (BVERBOSE(V_LOAD_LIB))
        Print("// ** executing %s\n", buf);
      fclose(rc);
      newFile(buf);
    }
  }

  /* start shell */
  if (fe_fgets_stdin==fe_fgets_dummy)
  {
    singular_in_batchmode=TRUE;
    char *linkname=(char*) feOptValue(FE_OPT_LINK);
    if((linkname!=NULL)&&(strcmp(linkname,"ssi")==0))
    {
      return ssiBatch((char*) feOptValue(FE_OPT_MPHOST),(char*) feOptValue(FE_OPT_MPPORT));
      //Print("batch: p:%s, h:%s\n",(char*) feOptValue(FE_OPT_MPPORT),(char*) feOptValue(FE_OPT_MPHOST));
      //exit(0);
    }
    else
    {
      Print("** missing arguments: -b requires --link/--MPhost/--MPport\n");
      return 1;
    }
  }
  setjmp(si_start_jmpbuf);
  yyparse();
  m2_end(0);
  return 0;
}

