#ifndef POLYS_IMPL_H
#define POLYS_IMPL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/***************************************************************
 *
 * File:       polys-impl.h
 * Purpose:    low-level and macro definition of polys
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.h should be used, instead.
 *
 ***************************************************************/
#include <kernel/structs.h>
#include <omalloc.h>

/***************************************************************
 *
 * definition of the poly structure and its fields
 *
 ***************************************************************/

#ifndef NDEBUG
#define VARS (10)   /*max. number of variables as constant*/
#else
#ifdef __GNUC__
#define VARS (0)
#else
#define VARS (1)
#endif
#endif

struct  spolyrec
{
  poly      next;           // next needs to be the first field
  number    coef;           // and coef the second --- do not change this !!!
  unsigned long exp[VARS];  // make sure that exp is aligned
};
#define POLYSIZE (sizeof(poly) + sizeof(number))
#define POLYSIZEW (POLYSIZE / sizeof(long))
#if SIZEOF_LONG == 8
#define POLY_NEGWEIGHT_OFFSET (((long)0x80000000) << 32)
#else
#define POLY_NEGWEIGHT_OFFSET ((long)0x80000000)
#endif


/***************************************************************
 *
 * What should be inlined and debugged?
 *
 ***************************************************************/
#ifdef NO_PDEBUG
#undef PDEBUG
#endif

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

#define pPolyAssumeReturn(cond, p, r)                       \
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

#define pPolyAssume(cond,p,r)                                   \
do                                                              \
{                                                               \
  if (! (cond))                                                 \
  {                                                             \
    dPolyReportError(p, r, "pPolyAssume violation of: %s",    \
                 #cond);                                        \
  }                                                             \
}                                                               \
while (0)

#define pPolyAssumeReturnMsg(cond, msg, p, r)   \
do                                              \
{                                               \
  if (! (cond))                                 \
  {                                             \
    dPolyReportError(p, r, "%s ",  msg);        \
    return FALSE;                               \
  }                                             \
}                                               \
while (0)

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
#define pPolyAssume(cond, p,r)      ((void)0)
#define pPolyAssumeReturn(cond,p,r) ((void)0)
#define p_SetRingOfLm(p, r)          ((void)0)
#define p_SetRingOfLeftv(l, r)       ((void)0)
#endif // defined(PDEBUG)

#if PDEBUG >= 1
#define pAssume1             pAssume
#define pPolyAssume1         pPolyAssume
#define pAssumeReturn1       pAssumeReturn
#define pPolyAssumeReturn1   pPolyAssumeReturn
#define _pPolyAssumeReturn1  _pPolyAssumeReturn
#define p_LmCheckPolyRing1    p_LmCheckPolyRing
#define p_CheckRing1        p_CheckRing
#define pIfThen1          pIfThen
#else
#define pAssume1(cond)               ((void)0)
#define pPolyAssume1(cond)           ((void)0)
#define pAssumeReturn1(cond)         ((void)0)
#define pPolyAssumeReturn1(cond)     ((void)0)
#define p_LmCheckPolyRing1(p,r)       ((void)0)
#define p_CheckRing1(r)             ((void)0)
#define pIfThen1(cond, check)     ((void)0)
#endif // PDEBUG >= 1

#if PDEBUG >= 2
#define pAssume2             pAssume
#define pPolyAssume2         pPolyAssume
#define pAssumeReturn2       pAssumeReturn
#define pPolyAssumeReturn2   pPolyAssumeReturn
#define p_LmCheckPolyRing2    p_LmCheckPolyRing
#define p_CheckRing2        p_CheckRing
#define pIfThen2          pIfThen
#else
#define pAssume2(cond)                   ((void)0)
#define pPolyAssume2(cond,p,r)           ((void)0)
#define pAssumeReturn2(cond,p,r)         ((void)0)
#define pPolyAssumeReturn2(cond,p,r)     ((void)0)
#define p_LmCheckPolyRing2(p,r)          ((void)0)
#define p_CheckRing2(r)                  ((void)0)
#define pIfThen2(cond, check)            ((void)0)
#endif // PDEBUG >= 2

/***************************************************************
 *
 * Macros for access/iteration
 *
 ***************************************************************/

#define __p_GetComp(p, r)   (p)->exp[r->pCompIndex]
#define _p_GetComp(p, r)    ((long) (r->pCompIndex >= 0 ? __p_GetComp(p, r) : 0))

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

/***************************************************************
 *
 * Misc macros
 *
 ***************************************************************/
#define rRing_has_Comp(r)   (r->pCompIndex >= 0)

// number of Variables
extern int pVariables;

#endif // POLYS_IMPL_H
