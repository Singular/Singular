#ifndef SPARSEMAT_H
#define SPARSEMAT_H
/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  sparsmat.h: operations with sparse matrices
 *          (bareis, ...)
 *
 *******************************************************************/
/* $Id$ */


poly smMult(poly, poly);
void smPolyDiv(poly, poly);
poly smMultDiv(poly, poly, const poly);
void smSpecialPolyDiv(poly, poly);
/* ----------------- macros ------------------ */
/* #define OLD_DIV 1 */

#ifdef OLD_DIV
#define SM_MULT(A,B,C) smMult(A,B)
#define SM_DIV smPolyDiv
#else
#define SM_MULT smMultDiv
#define SM_DIV smSpecialPolyDiv
#endif

poly smCallDet(ideal I);
void smCallBareiss(ideal smat, int x, int y, ideal & M, intvec ** iv);
ideal smCallSolv(ideal I);

ring smRingChange(ring *, long);
void smKillModifiedRing(ring r);
long smExpBound(ideal, int, int, int);
BOOLEAN smCheckDet(ideal, int, BOOLEAN);
#endif
