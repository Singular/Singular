/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_binom.h,v 1.3 1997-06-19 12:27:29 schmidt Exp $ */

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
