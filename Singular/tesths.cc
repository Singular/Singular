/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tesths.cc,v 1.9 1997-04-28 17:47:16 Singular Exp $ */

/*
* ABSTRACT - initialize SINGULARs components, run Script and start SHELL
*/

#ifndef macintosh
#include <unistd.h>
#endif
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
#include "febase.h"
#include "cntrlc.h"
#include "mmemory.h"
#include "silink.h"
#include "timer.h"


/*0 implementation*/
char * thisfile;
int main(          /* main entry to Singular */
    int argc,      /* number of parameter */
    char** argv)   /* parameter array */
{
  /* initialize components */
  siRandomStart=inits();
#ifdef INIT_BUG
  jjInitTab1();
#endif
#ifdef GENTABLE
  extern ttGen1();
  extern ttGen2();
#ifdef HAVE_MPSR    
  extern mpsr_ttGen(); // For initialization of (CMD, MP_COP) tables
  mpsr_ttGen();
#endif  
  ttGen1();
  ttGen2();
#else
  int i;
  thisfile = argv[0];
  /*. process parameters */
  for (;(argc > 1) && (!feBatch); --argc, ++argv)
  {
    if (strcmp(argv[1], "-m") == 0)
    {
      {
        if (argc > 2)
        {
          char* ptr = NULL;
#ifdef HAVE_STRTOD          
          double mintime = strtod(argv[2], &ptr);
          if (errno != ERANGE && ptr != argv[2])
#else
          double mintime = 0.0;
          sscanf(argv[2],"%f", &mintime);
          if (mintime != 0.0)
#endif
          {
            argc--;
            argv++;
            SetMinDisplayTime(mintime);
          }
          else
            printf("Can not convert %s to a float\n", argv[2]);
        }
        else
        {
          printf("Need a float to set mintime");
        }
      }
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
      if (argc > 2)
      {
        char* ptr = NULL;
#ifdef HAVE_STRTOL        
        long res = strtol(argv[2], &ptr, 10);
        if (errno != ERANGE && ptr != argv[2])
#else
        long res = 0;
        sscanf(argv[2],"%d", &res);
        if (res != 0)
#endif          
        {
          argc--;
          argv++;
          SetTimerResolution(res);
        }
        else
          printf("Can not convert %s to an integer\n", argv[2]);
      }
      else
      {
        printf("Need an integer to set timer resolution");
      }
    }
    else
    {
    
      if ((argv[1][0] != '-') ||(argv[1][1] == '-'))
        break;
      for (i=1;argv[1][i]!='\0';i++)
      {
        switch (argv[1][i])
        {
            case 'V':
            case 'v':{
              printf("Singular %s %s(%d) %s %s\n",
                     S_VERSION1,S_VERSION2,
                     VERSION_ID,__DATE__,__TIME__);
              printf("with ");
#ifdef HAVE_FACTORY
              printf("factory,");
#endif
#ifdef HAVE_FACTORY
              printf("fac(p),");
#endif
#ifdef SRING
              printf("super algebra,");
#endif
#ifdef DRING
              printf("Weyl algebra,");
#endif
#ifdef HAVE_GMP
              printf("GMP,");
#endif
#ifdef HAVE_DBM
              printf("DBM,");
#endif
#ifdef HAVE_MPSR
              printf("MP,");
#endif
#if defined(HAVE_READLINE) && !defined(FEREAD)
              printf("RL,");
#endif
#ifdef HAVE_FEREAD
              printf("SRL,");
#endif
#ifdef TEST
              printf("TESTs,");
#endif
#if YYDEBUG
              printf("YYDEBUG,");
#endif
#ifdef MDEBUG
              printf("MDEBUG=%d,",MDEBUG);
#endif
#ifndef __OPTIMIZE__
              printf("-g,");
#endif
              printf("random=%d\n",siRandomStart);
#ifdef MSDOS
              char *p=getenv("SPATH");
#else
              char *p=getenv("SINGULARPATH");
#endif
              if (p!=NULL)
                printf("search path:%s:%s\n\n",p,SINGULAR_DATADIR);
              else
                printf("search path:%s\n\n", SINGULAR_DATADIR);
              break;
            }
            case 'e': if ((argv[1][i+1]>'0') && (argv[1][i+1]<='9'))
            {
              i++;
              si_echo = (int)(argv[1][i] - '0');
            }
            else si_echo = 2;
            break;
            case 'r': siRandomStart = 0;
              while((argv[1][i+1]>='0') && (argv[1][i+1]<='9'))
              {
                i++;
                siRandomStart = siRandomStart*10+(int)(argv[1][i] - '0');
              }
#ifdef buildin_rand
              siSeed=siRandomStart;
#else
              srand((unsigned int)siRandomStart);
#endif
              break;
            case 'x': tclmode=TRUE;
              break;
#ifdef HAVE_MPSR
            case 'b': feBatch=TRUE;
#endif
            case 'q': verbose &= ~(Sy_bit(0)|Sy_bit(V_LOAD_LIB));
              break;
            case 't':
#ifdef HAVE_FEREAD
              fe_use_fgets=TRUE;
#endif
              break;
            default : printf("Unknown option -%c\n",argv[1][i]);
              printf("Usage: %s [-bemqtvx] [file]\n",thisfile);
              exit(1);
        }
      }
    }
  }
  

  /*. say hello */
  if (BVERBOSE(0))
  {
    printf(
"              Welcome to SINGULAR                /\n"
"           A Computer Algebra System           o<\n"
" for Algebraic Geometry and Singularity Theory   \\\n\n"
"by: G.-M. Greuel, G. Pfister, H. Schoenemann\n"
"Fachbereich Mathematik der Universitaet, D-67653 Kaiserslautern\n"
"contributions: O.Bachmann,W.Decker,H.Grassmann,B.Martin,M.Messollen,W.Neumann,\n"
"W.Pohl,T.Siebert,R.Stobbe                 e-mail: singular@mathematik.uni-kl.de\n");
    printf("%s %s",S_VERSION1,S_VERSION2);
    printf(" (%d)\n\nPlease note:  EVERY COMMAND MUST END WITH A SEMICOLON \";"
           "\"\n(e.g. help; help command; help General syntax; help ring; quit;)\n\n",
           VERSION_ID);
  }
  else
  if (!feBatch)
  {
#ifdef macintosh
    memcpy(stderr,stdout,sizeof(FILE));
#else
    dup2(1,2);
#endif
  }
  slStandardInit();
#ifdef HAVE_DLD
  dlInit(thisfile);
#endif
  myynest=0;
  /* if script is given */
  if ((argc > 1)&&(argv[1][0]!='-'))
  {
    /* read and run the Script */
    iiPStart(NULL,argv[1],NULL);
  }
  /* start shell */
  if (feBatch)
  {
#ifdef HAVE_MPSR
    extern int Batch_do(int argc, char **argv);
    return Batch_do(argc,argv);
#else
    fprintf(stderr, "Option -b not supported in this version");
    return 1;
#endif // HAVE_MPSR
  }
#ifndef macintosh
#if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
  fe_set_input_mode();
#endif
#endif
  yyparse();
#endif
  return 0;
}
