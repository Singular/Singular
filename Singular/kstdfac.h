#ifndef KSTDFAC_H
#define KSTDFAC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstdfac.h,v 1.3 1997-04-09 12:19:53 Singular Exp $ */
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/
#include "structs.h"

lists stdfac(ideal F, ideal Q, tHomog h,intvec ** w, ideal D=NULL);
#endif

