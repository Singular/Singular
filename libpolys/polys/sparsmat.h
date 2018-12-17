#ifndef SPARSEMAT_H
#define SPARSEMAT_H
/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  sparsmat.h: operations with sparse matrices
 *          (bareis, ...)
 *
 *******************************************************************/


poly sm_MultDiv(poly, poly, const poly, const ring);
void sm_SpecialPolyDiv(poly, poly, const ring);

/* ----------------- macros ------------------ */
/* #define OLD_DIV 1 */

#ifdef OLD_DIV
void sm_PolyDiv(poly, poly, const ring);
#define SM_MULT(A,B,C, R) pp_Mult_qq(A,B,R)
#define SM_DIV sm_PolyDiv
#else
#define SM_MULT sm_MultDiv
#define SM_DIV sm_SpecialPolyDiv
#endif

poly sm_CallDet(ideal I, const ring);
void sm_CallBareiss(ideal smat, int x, int y, ideal & M, intvec ** iv, const ring);
ideal sm_CallSolv(ideal I, const ring);

ring sm_RingChange(const ring, long);
void sm_KillModifiedRing(ring r);
long sm_ExpBound(ideal, int, int, int, const ring);
#endif
