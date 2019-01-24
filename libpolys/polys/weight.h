#ifndef WEIGHT_H
#define WEIGHT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

#include "polys/monomials/ring.h"

EXTERN_VAR short * ecartWeights;
//extern pFDegProc pFDegOld;
//extern pLDegProc pLDegOld;

void kEcartWeights(poly* s, int sl, short *eweight, const ring R);
//BOOLEAN kWeight(leftv res,leftv id);
//BOOLEAN kQHWeight(leftv res,leftv v);
long maxdegreeWecart(poly p,int *l, ring r);
long totaldegreeWecart(poly p, ring r);
long totaldegreeWecart_IV(poly p, ring r, const short *w);

short * iv2array(intvec * iv, const ring R);

// internal:
extern "C" double (*wFunctional)(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr);
extern "C" double wFunctionalBuch(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr);
void wCall(poly* s, int sl, int *x, double wNsqr, const ring R);

#endif

