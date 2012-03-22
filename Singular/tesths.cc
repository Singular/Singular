/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
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
#include <kernel/mod2.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/ipshell.h>
#include <kernel/febase.h>
#include <Singular/cntrlc.h>
#include <omalloc/omalloc.h>
#include <Singular/silink.h>
#include <Singular/ipid.h>
#include <kernel/timer.h>
#include <Singular/sdb.h>
#include <kernel/fegetopt.h>
#include <Singular/feOpt.h>
#include <Singular/distrib.h>
#include <Singular/version.h>
#include <Singular/slInit.h>
#include <Singular/ssiLink.h>
#include <Singular/bigintm.h>
#include <Singular/pyobject_setup.h>
#include <omalloc/omalloc.h>

#ifdef HAVE_FANS
#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbpolytope.h>
#include <callgfanlib/bbfan.h>
#endif

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory/factory.h>
#endif

#ifdef HAVE_SIMPLEIPC
#include <Singular/simpleipc.h>
#endif


extern int siInit(char *);

#if ! defined(LIBSINGULAR)
int mmInit2( void )
{
#if defined(OMALLOC_USES_MALLOC) || defined(X_OMALLOC)
    /* in mmstd.c, for some architectures freeSize() unconditionally uses the *system* free() */
    /* sage ticket 5344: http://trac.sagemath.org/sage_trac/ticket/5344 */
    /* do not rely on the default in Singular as libsingular may be different */
    mp_set_memory_functions(omMallocFunc,omReallocSizeFunc,omFreeSizeFunc);
#else
    mp_set_memory_functions(malloc,reallocSize,freeSize);
#endif
  return 1;
}
int mmInit( void )
{
#ifndef SI_THREADS
  return mmInit2();
#else
  return 1;
#endif
}

/*0 implementation*/
int main(          /* main entry to Singular */
    int argc,      /* number of parameter */
    char** argv)   /* parameter array */
{
  //mmInit();
  // Don't worry: ifdef OM_NDEBUG, then all these calls are undef'ed
  omInitRet_2_Info(argv[0]);
  omInitGetBackTrace();

  siInit(argv[0]);
  init_signals();

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
  }

// semaphore0: CPUs --------------------------------------------------
#ifdef HAVE_SIMPLEIPC
  feOptIndex cpu_opt = feGetOptIndex("cpus");
  int cpus = (int)(long)feOptValue(FE_OPT_CPUS);
  sipc_semaphore_init(0, cpus-1);
#endif

  /* say hello */
  //for official version: not active
  //bigintm_setup();

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
, S_VERSION1,S_VERSION2);
  }
  else
  {
#ifdef HAVE_FACTORY
    if (feOptValue(FE_OPT_SORT)) On(SW_USE_NTL_SORT);
#endif
#ifdef HAVE_SDB
    sdb_flags = 0;
#endif
    dup2(1,2);
    /* alternative:
    *    memcpy(stderr,stdout,sizeof(FILE));
    */
  }
  pyobject_setup();
#ifdef HAVE_FANS
  bbcone_setup();
  bbpolytope_setup();
  bbfan_setup();
#endif /* HAVE_FANS */
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
    #ifdef HAVE_MPSR
      BatchDoProc batch_do = slInitMPBatchDo();
      if (batch_do != NULL)
        return (*batch_do)((char*) feOptValue(FE_OPT_MPPORT),
                         (char*) feOptValue(FE_OPT_MPHOST));
      else
        return 1;
    #else
      assume(0);
    #endif
    }
  }
  setjmp(si_start_jmpbuf);
  yyparse();
  m2_end(0);
  return 0;
}
#endif // not LIBSINGULAR

