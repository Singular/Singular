// emacs edit mode for this file is -*- C++ -*-
// $Id: clapconv.h,v 1.1.1.1 1997-03-19 13:18:53 obachman Exp $

#ifndef INCL_SINGCONV_H
#define INCL_SINGCONV_H

#include "mod2.h"
#include "structs.h"
#include "longalg.h"
#include "../factory/singfactory.h"

alg convClapPSingTr ( const CanonicalForm & f );
CanonicalForm convSingTrClapP( alg p );

poly convClapPSingP ( const CanonicalForm & f );
CanonicalForm convSingPClapP( poly p );

CanonicalForm convSingAPClapAP ( poly p , const Variable & a );
poly convClapAPSingAP ( const CanonicalForm & f );

CanonicalForm convSingAClapA ( alg p , const Variable & a );
alg convClapASingA ( const CanonicalForm & f );

CanonicalForm convSingTrPClapP ( poly p );
poly convClapPSingTrP ( const CanonicalForm & f );

#endif /* INCL_SINGCONV_H */
