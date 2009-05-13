/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.39 2009-05-13 16:55:03 ederc Exp $ */
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
void qsortDegree(poly* left, poly* right);

/*!
 * ======================================================================
 * builds the sum of the entries of the exponent vectors, i.e. the degree
 * of the corresponding monomial
 * ======================================================================
*/
long sumVector(int* v, int k);

/**
==========================================================================
compare monomials, i.e. divisibility tests for criterion 1 and criterion 2
==========================================================================
*/
bool compareMonomials(int* m1, int** m2, int numberOfRules);

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
inline LList* F5inc(int i, poly f_i, LList* gPrev, ideal gbPrev, poly ONE, LTagList* lTag, RList* rules, RTagList* rTag);

/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
void criticalPair(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag, RList* rules);

/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
inline bool criterion1(LList* gPrev, poly t, LNode* l, LTagList* lTag);

/*
=====================================
Criterion 2, i.e. Rewritten Criterion
=====================================
*/
inline bool criterion2(int idx, poly t, LNode* l, RList* rules, RTagList* rTag);

/*
==========================================================================================================
Criterion 2, i.e. Rewritten Criterion, for its second call in sPols(), with added lastRuleTested parameter
==========================================================================================================
*/
inline bool criterion2(poly t, LPoly* l, RList* rules, Rule* testedRule);

/*
==================================
Computation of S-Polynomials in F5
==================================
*/
inline void computeSPols(CNode* first, RTagList* rTag, RList* rules, LList* sPolyList);

/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
inline void reduction(LList* sPolyList, CList* critPairs, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag,
                 ideal gbPrev);

/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
inline void newReduction(LList* sPolyList, CList* critPairs, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag, ideal gbPrev);

/*!
 * ================================================================================
 * searches for reducers of temp similar to the symbolic preprocessing of F4  and 
 * divides them into a "good" and "bad" part:
 * 
 * the "good" ones are the reducers which do not corrupt the label of temp, with
 * these the normal form of temp is computed
 *
 * the "bad" ones are the reducers which corrupt the label of temp, they are tested 
 * later on for possible new rules and S-polynomials to be added to the algorithm
 * ================================================================================
 */
void findReducers(LNode* l, LList* sPolyList, ideal gbPrev, LList* gPrev, CList* critPairs, RList* rules, LTagList* lTag, RTagList* rTag); 

/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
inline void topReduction(LNode* l, LList* sPolyList, LList* gPrev, CList* critPairs, RList* rules, LTagList* lTag, RTagList* rTag, ideal gbPrev); 

/*
=======================================================================================
merging 2 polynomials p & q without requiring that all monomials of p & q are different
if there are equal monomials in p & q only one of these monomials (always that of p!)
is taken into account
=======================================================================================

poly p_MergeEq_q(poly p, poly q, const ring r);
*/    
/*
=====================================================================
subalgorithm to find a possible reductor for the labeled polynomial l
=====================================================================
*/
inline LNode* findReductor(LNode* l, LList* sPolyList, LNode* gPrevRedCheck, LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag);

/*
======================================
main function of our F5 implementation
======================================
*/
ideal F5main(ideal i, ring r);

#endif
#endif
