/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: tesths.cc,v 1.30 1998-02-27 14:06:26 Singular Exp $ */

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
  extern ttGen1();
  extern ttGen2();
  #ifndef macintosh
    extern mpsr_ttGen(); // For initialization of (CMD, MP_COP) tables
    mpsr_ttGen();
  #endif
  ttGen1();
  ttGen2();
#else
  int i;
  thisfile = argv[0];
  BOOLEAN load_std_lib=TRUE;
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
          double mintime = 0;
          sscanf(argv[2],"%f", &mintime);
          if (mintime != 0.0)
#endif
          {
            argc--;
            argv++;
            SetMinDisplayTime(mintime);
          }
          else
            fprintf(stderr, "Can not convert %s to a float\n", argv[2]);
        }
        else
        {
          fprintf(stderr, "Need a float to set mintime");
        }
      }
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
      
      if (argc > 2)
      {
        char* ptr = NULL;
        // OLAF: try to avoid using long:
#ifdef HAVE_STRTOL
        long res = strtol(argv[2], &ptr, 10);
        if (errno != ERANGE && ptr != argv[2] && res > 0)
#else
          long res = 0;
        sscanf(argv[2],"%d", &res);
        if (res > 0)
#endif
        {
          argc--;
          argv++;
          i=0;
          SetTimerResolution(res);
        }
        else
          fprintf(stderr,"Can not convert %s to an integer > 0\n", argv[2]);
      }
      else
      {
        long res=0;
        while ((argv[1][i+1]>='0') && (argv[1][i+1]<='9'))
        {
          i++;
          res = res*10+(int)(argv[1][i] - '0');
          SetTimerResolution(res);
        }
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
              printf("Singular %s  %s  (%d)  %s %s\n",
                     S_VERSION1,S_VERSION2,
                     SINGULAR_VERSION_ID,__DATE__,__TIME__);
              printf("with\n");
              printf(versionString());
              printf("\n\n");
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
              #ifdef HAVE_FACTORY
                factoryseed(siRandomStart);
              #endif
              break;
#ifdef HAVE_TCL	      
            case 'x': tclmode=TRUE;
              break;
#endif	      
#ifdef HAVE_MPSR
            case 'b': feBatch=TRUE;
#endif
            case 'q': verbose &= ~(Sy_bit(0)|Sy_bit(V_LOAD_LIB));
              break;
            case 't':
#if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
              fe_use_fgets=TRUE;
#endif
              break;
            case 'n':
              load_std_lib=FALSE;
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
"              Welcome to SINGULAR                  /\n"
"           A Computer Algebra System             o<\n"
" for Commutative Algebra and Algebraic Geometry    \\\n\n"
"by: G.-M. Greuel, G. Pfister, H. Schoenemann\n"
"Fachbereich Mathematik der Universitaet, D-67653 Kaiserslautern\n"
"contributions: O.Bachmann,W.Decker,H.Grassmann,B.Martin,M.Messollen,W.Neumann,\n"
"\tW.Pohl,T.Siebert,R.Stobbe,T.Wichmann\n"
"e-mail: singular@mathematik.uni-kl.de\n");
    printf("%s  %s  (%d)",S_VERSION1,S_VERSION2, SINGULAR_VERSION_ID);
    printf("\n\nPlease note:  EVERY COMMAND MUST END WITH A SEMICOLON \";"
           "\"\n(e.g. help; help command; help General syntax; help ring; quit;)\n\n");
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
  dlInit(thisfile);
  myynest=0;
  if (load_std_lib)
  {
    iiLibCmd(mstrdup("standard.lib"),TRUE);
  }  
  errorreported = 0;
#ifndef macintosh
#if defined(HAVE_FEREAD) || defined(HAVE_READLINE)
  fe_set_input_mode();
#endif
#endif
  setjmp(si_start_jmpbuf);
  /* if script is given */
  if ((argc > 1)&&(argv[1][0]!='-'))
  {
    /* read and run the Script */
    argc=1;
    newFile(argv[1]);
  }
  else
  {
    currentVoice=feInitStdin();
  }
  // set up voice for .singularc
  {
    char * where=(char *)AllocL(256);
    FILE * rc=feFopen(".singularrc","r",where,FALSE);
    if (rc!=NULL)
    {
      fclose(rc);
      newFile(where);
    }
    FreeL((ADDRESS)where);
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
  setjmp(si_start_jmpbuf);
  yyparse();
#endif
  //Print("at very end\n");
  m2_end(0);
  return 0;
}
