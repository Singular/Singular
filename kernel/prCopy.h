/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prCopy.h,v 1.1.1.1 2003-10-06 12:16:02 Singular Exp $ */
/*
* ABSTRACT - declarations of functions for Copy/Move/Delete for Polys
*/


/*************************************************************************
 *
 * MoveR, CopyR, ShallowCopyR: operations to get ideals/polys
 *                             from source_r to dest_r where
 *  - Coeff(source_r) == Coeff(dest_r)
 *  - dest_r->N <= source_r->N
 * Move:        input is destroyed
 * ShallowCopy: monomials are copied, coeffs are set
 * Copy:        monomials and coeffs are copied
 *
 ************************************************************************/
poly prMoveR_NoSort(poly &p, ring r, ring dest_r = currRing);
poly prMoveR(poly &p, ring r, ring dest_r = currRing);
poly prCopyR_NoSort(poly p, ring r, ring dest_r = currRing);
poly prCopyR(poly p, ring r, ring dest_r = currRing);
poly prShallowCopyR_NoSort(poly p, ring r, ring dest_t = currRing);
poly prShallowCopyR(poly p, ring r, ring dest_t = currRing);
poly prHeadR(poly p, ring r, ring dest_r = currRing);

ideal idrMoveR_NoSort(ideal &id, ring r, ring dest_r = currRing);
ideal idrMoveR(ideal &id, ring r, ring dest_r = currRing);
ideal idrCopyR_NoSort(ideal id, ring r, ring dest_r = currRing);
ideal idrCopyR(ideal id, ring r, ring dest_r = currRing);
ideal idrShallowCopyR_NoSort(ideal id, ring r, ring dest_r = currRing);
ideal idrShallowCopyR(ideal id, ring r, ring dest_r = currRing);
ideal idrHeadR(ideal id, ring r, ring dest_r = currRing);




