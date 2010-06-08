#ifndef MODULOP_H
#define MODULOP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: numbers modulo p (<=32003)
*/
#include "coeffs.h"

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

extern int npGen;

void npInitChar(coeffs r, int ch);

BOOLEAN npGreaterZero (number k, const coeffs r);
number  npMult        (number a, number b, const coeffs r);
number  npInit        (int i, const coeffs r);
int     npInt         (number &n, const coeffs r);
number  npAdd         (number a, number b,const coeffs r);
number  npSub         (number a, number b,const coeffs r);
void    npPower       (number a, int i, number * result,const coeffs r);
BOOLEAN npIsZero      (number a,const coeffs r);
BOOLEAN npIsOne       (number a,const coeffs r);
BOOLEAN npIsMOne       (number a,const coeffs r);
number  npDiv         (number a, number b,const coeffs r);
number  npNeg         (number c,const coeffs r);
number  npInvers      (number c,const coeffs r);
BOOLEAN npGreater     (number a, number b,const coeffs r);
BOOLEAN npEqual       (number a, number b,const coeffs r);
void    npWrite       (number &a, const coeffs r);
const char *  npRead  (const char *s, number *a,const coeffs r);
#ifdef LDEBUG
BOOLEAN npDBTest      (number a, const char *f, const int l, const coeffs r);
#define npTest(A,r)     npDBTest(A,__FILE__,__LINE__, r)
#else
#define npTest(A,r)     (0)
#endif

//int     npGetChar();

nMapFunc npSetMap(const coeffs src, const coeffs dst);
number  npMapP(number from, const coeffs src, const coeffs r);
number  npMap0(number from, const coeffs src, const coeffs r);
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
static inline number npMultM(number a, number b, const coeffs r)
{
  return (number) 
    ((((unsigned long) a)*((unsigned long) b)) % ((unsigned long) r->npPrimeM));
}
#else
static inline number npMultM(number a, number b, const coeffs r)
{
  long x = (long)r->npLogTable[(long)a]+ r->npLogTable[(long)b];
  return (number)(long)r->npExpTable[x<r->npPminus1M ? x : x- r->npPminus1M];
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
static inline number npAddM(number a, number b, const coeffs r)
{
  long R = (long)a + (long)b;
  return (number)(R >= r->npPrimeM ? R - r->npPrimeM : R);
}
static inline number npSubM(number a, number b, const coeffs r)
{
  return (number)((long)a<(long)b ?
                       r->npPrimeM-(long)b+(long)a : (long)a-(long)b);
}
#else
static inline number npAddM(number a, number b, const coeffs r)
{
   long res = ((long)a + (long)b);
   res -= r->npPrimeM;
#if SIZEOF_LONG == 8
   res += (res >> 63) & r->npPrimeM;
#else
   res += (res >> 31) & r->npPrimeM;
#endif
   return (number)res;
}
static inline number npSubM(number a, number b, const coeffs r)
{
   long res = ((long)a - (long)b);
#if SIZEOF_LONG == 8
   res += (res >> 63) & r->npPrimeM;
#else
   res += (res >> 31) & r->npPrimeM;
#endif
   return (number)res;
}
#endif

static inline BOOLEAN npIsZeroM (number  a, const coeffs r)
{
  return 0 == (long)a;
}

/*
*inline number npMultM(number a, number b)
*{
*  return (number)(((long)a*(long)b) % npPrimeM);
*}
*/

#define npNegM(A,r)      (number)(r->npPrimeM-(long)(A))
#define npEqualM(A,B,r)  ((A)==(B))


#ifdef NV_OPS
static inline number nvMultM(number a, number b, const coeffs r)
{
#if SIZEOF_LONG == 4
#define ULONG64 (unsigned long long)(unsigned long)
#else
#define ULONG64 (unsigned long)
#endif
  return (number) 
    (unsigned long)((ULONG64 a)*(ULONG64 b) % (ULONG64 r->npPrimeM));
}
number  nvMult        (number a, number b, const coeffs r);
number  nvDiv         (number a, number b, const coeffs r);
number  nvInvers      (number c, const coeffs r);
void    nvPower       (number a, int i, number * result, const coeffs r);
#endif

#endif
