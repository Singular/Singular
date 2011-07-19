// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id$
/*
* ABSTRACT: interface between Singular and factory
*/


#ifndef INCL_FACTORYSING_H
#define INCL_FACTORYSING_H

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>
#include <polys/simpleideals.h>
#include <misc/intvec.h>
#include <polys/matpol.h>
//#include <polys/clapconv.h>
//#include <kernel/longtrans.h>

/* destroys f and g */
poly singclap_gcd ( poly f, poly g, const ring r );

/*
napoly singclap_alglcm ( napoly f, napoly g );
void singclap_algdividecontent ( napoly f, napoly g, napoly &ff, napoly &gg );
*/

poly singclap_resultant ( poly f, poly g , poly x, const ring r);

BOOLEAN singclap_extgcd ( poly f, poly g, poly &res, poly &pa, poly &pb , const ring r);

poly singclap_pdivide ( poly f, poly g, const ring r );

void singclap_divide_content ( poly f, const ring r);

ideal singclap_factorize ( poly f, intvec ** v , int with_exps, const ring r);

ideal singclap_sqrfree ( poly f, const ring r );

matrix singclap_irrCharSeries ( ideal I, const ring r);

#ifdef HAVE_NTL
matrix singntl_HNF(matrix A, const ring r);
intvec* singntl_HNF(intvec* A, const ring r);
matrix singntl_LLL(matrix A, const ring r);
intvec* singntl_LLL(intvec* A, const ring r);
#endif

BOOLEAN singclap_isSqrFree(poly f, const ring r);

char* singclap_neworder ( ideal I, const ring r);

poly singclap_det( const matrix m, const ring r );
int singclap_det_i( intvec * m, const ring r );
/*
BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w);
BOOLEAN jjCHARSERIES(leftv res, leftv u);
#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u);
#endif
*/

number   nChineseRemainder(number *x, number *q,int rl, const coeffs r);

#endif /* INCL_FACTORYSING_H */

