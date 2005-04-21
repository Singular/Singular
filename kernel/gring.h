#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.10 2005-04-21 16:16:45 levandov Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#ifdef HAVE_PLURAL
#include "structs.h"

/* MACROS */

#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )

/* the part, related to the interface */ 
BOOLEAN nc_CallPlural(matrix CC, matrix DD, poly CN, poly DN, ring r);
int nc_CheckOrdCondition(matrix D, ring r);
int nc_CheckSubalgebra(poly PolyVar, ring r);
BOOLEAN nc_InitMultiplication(ring r);
BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate);

ring nc_rCreateNCcomm(ring r);
void ncKill(ring r);
void ncCleanUp(ring r); /* smaller than kill */

/* poly functions defined in p_Procs : */
poly nc_pp_Mult_mm(poly p, poly m, const ring r, poly &last);
poly nc_p_Mult_mm(poly p, poly m, const ring r);
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, const ring r);
/* other routines we need in addition : */
poly nc_p_Mult_mm_Common(poly p, const poly m, int side, const ring r);
poly nc_mm_Mult_p(const poly m, poly p, const ring r);
poly nc_mm_Mult_nn (int *F, int *G, const ring r); 
poly nc_mm_Mult_uu (int *F,int jG,int bG, const ring r);

/* #define nc_uu_Mult_ww nc_uu_Mult_ww_vert */
poly nc_uu_Mult_ww (int i, int a, int j, int b, const ring r);
/* poly nc_uu_Mult_ww_vert (int i, int a, int j, int b, const ring r); */
/* poly nc_uu_Mult_ww_horvert (int i, int a, int j, int b, const ring r); */
/* poly nc_uu_Mult_ww_hvdiag (int i, int a, int j, int b, const ring r); */
/* not written yet */

poly _nc_p_Mult_q(poly p, poly q, const int copy, const ring r);

/* subst: */
poly nc_pSubst(poly p, int n, poly e);

/* copy : */
poly nc_p_CopyGet(poly a, const ring r);
poly nc_p_CopyPut(poly a, const ring r);

/* syzygies : */
/* former nc_spGSpolyCreate */
poly nc_CreateSpoly(poly p1, poly p2, poly spNoether, const ring r);
/* former nc_spGSpolyRed */
poly nc_ReduceSpoly(poly p1, poly p2, poly spNoether, const ring r);
/* former nc_spGSpolyRedNew */
poly nc_ReduceSpolyNew(poly p1, poly p2, poly spNoether, const ring r);
/* former nc_spGSpolyRedTail */
void nc_ReduceSpolyTail(poly p1, poly q, poly q2, poly spNoether, const ring r);
/* former nc_spShort */
poly nc_CreateShortSpoly(poly p1, poly p2, const ring r=currRing);

ideal gr_bba (ideal F, ideal Q,kStrategy strat);

/* brackets: */
poly nc_p_Bracket_qq(poly p, poly q);
poly nc_mm_Bracket_nn(poly m1, poly m2);

/* twostd: */
ideal twostd(ideal I);
/* Ann: */
ideal Approx_Step(ideal L);

/* complete reduction routines */

/* void nc_kBucketPolyRed(kBucket_pt b, poly p); */
void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c);
void nc_PolyPolyRed(poly &b, poly p, number *c);

matrix nc_PrintMat(int a, int b, ring r, int metric);

poly p_CopyEmbed(poly p, ring srcRing, int shift, int par_shift);
poly pOppose(ring Rop, poly p);
ideal idOppose(ring Rop, ideal I);

#else
/* dummy definition to make gcc happy */
#define nc_kBucketPolyRed(A,B,C) 0
#define nc_PolyPolyRed(A,B,C) 0

#endif /* HAVE_PLURAL */
#endif

