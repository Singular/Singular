// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_switches.cc,v 1.1 1997-04-07 16:12:38 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:44  stobbe
Initial revision

*/

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_switches.h"

CFSwitches::CFSwitches ( )
{
    for ( int i = 0; i < 10; i++ )
	switches[i] = false;
}
