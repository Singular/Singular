/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file cf_switches.cc
 *
 * definition of class CFSwitches.
 *
 * Used by: cf_globals.cc
 *
**/


#include "config.h"


#include "cf_defs.h"
#include "cf_switches.h"

/** CFSwitches::CFSwitches ()
 *
 * CFSwitches::CFSwitches() - default constructor.
 *
 * Turns all switches off.
 *
**/
CFSwitches::CFSwitches ()
{
    for ( int i = 0; i < CFSwitchesMax; i++ )
        switches[i] = false;
// and set the default (recommended) On-values:
#ifdef HAVE_NTL
  On(SW_USE_CHINREM_GCD);
  //Off(SW_USE_NTL_SORT);
#endif
  On(SW_USE_EZGCD);
  On(SW_USE_EZGCD_P);
  On(SW_USE_QGCD);
}

INST_VAR CFSwitches cf_glob_switches;

