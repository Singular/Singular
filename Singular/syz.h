#ifndef SYZ_H
#define SYZ_h
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: syz.h,v 1.2 1997-03-24 14:26:02 Singular Exp $ */
#include "structs.h"

void sySchreyersSyzygiesM(polyset F,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort);

void sySchreyersSyzygiesB(polyset F,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort);

resolvente sySchreyerResolvente(ideal arg, int maxlength, int * length,
   BOOLEAN isMonomial=FALSE);

resolvente syResolvente(ideal arg, int maxlength, int * length,
                        intvec *** weights, BOOLEAN minim);

resolvente syMinRes(ideal arg, int maxlength, int * length, BOOLEAN minim);

void syMinimizeResolvente(resolvente res, int length, int first);

intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights=NULL);

ideal syMinBase(ideal arg);

BOOLEAN syTestOrder(ideal i);

#endif
