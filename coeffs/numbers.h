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

#define SHORT_REAL_LENGTH 6 // use short reals for real <= 6 digits

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
BOOLEAN ndDBTest(number a, const char *f, const int l, const coeffs r);
#endif

#define nDivBy0 "div by 0"

// dummy routines
void   ndNormalize(number& d, const coeffs); // nNormalize...

// Tests:
static inline BOOLEAN nCoeff_is_Ring_2toM(const coeffs r)
{ return (r->ringtype == 1); }

static inline BOOLEAN nCoeff_is_Ring_ModN(const coeffs r)
{ return (r->ringtype == 2); }

static inline BOOLEAN nCoeff_is_Ring_PtoM(const coeffs r)
{ return (r->ringtype == 3); }

static inline BOOLEAN nCoeff_is_Ring_Z(const coeffs r)
{ return (r->ringtype == 4); }

static inline BOOLEAN nCoeff_is_Ring(const coeffs r)
{ return (r->ringtype != 0); }

static inline BOOLEAN nCoeff_is_Domain(const coeffs r)
{ return (r->ringtype == 4 || r->ringtype == 0); }

static inline BOOLEAN nCoeff_has_Units(const coeffs r)
{ return ((r->ringtype == 1) || (r->ringtype == 2) || (r->ringtype == 3)); }

static inline BOOLEAN nCoeff_is_Zp(const coeffs r)
{ return getCoeffType(r)==n_Zp; }

static inline BOOLEAN nCoeff_is_Zp(const coeffs r, int p)
{ return (getCoeffType(r)  && (r->ch == ABS(p))); }

static inline BOOLEAN nCoeff_is_Q(const coeffs r)
{ return getCoeffType(r)==n_Q; }

static inline BOOLEAN nCoeff_is_numeric(const coeffs r) /* R, long R, long C */
{  return (getCoeffType(r)==n_R) || (getCoeffType(r)==n_long_R) || (getCoeffType(r)==n_long_C); }

static inline BOOLEAN nCoeff_is_R(const coeffs r)
{ return getCoeffType(r)==n_R; }

static inline BOOLEAN nCoeff_is_GF(const coeffs r)
{ return getCoeffType(r)==n_GF; }

static inline BOOLEAN nCoeff_is_GF(const coeffs r, int q)
{ return (getCoeffType(r)==n_GF) && (r->ch == q); }

static inline BOOLEAN nCoeff_is_Zp_a(const coeffs r)
{ return (r->ringtype == 0) && (r->ch < -1); }

static inline BOOLEAN nCoeff_is_Zp_a(const coeffs r, int p)
{ return (r->ringtype == 0) && (r->ch < -1 ) && (-(r->ch) == ABS(p)); }

static inline BOOLEAN nCoeff_is_Q_a(const coeffs r)
{ return (r->ringtype == 0) && (r->ch == 1); }

static inline BOOLEAN nCoeff_is_long_R(const coeffs r)
{ return getCoeffType(r)==n_long_R; }

static inline BOOLEAN nCoeff_is_long_C(const coeffs r)
{ return getCoeffType(r)==n_long_C; }

static inline BOOLEAN nCoeff_is_CF(const coeffs r)
{ return getCoeffType(r)==n_CF; }

static inline BOOLEAN nCoeff_has_simple_inverse(const coeffs r)
{ return r->has_simple_Inverse; }
/* Z/2^n, Z/p, GF(p,n), R, long_R, long_C*/

static inline BOOLEAN nCoeff_has_simple_Alloc(const coeffs r)
{ return r->has_simple_Alloc; }
/* Z/p, GF(p,n), R, Ring_2toM: nCopy, nNew, nDelete are dummies*/

static inline BOOLEAN nCoeff_is_Extension(const coeffs r)
{ return (nCoeff_is_Q_a(r)) || (nCoeff_is_Zp_a(r)); } /* Z/p(a) and Q(a)*/

/// initialize an object of type coeff, return FALSE in case of success
typedef BOOLEAN (*cfInitCharProc)(coeffs, void *);
n_coeffType nRegister(n_coeffType n, cfInitCharProc p);

#endif
