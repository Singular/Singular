#ifndef JULIACF_H
#define JULIACF_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include "misc/auxiliary.h"
#include "omalloc/omalloc.h" /* for SIZEOF_DOUBLE */

struct n_Procs_s; typedef struct  n_Procs_s  *coeffs;
struct snumber; typedef struct snumber *   number;

/// Initialize r
BOOLEAN jcfInitChar(coeffs r, void*);

#endif

