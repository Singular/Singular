// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and factory
*/


#ifndef INCL_SINGCONV_H
#define INCL_SINGCONV_H

#include "polys/monomials/ring.h"
#include "factory/factory.h"


poly convFactoryPSingP ( const CanonicalForm & f, const ring r );
CanonicalForm convSingPFactoryP( poly p, const ring r );
int convFactoryISingI( const CanonicalForm & f);

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a, const ring r );
poly convFactoryAPSingAP ( const CanonicalForm & f, const ring r );
poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start );

//CanonicalForm convSingGFFactoryGF ( poly p, const ring r );
//poly convFactoryGFSingGF ( const CanonicalForm & f, const ring r );

CanonicalForm convSingAFactoryA ( poly p , const Variable & a, const ring r );
poly convFactoryASingA ( const CanonicalForm & f, const ring r );

CanonicalForm convSingTrPFactoryP ( poly p, const ring r );
BOOLEAN convSingTrP(poly p, const ring r );
poly convFactoryPSingTrP ( const CanonicalForm & f, const ring r );

#endif /* INCL_SINGCONV_H */
