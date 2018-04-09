#ifndef COHOMO_H
#define COHOMO_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include "kernel/linear_algebra/linearAlgebra.h"

#include "libpolys/misc/intvec.h"


void gradedpiece1(ideal h,poly a,poly b);
void gradedpiece2(ideal h,poly a,poly b);
intvec *gradedpiece1n(ideal h,poly a,poly b);
//void gradedpiece2n(ideal h,poly a,poly b);
void Tlink(ideal h,poly a,poly b,int n);
void T1(ideal h);
void T2(ideal h);
//void Gpt2(ideal h,poly a,poly b);
//intvec *T1mat(int a,int b);
ideal idsrRing(ideal h);
BOOLEAN idsr(leftv res, leftv args);
BOOLEAN gd(leftv res, leftv args);

//void firstorderdef_setup(SModulFunctions* p);
#endif
