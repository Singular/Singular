/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.h,v 1.3 2006-06-08 21:56:54 wienand Exp $ */
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
poly plain_spoly(poly f, poly g);
int testGB(ideal GI);

#endif
