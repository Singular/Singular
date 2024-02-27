#ifndef HILB_H
#define HILB_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/

#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "misc/intvec.h"
#include "coeffs/bigintmat.h"

intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
intvec * hFirstSeries1(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
intvec * hFirstSeries0(ideal S, ideal Q, intvec *wdegree, const ring src, const ring Qt);
poly hFirstSeries0p(ideal A,ideal Q, intvec *wdegree, const ring src, const ring Qt);
bigintmat* hPoly2BIV(poly h, const ring Qt, const coeffs biv_cf);
poly hBIV2Poly(bigintmat* b, const ring Qt, const coeffs biv_cf);
bigintmat* hFirstSeries0b(ideal I, ideal Q, intvec *wdegree, intvec *shifts,const ring src, const coeffs biv_cf);
bigintmat* hSecondSeries0b(ideal I, ideal Q, intvec *wdegree, intvec *shifts,const ring src, const coeffs biv_cf);
poly hFirstSeries0m(ideal A,ideal Q, intvec *wdegree, intvec *shifts, const ring src, const ring Qt);
intvec* hFirstSeries0wm(ideal A,ideal Q, intvec *wdegree, intvec *shifts, const ring src, const ring Qt);

intvec * hSecondSeries(intvec *hseries1);

void hLookSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
#endif
