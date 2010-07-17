/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id$ */
/*
* ABSTRACT: Declaration of the Groebner walk
*/

#ifndef WALK_H
#define WALK_H

#include <kernel/structs.h>

ideal  MwalkInitialForm(ideal G, intvec* curr_weight);

//compute the next weight vector
intvec* MwalkNextWeight(intvec* curr_weight,intvec* target_weight, ideal G);

int MivSame(intvec* u , intvec* v);
int M3ivSame(intvec* next_weight, intvec* u , intvec* v);



intvec* Mivdp(int nR);
intvec* Mivlp(int nR);

intvec* MivMatrixOrder(intvec* iv);  
intvec* MivMatrixOrderdp(int iv);  
intvec* MPertVectors(ideal G, intvec* ivtarget, int pdeg);  
intvec* MPertVectorslp(ideal G, intvec* ivtarget, int pdeg);  


intvec* MivMatrixOrderlp(int nV);

intvec* Mfpertvector(ideal G, intvec* iv);  
intvec* MivUnit(int nV);

intvec* MivWeightOrderlp(intvec* ivstart);  
intvec* MivWeightOrderdp(intvec* ivstart);  

ideal MidLift(ideal Gomega, ideal M);  
ideal MLiftLmalG(ideal L, ideal G);
ideal MLiftLmalGNew(ideal Gomega, ideal M, ideal G);  
ideal MLiftLmalGMin(ideal L, ideal G);  


intvec* MkInterRedNextWeight(intvec* iva, intvec* ivb, ideal G);
intvec* MPertNextWeight(intvec* iva, ideal G, int deg);
intvec* Mivperttarget(ideal G, int ndeg);


intvec* MSimpleIV(intvec* iv);

/* Okt -- Nov'01 */
// compute a Groebner basis of an ideal G w.r.t. lexicographic order
ideal Mwalk(ideal G, intvec* curr_weight, intvec* target_weight); 

/* the perturbation walk algorithm */
ideal Mpwalk(ideal G,int op,int tp,intvec* curr_weight,intvec* target_weight, int nP); 

/* The fractal walk algorithm */
ideal Mfwalk(ideal G, intvec* ivstart, intvec* ivtarget);


/* Implement Tran's idea */
intvec* TranMPertVectorslp(ideal G);
ideal TranMImprovwalk(ideal Go, intvec* curr_weight,intvec* target_weight, int nP);

/* the first alternative algorithm */
ideal MAltwalk1(ideal G,int op,int tp,intvec* curr_weight,intvec* target_weight); 

/* the second alternative algorithm */
ideal MAltwalk2(ideal G, intvec* curr_weight, intvec* target_weight); 

#endif  //WALK_H
