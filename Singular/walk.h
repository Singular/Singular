/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/* $Id: walk.h,v 1.2 1999-11-15 17:20:56 obachman Exp $ */
/*
* ABSTRACT: Declaration of the Groebner walk
*/

#include "mod2.h"
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
intvec* walkNextWeight(intvec* curr_weight, intvec* target_weight, ideal G);
// assume curr_weight and target_weight are arrays of length
// currRing->N storing current and target weight
int* walkNextWeight(const int* curr_weight, const int* target_weight,
                    const ideal G);

//////////////////////////////////////////////////////////////////////
//
// walkInitials
// assume polys of G are ordererd decreasingly w.r.t. curr_weight
// returns ideal consisting of leading (w.r.t. curr_weight) monomials
ideal walkInitials(ideal G);
