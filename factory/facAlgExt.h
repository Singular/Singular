// -*- c++ -*-
//*****************************************************************************
/** @file facAlgExt.h
 *
 * @author Martin Lee
 * @date 
 *
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal 
 * @version \$Id$
 *
**/
//*****************************************************************************

#include "assert.h"

#include "canonicalform.h"

CFList AlgExtSqrfFactorize (const CanonicalForm& F, const Variable& alpha);
CFFList AlgExtFactorize (const CanonicalForm& F, const Variable& alpha);
