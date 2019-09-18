/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT -
* IO for long rational numbers (Hubert Grassmann)
*/

#include <stdio.h>
#include <string.h>

#include "misc/auxiliary.h"
#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
#include "coeffs/longrat.h"

THREAD_VAR omBin rnumber_bin = omGetSpecBin(sizeof(snumber)); // TODO: move this into coeffs-struct (for Q)?!


#define SR_HDL(A) ((long)(A))
//#define SR_INT    1 // already in longrat.h
//#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)


/*2
* extracts the number a from s, returns the rest
*/
const char * nlRead (const char *s, number *a, const coeffs r)
{
  if (*s<'0' || *s>'9')
  {
    *a = INT_TO_SR(1); /* nlInit(1) */
    return s;
  }
  *a=(number)ALLOC_RNUMBER();
  {
    (*a)->s = 3;
#if defined(LDEBUG)
    (*a)->debug=123456;
#endif
    mpz_ptr z=(*a)->z;
    mpz_ptr n=(*a)->n;
    mpz_init(z);
    s = nEatLong((char *)s, z);
    if (*s == '/')
    {
      mpz_init(n);
      (*a)->s = 0;
      s++;
      s = nEatLong((char *)s, n);
      if (mpz_cmp_si(n,0L)==0)
      {
        WerrorS(nDivBy0);
        mpz_clear(n);
        (*a)->s = 3;
      }
      else if (mpz_cmp_si(n,1L)==0)
      {
        mpz_clear(n);
        (*a)->s=3;
      }
    }
    if (mpz_cmp_si(z,0L)==0)
    {
      mpz_clear(z);
      FREE_RNUMBER(*a);
      *a=INT_TO_SR(0);
    }
    else if ((*a)->s==3)
    {
      number nlShort3_noinline(number x);
      *a=nlShort3_noinline(*a);
    }
    else
    {
      number aa=*a;
      nlNormalize(aa,r); // FIXME? TODO? // extern void     nlNormalize(number &x, const coeffs r);
      *a=aa;
    }
  }
  return s;
}

/*2
* write a rational number
*/
void nlWrite (number a, const coeffs)
{
  char *s,*z;
  if (SR_HDL(a) & SR_INT)
  {
    StringAppend("%ld",SR_TO_INT(a));
  }
  else if (a==NULL)
  {
    StringAppendS("o");
  }
  else
  {
    int l=mpz_sizeinbase(a->z,10);
    if (a->s<2) l=si_max(l,(int)mpz_sizeinbase(a->n,10));
    l+=2;
    s=(char*)omAlloc(l);
    z=mpz_get_str(s,10,a->z);
    StringAppendS(z);
    if (a->s!=3)
    {
      StringAppendS("/");
      z=mpz_get_str(s,10,a->n);
      StringAppendS(z);
    }
    omFreeSize((void *)s,l);
  }
}

#if 0
void nlDebugWrite (number a)
{
  char *s,*z;
  if (SR_HDL(a) & SR_INT)
  {
    Print("%ld",SR_TO_INT(a));
  }
  else if (a==NULL)
  {
    PrintS("o");
  }
  else
  {
    int l=mpz_sizeinbase(a->z,10);
    if (a->s<2) l=si_max(l,(int)mpz_sizeinbase(a->n,10));
    l+=2;
    s=(char*)omAlloc(l);
    z=mpz_get_str(s,10,a->z);
    PrintS(z);
    if (a->s!=3)
    {
      PrintS("/");
      z=mpz_get_str(s,10,a->n);
      PrintS(z);
    }
    omFreeSize((void *)s,l);
  }
}
#endif
