#ifndef ALGMAP_H
#define ALGMAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: algmap.h,v 1.3 1997-04-09 12:19:37 Singular Exp $ */
/*
* ABSTRACT:
*/
#include "structs.h"

poly maAlgpolyFetch(ring R, poly preimage);
poly maAlgpolyMap(ring R, poly preimage, ideal F, ideal G);

#endif
