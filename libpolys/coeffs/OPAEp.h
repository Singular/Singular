#ifndef OPAEP_H
#define OPAEP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;

#ifdef SINGULAR_4_2
BOOLEAN n_pAEInitChar(coeffs , void *);
#endif

#endif
