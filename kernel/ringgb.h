/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.h,v 1.5 2006-06-12 13:04:26 Singular Exp $ */
/*
* ABSTRACT: ringgb interface
*/
#ifndef RINGGB_HEADER
#define RINGGB_HEADER
#include "mod2.h"
#ifdef HAVE_RING2TOM
#include "polys.h"

poly reduce_poly_fct(poly p, ring r);
poly ringRedNF(poly f, ideal G, ring r);
poly ringNF(poly f, ideal G, ring r);
poly plain_spoly(poly f, poly g);
int testGB(ideal I, ideal GI);

#endif
#endif
