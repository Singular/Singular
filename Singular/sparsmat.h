#ifndef SPARSEMAT_H
#define SPARSEMAT_H
/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  sparsmat.h: operations with sparse matrices
 *          (bareis, ...)
 *
 *******************************************************************/
/* $Id: sparsmat.h,v 1.1 1999-02-10 16:00:06 Singular Exp $ */


poly smMult(poly, poly);
void smPolyDiv(poly, poly);
poly smMultDiv(poly, poly, const poly);
void smSpecialPolyDiv(poly, poly);

lists smCallBareiss(ideal smat, int x, int y);
lists smCallNewBareiss(ideal smat, int x, int y);

#endif
