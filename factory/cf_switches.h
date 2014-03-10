/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_CF_SWITCHES_H
#define INCL_CF_SWITCHES_H

//{{{ docu
//
// cf_switches.h - header to cf_switches.cc.
//
//}}}

// #include "config.h"

//{{{ const int CFSwitchesMax
//{{{ docu
//
// const CFSwitchesMax - maximum number of switches.
//
//}}}
const int CFSwitchesMax = 8;
//}}}

//{{{ class CFSwitches
//{{{ docu
//
// class CFSwitches - manages boolean switches.
//
// An object of class `CFSwitches' is simply an array of booleans
// with some comfortable access methods (`On()', `isOn()', etc.).
// Each object may contain `CFSwitchesMax' switches.  When a new
// object of type `CFSwitches' is created, all its switches are
// turned off.
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
// On(), Off() - switch `s' on or off, resp.
//
// isOn(), isOff() - return true iff `s' is on or off, resp.
//
//}}}
class CFSwitches
{
private:
    bool switches [CFSwitchesMax];

    CFSwitches ();
public:
    // constructors, destructors
    ~CFSwitches () {}

    static inline CFSwitches& getInstance() 
    {
       static CFSwitches singleton;
       return singleton;
    }
    // selectors
    void On ( int s ) { switches[s] = true; }
    void Off ( int s ) { switches[s] = false; }
    bool isOn ( int s ) const { return switches[s]; }
    bool isOff ( int s ) const { return ! switches[s]; }
};
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
// extern CFSwitches& cf_glob_switches;
// CFSwitches& cf_glob_switches = CFSwitches::getInstance();
#define cf_glob_switches (CFSwitches::getInstance())
//}}}

#endif /* ! INCL_CF_SWITCHES_H */
