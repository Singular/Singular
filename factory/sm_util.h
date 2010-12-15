/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_SM_UTIL_H
#define INCL_SM_UTIL_H

//{{{ docu
//
// sm_util.h - header to sm_util.cc.
//
// Contributed by Marion Bruder <bruder@math.uni-sb.de>.
//
//}}}

#include <config.h>

#include "canonicalform.h"
#include "cf_reval.h"
#include "templates/ftmpl_array.h"

typedef Array<REvaluation> REArray;

int countmonome( const CanonicalForm & f );

CanonicalForm Leitkoeffizient( const CanonicalForm & f );

void ChinesePoly( int arraylength, const CFArray & Polys, const CFArray & primes, CanonicalForm & result );

CanonicalForm dinterpol( int d, const CanonicalForm & gi, const CFArray & zwischen, const REvaluation & alpha, int s, const CFArray & beta, int ni, int CHAR );

CanonicalForm sinterpol( const CanonicalForm & gi, const REArray & xi, CanonicalForm* zwischen, int n );

#endif /* ! INCL_SM_UTIL_H */
