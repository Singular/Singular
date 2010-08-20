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
#include <Singular/mod2.h>
#include <Singular/lists.h>
#include <kernel/longrat.h>
#include <Singular/misc_ip.h>

/* This works by Newton iteration, i.e.,
      a(1)   = n;
      a(i+1) = a(i)/2 + n/2/a(i), i > 0.
   This sequence is guaranteed to decrease monotonously and
   it is known to converge fast.
   All used numbers are bigints. */
number approximateSqrt(const number n)
{
  if (nlIsZero(n)) { number zero = nlInit(0, NULL); return zero; }
  number temp1; number temp2;
  number one = nlInit(1, NULL);
  number two = nlInit(2, NULL);
  number m = nlCopy(n);
  number mOld = nlSub(m, one); /* initially required to be different from m */
  number nHalf = nlIntDiv(n, two);
  bool check = true;
  while (!nlEqual(m, mOld) && check)
  {
    temp1 = nlIntDiv(m, two);
    temp2 = nlIntDiv(nHalf, m);
    mOld = m;
    m = nlAdd(temp1, temp2);
    nlDelete(&temp1, NULL); nlDelete(&temp2, NULL);
    temp1 = nlMult(m, m);
    check = nlGreater(temp1, n);
    nlDelete(&temp1, NULL);
  }
  nlDelete(&mOld, NULL); nlDelete(&two, NULL); nlDelete(&nHalf, NULL);
  while (!check)
  {
    temp1 = nlAdd(m, one);
    nlDelete(&m, NULL);
    m = temp1;
    temp1 = nlMult(m, m);
    check = nlGreater(temp1, n);
    nlDelete(&temp1, NULL);
  }
  temp1 = nlSub(m, one);
  nlDelete(&m, NULL);
  nlDelete(&one, NULL);
  m = temp1;
  return m;
}

/* returns the quotient resulting from division of n by the prime as many
   times as possible without remainder; afterwards, the parameter times
   will contain the highest exponent e of p such that p^e divides n
   e.g., divTimes(48, 4, t) = 3 with t = 2, since 48 = 4*4*3;
   n is expected to be a bigint; returned type is also bigint */
number divTimes(const number n, const int p, int* times)
{
  number nn = nlCopy(n);
  number dd = nlInit(p, NULL);
  number rr = nlIntMod(nn, dd);
  *times = 0;
  while (nlIsZero(rr))
  {
    (*times)++;
    number temp = nlIntDiv(nn, dd);
    nlDelete(&nn, NULL);
    nn = temp;
    nlDelete(&rr, NULL);
    rr = nlIntMod(nn, dd);
  }
  nlDelete(&rr, NULL); nlDelete(&dd, NULL);
  return nn;
}

/* returns an object of type lists which contains the entries
   theInts[0..(length-1)] as INT_CMDs*/
lists makeListsObject(const int* theInts, int length)
{
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(length);
  for (int i = 0; i < length; i++)
    { L->m[i].rtyp = INT_CMD; L->m[i].data = (void*)theInts[i]; }
  return L;
}

/* returns the i-th bit of the binary number which arises by
   concatenating array[length-1], ..., array[1], array[0],
   where array[0] contains the 32 lowest bits etc.;
   i is assumed to be small enough to address a valid index
   in the given array */
bool getValue(const unsigned i, const unsigned int* ii)
{
  if (i==2) return true;
  if ((i & 1)==0) return false;
  unsigned I= i/2;
  unsigned index = I / 32;
  unsigned offset = I % 32;
  unsigned int v = 1 << offset;
  return ((ii[index] & v) != 0);
}

/* sets the i-th bit of the binary number which arises by
   concatenating array[length-1], ..., array[1], array[0],
   where array[0] contains the 32 lowest bits etc.;
   i is assumed to be small enough to address a valid index
   in the given array */
void setValue(const unsigned i, bool value, unsigned int* ii)
{
  if ((i&1)==0) return; // ignore odd numbers
  unsigned I=i/2;
  unsigned index = I / 32;
  unsigned offset = I % 32;
  unsigned int v = 1 << offset;
  if (value) { ii[index] |= v;  }
  else       { ii[index] &= ~v; }
}

/* returns whether i is less than or equal to the bigint number n */
bool isLeq(const int i, const number n)
{
  number iN = nlInit(i - 1, NULL);
  bool result = nlGreater(n, iN);
  nlDelete(&iN, NULL);
  return result;
}

#if 0
lists primeFactorisation(const number n, const int pBound)
{
  number nn = nlCopy(n); int i;
  int pCounter = 0; /* for counting the number of mutually distinct
                       prime factors in n */
  /* we assume that there are at most 1000 mutually distinct prime
     factors in n */
  int* primes = new int[1000]; int* multiplicities = new int[1000];

  /* extra treatment for the primes 2 and 3;
     all other primes are equal to +1/-1 mod 6 */
  int e; number temp;
  temp = divTimes(nn, 2, &e); nlDelete(&nn, NULL); nn = temp;
  if (e > 0) { primes[pCounter] = 2; multiplicities[pCounter++] = e; }
  temp = divTimes(nn, 3, &e); nlDelete(&nn, NULL); nn = temp;
  if (e > 0) { primes[pCounter] = 3; multiplicities[pCounter++] = e; }

  /* now we simultaneously:
     - build the sieve of Erathostenes up to s,
     - divide out each prime factor of nn that we find along the way
       (This may result in an earlier termination.) */

  int s = 1<<25;       /* = 2^25 */
  int maxP = 2147483647; /* = 2^31 - 1, by the way a Mersenne prime */
  if ((pBound != 0) && (pBound < maxP))
  {
    maxP = pBound;
  }
  if (maxP< (2147483647-63) ) s=(maxP+63)/64;
  else                        s=2147483647/64+1;
  unsigned int* isPrime = new unsigned int[s];
  /* the lowest bit of isPrime[0] stores whether 1 is a prime,
     next bit is for 3, next for 5, etc. i.e.
     intended usage is: isPrime[0] = ...
     We shall make use only of bits which correspond to numbers =
     -1 or +1 mod 6. */
  //for (i = 0; i < s; i++) isPrime[i] = ~0;/*4294967295*/; /* all 32 bits set */
  memset(isPrime,0xff,s*sizeof(unsigned int));
  int p=9; while((p<maxP) && (p>0)) { setValue(p,false,isPrime); p+=6; }
  p = 5; bool add2 = true;
  /* due to possible overflows, we need to check whether p > 0, and
     likewise i > 0 below */
  while ((0 < p) && (p <= maxP) && (isLeq(p, nn)))
  {
    /* at this point, p is guaranteed to be a prime;
       we divide nn by the highest power of p and store p
       if nn is at all divisible by p */
    temp = divTimes(nn, p, &e);
    nlDelete(&nn, NULL); nn = temp;
    if (e > 0)
    { primes[pCounter] = p; multiplicities[pCounter++] = e; }
    /* invalidate all multiples of p, starting with 2*p */
    i = 2 * p;
    while ((0 < i) && (i < maxP)) { setValue(i, false, isPrime); i += p; }
    /* move on to the next prime in the sieve; we either add 2 or 4
       in order to visit just the numbers equal to -1/+1 mod 6 */
    if (add2) { p += 2; add2 = false; }
    else      { p += 4; add2 = true;  }
    while ((0 < p) && (p <= maxP) && (isLeq(p, nn)) && (!getValue(p, isPrime)))
    {
      if (add2) { p += 2; add2 = false; }
      else      { p += 4; add2 = true;  }
    }
  }

  /* build return structure and clean up */
  delete [] isPrime;
  lists primesL = makeListsObject(primes, pCounter);
  lists multiplicitiesL = makeListsObject(multiplicities, pCounter);
  delete [] primes; delete [] multiplicities;
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp = BIGINT_CMD; L->m[0].data = (void *)nn;
  /* try to fit nn into an int: */
  int nnAsInt = nlInt(nn, NULL);
  if (nlIsZero(nn) || (nnAsInt != 0))
  {
    nlDelete(&nn,NULL):
    L->m[0].rtyp = INT_CMD;
    L->m[0].data = (void *)nnAsInt;
  }
  L->m[1].rtyp = LIST_CMD; L->m[1].data = (void *)primesL;
  L->m[2].rtyp = LIST_CMD; L->m[2].data = (void *)multiplicitiesL;
  return L;
}
#else
/* Factoring , from gmp-demos

Copyright 1995, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2005 Free Software
Foundation, Inc.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/.  */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <kernel/si_gmp.h>

static unsigned add[] = {4, 2, 4, 2, 4, 6, 2, 6};

void factor_using_division (mpz_t t, unsigned int limit, int *L, int &L_ind, int *ex)
{
  mpz_t q, r;
  unsigned long int f;
  int ai;
  unsigned *addv = add;
  unsigned int failures;

  mpz_init (q);
  mpz_init (r);

  f = mpz_scan1 (t, 0);
  mpz_div_2exp (t, t, f);
  while (f)
  {
    if ((L_ind>0) && (L[L_ind-1]==2)) ex[L_ind-1]++;
    else
    {
      L[L_ind]=2;
      L_ind++;
    }
    --f;
  }

  for (;;)
  {
    mpz_tdiv_qr_ui (q, r, t, 3);
    if (mpz_cmp_ui (r, 0) != 0)  break;
    mpz_set (t, q);
    if ((L_ind>0) && (L[L_ind-1]==3)) ex[L_ind-1]++;
    else
    {
      L[L_ind]=3;
      L_ind++;
    }
  }

  for (;;)
  {
    mpz_tdiv_qr_ui (q, r, t, 5);
    if (mpz_cmp_ui (r, 0) != 0) break;
    mpz_set (t, q);
    if ((L_ind>0) && (L[L_ind-1]==5)) ex[L_ind-1]++;
    else
    {
      L[L_ind]=5;
      L_ind++;
    }
  }

  failures = 0;
  f = 7;
  ai = 0;
  while (mpz_cmp_ui (t, 1) != 0)
  {
    if (f>=((unsigned long)1 <<28)) break;
    if (mpz_cmp_ui (t, f) < 0) break;
    mpz_tdiv_qr_ui (q, r, t, f);
    if (mpz_cmp_ui (r, 0) != 0)
    {
        f += addv[ai];
        ai = (ai + 1) & 7;
        failures++;
        if (failures > limit) break;
    }
    else
    {
      mpz_swap (t, q);
      //gmp_printf("%d: %Zd\n",f,t);
      // here: f in 0,,2^28-1:
      if ((L_ind>0) && (L[L_ind-1]==(int)f)) ex[L_ind-1]++;
      else
      {
        L[L_ind]=f;
        L_ind++;
      }
      failures = 0;
    }
  }

  mpz_clear (q);
  mpz_clear (r);
}

void factor_using_pollard_rho (mpz_t n, int a_int, int *L, int &L_ind, int *ex)
{
  mpz_t x, x1, y, P;
  mpz_t a;
  mpz_t g;
  mpz_t t1, t2;
  int k, l, c, i;

  mpz_init (g);
  mpz_init (t1);
  mpz_init (t2);

  mpz_init_set_si (a, a_int);
  mpz_init_set_si (y, 2);
  mpz_init_set_si (x, 2);
  mpz_init_set_si (x1, 2);
  k = 1;
  l = 1;
  mpz_init_set_ui (P, 1);
  c = 0;

  while (mpz_cmp_ui (n, 1) != 0)
  {
S2:
    mpz_mul (x, x, x); mpz_add (x, x, a); mpz_mod (x, x, n);
    mpz_sub (t1, x1, x); mpz_mul (t2, P, t1); mpz_mod (P, t2, n);
    c++;
    if (c == 20)
    {
      c = 0;
      mpz_gcd (g, P, n);
      if (mpz_cmp_ui (g, 1) != 0) goto S4;
      mpz_set (y, x);
    }
S3:
    k--;
    if (k > 0) goto S2;

    mpz_gcd (g, P, n);
    if (mpz_cmp_ui (g, 1) != 0) goto S4;

    mpz_set (x1, x);
    k = l;
    l = 2 * l;
    for (i = 0; i < k; i++)
    {
      mpz_mul (x, x, x); mpz_add (x, x, a); mpz_mod (x, x, n);
    }
    mpz_set (y, x);
    c = 0;
    goto S2;
S4:
    do
    {
      mpz_mul (y, y, y); mpz_add (y, y, a); mpz_mod (y, y, n);
      mpz_sub (t1, x1, y); mpz_gcd (g, t1, n);
    }
    while (mpz_cmp_ui (g, 1) == 0);

    mpz_div (n, n, g);        /* divide by g, before g is overwritten */

    if (!mpz_probab_prime_p (g, 10))
    {
      do
      {
        mp_limb_t a_limb;
        mpn_random (&a_limb, (mp_size_t) 1);
        a_int = (int) a_limb;
      }
      while (a_int == -2 || a_int == 0);

      factor_using_pollard_rho (g, a_int,L,L_ind,ex);
    }
    else
    {
      if ((L_ind>0) && (mpz_cmp_si(g,L[L_ind-1])==0)) ex[L_ind-1]++;
      else
      {
        L[L_ind]=mpz_get_si(g);
        L_ind++;
      }
    }
    mpz_mod (x, x, n);
    mpz_mod (x1, x1, n);
    mpz_mod (y, y, n);
    if (mpz_probab_prime_p (n, 10))
    {
      int te=mpz_get_si(n);
      if (mpz_cmp_si(n,te)==0) /* does it fit into an int ? */
      {
        if ((L_ind>0) && (mpz_cmp_si(n,L[L_ind-1])==0)) ex[L_ind-1]++;
        else
        {
          L[L_ind]=mpz_get_si(n);
          L_ind++;
        }
        mpz_set_si(n,1); // add n itself the list of divisors, rest is 1
      }
      break;
    }
  }

  mpz_clear (g);
  mpz_clear (P);
  mpz_clear (t2);
  mpz_clear (t1);
  mpz_clear (a);
  mpz_clear (x1);
  mpz_clear (x);
  mpz_clear (y);
}

void mpz_factor (mpz_t t, int *L, int & L_ind, int *ex)
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

  factor_using_division (t, division_limit, L, L_ind, ex);

  if (mpz_cmp_ui (t, 1) != 0)
  {
    if (mpz_probab_prime_p (t, 10))
    {
      int tt=mpz_get_si(t);
      // check if t fits into int:
      if ((mpz_size1(t)==1)&&(mpz_cmp_si(t,tt)==0))
      {
        L[L_ind]=mpz_get_si(t);
        L_ind++;
        mpz_set_si(t,1);
      }
    }
    else
      factor_using_pollard_rho (t, 1, L,L_ind,ex);
  }
}

lists primeFactorisation(const number n, const int pBound)
{
  mpz_t t;
  number nn = nlCopy(n);
  lists L=(lists)omAllocBin(slists_bin);
  L->Init(3);
  L->m[0].rtyp = BIGINT_CMD; L->m[0].data = (void *)nn;
  /* try to fit nn into an int: */
  int nnAsInt = nlInt(nn, NULL);
  if (nlIsZero(nn) || (nnAsInt != 0))
  {
    mpz_init_set_si(t,nnAsInt);
  }
  else
  {
    mpz_init_set(t,(mpz_ptr)nn->z);
  }
  int *LL=(int*)omAlloc0(1000*sizeof(int));
  int *ex=(int*)omAlloc0(1000*sizeof(int));
  int L_ind=0;
  mpz_factor (t,LL,L_ind,ex);

  nnAsInt = mpz_get_si(t);
  if ((mpz_size1(t)==1) && (mpz_cmp_si(t,nnAsInt)==0))
  {
    nlDelete(&nn,NULL);
    L->m[0].rtyp = INT_CMD;
    L->m[0].data = (void *)nnAsInt;
  }
  else
  {
    mpz_set(nn->z,t);
    L->m[0].rtyp = BIGINT_CMD;
    L->m[0].data = (void *)nn;
  }
  mpz_clear(t);
  int i;
  for(i=0;i<L_ind;i++) ex[i]++;
  L->m[1].rtyp = LIST_CMD; L->m[1].data = (void *)makeListsObject(LL,L_ind);
  L->m[2].rtyp = LIST_CMD; L->m[2].data = (void *)makeListsObject(ex,L_ind);
  return L;
}
#endif

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include <mylimits.h>
#include <omalloc.h>
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
#include <factory.h>
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
