#ifndef SPARSEMAT_H
#define SPARSEMAT_H
/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  sparsmat.h: operations with sparse matrices
 *          (bareis, ...)
 *
 *******************************************************************/
/* $Id: sparsmat.h,v 1.1.1.1 2003-10-06 12:16:03 Singular Exp $ */


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
void smCallNewBareiss(ideal smat, int x, int y, ideal & M, intvec ** iv);
ideal smCallSolv(ideal I);

void smRingChange(ring *, sip_sring &, Exponent_t);
void smRingClean(ring, ip_sring &);
Exponent_t smExpBound(ideal, int, int, int);
BOOLEAN smCheckDet(ideal, int, BOOLEAN);
#endif
