// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_irred.h,v 1.1 1997-04-07 15:11:28 schmidt Exp $

#ifndef INCL_CF_IRRED_H
#define INCL_CF_IRRED_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:38  stobbe
Initial revision

*/

#include <config.h>

#include "canonicalform.h"
#include "cf_random.h"

/*BEGINPUBLIC*/

CanonicalForm find_irreducible ( int deg, CFRandom & gen, const Variable & x );

/*ENDPUBLIC*/

#endif /* INCL_CF_IRRED_H */
