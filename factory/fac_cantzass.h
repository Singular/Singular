// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_cantzass.h,v 1.1 1997-04-07 16:20:46 schmidt Exp $

#ifndef INCL_CANTORZASSENHAUS_H
#define INCL_CANTORZASSENHAUS_H

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:39  stobbe
Initial revision

*/

#include <config.h>

#include "variable.h"
#include "canonicalform.h"

CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f, bool issqrfree = false, int numext = 0, const Variable & alpha = Variable(), const Variable & beta = Variable() );

//CFFList FpFactorizeUnivariateCZ( const CanonicalForm& f );

#endif /* INCL_CANTORZASSENHAUS_H */
