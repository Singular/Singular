#ifndef OPAEP_H
#define OPAEP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

#ifdef SINGULAR_4_1
BOOLEAN n_pAEInitChar(coeffs , void *);
#endif

#endif
