/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longrat0.cc,v 1.5 1997-06-04 19:45:21 obachman Exp $ */
/*
* ABSTRACT -
* IO for long rational numbers (Hubert Grassmann)
*/

#include <stdio.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "longrat.h"

#ifdef HAVE_GMP

#define SR_HDL(A) ((long)(A))
#define SR_INT    1
#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))
#define SR_TO_INT(SR)   (((long)SR) >> 2)


/*2
* extracts a long integer from s, returns the rest
*/
char * nlEatLong(char *s, MP_INT *i)
{
  char * start=s;

  while (*s >= '0' && *s <= '9') s++;
  if (*s=='\0')
  {
    mpz_set_str(i,start,10);
  }
  else
  {
    char c=*s;
    *s='\0';
    mpz_set_str(i,start,10);
    *s=c;
  }
  return s;
}

/*2
* extracts the number a from s, returns the rest
*/
char * nlRead (char *s, number *a)
{
  if (*s<'0' || *s>'9')
  {
    *a = nlInit(1);
    return s;
  }
  *a=(number)Alloc(sizeof(rnumber));
  {
    (*a)->s = 3;
#ifdef LDEBUG
    (*a)->debug=123456;
#endif
    MP_INT *z=&((*a)->z);
    MP_INT *n=&((*a)->n);
    mpz_init(z);
    s = nlEatLong(s, z);
    if (*s == '/')
    {
      mpz_init(n);
      (*a)->s = 0;
      s++;
      s = nlEatLong(s, n);
      if (mpz_cmp_si(n,(long)0)==0)
      {
        WerrorS("Zero Denominator");
        mpz_clear(n);
        (*a)->s = 3;
      }
      else if (mpz_cmp_si(n,(long)1)==0)
      {
        mpz_clear(n);
        (*a)->s=3;
      }
    }
    if (mpz_cmp_si(z,(long)0)==0)
    {
      *a=INT_TO_SR(0);
    }
    else
    if ((*a)->s==3)
    {
      int ui=(int)mpz_get_si(&(*a)->z);
      if ((((ui<<3)>>3)==ui)
      && (mpz_cmp_si(&(*a)->z,(long)ui)==0))
      {
        mpz_clear(&(*a)->z);
        Free((ADDRESS)(*a),sizeof(rnumber));
        (*a)=INT_TO_SR(ui);
      }
    }
    else
      nlNormalize(*a);
  }
  return s;
}

/*2
* write a rational number
*/
void nlWrite (number &a)
{
  char *s,*z;
  if (SR_HDL(a) & SR_INT)
  {
    StringAppend("%d",SR_TO_INT(a));
  }
#ifdef TEST
  else if (a==NULL)
  {
    StringAppend("o");
  }
#endif
  else
  {
    if (a->s==0)
    {
      nlNormalize(a);
      nlWrite(a);
      return;
    }
    int l=mpz_sizeinbase(&a->z,10);
    if (a->s<2) l=max(l,mpz_sizeinbase(&a->n,10));
    l+=2;
    s=(char*)Alloc(l);
    z=mpz_get_str(s,10,&a->z);
    StringAppend(z);
    if (a->s!=3)
    {
      StringAppend("/");
      z=mpz_get_str(s,10,&a->n);
      StringAppend(z);
    }
    Free((ADDRESS)s,l);
  }
}

#else
#define NLLENGTH(a) ((int)(*(a)&0X7FFF))
#define NUM_BASIS 10000
#define NLISRAT(a)   ((a)->s)

/*3
* extracts a long integer from s, returns the rest
*/
static char * nlEatLong(char *s, number *i)
{
  number j, k, ten, n;
  j = nlInit(0);
  ten = nlInit(10);
  while (*s >= '0' && *s <= '9')
  {
    k = nlMult(j, ten);
    nlDelete(&j);
    j = nlInit((int)*s-(int)'0');
    s++;
    n = nlAdd(j, k);
    nlDelete(&j);
    nlDelete(&k);
    j = n;
  }
  nlDelete(&ten);
  *i = j;
  return s;
}

/*2
* extracts the numberio a from s, returns the rest
*/
char * nlRead (char *s, number *a)
{
  number z, n;
  BOOLEAN neg = (*s == '-');

  if (*s == '+' || *s == '-')
    s++;
  if (*s<'0' || *s>'9')
  {
    if (neg)
      *a = nlInit(-1);
    else
      *a = nlInit(1);
    return s;
  }
  s = nlEatLong(s, &z);
  if (neg)
    nlNeg(z);
  *a = z;
  if (*s == '/')
  {
    s++;
    s = nlEatLong(s, &n);
    if (nlIsZero(n))
    {
      WerrorS("Zero Denominator");
      nlDelete(&n);
      nlDelete(a);
      return s;
    }
    if (nlIsOne(n))
      nlDelete(&n);
    else
    {
      *a = nlDiv(z, n);
      nlNormalize(*a);
    }
  }
  return s;
}

/*3
* write long integer, assume n!=NULL
*/
static void nlWriteLong(lint n)
{
  char save;
  char *lonstr, *str1;
  int i, k, j;
  lint a;
  int16 al, w;
  al = NLLENGTH(n);
  k = al+1;
  i = k*sizeof(int16);
  a = (lint)Alloc(i);
  memcpy(a, n, i);
  j = k*5*sizeof(char);
  lonstr = (char *)Alloc(j);
  str1 = lonstr+(k*5-1);
  *str1 = '\0';
  do
  {
    nlDivMod(a, al, NUM_BASIS, &w);
    save = *str1;
    str1 -= 4;
    sprintf(str1, "%04u", w);
    str1[4] = save;
    if (a[al]==0)
      al--;
  } while (al!=0);
  Free((ADDRESS)a,i);
  while (*str1 == '0') str1++;
  StringAppend(str1);
  Free((ADDRESS)lonstr, j);
}

/*2
* write a number
*/
void nlWrite (number &a)
{
  if (nlIsZero(a))
  {
    StringAppend("0");
    return;
  }
  if (!nlGreaterZero(a))
  {
    StringAppend("-");
  }
  if (a->s<0)
    nlNormalize(a);
  nlWriteLong(a->z);
  if (NLISRAT(a))
  {
    StringAppend("/");
    nlWriteLong(a->n);
  }
}
#endif

