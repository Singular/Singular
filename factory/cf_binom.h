// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_binom.h,v 1.1 1996-07-08 08:13:08 stobbe Exp $

#ifndef INCL_CF_BINOM_H
#define INCL_CF_BINOM_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:37  stobbe
Initial revision

*/

#include "canonicalform.h"

void initPT();

void resetFPT();

/*BEGINPUBLIC*/

CanonicalForm binomialpower ( const Variable&, const CanonicalForm&, int );

/*ENDPUBLIC*/

#endif /* INCL_CF_BINOM_H */
