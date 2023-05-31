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

#ifdef HAVE_FLINT
#include <flint/flint.h>
#endif

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
#if defined(HAVE_NTL) || defined(HAVE_FLINT)
  On(SW_USE_CHINREM_GCD);
#endif
  On(SW_USE_NTL_SORT);
#ifdef HAVE_FLINT
  On(SW_USE_FL_GCD_P);
  On(SW_USE_FL_GCD_0);
#if (__FLINT_RELEASE >= 20700)  
  On(SW_USE_FL_FAC_P);
#endif
#endif
  On(SW_USE_EZGCD);
  On(SW_USE_EZGCD_P);
  On(SW_USE_QGCD);
  On(SW_USE_FL_FAC_0);
  //On(SW_USE_FL_FAC_0A);
}

INST_VAR CFSwitches cf_glob_switches;

