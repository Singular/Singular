/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_sqrfree.h,v 1.6 2008-03-17 17:44:04 Singular Exp $ */

#ifndef INCL_FAC_SQRFREE_H
#define INCL_FAC_SQRFREE_H

#include <config.h>

#include "canonicalform.h"

CFFList sortCFFList ( CFFList & F );

CFFList sqrFreeFp ( const CanonicalForm & f );

bool isSqrFreeFp ( const CanonicalForm & f );

CFFList sqrFreeZ ( const CanonicalForm & f );

bool isSqrFreeZ ( const CanonicalForm & f );

#endif /* ! INCL_FAC_SQRFREE_H */
