/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pInline.cc
 *  Purpose: implementation of poly Level 0 functions
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PINLINE0_H
#define PINLINE0_H

#if defined(DO_PINLINE0)
#define PINLINE0 static inline
#else
#define PINLINE0
#endif

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>

/***************************************************************
 *
 * poly things which are independent of ring
 *
 ***************************************************************/


/*2
* returns the length of a (numbers of monomials)
* respect syzComp
*/
poly pLast(poly a, int &l);

#endif // PINLINE_CC

