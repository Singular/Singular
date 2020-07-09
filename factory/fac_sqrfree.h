/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file fac_sqrfree.h
 *
 * squarefree part and factorization over Q, Q(a)
**/

#ifndef INCL_FAC_SQRFREE_H
#define INCL_FAC_SQRFREE_H

// #include "config.h"

#include "canonicalform.h"

CFFList sortCFFList ( CFFList & F );

CFFList sqrFreeZ ( const CanonicalForm & f );

/// squarefree part of a poly
CanonicalForm sqrfPart (const CanonicalForm& F ///<[in] some poly
                       );

CFFList sqrFreeFp ( const CanonicalForm & f );
#endif /* ! INCL_FAC_SQRFREE_H */
