/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.16 2009-01-25 17:13:06 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef F5_HEADER
#define F5_HEADER

#ifdef HAVE_F5
#include "lpolynomial.h"
#include "lists.h"


/*
======================================================
sort polynomials in ideal i by decreasing total degree
======================================================
*/
void qsort_degree(poly* left, poly* right);

/*
==============================================
generating the list lp of ideal generators and 
test if 1 is in lp(return 1) or not(return 0)
==============================================
*/
void generate_input_list(LPoly* lp, ideal id, poly one);

/*
==================================================
computes incrementally gbs of subsets of the input 
gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}  
==================================================
*/
LList* F5inc(int* i, poly* f_i, LList* gPrev, poly* ONE);

/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
CList* criticalPair(LList* gPrev);

/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
bool criterion1(LNode* l, LList* gPrev);
/*
======================================
main function of our f5 implementation
======================================
*/
ideal F5main(ideal i, ring r);

#endif
#endif
