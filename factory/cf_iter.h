/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_CF_ITER_H
#define INCL_CF_ITER_H

#include <config.h>

#include "canonicalform.h"

/*BEGINPUBLIC*/

#undef CF_INLINE
#define CF_INLINE
#undef CF_NO_INLINE
#define CF_NO_INLINE

/*ENDPUBLIC*/

#ifdef CF_USE_INLINE
#undef CF_INLINE
#define CF_INLINE inline
#else
#undef CF_INLINE
#define CF_INLINE
#endif

/*BEGINPUBLIC*/

class term;
typedef term * termList;

class CFIterator {
private:
    CanonicalForm data;
    termList cursor;
    bool ispoly, hasterms;
public:
    CFIterator ();
    CFIterator ( const CFIterator& );
    CFIterator ( const CanonicalForm& );
    CFIterator ( const CanonicalForm&, const Variable& );

    ~CFIterator ();

    CFIterator& operator= ( const CFIterator& );
    CFIterator& operator= ( const CanonicalForm& );

    CF_NO_INLINE CFIterator& operator++ ();
    CF_NO_INLINE CFIterator& operator++ ( int );
    CF_NO_INLINE int hasTerms () const;
    CF_NO_INLINE CanonicalForm coeff () const;
    CF_NO_INLINE int exp () const;
};

/*ENDPUBLIC*/

#ifdef CF_USE_INLINE
#include "cf_iter_inline.cc"
#endif

#endif /* ! INCL_CF_ITER_H */
