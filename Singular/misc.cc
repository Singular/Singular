/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:
*/

#include <string.h>
#include "mod2.h"
#ifndef macintosh
#include <unistd.h>
#endif
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "tok.h"
#include "febase.h"
#include "cntrlc.h"
#include "mmemory.h"
#include "ipid.h"
#include "ipshell.h"
#include "kstd1.h"
#include "subexpr.h"
#include "timer.h"
#include "intvec.h"
#define SI_DONT_HAVE_GLOBAL_VARS

#ifdef HAVE_FACTORY
#include <factory.h>
#endif

/* version strings */
#ifdef HAVE_LIBFAC_P
  extern const char * libfac_version;
  extern const char * libfac_date;
#endif
#ifdef HAVE_GMP
extern "C" {
#include <gmp.h>
}
#endif
#ifdef HAVE_MPSR
#include <MP_Config.h>
#endif

/*0 implementation*/

/*2
* initialize components of Singular
*/
int inits(void)
{
  int t;
/*4 signal handler:*/
  init_signals();
/*4 randomize: */
  t=initTimer();
  /*t=(int)time(NULL);*/
  if (t==0) t=1;
#ifdef HAVE_RTIMER
  initRTimer();
#endif
#ifdef buildin_rand
  siSeed=t;
#else
  srand((unsigned int)t);
#endif
#ifdef HAVE_FACTORY
  factoryseed(t);
#endif
/*4 private data of other modules*/
  I_FEbase();
  memset(&sLastPrinted,0,sizeof(sleftv));
  sLastPrinted.rtyp=NONE;
#ifdef HAVE_MPSR
  extern void mpsr_Init();
  mpsr_Init();
#endif
  return t;
}

/*2
* the global exit routine of Singular
*/
extern "C" {
void m2_end(short i)
{
  if (i==0)
  {
    #ifdef HAVE_TCL
    if (tclmode)
      PrintTCL('Q',0,NULL);
    else
    #endif
      printf("Auf Wiedersehen.\n");
  }
  else
  {
    #ifdef HAVE_TCL
    if (tclmode)
      PrintTCL('Q',0,NULL);
    else
    #endif
      printf("\nhalt %d\n",i);
  }
  exit(i);
}
}

/*2
* the renice routine for very large jobs
* works only on unix machines,
* testet on : linux, HP 9.0
*
*#ifndef MSDOS
*#ifndef macintosh
*#include <sys/times.h>
*#include <sys/resource.h>
*extern "C" int setpriority(int,int,int);
*void very_nice()
*{
*#ifndef NO_SETPRIORITY
*  setpriority(PRIO_PROCESS,0,19);
*#endif
*  sleep(10);
*}
*#else
*void very_nice(){}
*#endif
*#else
*void very_nice(){}
*#endif
*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#ifdef buildin_rand
/*
 *
 *  A prime modulus multiplicative linear congruential
 *  generator (PMMLCG), or "Lehmer generator".
 *  Implementation directly derived from the article:
 *
 *        S. K. Park and K. W. Miller
 *        Random Number Generators: Good Ones are Hard to Find
 *        CACM vol 31, #10. Oct. 1988. pp 1192-1201.
 *
 *  Using the following multiplier and modulus, we obtain a
 *  generator which:
 *
 *        1)  Has a full period: 1 to 2^31 - 2.
 *        2)  Is testably "random" (see the article).
 *        3)  Has a known implementation by E. L. Schrage.
 */


#define  A        16807L        /*  A "good" multiplier          */
#define  M   2147483647L        /*  Modulus: 2^31 - 1          */
#define  Q       127773L        /*  M / A                  */
#define  R         2836L        /*  M % A                  */


int siSeed = 1L;


int siRand()
{
  siSeed = A * (siSeed % Q) - R * (siSeed / Q);

  if ( siSeed < 0 )
    siSeed += M;

  return( siSeed );
}
#endif

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
#define HELP_OK        0

#ifdef buildin_help

#define FIN_INDEX    '\037'
#define not  !
#define HELP_NOT_OPEN  1
#define HELP_NOT_FOUND 2
#ifndef macintosh
#define Index_File     SINGULAR_INFODIR "/singular.hlp"
#define Help_File      SINGULAR_INFODIR "/singular.hlp"
#else
#define Index_File     "singular.hlp"
#define Help_File      "singular.hlp"
#endif
#define BUF_LEN        128
#define IDX_LEN        64
#define MAX_LINES      21

static int compare(char *s1,char *s2)
{
  for(;*s1==*s2;s1++,s2++)
     if(*s2=='\0') return(0);
  return(*s2);
}

#ifdef macintosh
static char tolow(char p)
#else
static inline char tolow(char p)
#endif
{
  if (('A'<=p)&&(p<='Z')) return p | 040;
  return p;
}

/*************************************************/
static int show(unsigned long offset,FILE *help, char *close)
{ char buffer[BUF_LEN+1];
  int  lines = 0;

  if( help== NULL)
    if( (help = feFopen(Help_File, "r")) == NULL)
      return HELP_NOT_OPEN;

  fseek(help,  (long)(offset+1), (int)0);
  while( !feof(help)
        && *fgets(buffer, BUF_LEN, help) != EOF
        && buffer[0] != FIN_INDEX)
  { (void)printf("%s", buffer);
    if(lines++> MAX_LINES)
    { printf(
#ifdef macintosh
      "\n Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
      "\n Press <RETURN> to continue or x to exit help.\n");
#endif
      fflush(stdout);
      *close = (char)getchar();
      if(*close=='x')
      {
        getchar();
        break;
      }
      lines=0;
    }
  }
  if(*close!='x')
  {
    printf(
#ifdef macintosh
      "\nEnd of part. Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
      "\nEnd of part. Press <RETURN> to continue or x to exit help.\n");
#endif
    fflush(stdout);
    *close = (char)getchar();
    if(*close=='x')
      getchar();
  }
  return HELP_OK;
}

/*************************************************/
int singular_manual(char *str)
{ FILE *index=NULL,*help=NULL;
  unsigned long offset;
  char *p,close;
  int done = 0;
  char buffer[BUF_LEN+1],
       Index[IDX_LEN+1],
       String[IDX_LEN+1];

  if( (index = feFopen(Index_File, "r",NULL,TRUE)) == NULL)
  {
    return HELP_NOT_OPEN;
  }

  for(p=str; *p; p++) *p = tolow(*p);/* */
  do
  {
    p--;
  }
  while ((p != str) && (*p<=' '));
  p++;
  *p='\0';
  (void)sprintf(String, " %s ", str);

  while(!feof(index)
        && *fgets(buffer, BUF_LEN, index) != EOF
        && buffer[0] != FIN_INDEX);

  while(!feof(index))
  {
    (void)fgets(buffer, BUF_LEN, index); /* */
    (void)sscanf(buffer, "Node:%[^\177]\177%ld\n", Index, &offset);
    for(p=Index; *p; p++) *p = tolow(*p);/* */
    (void)strcat(Index, " ");
    if( strstr(Index, String)!=NULL)
    {
      done++; (void)show(offset, help, &close);
    }
    Index[0]='\0';
    if(close=='x')
    break;
  }
  (void)fclose(index);
  (void)fclose(help);
  if(not done)
  {
    Warn("`%s` not found",String);
    return HELP_NOT_FOUND;
  }
  return HELP_OK;
}
#endif
/*************************************************/

void singular_help(char *str,BOOLEAN example)
{
  char *s=str;
  while (*s==' ') s++;
  char *ss=s;
  while (*ss!='\0') ss++;
  while (*ss<=' ')
  {
    *ss='\0';
    ss--;
  }
  /* --------- is it a proc ? --------------------------------*/
  idhdl h=idroot->get(s,myynest);
  if ((h!=NULL) && (IDTYP(h)==PROC_CMD))
  {
    char *lib=iiGetLibName(IDSTRING(h));
    Print("// proc %s ",s);
    if((lib==NULL)||(*lib=='\0'))
    {
      PrintLn();
    }
    else
    {
      Print("from lib %s\n",lib);
      if (!example)
        iiGetLibProcBuffer(lib,s,0);
      else
      {
        s=iiGetLibProcBuffer(lib,s,2);
        if (s!=NULL)
        {
          if (strlen(s)>5) iiEStart(s); /*newBuffer(s,BT_execute);*/
          else FreeL((ADDRESS)s);
        }
      }
    }
  }
  else if (example)
  {
    Werror("%s not found",s);
  }
  else
  {
  /* --------- is it a library ? --------------------------------*/
    FILE *fp=feFopen(str,"rb");
    if (fp!=NULL)
    {
      char buf[256];
      BOOLEAN found=FALSE;
      while (fgets( buf, sizeof(buf), fp))
      {
        if (strncmp(buf,"//",2)==0)
        {
          if (found) return;
        }
        else if ((strncmp(buf,"proc ",5)==0)||(strncmp(buf,"LIB ",4)==0))
        {
          if (!found) Warn("no help part in library found");
          return;
        }
        else
        {
          found=TRUE;
          PrintS(buf);
        }
      }
    }
  /* --------- everything else is for the manual ----------------*/
    else
    {
#ifdef buildin_help
      singular_manual(str);
#else
      char tmp[150];
      char tmp2[150];
      char strstr[100];
      sprintf(tmp,"%s/singular.hlp", SINGULAR_INFODIR);
      if (strcmp(str,"index")==0)
         strstr[0]='\0';
       else
         sprintf(strstr," Index \"%s\"",str);
      if (!access(tmp, R_OK))
      {
        sprintf(tmp, "info -f %s/singular.hlp %s", SINGULAR_INFODIR, strstr);
      }
      else
      {
        FILE *fd = feFopen("singular.hlp", "r", tmp2, FALSE);
        if (fd != NULL)
        {
          fclose(fd);
          sprintf(tmp, "info -f %s %s", tmp2,strstr);
        }
        else
          sprintf(tmp,"info singular %s",strstr);
      }
      system(tmp);
#ifndef MSDOS
      //sprintf(tmp,"clear");
      //system(tmp);
#endif
#endif
    }
  }
}


struct soptionStruct
{
  char * name;
  int   setval;
  int   resetval;
};

struct soptionStruct optionStruct[]=
{
  {"prot",         Sy_bit(OPT_PROT),           ~Sy_bit(OPT_PROT)   },
  {"redSB",        Sy_bit(OPT_REDSB),          ~Sy_bit(OPT_REDSB)   },
  /* 2 Gebauer/Moeller */
  {"notSugar",     Sy_bit(OPT_NOT_SUGAR),      ~Sy_bit(OPT_NOT_SUGAR)   },
  {"interrupt",    Sy_bit(OPT_INTERRUPT),      ~Sy_bit(OPT_INTERRUPT)   },
  {"sugarCrit",    Sy_bit(OPT_SUGARCRIT),      ~Sy_bit(OPT_SUGARCRIT)   },
  /* {"teach",     Sy_bit(OPT_DEBUG),          ~Sy_bit(OPT_DEBUG)  }, */
  /* 7 cancel unit */
  {"morePairs",    Sy_bit(OPT_MOREPAIRS),      ~Sy_bit(OPT_MOREPAIRS)   },
  /* 9 return SB in syz, quotient, intersect */
  {"returnSB",     Sy_bit(OPT_RETURN_SB),      ~Sy_bit(OPT_RETURN_SB)  },
  {"fastHC",       Sy_bit(OPT_FASTHC),         ~Sy_bit(OPT_FASTHC)  },
  /* 11-19 sort in L/T */
  /* 20 redBest */
  {"keepvars",     Sy_bit(OPT_KEEPVARS),       ~Sy_bit(OPT_KEEPVARS) },
  {"staircaseBound",Sy_bit(OPT_STAIRCASEBOUND),~Sy_bit(OPT_STAIRCASEBOUND)  },
  {"multBound",    Sy_bit(OPT_MULTBOUND),      ~Sy_bit(OPT_MULTBOUND)  },
  {"degBound",     Sy_bit(OPT_DEGBOUND),       ~Sy_bit(OPT_DEGBOUND)  },
  /* 25 no redTail(p)/redTail(s) */
  {"redTail",      Sy_bit(OPT_REDTAIL),        ~Sy_bit(OPT_REDTAIL)  },
  {"intStrategy",  Sy_bit(OPT_INTSTRATEGY),    ~Sy_bit(OPT_INTSTRATEGY)  },
  /* 27 stop at HC (finiteDeterminacyTest) */
  {"minRes",       Sy_bit(OPT_MINRES),         ~Sy_bit(OPT_MINRES)  },
  /* 30: use not regularity for syz */
  {"notRegularity",Sy_bit(OPT_NOTREGULARITY),  ~Sy_bit(OPT_NOTREGULARITY)  },
  {"weightM",      Sy_bit(OPT_WEIGHTM),        ~Sy_bit(OPT_WEIGHTM)  },
/*special for "none" and also end marker for showOption:*/
  {"ne",           0,                          0 }
};

struct soptionStruct verboseStruct[]=
{
  {"mem",      Sy_bit(V_SHOW_MEM),  ~Sy_bit(V_SHOW_MEM)   },
  {"yacc",     Sy_bit(V_YACC),      ~Sy_bit(V_YACC)       },
  {"redefine", Sy_bit(V_REDEFINE),  ~Sy_bit(V_REDEFINE)   },
  {"reading",  Sy_bit(V_READING),   ~Sy_bit(V_READING)    },
  {"loadLib",  Sy_bit(V_LOAD_LIB),  ~Sy_bit(V_LOAD_LIB)   },
  {"debugLib", Sy_bit(V_DEBUG_LIB), ~Sy_bit(V_DEBUG_LIB)  },
  {"loadProc", Sy_bit(V_LOAD_PROC), ~Sy_bit(V_LOAD_PROC)  },
  {"defRes",   Sy_bit(V_DEF_RES),   ~Sy_bit(V_DEF_RES)    },
  {"debugMem", Sy_bit(V_DEBUG_MEM), ~Sy_bit(V_DEBUG_MEM)  },
  {"usage",    Sy_bit(V_SHOW_USE),  ~Sy_bit(V_SHOW_USE)   },
  {"Imap",     Sy_bit(V_IMAP),      ~Sy_bit(V_IMAP)       },
  {"prompt",   Sy_bit(V_PROMPT),    ~Sy_bit(V_PROMPT)     },
  {"notWarnSB",Sy_bit(V_NSB),       ~Sy_bit(V_NSB)        },
/*special for "none" and also end marker for showOption:*/
  {"ne",         0,          0 }
};

BOOLEAN setOption(leftv res, leftv v)
{
  char *n;
  do
  {
    n=v->name;
    if (n==NULL) return TRUE;
    v->name=NULL;

    int i;

    if(strcmp(n,"get")==0)
    {
      intvec *w=new intvec(2);
      (*w)[0]=test;
      (*w)[1]=verbose;
      res->rtyp=INTVEC_CMD;
      res->data=(void *)w;
      goto okay;
    }
    if(strcmp(n,"set")==0)
    {
      if((v->next!=NULL)
      &&(v->next->Typ()==INTVEC_CMD))
      {
        v=v->next;
        intvec *w=(intvec*)v->Data();
        test=(*w)[0];
        verbose=(*w)[1];

        if (TEST_OPT_INTSTRATEGY && (currRing!=NULL) && (currRing->ch>=2))
        {
          test &=~Sy_bit(OPT_INTSTRATEGY);
        }
        goto okay;
      }
    }
    if(strcmp(n,"none")==0)
    {
      test=0;
      verbose=0;
      goto okay;
    }
    for (i=0; (i==0) || (optionStruct[i-1].setval!=0); i++)
    {
      if (strcmp(n,optionStruct[i].name)==0)
      {
        if (optionStruct[i].setval & validOpts)
          test |= optionStruct[i].setval;
        else
          Warn("cannot set option");
        if (TEST_OPT_INTSTRATEGY && (currRing!=NULL) && (currRing->ch>=2))
        {
          test &=~Sy_bit(OPT_INTSTRATEGY);
        }
        goto okay;
      }
      else if ((strncmp(n,"no",2)==0)
      && (strcmp(n+2,optionStruct[i].name)==0))
      {
        if (optionStruct[i].setval & validOpts)
        {
          test &= optionStruct[i].resetval;
        }
        else
          Warn("cannot clear option");
        goto okay;
      }
    }
    for (i=0; (i==0) || (verboseStruct[i-1].setval!=0); i++)
    {
      if (strcmp(n,verboseStruct[i].name)==0)
      {
        verbose |= verboseStruct[i].setval;
        #ifdef YYDEBUG
        #if YYDEBUG
        if (BVERBOSE(V_YACC)) yydebug=1;
        else                  yydebug=0;
        #endif
        #endif
        goto okay;
      }
      else if ((strncmp(n,"no",2)==0)
      && (strcmp(n+2,verboseStruct[i].name)==0))
      {
        verbose &= verboseStruct[i].resetval;
        #ifdef YYDEBUG
        #if YYDEBUG
        if (BVERBOSE(V_YACC)) yydebug=1;
        else                  yydebug=0;
        #endif
        #endif
        goto okay;
      }
    }
    Werror("unknown option `%s`",n);
  okay:
    FreeL((ADDRESS)n);
    v=v->next;
  } while (v!=NULL);
  return FALSE;
}

void showOption()
{
  int i;
  BITSET tmp;

  PrintS("//options:");
  if ((test!=0)||(verbose!=0))
  {
    tmp=test;
    if(tmp)
    {
      for (i=0; optionStruct[i].setval!=0; i++)
      {
        if (optionStruct[i].setval & test)
        {
          Print(" %s",optionStruct[i].name);
          tmp &=optionStruct[i].resetval;
        }
      }
      for (i=0; i<32; i++)
      {
        if (tmp & Sy_bit(i)) Print(" %d",i);
      }
    }
    tmp=verbose;
    if (tmp)
    {
      for (i=0; verboseStruct[i].setval!=0; i++)
      {
        if (verboseStruct[i].setval & tmp)
        {
          Print(" %s",verboseStruct[i].name);
          tmp &=verboseStruct[i].resetval;
        }
      }
      for (i=1; i<32; i++)
      {
        if (tmp & Sy_bit(i)) Print(" %d",i+32);
      }
    }
    PrintLn();
  }
  else
    PrintS(" none\n");
}

#ifndef HAVE_STRSTR
char *strstr(const char *haystack, const char *needle)
{
  char *found = strchr(haystack,*needle);
  if (*(needle+1)!='\0')
  {
    while((found !=NULL) && (strncmp(found+1,needle+1,strlen(needle+1))!=0))
    {
      found=strchr(found+1,*needle);
    }
  }
  return found;
}
#endif

char * versionString()
{
  StringSet("");
#ifdef HAVE_FACTORY
              StringAppend("\tfactory (%s),\n", factoryVersion);
#endif
#ifdef HAVE_LIBFAC_P
              StringAppend("\tlibfac(%s,%s),\n",libfac_version,libfac_date);
#endif
#ifdef SRING
              StringAppend("\tsuper algebra,\n");
#endif
#ifdef DRING
              StringAppend("\tWeyl algebra,\n");
#endif
#ifdef HAVE_GMP
#if defined (__GNU_MP_VERSION) && defined (__GNU_MP_VERSION_MINOR)
              StringAppend("\tGMP(%d.%d),\n",__GNU_MP_VERSION,__GNU_MP_VERSION_MINOR);
#elif defined (HAVE_SMALLGMP)
              StringAppend("\tSmallGMP(2.0.2.0),\n");
#else
              StringAppend("\tGMP(1.3),\n");
#endif
#endif
#ifdef HAVE_DBM
              StringAppend("\tDBM,\n");
#endif
#ifdef HAVE_MPSR
              StringAppend("\tMP(%s),\n",MP_VERSION);
#endif
#if defined(HAVE_READLINE) && !defined(FEREAD)
              StringAppend("\tlibreadline,\n");
#endif
#ifdef HAVE_FEREAD
              StringAppend("\temulated libreadline,\n");
#endif
#ifdef HAVE_INFO
              StringAppend("\tinfo,\n");
#else
              StringAppend("\twithout info,\n");
#endif
#ifdef TEST
              StringAppend("\tTESTs,\n");
#endif
#if YYDEBUG
              StringAppend("\tYYDEBUG=1,\n");
#endif
#ifdef MDEBUG
              StringAppend("\tMDEBUG=%d,\n",MDEBUG);
#endif
#ifndef __OPTIMIZE__
              StringAppend("\t-g,\n");
#endif
              StringAppend("\trandom=%d\n",siRandomStart);
#ifdef MSDOS
              char *p=getenv("SPATH");
#else
              char *p=getenv("SINGULARPATH");
#endif
              if (p!=NULL)
                return StringAppend("search path:%s:%s",p,SINGULAR_DATADIR);
              else
                return StringAppend("search path:%s", SINGULAR_DATADIR);
}
