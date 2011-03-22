#ifndef WEIGHT_H
#define WEIGHT_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id$ */

#include <kernel/structs.h>
#include <kernel/ring.h>

extern short * ecartWeights;
extern pFDegProc pFDegOld;
extern pLDegProc pLDegOld;

void kEcartWeights(polyset s, int sl, short *eweight);
BOOLEAN kWeight(leftv res,leftv id);
BOOLEAN kQHWeight(leftv res,leftv v);
long maxdegreeWecart(poly p,int *l, ring r = currRing);
long totaldegreeWecart(poly p, ring r = currRing);
long totaldegreeWecart_IV(poly p, ring r, const short *w);

short * iv2array(intvec * iv);

// internal:
extern "C" double (*wFunctional)(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr);
extern "C" double wFunctionalBuch(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr);
void wCall(polyset s, int sl, int *x, double wNsqr);

#endif

