// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_berlekamp.h,v 1.1 1997-04-07 16:17:52 schmidt Exp $

#ifndef INCL_BERLEKAMP_H
#define INCL_BERLEKAMP_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:39  stobbe
Initial revision

*/

#include <config.h>

#include "canonicalform.h"

CFFList FpFactorizeUnivariateB ( const CanonicalForm & f, bool issqrfree = false );

#endif /* INCL_BERLEKAMP_H */
