/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// cf_iter_inline.cc - definition of configurable inline
//   `CFIterator' methods.
//
// Hierarchy: canonicalform, utility class
//
// Header file: cf_iter.h
//
// See `cf_inline.cc' for a description of "configurable inline
// methods".
//
//}}}

// check whether we are included or translated and
// define `INCL_CF_ITER_INLINE_CC' if we are included
#ifdef INCL_CF_ITER_H
#define INCL_CF_ITER_INLINE_CC
#endif

#include <config.h>

#include "assert.h"

// regular include file
#include "canonicalform.h"

// temporarily switch off `CF_USE_INLINE' and include
// `cf_iter.h' if we are being translated.
// `CF_USE_INLINE_SAVE' is used to save the state of
// `CF_USE_INLINE'.  It is unset after use.
#ifndef INCL_CF_ITER_INLINE_CC
#ifdef CF_USE_INLINE
#define CF_USE_INLINE_SAVE
#undef CF_USE_INLINE
#endif
#include "cf_iter.h"
#ifdef CF_USE_INLINE_SAVE
#define CF_USE_INLINE
#undef CF_USE_INLINE_SAVE
#endif
#endif /* ! INCL_CF_ITER_INLINE_CC */

// more regular include files
#include "int_cf.h"
#include "int_poly.h"

// set the value of `CF_INLINE' for the following methods and
// functions
#if defined( CF_USE_INLINE ) && defined( INCL_CF_ITER_INLINE_CC )
#undef CF_INLINE
#define CF_INLINE inline
#else
#undef CF_INLINE
#define CF_INLINE
#endif /* ! defined( CF_USE_INLINE ) && defined( INCL_CF_ITER_INLINE_CC ) */

#ifndef INCL_CF_ITER_INLINE_CC
// selectors
//{{{ CF_INLINE int CFIterator::hasTerms () const
//{{{ docu
//
// hasTerm() - check whether CO points to a valid term.
//
// Return true if CO points to a valid term, false if CO points
// to the end of the sequence of terms.
//
//}}}
CF_INLINE int
CFIterator::hasTerms () const
{
    return hasterms;
}
//}}}

//{{{ CF_INLINE CanonicalForm CFIterator::coeff () const
//{{{ docu
//
// coeff() - return coefficient of current term of CO.
//
// CO has to point to a valid term.
//
//}}}
CF_INLINE CanonicalForm
CFIterator::coeff () const
{
    ASSERT( hasterms, "lib error: iterator out of terms" );
    if ( ispoly )
        return cursor->coeff;
    else
        return data;
}
//}}}

//{{{ CF_INLINE int CFIterator::exp () const
//{{{ docu
//
// exp() - return exponent of current term of CO.
//
// CO has to point to a valid term.
//
//}}}
CF_INLINE int
CFIterator::exp () const
{
    ASSERT( hasterms, "lib error: iterator out of terms" );
    if ( ispoly )
        return cursor->exp;
    else
        return 0;
}
//}}}

// implementor methods
//{{{ CFIterator::operator ++ (), operator ++ ( int )
//{{{ docu
//
// operator ++() - advance CO to next term.
//
// Advance current term to next term in the sequence of terms or
// to end of sequence.  CO has to point to a valid term.
//
// The postfix and prefix operator are identical.
//
//}}}
CF_INLINE CFIterator &
CFIterator::operator ++ ()
{
    ASSERT( hasterms, "lib error: iterator out of terms" );
    if ( ispoly ) {
        cursor = cursor->next;
        hasterms = cursor != 0;
    } else
        hasterms = false;

    return *this;
}

CF_INLINE CFIterator &
CFIterator::operator ++ ( int )
{
    ASSERT( hasterms, "lib error: iterator out of terms" );
    if ( ispoly ) {
        cursor = cursor->next;
        hasterms = cursor != 0;
    } else
        hasterms = false;

    return *this;
}
//}}}
#endif
