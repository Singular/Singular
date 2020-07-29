/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_binom.h 12231 2009-11-02 10:12:22Z hannes $ */

#ifndef INCL_CF_BINOM_H
#define INCL_CF_BINOM_H

#include <config.h>

#include "canonicalform.h"

void initPT();

void resetFPT();

/*BEGINPUBLIC*/

CanonicalForm binomialpower ( const Variable&, const CanonicalForm&, int );

/*ENDPUBLIC*/

#endif /* ! INCL_CF_BINOM_H */
