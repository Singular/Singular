// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapconv.h,v 1.2 2005-06-13 16:23:30 Singular Exp $
/*
* ABSTRACT: convert data between Singular and factory
*/


#ifndef INCL_SINGCONV_H
#define INCL_SINGCONV_H

#include "structs.h"
#include "longalg.h"
#include <factory.h>

napoly convClapPSingTr ( const CanonicalForm & f );
CanonicalForm convSingTrClapP( napoly p );

poly convClapPSingP ( const CanonicalForm & f );
#define  convSingPClapP(p) conv_SingPClapP(p,currRing)

poly conv_ClapPSingP ( const CanonicalForm & f, ring r );
CanonicalForm conv_SingPClapP( poly p, ring r );

CanonicalForm convSingAPClapAP ( poly p , const Variable & a );
poly convClapAPSingAP ( const CanonicalForm & f );
poly convClapAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start );

CanonicalForm convSingGFClapGF ( poly p );
poly convClapGFSingGF ( const CanonicalForm & f );

CanonicalForm convSingAClapA ( napoly p , const Variable & a );
napoly convClapASingA ( const CanonicalForm & f );

CanonicalForm convSingTrPClapP ( poly p );
poly convClapPSingTrP ( const CanonicalForm & f );

CanonicalForm convSingNClapN( number n );
number convClapNSingN( const CanonicalForm & n);


// CanonicalForm convSingIClapI( int i);a <- by constructor of CanonicalForm
int convClapISingI( const CanonicalForm & f);
#endif /* INCL_SINGCONV_H */
