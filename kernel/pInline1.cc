/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline1.cc
 *  Purpose: implementation of poly procs which iter over ExpVector
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pInline1.cc,v 1.1.1.1 2003-10-06 12:15:58 Singular Exp $
 *******************************************************************/
#ifndef PINLINE1_CC
#define PINLINE1_CC

#include "mod2.h"

#ifdef PDEBUG

#if PDEBUG < 1
#undef PDEBUG
#define PDEBUG 1
#endif

#undef NO_INLINE1
#define NO_INLINE1

#endif // PDEBUG

#include "p_polys.h"

#endif // PINLINE1_CC

