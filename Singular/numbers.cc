/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: numbers.cc,v 1.15 1999-06-22 08:09:19 Singular Exp $ */

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
#ifndef FAST_AND_DIRTY
#undef npMultM
#undef npSubM
#undef npNegM
#undef npEqualM
#endif
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
BOOLEAN (*nGreater)(number a,number b);
BOOLEAN (*nEqual)(number a,number b);
BOOLEAN (*nIsZero)(number a);
BOOLEAN (*nIsOne)(number a);
BOOLEAN (*nIsMOne)(number a);
BOOLEAN (*nGreaterZero)(number a);
void    (*nWrite)(number &a);
char *  (*nRead)(char *s,number *a);
void    (*nPower)(number a, int i, number * result);
number  (*nGetDenom)(number &n);
numberfunc nGcd,nLcm;
BOOLEAN (*nSetMap)(int c,char **par, int nop, number minpol);
number (*nMap)(number from);
char * (*nName)(number n);
#ifdef LDEBUG
BOOLEAN (*nDBTest)(number a, char *f, int l);
void (*nDBDelete)(number *a, char *f, int l);
#else
void   (*nDelete)(number *a);
#endif

/*0 implementation*/
number nNULL; /* the 0 as constant */
int    nChar;

void   nDummy1(number* d) { *d=NULL; }

#ifdef LDEBUG
void   nDBDummy1(number* d,char *f, int l) { *d=NULL; }
#endif

void   nDummy2(number& d) { }

char * ndName(number n) { return NULL; }

number ndPar(int i) { return nInit(0); }

int    ndParDeg(number n) { return 0; }

number ndGcd(number a, number b) { return nInit(1); }

number ndIntMod(number a, number b) { return nInit(0); }

number ndGetDenom(number &n) { return nInit(1); }

int    nGetChar() { return nChar; }

int ndSize(number a) {return (int)nIsZero(a)==FALSE; }

/*2
* init operations for characteristic c (complete==TRUE)
* init nDelete    for characteristic c (complete==FALSE)
*/
void nSetChar(ring r, BOOLEAN complete)
{
  int c=rInternalChar(r);

  if (nNULL!=NULL)
  {
    nDelete(&nNULL);nNULL=NULL;
  }
  if (complete)
  {
    nChar=c;
    nPar   = ndPar;
    nParDeg= ndParDeg;
    nSize  = ndSize;
    nGetDenom = ndGetDenom;
    nName = ndName;
  }
  //Print("n:c=%d compl=%d param=%d\n",c,complete,r->parameter);
  //if ((c == 1) || (c< (-1)))
  if (rField_is_Extension(r))
  {
    naSetChar(c,complete,r->parameter,rPar(r));
#ifdef LDEBUG
    nDBDelete= naDBDelete;
#else
    nDelete= naDelete;
#endif
    if (complete)
    {
      test |= Sy_bit(OPT_INTSTRATEGY); /*intStrategy*/
      test &= ~Sy_bit(OPT_REDTAIL); /*noredTail*/
      nNew   = naNew;
      nNormalize=naNormalize;
      nInit  = naInit;
      nPar   = naPar;
      nParDeg= naParDeg;
      nInt   = naInt;
      nAdd   = naAdd;
      nSub   = naSub;
      nMult  = naMult;
      nDiv   = naDiv;
      nExactDiv= naDiv;
      nIntDiv= naIntDiv;
      nIntMod= ndIntMod; /* dummy !! */
      nNeg   = naNeg;
      nInvers= naInvers;
      nCopy  = naCopy;
      nGreater = naGreater;
      nEqual = naEqual;
      nIsZero = naIsZero;
      nIsOne = naIsOne;
      nIsMOne = naIsMOne;
      nGreaterZero = naGreaterZero;
      nWrite = naWrite;
      nRead = naRead;
      nPower = naPower;
      nGcd  = naGcd;
      nLcm  = naLcm;
      nSetMap = naSetMap;
      nName= naName;
      nSize  = naSize;
      nGetDenom = naGetDenom;
#ifdef LDEBUG
      nDBTest=naDBTest;
#endif
    }
  }
  else
  if (rField_is_Q(r))
  {
#ifdef LDEBUG
    nDBDelete= nlDBDelete;
#else
    nDelete= nlDelete;
#endif
    if (complete)
    {
      test |= Sy_bit(OPT_INTSTRATEGY); /*26*/
      nNew   = nlNew;
      nNormalize=nlNormalize;
      nInit  = nlInit;
      nInt   = nlInt;
      nAdd   = nlAdd;
      nSub   = nlSub;
      nMult  = nlMult;
      nDiv   = nlDiv;
      nExactDiv= nlExactDiv;
      nIntDiv= nlIntDiv;
      nIntMod= nlIntMod;
      nNeg   = nlNeg;
      nInvers= nlInvers;
      nCopy  = nlCopy;
      nGreater = nlGreater;
      nEqual = nlEqual;
      nIsZero = nlIsZero;
      nIsOne = nlIsOne;
      nIsMOne = nlIsMOne;
      nGreaterZero = nlGreaterZero;
      nWrite = nlWrite;
      nRead = nlRead;
      nPower = nlPower;
      nGcd  = nlGcd;
      nLcm  = nlLcm;
      nSetMap = nlSetMap;
      nSize  = nlSize;
      nGetDenom = nlGetDenom;
#ifdef LDEBUG
      nDBTest=nlDBTest;
#endif
    }
  }
  else if (rField_is_Zp(r))
  /*----------------------char. p----------------*/
  {
#ifdef LDEBUG
    nDBDelete= nDBDummy1;
#else
    nDelete= nDummy1;
#endif
    if (complete)
    {
      npSetChar(c);
      test &= ~Sy_bit(OPT_INTSTRATEGY); /*26*/
      nNew   = nDummy1;
      nNormalize=nDummy2;
      nInit  = npInit;
      nInt   = npInt;
      nAdd   = npAdd;
      nSub   = npSub;
      nMult  = npMult;
      nDiv   = npDiv;
      nExactDiv= npDiv;
      nIntDiv= npDiv;
      nIntMod= ndIntMod; /* dummy !! */
      nNeg   = npNeg;
      nInvers= npInvers;
      nCopy  = npCopy;
      nGreater = npGreater;
      nEqual = npEqual;
      nIsZero = npIsZero;
      nIsOne = npIsOne;
      nIsMOne = npIsMOne;
      nGreaterZero = npGreaterZero;
      nWrite = npWrite;
      nRead = npRead;
      nPower = npPower;
      nGcd  = ndGcd;
      nLcm  = ndGcd; /* tricky, isn't it ?*/
      nSetMap = npSetMap;
      /* nName= ndName; */
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=npDBTest;
#endif
    }
  }
  else
  /* -------------- GF(p^m) -----------------------*/
  if (rField_is_GF(r))
  {
#ifdef LDEBUG
    nDBDelete= nDBDummy1;
#else
    nDelete= nDummy1;
#endif
    if (complete)
    {
      test &= ~Sy_bit(OPT_INTSTRATEGY); /*26*/
      nfSetChar(c,r->parameter);
      nNew   = nDummy1;
      nNormalize=nDummy2;
      nInit  = nfInit;
      nPar   = nfPar;
      nParDeg= nfParDeg;
      nInt   = nfInt;
      nAdd   = nfAdd;
      nSub   = nfSub;
      nMult  = nfMult;
      nDiv   = nfDiv;
      nExactDiv= nfDiv;
      nIntDiv= nfDiv;
      nIntMod= ndIntMod; /* dummy !! */
      nNeg   = nfNeg;
      nInvers= nfInvers;
      nCopy  = nfCopy;
      nGreater = nfGreater;
      nEqual = nfEqual;
      nIsZero = nfIsZero;
      nIsOne = nfIsOne;
      nIsMOne = nfIsMOne;
      nGreaterZero = nfGreaterZero;
      nWrite = nfWrite;
      nRead = nfRead;
      nPower = nfPower;
      nGcd  = ndGcd;
      nLcm  = ndGcd; /* tricky, isn't it ?*/
      nSetMap = nfSetMap;
      nName= nfName;
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=nfDBTest;
#endif
    }
  }
  else
  /* -------------- R -----------------------*/
  //if (c==(-1))
  if (rField_is_R(r))
  {
#ifdef LDEBUG
    nDBDelete= nDBDummy1;
#else
    nDelete= nDummy1;
#endif
    if (complete)
    {
      nNew=nDummy1;
      nNormalize=nDummy2;
      nInit  = nrInit;
      nInt   = nrInt;
      nAdd   = nrAdd;
      nSub   = nrSub;
      nMult  = nrMult;
      nDiv   = nrDiv;
      nExactDiv= nrDiv;
      nIntDiv= nrDiv;
      nIntMod= ndIntMod; /* dummy !! */
      nNeg   = nrNeg;
      nInvers= nrInvers;
      nCopy  = nrCopy;
      nGreater = nrGreater;
      nEqual = nrEqual;
      nIsZero = nrIsZero;
      nIsOne = nrIsOne;
      nIsMOne = nrIsMOne;
      nGreaterZero = nrGreaterZero;
      nWrite = nrWrite;
      nRead = nrRead;
      nPower = nrPower;
      nGcd  = ndGcd;
      nLcm  = ndGcd; /* tricky, isn't it ?*/
      nSetMap=nrSetMap;
      /* nName= ndName; */
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=nrDBTest;
#endif
    }
  }
  else
  /* -------------- long R -----------------------*/
  if (rField_is_long_R(r))
  {
    setGMPFloatPrecBytes(r->ch_flags);
#ifdef LDEBUG
    nDBDelete= ngfDBDelete;
#else
    nDelete= ngfDelete;
#endif
    if (complete)
    {
      nNew=ngfNew;
      nNormalize=nDummy2;
      nInit  = ngfInit;
      nInt   = ngfInt;
      nAdd   = ngfAdd;
      nSub   = ngfSub;
      nMult  = ngfMult;
      nDiv   = ngfDiv;
      nExactDiv= ngfDiv;
      nIntDiv= ngfDiv;
      nIntMod= ndIntMod; /* dummy !! */
      nNeg   = ngfNeg;
      nInvers= ngfInvers;
      nCopy  = ngfCopy;
      nGreater = ngfGreater;
      nEqual = ngfEqual;
      nIsZero = ngfIsZero;
      nIsOne = ngfIsOne;
      nIsMOne = ngfIsMOne;
      nGreaterZero = ngfGreaterZero;
      nWrite = ngfWrite;
      nRead = ngfRead;
      nPower = ngfPower;
      nGcd  = ndGcd;
      nLcm  = ndGcd; /* tricky, isn't it ?*/
      nSetMap=ngfSetMap;
      /* nName= ndName; */
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=ngfDBTest;
#endif
    }
  }
  /* -------------- long C -----------------------*/
  else if (rField_is_long_C(r))
  {
    setGMPFloatPrecBytes(r->ch_flags);
#ifdef LDEBUG
    nDBDelete= ngcDBDelete;
#else
    nDelete= ngcDelete;
#endif
    if (complete)
    {
      nNew=ngcNew;
      nNormalize=nDummy2;
      nInit  = ngcInit;
      nInt   = ngcInt;
      nAdd   = ngcAdd;
      nSub   = ngcSub;
      nMult  = ngcMult;
      nDiv   = ngcDiv;
      nExactDiv= ngcDiv;
      nIntDiv= ngcDiv;
      nIntMod= ndIntMod; /* dummy !! */
      nNeg   = ngcNeg;
      nInvers= ngcInvers;
      nCopy  = ngcCopy;
      nGreater = ngcGreater;
      nEqual = ngcEqual;
      nIsZero = ngcIsZero;
      nIsOne = ngcIsOne;
      nIsMOne = ngcIsMOne;
      nGreaterZero = ngcGreaterZero;
      nWrite = ngcWrite;
      nRead = ngcRead;
      nPower = ngcPower;
      nGcd  = ndGcd;
      nLcm  = ndGcd; /* tricky, isn't it ?*/
      nSetMap=ngcSetMap;
      nPar=ngcPar;
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=ngcDBTest;
#endif
    }
  }
#ifdef TEST
  else
  {
    WerrorS("unknown field");
  }
#endif
  if (complete&&(!errorreported)) nNULL=nInit(0);
}

