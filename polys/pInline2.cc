/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline2.cc
 *  Purpose: implementation of poly procs which are of constant time
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PINLINE2_CC
#define PINLINE2_CC
#include <kernel/mod2.h>

#ifdef PDEBUG

#if PDEBUG < 2
#undef PDEBUG
#define PDEBUG 2
#endif

#undef NO_INLINE2
#define NO_INLINE2 1

#endif // PDEBUG

#include <kernel/p_polys.h>

#endif
