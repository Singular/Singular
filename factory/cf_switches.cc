/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

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
  On(SW_USE_CHINREM_GCD);
  //Off(SW_USE_NTL_GCD_0);
  //Off(SW_USE_NTL_GCD_P);
  //Off(SW_USE_NTL_SORT);
#endif
  On(SW_USE_EZGCD);
  //On(SW_USE_EZGCD_P); // still testing
  On(SW_USE_QGCD);
  //On(SW_USE_fieldGCD);
}
//}}}
