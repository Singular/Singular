/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_iter.h,v 1.3 1997-06-19 12:24:22 schmidt Exp $ */

#ifndef INCL_CF_ITER_H
#define INCL_CF_ITER_H

#include <config.h>

#include "canonicalform.h"

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

    CFIterator& operator++ ();
    CFIterator& operator++ ( int );
    int hasTerms () const;
    CanonicalForm coeff () const;
    int exp () const;
};

/*ENDPUBLIC*/

#endif /* ! INCL_CF_ITER_H */
