// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id$
/*
* ABSTRACT: convert data between Singular and factory
*/


#ifndef INCL_SINGCONV_H
#define INCL_SINGCONV_H

#include <polys/monomials/ring.h>
#include <polys/ext_fields/longtrans.h>
#ifdef HAVE_FACTORY
#  include <factory/factory.h>
#endif /* HAVE_FACTORY */

poly convFactoryPSingP ( const CanonicalForm & f, const ring r );
CanonicalForm convSingPFactoryP( poly p, const ring r );
//CanonicalForm convSingIFactoryI( int i);// by constructor of CanonicalForm
int convFactoryISingI( const CanonicalForm & f);

//CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a, const ring r );
//poly convFactoryAPSingAP ( const CanonicalForm & f, const ring r );
//poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start );

//CanonicalForm convSingGFFactoryGF ( poly p, const ring r );
//poly convFactoryGFSingGF ( const CanonicalForm & f, const ring r );

//CanonicalForm convSingAFactoryA ( number p , const Variable & a, const ring r );
//number convFactoryASingA ( const CanonicalForm & f, const ring r );

//CanonicalForm convSingTrPFactoryP ( poly p, const ring r );
//poly convFactoryPSingTrP ( const CanonicalForm & f, const ring r );

#endif /* INCL_SINGCONV_H */
