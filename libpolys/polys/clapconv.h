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

#ifdef HAVE_FACTORY

#include <polys/monomials/ring.h>
#include <factory/factory.h>


poly convFactoryPSingP ( const CanonicalForm & f, const ring r );
CanonicalForm convSingPFactoryP( poly p, const ring r );
int convFactoryISingI( const CanonicalForm & f);

CanonicalForm convSingAFactoryA( number pp, Variable a, const coeffs cf );
number convFactoryASingA ( const CanonicalForm & f, const coeffs cf );

// HAVE_FACTORY
#endif 

#endif /* INCL_SINGCONV_H */
