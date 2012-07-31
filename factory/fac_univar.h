/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_FAC_UNIVAR_H
#define INCL_FAC_UNIVAR_H

// #include "config.h"

#include "canonicalform.h"
#include "fac_util.h"

CFFList ZFactorizeUnivariate( const CanonicalForm& ff, bool issqrfree = false );

modpk getZFacModulus();

#endif /* ! INCL_FAC_UNIVAR_H */
