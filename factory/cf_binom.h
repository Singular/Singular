// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_binom.h,v 1.2 1997-04-07 14:56:28 schmidt Exp $

#ifndef INCL_CF_BINOM_H
#define INCL_CF_BINOM_H

/*
$Log: not supported by cvs2svn $
Revision 1.1  1996/07/08 08:13:08  stobbe
"New function resetFPT that resets the pascal triangle for finite
characteristic.
"

Revision 1.0  1996/05/17 10:59:37  stobbe
Initial revision

*/

#include <config.h>

#include "canonicalform.h"

void initPT();

void resetFPT();

/*BEGINPUBLIC*/

CanonicalForm binomialpower ( const Variable&, const CanonicalForm&, int );

/*ENDPUBLIC*/

#endif /* INCL_CF_BINOM_H */
