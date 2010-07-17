/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */

/*
* ABSTRACT: interface to coefficient aritmetics
*/

#include <string.h>
#include <stdlib.h>
#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/longalg.h>
#include <kernel/modulop.h>
#include <kernel/gnumpfl.h>
#include <kernel/gnumpc.h>
#include <kernel/ring.h>
#include <kernel/ffields.h>
#include <kernel/shortfl.h>
#ifdef HAVE_RINGS
#include <kernel/rmodulo2m.h>
#include <kernel/rmodulon.h>
#include <kernel/rintegers.h>

extern omBin gmp_nrz_bin;
#endif

//static int characteristic = 0;
extern int IsPrime(int p);

number  (*nInit_bigint)(number i);
number (*nPar)(int i);
int    (*nParDeg)(number n);
int    (*nSize)(number n);
int    (*n_Int)(number &n, const ring r);
numberfunc nMult, nSub, nAdd, nDiv, nIntDiv, nIntMod, nExactDiv;
number (*nNeg)(number a);
number (*nInvers)(number a);
void   (*nNormalize)(number &a);
number (*nCopy)(number a);
number (*nRePart)(number a);
number (*nImPart)(number a);
#ifdef HAVE_RINGS
BOOLEAN (*nDivBy)(number a,number b);
int     (*nDivComp)(number a,number b);
BOOLEAN (*nIsUnit)(number a);
number  (*nGetUnit)(number a);
number  (*nExtGcd)(number a, number b, number *s, number *t);
#endif
BOOLEAN (*nGreater)(number a,number b);
BOOLEAN (*nEqual)(number a,number b);
BOOLEAN (*nIsZero)(number a);
BOOLEAN (*nIsOne)(number a);
BOOLEAN (*nIsMOne)(number a);
BOOLEAN (*nGreaterZero)(number a);
const char* (*nRead)(const char *s,number *a);
void    (*nPower)(number a, int i, number * result);
number  (*nGcd)(number a, number b, const ring r);
number  (*nLcm)(number a, number b, const ring r);
char * (*nName)(number n);

/*0 implementation*/
number nNULL; /* the 0 as constant */

n_Procs_s *cf_root=NULL;

void   nNew(number* d) { *d=NULL; }
void   ndDelete(number* d, const ring r) { *d=NULL; }
void   ndInpMult(number &a, number b, const ring r)
{
  number n=n_Mult(a,b,r);
  n_Delete(&a,r);
  a=n;
}
number ndInpAdd(number &a, number b, const ring r)
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

void   nDummy2(number& d) { }

char * ndName(number n) { return NULL; }

number ndPar(int i) { return nInit(0); }

number ndReturn0(number n) { return nInit(0); }

int    ndParDeg(number n) { return 0; }

number ndGcd(number a, number b, const ring r) { return n_Init(1,r); }

number ndIntMod(number a, number b) { return nInit(0); }

number ndGetDenom(number &n, const ring r) { return n_Init(1,r); }
number ndGetNumerator(number &a,const ring r) { return r->cf->nCopy(a); }

int ndSize(number a) { return (int)nIsZero(a)==FALSE; }

number ndCopy(number a) { return a; }
number nd_Copy(number a,const ring r) { return r->cf->nCopy(a); }

#ifdef HAVE_RINGS
BOOLEAN ndDivBy(number a, number b) { return TRUE; } // assume a,b !=0
int ndDivComp(number a, number b) { return 0; }
BOOLEAN ndIsUnit(number a) { return !nIsZero(a); }
number  ndExtGcd (number a, number b, number *s, number *t) { return nInit(1); }
#endif

/*2
* init operations for characteristic c (complete==TRUE)
* init nDelete    for characteristic c (complete==FALSE)
*/
void nSetChar(ring r)
{
  int c=rInternalChar(r);

  /*--------------------- Q -----------------*/
  if (rField_is_Q(r))
  {
    nInit_bigint=nlCopy;
  }
  /*--------------------- Q_a/ Zp_a -----------------*/
  else if (rField_is_Extension(r))
  {
    naSetChar(c,r);
    if (rField_is_Q_a()) nInit_bigint=naMap00;
    if (rField_is_Zp_a()) nInit_bigint=naMap0P;
  }
#ifdef HAVE_RINGS
  /*----------------------ring Z / 2^m----------------*/
  else if (rField_is_Ring_2toM(r))
  {
    nr2mSetExp(c, r);
    nInit_bigint=nr2mMapQ;
  }
  /*----------------------ring Z / 2^m----------------*/
  else if (rField_is_Ring_Z(r))
  {
    nrzSetExp(c, r);
    nInit_bigint=nrzMapQ;
  }
  /*----------------------ring Z / n----------------*/
  else if (rField_is_Ring_ModN(r))
  {
    nrnSetExp(c, r);
    nInit_bigint=nrnMapQ;
  }
  /*----------------------ring Z / n----------------*/
  else if (rField_is_Ring_PtoM(r))
  {
    nrnSetExp(c, r);
    nInit_bigint=nrnMapQ;
  }
#endif
  else if (rField_is_Zp(r))
  /*----------------------char. p----------------*/
  {
    npSetChar(c, r);
    nInit_bigint=npMap0;
  }
  /* -------------- GF(p^m) -----------------------*/
  else if (rField_is_GF(r))
  {
    nfSetChar(c,r->parameter);
    nInit_bigint=ndReturn0; // not impl.
  }
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (rField_is_R(r))
  {
    nInit_bigint=nrMapQ;
  }
  /* -------------- long R -----------------------*/
  /* -------------- long C -----------------------*/
  else if ((rField_is_long_R(r))
  || (rField_is_long_C(r)))
  {
    setGMPFloatDigits(r->float_len,r->float_len2);
    if (rField_is_long_R(r)) nInit_bigint=ngfMapQ;
    else                     nInit_bigint=ngcMapQ;
  }
#ifdef TEST
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (!rField_is_R(r) && !rField_is_Q(r))
  {
    WerrorS("unknown field");
  }
#endif
  nNormalize=r->cf->nNormalize;
  nPar   = r->cf->nPar;
  nParDeg= r->cf->nParDeg;
  n_Int  = r->cf->n_Int;
  nAdd   = r->cf->nAdd;
  nSub   = r->cf->nSub;
  nMult  = r->cf->nMult;
  nDiv   = r->cf->nDiv;
  nExactDiv= r->cf->nExactDiv;
  nIntDiv= r->cf->nIntDiv;
  nIntMod= r->cf->nIntMod;
  nNeg   = r->cf->nNeg;
  nInvers= r->cf->nInvers;
  nCopy  = r->cf->nCopy;
#ifdef HAVE_RINGS
  nDivComp  = r->cf->nDivComp;
  nDivBy = r->cf->nDivBy;
  nIsUnit = r->cf->nIsUnit;
  nGetUnit = r->cf->nGetUnit;
  nExtGcd = r->cf->nExtGcd;
#endif
  nGreater = r->cf->nGreater;
  nEqual = r->cf->nEqual;
  nIsZero = r->cf->nIsZero;
  nIsOne = r->cf->nIsOne;
  nIsMOne = r->cf->nIsMOne;
  nGreaterZero = r->cf->nGreaterZero;
  nRead = r->cf->nRead;
  nPower = r->cf->nPower;
  nGcd  = r->cf->nGcd;
  nLcm  = r->cf->nLcm;
  nName= r->cf->nName;
  nSize  = r->cf->nSize;
  nRePart = r->cf->nRePart;
  nImPart = r->cf->nImPart;
  nNULL=r->cf->nNULL;
}

/*2
* init operations for ring r
*/
void nInitChar(ring r)
{
  int c=rInternalChar(r);
  n_coeffType t=rFieldType(r);

  if (rField_is_Extension(r))
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

  n_Procs_s *n=cf_root;
  while((n!=NULL)
    && ((n->nChar!=c) || (n->type!=t)))
      n=n->next;
  if (n==NULL)
  {
    n=(n_Procs_s*)omAlloc0(sizeof(n_Procs_s));
    n->next=cf_root;
    n->ref=1;
    n->nChar=c;
    n->type=t;
    cf_root=n;
  }
  else if ((n->nChar==c) && (n->type==t))
  {
    n->ref++;
    r->cf=n;
    return;
  }
  else
  {
    WerrorS("nInitChar failed");
    return;
  }
  r->cf=n;
  n->nChar = c;
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
  n->nNormalize=nDummy2;
  n->nGcd  = ndGcd;
  n->nLcm  = ndGcd; /* tricky, isn't it ?*/
#ifdef HAVE_RINGS
  n->nDivComp = ndDivComp;
  n->nDivBy = ndDivBy;
  n->nIsUnit = ndIsUnit;
  n->nExtGcd = ndExtGcd;
  n->nGetUnit = (nMapFunc)NULL;
#endif
  if (rField_is_Extension(r))
  {
    //naInitChar(c,TRUE,r);
    n->cfDelete = naDelete;
    n-> nNormalize = naNormalize;
    n->cfInit      = naInit;
    n->nPar        = naPar;
    n->nParDeg     = naParDeg;
    n->n_Int       = naInt;
    n->nAdd        = naAdd;
    n->nSub        = naSub;
    n->nMult       = naMult;
    n->nDiv        = naDiv;
    n->nExactDiv   = naDiv;
    n->nIntDiv     = naIntDiv;
    n->nNeg        = naNeg;
    n->nInvers     = naInvers;
    n->nCopy       = naCopy;
    n->cfCopy      = na_Copy;
    n->nGreater    = naGreater;
    n->nEqual      = naEqual;
    n->nIsZero     = naIsZero;
    n->nIsOne      = naIsOne;
    n->nIsMOne     = naIsMOne;
    n->nGreaterZero= naGreaterZero;
    n->cfWrite     = naWrite;
    n->nRead       = naRead;
    n->nPower      = naPower;
    n->nGcd        = naGcd;
    n->nLcm        = naLcm;
    n->cfSetMap    = naSetMap;
    n->nName       = naName;
    n->nSize       = naSize;
    n->cfGetDenom   = naGetDenom;
    n->cfGetNumerator= naGetNumerator;
#ifdef LDEBUG
    n->nDBTest     = naDBTest;
#endif
  }
#ifdef HAVE_RINGS
  /* -------------- Z/2^m ----------------------- */
  else if (rField_is_Ring_2toM(r))
  {
     nr2mInitExp(c,r);
     n->cfInit = nr2mInit;
     n->nCopy  = ndCopy;
     n->n_Int  = nr2mInt;
     n->nAdd   = nr2mAdd;
     n->nSub   = nr2mSub;
     n->nMult  = nr2mMult;
     n->nDiv   = nr2mDiv;
     n->nIntDiv       = nr2mIntDiv;
     n->nIntMod=nr2mMod;
     n->nExactDiv= nr2mDiv;
     n->nNeg   = nr2mNeg;
     n->nInvers= nr2mInvers;
     n->nDivBy = nr2mDivBy;
     n->nDivComp = nr2mDivComp;
     n->nGreater = nr2mGreater;
     n->nEqual = nr2mEqual;
     n->nIsZero = nr2mIsZero;
     n->nIsOne = nr2mIsOne;
     n->nIsMOne = nr2mIsMOne;
     n->nGreaterZero = nr2mGreaterZero;
     n->cfWrite = nr2mWrite;
     n->nRead = nr2mRead;
     n->nPower = nr2mPower;
     n->cfSetMap = nr2mSetMap;
     n->nNormalize = nDummy2;
     n->nLcm          = nr2mLcm;
     n->nGcd          = nr2mGcd;
     n->nIsUnit = nr2mIsUnit;
     n->nGetUnit = nr2mGetUnit;
     n->nExtGcd = nr2mExtGcd;
     n->nName= ndName;
#ifdef LDEBUG
     n->nDBTest=nr2mDBTest;
#endif
  }
  /* -------------- Z/n ----------------------- */
  else if (rField_is_Ring_ModN(r) || rField_is_Ring_PtoM(r)
  )
  {
     nrnInitExp(c,r);
     n->cfInit  = nrnInit;
     n->cfDelete= nrnDelete;
     n->nCopy  = nrnCopy;
     n->cfCopy = cfrnCopy;
     n->nSize  = nrnSize;
     n->n_Int  = nrnInt;
     n->nAdd   = nrnAdd;
     n->nSub   = nrnSub;
     n->nMult  = nrnMult;
     n->nDiv   = nrnDiv;
     n->nIntDiv= nrnIntDiv;
     n->nIntMod= nrnMod;
     n->nExactDiv= nrnDiv;
     n->nNeg   = nrnNeg;
     n->nInvers= nrnInvers;
     n->nDivBy = nrnDivBy;
     n->nDivComp = nrnDivComp;
     n->nGreater = nrnGreater;
     n->nEqual = nrnEqual;
     n->nIsZero = nrnIsZero;
     n->nIsOne = nrnIsOne;
     n->nIsMOne = nrnIsMOne;
     n->nGreaterZero = nrnGreaterZero;
     n->cfWrite = nrnWrite;
     n->nRead = nrnRead;
     n->nPower = nrnPower;
     n->cfSetMap = nrnSetMap;
     n->nNormalize = nDummy2;
     n->nLcm          = nrnLcm;
     n->nGcd          = nrnGcd;
     n->nIsUnit = nrnIsUnit;
     n->nGetUnit = nrnGetUnit;
     n->nExtGcd = nrnExtGcd;
     n->nName= ndName;
#ifdef LDEBUG
     n->nDBTest=nrnDBTest;
#endif
  }
  /* -------------- Z ----------------------- */
  else if (rField_is_Ring_Z(r))
  {
     n->cfInit  = nrzInit;
     n->cfDelete= nrzDelete;
     n->nCopy  = nrzCopy;
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
     n->nNormalize = nDummy2;
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
#endif
  else if (rField_is_Q(r))
  {
    n->cfDelete= nlDelete;
    n->nNormalize=nlNormalize;
    n->cfInit = nlInit;
    n->n_Int  = nlInt;
    n->nAdd   = nlAdd;
    n->nSub   = nlSub;
    n->nMult  = nlMult;
    n->nInpMult=nlInpMult;
    n->nDiv   = nlDiv;
    n->nExactDiv= nlExactDiv;
    n->nIntDiv= nlIntDiv;
    n->nIntMod= nlIntMod;
    n->nNeg   = nlNeg;
    n->nInvers= nlInvers;
    n->nCopy  = nlCopy;
    n->nGreater = nlGreater;
    n->nEqual = nlEqual;
    n->nIsZero = nlIsZero;
    n->nIsOne = nlIsOne;
    n->nIsMOne = nlIsMOne;
    n->nGreaterZero = nlGreaterZero;
    n->cfWrite = nlWrite;
    n->nRead = nlRead;
    n->nPower = nlPower;
    n->nGcd  = nlGcd;
    n->nLcm  = nlLcm;
    n->cfSetMap = nlSetMap;
    n->nSize  = nlSize;
    n->cfGetDenom = nlGetDenom;
    n->cfGetNumerator = nlGetNumerator;
#ifdef LDEBUG
    n->nDBTest=nlDBTest;
#endif
  }
  else if (rField_is_Zp(r))
  /*----------------------char. p----------------*/
  {
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
    /* nName= ndName; */
    /*nSize  = ndSize;*/
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
  }
  /* -------------- GF(p^m) -----------------------*/
  else if (rField_is_GF(r))
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
    n->nCopy  = ndCopy;
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
  else if (rField_is_R(r))
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
    n->nCopy  = ndCopy;
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
  else if (rField_is_long_R(r))
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
    n->nCopy  = ngfCopy;
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
  else if (rField_is_long_C(r))
  {
    n->cfDelete= ngcDelete;
    n->nNormalize=nDummy2;
    n->cfInit = ngcInit;
    n->n_Int  = ngcInt;
    n->nAdd   = ngcAdd;
    n->nSub   = ngcSub;
    n->nMult  = ngcMult;
    n->nDiv   = ngcDiv;
    n->nExactDiv= ngcDiv;
    n->nNeg   = ngcNeg;
    n->nInvers= ngcInvers;
    n->nCopy  = ngcCopy;
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
  if (n->nGetUnit==(nMapFunc)NULL) n->nGetUnit=n->nCopy;
#endif
  if (!errorreported)
  {
    n->nNULL=n->cfInit(0,r);
    if (n->nRePart==NULL)
      n->nRePart=n->nCopy;
    if (n->nIntDiv==NULL)
      n->nIntDiv=n->nDiv;
  }
}

void nKillChar(ring r)
{
  if (r!=NULL)
  {
    if (r->cf!=NULL)
    {
      r->cf->ref--;
      if (r->cf->ref<=0)
      {
        n_Procs_s tmp;
        n_Procs_s* n=&tmp;
        tmp.next=cf_root;
        while((n->next!=NULL) && (n->next!=r->cf)) n=n->next;
        if (n->next==r->cf)
        {
          n->next=n->next->next;
          if (cf_root==r->cf) cf_root=n->next;
          r->cf->cfDelete(&(r->cf->nNULL),r);
          switch(r->cf->type)
          {
            case n_Zp:
                 #ifdef HAVE_DIV_MOD
                 if (r->cf->npInvTable!=NULL)
                 omFreeSize( (ADDRESS)r->cf->npInvTable,
                             r->cf->npPrimeM*sizeof(unsigned short) );
                 #else
                 if (r->cf->npExpTable!=NULL)
                 {
                   omFreeSize( (ADDRESS)r->cf->npExpTable,
                               r->cf->npPrimeM*sizeof(unsigned short) );
                   omFreeSize( (ADDRESS)r->cf->npLogTable,
                               r->cf->npPrimeM*sizeof(unsigned short) );
                 }
                 #endif
                 break;
            case n_Zp_a:
            case n_Q_a:
                 {
                   number n=r->minpoly;
                   if (n!=NULL)
                   {
                     r->minpoly=NULL;
                     if (r==currRing) naMinimalPoly=NULL;
                     naDelete(&n,r);
                   }
                 }
                 break;

            default:
                 break;
          }
          omFreeSize((ADDRESS)r->cf, sizeof(n_Procs_s));
          r->cf=NULL;
        }
        else
        {
          WarnS("cf_root list destroyed");
        }
      }
    }
    if (r->algring!=NULL)
    {
      rKill(r->algring);
      r->algring=NULL;
    }
  }
}
