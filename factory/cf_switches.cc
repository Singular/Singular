/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_switches.cc,v 1.5 1998-01-16 08:09:21 schmidt Exp $ */

//{{{ docu
//
// cf_switches.cc - definition of class CFSwitches.
//
// Used by: cf_globals.cc, ftest/ftest_util.cc
//
//}}}

#include <config.h>

#include "cf_switches.h"

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
}
//}}}
