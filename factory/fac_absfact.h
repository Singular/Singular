/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfModResultant.h
 *
 * modular resultant algorithm as described by G.E. Collins in "The Calculation
 * of multivariate polynomial resultants"
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
