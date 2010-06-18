/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */

/*
* ABSTRACT: interface to coefficient aritmetics
*/

#include <string.h>
#include <stdlib.h>
#include "coeffs.h"
#include "reporter.h"
#include "omalloc.h"
#include "numbers.h"
#include "longrat.h"
#include "modulop.h"
#include "gnumpfl.h"
#include "gnumpc.h"
#include "ffields.h"
#include "shortfl.h"
#include "longtrans.h"
#ifdef HAVE_RINGS
#include <kernel/rmodulo2m.h>
#include <kernel/rmodulon.h>
#include <kernel/rintegers.h>

extern omBin gmp_nrz_bin;
#endif

#ifndef assume
#  define assume(a) if(!(a)){ Werror( "Assumption: is wrong: %s\n", #a ); };
#endif


//static int characteristic = 0;
extern int IsPrime(int p);

/*0 implementation*/
number nNULL; /* the 0 as constant */

n_Procs_s *cf_root=NULL;

void   nNew(number* d) { *d=NULL; }
void   ndDelete(number* d, const coeffs r) { *d=NULL; }
void   ndInpMult(number &a, number b, const coeffs r)
{
  number n=n_Mult(a,b,r);
  n_Delete(&a,r);
  a=n;
}
number ndInpAdd(number &a, number b, const coeffs r)
{
  number n=n_Add(a,b,r);
  n_Delete(&a,r);
  a=n;
  return a;
}

#ifdef LDEBUG
void   nDBDummy1(number* d,char *f, int l) { *d=NULL; }
BOOLEAN ndDBTest(number a, const char *f, const int l, const coeffs r)
{
  return TRUE;
}
#endif

void   ndNormalize(number& d, const coeffs r) { }

char * ndName(number n, const coeffs r) { return NULL; }

number ndPar(int i, const coeffs r) { return n_Init(0,r); }

number ndReturn0(number n, const coeffs r) { return n_Init(0,r); }

int    ndParDeg(number n, const coeffs r) { return 0; }

number ndGcd(number a, number b, const coeffs r) { return n_Init(1,r); }

number ndIntMod(number a, number b, const coeffs r) { return n_Init(0,r); }

number ndGetDenom(number &n, const coeffs r) { return n_Init(1,r); }
number ndGetNumerator(number &a,const coeffs r) { return n_Copy(a,r); }

int ndSize(number a, const coeffs r) { return (int)n_IsZero(a,r)==FALSE; }

number ndCopy(number a, const coeffs) { return a; }
number ndCopyMap(number a, const coeffs aRing, const coeffs r)
{
  assume( getCoeffType(r) == getCoeffType(aRing) );
  assume( nField_has_simple_Alloc(r) && nField_has_simple_Alloc(aRing) );
  
  return a;
}

number nd_Copy(number a, const coeffs r) { return n_Copy(a, r); }

#ifdef HAVE_RINGS
BOOLEAN ndDivBy(number a, number b) { return TRUE; } // assume a,b !=0
int ndDivComp(number a, number b) { return 2; }
BOOLEAN ndIsUnit(number a) { return !nIsZero(a); }
number  ndExtGcd (number a, number b, number *s, number *t) { return nInit(1); }
#endif

static n_coeffType nLastCoeffs=n_Z2m;
static cfInitCharProc *nInitCharTable=NULL;
/*2
* init operations for coeffs r
*/
coeffs nInitChar(n_coeffType t, void * parameter)
{
  n_Procs_s *n=cf_root;
  while((n!=NULL) && (!n->nCoeffIsEqual(n,t,parameter)))
      n=n->next;
  if (n==NULL)
  {
    n=(n_Procs_s*)omAlloc0(sizeof(n_Procs_s));
    n->next=cf_root;
    n->ref=1;
    n->type=t;

    // default entries (different from NULL) for some routines:
    n->cfPar  = ndPar;
    n->cfParDeg=ndParDeg;
    n->cfSize = ndSize;
    n->cfGetDenom= ndGetDenom;
    n->cfGetNumerator= ndGetNumerator;
    n->cfName =  ndName;
    n->cfImPart=ndReturn0;
    n->cfDelete= ndDelete;
    n->cfInpMult=ndInpMult;
    n->cfCopy=nd_Copy;
    n->cfIntMod=ndIntMod; /* dummy !! */
    n->cfNormalize=ndNormalize;
    n->cfGcd  = ndGcd;
    n->cfLcm  = ndGcd; /* tricky, isn't it ?*/
#ifdef HAVE_RINGS
    n->cfDivComp = ndDivComp;
    n->cfDivBy = ndDivBy;
    n->cfIsUnit = ndIsUnit;
    n->cfExtGcd = ndExtGcd;
    //n->cfGetUnit = (nMapFunc)NULL;
#endif
  #if 0 /*vertagt*/
  if (nField_is_Extension(r))
  {
    //ntInitChar(c,TRUE,r);
    n->cfDelete       = ntDelete;
    n->nNormalize     = ntNormalize;
    n->cfInit         = ntInit;
    n->nPar           = ntPar;
    n->nParDeg        = ntParDeg;
    n->n_Int          = ntInt;
    n->nAdd           = ntAdd;
    n->nSub           = ntSub;
    n->nMult          = ntMult;
    n->nDiv           = ntDiv;
    n->nExactDiv      = ntDiv;
    n->nIntDiv        = ntIntDiv;
    n->nNeg           = ntNeg;
    n->nInvers        = ntInvers;
    //n->nCopy          = ntCopy;
    n->cfCopy         = nt_Copy;
    n->nGreater       = ntGreater;
    n->nEqual         = ntEqual;
    n->nIsZero        = ntIsZero;
    n->nIsOne         = ntIsOne;
    n->nIsMOne        = ntIsMOne;
    n->nGreaterZero   = ntGreaterZero;
    n->cfWrite        = ntWrite;
    n->nRead          = ntRead;
    n->nPower         = ntPower;
    n->nGcd           = ntGcd;
    n->nLcm           = ntLcm;
    n->cfSetMap       = ntSetMap;
    n->nName          = ntName;
    n->nSize          = ntSize;
    n->cfGetDenom     = napGetDenom;
    n->cfGetNumerator = napGetNumerator;
#ifdef LDEBUG
    n->nDBTest        = naDBTest;
#endif
  }
  #endif
    cf_root=n;
    // init
    if ((nInitCharTable!=NULL) && (t<=nLastCoeffs))
       (nInitCharTable[t])(n,parameter);
     else
       Werror("coeff init missing for %d",(int)t);
    // post init settings:
    if (n->cfRePart==NULL) n->cfRePart=n->cfCopy;
    if (n->cfIntDiv==NULL) n->cfIntDiv=n->cfDiv;
#ifdef HAVE_RINGS
   if (n->cfGetUnit==(nMapFunc)NULL) n->cfGetUnit=n->cfCopy;
#endif
#ifndef NDEBUG
   assume(n->nCoeffIsEqual!=NULL);
   if(n->cfKillChar==NULL) Warn("cfKillChar is NULL for coeff %d",t);
   if(n->cfSetChar!=NULL) Warn("cfSetChar is NOT NULL for coeff %d",t);
   assume(n->cfMult!=NULL);
   assume(n->cfSub!=NULL);
   assume(n->cfAdd!=NULL);
   assume(n->cfDiv!=NULL);
   assume(n->cfIntDiv!=NULL);
   assume(n->cfIntMod!=NULL);
   assume(n->cfExactDiv!=NULL);
   assume(n->cfInit!=NULL);
   assume(n->cfPar!=NULL);
   assume(n->cfParDeg!=NULL);
   assume(n->cfSize!=NULL);
   assume(n->cfInt!=NULL);
   //assume(n->n->cfDivComp!=NULL);
   //assume(n->cfIsUnit!=NULL);
   //assume(n->cfGetUnit!=NULL);
   //assume(n->cfExtGcd!=NULL);
   assume(n->cfNeg!=NULL);
   assume(n->cfCopy!=NULL);
   assume(n->cfRePart!=NULL);
   assume(n->cfImPart!=NULL);
   assume(n->cfWrite!=NULL);
   assume(n->cfRead!=NULL);
   assume(n->cfNormalize!=NULL);
   assume(n->cfGreater!=NULL);
   //assume(n->cfDivBy!=NULL);
   assume(n->cfEqual!=NULL);
   assume(n->cfIsZero!=NULL);
   assume(n->cfIsOne!=NULL);
   assume(n->cfIsMOne!=NULL);
   assume(n->cfGreaterZero!=NULL);
   assume(n->cfPower!=NULL);
   assume(n->cfGetDenom!=NULL);
   assume(n->cfGetNumerator!=NULL);
   assume(n->cfGcd!=NULL);
   assume(n->cfLcm!=NULL);
   assume(n->cfDelete!=NULL);
   assume(n->cfSetMap!=NULL);
   assume(n->cfName!=NULL);
   assume(n->cfInpMult!=NULL);
   assume(n->cfInit_bigint!=NULL);
   #ifdef LDEBUG
   assume(n->cfDBTest!=NULL);
   #endif
   assume(n->type==t);
#endif
  }
  else
  {
    n->ref++;
  }
  return n;
}

void nKillChar(coeffs r)
{
  if (r!=NULL)
  {
    r->ref--;
    if (r->ref<=0)
    {
      n_Procs_s tmp;
      n_Procs_s* n=&tmp;
      tmp.next=cf_root;
      while((n->next!=NULL) && (n->next!=r)) n=n->next;
      if (n->next==r)
      {
        n->next=n->next->next;
        if (cf_root==r) cf_root=n->next;
        r->cfDelete(&(r->nNULL),r);
        if (r->cfKillChar!=NULL) r->cfKillChar(r);
        omFreeSize((void *)r, sizeof(n_Procs_s));
        r=NULL;
      }
      else
      {
        WarnS("cf_root list destroyed");
      }
      r->cf=NULL;
    }
  }
}

n_coeffType nRegister(n_coeffType n, cfInitCharProc p)
{
  if (n==n_unknown)
  {
    nLastCoeffs=(n_coeffType)(int(nLastCoeffs)+1);
    if (nInitCharTable==NULL)
    {
      nInitCharTable=(cfInitCharProc*)omAlloc0(
                                          nLastCoeffs*sizeof(cfInitCharProc));
    }
    else
    {
      nInitCharTable=(cfInitCharProc*)omReallocSize(nInitCharTable,
                                          (((int)nLastCoeffs)-1)*sizeof(cfInitCharProc),
                                          ((int)nLastCoeffs)*sizeof(cfInitCharProc));
    }

    nInitCharTable[nLastCoeffs]=p;
    return nLastCoeffs;
  }
  else
  {
    if (nInitCharTable==NULL)
    {
      nInitCharTable=(cfInitCharProc*)omAlloc0(
                                         ((int) nLastCoeffs)*sizeof(cfInitCharProc));
    }
    nInitCharTable[n]=p;
    return n;
  }
}

