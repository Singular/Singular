// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_sqrfree.h,v 1.0 1996-05-17 10:59:40 stobbe Exp $

#ifndef INCL_FAC_SQRFREE_H
#define INCL_FAC_SQRFREE_H

/*
$Log: not supported by cvs2svn $
*/

#include "canonicalform.h"

CFFList sqrFreeFp ( const CanonicalForm & f );

bool isSqrFreeFp ( const CanonicalForm & f );

CFFList sqrFreeZ ( const CanonicalForm & f );

bool isSqrFreeZ ( const CanonicalForm & f );

#endif /* INCL_FAC_SQRFREE_H */
