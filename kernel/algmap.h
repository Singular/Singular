#ifndef ALGMAP_H
#define ALGMAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: algmap.h,v 1.1.1.1 2003-10-06 12:15:50 Singular Exp $ */
/*
* ABSTRACT:
*/
#include "structs.h"

poly maAlgpolyFetch(ring R, poly preimage);
poly maAlgpolyMap(ring R, poly preimage, ideal F, ideal G);

#endif
