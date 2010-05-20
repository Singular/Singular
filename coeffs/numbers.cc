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
  //  nr2mSetExp(c, r);
  //  r->nInit_bigint=nr2mMapQ;
  }
  /*----------------------ring Z ----------------*/
  else if (nField_is_Ring_Z(r))
  {
  //  nrzSetExp(c, r);
  //  r->nInit_bigint=nrzMapQ;
  }
  /*----------------------ring Z / n----------------*/
  else if (nField_is_Ring_ModN(r))
  {
  //  nrnSetExp(c, r);
  //  r->nInit_bigint=nrnMapQ;
  }
  /*----------------------ring Z / n----------------*/
  else if (nField_is_Ring_PtoM(r))
  {
  //  nrnSetExp(c, r);
  //  r->nInit_bigint=nrnMapQ;
  }
#endif
  /* -------------- GF(p^m) -----------------------*/
  else if (nField_is_GF(r))
  {
  //  nfSetChar(c,r->parameter);
  }
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (nField_is_R(r))
  {
  //  r->nInit_bigint=nrMapQ;
  }
  /* -------------- long R -----------------------*/
  /* -------------- long C -----------------------*/
  else if ((nField_is_long_R(r))
  || (nField_is_long_C(r)))
  {
   // setGMPFloatDigits(r->float_len,r->float_len2);
   // if (nField_is_long_R(r)) r->nInit_bigint=ngfMapQ;
   // else                     r->nInit_bigint=ngcMapQ;
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

static n_coeffType nLastCoeffs=n_Z2n;
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
    //n->nGetUnit = (nMapFunc)NULL;
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
    n->nDBTest        = ntDBTest;
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
    if (n->nRePart==NULL) n->nRePart=n->cfCopy;
    if (n->nIntDiv==NULL) n->nIntDiv=n->nDiv;
#ifdef HAVE_RINGS
   if (n->nGetUnit==(nMapFunc)NULL) n->nGetUnit=n->cfCopy;
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
    }
    //if (r->algring!=NULL)
    //{
    //  rKill(r->algring);
    //  r->algring=NULL;
    //}
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

