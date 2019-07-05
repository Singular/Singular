/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: Implementation of option buisness
*/


#include <string.h>

#include "kernel/mod2.h"

#include "factory/factory.h"

#define FE_OPT_STRUCTURE
#include "feOpt.h"

#if !defined(GENERATE_OPTION_INDEX) && !defined(ESINGULAR) && !defined(TSINGULAR)
#include "misc/options.h"
#include "misc/sirandom.h"
#endif

#include "fehelp.h"

#ifdef HAVE_FLINT
#include <flint/flint.h>
#endif

const char SHORT_OPTS_STRING[] = "bdhpqstvxec:r:u:";

//////////////////////////////////////////////////////////////
//
// Generation of feOptIndex
//
#ifdef GENERATE_OPTION_INDEX

#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
int main()
{
  FILE* fd;
#ifdef ESINGULAR
  fd = fopen("feOptES.xx", "w");
#elif defined(TSINGULAR)
  fd = fopen("feOptTS.xx", "w");
#else
  fd = fopen("feOpt.xx", "w");
#endif

  if (fd == NULL) exit(1);

  int i = 0;

  fputs("typedef enum\n{\n", fd);

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
#ifdef ESINGULAR
  rename("feOptES.xx", "feOptES.inc");
#elif defined(TSINGULAR)
  rename("feOptTS.xx", "feOptTS.inc");
#else
  rename("feOpt.xx", "feOpt.inc");
#endif
  return(0);
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
//
// Return: NULL -- everything ok
//         "error-string" on error
#if !defined(ESINGULAR) && !defined(TSINGULAR)

#include "omalloc/omalloc.h"
#include "resources/feResource.h"
#include "kernel/oswrapper/feread.h"
#include "kernel/oswrapper/timer.h"

#include "ipshell.h"
#include "tok.h"
#include "sdb.h"
#include "cntrlc.h"

#include <errno.h>

static const char* feOptAction(feOptIndex opt);
const char* feSetOptValue(feOptIndex opt, char* optarg)
{
  if (opt == FE_OPT_UNDEF) return "option undefined";

  if (feOptSpec[opt].type != feOptUntyped)
  {
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
        omFree(feOptSpec[opt].value);
      if (optarg != NULL)
        feOptSpec[opt].value = omStrDup(optarg);
      else
        feOptSpec[opt].value = NULL;
      feOptSpec[opt].set = 1;
    }
  }
  return feOptAction(opt);
}

const char* feSetOptValue(feOptIndex opt, int optarg)
{
  if (opt == FE_OPT_UNDEF) return "option undefined";

  if (feOptSpec[opt].type != feOptUntyped)
  {
    if (feOptSpec[opt].type == feOptString)
      return "option value needs to be an integer";

    feOptSpec[opt].value = (void*)(long) optarg;
  }
  return feOptAction(opt);
}

static const char* feOptAction(feOptIndex opt)
{
  // do some special actions
  switch(opt)
  {
      case FE_OPT_BATCH:
        if (feOptSpec[FE_OPT_BATCH].value)
          fe_fgets_stdin=fe_fgets_dummy;
        return NULL;

      case FE_OPT_HELP:
        feOptHelp(feArgv0);
        return NULL;

      case FE_OPT_PROFILE:
         traceit=1024;
         return NULL;

      case FE_OPT_QUIET:
        if (feOptSpec[FE_OPT_QUIET].value)
          si_opt_2 &= ~(Sy_bit(0)|Sy_bit(V_LOAD_LIB));
        else
          si_opt_2 |= Sy_bit(V_LOAD_LIB)|Sy_bit(0);
        return NULL;

      case FE_OPT_NO_TTY:
        if (feOptSpec[FE_OPT_NO_TTY].value)
          fe_fgets_stdin=fe_fgets;
        return NULL;

      case FE_OPT_SDB:
      #ifdef HAVE_SDB
        if (feOptSpec[FE_OPT_SDB].value)
          sdb_flags = 1;
        else
          sdb_flags = 0;
      #endif
        return NULL;

      case FE_OPT_VERSION:
        {
        char *s=versionString();
        printf("%s",s);
        omFree(s);
        return NULL;
        }

      case FE_OPT_ECHO:
        si_echo = (int) ((long)(feOptSpec[FE_OPT_ECHO].value));
        if (si_echo < 0 || si_echo > 9)
          return "argument of option is not in valid range 0..9";
        return NULL;

      case FE_OPT_RANDOM:
        siRandomStart = (unsigned int) ((unsigned long)
                                          (feOptSpec[FE_OPT_RANDOM].value));
        siSeed=siRandomStart;
        factoryseed(siRandomStart);
        return NULL;

      case FE_OPT_EMACS:
        if (feOptSpec[FE_OPT_EMACS].value)
        {
          // print EmacsDir and InfoFile so that Emacs
          // mode can pcik it up
          Warn("EmacsDir: %s", (feResource('e' /*"EmacsDir"*/) != NULL ?
                                feResource('e' /*"EmacsDir"*/) : ""));
          Warn("InfoFile: %s", (feResource('i' /*"InfoFile"*/) != NULL ?
                                feResource('i' /*"InfoFile"*/) : ""));
        }
        return NULL;

      case FE_OPT_NO_WARN:
        if (feOptSpec[FE_OPT_NO_WARN].value)
          feWarn = FALSE;
        else
          feWarn = TRUE;
        return NULL;

      case FE_OPT_NO_OUT:
        if (feOptSpec[FE_OPT_NO_OUT].value)
          feOut = FALSE;
        else
          feOut = TRUE;
        return NULL;

      case FE_OPT_MIN_TIME:
      {
        double mintime = atof((char*) feOptSpec[FE_OPT_MIN_TIME].value);
        if (mintime <= 0) return "invalid float argument";
        SetMinDisplayTime(mintime);
        return NULL;
      }

      case FE_OPT_BROWSER:
        feHelpBrowser((char*) feOptSpec[FE_OPT_BROWSER].value, 1);

      case FE_OPT_TICKS_PER_SEC:
      {
        int ticks = (int) ((long)(feOptSpec[FE_OPT_TICKS_PER_SEC].value));
        if (ticks <= 0)
          return "integer argument must be larger than 0";
        SetTimerResolution(ticks);
        return NULL;
      }

      case FE_OPT_DUMP_VERSIONTUPLE:
      {
        feOptDumpVersionTuple();
        return NULL;
      }

      #ifdef HAVE_FLINT
      #if __FLINT_RELEASE >= 20503
      case FE_OPT_FLINT_THREADS:
      {
        slong nthreads = (slong)feOptSpec[FE_OPT_FLINT_THREADS].value;
        nthreads = FLINT_MAX(nthreads, WORD(1));
        flint_set_num_threads(nthreads);
        int * cpu_affinities = new int[nthreads];
        for (slong i = 0; i < nthreads; i++)
          cpu_affinities[i] = (int)i;
        flint_set_thread_affinity(cpu_affinities, nthreads);
        delete[] cpu_affinities;
        return NULL;
      }
      #endif
      #endif

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
    if (feOptSpec[i].help != NULL && feOptSpec[i].type != feOptUntyped
#ifndef SING_NDEBUG
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
        Print("// --%-15s %d\n", feOptSpec[i].name, (int)(long)feOptSpec[i].value);
      }
    }
    i++;
  }
}

#endif // ! ESingular

// Prints help message
void feOptHelp(const char* name)
{
  int i = 0;
  char tmp[20];
#if defined(ESINGULAR)
  printf("ESingular starts up Singular within emacs;\n");
#elif defined(TSINGULAR)
  printf("TSingular starts up Singular within a terminal window;\n");
#endif
  printf("Singular is a Computer Algebra System (CAS) for Polynomial Computations.\n");
  printf("Usage: %s [options] [file1 [file2 ...]]\n", name);
  printf("Options:\n");

  while (feOptSpec[i].name != 0)
  {
    if (feOptSpec[i].help != NULL
#ifdef SING_NDEBUG
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

        printf(" %c%c --%-20s %s\n",
               (feOptSpec[i].val != LONG_OPTION_RETURN ? '-' : ' '),
               (feOptSpec[i].val != LONG_OPTION_RETURN ? feOptSpec[i].val : ' '),
               tmp,
               feOptSpec[i].help);
      }
      else
      {
        printf(" %c%c --%-20s %s\n",
               (feOptSpec[i].val != LONG_OPTION_RETURN ? '-' : ' '),
               (feOptSpec[i].val != LONG_OPTION_RETURN ? feOptSpec[i].val : ' '),
               feOptSpec[i].name,
               feOptSpec[i].help);
      }
    }
    i++;
  }

  printf("\nFor more information, type `help;' from within Singular or visit\n");
  printf("http://www.singular.uni-kl.de or consult the\n");
  printf("Singular manual (available as on-line info or html manual).\n");
}

void feOptDumpVersionTuple(void)
{
  printf("%s\n",VERSION);
}

#endif // GENERATE_OPTION_INDEX
