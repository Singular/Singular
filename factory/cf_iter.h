// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_iter.h,v 1.2 1997-04-07 16:01:03 schmidt Exp $

#ifndef INCL_CFITERATOR_H
#define INCL_CFITERATOR_H

/*
$Log: not supported by cvs2svn $
Revision 1.1  1997/03/26 16:44:13  schmidt
spurious #include <iostream.h> removed

Revision 1.0  1996/05/17 10:59:38  stobbe
Initial revision

*/

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

#endif /* INCL_CFITERATOR_H */
