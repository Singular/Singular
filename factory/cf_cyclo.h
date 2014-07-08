// -*- c++ -*-
//*****************************************************************************
/** @file cf_cyclo.h
 *
 * Compute cyclotomic polynomials and factorize integers by brute force
 *
 * @par Copyright:
 *   (c) by The SINGULAR Team, see LICENSE file
 *
 * @author Martin Lee
**/
//*****************************************************************************

#ifndef CF_CYCLO_H
#define CF_CYCLO_H

// #include "config.h"

int* integerFactorizer (const long integer, int& length, bool& fail);
CanonicalForm cyclotomicPoly (int n, bool& fail);
bool isPrimitive (const Variable& alpha, bool& fail);

#endif

