// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapsing.h,v 1.5 1998-12-16 18:43:36 Singular Exp $
/*
* ABSTRACT: interface between Singular and factory
*/


#ifndef INCL_FACTORYSING_H
#define INCL_FACTORYSING_H

#include "structs.h"
#include "polys.h"
#include "ideals.h"
#include "intvec.h"
#include "matpol.h"
#include "lists.h"

poly singclap_gcd ( poly f, poly g );
alg singclap_alglcm ( alg f, alg g );
void singclap_algdividecontent ( alg f, alg g, alg &ff, alg &gg );

poly singclap_resultant ( poly f, poly g , poly x);

lists singclap_extgcd ( poly f, poly g );

poly singclap_pdivide ( poly f, poly g );

void singclap_divide_content ( poly f );

ideal singclap_factorize ( poly f, intvec ** v , int with_exps);

matrix singclap_irrCharSeries ( ideal I);

BOOLEAN singclap_isSqrFree(poly f);

char* singclap_neworder ( ideal I);

poly singclap_det( const matrix m );
int singclap_det_i( intvec * m );

BOOLEAN jjGCD_P(leftv res, leftv u, leftv v);
BOOLEAN jjFAC_P(leftv res, leftv u);
BOOLEAN jjEXTGCD_P(leftv res, leftv u, leftv v);
BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w);
BOOLEAN jjSQR_FREE_DEC(leftv res, leftv u, leftv dummy);
BOOLEAN jjCHARSERIES(leftv res, leftv u);
#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u);
#endif

#endif /* INCL_FACTORYSING_H */

