/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longrat.cc,v 1.5 1997-04-25 15:04:02 obachman Exp $ */
/*
* ABSTRACT: computation with long rational numbers (Hubert Grassmann)
*/

#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "modulop.h"
#include "longrat.h"

#ifdef HAVE_GMP
#if __GNU_MP_VERSION >= 2 && __GNU_MP_VERSION_MINOR >= 0
#  define HAVE_LIBGMP2
#else
#  define HAVE_LIBGMP1
#endif 

#endif /* HAVE_GMP */

#ifndef BYTES_PER_MP_LIMB
#ifdef HAVE_LIBGMP2
#define BYTES_PER_MP_LIMB sizeof(mp_limb_t)
#else
#define BYTES_PER_MP_LIMB sizeof(mp_limb)
#endif
#endif

static number nlMapP(number from)
{
  number to;
  to = nlInit(npInt(from));
  return to;
}

BOOLEAN nlSetMap(int c, char ** par, int nop, number minpol)
{
  if (c == 0)
  {
    nMap = nlCopy;   /*Q -> Q*/
    return TRUE;
  }
  if (c>1)
  {
    if (par==NULL)
    {
      npPrimeM=c;
      nMap = nlMapP; /* Z/p -> Q */
      return TRUE;
    }
    else
     return FALSE;  /* GF(q) ->Q */
  }
  else if (c<0)
     return FALSE; /* Z/p(a) -> Q*/
  else if (c==1)
     return FALSE; /* Q(a) -> Q */
  return FALSE;
}

/*-----------------------------------------------------------------*/
#ifdef HAVE_GMP
/*3
* parameter s in number:
* 0 (or FALSE): not normalised rational
* 1 (or TRUE):  normalised rational
* 3          :  integer with n==NULL
*/
/*3
**  'SR_INT' is the type of those integers small enough to fit into  29  bits.
**  Therefor the value range of this small integers is: $-2^{28}...2^{28}-1$.
**
**  Small integers are represented by an immediate integer handle, containing
**  the value instead of pointing  to  it,  which  has  the  following  form:
**
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**      | guard | sign  | bit   | bit   |       | bit   | tag   | tag   |
**      | bit   | bit   | 27    | 26    |       | 0     | 0     | 1     |
**      +-------+-------+-------+-------+- - - -+-------+-------+-------+
**
**  Immediate integers handles carry the tag 'SR_INT', i.e. the last bit is 1.
**  This distuingishes immediate integers from other handles which  point  to
**  structures aligned on 4 byte boundaries and therefor have last bit  zero.
**  (The second bit is reserved as tag to allow extensions of  this  scheme.)
**  Using immediates as pointers and dereferencing them gives address errors.
**
**  To aid overflow check the most significant two bits must always be equal,
**  that is to say that the sign bit of immediate integers has a  guard  bit.
**
**  The macros 'INT_TO_SR' and 'SR_TO_INT' should be used to convert  between
**  a small integer value and its representation as immediate integer handle.
**
**  Large integers and rationals are represented by z and n
**  where n may be undefined (if s==3)
**  NULL represents only deleted values
*/
#define SR_HDL(A) ((long)(A))
/*#define SR_INT    1*/
/*#define INT_TO_SR(INT)  ((number) (((long)INT << 2) + SR_INT))*/
#define SR_TO_INT(SR)   (((long)SR) >> 2)

#ifdef macintosh
#define MP_SMALL 2
#else
#define MP_SMALL 1
#endif
//#define mpz_isNeg(A) (mpz_cmp_si(A,(long)0)<0)
#ifdef HAVE_LIBGMP1
#define mpz_isNeg(A) ((A)->size<0)
#define mpz_limb_size(A) ((A)->size)
#define mpz_limb_d(A) ((A)->d)
#define MPZ_DIV(A,B,C) mpz_div((A),(B),(C))
#define MPZ_EXACTDIV(A,B,C) mpz_div((A),(B),(C))
#else
#define mpz_isNeg(A) ((A)->_mp_size<0)
#define mpz_limb_size(A) ((A)->_mp_size)
#define mpz_limb_d(A) ((A)->_mp_d)
#define MPZ_DIV(A,B,C) mpz_tdiv_q((A),(B),(C))
#define MPZ_EXACTDIV(A,B,C) mpz_divexact((A),(B),(C))
#define nlGmpSimple(A)
#endif


#ifdef LDEBUG
#define nlTest(a) nlDBTest(a,__FILE__,__LINE__)
BOOLEAN nlDBTest(number a, char *f,int l)
{
  if (a==NULL)
  {
    Print("!!longrat: NULL in %s:%d\n",f,l);
    return FALSE;
  }
  //if ((int)a==1) Print("!! 0x1 as number ? %s %d\n",f,l);
  if ((((int)a)&3)==3)
  {
    Print(" !!longrat:ptr(3) in %s:%d\n",f,l);
    return FALSE;
  }
  if ((((int)a)&3)==1)
  {
    if (((((int)a)<<1)>>1)!=((int)a))
    {
      Print(" !!longrat:arith:%x in %s:%d\n",(int)a, f,l);
      return FALSE;
    }
    return TRUE;
  }
#ifdef MDEBUG
  mmDBTestBlock(a,sizeof(*a),f,l);
#else
  if (a->debug!=123456)
  {
    Print("!!longrat:debug:%d in %s:%d\n",a->debug,f,l);
    a->debug=123456;
    return FALSE;
  }
#endif
  if ((a->s<0)||(a->s>4))
  {
    Print(" !!longrat:s=%d in %s:%d\n",a->s,f,l);
    return FALSE;
  }
#ifdef MDEBUG
#ifdef HAVE_LIBGMP2
  mmDBTestBlock(a->z._mp_d,a->z._mp_alloc*BYTES_PER_MP_LIMB,f,l);
#else
  mmDBTestBlock(a->z.d,a->z.alloc*BYTES_PER_MP_LIMB,f,l);
#endif
#endif
  if (a->s<2)
  {
#ifdef MDEBUG
#ifdef HAVE_LIBGMP2
    mmDBTestBlock(a->n._mp_d,a->n._mp_alloc*BYTES_PER_MP_LIMB,f,-l);
#else
    mmDBTestBlock(a->n.d,a->n.alloc*BYTES_PER_MP_LIMB,f,-l);
#endif
#endif
    if (mpz_cmp_si(&a->n,(long)1)==0)
    {
      Print("!!longrat:integer as rational in %s:%d\n",f,l);
      mpz_clear(&a->n);
      a->s=3;
      return FALSE;
    }
    else if (mpz_isNeg(&a->n))
    {
      Print("!!longrat:div. by negative in %s:%d\n",f,l);
      mpz_neg(&a->z,&a->z);
      mpz_neg(&a->n,&a->n);
      return FALSE;
    }
    return TRUE;
  }
  if (a->s==2)
  {
    Print("!!longrat:s=2 in %s:%d\n",f,l);
    return FALSE;
  }
  if (mpz_size1(&a->z)>MP_SMALL) return TRUE;
  int ui=(int)mpz_get_si(&a->z);
  if ((((ui<<3)>>3)==ui)
  && (mpz_cmp_si(&a->z,(long)ui)==0))
  {
    Print("!!longrat:im int %d in %s:%d\n",ui,f,l);
    f=NULL;
    return FALSE;
  }
  return TRUE;
}
#endif

void nlNew (number * r)
{
  //*r=INT_TO_SR(0);
  *r=NULL;
}

/*2
* z := i
*/
#ifdef MDEBUG
#define nlRInit(A) nlRDBInit(A,__LINE__)
static inline number nlRDBInit(int i, int n)
#else
static inline number nlRInit (int i)
#endif
{
#ifdef MDEBUG
  number z=(number)mmDBAllocBlock(sizeof(rnumber),"longrat.cc",n);
#else
  number z=(number)Alloc(sizeof(rnumber));
#endif
  mpz_init_set_si(&z->z,(long)i);
  z->s = 3;
  return z;
}

number nlInit (int i)
{
  number n;
  if ( ((i << 3) >> 3) == i ) n=INT_TO_SR(i);
  else                        n=nlRInit(i);
#ifdef LDEBUG
  nlTest(n);
#endif
  return n;
}

int nlSize(number a)
{
  if ((int)a==1)
     return 0; /* rational 0*/
  if ((((int)a)&3)==1)
     return 1; /* immidiate int */
#ifdef HAVE_LIBGMP2
  int s=a->z._mp_alloc;
#else
  int s=a->z.alloc;
#endif
  if (a->s<2)
  {
#ifdef HAVE_LIBGMP2
    s+=a->n._mp_alloc;
#else
    s+=a->n.alloc;
#endif
  }
  return s;
}

/*
* delete leading 0s from a gmp number
*/
#ifdef HAVE_LIBGMP1
static void nlGmpSimple(MP_INT *z)
{
  int k;
  if (mpz_limb_size(z)<0)
  {
    k=-mpz_limb_size(z)-1;
    while ((k>0) && (mpz_limb_d(z)[k]==0))
    { k--;printf("X"); }
    k++;
    mpz_limb_size(z)=-k;
  }
  else
  {
    k=mpz_limb_size(z)-1;
    while ((k>0) && (mpz_limb_d(z)[k]==0))
    { k--;printf("X"); }
    k++;
    mpz_limb_size(z)=k;
  }
}
#endif

/*2
* convert number to int
*/
int nlInt(number &i)
{
#ifdef LDEBUG
  nlTest(i);
#endif
  nlNormalize(i);
  if (SR_HDL(i) &SR_INT) return SR_TO_INT(i);
  nlGmpSimple(&i->z);
  if ((i->s!=3)||(mpz_size1(&i->z)>MP_SMALL)) return 0;
  int ul=(int)mpz_get_si(&i->z);
  if (mpz_cmp_si(&i->z,(long)ul)!=0) return 0;
  return ul;
}

/*2
* delete a
*/
#ifdef LDEBUG
void nlDBDelete (number * a,char *f, int l)
#else
void nlDelete (number * a)
#endif
{
  if (*a!=NULL)
  {
#ifdef LDEBUG
    nlTest(*a);
#endif
    if ((SR_HDL(*a) & SR_INT)==0)
    {
      switch ((*a)->s)
      {
        case 0:
        case 1:
          mpz_clear(&(*a)->n);
        case 3:
          mpz_clear(&(*a)->z);
#ifdef LDEBUG
          (*a)->s=2;
          (*a)->debug=654321;
#endif
      }
#if defined(MDEBUG) && defined(LDEBUG)
      mmDBFreeBlock((ADDRESS) *a,sizeof(rnumber),f,l);
#else
      Free((ADDRESS) *a,sizeof(rnumber));
#endif
    }
    *a=NULL;
  }
}

/*2
* copy a to b
*/
number nlCopy(number a)
{
  number b;
  if ((SR_HDL(a) & SR_INT)||(a==NULL))
  {
    return a;
  }
#ifdef LDEBUG
  nlTest(a);
#endif
  b=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  b->debug=123456;
#endif
  switch (a->s)
  {
    case 0:
    case 1:
            nlGmpSimple(&a->n);
            mpz_init_set(&b->n,&a->n);
    case 3:
            nlGmpSimple(&a->z);
            mpz_init_set(&b->z,&a->z);
            break;
  }
  b->s = a->s;
#ifdef LDEBUG
  nlTest(b);
#endif
  return b;
}

/*2
* za:= - za
*/
number nlNeg (number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  if(SR_HDL(a) &SR_INT)
  {
    int r=SR_TO_INT(a);
    if (r==(-(1<<28))) a=nlRInit(1<<28);
    else               a=INT_TO_SR(-r);
  }
  else
  {
    mpz_neg(&a->z,&a->z);
    if ((a->s==3) && (mpz_size1(&a->z)<=MP_SMALL))
    {
      int ui=(int)mpz_get_si(&a->z);
      if ((((ui<<3)>>3)==ui)
      && (mpz_cmp_si(&a->z,(long)ui)==0))
      {
        mpz_clear(&a->z);
        Free((ADDRESS)a,sizeof(rnumber));
        a=INT_TO_SR(ui);
      }
    }
  }
#ifdef LDEBUG
  nlTest(a);
#endif
  return a;
}

/*
* 1/a
*/
number nlInvers(number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  number n;
  if (SR_HDL(a) & SR_INT)
  {
    if ((a==INT_TO_SR(1)) || (a==INT_TO_SR(-1)))
    {
      return a;
    }
    if (nlIsZero(a))
    {
      WerrorS("div. 1/0");
      return INT_TO_SR(0);
    }
    n=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
    n->debug=123456;
#endif
    n->s=1;
    if ((int)a>0)
    {
      mpz_init_set_si(&n->z,(long)1);
      mpz_init_set_si(&n->n,(long)SR_TO_INT(a));
    }
    else
    {
      mpz_init_set_si(&n->z,(long)-1);
      mpz_init_set_si(&n->n,(long)-SR_TO_INT(a));
#ifdef LDEBUG
    nlTest(n);
#endif
    }
#ifdef LDEBUG
    nlTest(n);
#endif
    return n;
  }
  n=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  n->debug=123456;
#endif
  {
    n->s=a->s;
    mpz_init_set(&n->n,&a->z);
    switch (a->s)
    {
      case 0:
      case 1:
              mpz_init_set(&n->z,&a->n);
              if (mpz_isNeg(&n->n)) /* && n->s<2*/
              {
                mpz_neg(&n->z,&n->z);
                mpz_neg(&n->n,&n->n);
              }
              if (mpz_cmp_si(&n->n,(long)1)==0)
              {
                mpz_clear(&n->n);
                n->s=3;
                if (mpz_size1(&n->z)<=MP_SMALL)
                {
                  int ui=(int)mpz_get_si(&n->z);
                  if ((((ui<<3)>>3)==ui)
                  && (mpz_cmp_si(&n->z,(long)ui)==0))
                  {
                    mpz_clear(&n->z);
                    Free((ADDRESS)n,sizeof(rnumber));
                    n=INT_TO_SR(ui);
                  }
                }
              }
              break;
      case 3:
              n->s=1;
              if (mpz_isNeg(&n->n)) /* && n->s<2*/
              {
                mpz_neg(&n->n,&n->n);
                mpz_init_set_si(&n->z,(long)-1);
              }
              else
              {
                mpz_init_set_si(&n->z,(long)1);
              }
              break;
    }
  }
#ifdef LDEBUG
  nlTest(n);
#endif
  return n;
}

/*2
* u:= a + b
*/
number nlAdd (number a, number b)
{
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    int r=SR_HDL(a)+SR_HDL(b)-1;
    if ( ((r << 1) >> 1) == r )
    {
      return (number)r;
    }
    u=(number)Alloc(sizeof(rnumber));
    u->s=3;
    mpz_init_set_si(&u->z,(long)SR_TO_INT(r));
#ifdef LDEBUG
    u->debug=123456;
    nlTest(u);
#endif
    return u;
  }
  u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  u->debug=123456;
#endif
  mpz_init(&u->z);
  if (SR_HDL(b) & SR_INT)
  {
    number x=a;
    a=b;
    b=x;
  }
  if (SR_HDL(a) & SR_INT)
  {
    switch (b->s)
    {
      case 0:
      case 1:/* a:short, b:1 */
      {
        MP_INT x;
        mpz_init(&x);
        if ((int)a>0)
        {
          mpz_mul_ui(&x,&b->n,SR_TO_INT(a));
        }
        else
        {
          mpz_mul_ui(&x,&b->n,-SR_TO_INT(a));
          mpz_neg(&x,&x);
        }
        mpz_add(&u->z,&b->z,&x);
        nlGmpSimple(&u->z);
        mpz_clear(&x);
        if (mpz_cmp_ui(&u->z,(long)0)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(0);
        }
        if (mpz_cmp(&u->z,&b->n)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(1);
        }
        mpz_init_set(&u->n,&b->n);
        u->s = 0;
        break;
      }
      case 3:
      {
        if ((int)a>0)
          mpz_add_ui(&u->z,&b->z,SR_TO_INT(a));
        else
          mpz_sub_ui(&u->z,&b->z,-SR_TO_INT(a));
        nlGmpSimple(&u->z);
        if (mpz_cmp_ui(&u->z,(long)0)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(0);
        }
        //u->s = 3;
        if (mpz_size1(&u->z)<=MP_SMALL)
        {
          int ui=(int)mpz_get_si(&u->z);
          if ((((ui<<3)>>3)==ui)
          && (mpz_cmp_si(&u->z,(long)ui)==0))
          {
            mpz_clear(&u->z);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(ui);
          }
        }
        u->s = 3;
        break;
      }
    }
  }
  else
  {
    switch (a->s)
    {
      case 0:
      case 1:
      {
        switch(b->s)
        {
          case 0:
          case 1:
          {
            MP_INT x;
            MP_INT y;
            mpz_init(&x);
            mpz_init(&y);
            mpz_mul(&x,&b->z,&a->n);
            mpz_mul(&y,&a->z,&b->n);
            mpz_add(&u->z,&x,&y);
            nlGmpSimple(&u->z);
            mpz_clear(&x);
            mpz_clear(&y);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            mpz_init(&u->n);
            mpz_mul(&u->n,&a->n,&b->n);
            nlGmpSimple(&u->n);
            if (mpz_cmp(&u->z,&u->n)==0)
            {
               mpz_clear(&u->z);
               mpz_clear(&u->n);
               Free((ADDRESS)u,sizeof(rnumber));
               return INT_TO_SR(1);
            }
            u->s = 0;
            break;
          }
          case 3: /* a:1 b:3 */
          {
            MP_INT x;
            mpz_init(&x);
            mpz_mul(&x,&b->z,&a->n);
            mpz_add(&u->z,&a->z,&x);
            nlGmpSimple(&u->z);
            mpz_clear(&x);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            if (mpz_cmp(&u->z,&a->n)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(1);
            }
            mpz_init_set(&u->n,&a->n);
            u->s = 0;
            break;
          }
        } /*switch (b->s) */
        break;
      }
      case 3:
      {
        switch(b->s)
        {
          case 0:
          case 1:/* a:3, b:1 */
          {
            MP_INT x;
            mpz_init(&x);
            mpz_mul(&x,&a->z,&b->n);
            mpz_add(&u->z,&b->z,&x);
            nlGmpSimple(&u->z);
            mpz_clear(&x);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            if (mpz_cmp(&u->z,&b->n)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(1);
            }
            mpz_init_set(&u->n,&b->n);
            u->s = 0;
            break;
          }
          case 3:
          {
            mpz_add(&u->z,&a->z,&b->z);
            nlGmpSimple(&u->z);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            if (mpz_size1(&u->z)<=MP_SMALL)
            {
              int ui=(int)mpz_get_si(&u->z);
              if ((((ui<<3)>>3)==ui)
              && (mpz_cmp_si(&u->z,(long)ui)==0))
              {
                mpz_clear(&u->z);
                Free((ADDRESS)u,sizeof(rnumber));
                return INT_TO_SR(ui);
              }
            }
            u->s = 3;
            break;
          }
        }
        break;
      }
    }
  }
#ifdef LDEBUG
  nlTest(u);
#endif
  return u;
}

/*2
* u:= a - b
*/
number nlSub (number a, number b)
{
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    int r=SR_HDL(a)-SR_HDL(b)+1;
    if ( ((r << 1) >> 1) == r )
    {
      return (number)r;
    }
    u=(number)Alloc(sizeof(rnumber));
    u->s=3;
    mpz_init_set_si(&u->z,(long)SR_TO_INT(r));
#ifdef LDEBUG
    u->debug=123456;
    nlTest(u);
#endif
    return u;
  }
  u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  u->debug=123456;
#endif
  mpz_init(&u->z);
  if (SR_HDL(a) & SR_INT)
  {
    switch (b->s)
    {
      case 0:
      case 1:/* a:short, b:1 */
      {
        MP_INT x;
        mpz_init(&x);
        if ((int)a>0)
        {
          mpz_mul_ui(&x,&b->n,SR_TO_INT(a));
        }
        else
        {
          mpz_mul_ui(&x,&b->n,-SR_TO_INT(a));
          mpz_neg(&x,&x);
        }
        mpz_sub(&u->z,&x,&b->z);
        mpz_clear(&x);
        if (mpz_cmp_ui(&u->z,(long)0)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(0);
        }
        if (mpz_cmp(&u->z,&b->n)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(1);
        }
        mpz_init_set(&u->n,&b->n);
        u->s = 0;
        break;
      }
      case 3:
      {
        if ((int)a>0)
        {
          mpz_sub_ui(&u->z,&b->z,SR_TO_INT(a));
          mpz_neg(&u->z,&u->z);
        }
        else
        {
          mpz_add_ui(&u->z,&b->z,-SR_TO_INT(a));
          mpz_neg(&u->z,&u->z);
        }
        nlGmpSimple(&u->z);
        if (mpz_cmp_ui(&u->z,(long)0)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(0);
        }
        //u->s = 3;
        if (mpz_size1(&u->z)<=MP_SMALL)
        {
          int ui=(int)mpz_get_si(&u->z);
          if ((((ui<<3)>>3)==ui)
          && (mpz_cmp_si(&u->z,(long)ui)==0))
          {
            mpz_clear(&u->z);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(ui);
          }
        }
        u->s = 3;
        break;
      }
    }
  }
  else if (SR_HDL(b) & SR_INT)
  {
    switch (a->s)
    {
      case 0:
      case 1:/* b:short, a:1 */
      {
        MP_INT x;
        mpz_init(&x);
        if ((int)b>0)
        {
          mpz_mul_ui(&x,&a->n,SR_TO_INT(b));
        }
        else
        {
          mpz_mul_ui(&x,&a->n,-SR_TO_INT(b));
          mpz_neg(&x,&x);
        }
        mpz_sub(&u->z,&a->z,&x);
        mpz_clear(&x);
        if (mpz_cmp_ui(&u->z,(long)0)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(0);
        }
        if (mpz_cmp(&u->z,&a->n)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(1);
        }
        mpz_init_set(&u->n,&a->n);
        u->s = 0;
        break;
      }
      case 3:
      {
        if ((int)b>0)
        {
          mpz_sub_ui(&u->z,&a->z,SR_TO_INT(b));
        }
        else
        {
          mpz_add_ui(&u->z,&a->z,-SR_TO_INT(b));
        }
        if (mpz_cmp_ui(&u->z,(long)0)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(0);
        }
        //u->s = 3;
        nlGmpSimple(&u->z);
        if (mpz_size1(&u->z)<=MP_SMALL)
        {
          int ui=(int)mpz_get_si(&u->z);
          if ((((ui<<3)>>3)==ui)
          && (mpz_cmp_si(&u->z,(long)ui)==0))
          {
            mpz_clear(&u->z);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(ui);
          }
        }
        u->s = 3;
        break;
      }
    }
  }
  else
  {
    switch (a->s)
    {
      case 0:
      case 1:
      {
        switch(b->s)
        {
          case 0:
          case 1:
          {
            MP_INT x;
            MP_INT y;
            mpz_init(&x);
            mpz_init(&y);
            mpz_mul(&x,&b->z,&a->n);
            mpz_mul(&y,&a->z,&b->n);
            mpz_sub(&u->z,&y,&x);
            mpz_clear(&x);
            mpz_clear(&y);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            mpz_init(&u->n);
            mpz_mul(&u->n,&a->n,&b->n);
            if (mpz_cmp(&u->z,&u->n)==0)
            {
              mpz_clear(&u->z);
              mpz_clear(&u->n);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(1);
            }
            u->s = 0;
            break;
          }
          case 3: /* a:1, b:3 */
          {
            MP_INT x;
            mpz_init(&x);
            mpz_mul(&x,&b->z,&a->n);
            mpz_sub(&u->z,&a->z,&x);
            mpz_clear(&x);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            if (mpz_cmp(&u->z,&a->n)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(1);
            }
            mpz_init_set(&u->n,&a->n);
            u->s = 0;
            break;
          }
        }
        break;
      }
      case 3:
      {
        switch(b->s)
        {
          case 0:
          case 1: /* a:3, b:1 */
          {
            MP_INT x;
            mpz_init(&x);
            mpz_mul(&x,&a->z,&b->n);
            mpz_sub(&u->z,&x,&b->z);
            mpz_clear(&x);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            if (mpz_cmp(&u->z,&b->n)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(1);
            }
            mpz_init_set(&u->n,&b->n);
            u->s = 0;
            break;
          }
          case 3: /* a:3 , b:3 */
          {
            mpz_sub(&u->z,&a->z,&b->z);
            nlGmpSimple(&u->z);
            if (mpz_cmp_ui(&u->z,(long)0)==0)
            {
              mpz_clear(&u->z);
              Free((ADDRESS)u,sizeof(rnumber));
              return INT_TO_SR(0);
            }
            //u->s = 3;
            if (mpz_size1(&u->z)<=MP_SMALL)
            {
              int ui=(int)mpz_get_si(&u->z);
              if ((((ui<<3)>>3)==ui)
              && (mpz_cmp_si(&u->z,(long)ui)==0))
              {
                mpz_clear(&u->z);
                Free((ADDRESS)u,sizeof(rnumber));
                return INT_TO_SR(ui);
              }
            }
            u->s = 3;
            break;
          }
        }
        break;
      }
    }
  }
#ifdef LDEBUG
  nlTest(u);
#endif
  return u;
}

/*2
* u := a * b
*/
number nlMult (number a, number b)
{
  number u;

#ifdef LDEBUG
  nlTest(a);
  nlTest(b);
#endif
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    if (b==INT_TO_SR(0)) return INT_TO_SR(0);
    int r=(SR_HDL(a)-1)*(SR_HDL(b)>>1);
    if ((r/(SR_HDL(b)>>1))==(SR_HDL(a)-1))
    {
      u=((number) ((r>>1)+SR_INT));
      if (((SR_HDL(u)<<1)>>1)==SR_HDL(u)) return (u);
      return nlRInit(SR_HDL(u)>>2);
    }
    u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
    u->debug=123456;
#endif
    u->s=3;
    if ((int)b>0)
    {
      mpz_init_set_si(&u->z,(long)SR_TO_INT(a));
      mpz_mul_ui(&u->z,&u->z,(unsigned long)SR_TO_INT(b));
    }
//    else if ((int)a>=0)
//    {
//      mpz_init_set_si(&u->z,(long)SR_TO_INT(b));
//      mpz_mul_ui(&u->z,&u->z,(unsigned long)SR_TO_INT(a));
//    }
    else
    {
      mpz_init_set_si(&u->z,(long)(-SR_TO_INT(a)));
      mpz_mul_ui(&u->z,&u->z,(long)(-SR_TO_INT(b)));
    }
#ifdef LDEBUG
    nlTest(u);
#endif
    return u;
  }
  else
  {
    if ((a==INT_TO_SR(0))
    ||(b==INT_TO_SR(0)))
      return INT_TO_SR(0);
    u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
    u->debug=123456;
#endif
    mpz_init(&u->z);
    if (SR_HDL(b) & SR_INT)
    {
      number x=a;
      a=b;
      b=x;
    }
    if (SR_HDL(a) & SR_INT)
    {
      u->s=b->s;
      if (u->s==1) u->s=0;
      if ((int)a>0)
      {
        mpz_mul_ui(&u->z,&b->z,(unsigned long)SR_TO_INT(a));
      }
      else
      {
        if (a==INT_TO_SR(-1))
        {
          mpz_set(&u->z,&b->z);
          mpz_neg(&u->z,&u->z);
          u->s=b->s;
        }
        else
        {
          mpz_mul_ui(&u->z,&b->z,(unsigned long)-SR_TO_INT(a));
          mpz_neg(&u->z,&u->z);
        }  
      }
      nlGmpSimple(&u->z);
      if (u->s<2)
      {
        if (mpz_cmp(&u->z,&b->n)==0)
        {
          mpz_clear(&u->z);
          Free((ADDRESS)u,sizeof(rnumber));
          return INT_TO_SR(1);
        }
        mpz_init_set(&u->n,&b->n);
      }
      else //u->s==3
      {
        if (mpz_size1(&u->z)<=MP_SMALL)
        {
          int ui=(int)mpz_get_si(&u->z);
          if ((((ui<<3)>>3)==ui)
          && (mpz_cmp_si(&u->z,(long)ui)==0))
          {
            mpz_clear(&u->z);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(ui);
          }
        }
      }
    }
    else
    {
      mpz_mul(&u->z,&a->z,&b->z);
      u->s = 0;
      if(a->s==3)
      {
        if(b->s==3)
        {
          u->s = 3;
        }
        else
        {
          if (mpz_cmp(&u->z,&b->n)==0)
          {
            mpz_clear(&u->z);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(1);
          }
          mpz_init_set(&u->n,&b->n);
        }
      }
      else
      {
        if(b->s==3)
        {
          if (mpz_cmp(&u->z,&a->n)==0)
          {
            mpz_clear(&u->z);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(1);
          }
          mpz_init_set(&u->n,&a->n);
        }
        else
        {
          mpz_init(&u->n);
          mpz_mul(&u->n,&a->n,&b->n);
          if (mpz_cmp(&u->z,&u->n)==0)
          {
            mpz_clear(&u->z);
            mpz_clear(&u->n);
            Free((ADDRESS)u,sizeof(rnumber));
            return INT_TO_SR(1);
          }
        }
      }
    }
  }
#ifdef LDEBUG
  nlTest(u);
#endif
  return u;
}

/*2
* u := a / b in Z
*/
number nlIntDiv (number a, number b)
{
  if (b==INT_TO_SR(0))
  {
    WerrorS("div. by 0");
    return INT_TO_SR(0);
  }
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    /* the small int -(1<<28) divided by -1 is the large int (1<<28)   */
    if ((a==INT_TO_SR(-(1<<28)))&&(b==INT_TO_SR(-1)))
    {
      return nlRInit(1<<28);
    }
    /* consider the signs of a and b:
    *  + + -> u=a+b-1
    *  + - -> u=a-b-1
    *  - + -> u=a-b+1
    *  - - -> u=a+b+1
    */
    int aa=SR_TO_INT(a);
    int bb=SR_TO_INT(b);
    if (aa>0)
    {
      if (bb>0)
        return INT_TO_SR((aa+bb-1)/bb);
      else
        return INT_TO_SR((aa-bb-1)/bb);
    }
    else
    {
      if (bb>0)
      {
        return INT_TO_SR((aa-bb+1)/bb);
      }
      else
      {
        return INT_TO_SR((aa+bb+1)/bb);
      }
    }
  }
  if (SR_HDL(a) & SR_INT)
  {
    /* a is a small and b is a large int: -> 0 */
    return INT_TO_SR(0);
  }
  number bb=NULL;
  if (SR_HDL(b) & SR_INT)
  {
    bb=nlRInit(SR_TO_INT(b));
    b=bb;
  }
  u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  u->debug=123456;
#endif
  mpz_init_set(&u->z,&a->z);
  /* consider the signs of a and b:
  *  + + -> u=a+b-1
  *  + - -> u=a-b-1
  *  - + -> u=a-b+1
  *  - - -> u=a+b+1
  */
  if (mpz_isNeg(&a->z))
  {
    if (mpz_isNeg(&b->z))
    {
      mpz_add(&u->z,&u->z,&b->z);
    }
    else
    {
      mpz_sub(&u->z,&u->z,&b->z);
    }
    mpz_add_ui(&u->z,&u->z,1);
  }
  else
  {
    if (mpz_isNeg(&b->z))
    {
      mpz_sub(&u->z,&u->z,&b->z);
    }
    else
    {
      mpz_add(&u->z,&u->z,&b->z);
    }
    mpz_sub_ui(&u->z,&u->z,1);
  }
  /* u=u/b */
  u->s = 3;
  MPZ_DIV(&u->z,&u->z,&b->z);
  nlGmpSimple(&u->z);
  if (bb!=NULL)
  {
    mpz_clear(&bb->z);
    Free((ADDRESS)bb,sizeof(rnumber));
  }
  if (mpz_size1(&u->z)<=MP_SMALL)
  {
    int ui=(int)mpz_get_si(&u->z);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(&u->z,(long)ui)==0))
    {
      mpz_clear(&u->z);
      Free((ADDRESS)u,sizeof(rnumber));
      u=INT_TO_SR(ui);
    }
  }
#ifdef LDEBUG
  nlTest(u);
#endif
  return u;
}

/*2
* u := a mod b in Z, u>=0
*/
number nlIntMod (number a, number b)
{
  if (b==INT_TO_SR(0))
  {
    WerrorS("div. by 0");
    return INT_TO_SR(0);
  }
  if (a==INT_TO_SR(0))
      return INT_TO_SR(0);
  number u;
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    if ((int)a>0)
    {
      if ((int)b>0)
        return INT_TO_SR(SR_TO_INT(a)%SR_TO_INT(b));
      else
        return INT_TO_SR(SR_TO_INT(a)%(-SR_TO_INT(b)));
    }
    else
    {
      if ((int)b>0)
      {
        int i=(-SR_TO_INT(a))%SR_TO_INT(b);
        if ( i != 0 ) i = (SR_TO_INT(b))-i;
        return INT_TO_SR(i);
      }
      else
      {
        int i=(-SR_TO_INT(a))%(-SR_TO_INT(b));
        if ( i != 0 ) i = (-SR_TO_INT(b))-i;
        return INT_TO_SR(i);
      }
    }
  }
  if (SR_HDL(a) & SR_INT)
  {
    /* a is a small and b is a large int: -> a or (a+b) or (a-b) */
    if ((int)a<0)
    {
      if (mpz_isNeg(&b->z))
        return nlSub(a,b);
      /*else*/
        return nlAdd(a,b);
    }
    /*else*/
      return a;
  }
  number bb=NULL;
  if (SR_HDL(b) & SR_INT)
  {
    bb=nlRInit(SR_TO_INT(b));
    b=bb;
  }
  u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  u->debug=123456;
#endif
  mpz_init(&u->z);
  u->s = 3;
  mpz_mod(&u->z,&a->z,&b->z);
  if (bb!=NULL)
  {
    mpz_clear(&bb->z);
    Free((ADDRESS)bb,sizeof(rnumber));
  }
  if (mpz_isNeg(&u->z))
  {
    if (mpz_isNeg(&b->z))
      mpz_sub(&u->z,&u->z,&b->z);
    else
      mpz_add(&u->z,&u->z,&b->z);
  }
  nlGmpSimple(&u->z);
  if (mpz_size1(&u->z)<=MP_SMALL)
  {
    int ui=(int)mpz_get_si(&u->z);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(&u->z,(long)ui)==0))
    {
      mpz_clear(&u->z);
      Free((ADDRESS)u,sizeof(rnumber));
      u=INT_TO_SR(ui);
    }
  }
#ifdef LDEBUG
  nlTest(u);
#endif
  return u;
}

/*2
* u := a / b
*/
number nlDiv (number a, number b)
{
  number u;
  if (nlIsZero(b))
  {
    WerrorS("div. by 0");
    return INT_TO_SR(0);
  }
  u=(number)Alloc(sizeof(rnumber));
  u->s=0;
#ifdef LDEBUG
  u->debug=123456;
#endif
// ---------- short / short ------------------------------------
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    int i=SR_TO_INT(a);
    int j=SR_TO_INT(b);
    int r=i%j;
    if (r==0)
    {
      Free((ADDRESS)u,sizeof(rnumber));
      return INT_TO_SR(i/j);
    }
    mpz_init_set_si(&u->z,(long)i);
    mpz_init_set_si(&u->n,(long)j);
  }
  else
  {
    mpz_init(&u->z);
// ---------- short / long ------------------------------------
    if (SR_HDL(a) & SR_INT)
    {
      // short a / (z/n) -> (a*n)/z
      if (b->s<2)
      {
        if ((int)a>=0)
          mpz_mul_ui(&u->z,&b->n,SR_TO_INT(a));
        else
        {
          mpz_mul_ui(&u->z,&b->n,-SR_TO_INT(a));
          mpz_neg(&u->z,&u->z);
        }
      }
      else
      // short a / long z -> a/z
      {
        mpz_set_si(&u->z,SR_TO_INT(a));
      }
      if (mpz_cmp(&u->z,&b->z)==0)
      {
        mpz_clear(&u->z);
        Free((ADDRESS)u,sizeof(rnumber));
        return INT_TO_SR(1);
      }
      mpz_init_set(&u->n,&b->z);
    }
// ---------- long / short ------------------------------------
    else if (SR_HDL(b) & SR_INT)
    {
      mpz_set(&u->z,&a->z);
      // (z/n) / b -> z/(n*b)
      if (a->s<2)
      {
        mpz_init_set(&u->n,&a->n);
        if ((int)b>0)
          mpz_mul_ui(&u->n,&u->n,SR_TO_INT(b));
        else
        {
          mpz_mul_ui(&u->n,&u->n,-SR_TO_INT(b));
          mpz_neg(&u->z,&u->z);
        }
      }
      else
      // long z / short b -> z/b
      {
        //mpz_set(&u->z,&a->z);
        mpz_init_set_si(&u->n,SR_TO_INT(b));
      }
    }
// ---------- long / long ------------------------------------
    else
    {
      //u->s=0;
      mpz_set(&u->z,&a->z);
      mpz_init_set(&u->n,&b->z);
      if (a->s<2) mpz_mul(&u->n,&u->n,&a->n);
      if (b->s<2) mpz_mul(&u->z,&u->z,&b->n);
    }
  }
  if (mpz_isNeg(&u->n))
  {
    mpz_neg(&u->z,&u->z);
    mpz_neg(&u->n,&u->n);
  }
  if (mpz_cmp_si(&u->n,(long)1)==0)
  {
    mpz_clear(&u->n);
    if (mpz_size1(&u->z)<=MP_SMALL)
    {
      int ui=(int)mpz_get_si(&u->z);
      if ((((ui<<3)>>3)==ui)
      && (mpz_cmp_si(&u->z,(long)ui)==0))
      {
        mpz_clear(&u->z);
        Free((ADDRESS)u,sizeof(rnumber));
        return INT_TO_SR(ui);
      }
    }
    u->s=3;
  }
#ifdef LDEBUG
  nlTest(u);
#endif
  return u;
}

#if (defined(i386)) && (defined(HAVE_LIBGMP1))
/*
* compute x^exp for x in Z
* there seems to be an bug in mpz_pow_ui for i386
*/
static inline void nlPow (MP_INT * res,MP_INT * x,int exp)
{
  if (exp==0)
  {
    mpz_set_si(res,(long)1);
  }
  else
  {
    MP_INT xh;
    mpz_init(&xh);
    mpz_set(res,x);
    exp--;
    while (exp!=0)
    {
      mpz_mul(&xh,res,x);
      mpz_set(res,&xh);
      exp--;
    }
    mpz_clear(&xh);
  }
}
#endif

/*2
* u:= x ^ exp
*/
void nlPower (number x,int exp,number * u)
{
  if (!nlIsZero(x))
  {
#ifdef LDEBUG
    nlTest(x);
#endif
    number aa=NULL;
    if (SR_HDL(x) & SR_INT)
    {
      aa=nlRInit(SR_TO_INT(x));
      x=aa;
    }
    *u=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
    (*u)->debug=123456;
#endif
    mpz_init(&(*u)->z);
    (*u)->s = x->s;
#if (!defined(i386)) || (defined(HAVE_LIBGMP2))
    mpz_pow_ui(&(*u)->z,&x->z,(unsigned long)exp);
#else
    nlPow(&(*u)->z,&x->z,exp);
#endif
    if (x->s<2)
    {
      mpz_init(&(*u)->n);
#if (!defined(i386)) || (defined(HAVE_LIBGMP2))
      mpz_pow_ui(&(*u)->n,&x->n,(unsigned long)exp);
#else
      nlPow(&(*u)->n,&x->n,exp);
#endif
    }
    if (aa!=NULL)
    {
      mpz_clear(&aa->z);
      Free((ADDRESS)aa,sizeof(rnumber));
    }
    if (((*u)->s<=2) && (mpz_cmp_si(&(*u)->n,(long)1)==0))
    {
      mpz_clear(&(*u)->n);
      (*u)->s=3;
    }
    if (((*u)->s==3) && (mpz_size1(&(*u)->z)<=MP_SMALL))
    {
      int ui=(int)mpz_get_si(&(*u)->z);
      if ((((ui<<3)>>3)==ui)
      && (mpz_cmp_si(&(*u)->z,(long)ui)==0))
      {
        mpz_clear(&(*u)->z);
        Free((ADDRESS)*u,sizeof(rnumber));
        *u=INT_TO_SR(ui);
      }
    }
  }
  else
    *u = INT_TO_SR(0);
#ifdef LDEBUG
  nlTest(*u);
#endif
}

BOOLEAN nlIsZero (number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  return (a==INT_TO_SR(0));
}

/*2
* za >= 0 ?
*/
BOOLEAN nlGreaterZero (number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  if (SR_HDL(a) & SR_INT) return SR_HDL(a)>=0;
  return (!mpz_isNeg(&a->z));
}

/*2
* a > b ?
*/
BOOLEAN nlGreater (number a, number b)
{
#ifdef LDEBUG
  nlTest(a);
  nlTest(b);
#endif
  number r;
  BOOLEAN rr;
  r=nlSub(a,b);
  rr=(!nlIsZero(r)) && (nlGreaterZero(r));
  nlDelete(&r);
  return rr;
}

/*2
* a = b ?
*/
BOOLEAN nlEqual (number a, number b)
{
#ifdef LDEBUG
  nlTest(a);
  nlTest(b);
#endif
// short - short
  if (SR_HDL(a) & SR_HDL(b) & SR_INT) return a==b;
//  long - short
  BOOLEAN bo;
  if (SR_HDL(b) & SR_INT)
  {
    if (a->s!=0) return FALSE;
    number n=b; b=a; a=n;
  }
//  short - long
  if (SR_HDL(a) & SR_INT)
  {
    if (b->s!=0)
      return FALSE;
    if (((int)a > 0) && (mpz_isNeg(&b->z)))
      return FALSE;
    if (((int)a < 0) && (!mpz_isNeg(&b->z)))
      return FALSE;
    MP_INT  bb;
    mpz_init_set(&bb,&b->n);
    if ((int)a<0)
    {
      mpz_neg(&bb,&bb);
      mpz_mul_ui(&bb,&bb,(long)-SR_TO_INT(a));
    }
    else
    {
      mpz_mul_ui(&bb,&bb,(long)SR_TO_INT(a));
    }
    bo=(mpz_cmp(&bb,&b->z)==0);
    mpz_clear(&bb);
    return bo;
  }
// long - long
  if (((a->s==1) && (b->s==3))
  ||  ((b->s==1) && (a->s==3)))
    return FALSE;
  if (mpz_isNeg(&a->z)&&(!mpz_isNeg(&b->z)))
    return FALSE;
  if (mpz_isNeg(&b->z)&&(!mpz_isNeg(&a->z)))
    return FALSE;
  nlGmpSimple(&a->z);
  nlGmpSimple(&b->z);
  if (a->s<2)
    nlGmpSimple(&a->n);
  if (b->s<2)
    nlGmpSimple(&b->n);
  MP_INT  aa;
  MP_INT  bb;
  mpz_init_set(&aa,&a->z);
  mpz_init_set(&bb,&b->z);
  if (a->s<2) mpz_mul(&bb,&bb,&a->n);
  if (b->s<2) mpz_mul(&aa,&aa,&b->n);
  bo=(mpz_cmp(&aa,&bb)==0);
  mpz_clear(&aa);
  mpz_clear(&bb);
  return bo;
}

/*2
* a == 1 ?
*/
BOOLEAN nlIsOne (number a)
{
#ifdef LDEBUG
  if (a==NULL) return FALSE;
  nlTest(a);
#endif
  if (SR_HDL(a) & SR_INT) return (a==INT_TO_SR(1));
  return FALSE;
}

/*2
* a == -1 ?
*/
BOOLEAN nlIsMOne (number a)
{
#ifdef LDEBUG
  if (a==NULL) return FALSE;
  nlTest(a);
#endif
  if (SR_HDL(a) & SR_INT) return (a==INT_TO_SR(-1));
  return FALSE;
}

/*2
* result =gcd(a,b)
*/
number nlGcd(number a, number b)
{
  number result;
#ifdef LDEBUG
  nlTest(a);
  nlTest(b);
#endif
  //nlNormalize(a);
  //nlNormalize(b);
  if ((SR_HDL(a)==5)||(a==INT_TO_SR(-1))
  ||  (SR_HDL(b)==5)||(b==INT_TO_SR(-1))) return INT_TO_SR(1);
  if (SR_HDL(a) & SR_HDL(b) & SR_INT)
  {
    int i=ABS(SR_TO_INT(a));
    int j=ABS(SR_TO_INT(b));
    int l;
    do
    {
      l=i%j;
      i=j;
      j=l;
    } while (l!=0);
    return INT_TO_SR(i);
  }
  if (((!(SR_HDL(a) & SR_INT))&&(a->s<2))
  ||  ((!(SR_HDL(b) & SR_INT))&&(b->s<2))) return INT_TO_SR(1);
  number aa=NULL;
  if (SR_HDL(a) & SR_INT)
  {
    aa=nlRInit(SR_TO_INT(a));
    a=aa;
  }
  else
  if (SR_HDL(b) & SR_INT)
  {
    aa=nlRInit(SR_TO_INT(b));
    b=aa;
  }
  result=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  result->debug=123456;
#endif
  mpz_init(&result->z);
  mpz_gcd(&result->z,&a->z,&b->z);
  nlGmpSimple(&result->z);
  result->s = 3;
  if (mpz_size1(&result->z)<=MP_SMALL)
  {
    int ui=(int)mpz_get_si(&result->z);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(&result->z,(long)ui)==0))
    {
      mpz_clear(&result->z);
      Free((ADDRESS)result,sizeof(rnumber));
      result=INT_TO_SR(ui);
    }
  }
  if (aa!=NULL)
  {
    mpz_clear(&aa->z);
    Free((ADDRESS)aa,sizeof(rnumber));
  }
#ifdef LDEBUG
  nlTest(result);
#endif
  return result;
}

/*2
* simplify x
*/
void nlNormalize (number &x)
{
#ifdef LDEBUG
  nlTest(x);
#endif
  if ((SR_HDL(x) & SR_INT) ||(x==NULL))
    return;
  if (x->s!=1) nlGmpSimple(&x->z);
  if (x->s==0)
  {
    BOOLEAN divided=FALSE;
    MP_INT gcd;
    mpz_init(&gcd);
    mpz_gcd(&gcd,&x->z,&x->n);
    x->s=1;
    if (mpz_cmp_si(&gcd,(long)1)!=0)
    {
      MP_INT r;
      mpz_init(&r);
      MPZ_EXACTDIV(&r,&x->z,&gcd);
      mpz_set(&x->z,&r);
      MPZ_EXACTDIV(&r,&x->n,&gcd);
      mpz_set(&x->n,&r);
      mpz_clear(&r);
      nlGmpSimple(&x->z);
      divided=TRUE;
    }
    mpz_clear(&gcd);
    nlGmpSimple(&x->n);
    if (mpz_cmp_si(&x->n,(long)1)==0)
    {
      mpz_clear(&x->n);
      if (mpz_size1(&x->z)<=MP_SMALL)
      {
        int ui=(int)mpz_get_si(&x->z);
        if ((((ui<<3)>>3)==ui)
        && (mpz_cmp_si(&x->z,(long)ui)==0))
        {
          mpz_clear(&x->z);
#ifdef LDEBUG
          x->debug=654324;
#endif
          Free((ADDRESS)x,sizeof(rnumber));
          x=INT_TO_SR(ui);
          return;
        }
      }
      x->s=3;
    }
    else if (divided)
    {
      _mpz_realloc(&x->n,mpz_size1(&x->n));
    }
    if (divided) _mpz_realloc(&x->z,mpz_size1(&x->z));
  }
#ifdef LDEBUG
  nlTest(x);
#endif
}

/*2
* returns in result->z the lcm(a->z,b->n)
*/
number nlLcm(number a, number b)
{
  number result;
#ifdef LDEBUG
  nlTest(a);
  nlTest(b);
#endif
  if ((SR_HDL(b) & SR_INT)
  || (b->s==3))
  {
    // b is 1/(b->n) => b->n is 1 => result is a
    return nlCopy(a);
  }
  number aa=NULL;
  if (SR_HDL(a) & SR_INT)
  {
    aa=nlRInit(SR_TO_INT(a));
    a=aa;
  }
  result=(number)Alloc(sizeof(rnumber));
#ifdef LDEBUG
  result->debug=123456;
#endif
  result->s=3;
  MP_INT gcd;
  mpz_init(&gcd);
  mpz_init(&result->z);
  mpz_gcd(&gcd,&a->z,&b->n);
  if (mpz_cmp_si(&gcd,(long)1)!=0)
  {
    MP_INT bt;
    mpz_init_set(&bt,&b->n);
    MPZ_EXACTDIV(&bt,&bt,&gcd);
    mpz_mul(&result->z,&a->z,&bt);
    mpz_clear(&bt);
  }
  else
    mpz_mul(&result->z,&a->z,&b->n);
  mpz_clear(&gcd);
  if (aa!=NULL)
  {
    mpz_clear(&aa->z);
    Free((ADDRESS)aa,sizeof(rnumber));
  }
  nlGmpSimple(&result->z);
  if (mpz_size1(&result->z)<=MP_SMALL)
  {
    int ui=(int)mpz_get_si(&result->z);
    if ((((ui<<3)>>3)==ui)
    && (mpz_cmp_si(&result->z,(long)ui)==0))
    {
      mpz_clear(&result->z);
      Free((ADDRESS)result,sizeof(rnumber));
      return INT_TO_SR(ui);
    }
  }
#ifdef LDEBUG
  nlTest(result);
#endif
  return result;
}

int nlModP(number n, int p)
{
  if (SR_HDL(n) & SR_INT)
  {
    int i=SR_TO_INT(n);
    if (i<0) return (p-((-i)%p));
    return i%p;
  }
  int iz=mpz_mmod_ui(NULL,&n->z,(unsigned long)p);
  if (n->s!=3)
  {
    int in=mpz_mmod_ui(NULL,&n->n,(unsigned long)p);
    return (int)npDiv((number)iz,(number)in);
  }
  return iz;
}

#else

/*0 --------------------------------------------implementation*/

typedef unsigned int int32;
#define MAX_INT16 0XFFFF
#define SIGNUM 0X8000
#define BASIS 0X10000
#define MOD_BASIS 0XFFFF
#define SHIFT_BASIS 0X10

/*3
*  x%SIGNUM == length(x)
*  x/SIGNUM == 1 : x < 0
*/
#define NLLENGTH(a) ((int)(*(a)&0X7FFF))
#define NLNEG(a) (*(a)^SIGNUM)
#define NLSIGN(a) (*(a)>SIGNUM)

/*3
*allocate memory for a long integer
*/
static lint nlAllocLint (int L)
{
  lint x;

  if (L < SIGNUM)
  {
    x = (lint)Alloc((L+1)*sizeof(int16));
    *x = L;
    return x;
  }
  else
  {
    WerrorS("number too large");
    return NULL;
  }
}

/*3
*deallocate Memory of a long integer
*/
static void nlDeleteLint (lint x)
{
  if (x)
  {
    Free((ADDRESS)x,(NLLENGTH(x)+1)*sizeof(int16));
  }
}

/*3
*nlShrink the memory used by c
*/
static void nlShrink (lint * c)
{
  int j, i;
  lint  cc = *c;

  i = j = NLLENGTH(cc);
  while ((i!=0) && (cc[i]==0))
  {
    i--;
  }
  if (i != j)
  {
    cc = nlAllocLint(i);
    memcpy(cc+1,(*c)+1,i*sizeof(int16));
    nlDeleteLint(*c);
    *c = cc;
  }
}

/*3
* x == 1 ?
*/
static BOOLEAN nlIsOneLint (lint x)
{
  return ((*x == 1) && (x[1] == 1));
}

/*3
*compares absolute values
*abs(x) > abs(y) nlCompAbs(x,y) = 1;
*abs(x) < abs(y)               -1;
*abs(x) = abs(y)                0;
*/
static int nlCompAbs (lint x, lint y)
{
  int i = NLLENGTH(x);

  if (i != NLLENGTH(y))
  {
    if (i > NLLENGTH(y))
    {
      return 1;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    do
    {
      if (x[i] > y[i])
      {
        return 1;
      }
      if (x[i] < y[i])
      {
        return -1;
      }
      i--;
    }
    while (i!=0);
    return 0;
  }
}

/*3
*compares long integers
*(x) > (y)     nlComp(x,y) = 1;
*(x) < (y)                -1;
*(x) = (y)                 0;
*/
static int nlComp (lint x, lint y)
{
  int i = NLSIGN(x);

  if (i != NLSIGN(y))
  {
    if (i != 0)
    {
      return -1;
    }
    else
    {
      return 1;
    }
  }
  else if (i != 0)
  {
    return -nlCompAbs(x, y);
  }
  else
  {
    return nlCompAbs(x, y);
  }
}

/*3
* copy f to t
*/
static lint nlCopyLint (lint f)
{
  int i;
  lint t;

  if (f!=NULL)
  {
    i = (NLLENGTH(f)+1)*sizeof(int16);
    t = (lint)Alloc(i);
    memcpy(t, f, i);
    return t;
  }
  else
  {
    return NULL;
  }
}

/*3
* n := 1
*/
static lint nlInitOne()
{
  lint n = (lint)Alloc(2*sizeof(int16));
  *n = 1;
  n[1] = 1;
  return n;
}

/*3
* n := i
*/
static lint nlToLint (int16 i)
{
  if (i)
  {
    lint n = (lint)Alloc(2*sizeof(int16));
    *n = 1;
    n[1] = i;
    return n;
  }
  else
  {
    return NULL;
  }
}

/*3
* Add longs, signs are equal
*/
static lint nlAddBasic (lint a, lint b)
{
  int i, max, min;
  int32 w;
  lint  xx,h;

  max = NLLENGTH(a);
  min = NLLENGTH(b);
  if ((min == 1) && (max == 1))
  {
    w = (int32) a[1] + (int32) b[1];
    if (w < BASIS)
    {
      xx = (lint)Alloc(2*sizeof(int16));
      *xx = 1;
      xx[1] = (int16) w;
    }
    else
    {
      xx = (lint)Alloc(3 * sizeof(int16));
      *xx = 2;
      xx[1] = (int16) (MOD_BASIS&w);
      xx[2] = 1;
    }
    if (NLSIGN(a))
    {
      *xx += SIGNUM;
    }
    return xx;
  }
  if(min>max)
  {
    i = max;
    max = min;
    min = i;
    h = b;
  }
  else
  {
    h = a;
  }
  xx = (lint)Alloc((max+1)*sizeof(int16));
  *xx = (int16)max;
  w = (int32) a[1] + (int32) b[1];
  xx[1] = (int16) (MOD_BASIS&w);
  for (i=2; i<=min; i++)
  {
    w = (w>>SHIFT_BASIS) + (int32) a[i] + (int32) b[i];
    xx[i] = (int16) (MOD_BASIS&w);
  }
  i = min;
  loop
  {
    i++;
    if ((w<BASIS) || (i>max))
    {
      break;
    }
    w = (w>>SHIFT_BASIS) + (int32) h[i];
    xx[i] = (int16) (MOD_BASIS&w);
  }
  for(; i<=max; i++)
  {
    xx[i] = h[i];
  }
  if (w>=BASIS)
  {
    h = (lint)Alloc((max+2)*sizeof(int16));
    memcpy(h+1,xx+1,max*sizeof(int16));
    max++;
    *h = (int16)max;
    h[max] = 1;
    nlDeleteLint(xx);
    xx = h;
  }
  if (NLSIGN(a))
  {
    *xx += SIGNUM;
  }
  return xx;
}

/*3
* Subtract longs, signs are equal, a > b
*/
static lint nlSubBasic (lint a, lint b)
{
  int i,j1,j2;
  int16 w;
  lint xx;
  j1 = NLLENGTH(a);
  if (j1 == 1)
  {
    xx = (lint)Alloc(2*sizeof(int16));
    *xx = 1;
    xx[1] = a[1]-b[1];
    if (NLSIGN(a))
    {
      *xx += SIGNUM;
    }
    return xx;
  }
  j2 = NLLENGTH(b);
  xx = (lint)Alloc((j1+1)*sizeof(int16));
  *xx = (int16)j1;
  if(a[1] < b[1])
  {
    xx[1] = MAX_INT16-b[1]+a[1]+1;
    w = 1;
  }
  else
  {
    xx[1] = a[1]-b[1];
    w = 0;
  }
  for (i=2; i<=j2; i++)
  {
    if(a[i]>b[i])
    {
      xx[i] = a[i]-b[i]-w;
      w = 0;
    }
    else if(a[i]<b[i])
    {
      if(w!=0)
      {
        xx[i] = MAX_INT16 - b[i] + a[i];
      }
      else
      {
        xx[i] = MAX_INT16 - b[i] + a[i] + 1;
      }
      w = 1;
    }
    else
    {
      if(w!=0)
      {
        xx[i] = MAX_INT16;
      }
      else
      {
        xx[i] = 0;
      }
    }
  }
  i = j2;
  while(w!=0)
  {
    i++;
    if(a[i])
    {
      xx[i] = a[i]-w;
      w = 0;
    }
    else
    {
      xx[i] = MAX_INT16;
    }
  }
  while(i < j1)
  {
    i++;
    xx[i] = a[i];
  }
  nlShrink(&xx);
  if (NLSIGN(a))
  {
    *xx += SIGNUM;
  }
  return xx;
}

/*3
* c:= a + b
*/
static lint nlAddLong (lint a,lint b)
{
  if (a==NULL)
    return nlCopyLint(b);
  else if (b==NULL)
    return nlCopyLint(a);
  if (NLSIGN(a) == NLSIGN(b))
    return nlAddBasic(a,b);
  else
  {
    switch(nlCompAbs(a, b))
    {
      case -1:
        return nlSubBasic(b, a);
      case 1:
        return nlSubBasic(a, b);
      default:
        ;
    }
  }
  return NULL;
}

/*3
* c:= a - b
*/
static lint nlSubLong (lint a,lint b)
{
  lint c;
  if(b==NULL)
    return nlCopyLint(a);
  if(a==NULL)
  {
    c = nlCopyLint(b);
    *c = NLNEG(c);
    return c;
  }
  if (NLSIGN(a) != NLSIGN(b))
    return nlAddBasic(a,b);
  else
  {
    switch(nlCompAbs(a, b))
    {
      case 1:
        return nlSubBasic(a, b);
      case -1:
        c = nlSubBasic(b, a);
        *c = NLNEG(c);
        return c;
      default:
        ;
    }
  }
  return NULL;
}

/*3
* a[i] = a[i] + b[i]*x, i = 1 to length(b)
*/
static void nlAddMult (lint a, lint  b, int32 x)
{
  int32 w;
  int i, l = NLLENGTH(b);
  w = (int32) a[1] + (int32) b[1]*x;
  a[1] = (int16) (MOD_BASIS&w);
  for (i=2; i<=l; i++)
  {
    w = (w>>SHIFT_BASIS) + (int32) a[i] + (int32) b[i]*x;
    a[i] = (int16) (MOD_BASIS&w);
  }
  if (w>=BASIS)
    a[l+1] = (int16)(w>>SHIFT_BASIS);
}

/*3
* a[i] = b[i]*x, i = 1 to length(b)
*/
static void nlIniMult (lint a, lint  b, int32 x)
{
  int i, l = NLLENGTH(b);
  int32 w;
  w = (int32) b[1]*x;
  a[1] = (int16) (MOD_BASIS&w);
  for (i=2; i<=l; i++)
  {
    w = (w>>SHIFT_BASIS) + (int32) b[i]*x;
    a[i] = (int16) (MOD_BASIS&w);
  }
  if (w>=BASIS)
    a[l+1] = (int16)(w>>SHIFT_BASIS);
}

/*3
* mult long, c:= a * b
*/
static lint nlMultLong (lint a,lint b)
{
  int la, lb, i;
  int32 h;
  lint xx;
  if ((a==NULL) || (b==NULL))
    return NULL;
  la = NLLENGTH(a);
  lb = NLLENGTH(b);
  if ((la == 1) && (lb == 1))
  {
    h = (int32) a[1] * (int32) b[1];
    if (h < BASIS)
    {
      xx = (lint)Alloc(2*sizeof(int16));
      *xx = 1;
      xx[1] = (int16) h;
    }
    else
    {
      xx = (lint)Alloc(3 * sizeof(int16));
      *xx = 2;
      xx[1] = (int16) (MOD_BASIS&h);
      xx[2] = (int16) (h>>SHIFT_BASIS);
    }
    if ((NLSIGN(a)) != (NLSIGN(b)))
      *xx += SIGNUM;
    return xx;
  }
  i = la+lb;
  xx = (lint)Alloc((i+1)*sizeof(int16));
  *xx = (int16)i;
  memset(xx+1,0,i*sizeof(int16));
  if (la < lb)
  {
    nlIniMult(xx, b, (int32) a[1]);
    for (i=1; i<la; i++)
      nlAddMult(xx+i, b, (int32) a[i+1]);
  }
  else
  {
    nlIniMult(xx, a, (int32) b[1]);
    for (i=1; i<lb; i++)
      nlAddMult(xx+i, a, (int32) b[i+1]);
  }
  nlShrink(&xx);
  if ((NLSIGN(a)) != (NLSIGN(b)))
    *xx += SIGNUM;
  return xx;
}

/*3
* aq := aq div b, b small
*/
static void nlQuotSmall (lint * aq, int16 b)
{
  int al,j;
  int16 a1,c1;
  int32 db,da,dc;
  lint a,cc;
  db = (int32) b;
  a = *aq;
  al = NLLENGTH(a);
  a1 = a[al];
  if (al == 1)
  {
    a[1] = a1 / b;
    return;
  }
  if (a1 >= b)
  {
    cc = a;
    c1 = a1 / b;
    cc[al] = c1;
    a1 -= b*c1;
  }
  else
    cc = nlAllocLint(al-1);
  loop
  {
    al--;
    if (al==0)
    {
      if(a != cc)
        nlDeleteLint(a);
      *aq = cc;
      return;
    }
    if (a1==0)
    {
      j = al;
      loop
      {
        a1 = a[j];
        if (a1 >= b)
        {
          c1 = a1 / b;
          cc[j] = c1;
          a1 -= b*c1;
        }
        else
          cc[j] = 0;
        j--;
        if (j==0)
        {
          if(a != cc)
            nlDeleteLint(a);
          *aq = cc;
          return;
        }
        else if (a1!=0)
        {
          al = j;
          break;
        }
      }
    }
    da = BASIS*(int32) a1 + (int32) a[al];
    dc = da / db;
    cc[al] = (int16) dc;
    da -= db*dc;
    a1 = (int16) da;
  }
}

/*3
* a := a - b, b1...bj
*/
static void nlSubSimple (lint a, lint b, int j)
{
  int i;
  int16 w;
  if (a[1] < b[1])
  {
    a[1] += MAX_INT16-b[1]+1;
    w = 1;
  }
  else
  {
    a[1] -= b[1];
    w = 0;
  }
  for (i=2; i<=j; i++)
  {
    if(a[i] > b[i])
    {
      a[i] -= b[i]+w;
      w = 0;
    }
    else if(a[i] < b[i])
    {
      if (w)
        a[i] += MAX_INT16 - b[i];
      else
        a[i] += MAX_INT16 - b[i] + 1;
      w = 1;
    }
    else
    {
      if(w)
        a[i] = MAX_INT16;
      else
        a[i] = 0;
    }
  }
  if(w!=0)
    a[j+1]--;
}

/*3
* a := a - b*x, b1...bj
*/
static void nlSubMult(lint a, lint b, int32 x, int l)
{
  int i;
  int16 k, w;
  int32 y;
  y = (int32)b[1]*x;
  k = (int16)(MOD_BASIS&y);
  if (a[1] < k)
  {
    a[1] += MAX_INT16-k+1;
    w = 1;
  }
  else
  {
    a[1] -= k;
    w = 0;
  }
  for(i=2; i<=l; i++)
  {
    y = (y>>SHIFT_BASIS) + (int32)b[i]*x;
    k = (int16)(MOD_BASIS&y);
    if(a[i] > k)
    {
      a[i] -= k+w;
      w = 0;
    }
    else if(a[i] < k)
    {
      if (w!=0)
        a[i] += MAX_INT16 - k;
      else
        a[i] += MAX_INT16 - k + 1;
      w = 1;
    }
    else
    {
      if(w!=0)
        a[i] = MAX_INT16;
      else
        a[i] = 0;
    }
  }
  w += (int16)(y>>SHIFT_BASIS);
  if(w!=0)
    a[l+1] -= w;
}

/*3
* compare xi and yi from i=l to 1
*/
static BOOLEAN nlCompHigh (lint x, lint y, int l)
{
  int i;
  for (i=l; i!=0; i--)
  {
    if (x[i] > y[i])
      return TRUE;
    else if (x[i] < y[i])
      return FALSE;
  }
  return TRUE;
}

/*3
* aq := aq div bn, bn long
*/
static void nlQuotLong (lint * aq, lint bn)
{
  int bl,al,si,j;
  int16 b1,a1,a2;
  int32 db,db1,dc,dc1,dc2;
  lint  an,cc,ansi;
  BOOLEAN rel;
  bl = NLLENGTH(bn);
  b1 = bn[bl];
  db = BASIS*(int32)b1 + (int32)bn[bl-1];
  if ((bl > 2) && ((b1 < MAX_INT16) || (bn[bl-1] < MAX_INT16)))
    db++;
  db1 = (int32)(b1+1);
  an = *aq;
  al = NLLENGTH(an);
  a1 = an[al];
  a2 = an[al-1];
  si = al-bl;
  ansi = an+si;
  rel = nlCompHigh(ansi, bn, bl);
  if (rel)
  {
    cc = nlAllocLint(si+1);
    dc = (BASIS*(int32)a1 + (int32)a2) / db;
    if (dc > 1)
    {
      nlSubMult(ansi, bn, dc, bl);
      rel = nlCompHigh(ansi, bn, bl);
    }
    else
      dc = 0;
    while (rel)
    {
      dc++;
      nlSubSimple(ansi, bn, bl);
      rel = nlCompHigh(ansi, bn, bl);
    }
    cc[si+1] = (int16)dc;
    a1 = an[al];
    a2 = an[al-1];
  }
  else
    cc = nlAllocLint(si);
  loop
  {
    al--;
    if (al < bl)
    {
      nlDeleteLint(an);
      *aq = cc;
      return;
    }
    si--;
    ansi--;
    if (a1==0)
    {
      j = al;
      loop
      {
        a1 = a2;
        a2 = an[j-1];
        rel = nlCompHigh(ansi, bn, bl);
        if (rel)
        {
          dc = (BASIS*(int32)a1 + (int32)a2) / db;
          if (dc > 1)
          {
            nlSubMult(ansi, bn, dc, bl);
            rel = nlCompHigh(ansi, bn, bl);
          }
          else
            dc = 0;
          while (rel)
          {
            dc++;
            nlSubSimple(ansi, bn, bl);
            rel = nlCompHigh(ansi, bn, bl);
          }
          cc[si+1] = (int16)dc;
          a1 = an[j];
          a2 = an[j-1];
        }
        else
          cc[si+1] = 0;
        si--;
        ansi--;
        j--;
        if (j < bl)
        {
          nlDeleteLint(an);
          *aq = cc;
          return;
        }
        else if(a1!=0)
        {
          al = j;
          break;
        }
      }
    }
    dc2 = dc1 = (BASIS*(int32)a1+(int32)a2) / db1;
    loop
    {
      nlSubMult(ansi, bn, dc1, bl);
      a1 = an[al+1];
      if (a1!=0)
      {
        dc1 = (BASIS*(int32)a1+(int32)an[al]) / db1;
        dc2 += dc1;
      }
      else
        break;
    }
    a1 = an[al];
    a2 = an[al-1];
    rel = nlCompHigh(ansi, bn, bl);
    if (rel)
    {
      dc = (BASIS*(int32)a1 + (int32)a2) / db;
      if (dc > 1)
      {
        nlSubMult(ansi, bn, dc, bl);
        rel = nlCompHigh(ansi, bn, bl);
      }
      else
        dc = 0;
      while (rel)
      {
        dc++;
        nlSubSimple(ansi, bn, bl);
        rel = nlCompHigh(ansi, bn, bl);
      }
      a1 = an[al];
      a2 = an[al-1];
    }
    else
      dc = 0;
    cc[si+1] = (int16) (dc2+dc);
  }
}

/*3
* aq := aq div b
*/
static void nlQuotient (lint * aq, lint b)
{
  lint a;
  int bl;
  BOOLEAN neg;
  a = *aq;
  neg = (NLSIGN(a) != NLSIGN(b));
  bl = NLLENGTH(b);
  if(bl == 1)
    nlQuotSmall(&a,b[1]);
  else
    nlQuotLong(&a,b);
  if(neg && !NLSIGN(a))
    *a += SIGNUM;
  else if (!neg && NLSIGN(a))
    *a -= SIGNUM;
  *aq = a;
}

/*3
* r := a mod b, b small
* al = length(a)
*/
static void nlRemSmall (lint a, int al, int16 b, int16 * r)
{
  int j;
  int16 a1,c1;
  int32 db,da,dc;
  db = (int32)b;
  a1 = a[al];
  if (al == 1)
  {
    c1 = a1 / b;
    a1 -= b*c1;
    *r = a1;
    return;
  }
  if (a1 >= b)
  {
    c1 = a1 / b;
    a1 -= b*c1;
  }
  loop
  {
    al--;
    if (al==0)
    {
      *r = a1;
      return;
    }
    if (a1==0)
    {
      j = al;
      loop
      {
        a1 = a[j];
        if (a1 >= b)
        {
          c1 = a1 / b;
          a1 -= b*c1;
        }
        j--;
        if (j==0)
        {
          *r = a1;
          return;
        }
        else if (a1!=0)
        {
          al = j;
          break;
        }
      }
    }
    da = BASIS*(int32)a1 + (int32)a[al];
    dc = da / db;
    da -= db*dc;
    a1 = (int16)da;
  }
}

static void nlShrink0(lint c, int j, int * i)
{
  while((j!=0) && (c[j]==0)) j--;
  *i = j;
}

/*3
* rl := an mod bn, bn long, but <= an
* al = length(an)
*/
static void nlRemLong(lint an, lint bn, int al, int bl, int * rl)
{
  int si,j;
  int16 b1,a1,a2;
  int32 db,db1,dc;
  lint ansi;
  BOOLEAN rel;
  b1 = bn[bl];
  db = (int32)bn[bl-1];
  db += (int32)b1<<SHIFT_BASIS;
  if ((bl > 2) && ((b1 < MAX_INT16) || (bn[bl-1] < MAX_INT16)))
    db++;
  db1 = (int32)b1+1;
  a1 = an[al];
  a2 = an[al-1];
  si = al-bl;
  ansi = an+si;
  rel = nlCompHigh(ansi, bn, bl);
  if (rel)
  {
    dc = (BASIS*(int32)a1 + (int32)a2) / db;
    if (dc > 1)
    {
      nlSubMult(ansi, bn, dc, bl);
      rel = nlCompHigh(ansi, bn, bl);
    }
    while (rel)
    {
      nlSubSimple(ansi, bn, bl);
      rel = nlCompHigh(ansi, bn, bl);
    }
    a1 = an[al];
    a2 = an[al-1];
  }
  loop
  {
    al--;
    if (al < bl)
    {
      nlShrink0(an,bl,rl);
      return;
    }
    si--;
    ansi--;
    if(a1==0)
    {
      j = al;
      loop
      {
        a1 = a2;
        a2 = an[j-1];
        rel = nlCompHigh(ansi, bn, bl);
        if (rel)
        {
          dc = (BASIS*(int32)a1 + (int32)a2) / db;
          if (dc > 1)
          {
            nlSubMult(ansi, bn, dc, bl);
            rel = nlCompHigh(ansi, bn, bl);
          }
          while (rel)
          {
            nlSubSimple(ansi, bn, bl);
            rel = nlCompHigh(ansi, bn, bl);
          }
          a1 = an[j];
          a2 = an[j-1];
        }
        si--;
        ansi--;
        j--;
        if (j < bl)
        {
          nlShrink0(an,bl,rl);
          return;
        }
        else if (a1!=0)
        {
          al = j;
          break;
        }
      }
    }
    dc = (BASIS*(int32)a1+(int32)a2) / db1;
    loop
    {
      nlSubMult(ansi, bn, dc, bl);
      a1 = an[al+1];
      if (a1!=0)
        dc = (BASIS*(int32)a1+(int32)an[al]) / db1;
      else
        break;
    }
    a1 = an[al];
    a2 = an[al-1];
    rel = nlCompHigh(ansi, bn, bl);
    if (rel)
    {
      dc = (BASIS*(int32)a1 + (int32)a2) / db;
      if (dc > 1)
      {
        nlSubMult(ansi, bn, dc, bl);
        rel = nlCompHigh(ansi, bn, bl);
      }
      while (rel)
      {
        nlSubSimple(ansi, bn, bl);
        rel = nlCompHigh(ansi, bn, bl);
      }
      a1 = an[al];
      a2 = an[al-1];
    }
  }
}

/*3
* z = GCD(a, b)
*/
static void nlGcdLong (lint a, lint b, lint * z)
{
  lint h, x, y;
  int xl,yl,hl;
  int16 xc,yc,hc;

  x = nlCopyLint(a);
  if (NLSIGN(x))
    *x %= SIGNUM;
  if (nlIsOneLint(x))
  {
    *z=x;
    return;
  }
  y = nlCopyLint(b);
  if (NLSIGN(y))
    *y %= SIGNUM;
  if (nlIsOneLint(y))
  {
    *z=y;
    nlDeleteLint(x);
    return;
  }
  switch (nlCompAbs(x,y))
  {
    case -1: h=x;x=y;y=h;h=NULL;
            break;
    case 0: *z=x;
            nlDeleteLint(y);
            return;
    case 1: break;
  }
  xl = NLLENGTH(x);
  yl = NLLENGTH(y);
  /* we have: x > y >=0 as lint, xl>=yl>=0 as int*/
  while (yl > 1)
  {
    nlRemLong(x,y,xl,yl,&hl);
    h = x;
    x = y;
    y = h;
    xl = yl;
    yl = hl;
  }
  if(yl==0)
  {
    /*result is x with xl >1*/
    nlDeleteLint(y);
    nlShrink(&x);
    *z = x;
    return;
  }
  else
  {
    yc = y[1];
    if (xl>1)
      nlRemSmall(x,xl,yc,&hc);
    else
    {
      hc = x[1];
      if (hc > yc)
      {
        xc=yc;
        yc=hc;
        hc=xc;
      }
    }
  }
  nlDeleteLint(x);
  nlDeleteLint(y);
  /*we have now yc >hc*/
  while(hc!=0)
  {
    xc = yc;
    yc = hc;
    hc = xc % yc;
  }
  *z = nlToLint(yc);
}

/*-----------------------------------------------------------------*/

/*0
* Begin of operations with rational numbers
*/

/*3
* parameter s in number:
*  0 :  integer
*  1 :  normalised rational
* -1 :  not normalised rational
*/

#define NLISINT(a)  !((a)->s)
#define NLISRAT(a)   ((a)->s)

#ifdef LDEBUG
#define nlTest(a) nlDBTest(a,__FILE__,__LINE__)
BOOLEAN nlDBTest(number a, char *f,int l)
{
  BOOLEAN res;
  if (a==NULL)
  {
    return TRUE;
  }
  else
  {
    if (a->z!=NULL)
    {
      res=mmDBTestBlock(a->z,(NLLENGTH(a->z)+1)*sizeof(int16),f,l);
    }
    if (NLISRAT(a))
    {
      res = (res && mmDBTestBlock(a->n,(NLLENGTH(a->n)+1)*sizeof(int16),f,l));
    }
    return res;
  }
}
#endif

/*2
* delete a
*/
#ifdef LDEBUG
void nlDBDelete (number *a, char *f, int l)
#else
void nlDelete (number *a)
#endif
{
  number b=*a;
#ifdef LDEBUG
  nlTest(b);
#endif

  if (b!=NULL)
  {
    nlDeleteLint(b->z);
    if (NLISRAT(b))
    {
      nlDeleteLint(b->n);
    }
#if defined(LDEBUG) && defined(MDEBUG)
    mmDBFreeBlock((ADDRESS)b,sizeof(rnumber),f,l);
#else
    Free((ADDRESS)b,sizeof(rnumber));
#endif
    *a=NULL;
  }
}

/*2
* r = 0
*/
void nlNew (number * r)
{
  *r=NULL;
}

/*2
* result->z =gcd(a->z,b->z)
*/
number nlGcd(number a, number b)
{
  number result=(number)Alloc(sizeof(rnumber));

  result->s = 0;
  nlGcdLong(a->z,b->z,&result->z);
#ifdef LDEBUG
  nlTest(result);
#endif
  return result;
}

/*2
* result->z = lcm(a->z,b->n)
*/
number nlLcm(number a, number b)
{
  lint t,bt;
  number result=(number)Alloc(sizeof(rnumber));

  result->s = 0;
  if (NLISINT(b))
  {
    result->z = nlCopyLint(a->z);
  }
  else
  {
    nlGcdLong(a->z,b->n,&t);
    if (!nlIsOneLint(t))
    {
      bt = nlCopyLint(b->n);
      nlQuotient(&bt,t);
      result->z = nlMultLong(a->z,bt);
      nlDeleteLint(bt);
    }
    else
    {
      result->z = nlMultLong(a->z,b->n);
    }
    nlDeleteLint(t);
  }
#ifdef LDEBUG
  nlTest(result);
#endif
  return result;
}

/*2
* a == 0 ?
*/
BOOLEAN nlIsZero (number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  return (a == NULL);
}

/*2
* a = b ?
*/
BOOLEAN nlEqual (number a, number b)
{
  number h;

  if(a==NULL)
    return (b==NULL);
  if(b==NULL)
    return(a==NULL);
  if(NLISINT(a) && NLISINT(b))
  {
    return !nlComp(a->z,b->z);
  }
  h = nlSub(a, b);
  if (nlIsZero(h))
  {
    nlDelete(&h);
    return TRUE;
  }
  else
  {
    nlDelete(&h);
    return FALSE;
  }
}

/*2
* a == 1 ?
*/
BOOLEAN nlIsOne (number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  if(nlIsZero(a) || NLSIGN(a->z))
  {
    return FALSE;
  }
  if(NLISINT(a))
  {
    return ((a->z[1]==1) && (a->z[0]==1));
  }
  if (nlCompAbs(a->z, a->n))
  {
    return FALSE;
  }
  nlDeleteLint(a->z);
  nlDeleteLint(a->n);
  a->z = nlInitOne();
  a->s = 0;
  return TRUE;
}

/*2
* a == -1 ?
*/
BOOLEAN nlIsMOne (number a)
{
#ifdef LDEBUG
  nlTest(a);
#endif
  if(nlIsZero(a) || !NLSIGN(a->z))
  {
    return FALSE;
  }
  if(NLISINT(a))
  {
    return ((a->z[1]==1) && (a->z[0]==SIGNUM+1));
  }
  if (nlCompAbs(a->z, a->n))
  {
    return FALSE;
  }
  nlDeleteLint(a->z);
  nlDeleteLint(a->n);
  a->z = nlInitOne();
  a->z[0] += SIGNUM;
  a->s = 0;
  return TRUE;
}

/*2
* z := i
*/
number nlInit (int i)
{
  lint nom;
  BOOLEAN neg = FALSE;
  number z = NULL;

  if (i!=0)
  {
    z = (number)Alloc(sizeof(rnumber));
    z->s = 0;
    if (i < 0)
    {
      neg = TRUE;
      i = -i;
    }
    if (i>=BASIS)  // init a 4 byte number
    {
      nom = nlAllocLint(2);
      nom[1] = (int16) (MOD_BASIS&i);
      nom[2] = (int16) (i>>SHIFT_BASIS);
    }
    else           // init a 2 byte number
    {
      nom = nlAllocLint(1);
      nom[1] = (int16) i;
    }
    if (neg)
    {
      *nom += SIGNUM;
    }
    z->z = nom;
#ifdef LDEBUG
    nlTest(z);
#endif
  }
  return z;
}

/*2
* convert number to int
*/
int nlInt(number &a)
{
  lint x;
  int res, i;

  if (nlIsZero(a))
  {
    return 0;
  }
  nlNormalize(a);
  if (NLISRAT(a))
  {
    return 0;
  }
  x = a->z;
  i = NLLENGTH(x);
  if (i > 2)
  {
    return 0;
  }
  res = x[1];
  if (i == 2)
  {
    if (x[2] < SIGNUM)
    {
      res += BASIS*x[2];
    }
    else
    {
      return 0;
    }
  }
  if (NLSIGN(x))
  {
    return -res;
  }
  else
  {
    return res;
  }
}

/*2
* simplify x
*/
void nlNormalize (number &x)
{
  lint zh, nh, d;

  if ((x==NULL)||(x->s>=0))
  {
    return;
  }
  x->s = 1;
  zh = x->z;
  nh = x->n;
  nlGcdLong(zh, nh, &d);
  if (!nlIsOneLint(d))
  {
    nlQuotient(&zh,d);
    x->z = zh;
    nlQuotient(&nh,d);
    if (nlIsOneLint(nh))
    {
      nlDeleteLint(nh);
      x->s = 0;
    }
    else
    {
      x->n = nh;
    }
  }
  nlDeleteLint(d);
#ifdef LDEBUG
  nlTest(x);
#endif
}

/*2
* za >= 0 ?
*/
BOOLEAN nlGreaterZero (number a)
{
  if (nlIsZero(a))
  {
    return TRUE;
  }
  return !NLSIGN(a->z);
}

/*2
* a := -a
*/
number nlNeg (number a)
{
  if (!nlIsZero(a))
  {
    a->z[0] = NLNEG(a->z);
  }
  return a;
}

/*
* 1/a
*/
number nlInvers(number a)
{
  number n=(number)Alloc(sizeof(rnumber));

  if (nlIsZero(a))
  {
    WerrorS("div. 1/0");
    Free((ADDRESS)n,sizeof(rnumber));
    return NULL;
  }
  nlNormalize(a);
  if(NLISRAT(a))
  {
    n->z = nlCopyLint(a->n);
  }
  else
  {
    n->z = nlInitOne();
  }
  if((a->z[1]!=1) || (NLLENGTH(a->z)!=1))
  {
    n->n = nlCopyLint(a->z);
    n->s = 1;
    if(n->n[0]>SIGNUM)
    {
      n->n[0] -= SIGNUM;
      n->z[0] += SIGNUM;
    }
  }
  else
  {
    if (NLSIGN(a->z))
    {
      n->z[0] += SIGNUM;
    }
    n->s = 0;
  }
#ifdef LDEBUG
  nlTest(n);
#endif
  return n;
}

/*2
* copy a to b
*/
number nlCopy(number a)
{
  number b=(number)Alloc0(sizeof(rnumber));

  if (!nlIsZero(a))
  {
    b->s = a->s;
    b->z = nlCopyLint(a->z);
    if (NLISRAT(a))
    {
      b->n = nlCopyLint(a->n);
    }
#ifdef LDEBUG
    nlTest(b);
#endif
    return b;
  }
  else
  {
    Free((ADDRESS)b,sizeof(rnumber));
    return NULL;
  }
}

/*2
* lu:= la + li
*/
number nlAdd (number la, number li)
{
  number lu;
  lint x, y;
  BOOLEAN a0 = nlIsZero(la), i0 = nlIsZero(li);

  if (a0 || i0)
  {
    if (a0)
    {
      return nlCopy(li);
    }
    else
    {
      return nlCopy(la);
    }
  }
  lu = (number)Alloc(sizeof(rnumber));
  if (NLISINT(la))
  {
    if (NLISINT(li))
    {
      lu->z = nlAddLong(la->z, li->z);
      if (lu->z==NULL)
      {
        Free((ADDRESS)lu,sizeof(rnumber));
        return NULL;
      }
      lu->s = 0;
#ifdef LDEBUG
      nlTest(lu);
#endif
      return lu;
    }
    else
    {
      x = nlMultLong(la->z, li->n);
      lu->z = nlAddLong(x, li->z);
      nlDeleteLint(x);
      lu->n = nlCopyLint(li->n);
    }
  }
  else
  {
    if (NLISINT(li))
    {
      y = nlMultLong(li->z, la->n);
      lu->z = nlAddLong(la->z, y);
      nlDeleteLint(y);
      lu->n = nlCopyLint(la->n);
    }
    else
    {
      x = nlMultLong(la->z, li->n);
      y = nlMultLong(li->z, la->n);
      lu->z = nlAddLong(x, y);
      nlDeleteLint(x);
      nlDeleteLint(y);
      lu->n = nlMultLong(la->n, li->n);
    }
  }
  if (lu->z==NULL)
  {
    nlDeleteLint(lu->n);
    Free((ADDRESS)lu,sizeof(rnumber));
    return NULL;
  }
  lu->s = -1;
#ifdef LDEBUG
  nlTest(lu);
#endif
  return lu;
}

/*2
* lu:= la - li
*/
number nlSub (number la, number li)
{
  number lu;
  lint x, y;
  BOOLEAN a0 = nlIsZero(la), i0 = nlIsZero(li);

  if (a0 || i0)
  {
    if (a0)
    {
      lu = nlCopy(li);
      lu = nlNeg(lu);
      return lu;
    }
    else
    {
      return nlCopy(la);
    }
  }
  lu = (number)Alloc(sizeof(rnumber));
  if (NLISINT(la))
  {
    if (NLISINT(li))
    {
      lu->z = nlSubLong(la->z, li->z);
      if (lu->z==NULL)
      {
        Free((ADDRESS)lu,sizeof(rnumber));
        return NULL;
      }
      lu->s = 0;
#ifdef LDEBUG
      nlTest(lu);
#endif
      return lu;
    }
    else
    {
      x = nlMultLong(la->z, li->n);
      lu->z = nlSubLong(x, li->z);
      nlDeleteLint(x);
      lu->n = nlCopyLint(li->n);
    }
  }
  else
  {
    if (NLISINT(li))
    {
      y = nlMultLong(li->z, la->n);
      lu->z = nlSubLong(la->z, y);
      nlDeleteLint(y);
      lu->n = nlCopyLint(la->n);
    }
    else
    {
      x = nlMultLong(la->z, li->n);
      y = nlMultLong(li->z, la->n);
      lu->z = nlSubLong(x, y);
      nlDeleteLint(x);
      nlDeleteLint(y);
      lu->n = nlMultLong(la->n, li->n);
    }
  }
  if (lu->z==NULL)
  {
    nlDeleteLint(lu->n);
    Free((ADDRESS)lu,sizeof(rnumber));
    return NULL;
  }
  lu->s = -1;
#ifdef LDEBUG
  nlTest(lu);
#endif
  return lu;
}

/*2
* lo := la * li
*/
number nlMult (number la, number li)
{
  number lo=(number)Alloc(sizeof(rnumber));

  if (nlIsZero(la) || nlIsZero(li))
  {
    Free((ADDRESS)lo,sizeof(rnumber));
    return NULL;
  }
  lo->z = nlMultLong(la->z,li->z);
  if(NLISINT(la))
  {
    if(NLISINT(li))
    {
      lo->s = 0;
#ifdef LDEBUG
      nlTest(lo);
#endif
      return lo;
    }
    else
    {
      lo->n = nlCopyLint(li->n);
    }
  }
  else
  {
    if(NLISINT(li))
    {
      lo->n = nlCopyLint(la->n);
    }
    else
    {
      lo->n = nlMultLong(la->n,li->n);
    }
  }
  lo->s = -1;
#ifdef LDEBUG
  nlTest(lo);
#endif
  return lo;
}

/*2
* lo := la / li in Z
* with |li| > la - (lo * li) >= 0
*/
number nlIntDiv (number la, number li)
{
  int c;
  number lo=(number)Alloc(sizeof(rnumber));

  if (nlIsZero(la) || nlIsZero(li))
  {
    if (nlIsZero(li))
    {
      WerrorS("div. by 0");
    }
    Free((ADDRESS)lo,sizeof(rnumber));
    return NULL;
  }
  lo->s = 0;
  c = nlCompAbs(li->z, la->z);
  if (c >= 0)
  {
    if (c == 0)
    {
      lo->z = nlInitOne();
      if (NLSIGN(la->z) != NLSIGN(li->z))
      {
        lo->z[0] += SIGNUM;
      }
    }
    else if (NLSIGN(la->z))
    {
      lo->z = nlInitOne();
      if (!NLSIGN(li->z))
      {
        lo->z[0] += SIGNUM;
      }
    }
    else
    {
      Free((ADDRESS)lo,sizeof(rnumber));
      return NULL;
    }
#ifdef LDEBUG
    nlTest(lo);
#endif
    return lo;
  }
  lo->z = nlCopyLint(la->z);
  if (NLLENGTH(la->z)==1)
  {
    lo->z[1] /= li->z[1];
    if (NLSIGN(la->z))
    {
      int16 h = la->z[1] - (lo->z[1]*li->z[1]);
      if (h!=0)
      {
        lo->z[1]++;
      }
    }
    if (NLSIGN(li->z))
    {
      lo->z[0] = NLNEG(lo->z);
    }
#ifdef LDEBUG
    nlTest(lo);
#endif
    return lo;
  }
  if ((li->z[1]!=1) || (NLLENGTH(li->z)>1))
  {
    nlQuotient(&lo->z, li->z);
    if (NLSIGN(la->z))
    {
      lint o1, t, h = nlMultLong(lo->z, li->z);
      if (nlCompAbs(h, la->z))
      {
        o1 = nlInitOne();
        if (NLSIGN(li->z))
        {
          t = nlAddLong(lo->z,o1);
        }
        else
        {
          t = nlSubLong(lo->z,o1);
        }
        nlDeleteLint(o1);
        nlDeleteLint(lo->z);
        lo->z = t;
      }
      nlDeleteLint(h);
    }
  }
  else if (li->z[0] == (SIGNUM+1))
  {
    lo->z[0] = NLNEG(lo->z);
  }
#ifdef LDEBUG
  nlTest(lo);
#endif
  return lo;
}

/*2
* lo := la mod li == la - ((la / li) * li)
* with |li| > lo >= 0
*/
number nlIntMod (number la, number li)
{
  int16 xl, yl;
  int c;
  lint r, t;
  number lo=(number)Alloc(sizeof(rnumber));

  if (nlIsZero(la) || nlIsZero(li))
  {
    if (nlIsZero(li))
    {
      WerrorS("mod. by 0");
    }
    Free((ADDRESS)lo,sizeof(rnumber));
    return NULL;
  }
  c = nlCompAbs(li->z, la->z);
  if (c == 0)
  {
    Free((ADDRESS)lo,sizeof(rnumber));
    return NULL;
  }
  if (c > 0)
  {
    if (NLSIGN(la->z))
    {
      if (NLSIGN(li->z))
      {
        lo->z = nlSubLong(la->z,li->z);
      }
      else
      {
        lo->z = nlAddLong(la->z,li->z);
      }
    }
    else
    {
      lo->z = nlCopyLint(la->z);
    }
#ifdef LDEBUG
    nlTest(lo);
#endif
    return lo;
  }
  r = nlCopyLint(la->z);
  xl = NLLENGTH(r);
  if (xl == 1)
  {
    yl = r[1] / li->z[1];
    r[1] -= yl * li->z[1];
    if (r[1] != 0)
    {
      if (NLSIGN(la->z))
      {
        r[1] = li->z[1]-r[1];
      }
      lo->z = r;
#ifdef LDEBUG
      nlTest(lo);
#endif
      return lo;
    }
    else
    {
      nlDeleteLint(r);
      Free((ADDRESS)lo,sizeof(rnumber));
      return NULL;
    }
  }
  yl = NLLENGTH(li->z);
  if (yl > 1)
  {
    nlRemLong(r,li->z,xl,yl,&c);
    xl = c;
    if (xl != 0)
    {
      nlAllocLint(xl);
      for (c=xl; c>0; c--)
      {
        t[c] = r[c];
      }
    }
  }
  else if (li->z[1]!=1)
  {
    nlRemSmall(r,xl,li->z[1],&xl);
    if (xl != 0)
    {
      t = nlAllocLint(1);
      t[1] = xl;
    }
  }
  else
  {
    xl = 0;
  }
  nlDeleteLint(r);
  if (xl == 0)
  {
    Free((ADDRESS)lo,sizeof(rnumber));
    return NULL;
  }
  if (NLSIGN(la->z))
  {
    if (NLSIGN(li->z))
    {
      r = nlSubLong(t,li->z);
    }
    else
    {
      r = nlSubLong(li->z,t);
    }
    nlDeleteLint(t);
    t = r;
  }
  lo->z = t;
#ifdef LDEBUG
  nlTest(lo);
#endif
  return lo;
}

/*2
* lo := la / li
*/
number nlDiv (number la, number li)
{
  number lo=(number)Alloc(sizeof(rnumber));

  if (nlIsZero(la) || nlIsZero(li))
  {
    if (nlIsZero(li))
    {
      WerrorS("div. by 0");
    }
    Free((ADDRESS)lo,sizeof(rnumber));
    return NULL;
  }
  if (NLISINT(la))
  {
    lo->n = nlCopyLint(li->z);
  }
  else
  {
    lo->n = nlMultLong(la->n, li->z);
  }
  if (NLISINT(li))
  {
    lo->z = nlCopyLint(la->z);
  }
  else
  {
    lo->z = nlMultLong(la->z, li->n);
  }
  if(NLSIGN(lo->n))
  {
    lo->n[0] -= SIGNUM;
    lo->z[0] = NLNEG(lo->z);
  }
  lo->s = -1;
  if(nlIsOneLint(lo->n))
  {
    nlDeleteLint(lo->n);
    lo->s = 0;
  }
#ifdef LDEBUG
  nlTest(lo);
#endif
  return lo;
}

/*3
* Power of long; lu:= x^exp
*/
static void nlPow (lint x,int exp,lint *lu)
{
  lint yh, y, w;
  y = nlInitOne();
  w = nlCopyLint(x);
  while (exp)
  {
    if (exp%2 != 0)
    {
      yh = nlMultLong(y,w);
      nlDeleteLint(y);
      y = yh;
    }
    if (exp > 1)
    {
      yh = nlMultLong(w,w);
      nlDeleteLint(w);
      w = yh;
    }
    exp = exp / 2;
  }
  *lu = y;
}

/*2
* lu:= x ^ exp
*/
void nlPower (number x,int exp,number * lu)
{
  number b=NULL;

  if (!nlIsZero(x))
  {
    b = (number)Alloc(sizeof(rnumber));
    if (exp == 0)
    {
      b->z = nlInitOne();
      *lu = b;
#ifdef LDEBUG
      nlTest(*lu);
#endif
      return;
    }
    nlNormalize(x);
    if (exp > 0)
    {
      b->s = x->s;
      if ((x->z[1]!=1) || (NLLENGTH(x->z)>1))
      {
        nlPow(x->z,exp,&b->z);
      }
      else
      {
        b->z = nlInitOne();
      }
      if (NLISRAT(x))
      {
        nlPow(x->n,exp,&b->n);
      }
    }
    else
    {
      if ((x->z[1]!=1) || (NLLENGTH(x->z)>1))
      {
        b->s = 1;
        nlPow(x->z,exp,&b->n);
        if (NLSIGN(b->n))
        {
          b->n[0] -= SIGNUM;
        }
      }
      else
      {
        b->s = 0;
      }
      if (NLISRAT(x))
      {
        nlPow(x->n,exp,&b->z);
      }
      else
      {
        b->z = nlInitOne();
      }
    }
    if (exp%2 != 0) // make sign
    {
      if(NLSIGN(b->z) != NLSIGN(x->z))
      {
        b->z[0] = NLNEG(b->z);
      }
    }
    else
    {
      if(NLSIGN(b->z))
      {
        b->z[0] -= SIGNUM;
      }
    }
  }
  *lu = b;
#ifdef LDEBUG
  nlTest(*lu);
#endif
}

/*2
* a > b ?
*/
BOOLEAN nlGreater (number a, number b)
{
  number r;
  BOOLEAN rr;

  r=nlSub(a,b);
  rr=(!nlIsZero(r)) && (nlGreaterZero(r));
  nlDelete(&r);
  return rr;
}

/*3
* a := b * a + r
*/
void nlDivMod(lint a, int16 al, int16 b, int16 * r)
{
  int32 da;
  const int32 db = (int32)b;
  int16 a1 = a[al];

  a[al] /= b;
  a1 -= b*a[al];
  loop
  {
    al--;
    if (al==0)
    {
      *r = a1;
      return;
    }
    while (a1==0)
    {
      a1 = a[al];
      a[al] /= b;
      a1 -= b*a[al];
      al--;
      if (al==0)
      {
        *r = a1;
        return;
      }
    }
    da = BASIS*(int32) a1 + (int32) a[al];
    a[al] = da / db;
    da -= db*a[al];
    a1 = (int16)da;
  }
}

/*2
* returns (n->z mod p) / (n->n mod p)
*/
int nlModP(number n, int p)
{
  number a;
  int16 iz, in;
  int jz, jn, al;

  if (nlIsZero(n))
  {
    return 0;
  }
  a = nCopy(n);
  al = NLLENGTH(a->z);
  if (al == 1)
  {
    iz = (a->z[1])%p;
  }
  else
  {
    nlRemSmall(a->z,al,p,&iz);
  }
  jz = iz;
  if (NLSIGN(a->z))
  {
    jz = p-jz;
  }
  if (NLISRAT(a))
  {
    al = *(a->n);
    if (al == 1)
    {
      in = (a->n[1])%p;
    }
    else
    {
      nlRemSmall(a->n,al,p,&in);
    }
    jn = in;
    jz = (int)npDiv((number)jz,(number)jn);
  }
  nlDelete(&a);
  return jz;
}

int nlSize(number a)
{
  /*to be implemtented*/ return 0;
}
#endif

