#ifndef RATGRING_H
#define RATGRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT additional defines etc for --with-plural
*/
// #define HAVE_RATGRING to activate

#ifdef HAVE_RATGRING
#include <kernel/structs.h>
#include <kernel/gring.h>

/* MACROS */

/* the part, related to the interface */ 

/* ring nc_rCreateNCcomm(ring r); */

void pLcmRat(poly a, poly b, poly m, int rat_shift);

poly p_LcmRat(const poly a, const poly b, const long lCompM, const ring r);

poly p_GetCoeffRat(poly p, int ishift, ring r);

poly p_HeadRat(poly p, int ishift, ring r);

void p_LmDeleteAndNextRat(poly *p, int ishift, ring r);

void p_ExpVectorDiffRat(poly pr, poly p1, poly p2, int ishift, ring r);

ideal ncGCD2(poly p, poly q, ring r); // real nc stuff

ideal ncGCD(poly p, poly q, ring r); // for p,q from a commutative ring

poly nc_rat_CreateSpoly(poly p1, poly p2, int ishift, ring r);

poly nc_rat_ReduceSpolyNew(poly p1, poly p2, int ishift, ring r);


/* poly functions defined in p_Procs : */
// poly nc_pp_Mult_mm(poly p, poly m, const ring r, poly &last);
// poly nc_p_Mult_mm(poly p, const poly m, const ring r);
// poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, const ring r);
// poly nc_p_Minus_mm_Mult_qq_ign(poly p, const poly m, poly q, int & d1, poly d2, const ring ri, poly &d3);

/* other routines we need in addition : */
// poly nc_p_Mult_mm_Common(poly p, const poly m, int side, const ring r);
// poly nc_mm_Mult_p(const poly m, poly p, const ring r);
// poly nc_mm_Mult_nn (int *F, int *G, const ring r); 
// poly nc_mm_Mult_uu (int *F,int jG,int bG, const ring r);

// /* subst: */
// poly nc_pSubst(poly p, int n, poly e);

// /* copy : */
// poly nc_p_CopyGet(poly a, const ring r);
// poly nc_p_CopyPut(poly a, const ring r);

// /* syzygies : */
// /* former nc_spGSpolyCreate */
// poly nc_CreateSpoly(poly p1, poly p2, poly spNoether, const ring r);
// /* former nc_spGSpolyRed */
// poly nc_ReduceSpoly(poly p1, poly p2, poly spNoether, const ring r);
// /* former nc_spGSpolyRedNew */
// poly nc_ReduceSpolyNew(poly p1, poly p2, poly spNoether, const ring r);
// /* former nc_spGSpolyRedTail */
// void nc_ReduceSpolyTail(poly p1, poly q, poly q2, poly spNoether, const ring r);
// /* former nc_spShort */
// poly nc_CreateShortSpoly(poly p1, poly p2, const ring r=currRing);

// ideal gr_bba (ideal F, ideal Q,kStrategy strat);

// /* brackets: */
// poly nc_p_Bracket_qq(poly p, poly q);
// poly nc_mm_Bracket_nn(poly m1, poly m2);

// /* twostd: */
// ideal twostd(ideal I);
// /* Ann: */
// ideal Approx_Step(ideal L);

// /* complete reduction routines */

// /* void nc_kBucketPolyRed(kBucket_pt b, poly p); */
// void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c);
// void nc_kBucketPolyRed_Z(kBucket_pt b, poly p, number *c);
// void nc_PolyPolyRed(poly &b, poly p, number *c);

// matrix nc_PrintMat(int a, int b, ring r, int metric);

// poly p_CopyEmbed(poly p, ring srcRing, int shift, int par_shift);
// poly pOppose(ring Rop, poly p);
// ideal idOppose(ring Rop, ideal I);

// #else
// /* dummy definition to make gcc happy */
// #define nc_kBucketPolyRed(A,B,C) 0
// #define nc_PolyPolyRed(A,B,C) 0

// return: FALSE, if there exists i in ishift..r->N,
//                 such that a->exp[i] > b->exp[i]
//         TRUE, otherwise
BOOLEAN p_DivisibleByRat(poly a, poly b, int ishift, const ring r);

/*2
*reduces h with elements from reducer choosing the best possible
* element in t with respect to the given red_length
* arrays reducer and red_length are [0..(rl-1)]
*/
int redRat (poly* h,poly *reducer, int *red_length,int rl, int ishift, ring r);

// Content stuff

void pContentRat(poly &ph);

BOOLEAN p_LmIsConstantRat(const poly p, const ring r);

BOOLEAN p_LmIsConstantCompRat(const poly p, const ring r);

#endif /* HAVE_PLURAL */
#endif
