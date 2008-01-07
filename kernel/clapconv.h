// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapconv.h,v 1.3 2008-01-07 13:36:16 Singular Exp $
/*
* ABSTRACT: convert data between Singular and factory
*/


#ifndef INCL_SINGCONV_H
#define INCL_SINGCONV_H

#include "structs.h"
#include "longalg.h"
#include <factory.h>

napoly convFactoryPSingTr ( const CanonicalForm & f );
CanonicalForm convSingTrFactoryP( napoly p );

#define  convSingPFactoryP(p) conv_SingPFactoryP(p,currRing)

poly conv_FactoryPSingP ( const CanonicalForm & f, ring r );
CanonicalForm conv_SingPFactoryP( poly p, ring r );

#define  convFactoryPSingP(p) conv_FactoryPSingP(p,currRing)

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a );
poly convFactoryAPSingAP ( const CanonicalForm & f );
poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start );

CanonicalForm convSingGFFactoryGF ( poly p );
poly convFactoryGFSingGF ( const CanonicalForm & f );

CanonicalForm convSingAFactoryA ( napoly p , const Variable & a );
napoly convFactoryASingA ( const CanonicalForm & f );

CanonicalForm convSingTrPFactoryP ( poly p );
poly convFactoryPSingTrP ( const CanonicalForm & f );

CanonicalForm convSingNFactoryN( number n );
number convFactoryNSingN( const CanonicalForm & n);


// CanonicalForm convSingIFactoryI( int i);a <- by constructor of CanonicalForm
int convFactoryISingI( const CanonicalForm & f);
#endif /* INCL_SINGCONV_H */
