/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_switches.h,v 1.6 1998-01-16 18:08:44 schmidt Exp $ */

#ifndef INCL_CF_SWITCHES_H
#define INCL_CF_SWITCHES_H

//{{{ docu
//
// cf_switches.h - header to cf_switches.cc.
//
//}}}

#include <config.h>

//{{{ const int CFSwitchesMax
//{{{ docu
//
// const CFSwitchesMax - maximum number of switches.
//
//}}}
const int CFSwitchesMax = 10;
//}}}

//{{{ class CFSwitches
//{{{ docu
//
// class CFSwitches - manages boolean switches.
//
// An object of type CFSwitches is simply an array of booleans
// with some comfortable access methods (On, Off).  Each object
// may contain CFSwitchesMax switches.  When a new object of type
// CFSwitches is created, all its switches are turned off.
//
// Note: No range checking is done when accessing switches.
//
// switches: the switches
//
//}}}
//{{{ inline method docu
//
// void On ( int s )
// void Off ( int s )
// bool isOn ( int s ) const
// bool isOff ( int s ) const
//
// On(), Off() - switch s on or off, resp.
//
// isOn(), isOff() - return true iff s is on or off, resp.
//
//}}}
class CFSwitches
{
private:
    bool switches [CFSwitchesMax];
public:
    CFSwitches ();
    ~CFSwitches () {}

    void On ( int s ) { switches[s] = true; }
    void Off ( int s ) { switches[s] = false; }
    bool isOn ( int s ) const { return switches[s]; }
    bool isOff ( int s ) const { return ! switches[s]; }
};
//}}}

#endif /* ! INCL_CF_SWITCHES_H */
