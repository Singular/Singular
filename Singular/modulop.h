#ifndef MODULOP_H
#define MODULOP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: modulop.h,v 1.7 1999-09-27 14:42:31 obachman Exp $ */
/*
* ABSTRACT: numbers modulo p (<=32003)
*/
#include "structs.h"

extern int npPrimeM;
extern int npGen;
extern int npMapPrime;

BOOLEAN npGreaterZero (number k);
number  npMult        (number a, number b);
number  npInit        (int i);
int     npInt         (number &n);
number  npCopy        (number k1);
number  npAdd         (number a, number b);
number  npSub         (number a, number b);
void    npPower       (number a, int i, number * result);
BOOLEAN npIsZero      (number a);
BOOLEAN npIsOne       (number a);
BOOLEAN npIsMOne       (number a);
number  npDiv         (number a, number b);
number  npNeg         (number c);
number  npInvers      (number c);
BOOLEAN npGreater     (number a, number b);
BOOLEAN npEqual       (number a, number b);
void    npWrite       (number &a);
char *  npRead        (char *s, number *a);
#ifdef LDEBUG
BOOLEAN npDBTest      (number a, char *f, int l);
#endif
void    npSetChar(int c);
//int     npGetChar();

BOOLEAN npSetMap(ring r);
number  npMapP(number from);
/*-------specials for spolys, do NOT use otherwise--------------------------*/
/* for npMultM, npSubM, npNegM, npEqualM : */
extern int npPminus1M;
extern CARDINAL *npExpTable;
extern CARDINAL *npLogTable;

inline number npMultM(number a, number b)
{
  int x = npLogTable[(int)a]+npLogTable[(int)b];
  return (number)npExpTable[x<npPminus1M ? x : x-npPminus1M];
}

inline number npAddM(number a, number b)
{
  int ka = (int)a + (int)b;
  if (ka >= npPrimeM) ka -= npPrimeM;
  return (number)ka;
}

inline BOOLEAN npIsZeroM (number  a)
{
  return 0 == (int)a;
}

/*
*inline number npMultM(number a, number b)
*{
*  return (number)(((int)a*(int)b) % npPrimeM);
*}
*/

#define npSubM(a,b)    (number)((int)a<(int)b ?\
                       npPrimeM-(int)b+(int)a : (int)a-(int)b)

#define npNegM(A)      (number)(npPrimeM-(int)(A))
#define npEqualM(A,B)  ((int)A==(int)B)
#define npIsZeroM(a)   (0 == (int)a)
#endif

