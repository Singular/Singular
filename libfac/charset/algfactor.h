////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
// $Id$
////////////////////////////////////////////////////////////

#ifndef INCL_ALGFACTOR_H
#define INCL_ALGFACTOR_H

#include <factory.h>
#include <tmpl_inst.h>  // for typedef's

CanonicalForm algcd(const CanonicalForm & f, const CanonicalForm & g, const CFList & as, const Varlist & order);
/*BEGINPUBLIC*/
int hasVar(const CanonicalForm &f, const Variable &v);
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */
