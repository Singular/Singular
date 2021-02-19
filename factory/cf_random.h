/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_random.h
 *
 * generate random integers, random elements of finite fields
**/

#ifndef INCL_CF_RANDOM_H
#define INCL_CF_RANDOM_H

// #include "config.h"

#include "canonicalform.h"

/*BEGINPUBLIC*/

/**
 * virtual class for random element generation
**/
class CFRandom {
public:
    virtual ~CFRandom() {}
    virtual CanonicalForm generate() const { return 0; }
    virtual CFRandom * clone() const { return new CFRandom(); }
};

/**
 * generate random elements in GF
**/
class GFRandom : public CFRandom
{
public:
    GFRandom() {};
    ~GFRandom() {}
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

/**
 * generate random elements in F_p
**/
class FFRandom : public CFRandom
{
public:
    FFRandom() {}
    ~FFRandom() {}
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

/**
 * generate random integers
**/
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

/**
 * generate random elements in F_p(alpha)
**/
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

/// random integers with abs less than n
int factoryrandom( int n );

/// random seed initializer
void FACTORY_PUBLIC factoryseed( int s );

/*ENDPUBLIC*/

#endif /* ! INCL_CF_RANDOM_H */
