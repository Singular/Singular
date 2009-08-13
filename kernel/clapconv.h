// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapconv.h,v 1.5 2009-08-13 15:17:02 Singular Exp $
/*
* ABSTRACT: convert data between Singular and factory
*/


#ifndef INCL_SINGCONV_H
#define INCL_SINGCONV_H

#include "structs.h"
#include "longalg.h"
#include <factory.h>

poly convFactoryPSingP ( const CanonicalForm & f, const ring r=currRing );
CanonicalForm convSingPFactoryP( poly p, const int off=0, const ring r=currRing );

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a );
poly convFactoryAPSingAP ( const CanonicalForm & f );
poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start );

CanonicalForm convSingGFFactoryGF ( poly p );
poly convFactoryGFSingGF ( const CanonicalForm & f );

CanonicalForm convSingAFactoryA ( napoly p , const Variable & a );
napoly convFactoryASingA ( const CanonicalForm & f );

CanonicalForm convSingTrPFactoryP ( poly p, const ring r=currRing );
poly convFactoryPSingTrP ( const CanonicalForm & f, const ring r=currRing );

CanonicalForm convSingNFactoryN( number n );
number convFactoryNSingN( const CanonicalForm & n);


// CanonicalForm convSingIFactoryI( int i);a <- by constructor of CanonicalForm
int convFactoryISingI( const CanonicalForm & f);
#endif /* INCL_SINGCONV_H */
