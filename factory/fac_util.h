// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_util.h,v 1.4 1997-04-08 10:38:05 schmidt Exp $

#ifndef INCL_FAC_UTIL_H
#define INCL_FAC_UTIL_H

/*
$Log: not supported by cvs2svn $
Revision 1.3  1996/07/16 12:26:05  stobbe
"New functions prod, crossprod ans sum for CFArrays, since these functions
are no longer contained in the template specification of Array.
"

Revision 1.2  1996/07/08 08:22:02  stobbe
"New organization of the factorization stuff. Some functions moved from
fac_diophand.cc which no longer exists.
"

Revision 1.1  1996/06/27 11:34:25  stobbe
"New function dviremainder.
"

Revision 1.0  1996/05/17 10:59:40  stobbe
Initial revision

*/

#include <config.h>

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

CanonicalForm maxCoeff( const CanonicalForm & f );

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

#endif /* INCL_FAC_UTIL_H */
