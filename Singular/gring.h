#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.2 2000-11-20 16:02:03 levandov Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#ifdef HAVE_PLURAL
#include "polys.h" // do we need it really?
#include "p_Procs.h"

#define UPMATELEM(i,j) ( ((pVariables) * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )
// poly functions defined in p_Procs :
poly nc_pp_Mult_mm(poly p, poly m, poly spNoether, const ring r);
poly nc_p_Mult_mm(poly p, poly m, const ring r);
poly nc_p_Minus_mm_Mult_qq(poly p, poly m, poly q,
                           int &shorter, poly spNoether, const ring r);
// other routines we need in addition :
poly nc_m_Mult_pp(poly m, poly p, poly spNoether, const ring r);
poly nc_m_Mult_n(poly m, poly n, const ring r);
poly nc_mm_Mult_uu(poly p, poly m, const ring r);
poly nc_uu_Mult_ww(poly p, poly m, const ring r);

// #define nc_pp_Mult_mm(p, m, spNoether) nc_pp_Mult_mm(p, poly m, poly spNoether, const ring r)

#endif

#endif

