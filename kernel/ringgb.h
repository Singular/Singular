/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.h,v 1.7 2007-05-11 10:56:03 wienand Exp $ */
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

#ifdef HAVE_RING2TOM
poly reduce_poly_fct(poly p, ring r);
poly ringRedNF(poly f, ideal G, ring r);
#endif

#endif
#endif
