#ifndef COEFFRINGS_H
#define COEFFRINGS_H

#include <misc/auxiliary.h>
#include <coeffs/coeffs.h>
#include <polys/monomials/ring.h>

static inline number n_Copy(number n, const ring r){ return n_Copy(n, r->cf); }
static inline void n_Delete(number* p, const ring r){ n_Delete(p, r->cf); }
static inline BOOLEAN n_Equal(number a, number b, const ring r){ return n_Equal(a, b, r->cf); }
static inline nMapFunc n_SetMap(const ring src, const ring dst){ return dst->cf->cfSetMap(src->cf,dst->cf); }
static inline int n_GetChar(const ring r){ return n_GetChar(r->cf); }

// static inline BOOLEAN n_Test(number n, const char *filename, const int linenumber, const ring r){ return n_DBTest( n, filename, linenumber, r->cf); }
// static inline BOOLEAN n_Test(number a, const ring r){  return n_DBTest(a, __FILE__, __LINE__, r); }
// #define n_Test(a,r)  



static inline BOOLEAN n_IsZero(number n, const ring r){ return n_IsZero(n,r->cf); }
static inline BOOLEAN n_IsOne(number n,  const ring r){ return n_IsOne(n,r->cf); }
static inline BOOLEAN n_IsMOne(number n, const ring r){ return n_IsMOne(n,r->cf); }
static inline BOOLEAN n_GreaterZero(number n, const ring r){ return n_GreaterZero(n,r->cf); }
static inline number n_Init(int i,       const ring r){ return n_Init(i,r->cf); }
static inline number n_Neg(number n,     const ring r){ return n_Neg(n,r); }
static inline number n_Invers(number a,  const ring r){ return n_Invers(a,r); }
static inline int    n_Size(number n,    const ring r){ return n_Size(n,r); }
static inline void   n_Normalize(number& n, const ring r){ return n_Normalize(n,r); }
static inline void   n_Write(number& n,  const ring r){ return n_Write(n,r); }
static inline number n_GetDenom(number& n, const ring r){ return n_GetDenom(n, r->cf);}
static inline number n_GetNumerator(number& n, const ring r){ return n_GetNumerator(n, r->cf);}
static inline void   n_Power(number a, int b, number *res, const ring r){ n_Power(a,b,res,r); }
static inline number n_Mult(number a, number b, const ring r){ return n_Mult(a, b, r->cf);}
static inline void n_InpMult(number &a, number b, const ring r){ n_InpMult(a,b,r); }
static inline number n_Sub(number a, number b, const ring r){ return n_Sub(a, b, r->cf);}
static inline number n_Add(number a, number b, const ring r){ return n_Add(a, b, r->cf);}
static inline number n_Div(number a, number b, const ring r){ return n_Div(a,b, r->cf);}
static inline number n_IntDiv(number a, number b, const ring r){ return n_IntDiv(a,b, r->cf);}
static inline number n_ExactDiv(number a, number b, const ring r){ return n_ExactDiv(a,b, r->cf);}
static inline number n_Gcd(number a, number b, const ring r){ return n_Gcd(a,b, r->cf);}
static inline number n_Lcm(number a, number b, const ring r){ return n_Lcm(a,b, r->cf);}

#ifdef HAVE_RINGS
static inline BOOLEAN n_IsUnit(number n, const ring r){ return n_IsUnit(n, r->cf);}
static inline number n_GetUnit(number n, const ring r){ return n_GetUnit(n, r->cf);}
static inline BOOLEAN n_DivBy(number a, number b, const ring r){ return n_DivBy(a,b, r->cf);}
#endif



#endif /* COEFFRINGS_H */
