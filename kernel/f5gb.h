/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.9 2008-10-11 12:12:46 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef F5_HEADER
#define F5_HEADER

#ifdef HAVE_F5
#include "lpolynom.h"


// sort polynomials in ideal i by decreasing total degree
void qsort_degree(poly* left, poly* right);


/* computes incrementally gbs of subsets of the input 
** gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}  
*/
lpoly* f5_inc(lpoly* lp, lpoly* g_prev);


// main function of our f5 implementation
ideal F5main(ideal i, ring r);
#endif
#endif

