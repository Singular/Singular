/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_switches.h,v 1.3 1997-06-19 12:23:53 schmidt Exp $ */

#ifndef INCL_CF_SWITCHES_H
#define INCL_CF_SWITCHES_H

#include <config.h>

#include "cf_defs.h"

class CFSwitches
{
private:
    bool switches [10];
public:
    CFSwitches();
    ~CFSwitches() {}
    void On( int s ) { switches[s] = true; }
    void Off( int s ) { switches[s] = false; }
    bool isOn( int s ) const { return switches[s]; }
    bool isOff( int s ) const { return ! switches[s]; }
};

#endif /* ! INCL_CF_SWITCHES_H */
