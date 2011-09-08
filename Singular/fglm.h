#ifndef SINGULAR_FGLM_H
#define SINGULAR_FGLM_H

#ifdef HAVE_FACTORY

#include <misc/auxiliary.h>

#include <kernel/ideals.h>
#include <kernel/structs.h>

// fglmproc(...):
// The procedure which has to be called from the interpreter for fglm.
// first is the sourceRing, second is the given ideal in sourceRing.
// Returns the groebnerbasis of the sourceIdeal in the currentRing.
// Checks, if the ideal is really a reduced groebner basis of a
// 0-dimensional Ideal. Returns TRUE if an error occoured.
BOOLEAN fglmProc( leftv result, leftv first, leftv second );

// fglmquotproc(...):
// The procedure which has to be called from the interpreter for fglmquot.
// first is the ideal I, second is the polynomial q. The polynomial must
// be reduced with respect to I.
// Returns the groebnerbasis of I:q in the currentRing.
// Checks, if the ideal is really a reduced groebner basis of a
// 0-dimensional Ideal and if q is really reduced.
//  Returns TRUE if an error occoured.
BOOLEAN fglmQuotProc( leftv result, leftv first, leftv second );

// FindUnivariatePolys (test)
BOOLEAN FindUnivariateWrapper( ideal source, ideal & dest );

// wrapper for FindUnivariatePolys (test)
BOOLEAN findUniProc( leftv result, leftv first);

// homogeneous FGLM
ideal fglmhomProc(leftv first, leftv second);


#endif // #ifdef HAVE_FACTORY

#endif // #ifndef SINGULAR_FGLM_H
