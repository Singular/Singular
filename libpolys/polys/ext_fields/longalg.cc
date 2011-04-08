/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: longalg.cc 12469 2010-01-28 10:39:49Z hannes $ */
/*
* ABSTRACT:   algebraic numbers
*/

#include <stdio.h>
#include <string.h>
#include "polys/config.h"
// #include <polys/structs.h>
#include <omalloc/omalloc.h>
// #include <???/febase.h>
#include <coeffs/longrat.h>
#include <polys/modulop.h>
#include <coeffs/numbers.h>
// #include <polys/polys.h>
#include <polys/simpleideals.h>
#include <polys/monomials/ring.h>
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory/factory.h>
#include <polys/clapsing.h>
#include <polys/clapconv.h>
#endif
#include <polys/ext_fields/longtrans.h>
#include <polys/ext_fields/longalg.h>

struct snaIdeal
{
  int anz;
  poly *liste;
};
typedef struct snaIdeal * naIdeal;

naIdeal naI=NULL;

omBin snaIdeal_bin = omGetSpecBin(sizeof(snaIdeal));

int naNumbOfPar;
poly naMinimalPoly;
#define naParNames (currRing->parameter)
static int naIsChar0;
static ring naMapRing;

#ifdef LDEBUG
#define naTest(a) naDBTest(a,__FILE__,__LINE__)
BOOLEAN naDBTest(number a, const char *f,const int l);
#else
#define naTest(a)
#endif

naIdeal naI = NULL;
poly  naMinimalPoly;
omBin   snaIdeal_bin = omGetSpecBin(sizeof(snaIdeal));
number  (*naMap)(number from);
//omBin lnumber_bin = omGetSpecBin(sizeof(slnumber));
//omBin rnumber_bin = omGetSpecBin(sizeof(snumber));

void redefineFunctionPointers()
{
  n_Procs_s* n = currRing->cf;
  /* re-defining function pointers */
  n->cfDelete       = naDelete;
  n->nNormalize     = naNormalize;
  n->cfInit         = naInit;
  n->nPar           = naPar;
  n->nParDeg        = naParDeg;
  n->n_Int          = naInt;
  n->nAdd           = naAdd;
  n->nSub           = naSub;
  n->nMult          = naMult;
  n->nDiv           = naDiv;
  n->nExactDiv      = naDiv;
  n->nIntDiv        = naIntDiv;
  n->nNeg           = naNeg;
  n->nInvers        = naInvers;
  n->nCopy          = naCopy;
  n->cfCopy         = na_Copy;
  n->nGreater       = naGreater;
  n->nEqual         = naEqual;
  n->nIsZero        = naIsZero;
  n->nIsOne         = naIsOne;
  n->nIsMOne        = naIsMOne;
  n->nGreaterZero   = naGreaterZero;
  n->nGreater       = naGreater;
  n->cfWrite        = naWrite;
  n->nRead          = naRead;
  n->nPower         = naPower;
  n->nGcd           = naGcd;
  n->nLcm           = naLcm;
  n->cfSetMap       = naSetMap;
  n->nName          = naName;
  n->nSize          = naSize;
  n->cfGetDenom     = napGetDenom;
  n->cfGetNumerator = napGetNumerator;
#ifdef LDEBUG
  n->nDBTest        = naDBTest;
#endif
  /* re-defining global function pointers */
  nNormalize=naNormalize;
  nPar   = naPar;
  nParDeg= nParDeg;
  n_Int  = naInt;
  nAdd   = naAdd;
  nSub   = naSub;
  nMult  = naMult;
  nDiv   = naDiv;
  nExactDiv= naDiv;
  nIntDiv= naIntDiv;
  nNeg   = naNeg;
  nInvers= naInvers;
  nCopy  = naCopy;
  nGreater = naGreater;
  nEqual = naEqual;
  nIsZero = naIsZero;
  nIsOne = naIsOne;
  nIsMOne = naIsMOne;
  nGreaterZero = naGreaterZero;
  nGreater = naGreater;
  nRead = naRead;
  nPower = naPower;
  nGcd  = naGcd;
  nLcm  = naLcm;
  nName= naName;
  nSize  = naSize;
}

static number nadGcd( number a, number b, const ring r) { return nacInit(1,r); }
/*2
*  sets the appropriate operators
*/
void naSetChar(int i, ring r)
{
  assume((r->minpoly  != NULL) ||
         (r->minideal != NULL)    );
  
  if (naI!=NULL)
  {
    int j;
    for (j=naI->anz-1; j>=0; j--)
       p_Delete (&naI->liste[j],nacRing);
    omFreeSize((void *)naI->liste,naI->anz*sizeof(poly));
    omFreeBin((void *)naI, snaIdeal_bin);
    naI=NULL;
  }
  naMap = naCopy;

  if (r->minpoly!=NULL)
  {
    naMinimalPoly=((lnumber)r->minpoly)->z;
    #ifdef LDEBUG
    omCheckAddr(naMinimalPoly);
    #endif
  }
  else
    naMinimalPoly = NULL;
    
  if (r->minideal!=NULL)
  {
    naI=(naIdeal)omAllocBin(snaIdeal_bin);
    naI->anz=IDELEMS(r->minideal);
    naI->liste=(poly*)omAlloc(naI->anz*sizeof(poly));
    int j;
    for (j=naI->anz-1; j>=0; j--)
    {
      lnumber a = (lnumber)pGetCoeff(r->minideal->m[j]);
      naI->liste[j]=napCopy(a->z);
    }
  }

  ntNumbOfPar=rPar(r);
  if (i == 1)
    ntIsChar0 = 1;
  else if (i < 0)
  {
    ntIsChar0 = 0;
    npSetChar(-i, r->algring); // to be changed HS
  }
#ifdef TEST
  else
  {
    Print("naSetChar:c=%d param=%d\n",i,rPar(r));
  }
#endif
  nacRing        = r->algring;
  nacInit        = nacRing->cf->cfInit;
  nacInt         = nacRing->cf->n_Int;
  nacCopy        = nacRing->cf->nCopy;
  nacAdd         = nacRing->cf->nAdd;
  nacSub         = nacRing->cf->nSub;
  nacNormalize   = nacRing->cf->nNormalize;
  nacNeg         = nacRing->cf->nNeg;
  nacIsZero      = nacRing->cf->nIsZero;
  nacGreaterZero = nacRing->cf->nGreaterZero;
  nacGreater     = nacRing->cf->nGreater;
  nacIsOne       = nacRing->cf->nIsOne;
  nacGcd         = nacRing->cf->nGcd;
  nacLcm         = nacRing->cf->nLcm;
  nacMult        = nacRing->cf->nMult;
  nacDiv         = nacRing->cf->nDiv;
  nacIntDiv      = nacRing->cf->nIntDiv;
  nacInvers      = nacRing->cf->nInvers;
}

/*================ procedure for rational functions: naXXXX =================*/

/*2
*  z:= i
*/
number naInit(int i, const ring r)
{
  if (i!=0)
  {
    number c=n_Init(i,r->algring);
    if (!n_IsZero(c,r->algring))
    {
      poly z=p_Init(r->algring);
      pSetCoeff0(z,c);
      lnumber l = ALLOC_LNUMBER();
      l->z = z;
      l->s = 2;
      l->n = NULL;
      return (number)l;
    }
  }
  /*else*/
  return NULL;
}

number  naPar(int i)
{
  lnumber l = ALLOC_LNUMBER();
  l->s = 2;
  l->z = p_ISet(1,nacRing);
  napSetExp(l->z,i,1);
  p_Setm(l->z,nacRing);
  l->n = NULL;
  return (number)l;
}

int     naParDeg(number n)     /* i := deg(n) */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  return napDeg(l->z);
}

//int     naParDeg(number n)     /* i := deg(n) */
//{
//  lnumber l = (lnumber)n;
//  if (l==NULL) return -1;
//  return napMaxDeg(l->z)+napMaxDeg(l->n);
//}

int     naSize(number n)     /* size desc. */
{
  lnumber l = (lnumber)n;
  if (l==NULL) return -1;
  int len_z;
  int len_n;
  int o=napMaxDegLen(l->z,len_z)+napMaxDegLen(l->n,len_n);
  return (len_z+len_n)+o;
}

/*2
* convert a number to int (if possible)
*/
int naInt(number &n, const ring r)
{
  lnumber l=(lnumber)n;
  if ((l!=NULL)&&(l->n==NULL)&&(p_IsConstant(l->z,r->algring)))
  {
    return nacInt(pGetCoeff(l->z),r->algring);
  }
  return 0;
}

/*2
*  deletes p
*/
void naDelete(number *p, const ring r)
{
  if ((*p)!=r->minpoly)
  {
    lnumber l = (lnumber) * p;
    if (l==NULL) return;
    p_Delete(&(l->z),r->algring);
    p_Delete(&(l->n),r->algring);
    omFreeBin((void *)l,  rnumber_bin);
  }
  *p = NULL;
}

/*2
* copy p to erg
*/
number naCopy(number p)
{
  if (p==NULL) return NULL;
  naTest(p);
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = ALLOC_LNUMBER();
  erg->z = p_Copy(src->z, nacRing);
  erg->n = p_Copy(src->n, nacRing);
  erg->s = src->s;
  return (number)erg;
}
number na_Copy(number p, const ring r)
{
  if (p==NULL) return NULL;
  lnumber erg;
  lnumber src = (lnumber)p;
  erg = (lnumber)ALLOC_LNUMBER();
  erg->z = p_Copy(src->z,r->algring);
  erg->n = p_Copy(src->n,r->algring);
  erg->s = src->s;
  return (number)erg;
}

/*2
*  addition; lu:= la + lb
*/
number naAdd(number la, number lb)
{
  if (la==NULL) return naCopy(lb);
  if (lb==NULL) return naCopy(la);

  poly x, y;
  lnumber lu;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  if (b->n!=NULL) x = pp_Mult_qq(a->z, b->n,nacRing);
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = pp_Mult_qq(b->z, a->n,nacRing);
  else            y = napCopy(b->z);
  poly res = napAdd(x, y);
  if (res==NULL)
  {
    return (number)NULL;
  }
  lu = ALLOC_LNUMBER();
  lu->z=res;
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = pp_Mult_qq(a->n, b->n,nacRing);
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
  //if (x!=NULL)
  //{
  //  if (p_LmIsConstant(x,nacRing))
  //  {
  //    number inv=nacInvers(pGetCoeff(x));
  //    napMultN(lu->z,inv);
  //    n_Delete(&inv,nacRing);
  //    napDelete(&x);
  //  }
  //}
  lu->n = x;
  lu->s = FALSE;
  if (/*lu->n*/ x!=NULL)
  {
     number luu=(number)lu;
     //if (p_IsConstant(lu->n,nacRing)) naCoefNormalize(luu);
     //else
                naNormalize(luu);
     lu=(lnumber)luu;
  }
  //else lu->s=2;
  naTest((number)lu);
  return (number)lu;
}

/*2
*  subtraction; r:= la - lb
*/
number naSub(number la, number lb)
{
  lnumber lu;

  if (lb==NULL) return naCopy(la);
  if (la==NULL)
  {
    lu = (lnumber)naCopy(lb);
    lu->z = napNeg(lu->z);
    return (number)lu;
  }

  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;

  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif

  poly x, y;
  if (b->n!=NULL) x = pp_Mult_qq(a->z, b->n,nacRing);
  else            x = napCopy(a->z);
  if (a->n!=NULL) y = p_Mult_q(napCopy(b->z), napCopyNeg(a->n),nacRing);
  else            y = napCopyNeg(b->z);
  poly res = napAdd(x, y);
  if (res==NULL)
  {
    return (number)NULL;
  }
  lu = ALLOC_LNUMBER();
  lu->z=res;
  if (a->n!=NULL)
  {
    if (b->n!=NULL) x = pp_Mult_qq(a->n, b->n,nacRing);
    else            x = napCopy(a->n);
  }
  else
  {
    if (b->n!=NULL) x = napCopy(b->n);
    else            x = NULL;
  }
  lu->n = x;
  lu->s = FALSE;
  if (/*lu->n*/ x!=NULL)
  {
     number luu=(number)lu;
     //if (p_IsConstant(lu->n,nacRing)) naCoefNormalize(luu);
     //else
                         naNormalize(luu);
     lu=(lnumber)luu;
  }
  //else lu->s=2;
  naTest((number)lu);
  return (number)lu;
}

/*2
*  multiplication; r:= la * lb
*/
number naMult(number la, number lb)
{
  if ((la==NULL) || (lb==NULL))   /* never occurs even when la or lb
                                     represents zero??? */
    return NULL;

  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  lnumber lo;
  poly x;

  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  naTest(la);
  naTest(lb);

  lo = ALLOC_LNUMBER();
  lo->z = pp_Mult_qq(a->z, b->z,nacRing);

  if (a->n==NULL)
  {
    if (b->n==NULL)
      x = NULL;
    else
      x = napCopy(b->n);
  }
  else
  {
    if (b->n==NULL)
    {
      x = napCopy(a->n);
    }
    else
    {
      x = pp_Mult_qq(b->n, a->n, nacRing);
    }
  }
  if (naMinimalPoly!=NULL)
  {
    if ((lo->z != NULL) &&
        (p_GetExp(lo->z,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing)))
      lo->z = napRemainder(lo->z, naMinimalPoly);
    if ((x!=NULL) &&
        (p_GetExp(x,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing)))
      x = napRemainder(x, naMinimalPoly);
  }
  if (naI!=NULL)
  {
    lo->z = napRedp (lo->z);
    if (lo->z != NULL)
       lo->z = napTailred (lo->z);
    if (x!=NULL)
    {
      x = napRedp (x);
      if (x!=NULL)
        x = napTailred (x);
    }
  }
  if ((x!=NULL) && (p_LmIsConstant(x,nacRing)) && nacIsOne(pGetCoeff(x)))
    p_Delete(&x,nacRing);
  lo->n = x;
  lo->s = 0;
  if(lo->z==NULL)
  {
    omFreeBin((void *)lo, rnumber_bin);
    lo=NULL;
  }
  else if (lo->n!=NULL)
  {
    number luu=(number)lo;
    // if (p_IsConstant(lo->n,nacRing)) naCoefNormalize(luu);
    // else
                      naNormalize(luu);
    lo=(lnumber)luu;
  }
  //if (naMinimalPoly==NULL) lo->s=2;
  naTest((number)lo);
  return (number)lo;
}

number naIntDiv(number la, number lb)
{
  lnumber res;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  if (a==NULL)
  {
    return NULL;
  }
  if (b==NULL)
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  assume(a->z!=NULL && b->z!=NULL);
  assume(a->n==NULL && b->n==NULL);
  res = ALLOC_LNUMBER();
  res->z = napCopy(a->z);
  res->n = napCopy(b->z);
  res->s = 0;
  number nres=(number)res;
  naNormalize(nres);

  //napDelete(&res->n);
  naTest(nres);
  return nres;
}

/*2
*  division; lo:= la / lb
*/
number naDiv(number la, number lb)
{
  lnumber lo;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  poly x;

  if (a==NULL)
    return NULL;

  if (b==NULL)
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  #ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  lo = ALLOC_LNUMBER();
  if (b->n!=NULL)
    lo->z = pp_Mult_qq(a->z, b->n,nacRing);
  else
    lo->z = napCopy(a->z);
  if (a->n!=NULL)
    x = pp_Mult_qq(b->z, a->n, nacRing);
  else
    x = napCopy(b->z);
  if (naMinimalPoly!=NULL)
  {
    if (p_GetExp(lo->z,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing))
      lo->z = napRemainder(lo->z, naMinimalPoly);
    if (p_GetExp(x,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing))
      x = napRemainder(x, naMinimalPoly);
  }
  if (naI!=NULL)
  {
    lo->z = napRedp (lo->z);
    if (lo->z != NULL)
       lo->z = napTailred (lo->z);
    if (x!=NULL)
    {
      x = napRedp (x);
      if (x!=NULL)
        x = napTailred (x);
    }
  }
  if ((p_LmIsConstant(x,nacRing)) && nacIsOne(pGetCoeff(x)))
    p_Delete(&x,nacRing);
  lo->n = x;
  lo->s = 0;
  if (lo->n!=NULL)
  {
    number luu=(number)lo;
     //if (p_IsConstant(lo->n,nacRing)) naCoefNormalize(luu);
     //else
                         naNormalize(luu);
    lo=(lnumber)luu;
  }
  //else lo->s=2;
  naTest((number)lo);
  return (number)lo;
}

/*2
*  za:= - za, inplace
*/
number naNeg(number za)
{
  if (za!=NULL)
  {
    lnumber e = (lnumber)za;
    naTest(za);
    e->z = napNeg(e->z);
  }
  return za;
}

/*2
* 1/a
*/
number naInvers(number a)
{
  lnumber lo;
  lnumber b = (lnumber)a;
  poly x;

  if (b==NULL)
  {
    WerrorS(nDivBy0);
    return NULL;
  }
  #ifdef LDEBUG
  omCheckAddrSize(b,sizeof(slnumber));
  #endif
  lo = ALLOC0_LNUMBER();
  lo->s = b->s;
  if (b->n!=NULL)
    lo->z = napCopy(b->n);
  else
    lo->z = p_ISet(1,nacRing);
  x = b->z;
  if ((!p_LmIsConstant(x,nacRing)) || !nacIsOne(pGetCoeff(x)))
    x = napCopy(x);
  else
  {
    lo->n = NULL;
    naTest((number)lo);
    return (number)lo;
  }
  if (naMinimalPoly!=NULL)
  {
    x = napInvers(x, naMinimalPoly);
    x = p_Mult_q(x, lo->z,nacRing);
    if (p_GetExp(x,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing))
      x = napRemainder(x, naMinimalPoly);
    lo->z = x;
    lo->n = NULL;
    while (x!=NULL)
    {
      nacNormalize(pGetCoeff(x));
      pIter(x);
    }
  }
  else
    lo->n = x;
  if (lo->n!=NULL)
  {
     number luu=(number)lo;
     //if (p_IsConstant(lo->n,nacRing)) naCoefNormalize(luu);
     //else
                           naNormalize(luu);
     lo=(lnumber)luu;
  }
  naTest((number)lo);
  return (number)lo;
}


BOOLEAN naIsZero(number za)
{
  lnumber zb = (lnumber)za;
  naTest(za);
#ifdef LDEBUG
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(2)");
#endif
  return (zb==NULL);
}


BOOLEAN naGreaterZero(number za)
{
  lnumber zb = (lnumber)za;
#ifdef LDEBUG
  if ((zb!=NULL) && (zb->z==NULL)) WerrorS("internal zero error(3)");
#endif
  naTest(za);
  if (zb!=NULL)
  {
    return (nacGreaterZero(pGetCoeff(zb->z))||(!p_LmIsConstant(zb->z,nacRing)));
  }
  /* else */ return FALSE;
}


/*2
* a = b ?
*/
BOOLEAN naEqual (number a, number b)
{
  if(a==b) return TRUE;
  if((a==NULL)&&(b!=NULL)) return FALSE;
  if((b==NULL)&&(a!=NULL)) return FALSE;

  lnumber aa=(lnumber)a;
  lnumber bb=(lnumber)b;

  int an_deg=0;
  if(aa->n!=NULL)
    an_deg=napDeg(aa->n);
  int bn_deg=0;
  if(bb->n!=NULL)
    bn_deg=napDeg(bb->n);
  if(an_deg+napDeg(bb->z)!=bn_deg+napDeg(aa->z))
    return FALSE;
#if 0
  naNormalize(a);
  aa=(lnumber)a;
  naNormalize(b);
  bb=(lnumber)b;
  if((aa->n==NULL)&&(bb->n!=NULL)) return FALSE;
  if((bb->n==NULL)&&(aa->n!=NULL)) return FALSE;
  if(napComp(aa->z,bb->z)!=0) return FALSE;
  if((aa->n!=NULL) && (napComp(aa->n,bb->n))) return FALSE;
#endif
  number h = naSub(a, b);
  BOOLEAN bo = naIsZero(h);
  naDelete(&h,currRing);
  return bo;
}


/* This method will only consider the numerators of a and b.
   Moreover it may return TRUE only if one or both numerators
   are zero or if their degrees are equal. Then TRUE is returned iff
   coeff(numerator(a)) > coeff(numerator(b));
   In all other cases, FALSE will be returned. */
BOOLEAN naGreater (number a, number b)
{
  int az = 0; int ad = 0;
  if (naIsZero(a)) az = 1;
  else ad = napDeg(((lnumber)a)->z);
  int bz = 0; int bd = 0;
  if (naIsZero(b)) bz = 1;
  else bd = napDeg(((lnumber)b)->z);
  
  if ((az == 1) && (bz == 1)) /* a = b = 0 */ return FALSE;
  if (az == 1) /* a = 0, b != 0 */
  {
    return (!nacGreaterZero(pGetCoeff(((lnumber)b)->z)));
  }
  if (bz == 1) /* a != 0, b = 0 */
  {
    return (nacGreaterZero(pGetCoeff(((lnumber)a)->z)));
  }
  if (ad == bd)  
    return nacGreater(pGetCoeff(((lnumber)a)->z),
                      pGetCoeff(((lnumber)b)->z));
  return FALSE;
}

/*2
* reads a number
*/
const char  *naRead(const char *s, number *p)
{
  poly x;
  lnumber a;
  s = napRead(s, &x);
  if (x==NULL)
  {
    *p = NULL;
    return s;
  }
  *p = (number)ALLOC0_LNUMBER();
  a = (lnumber)*p;
  if ((naMinimalPoly!=NULL)
  && (p_GetExp(x,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing)))
    a->z = napRemainder(x, naMinimalPoly);
  else if (naI!=NULL)
  {
    a->z = napRedp(x);
    if (a->z != NULL)
      a->z = napTailred (a->z);
  }
  else
    a->z = x;
  if(a->z==NULL)
  {
    omFreeBin((void *)*p, rnumber_bin);
    *p=NULL;
  }
  else
  {
    a->n = NULL;
    a->s = 0;
    naTest(*p);
  }
  return s;
}

/*2
* tries to convert a number to a name
*/
char * naName(number n)
{
  lnumber ph = (lnumber)n;
  if (ph==NULL)
    return NULL;
  int i;
  char *s=(char *)omAlloc(4* ntNumbOfPar);
  char *t=(char *)omAlloc(8);
  s[0]='\0';
  for (i = 0; i <= ntNumbOfPar - 1; i++)
  {
    int e=p_GetExp(ph->z,i+1,nacRing);
    if (e > 0)
    {
      if (e >1)
      {
        sprintf(t,"%s%d",ntParNames[i],e);
        strcat(s,t);
      }
      else
      {
        strcat(s,ntParNames[i]);
      }
    }
  }
  omFreeSize((void *)t,8);
  if (s[0]=='\0')
  {
    omFree((void *)s);
    return NULL;
  }
  return s;
}

/*2
*  writes a number
*/
void naWrite(number &phn, const ring r)
{
  lnumber ph = (lnumber)phn;
  if (ph==NULL)
    StringAppendS("0");
  else
  {
    phn->s = 0;
    BOOLEAN has_denom=(ph->n!=NULL);
    napWrite(ph->z,has_denom/*(ph->n!=NULL)*/,r);
    if (has_denom/*(ph->n!=NULL)*/)
    {
      StringAppendS("/");
      napWrite(ph->n,TRUE,r);
    }
  }
}

/*2
* za == 1 ?
*/
BOOLEAN naIsOne(number za)
{
  lnumber a = (lnumber)za;
  poly x, y;
  number t;
  if (a==NULL) return FALSE;
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  if (a->z==NULL)
  {
    WerrorS("internal zero error(4)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (p_LmIsConstant(a->z,nacRing))
    {
      return nacIsOne(pGetCoeff(a->z));
    }
    else                 return FALSE;
  }
#if 0
  x = a->z;
  y = a->n;
  do
  {
    if (napComp(x, y))
      return FALSE;
    else
    {
      t = nacSub(pGetCoeff(x), pGetCoeff(y));
      if (!nacIsZero(t))
      {
        n_Delete(&t,nacRing);
        return FALSE;
      }
      else
        n_Delete(&t,nacRing);
    }
    pIter(x);
    pIter(y);
  }
  while ((x!=NULL) && (y!=NULL));
  if ((x!=NULL) || (y!=NULL)) return FALSE;
  p_Delete(&a->z,nacRing);
  p_Delete(&a->n,nacRing);
  a->z = p_ISet(1,nacRing);
  a->n = NULL;
  return TRUE;
#else
  return FALSE;
#endif
}

/*2
* za == -1 ?
*/
BOOLEAN naIsMOne(number za)
{
  lnumber a = (lnumber)za;
  poly x, y;
  number t;
  if (a==NULL) return FALSE;
#ifdef LDEBUG
  omCheckAddrSize(a,sizeof(slnumber));
  if (a->z==NULL)
  {
    WerrorS("internal zero error(5)");
    return FALSE;
  }
#endif
  if (a->n==NULL)
  {
    if (p_LmIsConstant(a->z,nacRing)) return n_IsMOne(pGetCoeff(a->z),nacRing);
    /*else                   return FALSE;*/
  }
  return FALSE;
}

/*2
* returns the i-th power of p (i>=0)
*/
void naPower(number p, int i, number *rc)
{
  number x;
  *rc = naInit(1,currRing);
  for (; i > 0; i--)
  {
    x = naMult(*rc, p);
    naDelete(rc,currRing);
    *rc = x;
  }
}

/*2
* result =gcd(a,b)
*/
number naGcd(number a, number b, const ring r)
{
  if (a==NULL)  return naCopy(b);
  if (b==NULL)  return naCopy(a);

  lnumber x, y;
  lnumber result = ALLOC0_LNUMBER();

  x = (lnumber)a;
  y = (lnumber)b;
  if ((ntNumbOfPar == 1) && (naMinimalPoly!=NULL))
  {
    if (pNext(x->z)!=NULL)
      result->z = p_Copy(x->z, r->algring);
    else
      result->z = napGcd0(x->z, y->z);
  }
  else
#ifndef HAVE_FACTORY
    result->z = napGcd(x->z, y->z); // change from napGcd0
#else
  {
    int c=ABS(nGetChar());
    if (c==1) c=0;
    setCharacteristic( c );

    poly rz=napGcd(x->z, y->z);
    CanonicalForm F, G, R;
    R=convSingPFactoryP(rz,r->algring);
    p_Normalize(x->z,nacRing);
    F=convSingPFactoryP(x->z,r->algring)/R;
    p_Normalize(y->z,nacRing);
    G=convSingPFactoryP(y->z,r->algring)/R;
    F = gcd( F, G );
    if (F.isOne())
      result->z= rz;
    else
    {
      p_Delete(&rz,r->algring);
      result->z=convFactoryPSingP( F*R,r->algring );
      p_Normalize(result->z,nacRing);
    }
  }
#endif
  naTest((number)result);
  return (number)result;
}


/*2
* ntNumbOfPar = 1:
* clears denominator         algebraic case;
* tries to simplify ratio    transcendental case;
*
* cancels monomials
* occuring in denominator
* and enumerator  ?          ntNumbOfPar != 1;
*
* #defines for Factory:
* FACTORY_GCD_TEST: do not apply built in gcd for
*   univariate polynomials, always use Factory
*/
//#define FACTORY_GCD_TEST
void naCoefNormalize(number pp)
{
  if (pp==NULL) return;
  lnumber p = (lnumber)pp;
  number nz; // all denom. of the numerator
  nz=p_GetAllDenom(p->z,nacRing);
  BOOLEAN norm=FALSE;
  if (!n_IsOne(nz,nacRing))
  {
    norm=TRUE;
    p->z=p_Mult_nn(p->z,nz,nacRing);
    if (p->n==NULL)
    {
      p->n=p_NSet(nz,nacRing);
    }
    else
    {
      p->n=p_Mult_nn(p->n,nz,nacRing);
      n_Delete(&nz, nacRing);
    }
  }
  else
  {
    n_Delete(&nz, nacRing);
  }
  if (norm)
  {
    norm=FALSE;
    p_Normalize(p->z,nacRing);
    p_Normalize(p->n,nacRing);
  }
  number nn;
  nn=p_GetAllDenom(p->n,nacRing);
  if (!n_IsOne(nn,nacRing))
  {
    norm=TRUE;
    p->n=p_Mult_nn(p->n,nn,nacRing);
    p->z=p_Mult_nn(p->z,nn,nacRing);
    n_Delete(&nn, nacRing);
  }
  else
  {
    n_Delete(&nn, nacRing);
  }
  if (norm)
  {
    p_Normalize(p->z,nacRing);
    p_Normalize(p->n,nacRing);
  }
  // remove common factors in n, z:
  if (p->n!=NULL)
  {
    poly pp=p->z;
    nz=n_Copy(pGetCoeff(pp),nacRing);
    pIter(pp);
    while(pp!=NULL)
    {
      if (n_IsOne(nz,nacRing)) break;
      number d=n_Gcd(nz,pGetCoeff(pp),nacRing);
      n_Delete(&nz,nacRing); nz=d;
      pIter(pp);
    }
    if (!n_IsOne(nz,nacRing))
    {
      pp=p->n;
      nn=n_Copy(pGetCoeff(pp),nacRing);
      pIter(pp);
      while(pp!=NULL)
      {
        if (n_IsOne(nn,nacRing)) break;
        number d=n_Gcd(nn,pGetCoeff(pp),nacRing);
        n_Delete(&nn,nacRing); nn=d;
        pIter(pp);
      }
      number ng=n_Gcd(nz,nn,nacRing);
      n_Delete(&nn,nacRing);
      if (!n_IsOne(ng,nacRing))
      {
        number ni=n_Invers(ng,nacRing);
	p->z=p_Mult_nn(p->z,ni,nacRing);
	p->n=p_Mult_nn(p->n,ni,nacRing);
        p_Normalize(p->z,nacRing);
        p_Normalize(p->n,nacRing);
	n_Delete(&ni,nacRing);
      }
      n_Delete(&ng,nacRing);
    }
    n_Delete(&nz,nacRing);
  }
  if (p->n!=NULL)
  {
    if(!nacGreaterZero(pGetCoeff(p->n)))
    {
      p->z=napNeg(p->z);
      p->n=napNeg(p->n);
    }

    if (/*(p->n!=NULL) && */
    (p_IsConstant(p->n,nacRing))
    && (n_IsOne(pGetCoeff(p->n),nacRing)))
    {
      p_Delete(&(p->n), nacRing);
      p->n = NULL;
    }
  }
}

void naNormalize(number &pp)
{

  //naTest(pp); // input may not be "normal"
  lnumber p = (lnumber)pp;

  if (p==NULL)
    return;
  naCoefNormalize(pp);
  p->s = 2;
  poly x = p->z;
  poly y = p->n;

  BOOLEAN norm=FALSE;

  if ((y!=NULL) && (naMinimalPoly!=NULL))
  {
    y = napInvers(y, naMinimalPoly);
    x = p_Mult_q(x, y,nacRing);
    if (p_GetExp(x,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing))
      x = napRemainder(x, naMinimalPoly);
    p->z = x;
    p->n = y = NULL;
    norm=ntIsChar0;
  }

  /* check for degree of x too high: */
  if ((x!=NULL) && (naMinimalPoly!=NULL) && (x!=naMinimalPoly)
  && (p_GetExp(x,1,nacRing)>p_GetExp(naMinimalPoly,1,nacRing)))
  // DO NOT REDUCE naMinimalPoly with itself
  {
    x = napRemainder(x, naMinimalPoly);
    p->z = x;
    norm=ntIsChar0;
  }
  /* normalize all coefficients in n and z (if in Q) */
  if (norm) 
  {
    naCoefNormalize(pp);
    x = p->z;
    y = p->n;
  }
  if (y==NULL) return;

  if ((naMinimalPoly == NULL) && (x!=NULL) && (y!=NULL))
  {
    int i;
    for (i=ntNumbOfPar-1; i>=0; i--)
    {
      poly xx=x;
      poly yy=y;
      int m = napExpi(i, yy, xx);
      if (m != 0)          // in this case xx!=NULL!=yy
      {
        while (xx != NULL)
        {
          napAddExp(xx,i+1, -m);
          pIter(xx);
        }
        while (yy != NULL)
        {
          napAddExp(yy,i+1, -m);
          pIter(yy);
        }
      }
    }
  }
  if (p_LmIsConstant(y,nacRing)) /* i.e. => simplify to (1/c)*z / monom */
  {
    if (nacIsOne(pGetCoeff(y)))
    {
      p_LmDelete(&y,nacRing);
      p->n = NULL;
      naTest(pp);
      return;
    }
    number h1 = nacInvers(pGetCoeff(y));
    nacNormalize(h1);
    napMultN(x, h1);
    n_Delete(&h1,nacRing);
    p_LmDelete(&y,nacRing);
    p->n = NULL;
    naTest(pp);
    return;
  }
#ifndef FACTORY_GCD_TEST
  if (ntNumbOfPar == 1) /* apply built-in gcd */
  {
    poly x1,y1;
    if (p_GetExp(x,1,nacRing) >= p_GetExp(y,1,nacRing))
    {
      x1 = napCopy(x);
      y1 = napCopy(y);
    }
    else
    {
      x1 = napCopy(y);
      y1 = napCopy(x);
    }
    poly r;
    loop
    {
      r = napRemainder(x1, y1);
      if ((r==NULL) || (pNext(r)==NULL)) break;
      x1 = y1;
      y1 = r;
    }
    if (r!=NULL)
    {
      p_Delete(&r,nacRing);
      p_Delete(&y1,nacRing);
    }
    else
    {
      napDivMod(x, y1, &(p->z), &r);
      napDivMod(y, y1, &(p->n), &r);
      p_Delete(&y1,nacRing);
    }
    x = p->z;
    y = p->n;
    /* collect all denoms from y and multiply x and y by it */
    if (ntIsChar0)
    {
      number n=napLcm(y);
      napMultN(x,n);
      napMultN(y,n);
      n_Delete(&n,nacRing);
      while(x!=NULL)
      {
        nacNormalize(pGetCoeff(x));
        pIter(x);
      }
      x = p->z;
      while(y!=NULL)
      {
        nacNormalize(pGetCoeff(y));
        pIter(y);
      }
      y = p->n;
    }
    if (pNext(y)==NULL)
    {
      if (nacIsOne(pGetCoeff(y)))
      {
        if (p_GetExp(y,1,nacRing)==0)
        {
          p_LmDelete(&y,nacRing);
          p->n = NULL;
        }
        naTest(pp);
        return;
      }
    }
  }
#endif /* FACTORY_GCD_TEST */
#ifdef HAVE_FACTORY
#ifndef FACTORY_GCD_TEST
  else
#endif
  {
    poly xx,yy;
    singclap_algdividecontent(x,y,xx,yy);
    if (xx!=NULL)
    {
      p->z=xx;
      p->n=yy;
      p_Delete(&x,nacRing);
      p_Delete(&y,nacRing);
    }
  }
#endif
  /* remove common factors from z and n */
  x=p->z;
  y=p->n;
  if(!nacGreaterZero(pGetCoeff(y)))
  {
    x=napNeg(x);
    y=napNeg(y);
  }
  number g=nacCopy(pGetCoeff(x));
  pIter(x);
  while (x!=NULL)
  {
    number d=nacGcd(g,pGetCoeff(x), nacRing);
    if(nacIsOne(d))
    {
      n_Delete(&g,nacRing);
      n_Delete(&d,nacRing);
      naTest(pp);
      return;
    }
    n_Delete(&g,nacRing);
    g = d;
    pIter(x);
  }
  while (y!=NULL)
  {
    number d=nacGcd(g,pGetCoeff(y), nacRing);
    if(nacIsOne(d))
    {
      n_Delete(&g,nacRing);
      n_Delete(&d,nacRing);
      naTest(pp);
      return;
    }
    n_Delete(&g,nacRing);
    g = d;
    pIter(y);
  }
  x=p->z;
  y=p->n;
  while (x!=NULL)
  {
    number d = nacIntDiv(pGetCoeff(x),g);
    napSetCoeff(x,d);
    pIter(x);
  }
  while (y!=NULL)
  {
    number d = nacIntDiv(pGetCoeff(y),g);
    napSetCoeff(y,d);
    pIter(y);
  }
  n_Delete(&g,nacRing);
  naTest(pp);
}

/*2
* returns in result->n 1
* and in     result->z the lcm(a->z,b->n)
*/
number naLcm(number la, number lb, const ring r)
{
  lnumber result;
  lnumber a = (lnumber)la;
  lnumber b = (lnumber)lb;
  result = ALLOC0_LNUMBER();
  naTest(la);
  naTest(lb);
  poly x = p_Copy(a->z, r->algring);
  number t = napLcm(b->z); // get all denom of b->z
  if (!nacIsOne(t))
  {
    number bt, rr;
    poly xx=x;
    while (xx!=NULL)
    {
      bt = nacGcd(t, pGetCoeff(xx), r->algring);
      rr = nacMult(t, pGetCoeff(xx));
      n_Delete(&pGetCoeff(xx),r->algring);
      pGetCoeff(xx) = nacDiv(rr, bt);
      nacNormalize(pGetCoeff(xx));
      n_Delete(&bt,r->algring);
      n_Delete(&rr,r->algring);
      pIter(xx);
    }
  }
  n_Delete(&t,r->algring);
  result->z = x;
#ifdef HAVE_FACTORY
  if (b->n!=NULL)
  {
    result->z=singclap_alglcm(result->z,b->n);
    p_Delete(&x,r->algring);
  }
#endif
  naTest(la);
  naTest(lb);
  naTest((number)result);
  return ((number)result);
}

/*2
* input: a set of constant polynomials
* sets the global variable naI
*/
void naSetIdeal(ideal I)
{
  int i;

  if (idIs0(I))
  {
    for (i=naI->anz-1; i>=0; i--)
      p_Delete(&naI->liste[i],nacRing);
    omFreeBin((void *)naI, snaIdeal_bin);
    naI=NULL;
  }
  else
  {
    lnumber h;
    number a;
    poly x;

    naI=(naIdeal)omAllocBin(snaIdeal_bin);
    naI->anz=IDELEMS(I);
    naI->liste=(poly*)omAlloc(naI->anz*sizeof(poly));
    for (i=IDELEMS(I)-1; i>=0; i--)
    {
      h=(lnumber)pGetCoeff(I->m[i]);
      /* We only need the enumerator of h, as we expect it to be a polynomial */
      naI->liste[i]=napCopy(h->z);
      /* If it isn't normalized (lc = 1) do this */
      if (!nacIsOne(pGetCoeff(naI->liste[i])))
      {
        x=naI->liste[i];
        nacNormalize(pGetCoeff(x));
        a=nacCopy(pGetCoeff(x));
        number aa=nacInvers(a);
        n_Delete(&a,nacRing);
        napMultN(x,aa);
        n_Delete(&aa,nacRing);
      }
    }
  }
}

/*2
* map Z/p -> Q(a)
*/
number naMapP0(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(poly)p_Init(nacRing);
  int i=(int)((long)c);
  if (i>((long)ntMapRing->ch>>2)) i-=(long)ntMapRing->ch;
  pGetCoeff(l->z)=nlInit(i, nacRing);
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Q(a)
*/
number naMap00(number c)
{
  if (nlIsZero(c)) return NULL;
  lnumber l=ALLOC_LNUMBER();
  l->s=0;
  l->z=(poly)p_Init(nacRing);
  pGetCoeff(l->z)=nlCopy(c);
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p -> Z/p(a)
*/
number naMapPP(number c)
{
  if (npIsZero(c)) return NULL;
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(poly)p_Init(nacRing);
  pGetCoeff(l->z)=c; /* omit npCopy, because npCopy is a no-op */
  l->n=NULL;
  return (number)l;
}

/*2
* map Z/p' -> Z/p(a)
*/
number naMapPP1(number c)
{
  if (npIsZero(c)) return NULL;
  int i=(int)((long)c);
  if (i>(long)ntMapRing->ch) i-=(long)ntMapRing->ch;
  number n=npInit(i,ntMapRing);
  if (npIsZero(n)) return NULL;
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(poly)p_Init(nacRing);
  pGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

/*2
* map Q -> Z/p(a)
*/
number naMap0P(number c)
{
  if (nlIsZero(c)) return NULL;
  number n=npInit(nlModP(c,npPrimeM),nacRing);
  if (npIsZero(n)) return NULL;
  npTest(n);
  lnumber l=ALLOC_LNUMBER();
  l->s=2;
  l->z=(poly)p_Init(nacRing);
  pGetCoeff(l->z)=n;
  l->n=NULL;
  return (number)l;
}

/*2
* map _(a) -> _(b)
*/
number naMapQaQb(number c)
{
  if (c==NULL) return NULL;
  lnumber erg= ALLOC_LNUMBER();
  lnumber src =(lnumber)c;
  erg->s=src->s;
  erg->z=napMap(src->z);
  erg->n=napMap(src->n);
  if (naMinimalPoly!=NULL)
  {
    if (p_GetExp(erg->z,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing))
    {
      erg->z = napRemainder(erg->z, naMinimalPoly);
      if (erg->z==NULL)
      {
        number t_erg=(number)erg;
        naDelete(&t_erg,currRing);
        return (number)NULL;
      }
    }
    if (erg->n!=NULL)
    {
      if (p_GetExp(erg->n,1,nacRing) >= p_GetExp(naMinimalPoly,1,nacRing))
        erg->n = napRemainder(erg->n, naMinimalPoly);
      if ((p_IsConstant(erg->n,nacRing)) && nacIsOne(pGetCoeff(erg->n)))
        p_Delete(&(erg->n),nacRing);
    }
  }
  return (number)erg;
}

nMapFunc naSetMap(const ring src, const ring dst)
{
  ntMapRing=src;
  if (rField_is_Q_a(dst)) /* -> Q(a) */
  {
    if (rField_is_Q(src))
    {
      return naMap00;   /*Q -> Q(a)*/
    }
    if (rField_is_Zp(src))
    {
      return naMapP0;  /* Z/p -> Q(a)*/
    }
    if (rField_is_Q_a(src))
    {
      int i;
      ntParsToCopy=0;
      for(i=0;i<rPar(src);i++)
      {
        if ((i>=rPar(dst))
        ||(strcmp(src->parameter[i],dst->parameter[i])!=0))
           return NULL;
        ntParsToCopy++;
      }
      nacMap=nacCopy;
      if ((ntParsToCopy==rPar(dst))&&(ntParsToCopy==rPar(src)))
        return naCopy;    /* Q(a) -> Q(a) */
      return naMapQaQb;   /* Q(a..) -> Q(a..) */
    }
  }
  /*-----------------------------------------------------*/
  if (rField_is_Zp_a(dst)) /* -> Z/p(a) */
  {
    if (rField_is_Q(src))
    {
      return naMap0P;   /*Q -> Z/p(a)*/
    }
    if (rField_is_Zp(src))
    {
      if (src->ch==dst->ch)
      {
        return naMapPP;  /* Z/p -> Z/p(a)*/
      }
      else
      {
        return naMapPP1;  /* Z/p' -> Z/p(a)*/
      }
    }
    if (rField_is_Zp_a(src))
    {
      if (rChar(src)==rChar(dst))
      {
        nacMap=nacCopy;
      }
      else
      {
        nacMap = npMapP;
      }
      int i;
      ntParsToCopy=0;
      for(i=0;i<rPar(src);i++)
      {
        if ((i>=rPar(dst))
        ||(strcmp(src->parameter[i],dst->parameter[i])!=0))
           return NULL;
        ntParsToCopy++;
      }
      if ((ntParsToCopy==rPar(dst))&&(ntParsToCopy==rPar(src))
      && (nacMap==nacCopy))
        return naCopy;    /* Z/p(a) -> Z/p(a) */
      return naMapQaQb;   /* Z/p(a),Z/p'(a) -> Z/p(b)*/
    }
  }
  return NULL;      /* default */
}

#ifdef LDEBUG
BOOLEAN naDBTest(number a, const char *f,const int l)
{
  lnumber x=(lnumber)a;
  if (x == NULL)
    return TRUE;
  #ifdef LDEBUG
  omCheckAddrSize(a, sizeof(slnumber));
  #endif
  poly p = x->z;
  if (p==NULL)
  {
    Print("0/* in %s:%d\n",f,l);
    return FALSE;
  }
  while(p!=NULL)
  {
    if (( ntIsChar0  && nlIsZero(pGetCoeff(p)))
    || ((!ntIsChar0) && npIsZero(pGetCoeff(p))))
    {
      Print("coeff 0 in %s:%d\n",f,l);
      return FALSE;
    }
    if((naMinimalPoly!=NULL)
    &&(p_GetExp(p,1,nacRing)>p_GetExp(naMinimalPoly,1,nacRing))
    &&(p!=naMinimalPoly))
    {
      Print("deg>minpoly in %s:%d\n",f,l);
      return FALSE;
    }
    //if (ntIsChar0 && (((int)p->ko &3) == 0) && (p->ko->s==0) && (x->s==2))
    //{
    //  Print("normalized with non-normal coeffs in %s:%d\n",f,l);
    //  return FALSE;
    //}
    if (ntIsChar0 && !(nlDBTest(pGetCoeff(p),f,l)))
      return FALSE;
    pIter(p);
  }
  p = x->n;
  while(p!=NULL)
  {
    if (ntIsChar0 && !(nlDBTest(pGetCoeff(p),f,l)))
      return FALSE;
    pIter(p);
  }
  return TRUE;
}
#endif
