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
#include <omalloc.h>
#include <coeffs.h>

#define POLYSIZE (sizeof(poly) + sizeof(number))
#define POLYSIZEW (POLYSIZE / sizeof(long))
#if SIZEOF_LONG == 8
#define POLY_NEGWEIGHT_OFFSET (((long)0x80000000) << 32)
#else
#define POLY_NEGWEIGHT_OFFSET ((long)0x80000000)
#endif


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

#endif // POLYS_IMPL_H
