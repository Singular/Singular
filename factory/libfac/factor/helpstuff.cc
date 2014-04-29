////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include "tmpl_inst.h"
// some CC's need this:
#include "helpstuff.h"

bool
mydivremt ( const CanonicalForm& f, const CanonicalForm& g, CanonicalForm& a, CanonicalForm& b )
{
  bool retvalue;
  CanonicalForm aa,bb;
  retvalue = divremt(f,g,a,bb);
  aa= f-g*a;
  if ( aa==bb ) { b=bb; }
  else { b=aa; }
  return retvalue;
}

