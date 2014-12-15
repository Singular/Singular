#ifndef COEFFRINGS_H
#define COEFFRINGS_H

#include <misc/auxiliary.h>
#include <coeffs/coeffs.h>
#include <polys/monomials/ring.h>

static FORCE_INLINE number n_Copy(number n, const ring r){ return n_Copy(n, r->cf); }
static FORCE_INLINE void n_Delete(number* p, const ring r){ n_Delete(p, r->cf); }
static FORCE_INLINE BOOLEAN n_Equal(number a, number b, const ring r){ return n_Equal(a, b, r->cf); }
static FORCE_INLINE nMapFunc n_SetMap(const ring src, const ring dst){ return n_SetMap(src->cf,dst->cf); }
static FORCE_INLINE int n_GetChar(const ring r){ return n_GetChar(r->cf); }

// static FORCE_INLINE BOOLEAN n_Test(number n, const char *filename, const int linenumber, const ring r){ return n_Test( n, r->cf); }
// static FORCE_INLINE BOOLEAN n_Test(number a, const ring r){  return n_Test(a, r); }
// #define n_Test(a,r)



static FORCE_INLINE BOOLEAN n_IsZero(number n, const ring r){ return n_IsZero(n,r->cf); }
static FORCE_INLINE BOOLEAN n_IsOne(number n,  const ring r){ return n_IsOne(n,r->cf); }
static FORCE_INLINE BOOLEAN n_IsMOne(number n, const ring r){ return n_IsMOne(n,r->cf); }
static FORCE_INLINE BOOLEAN n_GreaterZero(number n, const ring r){ return n_GreaterZero(n,r->cf); }
static FORCE_INLINE number n_Init(int i,       const ring r){ return n_Init(i,r->cf); }
static FORCE_INLINE number n_InpNeg(number n,     const ring r){ return n_InpNeg(n,r->cf); }
static FORCE_INLINE number n_Invers(number a,  const ring r){ return n_Invers(a,r->cf); }
static FORCE_INLINE int    n_Size(number n,    const ring r){ return n_Size(n,r->cf); }
static FORCE_INLINE void   n_Normalize(number& n, const ring r){ return n_Normalize(n,r->cf); }
static FORCE_INLINE void   n_Write(number& n,  const ring r){ return n_Write(n, r->cf, rShortOut(r)); }
static FORCE_INLINE number n_GetDenom(number& n, const ring r){ return n_GetDenom(n, r->cf);}
static FORCE_INLINE number n_GetNumerator(number& n, const ring r){ return n_GetNumerator(n, r->cf);}
static FORCE_INLINE void   n_Power(number a, int b, number *res, const ring r){ n_Power(a,b,res,r->cf); }
static FORCE_INLINE number n_Mult(number a, number b, const ring r){ return n_Mult(a, b, r->cf);}
static FORCE_INLINE void n_InpMult(number &a, number b, const ring r){ n_InpMult(a,b,r->cf); }
static FORCE_INLINE number n_Sub(number a, number b, const ring r){ return n_Sub(a, b, r->cf);}
static FORCE_INLINE number n_Add(number a, number b, const ring r){ return n_Add(a, b, r->cf);}
static FORCE_INLINE number n_Div(number a, number b, const ring r){ return n_Div(a,b, r->cf);}
static FORCE_INLINE number n_ExactDiv(number a, number b, const ring r){ return n_ExactDiv(a,b, r->cf);}
static FORCE_INLINE number n_Gcd(number a, number b, const ring r){ return n_Gcd(a,b, r->cf);}

#ifdef HAVE_RINGS
static FORCE_INLINE BOOLEAN n_IsUnit(number n, const ring r){ return n_IsUnit(n, r->cf);}
static FORCE_INLINE number n_GetUnit(number n, const ring r){ return n_GetUnit(n, r->cf);}
static FORCE_INLINE BOOLEAN n_DivBy(number a, number b, const ring r){ return n_DivBy(a,b, r->cf);}
#endif

static FORCE_INLINE int n_ParDeg(number n, const ring r){ assume(r != NULL); assume(r->cf != NULL); return n_ParDeg(n,r->cf); }

#endif /* COEFFRINGS_H */
