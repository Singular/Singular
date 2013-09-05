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
 **/
/*****************************************************************************/

// include header files
#include <kernel/mod2.h>
#include <Singular/lists.h>
#include <kernel/longrat.h>
#include <Singular/misc_ip.h>
#include <Singular/feOpt.h>
#include <Singular/silink.h>
#include <Singular/si_signals.h>

void number2mpz(number n, mpz_t m)
{
  if (SR_HDL(n) & SR_INT) mpz_init_set_si(m, SR_TO_INT(n));     /* n fits in an int */
  else             mpz_init_set(m, (mpz_ptr)n->z);
}

number mpz2number(mpz_t m)
{
  number z = ALLOC_RNUMBER();
  mpz_init_set(z->z, m);
  mpz_init_set_ui(z->n, 1);
  z->s = 3;
  return z;
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
      L->m[index].rtyp = INT_CMD; L->m[index].data = (void*)(long)ui;
      return;
    }
  }
  number nn = mpz2number(n);
  L->m[index].rtyp = BIGINT_CMD; L->m[index].data = (void*)nn;
}

void setListEntry_ui(lists L, int index, unsigned long ui)
{ /* assumes n > 0 */
  /* try to fit nn into an int: */
  int i=(int)ui;
  if ((((unsigned long)i)==ui) && (((i<<3)>>3)==i))
  {
    L->m[index].rtyp = INT_CMD; L->m[index].data = (void*)(long)i;
  }
  else
  {
    number nn = nlRInit(ui);
    mpz_set_ui(nn->z,ui);
    L->m[index].rtyp = BIGINT_CMD; L->m[index].data = (void*)nn;
  }
}

/* Factoring with Pollard's rho method. stolen from GMP/demos */
static unsigned add[] = {4, 2, 4, 2, 4, 6, 2, 6};

static int factor_using_division (mpz_t t, unsigned int limit,lists primes, int *multiplicities,int &index, unsigned long bound)
{
  mpz_t q, r;
  unsigned long int f;
  int ai;
  unsigned *addv = add;
  unsigned int failures;
  int bound_not_reached=1;

  mpz_init (q);
  mpz_init (r);

  f = mpz_scan1 (t, 0);
  mpz_div_2exp (t, t, f);
  if (f>0)
  {
    setListEntry_ui(primes, index, 2);
    multiplicities[index++] = f;
  }

  f=0;
  loop
  {
    mpz_tdiv_qr_ui (q, r, t, 3);
    if (mpz_cmp_ui (r, 0) != 0)
        break;
    mpz_set (t, q);
    f++;
  }
  if (f>0)
  {
    setListEntry_ui(primes, index, 3);
    multiplicities[index++] = f;
  }

  f=0;
  loop
  {
    mpz_tdiv_qr_ui (q, r, t, 5);
    if (mpz_cmp_ui (r, 0) != 0)
        break;
    mpz_set (t, q);
    f++;
  }
  if (f>0)
  {
    setListEntry_ui(primes, index, 5);
    multiplicities[index++] = f;
  }

  failures = 0;
  f = 7;
  ai = 0;
  unsigned long last_f=0;
  while (mpz_cmp_ui (t, 1) != 0)
  {
    mpz_tdiv_qr_ui (q, r, t, f);
    if (mpz_cmp_ui (r, 0) != 0)
    {
      f += addv[ai];
      if (mpz_cmp_ui (t, f) < 0)
        break;
      ai = (ai + 1) & 7;
      failures++;
      if (failures > limit)
        break;
      if ((bound!=0) && (f>bound))
      {
        bound_not_reached=0;
        break;
      }
    }
    else
    {
      mpz_swap (t, q);
      if (f!=last_f)
      {
        setListEntry_ui(primes, index, f);
        multiplicities[index]++;
        index++;
      }
      else
      {
        multiplicities[index-1]++;
      }
      last_f=f;
      failures = 0;
    }
  }

  mpz_clear (q);
  mpz_clear (r);
  //printf("bound=%d,f=%d,failures=%d, reached=%d\n",bound,f,failures,bound_not_reached);
  return bound_not_reached;
}

static void factor_using_pollard_rho (mpz_t n, unsigned long a, lists primes, int * multiplicities,int &index)
{
  mpz_t x, x1, y, P;
  mpz_t t1, t2;
  mpz_t last_f;
  unsigned long long k, l, i;

  mpz_init (t1);
  mpz_init (t2);
  mpz_init_set_si (last_f, 0);
  mpz_init_set_si (y, 2);
  mpz_init_set_si (x, 2);
  mpz_init_set_si (x1, 2);
  mpz_init_set_ui (P, 1);
  k = 1;
  l = 1;

  while (mpz_cmp_ui (n, 1) != 0)
  {
    loop
    {
      do
      {
        mpz_mul (t1, x, x);
        mpz_mod (x, t1, n);
        mpz_add_ui (x, x, a);
        mpz_sub (t1, x1, x);
        mpz_mul (t2, P, t1);
        mpz_mod (P, t2, n);

        if (k % 32 == 1)
        {
          mpz_gcd (t1, P, n);
          if (mpz_cmp_ui (t1, 1) != 0)
            goto factor_found;
          mpz_set (y, x);
        }
      }
      while (--k != 0);

      mpz_gcd (t1, P, n);
      if (mpz_cmp_ui (t1, 1) != 0)
        goto factor_found;

      mpz_set (x1, x);
      k = l;
      l = 2 * l;
      for (i = 0; i < k; i++)
      {
        mpz_mul (t1, x, x);
        mpz_mod (x, t1, n);
        mpz_add_ui (x, x, a);
      }
      mpz_set (y, x);
    }

  factor_found:
    do
    {
      mpz_mul (t1, y, y);
      mpz_mod (y, t1, n);
      mpz_add_ui (y, y, a);
      mpz_sub (t1, x1, y);
      mpz_gcd (t1, t1, n);
    }
    while (mpz_cmp_ui (t1, 1) == 0);

    mpz_divexact (n, n, t1);        /* divide by t1, before t1 is overwritten */

    if (!mpz_probab_prime_p (t1, 10))
    {
      do
      {
        mp_limb_t a_limb;
        mpn_random (&a_limb, (mp_size_t) 1);
        a = a_limb;
      }
      while (a == 0);

      factor_using_pollard_rho (t1, a, primes,multiplicities,index);
    }
    else
    {
      if (mpz_cmp(t1,last_f)==0)
      {
        multiplicities[index-1]++;
      }
      else
      {
        mpz_set(last_f,t1);
        setListEntry(primes, index, t1);
        multiplicities[index++] = 1;
      }
    }
    mpz_mod (x, x, n);
    mpz_mod (x1, x1, n);
    mpz_mod (y, y, n);
    if (mpz_probab_prime_p (n, 10))
    {
      if (mpz_cmp(n,last_f)==0)
      {
        multiplicities[index-1]++;
      }
      else
      {
        mpz_set(last_f,n);
        setListEntry(primes, index, n);
        multiplicities[index++] = 1;
      }
      mpz_set_ui(n,1);
      break;
    }
  }

  mpz_clear (P);
  mpz_clear (t2);
  mpz_clear (t1);
  mpz_clear (x1);
  mpz_clear (x);
  mpz_clear (y);
  mpz_clear (last_f);
}

static void factor_gmp (mpz_t t,lists primes,int *multiplicities,int &index,unsigned long bound)
{
  unsigned int division_limit;

  if (mpz_sgn (t) == 0)
    return;

  /* Set the trial division limit according the size of t.  */
  division_limit = mpz_sizeinbase (t, 2);
  if (division_limit > 1000)
    division_limit = 1000 * 1000;
  else
    division_limit = division_limit * division_limit;

  if (factor_using_division (t, division_limit,primes,multiplicities,index,bound))
  {
    if (mpz_cmp_ui (t, 1) != 0)
    {
      if (mpz_probab_prime_p (t, 10))
      {
        setListEntry(primes, index, t);
        multiplicities[index++] = 1;
        mpz_set_ui(t,1);
      }
      else
        factor_using_pollard_rho (t, 1L, primes,multiplicities,index);
    }
  }
}
/* n and pBound are assumed to be bigint numbers */
lists primeFactorisation(const number n, const int pBound)
{
  int i;
  int index=0;
  mpz_t nn; number2mpz(n, nn);
  lists primes = (lists)omAllocBin(slists_bin); primes->Init(1000);
  int* multiplicities = (int*)omAlloc0(1000*sizeof(int));
  int positive=1;

  if (!nlIsZero(n))
  {
    if (!nlGreaterZero(n))
    {
      positive=-1;
      mpz_neg(nn,nn);
    }
    factor_gmp(nn,primes,multiplicities,index,pBound);
  }

  lists primesL = (lists)omAllocBin(slists_bin);
  primesL->Init(index);
  for (i = 0; i < index; i++)
  {
    primesL->m[i].rtyp = primes->m[i].rtyp;
    primesL->m[i].data = primes->m[i].data;
    primes->m[i].rtyp=0;
    primes->m[i].data=NULL;
  }
  primes->Clean(NULL);

  lists multiplicitiesL = (lists)omAllocBin(slists_bin);
  multiplicitiesL->Init(index);
  for (i = 0; i < index; i++)
  {
    multiplicitiesL->m[i].rtyp = INT_CMD;
    multiplicitiesL->m[i].data = (void*)(long)multiplicities[i];
  }
  omFree(multiplicities);

  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  if (positive==-1) mpz_neg(nn,nn);
  L->m[0].rtyp = LIST_CMD; L->m[0].data = (void*)primesL;
  L->m[1].rtyp = LIST_CMD; L->m[1].data = (void*)multiplicitiesL;
  setListEntry(L, 2, nn);

  mpz_clear(nn);

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
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory/factory.h>
// libfac:
  extern const char * libfac_version;
  extern const char * libfac_date;
/* version strings */
#ifdef HAVE_FLINT
#ifdef __cplusplus
extern "C"
{
#endif
#ifndef __GMP_BITS_PER_MP_LIMB
#define __GMP_BITS_PER_MP_LIMB GMP_LIMB_BITS
#endif
#include <flint/flint.h>
#ifdef __cplusplus
}
#endif
#endif
#endif

/* version strings */
#include <kernel/si_gmp.h>
#ifdef HAVE_MPSR
#include <MP_Config.h>
#endif

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
          omFree((ADDRESS)s);
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
      }
      else
      {
        s[length] = '\0';
        strcat(s, "\n;return();\n\n");
        si_echo = 2;
        iiEStart(s, NULL);
        si_echo = old_echo;
      }
      omFree(s);
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
  {"teach",        Sy_bit(OPT_DEBUG),          ~Sy_bit(OPT_DEBUG)  },
  {"notSyzMinim",  Sy_bit(OPT_NO_SYZ_MINIM),   ~Sy_bit(OPT_NO_SYZ_MINIM)  },
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
  {"interedSyz",Sy_bit(V_INTERSECT_SYZ), ~Sy_bit(V_INTERSECT_SYZ)},
  {"interedElim",Sy_bit(V_INTERSECT_ELIM), ~Sy_bit(V_INTERSECT_ELIM)},
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
  extern int om_sing_opt_show_mem;
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
        if (optionStruct[i].setval & tmp)
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
    return StringEndS();
  }
  else
  {
    StringAppendS(" none");
    return StringEndS();
  }
}

char * versionString()
{
  StringSetS("");
  StringAppend("Singular for %s version %s (%d)  %s\nwith\n",
               S_UNAME, S_VERSION1, SINGULAR_VERSION,
               singular_date);
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
#ifdef HAVE_FACTORY
#ifdef HAVE_FLINT
              StringAppend("FLINT(%s),",version);
#endif
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
#ifdef HAVE_FANS
              StringAppendS("fan/cone,");
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
              return StringEndS();
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

#if 0 /* debug only */
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
#endif

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
  return si_fstat(fd,buf);
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
  if (ssiToBeClosed_inactive)
  {
    link_list hh=ssiToBeClosed;
    while(hh!=NULL)
    {
      //Print("close %s\n",hh->l->name);
      slPrepClose(hh->l);
      hh=(link_list)hh->next;
    }
    ssiToBeClosed_inactive=FALSE;

    idhdl h = currPack->idroot;
    while(h != NULL)
    {
      if(IDTYP(h) == LINK_CMD)
      {
        idhdl hh=h->next;
        //Print("kill %s\n",IDID(h));
        killhdl(h, currPack);
        h = hh;
      }
      else
      {
        h = h->next;
      }
    }
    hh=ssiToBeClosed;
    while(hh!=NULL)
    {
      //Print("close %s\n",hh->l->name);
      slClose(hh->l);
      hh=ssiToBeClosed;
    }
  }
  if (!singular_in_batchmode)
  {
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
      i=0;
    }
    else
    {
      printf("\nhalt %d\n",i);
    }
  }
  #ifdef HAVE_MPSR
  if (MP_Exit_Env_Ptr!=NULL) (*MP_Exit_Env_Ptr)();
  #endif
  exit(i);
}
}

const char *singular_date=__DATE__ " " __TIME__;

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

/*2
* initialize components of Singular
*/
void siInit(char *name)
{
#ifdef HAVE_FACTORY
// factory default settings: -----------------------------------------------
  On(SW_USE_NTL);
  On(SW_USE_NTL_GCD_0); // On -> seg11 in Old/algnorm, Old/factor...
  On(SW_USE_NTL_GCD_P); // On -> cyle in Short/brnoeth_s: fixed
  On(SW_USE_EZGCD);
  On(SW_USE_CHINREM_GCD);
  //On(SW_USE_FF_MOD_GCD);
  On(SW_USE_EZGCD_P);
  On(SW_USE_QGCD);
  Off(SW_USE_NTL_SORT); // may be changed by an command line option
  factoryError=WerrorS;
#endif

// memory initialization: -----------------------------------------------
    om_Opts.OutOfMemoryFunc = omSingOutOfMemoryFunc;
#ifndef OM_NDEBUG
#ifndef __OPTIMIZE__
    om_Opts.ErrorHook = dErrorBreak;
#else
    om_Opts.Keep = 0; /* !OM_NDEBUG, __OPTIMIZE__*/
#endif
#else
    om_Opts.Keep = 0; /* OM_NDEBUG */
#endif
    omInitInfo();

// interpreter tables etc.: -----------------------------------------------
#ifdef INIT_BUG
  jjInitTab1();
#endif
  memset(&sLastPrinted,0,sizeof(sleftv));
  sLastPrinted.rtyp=NONE;
  extern int iiInitArithmetic();
  iiInitArithmetic();
  basePack=(package)omAlloc0(sizeof(*basePack));
  currPack=basePack;
  idhdl h;
  h=enterid("Top", 0, PACKAGE_CMD, &IDROOT, TRUE);
  IDPACKAGE(h)->language = LANG_TOP;
  IDPACKAGE(h)=basePack;
  currPackHdl=h;
  basePackHdl=h;

// random generator: -----------------------------------------------
  int t=initTimer();
  if (t==0) t=1;
  initRTimer();
  siSeed=t;
#ifdef HAVE_FACTORY
  factoryseed(t);
#endif
  siRandomStart=t;
  feOptSpec[FE_OPT_RANDOM].value = (void*) ((long)siRandomStart);

// resource table: ----------------------------------------------------
  // Don't worry: ifdef OM_NDEBUG, then all these calls are undef'ed
  // hack such that all shared' libs in the bindir are loaded correctly
  feInitResources(name);

// singular links: --------------------------------------------------
  slStandardInit();
  myynest=0;
// semapohore 0 -----------------------------------------------------
  int cpus=2;
  int cpu_n;
  #ifdef _SC_NPROCESSORS_ONLN
  if ((cpu_n=sysconf(_SC_NPROCESSORS_ONLN))>cpus) cpus=cpu_n;
  #elif defined(_SC_NPROCESSORS_CONF)
  if ((cpu_n=sysconf(_SC_NPROCESSORS_CONF))>cpus) cpus=cpu_n;
  #endif
  feSetOptValue(FE_OPT_CPUS, cpus);

// loading standard.lib -----------------------------------------------
  if (! feOptValue(FE_OPT_NO_STDLIB))
  {
    int vv=verbose;
    verbose &= ~Sy_bit(V_LOAD_LIB);
    iiLibCmd(omStrDup("standard.lib"), TRUE,TRUE,TRUE);
    verbose=vv;
  }
  errorreported = 0;
}

#ifdef LIBSINGULAR
#ifdef HAVE_FACTORY
// the init routines of factory need mmInit
int mmInit( void )
{
  return 1;
}
#endif
#endif
