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

#include <kernel/structs.h>
#include <kernel/longalg.h>
#include <kernel/ring.h>
#ifdef HAVE_FACTORY
#  include <factory/factory.h>
#endif /* HAVE_FACTORY */

poly convFactoryPSingP ( const CanonicalForm & f, const ring r=currRing );
CanonicalForm convSingPFactoryP( poly p, const ring r=currRing );

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a, const ring r );
poly convFactoryAPSingAP ( const CanonicalForm & f, const ring r );
poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start );

CanonicalForm convSingGFFactoryGF ( poly p );
poly convFactoryGFSingGF ( const CanonicalForm & f );

CanonicalForm convSingAFactoryA ( napoly p , const Variable & a, const ring r );
napoly convFactoryASingA ( const CanonicalForm & f, const ring r );

CanonicalForm convSingTrPFactoryP ( poly p, const ring r=currRing );
poly convFactoryPSingTrP ( const CanonicalForm & f, const ring r=currRing );

CanonicalForm convSingNFactoryN( number n, const ring r );
number convFactoryNSingN( const CanonicalForm & n);


// CanonicalForm convSingIFactoryI( int i);a <- by constructor of CanonicalForm
int convFactoryISingI( const CanonicalForm & f);
#endif /* INCL_SINGCONV_H */
