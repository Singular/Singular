/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_univar.h,v 1.2 1997-06-19 12:23:12 schmidt Exp $ */

#ifndef INCL_FAC_UNIVAR_H
#define INCL_FAC_UNIVAR_H

#include <config.h>

#include "canonicalform.h"
#include "fac_util.h"

CFFList ZFactorizeUnivariate( const CanonicalForm& ff, bool issqrfree = false );

modpk getZFacModulus();

#endif /* ! INCL_FAC_UNIVAR_H */
