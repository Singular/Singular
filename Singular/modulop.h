#ifndef MODULOP_H
#define MODULOP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: modulop.h,v 1.12 2000-11-25 20:30:18 obachman Exp $ */
/*
* ABSTRACT: numbers modulo p (<=32003)
*/
#include "structs.h"

// define if a*b is with mod instead of tables
#define HAVE_MULT_MOD

extern int npPrimeM;
extern int npGen;
extern int npMapPrime;

BOOLEAN npGreaterZero (number k);
number  npMult        (number a, number b);
number  npInit        (int i);
int     npInt         (number &n);
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
void    npSetChar(int c, ring r);
void    npInitChar(int c, ring r);

//int     npGetChar();

BOOLEAN npSetMap(ring r);
number  npMapP(number from);
/*-------specials for spolys, do NOT use otherwise--------------------------*/
/* for npMultM, npSubM, npNegM, npEqualM : */
extern int npPminus1M;
extern CARDINAL *npExpTable;
extern CARDINAL *npLogTable;

#ifdef HAVE_MULT_MOD
inline number npMultM(number a, number b)
{
  return (number) 
    ((((unsigned long) a)*((unsigned long) b)) % ((unsigned long) npPrimeM));
}
#else
inline number npMultM(number a, number b)
{
  int x = npLogTable[(int)a]+npLogTable[(int)b];
  return (number)npExpTable[x<npPminus1M ? x : x-npPminus1M];
}
#endif

#if 0
inline number npAddAsm(number a, number b, int m)
{
  number r;
    asm ("addl %2, %1; cmpl %3, %1; jb 0f; subl %3, %1; 0:"
         : "=&r" (r)
         : "%0" (a), "g" (b), "g" (m)
         : "cc");
  return r;
}
inline number npSubAsm(number a, number b, int m)
{
  number r;
  asm ("subl %2, %1; jnc 0f; addl %3, %1; 0:"
        : "=&r" (r)
        : "%0" (a), "g" (b), "g" (m)
        : "cc");
  return r;
}
#endif

inline number npAddM(number a, number b)
{
  int r = (int)a + (int)b;
  return (number)(r >= npPrimeM ? r - npPrimeM : r);
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

