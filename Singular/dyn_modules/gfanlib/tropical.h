#ifndef TROPICAL_H
#define TROPICAL_H

#include "Singular/ipid.h"


extern int tropicalVerboseLevel;


gfan::ZCone homogeneitySpace(ideal I, ring r);
void tropical_setup(SModulFunctions* p);

#endif
