/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prCopy.h,v 1.1 1999-11-15 17:20:42 obachman Exp $ */
/*
* ABSTRACT - declarations of functions for Copy/Move/Delete for Polys
*/


/*************************************************************************
 *
 * MoveR, CopyR, DeleteR, HeadR, SortR
 * Assume: ideal/poly from ring r
 *         Coef(r) == Coef(currRing)
 * Move, Delete: input object is destroyed and set to NULL
 *
 * Sort: revert == TRUE is faster for polys which are almost sorted correctly
 *
 ************************************************************************/
poly prMoveR_NoSort(poly &p, ring r);
poly prMoveR(poly &p, ring r);
poly prCopyR_NoSort(poly p, ring r);
poly prCopyR(poly p, ring r);
poly prHeadR(poly p, ring r);
void prDeleteR(poly &p, ring r);
poly prSortR(poly p, ring r, BOOLEAN revert = TRUE);
poly prMergeR(poly p1, poly p2, ring r);

ideal idrMoveR_NoSort(ideal &id, ring r);
ideal idrMoveR(ideal &id, ring r);
ideal idrCopyR_NoSort(ideal id, ring r);
ideal idrCopyR(ideal id, ring r);
ideal idrHeadR(ideal id, ring r);
void  idrDeleteR(ideal &id, ring r);

/*************************************************************************
 *
 * Copy, Delete, Sort
 *
 *************************************************************************/

inline poly pSort(poly p, BOOLEAN revert = TRUE)
{
  return prSortR(p, currRing, revert);
}

inline poly pMerge(poly p1, poly p2)
{
  return prMergeR(p1, p2, currRing);
}

/*************************************************************************
 *
 * Specialized sort routines
 *
 *************************************************************************/
// TBC: 
// Assume: If considerd only as poly in any component of p 
// (say, monomials of other components of p are set to 0), 
// then p is already sorted correctly 
#define pSortCompCorrect(p) pSort(p, TRUE)


