/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: n-tupel of coeffcients
*/



#include "misc/auxiliary.h"
#include "misc/mylimits.h"

#include "reporter/reporter.h"

#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"
#include "coeffs/mpr_complex.h"

#include "coeffs/ntupel.h"
#include "coeffs/modulop.h"
#include "coeffs/longrat.h"

typedef coeffs *coeffs_array;
typedef number *number_array;

static void nnCoeffWrite(const coeffs r, BOOLEAN b)
{
  int i=0;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    d[i]->cfCoeffWrite(d[i],b);
    i++;
    if (d[i]==NULL) break;
    PrintS(" x ");
  }
}


static BOOLEAN nnGreaterZero (number k, const coeffs r)
{
  int i=0;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    if (d[i]->type==n_R) return d[i]->cfGreaterZero(k,d[i]);
    if (d[i]->type==n_long_R) return d[i]->cfGreaterZero(k,d[i]);
    i++;
    if (d[i]==NULL) return d[i-1]->cfGreaterZero(k,d[i-1]);
  }
}

static number nnMult (number a,number b, const coeffs r)
{
  number_array A=(number_array)a;
  number_array B=(number_array)b;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfMult(A[i],B[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

/*2
* create a number from int
*/
static number nnInit (long l, const coeffs r)
{
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfInit(l,d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

/*2
* convert a number to int
*/
static long nnInt(number &n, const coeffs r)
{
  int i=0;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    if (d[i]->type==n_R) return d[i]->cfInt(n,d[i]);
    if (d[i]->type==n_long_R) return d[i]->cfInt(n,d[i]);
    i++;
    if (d[i]==NULL) return d[i-1]->cfInt(n,d[i-1]);
  }
}

static int nnSize(number n, const coeffs r)
{
  int s=0;
  int i=0;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    if (d[i]->type==n_R) s+=d[i]->cfSize(n,d[i]);
    else if (d[i]->type==n_long_R) s+=d[i]->cfSize(n,d[i]);
    i++;
    if (d[i]==NULL)
    {
      if (s==0) return d[i-1]->cfSize(n,d[i-1]);
      else return s;
    }
  }
}

static number nnCopy (number a, const coeffs r)
{
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfCopy(A[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

static number nnAdd (number a, number b, const coeffs r)
{
  number_array A=(number_array)a;
  number_array B=(number_array)b;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfAdd(A[i],B[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

static number nnSub (number a, number b, const coeffs r)
{
  number_array A=(number_array)a;
  number_array B=(number_array)b;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfSub(A[i],B[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

static BOOLEAN nnIsZero (number  a, const coeffs r)
{
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    if (!d[i]->cfIsZero(A[i],d[i])) return FALSE;
    i++;
    if (d[i]==NULL) return TRUE;
  }
}

static BOOLEAN nnIsOne (number a, const coeffs r)
{
  int i=0;
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    if (!d[i]->cfIsOne(A[i],d[i])) return FALSE;
    i++;
    if (d[i]==NULL) return TRUE;
  }
}

static BOOLEAN nnIsMOne (number a, const coeffs r)
{
  int i=0;
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    if (!d[i]->cfIsMOne(A[i],d[i])) return FALSE;
    i++;
    if (d[i]==NULL) return TRUE;
  }
}

static number nnDiv (number a,number b, const coeffs r)
{
  if (nnIsZero(b,r))
  {
    WerrorS(nDivBy0);
    return nnInit(0,r);
  }
  number_array A=(number_array)a;
  number_array B=(number_array)b;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfDiv(A[i],B[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

static number nnInvers (number a, const coeffs r)
{
  if (nnIsZero(a,r))
  {
    WerrorS(nDivBy0);
    return nnInit(0,r);
  }
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  i=0;
  loop
  {
    C[i]=d[i]->cfInvers(A[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

static number nnInpNeg (number a, const coeffs r)
{
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  i=0;
  loop
  {
    A[i]=d[i]->cfInpNeg(A[i],d[i]);
    i++;
    if (d[i]==NULL) return (number)A;
  }
}

static BOOLEAN nnGreater (number a,number b, const coeffs r)
{
  number_array A=(number_array)a;
  number_array B=(number_array)b;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    if (d[i]->type==n_R) return d[i]->cfGreater(A[i],B[i],d[i]);
    else if (d[i]->type==n_long_R) return d[i]->cfGreater(A[i],B[i],d[i]);
    i++;
    if (d[i]==NULL)
    {
      return d[i-1]->cfGreater(A[i],B[i],d[i-1]);
    }
  }
}

static BOOLEAN nnEqual (number a,number b, const coeffs r)
{
  int i=0;
  number_array A=(number_array)a;
  number_array B=(number_array)b;
  coeffs_array d=(coeffs_array)r->data;
  loop
  {
    if (!d[i]->cfEqual(A[i],B[i],d[i])) return FALSE;
    i++;
    if (d[i]==NULL) return TRUE;
  }
}

static void nnWriteShort (number a, const coeffs r)
{
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    if (d[i]->type==n_R) { d[i]->cfWriteLong(A[i],d[i]); return; }
    else if (d[i]->type==n_long_R) {d[i]->cfWriteLong(A[i],d[i]); return; }
    i++;
    if (d[i]==NULL) { d[i-1]->cfWriteLong(A[i-1],d[i-1]); return; }
  }
}

static void nnWriteLong (number a, const coeffs r)
{
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  StringSetS("(");
  loop
  {
    d[i]->cfWriteLong(A[i],d[i]);
    i++;
    if (d[i]==NULL) {StringAppendS(")"); return; }
    else StringAppendS(",");
  }
}

#if 0
void nrPower (number a, int i, number * result, const coeffs r)
{
  assume( getCoeffType(r) == n_R );

  if (i==0)
  {
    *result = nf(nf(1.0).F()).N();
    return;
  }
  if (i==1)
  {
    *result = nf(nf(a).F()).N();
    return;
  }
  nrPower(a,i-1,result,r);
  *result = nf(nf(a).F() * nf(*result).F()).N();
}
#endif

static const char * nnRead (const char *s, number *a, const coeffs r)
{
  // reading R/longR part, setting everything else to random
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc0(i*sizeof(number));
  *a=(number)C;
  int is_zero=2; /*1: read 0, 2: read <> 0 */
  int reader=-1;
  i=0;
  loop
  {
    if ((d[i]->type==n_R)
    || (d[i]->type==n_long_R))
    {
      s=d[i]->cfRead(s,&(C[i]),d[i]);
      is_zero=d[i]->cfIsZero(C[i],d[i]);
      reader=i;
      break;
    }
    i++;
    if (d[i]==NULL)
    {
      s=d[i-1]->cfRead(s,&(C[i-1]),d[i-1]);
      is_zero=d[i-1]->cfIsZero(C[i-1],d[i-1]);
      reader=i-1;
      break;
    }
  }
  if (is_zero==0)
  { // seting random values!=0 for Z/p
    i=0;
    loop
    {
      if (i!=reader)
      {
        if (d[i]->type==n_Zp)
        {
          int l=0;
          do
          {
            l=siRand();
          } while(l%(d[i]->ch)==0);
          C[i]=d[i]->cfInit(l,d[i]);
        }
        else if ((d[i]->type==n_R)
        || (d[i]->type==n_long_R))
        {
          nMapFunc nMap=d[i]->cfSetMap(d[reader],d[i]);
          C[i]=nMap(C[reader],d[reader],d[i]);
        }
        else
        {
          WerrorS("reading is not suppiorted for such compinations of coeffs");
          break;
        }
      }
      i++;
      if (d[i]==NULL) break;
    }
  }
  else if (is_zero==1)
  { // read a zero
    i=0;
    loop
    {
      C[i]=d[i]->cfInit(0,d[i]);
      i++;
      if (d[i]==NULL) break;
    }
  }
  else if (reader==-1)/* not R or long_R*/
  {
    s=d[0]->cfRead(s,&(C[0]),d[0]);
    i=1;
    loop
    {
      if (d[i]==NULL) break;
      i++;
      {
        nMapFunc nMap=d[i]->cfSetMap(d[0],d[i]);
        C[i]=nMap(C[0],d[0],d[i]);
      }
    }
  }
  else
  {
    WerrorS("nnRead: should not happen");
  }
  return s;
}

#ifdef LDEBUG
/*2
* test valid numbers: not implemented yet
*/
BOOLEAN  nnDBTest(number a, const char *f, const int l, const coeffs r)
{
  BOOLEAN bo=FALSE;
  number_array A=(number_array)a;
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    bo&=d[i]->cfDBTest(A[i],f,l,d[i]);
    i++;
    if (d[i]==NULL) return bo;
  }
}
#endif

static number nnMap(number from, const coeffs aRing, const coeffs r)
{
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  loop
  {
    i++;
    if (d[i]==NULL) break;
  }
  number *C=(number*)omAlloc(i*sizeof(number));
  nMapFunc nMap;
  i=0;
  loop
  {
    nMap=d[i]->cfSetMap(aRing,d[i]);
    if (nMap==NULL) Werror("no map for compoment %d",i);
    else C[i]=nMap(from,aRing,d[i]);
    i++;
    if (d[i]==NULL) return (number)C;
  }
}

nMapFunc nnSetMap(const coeffs src, const coeffs dst)
{
  return nnMap;
}

static char* nnCoeffString(const coeffs r)
{
  #define SI_BUF_SIZE 1024
  char buf[SI_BUF_SIZE];
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  buf[0]='\0';
  loop
  {
    char *s=d[i]->cfCoeffName(d[i]);
    strncat(buf,s, SI_BUF_SIZE-strlen(buf)-strlen(s)-1);
    i++;
    if (d[i]==NULL) break;
    strncat(buf," x ", SI_BUF_SIZE-strlen(buf)-4);
  }
  return omStrDup(buf);
}

typedef char si_char_SI_BUF_SIZE[SI_BUF_SIZE];
STATIC_VAR si_char_SI_BUF_SIZE nnCoeffName_buf;
static char* nnCoeffName(const coeffs r)
{
  coeffs_array d=(coeffs_array)r->data;
  int i=0;
  nnCoeffName_buf[0]='\0';
  loop
  {
    char *s=d[i]->cfCoeffName(d[i]);
    strncat(nnCoeffName_buf,s, SI_BUF_SIZE-strlen(nnCoeffName_buf)-strlen(s)-1);
    i++;
    if (d[i]==NULL) break;
    strncat(nnCoeffName_buf," x ", SI_BUF_SIZE-strlen(nnCoeffName_buf)-4);
  }
  return nnCoeffName_buf;
}

static void nnDelete (number * a, const coeffs r)
{
  if (*a!=NULL)
  {
    coeffs_array d=(coeffs_array)r->data;
    number_array A=(number_array)(*a);
    int i=0;
    loop
    {
      d[i]->cfDelete(&(A[i]),d[i]);
      i++;
      if (d[i]==NULL) break;
    }
    omFreeSize(d,i*sizeof(number));
    *a=NULL;
  }
}


static number nnChineseRemainder(number *x, number *q,int rl, BOOLEAN sym, CFArray &inv_cache,const coeffs CF)
{
  WerrorS("not yet");
  return NULL;
}


BOOLEAN nnInitChar(coeffs n, void* p)
{
  n->data=p;
  n->type=n_nTupel;
  n->is_field=TRUE;
  n->is_domain=TRUE;

  n->ch = 0;
  n->cfCoeffString = nnCoeffString;
  n->cfCoeffName = nnCoeffName;

  n->cfInit = nnInit;
  n->cfDelete = nnDelete;
  n->cfInt  = nnInt;
  n->cfAdd  = nnAdd;
  n->cfSub  = nnSub;
  n->cfMult = nnMult;
  n->cfDiv  = nnDiv;
  n->cfExactDiv= nnDiv;
  n->cfInpNeg   = nnInpNeg;
  n->cfInvers= nnInvers;
  n->cfCopy  = nnCopy;
  n->cfGreater = nnGreater;
  n->cfEqual = nnEqual;
  n->cfIsZero = nnIsZero;
  n->cfIsOne = nnIsOne;
  n->cfIsMOne = nnIsMOne;
  n->cfGreaterZero = nnGreaterZero;
  n->cfWriteShort = nnWriteShort;
  n->cfWriteLong = nnWriteLong;
  n->cfRead = nnRead;
  //n->cfPower = nnPower;
  n->cfSetMap = nnSetMap;
  n->cfCoeffWrite  = nnCoeffWrite;

  n->cfChineseRemainder=nnChineseRemainder;
  n->cfSize  = nnSize;
#ifdef LDEBUG
  n->cfDBTest=nnDBTest;
#endif
  //n->nCoeffIsEqual = ndCoeffIsEqual;
  return FALSE;
}
