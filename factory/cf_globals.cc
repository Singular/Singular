/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// cf_globals.cc - definitions of global variables.
//
// Used by: canonicalform.cc, int_int.cc, ffops.h, imm.h
//
//}}}

#include <config.h>

#include "cf_switches.h"

//{{{ extern const char factoryVersion[];
//{{{ docu
//
// factoryVersion - factory version string.
//
// factoryVersion is initialized from #define FACTORYVERSION
// which is defined in config.h.  It is not used in factory
// itself.
//
//}}}
extern const char factoryVersion[] = "@(#) factoryVersion = " FACTORYVERSION;

//{{{ extern const char factoryConfiguration[];
//{{{ docu
//
// factoryConfiguration - factory configuration.
//
// factoryConfiguration is initialized from #define
// FACTORYCONFIGURATION which is defined in config.h.  It is set
// to the option configure was called with plus the directory it
// was called in.  It is not used in factory itself.
//
//}}}
extern const char factoryConfiguration[] = "@(#) factoryConfiguration = " FACTORYCONFIGURATION;
//}}}

//{{{ CFSwitches cf_glob_switches;
//{{{ docu
//
// cf_glob_switches - factory switches.
//
// This is the only object of type CFSwitches in factory.  It is
// used either directly in the low level algorithms or by the
// functions On(), Off(), isOn() defined in canonicalform.cc.
//
//}}}
CFSwitches cf_glob_switches;
//}}}
