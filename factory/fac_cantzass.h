/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_FAC_CANTZASS_H
#define INCL_FAC_CANTZASS_H

#include <config.h>

#include "variable.h"
#include "canonicalform.h"

CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f, bool issqrfree, int numext, const Variable alpha, const Variable beta );

//CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f );

#endif /* ! INCL_FAC_CANTZASS_H */
