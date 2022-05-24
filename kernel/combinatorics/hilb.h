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

intvec * hHstdSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL, ring tailRing = currRing);
intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL, ring tailRing = currRing);

intvec * hSecondSeries(intvec *hseries1);

void hLookSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL, ring tailRing = currRing);

void sortMonoIdeal_pCompare(ideal I);
#endif


