/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
static char rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/numbers.cc,v 1.1.1.1 1997-03-19 13:18:50 obachman Exp $";
/* $Log: not supported by cvs2svn $
*/

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
#ifndef FAST_AND_DIRTY
#undef npMultM
#undef npSubM
#undef npNegM
#undef npEqualM
#endif
#include "ffields.h"
#include "shortfl.h"

//static int characteristic = 0;
short numbers_delind = 0;
extern int IsPrime(int p);

void   (*nNew)(number *a);
number (*nInit)(int i);
number (*nPar)(int i);
int    (*nParDeg)(number n);
int    (*nSize)(number n);
int    (*nInt)(number &n);
numberfunc nMult, nSub, nAdd, nDiv, nIntDiv, nIntMod;
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
void   (*nWrite)(number &a);
char * (*nRead)(char *s,number *a);
void   (*nPower)(number a, int i, number * result);
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

int    nGetChar() { return nChar; }

int ndSize(number a) {return (int)nIsZero(a); }

/*2
* init operations for characteristic c (complete==TRUE)
* init nDelete    for characteristic c (complete==FALSE)
* param: the names of the parameters (read-only)
*/
void nSetChar(int c, BOOLEAN complete, char ** param, int pars)
{
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
  }
  //Print("n:c=%d compl=%d param=%d\n",c,complete,param);
  if ((c == 1) || (c< (-1)))
  {
    naSetChar(c,complete,param,pars);
#ifdef LDEBUG
    nDBDelete= naDBDelete;
#else
    nDelete= naDelete;
#endif
    numbers_delind=1;
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
      nIntDiv= naIntDiv;
      nIntMod= npIntMod; /* dummy !! */
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
      nSize  = naParDeg;
#ifdef LDEBUG
      nDBTest=naDBTest;
#endif
    }
  }
  else
  if (c == 0)
  {
#ifdef LDEBUG
    nDBDelete= nlDBDelete;
#else
    nDelete= nlDelete;
#endif
    numbers_delind=1;
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
      nName= ndName;
      nSize  = nlSize;
#ifdef LDEBUG
      nDBTest=nlDBTest;
#endif
    }
  }
  else if ((c>1)&&(param==NULL))
  /*----------------------char. p----------------*/
  {
#ifdef LDEBUG
    nDBDelete= nDBDummy1;
#else
    nDelete= nDummy1;
#endif
    numbers_delind=0;
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
      nIntDiv= npDiv;
      nIntMod= npIntMod;
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
      nName= ndName;
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=npDBTest;
#endif
    }
  }
  else
  if (c>1)
  {
#ifdef LDEBUG
    nDBDelete= nDBDummy1;
#else
    nDelete= nDummy1;
#endif
    numbers_delind=0;
    if (complete)
    {
      test &= ~Sy_bit(OPT_INTSTRATEGY); /*26*/
      nfSetChar(c,param);
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
      nIntDiv= nfDiv;
      nIntMod= nfIntMod;
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
  if (c==(-1))
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
      nIntDiv= nrDiv;
      nIntMod= nrIntMod;
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
      nName=ndName;
      /*nSize  = ndSize;*/
#ifdef LDEBUG
      nDBTest=nrDBTest;
#endif
    }
  }
#ifdef TEST
  else
  {
    Werror("invalid char %d\n",c);
  }
#endif
  if (complete&&(!errorreported)) nNULL=nInit(0);
}

