// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_distrib.h,v 1.2 1997-04-07 16:21:56 schmidt Exp $

#ifndef INCL_FAC_DISTRIB_H
#define INCL_FAC_DISTRIB_H

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1  1996/12/06 14:45:57  schmidt
 * Initial revision
 *
 */

#include <config.h>

#include "canonicalform.h"


bool nonDivisors ( CanonicalForm omega, CanonicalForm delta, const CFArray & F, CFArray & d );

bool checkEvaluation ( const CanonicalForm & U, const CanonicalForm & lcU, const CanonicalForm & omega, const CFFList & F, const Evaluation & A, CanonicalForm & delta );

bool distributeLeadingCoeffs ( CanonicalForm & U, CFArray & G, CFArray & lcG, const CFFList & F, const CFArray & D, CanonicalForm & delta, CanonicalForm & omega, const Evaluation & A, int r );

CFList gcdFreeBasis ( const CFList L );

#endif /* INCL_FAC_DISTRIB_H */
