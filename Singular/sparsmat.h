#ifndef SPARSEMAT_H
#define SPARSEMAT_H
/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  sparsmat.h: operations with sparse matrices
 *          (bareis, ...)
 *
 *******************************************************************/
/* $Id: sparsmat.h,v 1.3 1999-11-15 17:20:50 obachman Exp $ */


poly smMult(poly, poly);
void smPolyDiv(poly, poly);
poly smMultDiv(poly, poly, const poly);
void smSpecialPolyDiv(poly, poly);

poly smCallDet(ideal I);
lists smCallBareiss(ideal smat, int x, int y);
lists smCallNewBareiss(ideal smat, int x, int y);

#endif
