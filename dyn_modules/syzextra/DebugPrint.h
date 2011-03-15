// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR    
\*****************************************************************************/
/** @file DebugPrint.h
 * 
 * Detailed print for debugging
 *
 * ABSTRACT: dPrint outputs as much info as possible
 *
 * @author Oleksandr Motsak
 *
 * @internal @version \$Id$
 *
 **/
/*****************************************************************************/

#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

// include basic definitions
#include "singularxx_defs.h"
#include <kernel/ring.h>

BEGIN_NAMESPACE_SINGULARXX    BEGIN_NAMESPACE(DEBUG)

/// debug-print at most nTerms (2 by default) terms from poly/vector p,
/// assuming that lt(p) lives in lmRing and tail(p) lives in tailRing.
void dPrint(const poly p, const ring lmRing = currRing, const ring tailRing = currRing, const int nTerms = 2);

/// prints an ideal, optionally with details
void dPrint(const ideal id, const ring lmRing = currRing, const ring tailRing = currRing, const int nTerms = 0);

END_NAMESPACE               END_NAMESPACE_SINGULARXX

#endif 
/* #ifndef DEBUGPRINT_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab

