/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_berlekamp.h,v 1.2 1997-06-19 12:23:45 schmidt Exp $ */

#ifndef INCL_FAC_BERLEKAMP_H
#define INCL_FAC_BERLEKAMP_H

#include <config.h>

#include "canonicalform.h"

CFFList FpFactorizeUnivariateB ( const CanonicalForm & f, bool issqrfree = false );

#endif /* ! INCL_FAC_BERLEKAMP_H */
