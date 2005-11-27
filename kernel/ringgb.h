/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.h,v 1.2 2005-11-27 15:28:46 wienand Exp $ */
/*
* ABSTRACT: ringgb interface
*/
#ifndef RINGGB_HEADER
#define RINGGB_HEADER
#include "mod2.h"
#include "polys.h"

poly reduce_poly_fct(poly p, ring r);
poly ringRedNF(poly f, ideal G, ring r);
poly ringNF(poly f, ideal G, ring r);

#endif
