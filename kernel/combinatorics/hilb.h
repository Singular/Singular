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

intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
intvec * hFirstSeries1(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
intvec * hFirstSeries0(ideal S, ideal Q, intvec *wdegree, const ring src, const ring Qt);
poly hFirstSeries0p(ideal A,ideal Q, intvec *wdegree, const ring src, const ring Qt);

intvec * hSecondSeries(intvec *hseries1);

void hLookSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL);
#endif
