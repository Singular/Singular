// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_univar.h,v 1.0 1996-05-17 10:59:40 stobbe Exp $

#ifndef INCL_FAC_UNIVAR_H
#define INCL_FAC_UNIVAR_H

/*
$Log: not supported by cvs2svn $
*/

#include "canonicalform.h"
#include "fac_util.h"

CFFList ZFactorizeUnivariate( const CanonicalForm& ff, bool issqrfree = false );

modpk getZFacModulus();

#endif /* INCL_FAC_UNIVAR_H */

