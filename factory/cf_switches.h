// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_switches.h,v 1.0 1996-05-17 10:59:39 stobbe Exp $

#ifndef INCL_CF_SWITCHES_H
#define INCL_CF_SWITCHES_H

/*
$Log: not supported by cvs2svn $
*/

#include "cf_defs.h"

#define RATIONAL 0;

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

#endif
