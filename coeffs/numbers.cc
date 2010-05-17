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
#include "output.h"
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
BOOLEAN ndDBTest(number a, const char *f, const int l)
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

/*2
* init operations for characteristic c (complete==TRUE)
* init nDelete    for characteristic c (complete==FALSE)
*/
void nSetChar(coeffs r)
{
  /*----------------------generic ----------------*/
    if ( r->cfSetChar!=NULL) r->cfSetChar(r);

  // not yet ready:
  /*--------------------- Q -----------------*/
  if (nField_is_Q(r))
  {
    r->nInit_bigint=nl_Copy;
  }
  #if 0 /* vertagt*/
  /*--------------------- Q_a/ Zp_a -----------------*/
  else if (nField_is_Extension(r))
  {
    if (r->minpoly != NULL)
    {
      naSetChar(c,r);
      if (rField_is_Q_a(r)) r->nInit_bigint=naMap00;
      if (rField_is_Zp_a(r)) r->nInit_bigint=naMap0P;
    }
    else
    {
      ntSetChar(c,r);
      if (rField_is_Q_a(r)) r->nInit_bigint=ntMap00;
      if (rField_is_Zp_a(r)) r->nInit_bigint=ntMap0P;
    }
  }
  #endif
#ifdef HAVE_RINGS
  /*----------------------ring Z / 2^m----------------*/
  else if (nField_is_Ring_2toM(r))
  {
    nr2mSetExp(c, r);
    r->nInit_bigint=nr2mMapQ;
  }
  /*----------------------ring Z ----------------*/
  else if (nField_is_Ring_Z(r))
  {
    nrzSetExp(c, r);
    r->nInit_bigint=nrzMapQ;
  }
  /*----------------------ring Z / n----------------*/
  else if (nField_is_Ring_ModN(r))
  {
    nrnSetExp(c, r);
    r->nInit_bigint=nrnMapQ;
  }
  /*----------------------ring Z / n----------------*/
  else if (nField_is_Ring_PtoM(r))
  {
    nrnSetExp(c, r);
    r->nInit_bigint=nrnMapQ;
  }
#endif
  /* -------------- GF(p^m) -----------------------*/
  else if (nField_is_GF(r))
  {
    nfSetChar(c,r->parameter);
    r->nInit_bigint=ndReturn0; // not impl.
  }
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (nField_is_R(r))
  {
    r->nInit_bigint=nrMapQ;
  }
  /* -------------- long R -----------------------*/
  /* -------------- long C -----------------------*/
  else if ((nField_is_long_R(r))
  || (nField_is_long_C(r)))
  {
    setGMPFloatDigits(r->float_len,r->float_len2);
    if (nField_is_long_R(r)) r->nInit_bigint=ngfMapQ;
    else                     r->nInit_bigint=ngcMapQ;
  }
#ifdef TEST
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (!nField_is_R(r) && !nField_is_Q(r))
  {
    WerrorS("unknown field");
  }
#endif
}

/*2
* init operations for coeffs r
*/
void nInitChar(coeffs r)
{
  int c=nInternalChar(r);
  n_coeffType t=getCoeffType(r);

  #if 0 /* vertagt*/
  if (nField_is_Extension(r))
  {
    if (r->algring==NULL)
    {
      int ch=-c;
      if (c==1) ch=0;
      r->algring=(ring) rDefault(ch,r->P,r->parameter);
      //r->algring->ShortOut=r->ShortOut;
      // includes: nInitChar(r->algring);
    }
  }
  #endif

  n_coeffType t=rFieldType(r);
  if ((r->cf!=NULL) && (r->cf->nChar==c) && (r->cf->type==t))
  { r->cf->ref++; return; }

  n_Procs_s *n=cf_root;
  while((n!=NULL)
    && ((n->ch!=c) || (n->type!=t)))
      n=n->next;
  if (n==NULL)
  {
    n=(n_Procs_s*)omAlloc0(sizeof(n_Procs_s));
    n->next=cf_root;
    n->ref=1;
    n->ch=c;
    n->type=t;
    cf_root=n;
  }
  else if ((n->ch==c) && (n->type==t))
  {
    n->ref++;
    r=n;
    return;
  }
  else
  {
    WerrorS("nInitChar failed");
    return;
  }
  r=n;
  n->ch = c;
  n->nPar  = ndPar;
  n->nParDeg=ndParDeg;
  n->nSize = ndSize;
  n->cfGetDenom= ndGetDenom;
  n->cfGetNumerator= ndGetNumerator;
  n->nName =  ndName;
  n->nImPart=ndReturn0;
  n->cfDelete= ndDelete;
  n->nInpMult=ndInpMult;
  n->cfCopy=nd_Copy;
  n->nIntMod=ndIntMod; /* dummy !! */
  n->nNormalize=ndNormalize;
  n->nGcd  = ndGcd;
  n->nLcm  = ndGcd; /* tricky, isn't it ?*/
#ifdef HAVE_RINGS
  n->nDivComp = ndDivComp;
  n->nDivBy = ndDivBy;
  n->nIsUnit = ndIsUnit;
  n->nExtGcd = ndExtGcd;
  n->nGetUnit = (nMapFunc)NULL;
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
#ifdef HAVE_RINGS
  /* -------------- Z/2^m ----------------------- */
  else if (nField_is_Ring_2toM(r))
  {
    nr2mCoeffInit(n, c, r->cf);
  }
  /* -------------- Z/n ----------------------- */
  else if (nField_is_Ring_ModN(r) || nField_is_Ring_PtoM(r))
  {
    nrnCoeffInit(n, c, r->cf);
  }
  /* -------------- Z ----------------------- */
  else if (nField_is_Ring_Z(r))
  {
     n->cfInit  = nrzInit;
     n->cfDelete= nrzDelete;
     n->cfCopy  = nrzCopy;
     n->cfCopy = cfrzCopy;
     n->nSize  = nrzSize;
     n->n_Int  = nrzInt;
     n->nAdd   = nrzAdd;
     n->nSub   = nrzSub;
     n->nMult  = nrzMult;
     n->nDiv   = nrzDiv;
     n->nIntDiv = nrzIntDiv;
     n->nIntMod = nrzIntMod;
     n->nExactDiv= nrzDiv;
     n->nNeg   = nrzNeg;
     n->nInvers= nrzInvers;
     n->nDivBy = nrzDivBy;
     n->nDivComp = nrzDivComp;
     n->nGreater = nrzGreater;
     n->nEqual = nrzEqual;
     n->nIsZero = nrzIsZero;
     n->nIsOne = nrzIsOne;
     n->nIsMOne = nrzIsMOne;
     n->nGreaterZero = nrzGreaterZero;
     n->cfWrite = nrzWrite;
     n->nRead = nrzRead;
     n->nPower = nrzPower;
     n->cfSetMap = nrzSetMap;
     n->nNormalize = ndNormalize;
     n->nLcm          = nrzLcm;
     n->nGcd          = nrzGcd;
     n->nIsUnit = nrzIsUnit;
     n->nGetUnit = nrzGetUnit;
     n->nExtGcd = nrzExtGcd;
     n->nName= ndName;
#ifdef LDEBUG
     n->nDBTest=ndDBTest; // not yet implemented: nrzDBTest;
#endif
  }
  else
#endif
  if (nField_is_Q(r))
  {
    r->cfInitChar=nlInitChar;
    nlInitChar();
  }
  else if (nField_is_Zp(r))
  /*----------------------char. p----------------*/
  {
    /* never again:
    npInitChar(c,r);
    n->cfInit = npInit;
    n->n_Int  = npInt;
    n->nAdd   = npAdd;
    n->nSub   = npSub;
    n->nMult  = npMult;
    n->nDiv   = npDiv;
    n->nExactDiv= npDiv;
    n->nNeg   = npNeg;
    n->nInvers= npInvers;
    n->nCopy  = ndCopy;
    n->nGreater = npGreater;
    n->nEqual = npEqual;
    n->nIsZero = npIsZero;
    n->nIsOne = npIsOne;
    n->nIsMOne = npIsMOne;
    n->nGreaterZero = npGreaterZero;
    n->cfWrite = npWrite;
    n->nRead = npRead;
    n->nPower = npPower;
    n->cfSetMap = npSetMap;
    // nName= ndName;
    // nSize  = ndSize;
#ifdef LDEBUG
    n->nDBTest=npDBTest;
#endif
#ifdef NV_OPS
    if (c>NV_MAX_PRIME)
    {
      n->nMult  = nvMult;
      n->nDiv   = nvDiv;
      n->nExactDiv= nvDiv;
      n->nInvers= nvInvers;
      n->nPower= nvPower;
      n->nInpMult= nvInpMult;
    }
#endif
    */
    r->cfInitChar=npInitChar;
    npInitChar(r,c);
  }
  /* -------------- GF(p^m) -----------------------*/
  else if (nField_is_GF(r))
  {
    //nfSetChar(c,r->parameter);
    n->cfInit = nfInit;
    n->nPar   = nfPar;
    n->nParDeg= nfParDeg;
    n->n_Int  = nfInt;
    n->nAdd   = nfAdd;
    n->nSub   = nfSub;
    n->nMult  = nfMult;
    n->nDiv   = nfDiv;
    n->nExactDiv= nfDiv;
    n->nNeg   = nfNeg;
    n->nInvers= nfInvers;
    n->cfCopy  = ndCopy;
    n->nGreater = nfGreater;
    n->nEqual = nfEqual;
    n->nIsZero = nfIsZero;
    n->nIsOne = nfIsOne;
    n->nIsMOne = nfIsMOne;
    n->nGreaterZero = nfGreaterZero;
    n->cfWrite = nfWrite;
    n->nRead = nfRead;
    n->nPower = nfPower;
    n->cfSetMap = nfSetMap;
    n->nName= nfName;
    /*nSize  = ndSize;*/
#ifdef LDEBUG
    n->nDBTest=nfDBTest;
#endif
  }
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (nField_is_R(r))
  {
    n->cfInit = nrInit;
    n->n_Int  = nrInt;
    n->nAdd   = nrAdd;
    n->nSub   = nrSub;
    n->nMult  = nrMult;
    n->nDiv   = nrDiv;
    n->nExactDiv= nrDiv;
    n->nNeg   = nrNeg;
    n->nInvers= nrInvers;
    n->cfCopy  = ndCopy;
    n->nGreater = nrGreater;
    n->nEqual = nrEqual;
    n->nIsZero = nrIsZero;
    n->nIsOne = nrIsOne;
    n->nIsMOne = nrIsMOne;
    n->nGreaterZero = nrGreaterZero;
    n->cfWrite = nrWrite;
    n->nRead = nrRead;
    n->nPower = nrPower;
    n->cfSetMap=nrSetMap;
    /* nName= ndName; */
    n->nSize = nrSize;
#ifdef LDEBUG
    n->nDBTest=ndDBTest; // not yet implemented: nrDBTest;
#endif
  }
  /* -------------- long R -----------------------*/
  else if (nField_is_long_R(r))
  {
    n->cfDelete= ngfDelete;
    n->cfInit = ngfInit;
    n->n_Int  = ngfInt;
    n->nAdd   = ngfAdd;
    n->nSub   = ngfSub;
    n->nMult  = ngfMult;
    n->nDiv   = ngfDiv;
    n->nExactDiv= ngfDiv;
    n->nNeg   = ngfNeg;
    n->nInvers= ngfInvers;
    n->cfCopy  = ngf_Copy;
    n->nGreater = ngfGreater;
    n->nEqual = ngfEqual;
    n->nIsZero = ngfIsZero;
    n->nIsOne = ngfIsOne;
    n->nIsMOne = ngfIsMOne;
    n->nGreaterZero = ngfGreaterZero;
    n->cfWrite = ngfWrite;
    n->nRead = ngfRead;
    n->nPower = ngfPower;
    n->cfSetMap=ngfSetMap;
    n->nName= ndName;
    n->nSize  = ngfSize;
#ifdef LDEBUG
    n->nDBTest=ndDBTest; // not yet implemented: ngfDBTest
#endif
  }
  /* -------------- long C -----------------------*/
  else if (nField_is_long_C(r))
  {
    n->cfDelete= ngcDelete;
    n->nNormalize=ndNormalize;
    n->cfInit = ngcInit;
    n->n_Int  = ngcInt;
    n->nAdd   = ngcAdd;
    n->nSub   = ngcSub;
    n->nMult  = ngcMult;
    n->nDiv   = ngcDiv;
    n->nExactDiv= ngcDiv;
    n->nNeg   = ngcNeg;
    n->nInvers= ngcInvers;
    n->cfCopy  = ngcCopy;
    n->nGreater = ngcGreater;
    n->nEqual = ngcEqual;
    n->nIsZero = ngcIsZero;
    n->nIsOne = ngcIsOne;
    n->nIsMOne = ngcIsMOne;
    n->nGreaterZero = ngcGreaterZero;
    n->cfWrite = ngcWrite;
    n->nRead = ngcRead;
    n->nPower = ngcPower;
    n->cfSetMap=ngcSetMap;
    n->nPar=ngcPar;
    n->nRePart=ngcRePart;
    n->nImPart=ngcImPart;
    n->nSize = ngcSize;
#ifdef LDEBUG
    n->nDBTest=ndDBTest; // not yet implemented: ngcDBTest
#endif
  }
#ifdef TEST
  else
  {
    WerrorS("unknown field");
  }
#endif
#ifdef HAVE_RINGS
  if (n->nGetUnit==(nMapFunc)NULL) n->nGetUnit=n->cfCopy;
#endif
  if (!errorreported)
  {
    n->nNULL=n->cfInit(0,r);
    if (n->nRePart==NULL)
      n->nRePart=n->cfCopy;
    if (n->nIntDiv==NULL)
      n->nIntDiv=n->nDiv;
  }
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
        /* was:
        switch(r->type)
        {
          case n_Zp_a:
          case n_Q_a:
               {
                 number n=r->minpoly;
                 if (n!=NULL)
                 {
                   r->minpoly=NULL;
                   naDelete(&n,r);
                 }
               }
               break;
            default:
                 break;
        }
        */
        omFreeSize((void *)r, sizeof(n_Procs_s));
        r=NULL;
      }
      else
      {
        WarnS("cf_root list destroyed");
      }
      r->cf=NULL;
    }
    //if (r->algring!=NULL)
    //{
    //  rKill(r->algring);
    //  r->algring=NULL;
    //}
  }
}

static n_coeffType nLastCoeffs=n_Z2n;
static cfInitCharProc *nInitCharTable=NULL;
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

