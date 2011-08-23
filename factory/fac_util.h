/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_FAC_UTIL_H
#define INCL_FAC_UTIL_H

// #include "config.h"

#include "canonicalform.h"
#include "cf_eval.h"

/*BEGINPUBLIC*/

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

CanonicalForm remainder( const CanonicalForm & f, const CanonicalForm & g, const modpk & pk );

void divremainder( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & quot, CanonicalForm & rem, const modpk & pk );

bool Hensel ( const CanonicalForm & U, CFArray & G, const CFArray & lcG, const Evaluation & A, const modpk & bound, const Variable & x );

/* some special array functions */

CanonicalForm sum ( const CFArray & a, int f, int l );

CanonicalForm prod ( const CFArray & a, int f, int l );

CanonicalForm sum ( const CFArray & a );

CanonicalForm prod ( const CFArray & a );

CanonicalForm crossprod ( const CFArray & a, const CFArray & b );

/*ENDPUBLIC*/

bool gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g, bool swap );

CanonicalForm ezgcd ( const CanonicalForm & f, const CanonicalForm & g );

void extgcd ( const CanonicalForm & a, const CanonicalForm & b, CanonicalForm & S, CanonicalForm & T, const modpk & pk );

#endif /* ! INCL_FAC_UTIL_H */
