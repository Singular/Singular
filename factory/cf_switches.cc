/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_switches.cc,v 1.6 2005-02-17 13:59:27 Singular Exp $ */

//{{{ docu
//
// cf_switches.cc - definition of class CFSwitches.
//
// Used by: cf_globals.cc, ftest/ftest_util.cc
//
//}}}

#include <config.h>

#include "cf_switches.h"
#include "cf_defs.h"

//{{{ CFSwitches::CFSwitches ()
//{{{ docu
//
// CFSwitches::CFSwitches() - default constructor.
//
// Turns all switches off.
//
//}}}
CFSwitches::CFSwitches ()
{
    for ( int i = 0; i < CFSwitchesMax; i++ )
	switches[i] = false;
// and set the default (recommended) On-values:
#ifdef HAVE_NTL
  On(SW_USE_NTL);
  //Off(SW_USE_NTL_GCD);
  //Off(SW_USE_NTL_SORT);
#endif
  On(SW_USE_EZGCD);
}
//}}}
