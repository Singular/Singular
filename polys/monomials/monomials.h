#ifndef MONOMIALS_H
#define MONOMIALS_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT
*/

#include <omalloc.h>
#include <coeffs.h>
#include "ring.h"

/***************************************************************
 *
 * definition of the poly structure and its fields
 *
 ***************************************************************/

struct spolyrec;
typedef struct spolyrec *          poly;
struct  spolyrec
{
  poly      next;           // next needs to be the first field
  number    coef;           // and coef the second --- do not change this !!!
  unsigned long exp[1];     // make sure that exp is aligned
};

/***************************************************************
 *
 * Macros for access/iteration
 *
 ***************************************************************/
#define pNext(p)           ((p)->next)
#define pIter(p)           ((p) = (p)->next)

// coeff
#define pGetCoeff(p)       ((p)->coef)
#define pSetCoeff0(p,n)    (p)->coef=n
#define __p_GetComp(p, r)   (p)->exp[r->pCompIndex]
#define p_GetComp(p, r)    ((long) (r->pCompIndex >= 0 ? __p_GetComp(p, r) : 0))

/***************************************************************
 *
 * What should be inlined and debugged?
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

extern BOOLEAN dPolyReportError(poly p, ring r, const char* fmt, ...);

// macros for checking of polys
#define pAssumeReturn(cond)                                  \
do                                                          \
{                                                           \
  if (! (cond))                                             \
  {                                                         \
    dPolyReportError(NULL, NULL, "pAssume violation of: %s", \
                 #cond);                                    \
    return FALSE;                                           \
  }                                                         \
}                                                           \
while (0)

#define pAssume(cond)                                        \
do                                                          \
{                                                           \
  if (! (cond))                                             \
  {                                                         \
    dPolyReportError(NULL, NULL, "pAssume violation of: %s", \
                 #cond);                                    \
  }                                                         \
}                                                           \
while (0)

#define _pPolyAssumeReturn(cond, p, r)                       \
do                                                          \
{                                                           \
  if (! (cond))                                             \
  {                                                         \
    dPolyReportError(p, r, "pPolyAssume violation of: %s",   \
                 #cond);                                    \
    return FALSE;                                           \
  }                                                         \
}                                                           \
while (0)

#define _pPolyAssume(cond,p,r)                                   \
do                                                              \
{                                                               \
  if (! (cond))                                                 \
  {                                                             \
    dPolyReportError(p, r, "pPolyAssume violation of: %s",    \
                 #cond);                                        \
  }                                                             \
}                                                               \
while (0)

#define _pPolyAssumeReturnMsg(cond, msg, p, r)   \
do                                              \
{                                               \
  if (! (cond))                                 \
  {                                             \
    dPolyReportError(p, r, "%s ",  msg);        \
    return FALSE;                               \
  }                                             \
}                                               \
while (0)

#define pPolyAssume(cond)        _pPolyAssume(cond, p, r)
#define pPolyAssumeReturn(cond)  _pPolyAssumeReturn(cond, p, r)
#define pPolyAssumeReturnMsg(cond, msg)  _pPolyAssumeReturnMsg(cond, msg, p, r)

#define pFalseReturn(cond)  do {if (! (cond)) return FALSE;} while (0)
#if (OM_TRACK > 2) && defined(OM_TRACK_CUSTOM)
#define p_SetRingOfLm(p, r) omSetCustomOfAddr(p, r)
void p_SetRingOfLeftv(leftv l, ring r);
#else
#define p_SetRingOfLm(p, r) ((void)0)
#define p_SetRingOfLeftv(l, r) ((void)0)
#endif

#else // ! defined(PDEBUG)
#define pFalseReturn(cond)           ((void)0)
#define pAssume(cond)                ((void)0)
#define pPolyAssume(cond)            ((void)0)
#define _pPolyAssume(cond, p,r)      ((void)0)
#define pAssumeReturn(cond)          ((void)0)
#define pPolyAssumeReturn(cond)      ((void)0)
#define _pPolyAssumeReturn(cond,p,r) ((void)0)
#define p_SetRingOfLm(p, r)          ((void)0)
#define p_SetRingOfLeftv(l, r)       ((void)0)
#endif // defined(PDEBUG)

#if PDEBUG >= 1
#define pAssume1             pAssume
#define pPolyAssume1         pPolyAssume
#define _pPolyAssume1        _pPolyAssume
#define pAssumeReturn1       pAssumeReturn
#define pPolyAssumeReturn1   pPolyAssumeReturn
#define _pPolyAssumeReturn1  _pPolyAssumeReturn
#define p_LmCheckPolyRing1    p_LmCheckPolyRing
#define p_CheckRing1        p_CheckRing
#define pIfThen1          pIfThen
#else
#define pAssume1(cond)               ((void)0)
#define pPolyAssume1(cond)           ((void)0)
#define _pPolyAssume1(cond,p,r)      ((void)0)
#define pAssumeReturn1(cond)         ((void)0)
#define pPolyAssumeReturn1(cond)     ((void)0)
#define _pPolyAssumeReturn1(cond,p,r)((void)0)
#define p_LmCheckPolyRing1(p,r)       ((void)0)
#define p_CheckRing1(r)             ((void)0)
#define pIfThen1(cond, check)     ((void)0)
#endif // PDEBUG >= 1

#if PDEBUG >= 2
#define pAssume2             pAssume
#define pPolyAssume2         pPolyAssume
#define _pPolyAssume2        _pPolyAssume
#define pAssumeReturn2       pAssumeReturn
#define pPolyAssumeReturn2   pPolyAssumeReturn
#define _pPolyAssumeReturn2  _pPolyAssumeReturn
#define p_LmCheckPolyRing2    p_LmCheckPolyRing
#define p_CheckRing2        p_CheckRing
#define pIfThen2          pIfThen
#else
#define pAssume2(cond)               ((void)0)
#define pPolyAssume2(cond)           ((void)0)
#define _pPolyAssume2(cond,p,r)      ((void)0)
#define pAssumeReturn2(cond)         ((void)0)
#define pPolyAssumeReturn2(cond)     ((void)0)
#define _pPolyAssumeReturn2(cond,p,r)((void)0)
#define p_LmCheckPolyRing2(p,r)       ((void)0)
#define p_CheckRing2(r)             ((void)0)
#define pIfThen2(cond, check)     ((void)0)
#endif // PDEBUG >= 2

/***************************************************************
 *
 * Macros for low-level allocation
 *
 ***************************************************************/
#ifdef PDEBUG
#define p_AllocBin(p, bin, r)                   \
do                                              \
{                                               \
  omTypeAllocBin(poly, p, bin);                 \
  p_SetRingOfLm(p, r);                        \
}                                               \
while (0)
#define p_FreeBinAddr(p, r) p_LmFree(p, r)
#else
#define p_AllocBin(p, bin, r)   omTypeAllocBin(poly, p, bin)
#define p_FreeBinAddr(p, r)     omFreeBinAddr(p)
#endif

#endif
