// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapsing.h,v 1.2 2008-01-23 15:42:10 Singular Exp $
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

poly singclap_gcd ( poly f, poly g );
napoly singclap_alglcm ( napoly f, napoly g );
void singclap_algdividecontent ( napoly f, napoly g, napoly &ff, napoly &gg );

poly singclap_resultant ( poly f, poly g , poly x);

BOOLEAN singclap_extgcd ( poly f, poly g, poly &res, poly &pa, poly &pb );

poly singclap_pdivide ( poly f, poly g );

void singclap_divide_content ( poly f );

ideal singclap_factorize ( poly f, intvec ** v , int with_exps);

ideal singclap_sqrfree ( poly f );

matrix singclap_irrCharSeries ( ideal I);

BOOLEAN singclap_isSqrFree(poly f);

char* singclap_neworder ( ideal I);

poly singclap_det( const matrix m );
int singclap_det_i( intvec * m );

BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w);
BOOLEAN jjCHARSERIES(leftv res, leftv u);
#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u);
#endif

#endif /* INCL_FACTORYSING_H */

