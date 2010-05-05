#ifndef NUMBERS_H
#define NUMBERS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: interface to coefficient aritmetics
*/
#include "coeffs.h"

/// Returns the type of coeffs domain
static inline n_coeffType getCoeffType(const coeffs r)
{
  return r->type;
}

static inline int nInternalChar(const coeffs r)
{
  return r->ch;
}

#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits

#define n_Copy(n, r)          (r)->cfCopy(n,r)
#define n_Delete(n, r)        (r)->cfDelete(n,r)
#define n_Mult(n1, n2, r)     (r)->nMult(n1, n2,r)
#define n_Add(n1, n2, r)      (r)->nAdd(n1, n2,r)
#define n_IsZero(n, r)        (r)->nIsZero(n,r)
#define n_Equal(n1, n2, r)    (r)->nEqual(n1, n2,r)
#define n_Neg(n, r)           (r)->nNeg(n,r)
#define n_Sub(n1, n2, r)      (r)->nSub(n1, n2,r)
#define n_GetChar(r)          ((r)->ch)
#define n_Init(i, r)          (r)->cfInit(i,r)
#define n_IsOne(n, r)         (r)->nIsOne(n,r)
#define n_IsMOne(n, r)        (r)->nIsMOne(n,r)
#define n_GreaterZero(n, r)   (r)->nGreaterZero(n,r)
#define n_Write(n, r)         (r)->cfWrite(n,r)
#define n_Normalize(n, r)     (r)->nNormalize(n,r)
#define n_Gcd(a, b, r)        (r)->nGcd(a,b,r)
#define n_IntDiv(a, b, r)     (r)->nIntDiv(a,b,r)
#define n_Div(a, b, r)        (r)->nDiv(a,b,r)
#define n_Invers(a, r)        (r)->nInvers(a,r)
#define n_ExactDiv(a, b, r)   (r)->nExactDiv(a,b,r)
#define n_Test(a,r)           (r)->nDBTest(a,r,__FILE__,__LINE__)

#define n_InpMult(a, b, r)    (r)->nInpMult(a,b,r)
#define n_Power(a, b, res, r) (r)->nPower(a,b,res,r)
#define n_Size(n,r)           (r)->nSize(n,r)
#define n_GetDenom(N,r)       (r)->cfGetDenom((N),r)
#define n_GetNumerator(N,r)   (r)->cfGetNumerator((N),r)

#define n_New(n, r)           nNew(n)

/* variables */
extern unsigned short fftable[];

/* prototypes */
void           nNew(number * a);
#define        nInit(i) n_Init(i,currRing)
#define nWrite(A) n_Write(A,currRing)

#define nTest(a) (1)

// please use n_* counterparts instead!!!
// #define nDelete(A) (currRing)->cf->cfDelete(A,currRing)
// #define nGetDenom(N) (currRing->cf->cfGetDenom((N),currRing))
// #define nGetNumerator(N) (currRing->cf->cfGetNumerator((N),currRing))


#define nSetMap(R) (currRing->cf->cfSetMap(R,currRing))

/* the dummy routines: */
void nDummy1(number* d);
void ndDelete(number* d, const coeffs r);
number ndGcd(number a, number b, const coeffs);
number ndCopy(number a, const coeffs r);
number ndCopyMap(number a, const coeffs src, const coeffs dst);
int ndSize(number a, const coeffs r);
char * ndName(number n, const coeffs r);
number ndPar(int i, const coeffs r);
int    ndParDeg(number n, const coeffs r);
number ndGetDenom(number &n, const coeffs r);
number ndGetNumerator(number &a,const coeffs r);
number ndReturn0(number n, const coeffs r);
number ndIntMod(number a, number b, const coeffs r);

void   ndInpMult(number &a, number b, const coeffs r);
number ndInpAdd(number &a, number b, const coeffs r);

#ifdef LDEBUG
void nDBDummy1(number* d,char *f, int l);
#endif
#define nGetChar() n_GetChar(currRing)

void nInitChar(coeffs r); // do one-time initialisations
void nKillChar(coeffs r); // undo all initialisations
void nSetChar(coeffs r); // initialisations after each ring chage

#define nDivBy0 "div by 0"

// dummy routines
void   ndNormalize(number& d, const coeffs); // nNormalize...

// Tests:
static inline BOOLEAN nField_is_Ring_2toM(const coeffs r)
{ return (r->ringtype == 1); }

static inline BOOLEAN nField_is_Ring_ModN(const coeffs r)
{ return (r->ringtype == 2); }

static inline BOOLEAN nField_is_Ring_PtoM(const coeffs r)
{ return (r->ringtype == 3); }

static inline BOOLEAN nField_is_Ring_Z(const coeffs r)
{ return (r->ringtype == 4); }

static inline BOOLEAN nField_is_Ring(const coeffs r)
{ return (r->ringtype != 0); }

static inline BOOLEAN nField_is_Domain(const coeffs r)
{ return (r->ringtype == 4 || r->ringtype == 0); }

static inline BOOLEAN nField_has_Units(const coeffs r)
{ return ((r->ringtype == 1) || (r->ringtype == 2) || (r->ringtype == 3)); }

static inline BOOLEAN nField_is_Zp(const coeffs r)
{ return getCoeffType(r)==n_Zp; }

static inline BOOLEAN nField_is_Zp(const coeffs r, int p)
{ return (getCoeffType(r)  && (r->ch == ABS(p))); }

static inline BOOLEAN nField_is_Q(const coeffs r)
{ return getCoeffType(r)==n_Q; }

static inline BOOLEAN nField_is_numeric(const coeffs r) /* R, long R, long C */
{  return (getCoeffType(r)==n_R) || (getCoeffType(r)==n_long_R) || (getCoeffType(r)==n_long_C); }

static inline BOOLEAN nField_is_R(const coeffs r)
{ return getCoeffType(r)==n_R; }

static inline BOOLEAN nField_is_GF(const coeffs r)
{ return getCoeffType(r)==n_GF; }

static inline BOOLEAN nField_is_GF(const coeffs r, int q)
{ return (getCoeffType(r)==n_GF) && (r->ch == q); }

static inline BOOLEAN nField_is_Zp_a(const coeffs r)
{ return (r->ringtype == 0) && (r->ch < -1); }

static inline BOOLEAN nField_is_Zp_a(const coeffs r, int p)
{ return (r->ringtype == 0) && (r->ch < -1 ) && (-(r->ch) == ABS(p)); }

static inline BOOLEAN nField_is_Q_a(const coeffs r)
{ return (r->ringtype == 0) && (r->ch == 1); }

static inline BOOLEAN nField_is_long_R(const coeffs r)
{ return getCoeffType(r)==n_long_R; }

static inline BOOLEAN nField_is_long_C(const coeffs r)
{ return getCoeffType(r)==n_long_C; }

static inline BOOLEAN nField_has_simple_inverse(const coeffs r)
/* { return (r->ch>1) || (r->ch== -1); } *//* Z/p, GF(p,n), R, long_R, long_C*/
#ifdef HAVE_RINGS
{ return (r->ringtype > 0) || (r->ch>1) || ((r->ch== -1) && (r->float_len < 10)); } /* Z/2^n, Z/p, GF(p,n), R, long_R, long_C*/
#else
{ return (r->ch>1) || ((r->ch== -1) && (r->float_len < 10)); } /* Z/p, GF(p,n), R, long_R, long_C*/
#endif

static inline BOOLEAN nField_has_simple_Alloc(const coeffs r)
{ return (nField_is_Zp(r)
       || nField_is_GF(r)
#ifdef HAVE_RINGS
       || nField_is_Ring_2toM(r)
#endif
       || nField_is_R(r));
}
/* Z/p, GF(p,n), R: nCopy, nNew, nDelete are dummies*/

static inline BOOLEAN nField_is_Extension(const coeffs r)
{ return (nField_is_Q_a(r)) || (nField_is_Zp_a(r)); } /* Z/p(a) and Q(a)*/




#endif
