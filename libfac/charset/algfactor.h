/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
// $Id: algfactor.h,v 1.3 2002-08-19 11:11:30 Singular Exp $
////////////////////////////////////////////////////////////

#ifndef INCL_ALGFACTOR_H
#define INCL_ALGFACTOR_H

#include <factory.h>
#include <tmpl_inst.h>  // for typedef's

CanonicalForm algcd(const CanonicalForm & f, const CanonicalForm & g, const CFList & as, const Varlist & order);
CFFList factoras( const CanonicalForm & f, const CFList & as, int & success );
CFFList cfactor(const CanonicalForm & f, const CFList & as, int success );
/*BEGINPUBLIC*/
int hasVar(const CanonicalForm &f, const Variable &v);
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */
