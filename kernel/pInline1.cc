/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline1.cc
 *  Purpose: implementation of poly procs which iter over ExpVector
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PINLINE1_CC
#define PINLINE1_CC

#include <kernel/mod2.h>

#ifdef PDEBUG

#if PDEBUG < 1
#undef PDEBUG
#define PDEBUG 1
#endif

#undef NO_INLINE1
#define NO_INLINE1

#endif // PDEBUG

#include <kernel/p_polys.h>

#endif // PINLINE1_CC

