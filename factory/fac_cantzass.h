/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_cantzass.h,v 1.2 1997-06-19 12:23:40 schmidt Exp $ */

#ifndef INCL_FAC_CANTZASS_H
#define INCL_FAC_CANTZASS_H

#include <config.h>

#include "variable.h"
#include "canonicalform.h"

CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f, bool issqrfree = false, int numext = 0, const Variable & alpha = Variable(), const Variable & beta = Variable() );

//CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f );

#endif /* ! INCL_FAC_CANTZASS_H */
