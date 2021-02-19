/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facIrredTest.h
 *
 * This file provides a probabilistic irreducibility test for polynomials over
 * Z/p.
 *
 * ABSTRACT: irreducibility test based on "Quick and Dirty Irreducibility Test"
 * by v. Bothmer and Schreyer
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_IRRED_TEST_H
#define FAC_IRRED_TEST_H

// #include "config.h"

#include "canonicalform.h"

/// evaluate F at k random points in Z/p^n and count the number of zeros that
/// occur
///
/// @return numZeros returns #zeros/trials
double numZeros (const CanonicalForm& F, ///< [in] some poly over Z/p with n
                                         ///< variables
                 int k                   ///< [in] number of trials
                );

/*BEGINPUBLIC*/

/// given some error probIrredTest detects irreducibility or reducibility of F
/// with confidence level 1-error
///
/// @return probIrredTest returns 1 for irreducibility, -1 for reducibility
///         or 0 if the test is not applicable
int FACTORY_PUBLIC probIrredTest (const CanonicalForm& F, ///< [in] some poly over Z/p
                   double error            ///< [in] 0 < error < 1
                  );

/*ENDPUBLIC*/

#endif

