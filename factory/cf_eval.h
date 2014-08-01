/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_eval.h
 *
 * evaluate polynomials at points
**/

#ifndef INCL_CF_EVAL_H
#define INCL_CF_EVAL_H

// #include "config.h"

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "canonicalform.h"

/*BEGINPUBLIC*/

/**
 * class to evaluate a polynomial at points
**/
class Evaluation
{
protected:
    CFArray values;
public:
    Evaluation() : values() {}
    Evaluation( int min0, int max0 ) : values( min0, max0 ) {}
    Evaluation( const Evaluation & e ) : values( e.values ) {}
    virtual ~Evaluation() {}
    Evaluation& operator= ( const Evaluation & e );
    int min() const { return values.min(); }
    int max() const { return values.max(); }
    CanonicalForm operator[] ( int i ) const { return values[i]; }
    CanonicalForm operator[] ( const Variable & v ) const { return operator[](v.level()); }
    CanonicalForm operator() ( const CanonicalForm& f ) const;
    CanonicalForm operator() ( const CanonicalForm & f, int i, int j ) const;
    void setValue (int i, const CanonicalForm& f);
    virtual void nextpoint();
#ifndef NOSTREAMIO
    friend OSTREAM& operator<< ( OSTREAM& s, const Evaluation &e );
#endif /* NOSTREAMIO */
};

/*ENDPUBLIC*/

#endif /* ! INCL_CF_EVAL_H */
