#ifndef KSTDFAC_H
#define KSTDFAC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstdfac.h,v 1.1.1.1 2003-10-06 12:15:56 Singular Exp $ */
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/
#include "structs.h"

ideal_list kStdfac(ideal F, ideal Q, tHomog h,intvec ** w, ideal D=NULL);
#endif

