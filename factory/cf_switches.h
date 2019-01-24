/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file cf_switches.h
 *
 * header to cf_switches.cc.
 *
**/


#ifndef INCL_CF_SWITCHES_H
#define INCL_CF_SWITCHES_H

// #include "config.h"

/** const int CFSwitchesMax
 *
 * const CFSwitchesMax - maximum number of switches.
 *
**/
const int CFSwitchesMax = 8;

/** class CFSwitches
 *
 * class CFSwitches - manages boolean switches.
 *
 * An object of class `CFSwitches' is simply an array of booleans
 * with some comfortable access methods (`On()', `isOn()', etc.).
 * Each object may contain `CFSwitchesMax' switches.  When a new
 * object of type `CFSwitches' is created, all its switches are
 * turned off.
 *
 * Note: No range checking is done when accessing switches.
 *
 * switches: the switches
 *
**/
class CFSwitches
{
private:
    bool switches [CFSwitchesMax];

public:
    // constructors, destructors
    CFSwitches ();
    ~CFSwitches () {}

    // selectors
    /// switch 's' on
    void On ( int s ) { switches[s] = true; }
    /// switch 's' off
    void Off ( int s ) { switches[s] = false; }
    /// check if 's' is on
    bool isOn ( int s ) const { return switches[s]; }
    /// check if 's' is off
    bool isOff ( int s ) const { return ! switches[s]; }
};
/** CFSwitches cf_glob_switches;
 *
 * cf_glob_switches - factory switches.
 *
 * This is the only object of type CFSwitches in factory.  It is
 * used either directly in the low level algorithms or by the
 * functions On(), Off(), isOn() defined in canonicalform.cc.
 *
**/
// extern CFSwitches& cf_glob_switches;
// CFSwitches& cf_glob_switches = CFSwitches::getInstance();
EXTERN_INST_VAR CFSwitches cf_glob_switches;

#endif /* ! INCL_CF_SWITCHES_H */
