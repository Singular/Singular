// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_switches.cc,v 1.0 1996-05-17 10:59:44 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "cf_defs.h"
#include "cf_switches.h"

CFSwitches::CFSwitches ( )
{
    for ( int i = 0; i < 10; i++ )
	switches[i] = false;
}
