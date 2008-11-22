/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.11 2008-11-22 20:48:23 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef F5_HEADER
#define F5_HEADER

#ifdef HAVE_F5
#include "lpolynomial.h"


// sort polynomials in ideal i by decreasing total degree
 void qsort_degree(poly* left, poly* right);


// generating the list lp of ideal generators and 
// test if 1 is in lp(return 1) or not(return 0)
 bool generate_input_list(lpoly* lp, ideal id, poly one);

/* computes incrementally gbs of subsets of the input 
   gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}  
*/
// lpoly* f5_inc(lpoly* lp, lpoly* g_prev);


// main function of our f5 implementation
ideal F5main(ideal i, ring r);
#endif
#endif

