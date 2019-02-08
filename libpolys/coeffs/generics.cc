/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: numbers (integers)
*/

#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "misc/mylimits.h"
#include "reporter/reporter.h"

#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "coeffs/numbers.h"

#include "coeffs/si_gmp.h"

#include "coeffs/generics.h"

#include "coeffs/rintegers.h"

#include <string.h>

struct generic_pair
{
  number a1;
  number a2;
};
typedef struct generic_pair *gcp;
VAR coeffs coeffs1,coeffs2;

static void gCoeffWrite(const coeffs r, BOOLEAN b)
{
  printf("debug: ");
  coeffs1->cfCoeffWrite(coeffs1,b);
}

char* gCoeffString(const coeffs r)
{
  return coeffs1->cfCoeffString(coeffs1);
}
char* gCoeffName(const coeffs r)
{
  return coeffs1->cfCoeffName(coeffs1);
}
void gKillChar(coeffs r)
{
  coeffs1->cfKillChar(coeffs1);
  coeffs2->cfKillChar(coeffs2);
}
void gSetChar(coeffs r)
{
  coeffs1->cfSetChar(coeffs1);
  coeffs2->cfSetChar(coeffs2);
}

static number gMult (number a, number b, const coeffs cf)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfMult(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfMult(aa->a2,bb->a2,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gSub (number a, number b, const coeffs cf)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfSub(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfSub(aa->a2,bb->a2,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gAdd (number a, number b, const coeffs cf)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfAdd(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfAdd(aa->a2,bb->a2,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gDiv (number a, number b, const coeffs cf)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfDiv(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfDiv(aa->a2,bb->a2,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gIntMod (number a, number b, const coeffs cf)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfIntMod(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfIntMod(aa->a2,bb->a2,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gExactDiv (number a, number b, const coeffs cf)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfExactDiv(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfExactDiv(aa->a2,bb->a2,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gInit (long i, const coeffs cf)
{
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfInit(i,coeffs1);
  cc->a2=coeffs2->cfInit(i,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static number gInitMPZ (mpz_t i, const coeffs cf)
{
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfInitMPZ(i,coeffs1);
  cc->a2=coeffs2->cfInitMPZ(i,coeffs2);
  n_Test((number)cc,cf);
  return (number)cc;
}
static int gSize (number a, const coeffs)
{
  gcp aa=(gcp)a;
  int s1=coeffs1->cfSize(aa->a1,coeffs1);
  int s2=coeffs2->cfSize(aa->a2,coeffs2);
  if (s1!=s2)
  {
    printf("gSize: %d, %d\n",s1,s2);
  }
  return s1;
}
static long gInt (number &a, const coeffs)
{
  gcp aa=(gcp)a;
  long s1=coeffs1->cfInt(aa->a1,coeffs1);
  long s2=coeffs2->cfInt(aa->a2,coeffs2);
  if (s1!=s2)
  {
    printf("gInt: %ld, %ld\n",s1,s2);
  }
  return s1;
}
static void gMPZ(mpz_t result, number &n, const coeffs r)
{
  coeffs1->cfMPZ(result,n,coeffs1);
  mpz_t r2;
  coeffs2->cfMPZ(r2,n,coeffs2);
  if(mpz_cmp(result,r2)!=0)
  {
    printf("gMPZ\n");
  }
  mpz_clear(r2);
}
static number  gInpNeg(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  aa->a1=coeffs1->cfInpNeg(aa->a1,coeffs1);
  aa->a2=coeffs2->cfInpNeg(aa->a2,coeffs2);
  return (number)aa;
}
static number  gInvers(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfInvers(aa->a1,coeffs1);
  cc->a2=coeffs2->cfInvers(aa->a2,coeffs2);
  return (number)cc;
}
static number  gCopy(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfCopy(aa->a1,coeffs1);
  cc->a2=coeffs2->cfCopy(aa->a2,coeffs2);
  return (number)cc;
}
static number  gRePart(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfRePart(aa->a1,coeffs1);
  cc->a2=coeffs2->cfRePart(aa->a2,coeffs2);
  return (number)cc;
}
static number  gImPart(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfRePart(aa->a1,coeffs1);
  cc->a2=coeffs2->cfRePart(aa->a2,coeffs2);
  return (number)cc;
}
static void  gWriteLong(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  coeffs1->cfWriteLong(aa->a1,coeffs1);
}
static void  gWriteShort(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  coeffs1->cfWriteShort(aa->a1,coeffs1);
}
static const char *gRead(const char * s, number * a, const coeffs r)
{
  gcp cc=(gcp)omalloc(sizeof(*cc));
  const char* ss=coeffs1->cfRead(s,&(cc->a1),coeffs1);
  number tmp=coeffs2->cfInit(1,coeffs2);
  mpz_ptr tt=(mpz_ptr)tmp;
  coeffs1->cfMPZ(tt,cc->a1,coeffs1);
  cc->a2=(number)tt;
  *a=(number)cc;
  return ss;
}
static void    gNormalize(number &a, const coeffs r)
{
  gcp aa=(gcp)a;
  coeffs1->cfNormalize(aa->a1,coeffs1);
  coeffs2->cfNormalize(aa->a2,coeffs2);
}
static  BOOLEAN gGreater(number a,number b, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  BOOLEAN b1=coeffs1->cfGreater(aa->a1,bb->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfGreater(aa->a2,bb->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gGreater\n");
  }
  return b1;
}
static  BOOLEAN gEqual(number a,number b, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  BOOLEAN b1=coeffs1->cfEqual(aa->a1,bb->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfEqual(aa->a2,bb->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gEqual\n");
  }
  return b1;
}
static  BOOLEAN gIsZero(number a, const coeffs r)
{
  if (a==NULL) return TRUE;
  gcp aa=(gcp)a;
  BOOLEAN b1=coeffs1->cfIsZero(aa->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfIsZero(aa->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gIsZero\n");
  }
  return b1;
}
static  BOOLEAN gIsOne(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  BOOLEAN b1=coeffs1->cfIsOne(aa->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfIsOne(aa->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gIsOne\n");
  }
  return b1;
}
static  BOOLEAN gIsMOne(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  BOOLEAN b1=coeffs1->cfIsMOne(aa->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfIsMOne(aa->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gIsMOne\n");
  }
  return b1;
}
static  BOOLEAN gGreaterZero(number a, const coeffs r)
{
  gcp aa=(gcp)a;
  BOOLEAN b1=coeffs1->cfGreaterZero(aa->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfGreaterZero(aa->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gGreaterZero\n");
  }
  return b1;
}
static void gPower(number a, int i, number * result, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  coeffs1->cfPower(aa->a1,i,&cc->a1,coeffs1);
  coeffs2->cfPower(aa->a2,i,&cc->a2,coeffs2);
  *result=(number)cc;
}
static number gGcd (number a, number b, const coeffs)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfGcd(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfGcd(aa->a2,bb->a2,coeffs2);
  return (number)cc;
}
static number gSubringGcd (number a, number b, const coeffs)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfSubringGcd(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfSubringGcd(aa->a2,bb->a2,coeffs2);
  return (number)cc;
}
static number gGetDenom (number &a, const coeffs)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfGetDenom(aa->a1,coeffs1);
  cc->a2=coeffs2->cfGetDenom(aa->a2,coeffs2);
  return (number)cc;
}
static number gGetNumerator (number &a, const coeffs)
{
  gcp aa=(gcp)a;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfGetNumerator(aa->a1,coeffs1);
  cc->a2=coeffs2->cfGetNumerator(aa->a2,coeffs2);
  return (number)cc;
}
static number  gQuotRem(number a, number b, number *rem, const coeffs r)
{
 printf("gQuotRem\n");
 return NULL;
}
static number  gLcm(number a, number b, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfLcm(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfLcm(aa->a2,bb->a2,coeffs2);
  return (number)cc;
}
static number  gNormalizeHelper(number a, number b, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfNormalizeHelper(aa->a1,bb->a1,coeffs1);
  cc->a2=coeffs2->cfNormalizeHelper(aa->a2,bb->a2,coeffs2);
  return (number)cc;
}
static void  gDelete(number * a, const coeffs r)
{
  if (*a!=NULL)
  {
    gcp aa=(gcp)*a;
    coeffs1->cfDelete(&aa->a1,coeffs1);
    coeffs2->cfDelete(&aa->a2,coeffs2);
    omFree(aa);
    *a=NULL;
  }
}
static nMapFunc gSetMap(const coeffs src, const coeffs dst)
{
  printf("gSetMap\n");
  return NULL;
}
static void gWriteFd(number a, FILE *f, const coeffs r)
{
  printf("gWriteFd\n");
}
static number gReadFd( s_buff f, const coeffs r)
{
  printf("gReadFd\n");
  return NULL;
}
static number  gFarey(number p, number n, const coeffs)
{
  gcp aa=(gcp)p;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfFarey(aa->a1,n,coeffs1);
  cc->a2=coeffs2->cfFarey(aa->a2,n,coeffs2);
  return (number)cc;
}
static number gChineseRemainder(number *x, number *q,int rl, BOOLEAN sym,CFArray &inv_cache,const coeffs)
{
  printf("gChineseREmainder\n");
  return NULL;
}
static  number gRandom(siRandProc p, number p1, number p2, const coeffs cf)
{
  printf("gRandom\n");
  return NULL;
}
static BOOLEAN gDivBy (number a,number b, const coeffs)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  BOOLEAN b1=coeffs1->cfDivBy(aa->a1,bb->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfDivBy(aa->a2,bb->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gDivBy:%d,%d\n",b1,b2);
  }
  return b1;
}
static number  gExtGcd (number a, number b, number *s, number *t, const coeffs)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  gcp ss=(gcp)omalloc(sizeof(*ss));
  gcp tt=(gcp)omalloc(sizeof(*ss));
  cc->a1=coeffs1->cfExtGcd(aa->a1,bb->a1,&ss->a1,&tt->a1,coeffs1);
  cc->a2=coeffs2->cfExtGcd(aa->a2,bb->a2,&ss->a2,&tt->a2,coeffs2);
  return (number)cc;
}
static number  gGetUnit (number n, const coeffs r)
{
  gcp aa=(gcp)n;
  gcp cc=(gcp)omalloc(sizeof(*cc));
  cc->a1=coeffs1->cfGetUnit(aa->a1,coeffs1);
  cc->a2=coeffs2->cfGetUnit(aa->a2,coeffs2);
  return (number)cc;
}
static BOOLEAN gIsUnit (number a, const coeffs)
{
  gcp aa=(gcp)a;
  BOOLEAN b1=coeffs1->cfIsUnit(aa->a1,coeffs1);
  BOOLEAN b2=coeffs2->cfIsUnit(aa->a2,coeffs2);
  if (b1!=b2)
  {
    printf("gIsUnit:%d,%d\n",b1,b2);
  }
  return b1;
}
static int gDivComp(number a, number b, const coeffs r)
{
  gcp aa=(gcp)a;
  gcp bb=(gcp)b;
  int i1=coeffs1->cfDivComp(aa->a1,bb->a1,coeffs1);
  int i2=coeffs2->cfDivComp(aa->a2,bb->a2,coeffs2);
  if (i1!=i2)
  {
    printf("gDivComp:%d,%d\n",i1,i2);
  }
  return i1;
}
static BOOLEAN gDBTest(number a, const char *f, const int l, const coeffs r)
{
 if (a==NULL)
   printf("NULL in %s:%d\n",f,l);
 return TRUE;
}
BOOLEAN gInitChar(coeffs r, void* p)
{
  coeffs1=nInitChar(n_Z_1,(void*)1);
  coeffs2=nInitChar(n_Z_2,(void*)1);
  r->is_field=FALSE;
  r->is_domain=TRUE;
  r->rep=n_rep_unknown;
  r->ch = 0;
  r->cfKillChar=gKillChar;
  //r->nCoeffIsEqual=gCoeffsEqual;
  r->cfCoeffString=gCoeffString;
  r->cfCoeffName=gCoeffName;
  r->cfCoeffWrite=gCoeffWrite;

  r->cfMult  = gMult;
  r->cfSub   = gSub;
  r->cfAdd   = gAdd;
  r->cfDiv   = gDiv;
  r->cfInit = gInit;
  r->cfSize  = gSize;
  r->cfInt  = gInt;
  #ifdef HAVE_RINGS
  r->cfDivComp = gDivComp;
  r->cfIsUnit = gIsUnit;
  r->cfGetUnit = gGetUnit;
  r->cfExtGcd = gExtGcd;
   r->cfDivBy = gDivBy;
  #endif
  r->cfInpNeg   = gInpNeg;
  r->cfInvers= gInvers;
  r->cfCopy  = gCopy;
  r->cfRePart = gCopy;
  //r->cfImPart = ndReturn0;
  r->cfWriteLong = gWriteLong;
  r->cfWriteShort = gWriteShort;
  r->cfRead = gRead;
  r->cfNormalize=gNormalize;
  r->cfGreater = gGreater;
  r->cfEqual = gEqual;
  r->cfIsZero = gIsZero;
  r->cfIsOne = gIsOne;
  r->cfIsMOne = gIsMOne;
  r->cfGreaterZero = gGreaterZero;
  r->cfPower = gPower;
  r->cfGetDenom = gGetDenom;
  r->cfGetNumerator = gGetNumerator;
  r->cfGcd  = gGcd;
  r->cfLcm  = gGcd;
  r->cfDelete= gDelete;
  r->cfSetMap = gSetMap;
  //r->cfInpMult=ndInpMult;
  r->cfRandom=gRandom;
  r->cfWriteFd=gWriteFd;
  r->cfReadFd=gReadFd;
  r->type=n_Z;
  #ifdef LDEBUG
  r->cfDBTest=gDBTest;
  #endif
  return FALSE;
}

