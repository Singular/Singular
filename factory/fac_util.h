/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file fac_util.h
 *
 * operations mod p^k and some other useful functions for factorization
**/

#ifndef INCL_FAC_UTIL_H
#define INCL_FAC_UTIL_H

// #include "config.h"

#include "canonicalform.h"
#include "cf_eval.h"

/*BEGINPUBLIC*/

/**
 * class to do operations mod p^k for int's p and k
**/
class modpk
{
private:
    CanonicalForm pk;
    CanonicalForm pkhalf;
    int p;
    int k;
public:
    modpk();
    modpk( int q, int l );
    modpk( const modpk & m );
    modpk& operator= ( const modpk& m );
    ~modpk() {}
    int getp() const { return p; }
    int getk() const { return k; }
    CanonicalForm inverse( const CanonicalForm & f, bool symmetric = true ) const;
    CanonicalForm getpk() const { return pk; }
    CanonicalForm operator() ( const CanonicalForm & f, bool symmetric = true ) const;
};


CanonicalForm replaceLc( const CanonicalForm & f, const CanonicalForm & c );

/*ENDPUBLIC*/

bool gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g, bool swap, int & d );

void extgcd ( const CanonicalForm & a, const CanonicalForm & b, CanonicalForm & S, CanonicalForm & T, const modpk & pk );

CanonicalForm remainder( const CanonicalForm & f, const CanonicalForm & g, const modpk & pk );

CanonicalForm prod ( const CFArray & a, int f, int l );

CanonicalForm prod ( const CFArray & a );

#endif /* ! INCL_FAC_UTIL_H */
