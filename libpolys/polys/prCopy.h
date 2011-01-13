#ifndef PRCOPY_H
#define PRCOPY_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
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
poly prMoveR_NoSort(poly &p, ring r, ring dest_r);
poly prMoveR(poly &p, ring r, ring dest_r);
poly prCopyR_NoSort(poly p, ring r, ring dest_r);
poly prCopyR(poly p, ring r, ring dest_r);
poly prShallowCopyR_NoSort(poly p, ring r, ring dest_t);
poly prShallowCopyR(poly p, ring r, ring dest_t);
poly prHeadR(poly p, ring r, ring dest_rg);

ideal idrMoveR_NoSort(ideal &id, ring r, ring dest_r );
ideal idrMoveR(ideal &id, ring r, ring dest_r );

ideal idrCopyR_NoSort(ideal id, ring r, ring dest_r );
ideal idrCopyR(ideal id, ring r, ring dest_r );

ideal idrShallowCopyR_NoSort(ideal id, ring r, ring dest_r );
ideal idrShallowCopyR(ideal id, ring r, ring dest_r );

/// Copy leading terms of id[i] via prHeeadR into dest_r
ideal idrHeadR(ideal id, ring r, ring dest_r );
#endif
