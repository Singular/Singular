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

#include "mod2.h"
#include <mylimits.h>
#include "omalloc.h"
#include "structs.h"
#include "tok.h"
#include "febase.h"
#include "cntrlc.h"
#include "page.h"
#include "ipid.h"
#include "ipshell.h"
#include "kstd1.h"
#include "subexpr.h"
#include "timer.h"
#include "intvec.h"
#include "ring.h"
#include "omSingularConfig.h"
#include "p_Procs.h"
#include "version.h"

#include "static.h"
#ifdef HAVE_STATIC
#undef HAVE_DYN_RL
#endif

#define SI_DONT_HAVE_GLOBAL_VARS

//#ifdef HAVE_LIBPARSER
//#  include "libparse.h"
//#endif /* HAVE_LIBPARSER */

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

/* init bins from structs.h */

omBin MP_INT_bin = omGetSpecBin(sizeof(MP_INT));
omBin char_ptr_bin = omGetSpecBin(sizeof(char_ptr));
omBin ideal_bin = omGetSpecBin(sizeof(ideal));
omBin int_bin = omGetSpecBin(sizeof(int));
omBin poly_bin = omGetSpecBin(sizeof(poly));
omBin void_ptr_bin = omGetSpecBin(sizeof(void_ptr));
omBin indlist_bin = omGetSpecBin(sizeof(indlist));
omBin naIdeal_bin = omGetSpecBin(sizeof(naIdeal));
omBin snaIdeal_bin = omGetSpecBin(sizeof(snaIdeal));
omBin sm_prec_bin = omGetSpecBin(sizeof(sm_prec));
omBin smprec_bin = omGetSpecBin(sizeof(smprec));
omBin sip_sideal_bin = omGetSpecBin(sizeof(sip_sideal));
omBin sip_smap_bin = omGetSpecBin(sizeof(sip_smap));
omBin sip_sring_bin = omGetSpecBin(sizeof(sip_sring));
omBin ip_sideal_bin = omGetSpecBin(sizeof(ip_sideal));
omBin ip_smap_bin = omGetSpecBin(sizeof(ip_smap));
omBin ip_sring_bin = omGetSpecBin(sizeof(ip_sring));

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
  return t;
}

/*2
* the global exit routine of Singular
*/
extern "C" {
void m2_end(short i)
{
  fe_reset_input_mode();
  #ifdef PAGE_TEST
  mmEndStat();
  #endif
  #ifdef HAVE_TCL
  if (tclmode)
  {
    PrintTCL('Q',0,NULL);
  }
  #endif
  fe_reset_input_mode();
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

void singular_example(char *str)
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
  idhdl h, ns;
  iiname2hdl(s, &ns, &h);
#else /* HAVE_NAMESPACES */
  idhdl h=IDROOT->get(s,myynest);
#endif /* HAVE_NAMESPACES */
  if ((h!=NULL) && (IDTYP(h)==PROC_CMD))
  {
    char *lib=iiGetLibName(IDPROC(h));
    if((lib!=NULL)&&(*lib!='\0'))
    {
      Print("// proc %s from lib %s\n",s,lib);
      s=iiGetLibProcBuffer(IDPROC(h), 2);
      if (s!=NULL)
      {
        if (strlen(s)>5)
        {
          iiEStart(s,IDPROC(h));
          return;
        }
        else omFree((ADDRESS)s);
      }
    }
  }
  else
  {
    char sing_file[MAXPATHLEN];
    FILE *fd;
    sprintf(sing_file, "%s/%s.sing", feResource('m', 0), s);
    fd = feFopen(sing_file, "r");
    if (fd != NULL)
    {

      int old_echo = si_echo;
      int length, got;
      char* s;

      fseek(fd, 0, SEEK_END);
      length = ftell(fd);
      fseek(fd, 0, SEEK_SET);
      s = (char*) omAlloc((length+20)*sizeof(char));
      got = fread(s, sizeof(char), length, fd);
      fclose(fd);
      if (got != length)
      {
        Werror("Error while reading file %s", sing_file);
        omFree(s);
      }
      else
      {
        s[length] = '\0';
        strcat(s, "\n;return();\n\n");
        si_echo = 2;
        iiEStart(s, NULL);
        si_echo = old_echo;
      }
    }
    else
    {
      Werror("no example for %s", str);
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
  {"notBuckets",   Sy_bit(OPT_NOT_BUCKETS),    ~Sy_bit(OPT_NOT_BUCKETS)   },
  {"notSugar",     Sy_bit(OPT_NOT_SUGAR),      ~Sy_bit(OPT_NOT_SUGAR)   },
  {"interrupt",    Sy_bit(OPT_INTERRUPT),      ~Sy_bit(OPT_INTERRUPT)   },
  {"sugarCrit",    Sy_bit(OPT_SUGARCRIT),      ~Sy_bit(OPT_SUGARCRIT)   },
  {"teach",     Sy_bit(OPT_DEBUG),          ~Sy_bit(OPT_DEBUG)  },
  /* 9 return SB in syz, quotient, intersect */
  {"returnSB",     Sy_bit(OPT_RETURN_SB),      ~Sy_bit(OPT_RETURN_SB)  },
  {"fastHC",       Sy_bit(OPT_FASTHC),         ~Sy_bit(OPT_FASTHC)  },
  /* 11-19 sort in L/T */
  {"keepvars",     Sy_bit(OPT_KEEPVARS),       ~Sy_bit(OPT_KEEPVARS) },
  {"staircaseBound",Sy_bit(OPT_STAIRCASEBOUND),~Sy_bit(OPT_STAIRCASEBOUND)  },
  {"multBound",    Sy_bit(OPT_MULTBOUND),      ~Sy_bit(OPT_MULTBOUND)  },
  {"degBound",     Sy_bit(OPT_DEGBOUND),       ~Sy_bit(OPT_DEGBOUND)  },
  /* 25 no redTail(p)/redTail(s) */
  {"redTail",      Sy_bit(OPT_REDTAIL),        ~Sy_bit(OPT_REDTAIL)  },
  {"redThrough",   Sy_bit(OPT_REDTHROUGH),     ~Sy_bit(OPT_REDTHROUGH)  },
  {"oldStd",       Sy_bit(OPT_OLDSTD),         ~Sy_bit(OPT_OLDSTD)  },
  {"intStrategy",  Sy_bit(OPT_INTSTRATEGY),    ~Sy_bit(OPT_INTSTRATEGY)  },
  {"infRedTail",   Sy_bit(OPT_INFREDTAIL),     ~Sy_bit(OPT_INFREDTAIL)  },
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
  {"contentSB",Sy_bit(V_CONTENTSB), ~Sy_bit(V_CONTENTSB)        },
/*special for "none" and also end marker for showOption:*/
  {"ne",         0,          0 }
};

BOOLEAN setOption(leftv res, leftv v)
{
  char *n;
  do
  {
    if (v->Typ()==STRING_CMD)
    {
      n=(char *)v->CopyD(STRING_CMD);
    }
    else
    {
      if (v->name==NULL)
        return TRUE;
      if (v->rtyp==0)
      {
        n=v->name;
        v->name=NULL;
      }
      else
      {
        n=omStrDup(v->name);
      }
    }

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

        if (TEST_OPT_INTSTRATEGY && (currRing!=NULL)
        && rField_has_simple_inverse())
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
        {
          test |= optionStruct[i].setval;
          // optOldStd disables redthrough
          if (optionStruct[i].setval == Sy_bit(OPT_OLDSTD))
            test &= ~Sy_bit(OPT_REDTHROUGH);
        }
        else
          Warn("cannot set option");
        if (TEST_OPT_INTSTRATEGY && (currRing!=NULL)
        && rField_has_simple_inverse())
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
    if (currRing != NULL)
      currRing->options = test & TEST_RINGDEP_OPTS;
    omFree((ADDRESS)n);
    v=v->next;
  } while (v!=NULL);
  #ifdef HAVE_TCL
    if (tclmode)
    {
      BITSET tmp;
      int i;
      StringSetS("");
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
        }
        PrintTCLS('O',StringAppendS(""));
        StringSetS("");
      }
      else
      {
        PrintTCLS('O'," ");
      }
    }
  #endif
    // set global variable to show memory usage
    if (BVERBOSE(V_SHOW_MEM)) om_sing_opt_show_mem = 1;
    else om_sing_opt_show_mem = 0;
  return FALSE;
}

char * showOption()
{
  int i;
  BITSET tmp;

  StringSetS("//options:");
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
    return omStrDup(StringAppendS(""));
  }
  else
    return omStrDup(StringAppendS(" none"));
}

char * versionString()
{
  char* str = StringSetS("");
  StringAppend("Singular for %s version %s  (%lu)  %s\nwith\n",
               S_UNAME, S_VERSION1,
               feVersionId,singular_date);
  StringAppend("\t");
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
#if defined(HAVE_DYN_RL)
              if (fe_fgets_stdin==fe_fgets_dummy)
                StringAppendS("no input,");
              else if (fe_fgets_stdin==fe_fgets)
                StringAppendS("fgets,");
              if (fe_fgets_stdin==fe_fgets_stdin_drl)
                StringAppendS("dynamic readline,");
              else if (fe_fgets_stdin==fe_fgets_stdin_emu)
                StringAppendS("emulated readline,");
              else
                StringAppendS("unknown fgets method,");
#else
  #if defined(HAVE_READLINE) && !defined(FEREAD)
              StringAppendS("static readline,");
  #else
    #ifdef HAVE_FEREAD
              StringAppendS("emulated readline,");
    #else
              StringAppendS("fgets,");
    #endif
  #endif
#endif
#ifdef SRING
              StringAppendS("super algebra,");
#endif
#ifdef DRING
              StringAppendS("Weyl algebra,");
#endif
#ifdef HAVE_DBM
              StringAppendS("DBM,\n\t");
#else
              StringAppendS("\n\t");
#endif
#ifdef HAVE_NAMESPACES
              StringAppendS("Namespaces,");
#endif
#ifdef HAVE_NS
              StringAppendS("namespaces,");
#endif
#ifdef HAVE_DYNAMIC_LOADING
              StringAppendS("dynamic modules,");
#endif
              if (p_procs_dynamic) StringAppendS("dynamic p_Procs,");
#ifdef TEST
              StringAppendS("TESTs,");
#endif
#if YYDEBUG
              StringAppendS("YYDEBUG=1,");
#endif
#ifdef HAVE_ASSUME
             StringAppendS("ASSUME,");
#endif
#ifdef MDEBUG
              StringAppend("MDEBUG=%d,",MDEBUG);
#endif
#ifdef OM_CHECK
              StringAppend("OM_CHECK=%d,",OM_CHECK);
#endif
#ifdef OM_TRACK
              StringAppend("OM_TRACK=%d,",OM_TRACK);
#endif
#ifdef OM_NDEBUG
              StringAppend("OM_NDEBUG,");
#endif
#ifdef PDEBUG
              StringAppendS("PDEBUG,");
#endif
#ifdef KDEBUG
              StringAppendS("KDEBUG,");
#endif
#ifndef __OPTIMIZE__
#ifdef __MWERKS__
              StringAppendS(" Codewarrior 2.0,");
#else
              StringAppendS("-g,");
#endif
#endif
              StringAppend("random=%d\n",siRandomStart);

              feStringAppendResources(0);
              feStringAppendBrowsers(0);
              StringAppend("\n");
              return str;
}

#ifdef HAVE_NS
void listall(int showproc)
{
      idhdl hh=basePack->idroot;
      PrintS("====== Top ==============\n");
      while (hh!=NULL)
      {
        if (showproc || (IDTYP(hh)!=PROC_CMD))
        {
          if (IDDATA(hh)==(void *)currRing) PrintS("(R)");
          else if (IDDATA(hh)==(void *)currPack) PrintS("(P)");
          else PrintS("   ");
          Print("::%s, typ %s level %d data %x",
                 IDID(hh),Tok2Cmdname(IDTYP(hh)),IDLEV(hh),IDDATA(hh));
          if ((IDTYP(hh)==RING_CMD)
          || (IDTYP(hh)==QRING_CMD))
            Print(" ref: %d\n",IDRING(hh)->ref);
          else
            PrintLn();
        }  
        hh=IDNEXT(hh);
      }
      hh=basePack->idroot;
      while (hh!=NULL)
      {
        if (IDDATA(hh)==(void *)basePack)
          Print("(T)::%s, typ %s level %d data %x\n",
          IDID(hh),Tok2Cmdname(IDTYP(hh)),IDLEV(hh),IDDATA(hh));
        else
        if ((IDTYP(hh)==RING_CMD)
        || (IDTYP(hh)==QRING_CMD)
        || (IDTYP(hh)==PACKAGE_CMD))
        {
          Print("====== %s ==============\n",IDID(hh));
          idhdl h2=IDRING(hh)->idroot;
          while (h2!=NULL)
          {
            if (showproc || (IDTYP(h2)!=PROC_CMD))
            {
              if (IDDATA(h2)==(void *)currRing) PrintS("(R)");
              else if (IDDATA(h2)==(void *)currPack) PrintS("(P)");
              else PrintS("   ");
              Print("%s::%s, typ %s level %d data %x\n",
              IDID(hh),IDID(h2),Tok2Cmdname(IDTYP(h2)),IDLEV(h2),IDDATA(h2));
	    }  
            h2=IDNEXT(h2);
          }
        }
        hh=IDNEXT(hh);
      }
      Print("currRing:%x, currPack:%x,basePack:%x\n",currRing,currPack,basePack);
}
void checkall()
{
      idhdl hh=basePack->idroot;
      while (hh!=NULL)
      {
        omCheckAddr(hh);
        omCheckAddr(IDID(hh));
        if (RingDependend(IDTYP(hh))) Print("%s typ %d in Top\n",IDID(hh),IDTYP(hh));
        hh=IDNEXT(hh);
      }
      hh=basePack->idroot;
      while (hh!=NULL)
      {
        if (IDTYP(hh)==PACKAGE_CMD)
        {
          idhdl h2=IDPACKAGE(hh)->idroot;
          while (h2!=NULL)
          {
            omCheckAddr(h2);
            omCheckAddr(IDID(h2));
            if (RingDependend(IDTYP(h2))) Print("%s typ %d in %s\n",IDID(h2),IDTYP(h2),IDID(hh));
            h2=IDNEXT(h2);
          }
        }
        hh=IDNEXT(hh);
      }
}
#endif
