#ifndef KSTDFAC_H
#define KSTDFAC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstdfac.h,v 1.5 1999-11-15 17:20:16 obachman Exp $ */
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/
#include "structs.h"

lists kStdfac(ideal F, ideal Q, tHomog h,intvec ** w, ideal D=NULL);
#endif

