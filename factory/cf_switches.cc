/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_switches.cc,v 1.3 1997-07-22 13:57:49 schmidt Exp $ */

//{{{ docu
//
// cf_switches.cc - definition of class CFSwitches.
//
// Used by: cf_globals.cc
//
//}}}

#include <config.h>

#include "cf_switches.h"

//{{{ CFSwitches::CFSwitches()
//{{{ docu
//
// CFSwitches::CFSwitches() - default constructor.
//
// Turns all switches off.
//
//}}}
CFSwitches::CFSwitches()
{
    for ( int i = 0; i < CFSwitchesMax; i++ )
	switches[i] = false;
}
//}}}
