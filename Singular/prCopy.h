/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prCopy.h,v 1.2 2000-12-31 15:14:43 obachman Exp $ */
/*
* ABSTRACT - declarations of functions for Copy/Move/Delete for Polys
*/


/*************************************************************************
 *
 * MoveR, CopyR, DeleteR, HeadR, SortR
 * Assume: ideal/poly from ring r
 *         Coeff(r) == Coeff(currRing)
 * Move, Delete: input object is destroyed and set to NULL
 *
 *
 ************************************************************************/
poly prMoveR_NoSort(poly &p, ring r);
poly prMoveR(poly &p, ring r);
poly prCopyR_NoSort(poly p, ring r);
poly prCopyR(poly p, ring r);
poly prHeadR(poly p, ring r);

ideal idrMoveR_NoSort(ideal &id, ring r);
ideal idrMoveR(ideal &id, ring r);
ideal idrCopyR_NoSort(ideal id, ring r);
ideal idrCopyR(ideal id, ring r);
ideal idrHeadR(ideal id, ring r);




