/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: numbers.cc,v 1.47 2002-07-23 13:09:37 Singular Exp $ */

/*
* ABSTRACT: interface to coefficient aritmetics
*/

#include <string.h>
#include <stdlib.h>
#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "kstd1.h"
#include "numbers.h"
#include "longrat.h"
#include "longalg.h"
#include "modulop.h"
#include "gnumpfl.h"
#include "gnumpc.h"
#include "ring.h"
#include "ffields.h"
#include "shortfl.h"

//static int characteristic = 0;
extern int IsPrime(int p);

void   (*nNew)(number *a);
number (*nInit)(int i);
number (*nPar)(int i);
int    (*nParDeg)(number n);
int    (*nSize)(number n);
int    (*nInt)(number &n);
numberfunc nMult, nSub, nAdd, nDiv, nIntDiv, nIntMod, nExactDiv;
number (*nNeg)(number a);
number (*nInvers)(number a);
void   (*nNormalize)(number &a);
number (*nCopy)(number a);
number (*nRePart)(number a);
number (*nImPart)(number a);
BOOLEAN (*nGreater)(number a,number b);
BOOLEAN (*nEqual)(number a,number b);
BOOLEAN (*nIsZero)(number a);
BOOLEAN (*nIsOne)(number a);
BOOLEAN (*nIsMOne)(number a);
BOOLEAN (*nGreaterZero)(number a);
void    (*nWrite)(number &a);
char *  (*nRead)(char *s,number *a);
void    (*nPower)(number a, int i, number * result);
number  (*nGcd)(number a, number b, ring r);
number  (*nLcm)(number a, number b, ring r);
char * (*nName)(number n);
void   (*n__Delete)(number *a, ring r);

/*0 implementation*/
number nNULL; /* the 0 as constant */
int    nChar;


n_Procs_s *cf_root=NULL;

void   nDummy1(number* d) { *d=NULL; }
void   ndDelete(number* d, const ring r) { *d=NULL; }

#ifdef LDEBUG
void   nDBDummy1(number* d,char *f, int l) { *d=NULL; }
#endif

void   nDummy2(number& d) { }

char * ndName(number n) { return NULL; }

number ndPar(int i) { return nInit(0); }

number ndReturn0(number n) { return nInit(0); }

int    ndParDeg(number n) { return 0; }

number ndGcd(number a, number b, const ring r) { return r->cf->nInit(1); }

number ndIntMod(number a, number b) { return nInit(0); }

number ndGetDenom(number &n, const ring r) { return n_Init(1,r); }

int    nGetChar() { return nChar; }

int ndSize(number a) { return (int)nIsZero(a)==FALSE; }

number ndCopy(number a) { return a; }
number nd_Copy(number a, ring r) { return r->cf->nCopy(a); }

/*2
* init operations for characteristic c (complete==TRUE)
* init nDelete    for characteristic c (complete==FALSE)
*/
void nSetChar(ring r)
{
  int c=rInternalChar(r);

  nChar=c;
  n__Delete= r->cf->cfDelete;
  if (rField_is_Extension(r))
  {
    naSetChar(c,r);
  }
  else if (rField_is_Zp(r))
  /*----------------------char. p----------------*/
  {
    npSetChar(c, r);
  }
  /* -------------- GF(p^m) -----------------------*/
  else if (rField_is_GF(r))
  {
    nfSetChar(c,r->parameter);
  }
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (rField_is_R(r))
  {
  }
  /* -------------- long R -----------------------*/
  /* -------------- long C -----------------------*/
  else if ((rField_is_long_R(r))
  || (rField_is_long_C(r)))
  {
    setGMPFloatDigits(r->float_len,r->float_len2);
  }
#ifdef TEST
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (!rField_is_R(r) && !rField_is_Q(r))
  {
    WerrorS("unknown field");
  }
#endif
  nNew   = r->cf->nNew;
  nNormalize=r->cf->nNormalize;
  nInit  = r->cf->nInit;
  nPar   = r->cf->nPar;
  nParDeg= r->cf->nParDeg;
  nInt   = r->cf->nInt;
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
  nGreater = r->cf->nGreater;
  nEqual = r->cf->nEqual;
  nIsZero = r->cf->nIsZero;
  nIsOne = r->cf->nIsOne;
  nIsMOne = r->cf->nIsMOne;
  nGreaterZero = r->cf->nGreaterZero;
  nWrite = r->cf->nWrite;
  nRead = r->cf->nRead;
  nPower = r->cf->nPower;
  nGcd  = r->cf->nGcd;
  nLcm  = r->cf->nLcm;
  nName= r->cf->nName;
  nSize  = r->cf->nSize;
  nRePart = r->cf->nRePart;
  nImPart = r->cf->nImPart;
  if (!errorreported) nNULL=r->cf->nNULL;
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
  }
  r->cf=n;
  r->cf->nChar = c;
  r->cf->nPar  = ndPar;
  r->cf->nParDeg=ndParDeg;
  r->cf->nSize = ndSize;
  r->cf->n_GetDenom= ndGetDenom;
  r->cf->nName =  ndName;
  r->cf->nImPart=ndReturn0;
  r->cf->cfDelete= ndDelete;
  r->cf->nNew=nDummy1;
  r->cf->nCopy  = ndCopy;
  r->cf->cfCopy  = nd_Copy;
  if (rField_is_Extension(r))
  {
    //naInitChar(c,TRUE,r);
    r->cf->cfDelete = naDelete;
    r->cf-> nNew       = naNew;
    r->cf-> nNormalize = naNormalize;
    r->cf->nInit       = naInit;
    r->cf->nPar        = naPar;
    r->cf->nParDeg     = naParDeg;
    r->cf->nInt        = naInt;
    r->cf->nAdd        = naAdd;
    r->cf->nSub        = naSub;
    r->cf->nMult       = naMult;
    r->cf->nDiv        = naDiv;
    r->cf->nExactDiv   = naDiv;
    r->cf->nIntDiv     = naIntDiv;
    r->cf->nIntMod     = ndIntMod; /* dummy !! */
    r->cf->nNeg        = naNeg;
    r->cf->nInvers     = naInvers;
    r->cf->nCopy       = naCopy;
    r->cf->cfCopy      = na_Copy;
    r->cf->nGreater    = naGreater;
    r->cf->nEqual      = naEqual;
    r->cf->nIsZero     = naIsZero;
    r->cf->nIsOne      = naIsOne;
    r->cf->nIsMOne     = naIsMOne;
    r->cf->nGreaterZero= naGreaterZero;
    r->cf->nWrite      = naWrite;
    r->cf->nRead       = naRead;
    r->cf->nPower      = naPower;
    r->cf->nGcd        = naGcd;
    r->cf->nLcm        = naLcm;
    r->cf->cfSetMap    = naSetMap;
    r->cf->nName       = naName;
    r->cf->nSize       = naSize;
    r->cf->n_GetDenom   = naGetDenom;
#ifdef LDEBUG
    //r->cf->nDBTest     = naDBTest;
#endif
  }
  else if (rField_is_Q(r))
  {
    r->cf->cfDelete= nlDelete;
    r->cf->nNew   = nlNew;
    r->cf->nNormalize=nlNormalize;
    r->cf->nInit  = nlInit;
    r->cf->nInt   = nlInt;
    r->cf->nAdd   = nlAdd;
    r->cf->nSub   = nlSub;
    r->cf->nMult  = nlMult;
    r->cf->nDiv   = nlDiv;
    r->cf->nExactDiv= nlExactDiv;
    r->cf->nIntDiv= nlIntDiv;
    r->cf->nIntMod= nlIntMod;
    r->cf->nNeg   = nlNeg;
    r->cf->nInvers= nlInvers;
    r->cf->nCopy  = nlCopy;
    r->cf->cfCopy  = nl_Copy;
    r->cf->nGreater = nlGreater;
    r->cf->nEqual = nlEqual;
    r->cf->nIsZero = nlIsZero;
    r->cf->nIsOne = nlIsOne;
    r->cf->nIsMOne = nlIsMOne;
    r->cf->nGreaterZero = nlGreaterZero;
    r->cf->nWrite = nlWrite;
    r->cf->nRead = nlRead;
    r->cf->nPower = nlPower;
    r->cf->nGcd  = nlGcd;
    r->cf->nLcm  = nlLcm;
    r->cf->cfSetMap = nlSetMap;
    r->cf->nSize  = nlSize;
    r->cf->n_GetDenom = nlGetDenom;
#ifdef LDEBUG
    //r->cf->nDBTest=nlDBTest;
#endif
  }
  else if (rField_is_Zp(r))
  /*----------------------char. p----------------*/
  {
    npInitChar(c,r);
    r->cf->nNormalize=nDummy2;
    r->cf->nInit  = npInit;
    r->cf->nInt   = npInt;
    r->cf->nAdd   = npAdd;
    r->cf->nSub   = npSub;
    r->cf->nMult  = npMult;
    r->cf->nDiv   = npDiv;
    r->cf->nExactDiv= npDiv;
    r->cf->nIntDiv= npDiv;
    r->cf->nIntMod= ndIntMod; /* dummy !! */
    r->cf->nNeg   = npNeg;
    r->cf->nInvers= npInvers;
    r->cf->nGreater = npGreater;
    r->cf->nEqual = npEqual;
    r->cf->nIsZero = npIsZero;
    r->cf->nIsOne = npIsOne;
    r->cf->nIsMOne = npIsMOne;
    r->cf->nGreaterZero = npGreaterZero;
    r->cf->nWrite = npWrite;
    r->cf->nRead = npRead;
    r->cf->nPower = npPower;
    r->cf->nGcd  = ndGcd;
    r->cf->nLcm  = ndGcd; /* tricky, isn't it ?*/
    r->cf->cfSetMap = npSetMap;
    /* nName= ndName; */
    /*nSize  = ndSize;*/
#ifdef LDEBUG
    //r->cf->nDBTest=npDBTest;
#endif
  }
  /* -------------- GF(p^m) -----------------------*/
  else if (rField_is_GF(r))
  {
    //nfSetChar(c,r->parameter);
    r->cf->nNormalize=nDummy2;
    r->cf->nInit  = nfInit;
    r->cf->nPar   = nfPar;
    r->cf->nParDeg= nfParDeg;
    r->cf->nInt   = nfInt;
    r->cf->nAdd   = nfAdd;
    r->cf->nSub   = nfSub;
    r->cf->nMult  = nfMult;
    r->cf->nDiv   = nfDiv;
    r->cf->nExactDiv= nfDiv;
    r->cf->nIntDiv= nfDiv;
    r->cf->nIntMod= ndIntMod; /* dummy !! */
    r->cf->nNeg   = nfNeg;
    r->cf->nInvers= nfInvers;
    r->cf->nGreater = nfGreater;
    r->cf->nEqual = nfEqual;
    r->cf->nIsZero = nfIsZero;
    r->cf->nIsOne = nfIsOne;
    r->cf->nIsMOne = nfIsMOne;
    r->cf->nGreaterZero = nfGreaterZero;
    r->cf->nWrite = nfWrite;
    r->cf->nRead = nfRead;
    r->cf->nPower = nfPower;
    r->cf->nGcd  = ndGcd;
    r->cf->nLcm  = ndGcd; /* tricky, isn't it ?*/
    r->cf->cfSetMap = nfSetMap;
    r->cf->nName= nfName;
    /*nSize  = ndSize;*/
#ifdef LDEBUG
    //r->cf->nDBTest=nfDBTest;
#endif
  }
  /* -------------- R -----------------------*/
  //if (c==(-1))
  else if (rField_is_R(r))
  {
    r->cf->nNormalize=nDummy2;
    r->cf->nInit  = nrInit;
    r->cf->nInt   = nrInt;
    r->cf->nAdd   = nrAdd;
    r->cf->nSub   = nrSub;
    r->cf->nMult  = nrMult;
    r->cf->nDiv   = nrDiv;
    r->cf->nExactDiv= nrDiv;
    r->cf->nIntDiv= nrDiv;
    r->cf->nIntMod= ndIntMod; /* dummy !! */
    r->cf->nNeg   = nrNeg;
    r->cf->nInvers= nrInvers;
    r->cf->nGreater = nrGreater;
    r->cf->nEqual = nrEqual;
    r->cf->nIsZero = nrIsZero;
    r->cf->nIsOne = nrIsOne;
    r->cf->nIsMOne = nrIsMOne;
    r->cf->nGreaterZero = nrGreaterZero;
    r->cf->nWrite = nrWrite;
    r->cf->nRead = nrRead;
    r->cf->nPower = nrPower;
    r->cf->nGcd  = ndGcd;
    r->cf->nLcm  = ndGcd; /* tricky, isn't it ?*/
    r->cf->cfSetMap=nrSetMap;
    /* nName= ndName; */
    /*nSize  = ndSize;*/
#ifdef LDEBUG
    //r->cf->nDBTest=nrDBTest;
#endif
  }
  /* -------------- long R -----------------------*/
  else if (rField_is_long_R(r))
  {
    r->cf->cfDelete= ngfDelete;
    r->cf->nNew=ngfNew;
    r->cf->nNormalize=nDummy2;
    r->cf->nInit  = ngfInit;
    r->cf->nInt   = ngfInt;
    r->cf->nAdd   = ngfAdd;
    r->cf->nSub   = ngfSub;
    r->cf->nMult  = ngfMult;
    r->cf->nDiv   = ngfDiv;
    r->cf->nExactDiv= ngfDiv;
    r->cf->nIntDiv= ngfDiv;
    r->cf->nIntMod= ndIntMod; /* dummy !! */
    r->cf->nNeg   = ngfNeg;
    r->cf->nInvers= ngfInvers;
    r->cf->nCopy  = ngfCopy;
    r->cf->cfCopy  = ngf_Copy;
    r->cf->nGreater = ngfGreater;
    r->cf->nEqual = ngfEqual;
    r->cf->nIsZero = ngfIsZero;
    r->cf->nIsOne = ngfIsOne;
    r->cf->nIsMOne = ngfIsMOne;
    r->cf->nGreaterZero = ngfGreaterZero;
    r->cf->nWrite = ngfWrite;
    r->cf->nRead = ngfRead;
    r->cf->nPower = ngfPower;
    r->cf->nGcd  = ndGcd;
    r->cf->nLcm  = ndGcd; /* tricky, isn't it ?*/
    r->cf->cfSetMap=ngfSetMap;
    r->cf->nName= ndName;
    r->cf->nSize  = ndSize;
#ifdef LDEBUG
    //r->cf->nDBTest=ngfDBTest;
#endif
  }
  /* -------------- long C -----------------------*/
  else if (rField_is_long_C(r))
  {
    r->cf->cfDelete= ngcDelete;
    r->cf->nNew=ngcNew;
    r->cf->nNormalize=nDummy2;
    r->cf->nInit  = ngcInit;
    r->cf->nInt   = ngcInt;
    r->cf->nAdd   = ngcAdd;
    r->cf->nSub   = ngcSub;
    r->cf->nMult  = ngcMult;
    r->cf->nDiv   = ngcDiv;
    r->cf->nExactDiv= ngcDiv;
    r->cf->nIntDiv= ngcDiv;
    r->cf->nIntMod= ndIntMod; /* dummy !! */
    r->cf->nNeg   = ngcNeg;
    r->cf->nInvers= ngcInvers;
    r->cf->nCopy  = ngcCopy;
    r->cf->cfCopy = ngc_Copy;
    r->cf->nGreater = ngcGreater;
    r->cf->nEqual = ngcEqual;
    r->cf->nIsZero = ngcIsZero;
    r->cf->nIsOne = ngcIsOne;
    r->cf->nIsMOne = ngcIsMOne;
    r->cf->nGreaterZero = ngcGreaterZero;
    r->cf->nWrite = ngcWrite;
    r->cf->nRead = ngcRead;
    r->cf->nPower = ngcPower;
    r->cf->nGcd  = ndGcd;
    r->cf->nLcm  = ndGcd; /* tricky, isn't it ?*/
    r->cf->cfSetMap=ngcSetMap;
    r->cf->nPar=ngcPar;
    r->cf->nRePart=ngcRePart;
    r->cf->nImPart=ngcImPart;
    /*nSize  = ndSize;*/
#ifdef LDEBUG
    //r->cf->nDBTest=ngcDBTest;
#endif
  }
#ifdef TEST
  else
  {
    WerrorS("unknown field");
  }
#endif
  if (!errorreported)
  {
    r->cf->nNULL=r->cf->nInit(0);
    if (r->cf->nRePart==NULL)
      r->cf->nRePart=r->cf->nCopy;
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
          cf_root=tmp.next;
          r->cf->cfDelete(&(r->cf->nNULL),r);
          switch(r->cf->type)
          {
            case n_Zp:
                 #ifdef HAVE_DIV_MOD
                 omFreeSize( (ADDRESS)r->cf->npInvTable,
                             r->cf->npPrimeM*sizeof(CARDINAL) );
                 #else
                 omFreeSize( (ADDRESS)r->cf->npExpTable,
                             r->cf->npPrimeM*sizeof(CARDINAL) );
                 omFreeSize( (ADDRESS)r->cf->npLogTable,
                             r->cf->npPrimeM*sizeof(CARDINAL) );
                 #endif
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
