#ifndef KSTDFAC_H
#define KSTDFAC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstdfac.h,v 1.4 1998-05-14 13:04:18 Singular Exp $ */
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/
#include "structs.h"

lists kStdfac(ideal F, ideal Q, tHomog h,intvec ** w, ideal D=NULL);
#endif

