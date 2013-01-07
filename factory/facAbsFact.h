/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAbsFact.h
 *
 * bivariate absolute factorization over Q described in "Modular Las Vegas
 * Algorithms for Polynomial Absolute Factorization" by Bertone, Chèze, Galligo
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ABS_FACT_H
#define FAC_ABS_FACT_H

#include "canonicalform.h"

/*BEGINPUBLIC*/

CFList absFactorize (const CanonicalForm& F);

/*ENDPUBLIC*/

#endif
