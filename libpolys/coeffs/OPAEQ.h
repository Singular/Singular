#ifndef OPAEQ_H
#define OPAEQ_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include <misc/auxiliary.h>

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

#ifdef SINGULAR_4_1
BOOLEAN n_QAEInitChar(coeffs , void *);
#endif

#endif
