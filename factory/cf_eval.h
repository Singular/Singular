// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_eval.h,v 1.1 1997-03-26 16:32:24 schmidt Exp $

#ifndef INCL_EVALUATION_H
#define INCL_EVALUATION_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:38  stobbe
Initial revision

*/

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

#include "canonicalform.h"

/*BEGINPUBLIC*/

class Evaluation
{
protected:
    CFArray values;
public:
    Evaluation() : values() {}
    Evaluation( int min, int max ) : values( min, max ) {}
    Evaluation( const Evaluation & e ) : values( e.values ) {}
    virtual ~Evaluation() {}
    Evaluation& operator= ( const Evaluation & e );
    int min() const { return values.min(); }
    int max() const { return values.max(); }
    CanonicalForm operator[] ( int i ) const { return values[i]; }
    CanonicalForm operator[] ( const Variable & v ) const { return operator[](v.level()); }
    CanonicalForm operator() ( const CanonicalForm& f ) const;
    CanonicalForm operator() ( const CanonicalForm & f, int i, int j ) const;
    virtual void nextpoint();
#ifndef NOSTREAMIO
    friend ostream& operator<< ( ostream& s, const Evaluation &e );
#endif /* NOSTREAMIO */
};

/*ENDPUBLIC*/

#endif /* INCL_EVALUATION_H */
