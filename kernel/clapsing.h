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

#include <kernel/structs.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/intvec.h>
#include <kernel/matpol.h>
#include <kernel/longalg.h>

poly singclap_gcd ( poly f, poly g );
poly singclap_gcd_r ( poly f, poly g, const ring r );
napoly singclap_alglcm ( napoly f, napoly g );
void singclap_algdividecontent ( napoly f, napoly g, napoly &ff, napoly &gg );

poly singclap_resultant ( poly f, poly g , poly x);

BOOLEAN singclap_extgcd ( poly f, poly g, poly &res, poly &pa, poly &pb );
BOOLEAN singclap_extgcd_r ( poly f, poly g, poly &res, poly &pa, poly &pb, const ring r );

poly singclap_pdivide ( poly f, poly g );
poly singclap_pdivide_r ( poly f, poly g, const ring r );

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

