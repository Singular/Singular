/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.25 2009-02-12 12:43:31 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef F5_HEADER
#define F5_HEADER

#ifdef HAVE_F5
#include "f5data.h"
#include "f5lists.h"


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
LList* F5inc(int i, poly f_i, LList* gPrev, ideal gbPrev, poly ONE, LTagList* lTag, RList* rules, RTagList* rTag);

/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
CList* criticalPair(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag, RList* rules);

/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
bool criterion1(poly* t, LNode* l, LTagList* lTag);

/*
=====================================
Criterion 2, i.e. Rewritten Criterion
=====================================
*/
bool criterion2(poly* t, LNode* l, RList* rules, RTagList* rTag);

/*
==========================================================================================================
Criterion 2, i.e. Rewritten Criterion, for its second call in sPols(), with added lastRuleTested parameter
==========================================================================================================
*/
bool criterion2(poly* t, LPoly* l, RList* rules, Rule* lastRuleTested);

/*
==================================
Computation of S-Polynomials in F5
==================================
*/
void computeSPols(CNode* first, RTagList* rTag, RList* rules, LList* sPolyList);

/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
LList* reduction(LList* sPolyList, LList* completed, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag,
                 ideal gbPrev);

/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
TopRed* topReduction(LNode* l, LList* completed, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag); 

/*
=====================================================================
subalgorithm to find a possible reductor for the labeled polynomial l
=====================================================================
*/
LNode* findReductor(LNode* l,LList* completed,LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag,
                    LNode* gPrevRedCheck, LNode* completedRedCheck);

/*
======================================
main function of our F5 implementation
======================================
*/
ideal F5main(ideal i, ring r);

#endif
#endif
