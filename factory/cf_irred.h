/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_irred.h,v 1.2 1997-06-19 12:24:25 schmidt Exp $ */

#ifndef INCL_CF_IRRED_H
#define INCL_CF_IRRED_H

#include <config.h>

#include "canonicalform.h"
#include "cf_random.h"

/*BEGINPUBLIC*/

CanonicalForm find_irreducible ( int deg, CFRandom & gen, const Variable & x );

/*ENDPUBLIC*/

#endif /* ! INCL_CF_IRRED_H */
