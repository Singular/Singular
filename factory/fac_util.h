// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_util.h,v 1.1 1996-06-27 11:34:25 stobbe Exp $

#ifndef INCL_FAC_UTIL_H
#define INCL_FAC_UTIL_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:40  stobbe
Initial revision

*/

#include "canonicalform.h"

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

/*ENDPUBLIC*/

#endif /* INCL_FAC_UTIL_H */
