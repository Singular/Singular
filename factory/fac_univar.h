// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_univar.h,v 1.1 1997-04-08 10:33:48 schmidt Exp $

#ifndef INCL_FAC_UNIVAR_H
#define INCL_FAC_UNIVAR_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:40  stobbe
Initial revision

*/

#include <config.h>

#include "canonicalform.h"
#include "fac_util.h"

CFFList ZFactorizeUnivariate( const CanonicalForm& ff, bool issqrfree = false );

modpk getZFacModulus();

#endif /* INCL_FAC_UNIVAR_H */
