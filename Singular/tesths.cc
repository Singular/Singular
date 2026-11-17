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
#include "kernel/oswrapper/vspace.h"

#include "ipshell.h"
#include "cntrlc.h"
#include "links/silink.h"
#include "ipid.h"
#include "sdb.h"
#include "feOpt.h"
#include "misc/distrib.h"
#include "mmalloc.h"
#include "tok.h"
#include "resources/fegetopt.h"

#include "Singular/countedref.h"
#include "Singular/pyobject_setup.h"

#include <unistd.h>
#ifdef HAVE_NTL
#include <NTL/config.h>
#endif

#ifdef HAVE_SIMPLEIPC
#include "Singular/links/simpleipc.h"
#endif


extern void siInit(char *);
#ifdef PSINGULAR
GLOBAL_VAR char *global_argv0;
#endif
EXTERN_VAR FILE* File_Log;
EXTERN_VAR BOOLEAN FE_OPT_NO_SHELL_FLAG;

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
// how many processes ? -----------------------------------------------------
  {
    int cpus=1;
    int cpu_n;
    #ifdef _SC_NPROCESSORS_ONLN
    if ((cpu_n=sysconf(_SC_NPROCESSORS_ONLN))>cpus) cpus=cpu_n;
    #elif defined(_SC_NPROCESSORS_CONF)
    if ((cpu_n=sysconf(_SC_NPROCESSORS_CONF))>cpus) cpus=cpu_n;
    #endif
    #ifdef HAVE_SIMPLEIPC
    if (cpus>SIPC_MAX_SEMAPHORES) cpus=SIPC_MAX_SEMAPHORES;
    #endif
    feSetOptValue(FE_OPT_CPUS, cpus);
// how many threads ? -----------------------------------------------------
    feSetOptValue(FE_OPT_THREADS, cpus);
  }
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
    // not more than MAX_PROCESS cpus

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
  long cpus=(long)feOptValue(FE_OPT_CPUS);
  #ifdef HAVE_VSPACE
  if (cpus>vspace::internals::MAX_PROCESS)
  {
    cpus=vspace::internals::MAX_PROCESS;
    feSetOptValue(FE_OPT_CPUS,cpus);
  }
  #endif

  /* say hello */

  if (TEST_V_QUIET)
  {
    const char *singular_url = "www.singular.uni-kl.de";
    const char *singular_description =
      "A Computer Algebra System for Polynomial Computations";
    const char *singular_authors =
      "by: J.Boehm, W.Decker, G.-M.Greuel, G.Pfister, H.Schoenemann";
    const char *singular_affiliation =
      "FB Mathematik, RPTU Kaiserslautern-Landau, D-67653 Kaiserslautern";
    const int version_column = 69;
    const int title_mark_column = 67;
    const int url_mark_column = 66;
    const int middle_mark_column = 64;
    const int author_mark_column = 66;
    const int affiliation_mark_column = 67;
    const int url_area_width = url_mark_column - 1;
    const int title_padding = 21;
    const int title_width = 8;
    int title_gap = version_column - title_mark_column - 1;
    if (title_gap < 1) title_gap = 1;
    int url_padding = title_padding
                      + (title_width - (int)strlen(singular_url))/2;
    if (url_padding < 0) url_padding = 0;
    int url_trailing = url_area_width
                       - url_padding
                       - (int)strlen(singular_url);
    if (url_trailing < 1) url_trailing = 1;
    int url_gap = version_column - url_mark_column - 1;
    if (url_gap < 1) url_gap = 1;
    int middle_padding = middle_mark_column
                         - (int)strlen(singular_description) - 1;
    if (middle_padding < 1) middle_padding = 1;
    int middle_gap = version_column - middle_mark_column - 2;
    if (middle_gap < 1) middle_gap = 1;
    int author_padding = author_mark_column
                         - (int)strlen(singular_authors) - 1;
    if (author_padding < 1) author_padding = 1;
    int author_gap = version_column - author_mark_column - 1;
    if (author_gap < 1) author_gap = 1;
    int affiliation_padding = affiliation_mark_column
                              - (int)strlen(singular_affiliation) - 1;
    if (affiliation_padding < 1) affiliation_padding = 1;

    (printf)("%s%*s%s\n",
             "                     SINGULAR                                     /",
             title_gap, "",
             "Version");
    (printf)("%*s%s%*s/%*s%s\n",
             url_padding, "",
             singular_url,
             url_trailing, "",
             url_gap, "",
             VERSION);
    (printf)("%s%*s0<%*s%s\n",
             singular_description,
             middle_padding, "",
             middle_gap, "",
             VERSION_DATE);
#ifndef MAKE_DISTRIBUTION
    (printf)("%s%*s\\%*s%s\n",
             singular_authors,
             author_padding, "",
             author_gap, "",
             "Dev.");
#else
    (printf)("%s%*s\\\n",
             singular_authors,
             author_padding, "");
#endif
    (printf)("%s%*s\\\n",
             singular_affiliation,
             affiliation_padding, "");
    if (feOptValue(FE_OPT_NO_SHELL))
    {
      WarnS("running in restricted mode:"
      " shell invocation and file links are disallowed");
      FE_OPT_NO_SHELL_FLAG=TRUE;
    }
  }
  else
  {
    FE_OPT_NO_SHELL_FLAG=FALSE;
    if (feOptValue(FE_OPT_SORT)) On(SW_USE_NTL_SORT);
    dup2(1,2);
    /* alternative:
    *    memcpy(stderr,stdout,sizeof(FILE));
    */
  }
  if (feOptValue(FE_OPT_LOG)!=NULL)
  {
    int pid=getpid();
    char *buf=(char*)malloc(10+strlen((char*) feOptSpec[FE_OPT_LOG].value));
    snprintf(buf,10+strlen((char*) feOptSpec[FE_OPT_LOG].value),
      "%s.%d",(char*) feOptSpec[FE_OPT_LOG].value,pid);
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
