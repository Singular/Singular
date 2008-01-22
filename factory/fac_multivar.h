/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_multivar.h,v 1.3 2008-01-22 09:30:31 Singular Exp $ */

#ifndef INCL_FAC_MULTIVAR_H
#define INCL_FAC_MULTIVAR_H

#include <config.h>

#include "canonicalform.h"

CFFList ZFactorizeMultivariate ( const CanonicalForm & f, bool issqrfree );
CFFList FpFactorizeMultivariate ( const CanonicalForm & f, bool issqrfree );

#endif /* ! INCL_FAC_MULTIVAR_H */
