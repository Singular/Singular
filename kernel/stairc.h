#ifndef STAIRC_H
#define STAIRC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: stairc.h,v 1.1.1.1 2003-10-06 12:16:03 Singular Exp $ */
/*
* ABSTRACT
*/

#include "structs.h"

void scComputeHC(ideal s,ideal Q, int  k,poly &hEdge, ring tailRing = currRing);

intvec * scIndIntvec(ideal S, ideal Q=NULL);
lists scIndIndset(ideal S, BOOLEAN all, ideal Q=NULL);
int scDimInt(ideal  s,ideal Q=NULL);
int scMultInt(ideal  s,ideal Q=NULL);
int scMult0Int(ideal  s,ideal Q=NULL);
void scPrintDegree(int co, int mu);
int scDegree(ideal  s,ideal Q=NULL);

ideal scKBase(int deg, ideal  s, ideal Q=NULL);

intvec * hHstdSeries(ideal S, intvec *modulweight, intvec *wdegree,
                     ideal Q=NULL, ring tailRing = currRing);
intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q=NULL, intvec *wdegree=NULL, ring tailRing = currRing);
intvec * hSecondSeries(intvec *hseries1);

void hLookSeries(ideal S, intvec *modulweight, ideal Q=NULL);

#endif


