/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_switches.cc,v 1.2 1997-06-19 12:23:55 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_switches.h"

CFSwitches::CFSwitches ( )
{
    for ( int i = 0; i < 10; i++ )
	switches[i] = false;
}
