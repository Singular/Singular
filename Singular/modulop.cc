/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: modulop.cc,v 1.22 2000-12-06 11:03:20 Singular Exp $ */
/*
* ABSTRACT: numbers modulo p (<=32003)
*/

#include <limits.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "omalloc.h"
#include "numbers.h"
#include "longrat.h"
#include "ring.h"
#include "modulop.h"

int npPrimeM=0;
int npGen=0;
int npPminus1M=0;
int npMapPrime;

CARDINAL *npExpTable=NULL;
CARDINAL *npLogTable=NULL;


BOOLEAN npGreaterZero (number k)
{
  int h = (int) k;
  return ((int)h !=0) && (h <= (npPrimeM>>1));
}

unsigned long npMultMod(unsigned long a, unsigned long b)
{
  unsigned long c = a*b;
  c = c % npPrimeM;
  assume(c == (unsigned long) npMultM((number) a, (number) b));
  return c;
}

number npMult (number a,number b)
{
  if (((int)a == 0) || ((int)b == 0))
    return (number)0;
  else
  {
    return npMultM(a,b);
  }
}

/*2
* create a number from int
*/
number npInit (int i)
{
  while (i <  0)                   i += npPrimeM;
  while ((i>1) && (i >= npPrimeM)) i -= npPrimeM;
  return (number)i;
}

/*2
* convert a number to int (-p/2 .. p/2)
*/
int npInt(number &n)
{
  if ((int)n > (npPrimeM >>1)) return ((int)n -npPrimeM);
  else                     return (int)n;
}

number npAdd (number a, number b)
{
  return npAddM(a,b);
}

number npSub (number a, number b)
{
  return npSubM(a,b);
}

BOOLEAN npIsZero (number  a)
{
  return 0 == (int)a;
}

BOOLEAN npIsOne (number a)
{
  return 1 == (int)a;
}

BOOLEAN npIsMOne (number a)
{
  return ((npPminus1M == (int)a)&&(1!=(int)a));
}

number npDiv (number a,number b)
{
  if ((int)a==0)
    return (number)0;
  else if ((int)b==0)
  {
    WerrorS("div by 0");
    return (number)0;
  }
  else
  {
    int s = npLogTable[(int)a] - npLogTable[(int)b];
    if (s < 0)
      s += npPminus1M;
    return (number)npExpTable[s];
  }
}

number  npInvers (number c)
{
  if ((int)c==0)
  {
    WerrorS("1/0");
    return (number)0;
  }
  return (number)npExpTable[npPminus1M - npLogTable[(int)c]];
}

number npNeg (number c)
{
  if ((int)c==0) return c;
  return npNegM(c);
}

BOOLEAN npGreater (number a,number b)
{
  return (int)a != (int)b;
}

BOOLEAN npEqual (number a,number b)
{
//  return (int)a == (int)b;
  return npEqualM(a,b);
}

void npWrite (number &a)
{
  if ((int)a > (npPrimeM >>1)) StringAppend("-%d",npPrimeM-((int)a));
  else                     StringAppend("%d",(int)a);
}

void npPower (number a, int i, number * result)
{
  if (i==0)
  {
    //npInit(1,result);
    *(int *)result = 1;
  }
  else if (i==1)
  {
    *result = a;
  }
  else
  {
    npPower(a,i-1,result);
    *result = npMultM(a,*result);
  }
}

char* npEati(char *s, int *i)
{

  if (((*s) >= '0') && ((*s) <= '9'))
  {
    (*i) = 0;
    do
    {
      (*i) *= 10;
      (*i) += *s++ - '0';
      if ((*i) >= (MAX_INT_VAL / 10)) (*i) = (*i) % npPrimeM;
    }
    while (((*s) >= '0') && ((*s) <= '9'));
    if ((*i) >= npPrimeM) (*i) = (*i) % npPrimeM;
  }
  else (*i) = 1;
  return s;
}

char * npRead (char *s, number *a)
{
  int z;
  int n=1;

  s = npEati(s, &z);
  if ((*s) == '/')
  {
    s++;
    s = npEati(s, &n);
  }
  *a = npDiv((number)z,(number)n);
  return s;
}

/*2
* the last used charcteristic
*/
//int npGetChar()
//{
//  return npPrimeM;
//}

/*2
* set the charcteristic (allocate and init tables)
*/

void npSetChar(int c, ring r)
{
//  int i, w;

//  if (c==npPrimeM) return;
//  if (npPrimeM > 1)
//  {
//    omFreeSize( (ADDRESS)npExpTable,npPrimeM*sizeof(CARDINAL) );
//    omFreeSize( (ADDRESS)npLogTable,npPrimeM*sizeof(CARDINAL) );
//  }
  if ((c>1) || (c<(-1)))
  {
    if (c>1) npPrimeM = c;
    else     npPrimeM = -c;
    npPminus1M = npPrimeM - 1;
//    npExpTable= (CARDINAL *)omAlloc( npPrimeM*sizeof(CARDINAL) );
//     npLogTable= (CARDINAL *)omAlloc( npPrimeM*sizeof(CARDINAL) );
//     omMarkAsStaticAddr(npExpTable);
//     omMarkAsStaticAddr(npLogTable);
//     memcpy(npExpTable,r->cf->npExpTable,npPrimeM*sizeof(CARDINAL));
//     memcpy(npLogTable,r->cf->npLogTable,npPrimeM*sizeof(CARDINAL));
    npExpTable=r->cf->npExpTable;
    npLogTable=r->cf->npLogTable;
    npGen = npExpTable[1];
  }
  else
  {
    npPrimeM=0;
    npExpTable=NULL;
    npLogTable=NULL;
  }
}

void npInitChar(int c, ring r)
{
  int i, w;

  if ((c>1) || (c<(-1)))
  {
    if (c>1) r->cf->npPrimeM = c;
    else     r->cf->npPrimeM = -c;
    r->cf->npPminus1M = r->cf->npPrimeM - 1;
    r->cf->npExpTable= (CARDINAL *)omAlloc( r->cf->npPrimeM*sizeof(CARDINAL) );
    r->cf->npLogTable= (CARDINAL *)omAlloc( r->cf->npPrimeM*sizeof(CARDINAL) );
    r->cf->npExpTable[0] = 1;
    r->cf->npLogTable[0] = 0;
    if (r->cf->npPrimeM > 2)
    {
      w = 1;
      loop
      {
        r->cf->npLogTable[1] = 0;
        w++;
        i = 0;
        loop
        {
          i++;
          r->cf->npExpTable[i] = (int)(((long)w * (long)r->cf->npExpTable[i-1])
                                 % r->cf->npPrimeM);
          r->cf->npLogTable[r->cf->npExpTable[i]] = i;
          if (/*(i == npPrimeM - 1 ) ||*/ (r->cf->npExpTable[i] == 1))
            break;
        }
        if (i == r->cf->npPrimeM - 1)
          break;
      }
    }
    else
    {
      r->cf->npExpTable[1] = 1;
      r->cf->npLogTable[1] = 0;
    }
  }
  else
  {
    WarnS("nInitChar failed");
  }
}


#ifdef LDEBUG
BOOLEAN npDBTest (number a, char *f, int l)
{
  if (((int)a<0) || ((int)a>npPrimeM))
  {
    return FALSE;
  }
  return TRUE;
}
#endif

number npMap0(number from)
{
  return npInit(nlModP(from,npPrimeM));
}

number npMapP(number from)
{
  int i = (int)from;
  if (i>npMapPrime/2)
  {
    i-=npMapPrime;
    while (i < 0) i+=npPrimeM;
  }
  i%=npPrimeM;
  return (number)i;
}

BOOLEAN npSetMap(ring r)
{
  if (rField_is_Q(r))
  {
    nMap = npMap0;   /*Q -> Z/p*/
    return TRUE;
  }
  if ( rField_is_Zp(r) )
  {
    if (rChar(r) == npPrimeM)
    {
      nMap = ndCopy;  /* Z/p -> Z/p*/
      return TRUE;
    }
    else
    {
      npMapPrime=rChar(r);
      nMap = npMapP; /* Z/p' -> Z/p */
      return TRUE;
    }
  }
  return FALSE;      /* default */
}
