// emacs editmode for this file is -*- C++ -*-
// $Id: cf_globals.cc,v 1.5 1997-06-12 12:28:28 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.4  1997/05/02 08:56:11  schmidt
o reference to #define VERSION replaced by FACTORYVERSION

Revision 1.3  1997/04/30 12:20:28  schmidt
o version string includes VERSION from config.h now

Revision 1.2  1997/04/07 15:08:12  schmidt
#include <config.h> added

Revision 1.1  1997/03/26 16:41:24  schmidt
version string added

Revision 1.0  1996/05/17 10:59:43  stobbe
Initial revision

*/

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_switches.h"

// FACTORYVERSION is a macro defined in config.h
extern const char factoryVersion[] = "@(#) factoryVersion = " FACTORYVERSION;
CFSwitches cf_glob_switches;
