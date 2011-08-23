/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_FAC_SQRFREE_H
#define INCL_FAC_SQRFREE_H

// #include "config.h"

#include "canonicalform.h"

CFFList sortCFFList ( CFFList & F );

CFFList sqrFreeFp ( const CanonicalForm & f );

bool isSqrFreeFp ( const CanonicalForm & f );

CFFList sqrFreeZ ( const CanonicalForm & f );

bool isSqrFreeZ ( const CanonicalForm & f );

/// squarefree part of a poly
CanonicalForm sqrfPart (const CanonicalForm& F ///<[in] some poly
                       );

#endif /* ! INCL_FAC_SQRFREE_H */
