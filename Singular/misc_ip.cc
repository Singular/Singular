/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file misc_ip.cc
 *
 * This file provides miscellaneous functionality.
 *
 * For more general information, see the documentation in
 * misc_ip.h.
 *
 * @author Frank Seelisch
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

// include header files
#include <kernel/mod2.h>
#include <Singular/lists.h>
#include <kernel/longrat.h>
#include <Singular/misc_ip.h>

void number2mpz(number n, mpz_t m)
{
  if (SR_HDL(n) & SR_INT) mpz_init_set_si(m, SR_TO_INT(n));     /* n fits in an int */
  else             mpz_init_set(m, (mpz_ptr)n->z);
}

number mpz2number(mpz_t m)
{
  number z = (number)omAllocBin(rnumber_bin);
  mpz_init_set(z->z, m);
  mpz_init_set_ui(z->n, 1);
  z->s = 3;
  return z;
}

void divTimes(mpz_t n, mpz_t d, int* times)
{
  *times = 0;
  mpz_t r; mpz_init(r);
  mpz_t q; mpz_init(q);
  mpz_fdiv_qr(q, r, n, d);
  while (mpz_cmp_ui(r, 0) == 0)
  {
    (*times)++;
    mpz_set(n, q);
    mpz_fdiv_qr(q, r, n, d);
  }
  mpz_clear(r);
  mpz_clear(q);
}

void divTimes_ui(mpz_t n, long d, int* times)
{
  *times = 0;
  mpz_t r; mpz_init(r);
  mpz_t q; mpz_init(q);
  mpz_fdiv_qr_ui(q, r, n, d);
  while (mpz_cmp_ui(r, 0) == 0)
  {
    (*times)++;
    mpz_set(n, q);
    mpz_fdiv_qr_ui(q, r, n, d);
  }
  mpz_clear(r);
  mpz_clear(q);
}

/* returns an object of type lists which contains the entries
   theInts[0..(length-1)] as INT_CMDs */
lists makeListsObject(const int* theInts, int length)
{
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(length);
  for (int i = 0; i < length; i++)
    { L->m[i].rtyp = INT_CMD; L->m[i].data = (void*)theInts[i]; }
  return L;
}

void setListEntry(lists L, int index, mpz_t n)
{ /* assumes n > 0 */
  /* try to fit nn into an int: */
  if (mpz_size1(n)<=1)
  {
    int ui=(int)mpz_get_si(n);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(n,(long)ui)==0))
    {
      L->m[index].rtyp = INT_CMD; L->m[index].data = (void*)ui;
      return;
    }
  }
  number nn = mpz2number(n);
  L->m[index].rtyp = BIGINT_CMD; L->m[index].data = (void*)nn;
}

void setListEntry_ui(lists L, int index, long ui)
{ /* assumes n > 0 */
  /* try to fit nn into an int: */
  if (((ui<<3)>>3)==ui)
  {
    L->m[index].rtyp = INT_CMD; L->m[index].data = (void*)ui;
  }
  else
  {
    number nn = nlInit(ui,NULL);
    L->m[index].rtyp = BIGINT_CMD; L->m[index].data = (void*)nn;
  }
}

/* true iff p is prime */
/*
bool isPrime(mpz_t p)
{
  if (mpz_cmp_ui(p, 2) == 0) return true;
  if (mpz_cmp_ui(p, 3) == 0) return true;
  if (mpz_cmp_ui(p, 5) < 0)  return false;

  mpz_t d; mpz_init_set_ui(d, 5); int add = 2;
  mpz_t sr; mpz_init(sr); mpz_sqrt(sr, p);
  mpz_t r; mpz_init(r);
  while (mpz_cmp(d, sr) <= 0)
  {
    mpz_cdiv_r(r, p, d);
    if (mpz_cmp_ui(r, 0) == 0)
    {
      mpz_clear(d); mpz_clear(sr); mpz_clear(r);
      return false;
    }
    mpz_add_ui(d, d, add);
    add += 2; if (add == 6) add = 2;
  }
  mpz_clear(d); mpz_clear(sr); mpz_clear(r);
  return true;
}
*/

/* finds the next prime q, bound >= q >= p;
   in case of success, puts q into p;
   otherwise sets q = bound + 1;
   e.g. p = 24; nextPrime(p, 30) produces p = 29 (success),
        p = 24; nextPrime(p, 29) produces p = 29 (success),
        p = 24; nextPrime(p, 28) produces p = 29 (no success),
        p = 24; nextPrime(p, 27) produces p = 28 (no success) */
/*
void nextPrime(mpz_t p, mpz_t bound)
{
  int add;
  mpz_t r; mpz_init(r); mpz_cdiv_r_ui(r, p, 6); // r = p mod 6, 0 <= r <= 5
  if (mpz_cmp_ui(r, 0) == 0) { mpz_add_ui(p, p, 1); add = 4; }
  if (mpz_cmp_ui(r, 1) == 0) {                      add = 4; }
  if (mpz_cmp_ui(r, 2) == 0) { mpz_add_ui(p, p, 3); add = 2; }
  if (mpz_cmp_ui(r, 3) == 0) { mpz_add_ui(p, p, 2); add = 2; }
  if (mpz_cmp_ui(r, 4) == 0) { mpz_add_ui(p, p, 1); add = 2; }
  if (mpz_cmp_ui(r, 5) == 0) {                      add = 2; }

  while (mpz_cmp(p, bound) <= 0)
  {
    if (isPrime(p)) { mpz_clear(r); return; }
    mpz_add_ui(p, p, add);
    add += 2; if (add == 6) add = 2;
  }
  mpz_set(p, bound);
  mpz_add_ui(p, p, 1);
  mpz_clear(r);
  return;
}
*/

/* n and pBound are assumed to be bigint numbers */
lists primeFactorisation(const number n, const number pBound)
{
  mpz_t nn; number2mpz(n, nn);
  mpz_t pb; number2mpz(pBound, pb);
  mpz_t b; number2mpz(pBound, b);
  mpz_t p; mpz_init(p); int tt;
  mpz_t sr; mpz_init(sr); int index = 0; int add;
  lists primes = (lists)omAllocBin(slists_bin); primes->Init(1000);
  int* multiplicities = new int[1000];

  divTimes_ui(nn, 2, &tt);
  if (tt > 0)
  {
    setListEntry_ui(primes, index, 2);
    multiplicities[index++] = tt;
  }

  divTimes_ui(nn, 3, &tt);
  if (tt > 0)
  {
    setListEntry_ui(primes, index, 3);
    multiplicities[index++] = tt;
  }

  unsigned long p_ui=5; add = 2;
  mpz_sqrt(sr, nn);
  if ((mpz_cmp_ui(b, 0) == 0) || (mpz_cmp(pb, sr) > 0)) mpz_set(pb, sr);
  int limit=mpz_get_ui(pb);
  if ((limit==0)||(mpz_cmp_ui(pb,limit)!=0)) limit=1<<31;
  while (p_ui <=limit)
  {
    divTimes_ui(nn, p_ui, &tt);
    if (tt > 0)
    {
      setListEntry_ui(primes, index, p_ui);
      multiplicities[index++] = tt;
      //mpz_sqrt(sr, nn);
      //if ((mpz_cmp_ui(b, 0) == 0) || (mpz_cmp(pb, sr) > 0)) mpz_set(pb, sr);
      if (mpz_size1(nn)<=2) 
      {
        mpz_sqrt(sr, nn);
        if ((mpz_cmp_ui(b, 0) == 0) || (mpz_cmp(pb, sr) > 0)) mpz_set(pb, sr);
        unsigned long l=mpz_get_ui(sr);
        if (l<limit) limit=l;
      }
    }
    p_ui +=add;
    add += 2; if (add == 6) add = 2;
  }
  mpz_set_ui(p, p_ui);
  mpz_sqrt(sr, nn);
  if ((mpz_cmp_ui(b, 0) == 0) || (mpz_cmp(pb, sr) > 0)) mpz_set(pb, sr);
  while (mpz_cmp(pb, p) >= 0)
  {
    divTimes(nn, p, &tt);
    if (tt > 0)
    {
      setListEntry(primes, index, p);
      multiplicities[index++] = tt;
      mpz_sqrt(sr, nn);
      if ((mpz_cmp_ui(b, 0) == 0) || (mpz_cmp(pb, sr) > 0)) mpz_set(pb, sr);
    }
    mpz_add_ui(p, p, add);
    add += 2; if (add == 6) add = 2;
  }
  if ((mpz_cmp_ui(nn, 1) > 0) &&
      ((mpz_cmp_ui(b, 0) == 0) || (mpz_cmp(nn, b) <= 0)))
  {
    setListEntry(primes, index, nn);
    multiplicities[index++] = 1;
    mpz_set_ui(nn, 1);
  }

  lists primesL = (lists)omAllocBin(slists_bin);
  primesL->Init(index);
  for (int i = 0; i < index; i++)
  {
    primesL->m[i].rtyp = primes->m[i].rtyp;
    primesL->m[i].data = primes->m[i].data;
  }
  omFreeSize((ADDRESS)primes->m, (primes->nr + 1) * sizeof(sleftv));
  omFreeBin((ADDRESS)primes, slists_bin);

  lists multiplicitiesL = (lists)omAllocBin(slists_bin);
  multiplicitiesL->Init(index);
  for (int i = 0; i < index; i++)
  {
    multiplicitiesL->m[i].rtyp = INT_CMD;
    multiplicitiesL->m[i].data = (void*)multiplicities[i];
  }
  delete[] multiplicities;

  lists L=(lists)omAllocBin(slists_bin);
  L->Init(4);
  setListEntry(L, 0, nn);
  L->m[1].rtyp = LIST_CMD; L->m[1].data = (void*)primesL;
  L->m[2].rtyp = LIST_CMD; L->m[2].data = (void*)multiplicitiesL;
  int probTest = 0;
  if (mpz_probab_prime_p(nn, 25) != 0) probTest = 1;
  L->m[3].rtyp =  INT_CMD; L->m[3].data = (void*)probTest;
  mpz_clear(nn); mpz_clear(pb); mpz_clear(b); mpz_clear(p); mpz_clear(sr);

  return L;
}

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <omalloc/mylimits.h>
#include <omalloc/omalloc.h>
#include <kernel/options.h>
#include <kernel/febase.h>
#include <Singular/cntrlc.h>
#include <kernel/page.h>
#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <kernel/kstd1.h>
#include <Singular/subexpr.h>
#include <kernel/timer.h>
#include <kernel/intvec.h>
#include <kernel/ring.h>
#include <Singular/omSingularConfig.h>
#include <kernel/p_Procs.h>
/* Needed for debug Version of p_SetRingOfLeftv, Oliver */
#ifdef PDEBUG
#include <kernel/polys.h>
#endif
#include <Singular/version.h>

#include <Singular/static.h>
#ifdef HAVE_STATIC
#undef HAVE_DYN_RL
#endif

#define SI_DONT_HAVE_GLOBAL_VARS

//#ifdef HAVE_LIBPARSER
//#  include "libparse.h"
//#endif /* HAVE_LIBPARSER */

#ifdef HAVE_FACTORY
#include <factory/factory.h>
// libfac:
  extern const char * libfac_version;
  extern const char * libfac_date;
#endif

/* version strings */
#include <kernel/si_gmp.h>
#ifdef HAVE_MPSR
#include <MP_Config.h>
#endif

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
* the renice routine for very large jobs
* works only on unix machines,
* testet on : linux, HP 9.0
*
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
*/

void singular_example(char *str)
{
  assume(str!=NULL);
  char *s=str;
  while (*s==' ') s++;
  char *ss=s;
  while (*ss!='\0') ss++;
  while (*ss<=' ')
  {
    *ss='\0';
    ss--;
  }
  idhdl h=IDROOT->get(s,myynest);
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
    FILE *fd=NULL;
    char *res_m=feResource('m', 0);
    if (res_m!=NULL)
    {
      sprintf(sing_file, "%s/%s.sing", res_m, s);
      fd = feFopen(sing_file, "r");
    }
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
  const char * name;
  unsigned   setval;
  unsigned   resetval;
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
  {"staircaseBound",Sy_bit(OPT_STAIRCASEBOUND),~Sy_bit(OPT_STAIRCASEBOUND)  },
  {"multBound",    Sy_bit(OPT_MULTBOUND),      ~Sy_bit(OPT_MULTBOUND)  },
  {"degBound",     Sy_bit(OPT_DEGBOUND),       ~Sy_bit(OPT_DEGBOUND)  },
  /* 25 no redTail(p)/redTail(s) */
  {"redTail",      Sy_bit(OPT_REDTAIL),        ~Sy_bit(OPT_REDTAIL)  },
  {"redThrough",   Sy_bit(OPT_REDTHROUGH),     ~Sy_bit(OPT_REDTHROUGH)  },
  {"lazy",         Sy_bit(OPT_OLDSTD),         ~Sy_bit(OPT_OLDSTD)  },
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
  {"usage",    Sy_bit(V_SHOW_USE),  ~Sy_bit(V_SHOW_USE)   },
  {"Imap",     Sy_bit(V_IMAP),      ~Sy_bit(V_IMAP)       },
  {"prompt",   Sy_bit(V_PROMPT),    ~Sy_bit(V_PROMPT)     },
  {"length",   Sy_bit(V_LENGTH),    ~Sy_bit(V_LENGTH)     },
  {"notWarnSB",Sy_bit(V_NSB),       ~Sy_bit(V_NSB)        },
  {"contentSB",Sy_bit(V_CONTENTSB), ~Sy_bit(V_CONTENTSB)  },
  {"cancelunit",Sy_bit(V_CANCELUNIT),~Sy_bit(V_CANCELUNIT)},
  {"modpsolve",Sy_bit(V_MODPSOLVSB),~Sy_bit(V_MODPSOLVSB)},
  {"geometricSB",Sy_bit(V_UPTORADICAL),~Sy_bit(V_UPTORADICAL)},
  {"findMonomials",Sy_bit(V_FINDMONOM),~Sy_bit(V_FINDMONOM)},
  {"coefStrat",Sy_bit(V_COEFSTRAT), ~Sy_bit(V_COEFSTRAT)},
  {"qringNF",  Sy_bit(V_QRING),     ~Sy_bit(V_QRING)},
  {"warn",     Sy_bit(V_ALLWARN),   ~Sy_bit(V_ALLWARN)},
/*special for "none" and also end marker for showOption:*/
  {"ne",         0,          0 }
};

BOOLEAN setOption(leftv res, leftv v)
{
  const char *n;
  do
  {
    if (v->Typ()==STRING_CMD)
    {
      n=(const char *)v->CopyD(STRING_CMD);
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
#if 0
        if (TEST_OPT_INTSTRATEGY && (currRing!=NULL)
        && rField_has_simple_inverse()
#ifdef HAVE_RINGS
        && !rField_is_Ring(currRing)
#endif
        ) {
          test &=~Sy_bit(OPT_INTSTRATEGY);
        }
#endif
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
#if 0
        if (TEST_OPT_INTSTRATEGY && (currRing!=NULL)
        && rField_has_simple_inverse()
#ifdef HAVE_RINGS
        && !rField_is_Ring(currRing)
#endif
        ) {
          test &=~Sy_bit(OPT_INTSTRATEGY);
        }
#endif
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
        /*debugging the bison grammar --> grammar.cc*/
        extern int    yydebug;
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
        /*debugging the bison grammar --> grammar.cc*/
        extern int    yydebug;
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
  StringAppend("Singular for %s version %s (%d-%s)  %s\nwith\n",
               S_UNAME, S_VERSION1, SINGULAR_VERSION,
               feVersionId,singular_date);
  StringAppendS("\t");
#ifdef HAVE_FACTORY
              StringAppend("factory(%s),", factoryVersion);
              StringAppend("libfac(%s,%s),\n\t",libfac_version,libfac_date);
#endif
#if defined (__GNU_MP_VERSION) && defined (__GNU_MP_VERSION_MINOR)
              StringAppend("GMP(%d.%d),",__GNU_MP_VERSION,__GNU_MP_VERSION_MINOR);
#else
              StringAppendS("GMP(1.3),");
#endif
#ifdef HAVE_NTL
#include <NTL/version.h>
              StringAppend("NTL(%s),",NTL_VERSION);
#endif
#ifdef HAVE_MPSR
              StringAppend("MP(%s),",MP_VERSION);
#endif
#if SIZEOF_VOIDP == 8
              StringAppendS("64bit,");
#else
              StringAppendS("32bit,");
#endif
#if defined(HAVE_DYN_RL)
              if (fe_fgets_stdin==fe_fgets_dummy)
                StringAppendS("no input,");
              else if (fe_fgets_stdin==fe_fgets)
                StringAppendS("fgets,");
              if (fe_fgets_stdin==fe_fgets_stdin_drl)
                StringAppendS("dynamic readline,");
              #ifdef HAVE_FEREAD
              else if (fe_fgets_stdin==fe_fgets_stdin_emu)
                StringAppendS("emulated readline,");
              #endif
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
#ifdef HAVE_PLURAL
              StringAppendS("Plural,");
#endif
#ifdef HAVE_DBM
              StringAppendS("DBM,\n\t");
#else
              StringAppendS("\n\t");
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
              StringAppendS("OM_NDEBUG,");
#endif
#ifdef PDEBUG
              StringAppendS("PDEBUG,");
#endif
#ifdef KDEBUG
              StringAppendS("KDEBUG,");
#endif
#ifndef __OPTIMIZE__
              StringAppendS("-g,");
#endif
#ifdef HAVE_EIGENVAL
              StringAppendS("eigenvalues,");
#endif
#ifdef HAVE_GMS
              StringAppendS("Gauss-Manin system,");
#endif
#ifdef HAVE_RATGRING
              StringAppendS("ratGB,");
#endif
              StringAppend("random=%d\n",siRandomStart);
              StringAppend("\tCC=%s,\n\tCXX=%s"
#ifdef __GNUC__
              "(" __VERSION__ ")"
#endif
              "\n",CC,CXX);
              feStringAppendResources(0);
              feStringAppendBrowsers(0);
              StringAppendS("\n");
              return str;
}

#ifdef PDEBUG
#if (OM_TRACK > 2) && defined(OM_TRACK_CUSTOM)
void p_SetRingOfLeftv(leftv l, ring r)
{
  switch(l->rtyp)
  {
    case INT_CMD:
    case BIGINT_CMD:
    case IDHDL:
    case DEF_CMD:
      break;
    case POLY_CMD:
    case VECTOR_CMD:
    {
      poly p=(poly)l->data;
      while(p!=NULL) { p_SetRingOfLm(p,r); pIter(p); }
      break;
    }
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
    {
      ideal I=(ideal)l->data;
      int i;
      for(i=IDELEMS(I)-1;i>=0;i--)
      {
        poly p=I->m[i];
        while(p!=NULL) { p_SetRingOfLm(p,r); pIter(p); }
      }
      break;
    }
    case COMMAND:
    {
      command d=(command)l->data;
      p_SetRingOfLeftv(&d->arg1, r);
      if (d->argc>1) p_SetRingOfLeftv(&d->arg2, r);
      if (d->argc>2) p_SetRingOfLeftv(&d->arg3, r);
      break;
    }
    default:
     printf("type %d not yet implementd in p_SetRingOfLeftv\n",l->rtyp);
     break;
  }
}
#endif
#endif

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
          Print("::%s, typ %s level %d data %lx",
                 IDID(hh),Tok2Cmdname(IDTYP(hh)),IDLEV(hh),(long)IDDATA(hh));
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
          Print("(T)::%s, typ %s level %d data %lx\n",
          IDID(hh),Tok2Cmdname(IDTYP(hh)),IDLEV(hh),(long)IDDATA(hh));
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
              if ((IDDATA(h2)==(void *)currRing)
              && ((IDTYP(h2)==RING_CMD)||(IDTYP(h2)==QRING_CMD)))
                PrintS("(R)");
              else if (IDDATA(h2)==(void *)currPack) PrintS("(P)");
              else PrintS("   ");
              Print("%s::%s, typ %s level %d data %lx\n",
              IDID(hh),IDID(h2),Tok2Cmdname(IDTYP(h2)),IDLEV(h2),(long)IDDATA(h2));
            }
            h2=IDNEXT(h2);
          }
        }
        hh=IDNEXT(hh);
      }
      Print("currRing:%lx, currPack:%lx,basePack:%lx\n",(long)currRing,(long)currPack,(long)basePack);
      iiCheckPack(currPack);
}
#ifndef NDEBUG
void checkall()
{
      idhdl hh=basePack->idroot;
      while (hh!=NULL)
      {
        omCheckAddr(hh);
        omCheckAddr((ADDRESS)IDID(hh));
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
            omCheckAddr((ADDRESS)IDID(h2));
            if (RingDependend(IDTYP(h2))) Print("%s typ %d in %s\n",IDID(h2),IDTYP(h2),IDID(hh));
            h2=IDNEXT(h2);
          }
        }
        hh=IDNEXT(hh);
      }
}
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern "C"
int singular_fstat(int fd, struct stat *buf)
{
  return fstat(fd,buf);
}

/*2
* the global exit routine of Singular
*/
#ifdef HAVE_MPSR
void (*MP_Exit_Env_Ptr)()=NULL;
#endif

extern "C" {

void m2_end(int i)
{
  fe_reset_input_mode();
  #ifdef PAGE_TEST
  mmEndStat();
  #endif
  fe_reset_input_mode();
  idhdl h = IDROOT;
  while(h != NULL)
  {
    if(IDTYP(h) == LINK_CMD)
    {
      idhdl hh=h->next;
      killhdl(h, currPack);
      h = hh;
    }
    else
    {
      h = h->next;
    }
  }
  if (i<=0)
  {
      if (TEST_V_QUIET)
      {
        if (i==0)
          printf("Auf Wiedersehen.\n");
        else
          printf("\n$Bye.\n");
      }
    //#ifdef sun
    //  #ifndef __svr4__
    //    _cleanup();
    //    _exit(0);
    //  #endif
    //#endif
    exit(0);
  }
  else
  {
      printf("\nhalt %d\n",i);
  }
  #ifdef HAVE_MPSR
  if (MP_Exit_Env_Ptr!=NULL) (*MP_Exit_Env_Ptr)();
  #endif
  exit(i);
}
}
