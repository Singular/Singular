#ifndef POLYS_IMPL_H
#define POLYS_IMPL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.h,v 1.52 2000-09-13 10:57:34 Singular Exp $ */

/***************************************************************
 *
 * File:       polys-impl.h
 * Purpose:    low-level definition and declarations for polys
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.h should be used, instead.
 *
 ***************************************************************/
#include "structs.h"
#include "omalloc.h"

/***************************************************************
 *
 * TODO:
 *
 ***************************************************************/
#define rRing_has_Comp(r)   (r->pCompIndex >= 0)
extern void p_Setm(poly p, ring r);
#define p_SetmComp(p, r) p_Setm(p, r)

/***************************************************************
 *
 * definition of the poly structure and its fields
 *
 ***************************************************************/

#ifndef NDEBUG
#define VARS (10)   /*max. number of variables as constant*/
#else
#define VARS (0)
#endif

typedef Exponent_t* Exponent_pt;
struct  spolyrec
{
  poly      next; // next needs to be the first field
  number    coef; // and coef the second --- do not change this !!!
  unsigned long exp[VARS];  // make sure that exp is aligned
};

/***************************************************************
 *
 * What should be inlined?
 *
 ***************************************************************/
// determines inlining of poly procs which iter through polys
#if defined(DO_PINLINE0) && !defined(PDEBUG)
#define PINLINE0 static inline
#else
#define PINLINE0
#endif

// determines inlining of poly procs which iter over ExpVector
#undef NO_PINLINE1
#if PDEBUG <= 0 && !defined(NO_INLINE1)
#define PINLINE1 static inline
#else
#define PINLINE1
#define NO_PINLINE1 1
#endif

// determines inlining of constant time poly procs
#undef NO_PINLINE2
#if PDEBUG <= 1 && !defined(NO_INLINE2)
#define PINLINE2 static inline
#else
#define PINLINE2
#define NO_PINLINE2 1
#endif

// determines inlining of stuff from polys-impl.h
#undef NO_PINLINE3
#if PDEBUG <= 2 && !defined(NO_INLINE3)
#define PINLINE3 static inline
#else
#define PINLINE3
#define NO_PINLINE3 1
#endif

/***************************************************************
 *
 * prepare debugging
 *
 ***************************************************************/

#if defined(PDEBUG)
#define passume assume
PINLINE3 int p_IsFromRing(poly p, ring r);

#if !defined(NO_PINLINE3) || defined(POLYS_IMPL_CC)
PINLINE3 int p_IsFromRing(poly p, ring r)
{
  void* custom = omGetCustomOfAddr(p);
  if (custom != NULL)
  {
    return custom == r;
  }
  else
  {
    if (omIsBinPageAddr(p) && omSizeWOfAddr(p) == r->PolyBin->sizeW)
      return 1;
  }
  return 0;
}
#endif
#else
#define passume(x) ((void)0)
#define p_IsFromRing(p, r)  (1)
#endif

#if PDEBUG > 1 || P_CHECK_RING
PINLINE3 void p_CheckPolyRing(poly p, ring r);
PINLINE3 void p_CheckRing(ring r);
#if !defined(NO_PINLINE3) || defined(POLYS_IMPL_CC)
PINLINE3 void p_CheckPolyRing(poly p, ring r)
{
  passume(r != NULL && r->PolyBin != NULL);
  passume(p != NULL);
  passume(p_IsFromRing(p, r));
}
PINLINE3 void p_CheckRing(ring r)
{
  passume(r != NULL && r->PolyBin != NULL);
}
#endif
#define p_CheckIf(cond, what) do{ if (cond) {what;}} while(0)
#else
#define p_CheckPolyRing(p, r)   ((void)0)
#define p_CheckRing(r)          ((void)0)
#define p_CheckIf(cond, what)   ((void)0)
#endif

#if OM_TRACK > 2 && (PDEBUG > 1 || P_CHECK_RING > 0)
#define p_SetRingOfPoly(p, r) omSetCustomOfAddr(p, r)
#else
#define p_SetRingOfPoly(p, r) ((void)0)
#endif

#if PDEBUG > 0
#define passume1 assume
#else
#define passume1(x) ((void)0)
#endif

#if PDEBUG > 1
#define passume2 assume
#else
#define passume2(x) ((void)0)
#endif

/***************************************************************
 *
 * Macros for access/iteration
 *
 ***************************************************************/
#define _pNext(p)           ((p)->next)
#define _pIter(p)           ((p) = (p)->next)

// coeff
#define _pGetCoeff(p)       ((p)->coef)
#define _pSetCoeff0(p,n)    (p)->coef=n
#define __p_GetComp(p, r)   ((long) (p)->exp[r->pCompIndex])
#define _p_GetComp(p, r)    (r->pCompIndex >= 0 ? __p_GetComp(p, r) : 0)



#define POLYSIZE (sizeof(poly) + sizeof(number))
#define POLYSIZEW (POLYSIZE / sizeof(long))
#define MAX_EXPONENT_NUMBER ((MAX_BLOCK_SIZE - POLYSIZE) / SIZEOF_EXPONENT)

// number of Variables
extern int pVariables;
// size of a monom in bytes - always a multiple of sizeof(void*)
extern int pMonomSize;
// size of a monom in units of sizeof(void*) -- i.e. in words
extern int pMonomSizeW;
// Ceiling((pVariables+1) / sizeof(void*)) == length of exp-vector in words
// extern int pVariables1W;
// Ceiling((pVariables) / sizeof(void*))
// extern int pVariablesW;
extern int *pVarOffset;
// extern int pVarLowIndex;
// extern int pVarHighIndex;
// extern int pVarCompIndex;
extern omBin currPolyBin;

#endif // POLYS_IMPL_H
