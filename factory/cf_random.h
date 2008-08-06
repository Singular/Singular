/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_random.h,v 1.4 2008-08-06 14:16:57 Singular Exp $ */

#ifndef INCL_CF_RANDOM_H
#define INCL_CF_RANDOM_H

#include <config.h>

#include "canonicalform.h"

/*BEGINPUBLIC*/

class CFRandom {
public:
    virtual ~CFRandom() {}
    virtual CanonicalForm generate() const { return 0; }
    virtual CFRandom * clone() const { return new CFRandom(); }
};

class GFRandom : public CFRandom
{
public:
    GFRandom() {};
    ~GFRandom() {}
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class FFRandom : public CFRandom
{
public:
    FFRandom() {}
    ~FFRandom() {}
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class IntRandom : public CFRandom
{
private:
    int max;
public:
    IntRandom();
    IntRandom( int m );
    ~IntRandom();
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class AlgExtRandomF : public CFRandom {
private:
    Variable algext;
    CFRandom * gen;
    int n;
    AlgExtRandomF();
    AlgExtRandomF( const Variable & v, CFRandom * g, int nn );
    AlgExtRandomF& operator= ( const AlgExtRandomF & );
public:
    AlgExtRandomF( const AlgExtRandomF & );
    AlgExtRandomF( const Variable & v );
    AlgExtRandomF( const Variable & v1, const Variable & v2 );
    ~AlgExtRandomF();
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class CFRandomFactory {
public:
    static CFRandom * generate();
};

int factoryrandom( int n );

void factoryseed( int s );

/*ENDPUBLIC*/

#endif /* ! INCL_CF_RANDOM_H */
