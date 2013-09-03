/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/***************************************************************
 *
 * File:       polys-impl.cc
 * Purpose:    low-level procuders for polys.
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.[h,cc] should be used, instead.
 *
 ***************************************************************/
#ifndef POLYS_IMPL_CC
#define POLYS_IMPL_CC

#include <stdio.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "libpolysconfig.h"
#endif /* HAVE_CONFIG_H */

#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>

#ifdef PDEBUG
#undef NO_INLINE3
#define NO_INLINE3
#endif


#endif // POLYS_IMPL_CC
