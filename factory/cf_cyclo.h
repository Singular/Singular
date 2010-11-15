#ifndef CF_CYCLO_H
#define CF_CYCLO_H
// -*- c++ -*-
//*****************************************************************************
/** @file cf_cyclo.h
 *
 * @author Martin Lee
 * @date 29.01.2010
 *
 * Compute cyclotomic polynomials and factorize integers by brute force
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @internal
 * @version \$Id$
 *
**/
//*****************************************************************************

#include <config.h>

int* integerFactorizer (const long integer, int& length, bool& fail);
CanonicalForm cyclotomicPoly (int n, bool& fail);
bool isPrimitive (const Variable& alpha, bool& fail);

#endif

