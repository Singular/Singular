// emacs edit mode for this file is -*- C++ -*-
// $Id: singext.h,v 1.2 1997-04-30 12:48:57 schmidt Exp $

#ifndef INCL_SINGEXT_H
#define INCL_SINGEXT_H

/*
$Log: not supported by cvs2svn $
Revision 1.1  1997/04/15 09:38:18  schmidt
#include <config.h> added

Revision 1.0  1996/05/17 10:59:42  stobbe
Initial revision

*/

#include <config.h>

#include "cf_gmp.h"

#include "canonicalform.h"


// we need this to copy singulars GF(q) table to ours
extern int nfCharQ;
extern int nfM1;
extern int nfMinPoly[];
extern short nfPlus1Table[];


/*BEGINPUBLIC*/

MP_INT gmp_numerator ( const CanonicalForm & f );

MP_INT gmp_denominator ( const CanonicalForm & f );

CanonicalForm make_cf ( const MP_INT & n );

CanonicalForm make_cf ( const MP_INT & n, const MP_INT & d, bool normalize );

/*ENDPUBLIC*/

#endif /* INCL_SINGEXT_H */
