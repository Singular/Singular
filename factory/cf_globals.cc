/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_globals.cc,v 1.6 1997-06-19 12:27:00 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_switches.h"

// FACTORYVERSION is a macro defined in config.h
extern const char factoryVersion[] = "@(#) factoryVersion = " FACTORYVERSION;
CFSwitches cf_glob_switches;
