/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: singext.h,v 1.5 1998-06-08 15:40:54 schmidt Exp $ */

#ifndef INCL_SINGEXT_H
#define INCL_SINGEXT_H

#include <config.h>

#include "cf_gmp.h"

#include "canonicalform.h"

// we need this to copy singulars GF(q) table to ours
extern int nfCharQ;
extern int nfM1;
extern int nfMinPoly[];
extern short nfPlus1Table[];

// used to open the GF(q) tables from Singular
extern FILE * feFopen ( char * path, char * mode, char *where = 0, int useWerror = 0 );

/*BEGINPUBLIC*/

MP_INT gmp_numerator ( const CanonicalForm & f );

MP_INT gmp_denominator ( const CanonicalForm & f );

CanonicalForm make_cf ( const MP_INT & n );

CanonicalForm make_cf ( const MP_INT & n, const MP_INT & d, bool normalize );

/*ENDPUBLIC*/

#endif /* ! INCL_SINGEXT_H */
