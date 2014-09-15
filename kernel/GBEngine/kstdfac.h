#ifndef KSTDFAC_H
#define KSTDFAC_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/
#include <kernel/structs.h>
#include <polys/simpleideals.h>

ideal_list kStdfac(ideal F, ideal Q, tHomog h,intvec ** w, ideal D=NULL);
#endif

