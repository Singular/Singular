#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.4 2001-02-23 15:41:11 levandov Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#include "mod2.h"
#ifdef HAVE_PLURAL
#include "polys.h" // do we need it really?
#include "numbers.h"
#include "matpol.h"
/* #include "p_Procs.h" */
#include "ideals.h"

#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )
// poly functions defined in p_Procs :
poly nc_pp_Mult_mm(poly p, poly m, const ring r,poly &last);
poly nc_p_Mult_mm(poly p, poly m, const ring r);
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, int &lp, int lq,
			   poly spNoether, const ring r);
// other routines we need in addition :
poly nc_mm_Mult_p(const poly m, poly p, const ring r);
poly nc_mm_Mult_nn (Exponent_t *F, Exponent_t *G, const ring r); 
poly nc_mm_Mult_uu (Exponent_t *F,int jG,int bG, const ring r);
poly nc_uu_Mult_ww (int i, int a, int j, int b, const ring r);

#endif /* HAVE_PLURAL */
#endif

