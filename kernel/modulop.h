#ifndef MODULOP_H
#define MODULOP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo p (<=32003)
*/
#include <kernel/structs.h>

// defines are in struct.h
// define if a*b is with mod instead of tables
//#define HAVE_MULT_MOD 
// define if a/b is with mod instead of tables
//#define HAVE_DIV_MOD 
// define if an if should be used
//#define HAVE_GENERIC_ADD

// enable large primes (32003 < p < 2^31-)
#define NV_OPS
#define NV_MAX_PRIME 32003

extern long npPrimeM;
extern int npGen;
extern long npMapPrime;

BOOLEAN npGreaterZero (number k);
number  npMult        (number a, number b);
number  npInit        (int i, const ring r);
int     npInt         (number &n, const ring r);
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
void    npWrite       (number &a, const ring r);
const char *  npRead  (const char *s, number *a);
#ifdef LDEBUG
BOOLEAN npDBTest      (number a, const char *f, const int l);
#define npTest(A)     npDBTest(A,__FILE__,__LINE__)
#else
#define npTest(A)     (0)
#endif
void    npSetChar(int c, ring r);
void    npInitChar(int c, ring r);

//int     npGetChar();

nMapFunc npSetMap(const ring src, const ring dst);
number  npMapP(number from);
number  npMap0(number from);
/*-------specials for spolys, do NOT use otherwise--------------------------*/
/* for npMultM, npSubM, npNegM, npEqualM : */
#ifdef HAVE_DIV_MOD
extern unsigned short *npInvTable;
#else
#ifndef HAVE_MULT_MOD
extern long npPminus1M;
extern unsigned short *npExpTable;
extern unsigned short *npLogTable;
#endif
#endif

#if 0
inline number npMultM(number a, number b)
// return (a*b)%n
{
   double ab;
   long q, res;

   ab = ((double) ((int)a)) * ((double) ((int)b));
   q  = (long) (ab/((double) npPrimeM));  // q could be off by (+/-) 1
   res = (long) (ab - ((double) q)*((double) npPrimeM));
   res += (res >> 31) & npPrimeM;
   res -= npPrimeM;
   res += (res >> 31) & npPrimeM;
   return (number)res;
}
#endif
#ifdef HAVE_MULT_MOD
static inline number npMultM(number a, number b)
{
  return (number) 
    ((((unsigned long) a)*((unsigned long) b)) % ((unsigned long) npPrimeM));
}
#else
static inline number npMultM(number a, number b)
{
  long x = (long)npLogTable[(long)a]+npLogTable[(long)b];
  return (number)(long)npExpTable[x<npPminus1M ? x : x-npPminus1M];
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
#ifdef HAVE_GENERIC_ADD
static inline number npAddM(number a, number b)
{
  long r = (long)a + (long)b;
  return (number)(r >= npPrimeM ? r - npPrimeM : r);
}
static inline number npSubM(number a, number b)
{
  return (number)((long)a<(long)b ?
                       npPrimeM-(long)b+(long)a : (long)a-(long)b);
}
#else
static inline number npAddM(number a, number b)
{
   long res = ((long)a + (long)b);
   res -= npPrimeM;
#if SIZEOF_LONG == 8
   res += (res >> 63) & npPrimeM;
#else
   res += (res >> 31) & npPrimeM;
#endif
   return (number)res;
}
static inline number npSubM(number a, number b)
{
   long res = ((long)a - (long)b);
#if SIZEOF_LONG == 8
   res += (res >> 63) & npPrimeM;
#else
   res += (res >> 31) & npPrimeM;
#endif
   return (number)res;
}
#endif

static inline BOOLEAN npIsZeroM (number  a)
{
  return 0 == (long)a;
}

/*
*inline number npMultM(number a, number b)
*{
*  return (number)(((long)a*(long)b) % npPrimeM);
*}
*/

#define npNegM(A)      (number)(npPrimeM-(long)(A))
#define npEqualM(A,B)  ((A)==(B))


#ifdef NV_OPS
static inline number nvMultM(number a, number b)
{
#if SIZEOF_LONG == 4
#define ULONG64 (unsigned long long)(unsigned long)
#else
#define ULONG64 (unsigned long)
#endif
  return (number) 
    (unsigned long)((ULONG64 a)*(ULONG64 b) % (ULONG64 npPrimeM));
}
number  nvMult        (number a, number b);
number  nvDiv         (number a, number b);
number  nvInvers      (number c);
void    nvPower       (number a, int i, number * result);
void    nvInpMult     (number &a, number b, const ring r);
#endif

#endif
