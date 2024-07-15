#ifndef SINGULAR_FGLM_H
#define SINGULAR_FGLM_H

#include "kernel/mod2.h"

#include "kernel/ideals.h"
#include "kernel/structs.h"

// fglmproc(...):
// The procedure which has to be called from the interpreter for fglm.
// first is the sourceRing, second is the given ideal in sourceRing.
// Returns the groebnerbasis of the sourceIdeal in the currentRing.
// Checks, if the ideal is really a reduced groebner basis of a
// 0-dimensional Ideal. Returns TRUE if an error occurred.
BOOLEAN fglmProc( leftv result, leftv first, leftv second );

// fglmquotproc(...):
// The procedure which has to be called from the interpreter for fglmquot.
// first is the ideal I, second is the polynomial q. The polynomial must
// be reduced with respect to I.
// Returns the groebnerbasis of I:q in the currentRing.
// Checks, if the ideal is really a reduced groebner basis of a
// 0-dimensional Ideal and if q is really reduced.
//  Returns TRUE if an error occurred.
BOOLEAN fglmQuotProc( leftv result, leftv first, leftv second );
ideal fglmQuot( ideal first, poly second );

// FindUnivariatePolys (test)
BOOLEAN FindUnivariateWrapper( ideal source, ideal & dest );

// wrapper for FindUnivariatePolys (test)
BOOLEAN findUniProc( leftv result, leftv first);
ideal findUni( ideal first );

// homogeneous FGLM
ideal fglmhomProc(leftv first, leftv second);

#endif // #ifndef SINGULAR_FGLM_H
