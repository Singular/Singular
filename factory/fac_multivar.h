// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_multivar.h,v 1.1 1997-04-08 10:31:31 schmidt Exp $

#ifndef INCL_FAC_MULTIVAR_H
#define INCL_FAC_MULTIVAR_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:40  stobbe
Initial revision

*/

#include <config.h>

#include "canonicalform.h"

CFFList ZFactorizeMultivariate ( const CanonicalForm & f, bool issqrfree );

#endif /* INCL_FAC_MULTIVAR_H */
