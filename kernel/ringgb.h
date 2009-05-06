/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.h,v 1.8 2009-05-06 12:53:49 Singular Exp $ */
/*
* ABSTRACT: ringgb interface
*/
#ifndef RINGGB_HEADER
#define RINGGB_HEADER
#include "mod2.h"

#ifdef HAVE_RINGS
#include "polys.h"

poly ringNF(poly f, ideal G, ring r);
poly plain_spoly(poly f, poly g);
int testGB(ideal I, ideal GI);

poly reduce_poly_fct(poly p, ring r);
poly ringRedNF(poly f, ideal G, ring r);

#endif
#endif
