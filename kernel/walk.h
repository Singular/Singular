/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: walk.h,v 1.1.1.1 2003-10-06 12:16:04 Singular Exp $ */
/*
* ABSTRACT: Declaration of the Groebner walk
*/

#ifndef WALK_H
#define WALK_H

#include "structs.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// IMPORTANT:
// The following routines assume that pGetOrder(p) yields the scalar
// product of the first row of the order matrix with the exponent
// vector of p
// Remark: This is true for all degree orderings (like dp) and block
// orderings (like a(...),lp) BUT NOT FOR lp!!!
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
//
// walkNextWeight
// Returns : weight vector for next step in Groebner walk, if exists
//           (int) 1, if next weight vector is target_weight
//           (int) 0, if no next weight vectro exist


// assumes that curr_weight is first row of order matrix
//intvec* walkNextWeight(intvec* curr_weight, intvec* target_weight, ideal G);

//intvec* MwalkNextWeight(intvec* curr_weight, intvec* target_weight, ideal G);
// assume curr_weight and target_weight are arrays of length
// currRing->N storing current and target weight

//////////////////////////////////////////////////////////////////////
//
// walkInitials
// assume polys of G are ordererd decreasingly w.r.t. curr_weight
// returns ideal consisting of leading (w.r.t. curr_weight) monomials
//ideal walkInitials(ideal G);

//intvec* walkAddIntVec(intvec* v1, intvec* v2);

//int MwalkWeightDegree(poly p, intvec* weight_vector);

//poly MpolyInitialForm(poly g, intvec* curr_weight);

ideal  MwalkInitialForm(ideal G, intvec* curr_weight);

//poly MOrderedPoly(poly p, intvec* curr_weight);

//compute the next weight vector
intvec* MwalkNextWeight(intvec* curr_weight,intvec* target_weight, ideal G);
//intvec* MwalkNextWeightZ(intvec* curr_weight);
//return lead exponent of the polynomial f
//intvec* MExpPol(poly f);//11.02

//return the product  of two intvecs
//intvec* MivMult(intvec* a, intvec* b); //11.02
intvec* Mivdp(int n);
intvec* Mivlp(int n);
intvec* Mivdp0(int n);
//intvec* MivUnit(int n);
poly MivSame(intvec* u , intvec* v);
poly M3ivSame(intvec* next_weight, intvec* u , intvec* v);
/***********************
 *  create a new ring  *
 ***********************/
//5.12 ring MNextRing(intvec* new_weight_vector);

//ring MNextRing(ring startRing, intvec* new_weight_vector);
char* MNextRingStringC(ring startRing, intvec* new_weight_vector);

// compute an intermediate Groebner basis
//ideal MwalkStep(ideal G,intvec* origin_weight, intvec* curr_weight, intvec* weight_order);

//ideal MwalkStep(ideal G, intvec* curr_weight, ring NRing);

// compute a Groebner basis of an ideal G w.r.t. lexicographic order
//ideal Mwalk(ideal G, intvec* curr_weight, intvec* target_weight);

//compute the division of two monoms
//poly MpDiv(poly a, poly b);

//compute the multiplication of two monoms
//poly MpMult(poly a, poly b);

//compare a intvec to intvec NULL
//int Mivcomp(intvec* op);

//define a monomial which exponent is intvec iv
poly MPolVar(intvec* iv);


//int* MExpSub(int* i1, int*i2);

//int* Mleadexp(poly f);

//compute the multiplikation of two ideals by "elementweise"
ideal MidMultLift(ideal A, ideal B);

//poly maIMap(ring r, poly h);

//compute a Groebner basis of an ideal G
ideal Mstd(ideal G);
ideal Mstdhom(ideal G);
//compute a reduced Groebner basis of a Groebner basis G
ideal MkInterRed(ideal G);
ideal MidMinBase(ideal G);
/********** Perturbation Walk ******************/
/*****************************************************************************
* compute an ordering matrix of the basering ordering.                       *
* if the basering ordering is a block order, then its weight vector must be  *
*    entered as the input this programm, otherwise the input is arbitrary    *
*    integer weight vector which its size is the numbers of variables.       *
******************************************************************************/

intvec* MivMatrixOrder(intvec* iv);
intvec* MivMatrixOrderdp(int iv);
intvec* MPertVectors(ideal G, intvec* ivtarget, int pdeg);
intvec* MPertVectorslp(ideal G, intvec* ivtarget, int pdeg);
//ideal pwalk(ideal G, intvec* delta, intvec* teta, int op_deg, int tp_deg);

/**** Fractal Walk *****/
intvec* MivMatrixOrderlp(int nV);

intvec* Mfpertvector(ideal G, intvec* iv);
intvec* MivUnit(int nV);
/*
//ideal MFractalWalkR(ideal G, int nlev, intvec* sigma, intvec* tau, int step);
ideal MFractalWalkR(ideal G, int nlev, intvec* tau, int step);
ideal MFractalWalk(ideal I, intvec* ivstart);
poly Mpsimple(poly p);
poly Mpofid(ideal H);

intvec* MivMatrixOrderlp(int n);
*/
intvec* MivWeightOrderlp(intvec* ivstart);
intvec* MivWeightOrderdp(intvec* ivstart);
//ideal Mimap(ring oldRing, ideal G);

ideal MidLift(ideal Gomega, ideal M);
ideal MLiftLmalG(ideal L, ideal G);
ideal MLiftLmalGNew(ideal Gomega, ideal M, ideal G);
ideal MLiftLmalGMin(ideal L, ideal G);
//intvec* MwalkNextWeight(intvec* curr_weight,intvec* target_weight, ideal G);
intvec* Mfivpert(ideal G, intvec* target, int p_deg);


//int MpSame(poly a, poly b);
//char* MidString(ideal G);


//intvec* MwalkNextWeightZ(intvec* iv);
//intvec* MNextWeightList(intvec* curr_weight, intvec* target_weight, ideal G);

ideal MNWstdhomRed(ideal G, intvec* iv);
poly MMinPoly(poly p, ideal G);
ideal MMinIdeal(ideal G);
ideal MadeLift4(ideal M, ideal pHGw, ideal Gw, ideal G);
ideal MadeLift(ideal M, ideal Gw, ideal G);
//poly MadepDivId(poly f, ideal Gw, ideal G);
ideal MpHeadIdeal(ideal G);
void* test_w_in_Cone(ideal G, intvec* iv);
void* checkideal(ideal G);
matrix MaMidLift(ideal Gomega, ideal M);

//ideal MNormalForm(poly f, ideal G);
//poly MpolyConversion(poly f, ideal GW, ideal G);
ideal MidealConversion(ideal M, ideal GW, ideal G);
//poly MCheckpRedId(poly f, ideal G);
poly MpReduceId(poly f, ideal G);
//poly MpMinimId(poly f, ideal M);
ideal MidMinimId(ideal M);


#endif  //WALK_H
