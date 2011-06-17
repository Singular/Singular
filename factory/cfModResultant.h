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
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef CF_MOD_RESULTANT_H
#define CF_MOD_RESULTANT_H

#include "canonicalform.h"

/// modular resultant algorihtm over Fp
///
/// @return @a resultantFp returns the resultant of A and B wrt. x
CanonicalForm
resultantFp (const CanonicalForm& A,///<[in] some poly
             const CanonicalForm& B,///<[in] some poly
             const Variable& x,     ///<[in] some polynomial variable
             bool prob= true        ///<[in] if true use probabilistic algorithm
            );

/// modular resultant algorihtm over Z
///
/// @return @a resultantZ returns the resultant of A and B wrt. x
CanonicalForm
resultantZ (const CanonicalForm& A, ///<[in] some poly
            const CanonicalForm& B, ///<[in] some poly
            const Variable& x,      ///<[in] some polynomial variable
            bool prob= true         ///<[in] if true use probabilistic algorithm
           );

#endif

