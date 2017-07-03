#ifndef OPAE_H
#define OPAE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include "misc/auxiliary.h"

#ifdef SINGULAR_4_2
struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

BOOLEAN n_AEInitChar(coeffs , void *);
#endif

#endif
