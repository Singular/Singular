// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_sqrfree.h,v 1.1 1997-04-08 10:32:27 schmidt Exp $

#ifndef INCL_FAC_SQRFREE_H
#define INCL_FAC_SQRFREE_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:40  stobbe
Initial revision

*/

#include <config.h>

#include "canonicalform.h"

CFFList sqrFreeFp ( const CanonicalForm & f );

bool isSqrFreeFp ( const CanonicalForm & f );

CFFList sqrFreeZ ( const CanonicalForm & f );

bool isSqrFreeZ ( const CanonicalForm & f );

#endif /* INCL_FAC_SQRFREE_H */
