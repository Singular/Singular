#ifndef ALGMAP_H
#define ALGMAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: algmap.h,v 1.4 1999-11-15 17:19:48 obachman Exp $ */
/*
* ABSTRACT:
*/
#include "structs.h"

poly maAlgpolyFetch(ring R, poly preimage);
poly maAlgpolyMap(ring R, poly preimage, ideal F, ideal G);

#endif
