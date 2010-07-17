/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

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
#include <Singular/mod2.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/ipshell.h>
#include <kernel/febase.h>
#include <Singular/cntrlc.h>
#include <omalloc.h>
#include <Singular/silink.h>
#include <Singular/ipid.h>
#include <kernel/timer.h>
#include <Singular/sdb.h>
#include <kernel/fegetopt.h>
#include <Singular/feOpt.h>
#include <Singular/distrib.h>
#include <Singular/version.h>
#include <Singular/slInit.h>

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory.h>
#endif

extern int iiInitArithmetic();

const char *singular_date=__DATE__ " " __TIME__;

#include <kernel/si_gmp.h>

int mmInit( void );
int mmIsInitialized=mmInit();

extern "C"
{
  void omSingOutOfMemoryFunc()
  {
    fprintf(stderr, "\nSingular error: no more memory\n");
    omPrintStats(stderr);
    m2_end(14);
    /* should never get here */
    exit(1);
  }
}

int mmInit( void )
{
  if(mmIsInitialized==0)
  {

#ifndef LIBSINGULAR
#if defined(OMALLOC_USES_MALLOC) || defined(X_OMALLOC)
    /* in mmstd.c, for some architectures freeSize() unconditionally uses the *system* free() */
    /* sage ticket 5344: http://trac.sagemath.org/sage_trac/ticket/5344 */
#include <omalloc.h>
    /* do not rely on the default in Singular as libsingular may be different */
    mp_set_memory_functions(omMallocFunc,omReallocSizeFunc,omFreeSizeFunc);
#else
    mp_set_memory_functions(malloc,reallocSize,freeSize);
#endif
#endif // ifndef LIBSINGULAR
    om_Opts.OutOfMemoryFunc = omSingOutOfMemoryFunc;
#ifndef OM_NDEBUG
    om_Opts.ErrorHook = dErrorBreak;
#endif
    omInitInfo();
#ifdef OM_SING_KEEP
    om_Opts.Keep = OM_SING_KEEP;
#endif
  }
  mmIsInitialized=1;
  return 1;
}

#ifdef LIBSINGULAR
int siInit(char *name)
{
  // hack such that all shared' libs in the bindir are loaded correctly
  feInitResources(name);
  iiInitArithmetic();

#if 0
  SingularBuilder::Ptr SingularInstance = SingularBuilder::instance();
#else
  basePack=(package)omAlloc0(sizeof(*basePack));
  currPack=basePack;
  idhdl h;
  h=enterid("Top", 0, PACKAGE_CMD, &IDROOT, TRUE);
  IDPACKAGE(h)->language = LANG_TOP;
  IDPACKAGE(h)=basePack;
  currPackHdl=h;
  basePackHdl=h;

  slStandardInit();
  myynest=0;
#endif
  if (! feOptValue(FE_OPT_NO_STDLIB))
  {
    int vv=verbose;
    verbose &= ~Sy_bit(V_LOAD_LIB);
    iiLibCmd(omStrDup("standard.lib"), TRUE,TRUE,TRUE);
    verbose=vv;
  }
  errorreported = 0;
}
#endif

#if ! defined(LIBSINGULAR)
/*0 implementation*/
int main(          /* main entry to Singular */
    int argc,      /* number of parameter */
    char** argv)   /* parameter array */
{
#ifdef HAVE_FACTORY
  On(SW_USE_NTL);
  Off(SW_USE_GCD_P);
  On(SW_USE_NTL_GCD_0); // On -> seg11 in Old/algnorm, Old/factor...
  On(SW_USE_NTL_GCD_P); // On -> cyle in Short/brnoeth_s: fixed
  On(SW_USE_EZGCD);
  On(SW_USE_CHINREM_GCD);
  //On(SW_USE_fieldGCD);
  //On(SW_USE_EZGCD_P);
  On(SW_USE_QGCD);
  Off(SW_USE_NTL_SORT); // may be changed by an command line option
#endif

#ifdef INIT_BUG
  jjInitTab1();
#endif
#ifdef GENTABLE
  extern void ttGen1();
  extern void ttGen2b();
  extern void ttGen4();
  extern void mpsr_ttGen(); // For initialization of (CMD, MP_COP) tables
  extern char *iparith_inc;
  #ifdef HAVE_MPSR
  extern char *mpsr_Tok_inc;
  #endif
  mpsr_ttGen();
  ttGen4();
  ttGen1();
  ttGen2b();
  rename(iparith_inc,"iparith.inc");
  rename("plural_cmd.xx","plural_cmd.inc");
  #ifdef HAVE_MPSR
  rename(mpsr_Tok_inc,"mpsr_Tok.inc");
  #endif
#else
  // Don't worry: ifdef OM_NDEBUG, then all these calls are undef'ed
  omInitRet_2_Info(argv[0]);
  omInitGetBackTrace();

  /* initialize components */
  siRandomStart=inits();
  feOptSpec[FE_OPT_RANDOM].value = (void*) ((long)siRandomStart);
  int optc, option_index;
  const char* errormsg;

  // do this first, because -v might print version path
  feInitResources(argv[0]);
  iiInitArithmetic();

  // parse command line options
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

  /* say hello */
#if 0
  SingularBuilder::Ptr SingularInstance = SingularBuilder::instance();
#else
  {
    basePack=(package)omAlloc0(sizeof(*basePack));
    currPack=basePack;
    idhdl h;
    h=enterid("Top", 0, PACKAGE_CMD, &IDROOT, TRUE);
    IDPACKAGE(h)->language = LANG_TOP;
    IDPACKAGE(h)=basePack;
    currPackHdl=h;
    basePackHdl=h;
  }
#endif
  if (TEST_V_QUIET)
  {
    (printf)(
"                     SINGULAR                             /"
#ifndef MAKE_DISTRIBUTION
"  Development"
#endif
"\n"
" A Computer Algebra System for Polynomial Computations   /   version %s\n"
"                                                       0<\n"
"     by: G.-M. Greuel, G. Pfister, H. Schoenemann        \\   %s\n"
"FB Mathematik der Universitaet, D-67653 Kaiserslautern    \\\n"
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
  slStandardInit();
  myynest=0;
  if (! feOptValue(FE_OPT_NO_STDLIB))
  {
    int vv=verbose;
    verbose &= ~Sy_bit(V_LOAD_LIB);
    iiLibCmd(omStrDup("standard.lib"), TRUE,TRUE,TRUE);
    verbose=vv;
  }
  errorreported = 0;

  // and again, ifdef OM_NDEBUG this call is undef'ed
  // otherwise, it marks all memory allocated so far as static
  // i.e. as memory which is not mention on omPrintUsedAddr:
  //omMarkMemoryAsStatic();

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
  setjmp(si_start_jmpbuf);
  yyparse();
  m2_end(0);
#endif
  return 0;
}
#endif // not LIBSINGULAR

