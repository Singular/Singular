#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.1 2000-11-13 14:50:23 levandov Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#ifdef HAVE_PLURAL
#define UPMATELEM(i,j) ( ((pVariables) * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )

poly nc_pp_Mult_mm(poly p, poly m, poly spNoether, const ring r);
poly nc_p_Mult_mm(poly p, poly m, const ring r);
poly nc_p_Minus_mm_Mult_qq(poly p, poly m, poly q,
                           int &shorter, poly spNoether, const ring r);

#endif

#endif

