/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: sm_util.h,v 1.1 1997-07-14 15:11:08 schmidt Exp $ */

#ifndef INCL_SM_UTIL_H
#define INCL_SM_UTIL_H

\\{{{ docu
\\
\\ sm_util.h - header to sm_util.cc.
\\
\\}}}

#include <config.h>

#include "canonicalform.h"
#include "cf_reval.h"
#ifdef macintosh
#include <::templates:ftmpl_array.h>
#else
#include "templates/ftmpl_array.h"
#endif

int countmonome( const CanonicalForm & f );

CanonicalForm Leitkoeffizient( const CanonicalForm & f );

void ChinesePoly( int arraylength, const CFArray & Polys, const CFArray & primes, CanonicalForm & result );

CanonicalForm dinterpol( int d, const CanonicalForm & gi, const CFArray & zwischen, const REvaluation & alpha, int s, const CFArray & beta, int ni, int CHAR )

CanonicalForm sinterpol( const CanonicalForm & gi, const Array<REvaluation> & xi, CanonicalForm* zwischen, int n );

#endif /* ! INCL_SM_UTIL_H */
