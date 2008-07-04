/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAMult.cc
 *  Purpose: implementation of multiplication in simple NC subalgebras
 *  Author:  motsak
 *  Created: 
 *  Version: $Id: ncSAMult.cc,v 1.1 2008-07-04 16:18:42 motsak Exp $
 *******************************************************************/


#define MYTEST 0
#define OUTPUT 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#include "mod2.h"

#include <ncSAMult.h> // for CMultiplier etc classes
#include <ring.h>
#include <p_polys.h>
#include <sbuckets.h>




/*
// poly functions defined in p_Procs: ;
poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly
&last);
poly gnc_p_Mult_mm(poly p, const poly m, const ring r);
poly gnc_mm_Mult_p(const poly m, poly p, const ring r);
poly gnc_mm_Mult_pp(const poly m, const poly p, const ring r);
*/
