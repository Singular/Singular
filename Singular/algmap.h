#ifndef ALGMAP_H
#define ALGMAP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
#include "structs.h"

poly maAlgpolyFetch(ring R, poly preimage);
poly maAlgpolyMap(ring R, poly preimage, ideal F, ideal G);

#endif
