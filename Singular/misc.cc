/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:
*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#include "mod2.h"
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

#ifdef HAVE_LIBPARSER
#  include "libparse.h"
#endif /* HAVE_LIBPARSER */

#ifdef HAVE_FACTORY
#include <factory.h>
#endif

/* version strings */
#ifdef HAVE_LIBFAC_P
  extern const char * libfac_version;
  extern const char * libfac_date;
#endif
extern "C" {
#include <gmp.h>
}
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
  #ifdef HAVE_TCL
  if (tclmode)
  {
    PrintTCL('Q',0,NULL);
  }
  #endif
  if (i<=0)
  {
    #ifdef HAVE_TCL
    if (!tclmode)
    #endif
      if (BVERBOSE(0))
      {
        if (i==0)
          printf("Auf Wiedersehen.\n");
        else
          printf("\n$Bye.\n");
      }
    #ifndef macintosh
      #ifdef HAVE_FEREAD
        #ifdef HAVE_ATEXIT
          fe_reset_input_mode();
        #else
          fe_reset_input_mode(0,NULL);
        #endif
      #else
        #ifdef HAVE_READLINE
          fe_reset_input_mode();
        #endif
      #endif
    #endif
    #ifdef sun
      #ifndef __svr4__
        _cleanup();
        _exit(0);
      #endif
    #endif
    exit(0);
  }
  else
  {
    #ifdef HAVE_TCL
    if (!tclmode)
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
#define Index_File     SINGULAR_INFODIR "singular.hlp"
#define Help_File      SINGULAR_INFODIR "singular.hlp"
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
  {
    printf("%s", buffer);
    if(lines++> MAX_LINES)
    {
#ifdef macintosh
      printf("\n Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
      printf("\n Press <RETURN> to continue or x to exit help.\n");
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
#ifdef macintosh
    printf("\nEnd of part. Press <RETURN> to continue or x and <RETURN> to exit help.\n");
#else
    printf("\nEnd of part. Press <RETURN> to continue or x to exit help.\n");
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
#endif // buildin_help
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
#ifdef HAVE_NAMESPACES
  char *p1 = AllocL(strlen(s)), *p2 = AllocL(strlen(s));
  *p1='\0';
  *p2='\0';
  sscanf(s, "%[^:]::%s", p1, p2);
  idhdl h, ns;
  if(*p2) {
    printf("singular_help:(%s, %s)\n", p1, p2);
    ns = namespaceroot->get(p1,0);
    namespaceroot->push(IDPACKAGE(ns), IDID(ns));
    h=namespaceroot->get(p2,myynest);
    namespaceroot->pop();
  } else {
    h=namespaceroot->get(p1,myynest);
  }
  FreeL(p1);
  FreeL(p2);
#else /* HAVE_NAMESPACES */
  /* --------- is it a proc ? --------------------------------*/
  idhdl h=idroot->get(s,myynest);
#endif /* HAVE_NAMESPACES */
  if ((h!=NULL) && (IDTYP(h)==PROC_CMD)
  && (example ||(strcmp(IDPROC(h)->libname, "standard.lib")!=0)))
  {
    char *lib=iiGetLibName(IDPROC(h));
    Print("// proc %s ",s);
    if((lib==NULL)||(*lib=='\0'))
    {
      PrintLn();
    }
    else
    {
      Print("from lib %s\n",lib);
      s=iiGetLibProcBuffer(IDPROC(h), example ? 2 : 0);
      if (!example)
      {
        PrintS(s);
        FreeL((ADDRESS)s);
      }
      else
      {
        if (s!=NULL)
        {
          if (strlen(s)>5) iiEStart(s,IDPROC(h));
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
    char libnamebuf[128];
    FILE *fp=feFopen(str,"rb", libnamebuf);
    if (fp!=NULL)
    {
#ifdef HAVE_LIBPARSER
      extern FILE *yylpin;
      lib_style_types lib_style; // = OLD_LIBSTYLE;

      yylpin = fp;
#  ifdef HAVE_NAMESPACES
      yylplex(str, libnamebuf, &lib_style, IDROOT, GET_INFO);
#  else /* HAVE_NAMESPACES */
      yylplex(str, libnamebuf, &lib_style, GET_INFO);
#  endif /* HAVE_NAMESPACES */
      reinit_yylp();
      if(lib_style == OLD_LIBSTYLE)
      {
        char buf[256];
        fseek(fp, 0, SEEK_SET);
#else /* HAVE_LIBPARSER */
        { char buf[256];
#endif /* HAVE_LIBPARSER */
        Warn( "library %s has an old format. Please fix it for the next time",
              str);
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
#ifdef HAVE_LIBPARSER
      else
      {
        fclose( yylpin );
        PrintS(text_buffer);
        FreeL(text_buffer);
      }
#endif /* HAVE_LIBPARSER */
    }
  /* --------- everything else is for the manual ----------------*/
    else
    {
#ifdef HAVE_TCL
      if(!tclmode)
#endif
     #ifdef buildin_help
        singular_manual(str);
     #else
        system(feGetInfoCall(str));
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
  {"teach",     Sy_bit(OPT_DEBUG),          ~Sy_bit(OPT_DEBUG)  },
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
      #ifdef HAVE_TCL
      if (tclmode)
        PrintTCLS('O',"none");
      #endif
      test=0;
      verbose=0;
      goto okay;
    }
    for (i=0; (i==0) || (optionStruct[i-1].setval!=0); i++)
    {
      if (strcmp(n,optionStruct[i].name)==0)
      {
        if (optionStruct[i].setval & validOpts)
	{
          test |= optionStruct[i].setval;
          #ifdef HAVE_TCL
          if (tclmode)
            PrintTCLS('O',n);
          #endif
	}
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
          #ifdef HAVE_TCL
          if (tclmode)
            PrintTCLS('O',n);
          #endif
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
        #ifdef HAVE_TCL
        if (tclmode)
          PrintTCLS('O',n);
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
        #ifdef HAVE_TCL
        if (tclmode)
          PrintTCLS('O',n);
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

char * showOption()
{
  int i;
  BITSET tmp;

  StringSet("//options:");
  if ((test!=0)||(verbose!=0))
  {
    tmp=test;
    if(tmp)
    {
      for (i=0; optionStruct[i].setval!=0; i++)
      {
        if (optionStruct[i].setval & test)
        {
          StringAppend(" %s",optionStruct[i].name);
          tmp &=optionStruct[i].resetval;
        }
      }
      for (i=0; i<32; i++)
      {
        if (tmp & Sy_bit(i)) StringAppend(" %d",i);
      }
    }
    tmp=verbose;
    if (tmp)
    {
      for (i=0; verboseStruct[i].setval!=0; i++)
      {
        if (verboseStruct[i].setval & tmp)
        {
          StringAppend(" %s",verboseStruct[i].name);
          tmp &=verboseStruct[i].resetval;
        }
      }
      for (i=1; i<32; i++)
      {
        if (tmp & Sy_bit(i)) StringAppend(" %d",i+32);
      }
    }
    return mstrdup(StringAppend(""));
  }
  else
    return mstrdup(StringAppend(" none"));
}

char * versionString()
{
  StringSet("\t");
#ifdef HAVE_FACTORY
              StringAppend("factory(%s),", factoryVersion);
#endif
#ifdef HAVE_LIBFAC_P
              StringAppend("libfac(%s,%s),\n\t",libfac_version,libfac_date);
#endif
#if defined (__GNU_MP_VERSION) && defined (__GNU_MP_VERSION_MINOR)
              StringAppend("GMP(%d.%d),",__GNU_MP_VERSION,__GNU_MP_VERSION_MINOR);
#elif defined (HAVE_SMALLGMP)
              StringAppendS("SmallGMP(2.0.2.0),");
#else
              StringAppendS("GMP(1.3),");
#endif
#ifdef HAVE_MPSR
              StringAppend("MP(%s),",MP_VERSION);
#endif
#if defined(HAVE_READLINE) && !defined(FEREAD)
              StringAppendS("libreadline,\n\t");
#else
#ifdef HAVE_FEREAD
              StringAppendS("emulated libreadline,\n\t");
#else
              StringAppendS("\n\t");
#endif
#endif
#ifdef SRING
              StringAppendS("super algebra,");
#endif
#ifdef DRING
              StringAppendS("Weyl algebra,");
#endif
#ifdef HAVE_DBM
              StringAppendS("DBM,");
#endif
#ifdef HAVE_INFO
              StringAppendS("info,");
#endif
#ifdef TEST
              StringAppendS("TESTs,");
#endif
#if YYDEBUG
              StringAppendS("YYDEBUG=1,");
#endif
#ifdef MDEBUG
              StringAppend("MDEBUG=%d,",MDEBUG);
#endif
#ifdef PDEBUG
              StringAppend("PDEBUG,");
#endif
#ifdef KDEBUG
              StringAppend("KDEBUG,");
#endif
#ifdef TEST_MAC_ORDER
              StringAppendS("mac_order,");
#endif
#ifndef __OPTIMIZE__
              StringAppend("-g,");
#endif
              StringAppend("random=%d\n",siRandomStart);
#ifndef __MWERKS__
#ifdef HAVE_INFO
              StringAppend("InfoFile   : %s\n", feGetInfoFile());
              StringAppend("InfoProgram: %s\n", feGetInfoProgram());
#endif
              StringAppend("Singular   : %s\n",feGetExpandedExecutable());
              return StringAppend("SearchPath : %s", feGetSearchPath());
#endif
}
