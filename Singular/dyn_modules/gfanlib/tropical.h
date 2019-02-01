#ifndef TROPICAL_H
#define TROPICAL_H

#include "Singular/ipid.h"


EXTERN_VAR int tropicalVerboseLevel;


gfan::ZCone maximalGroebnerCone(const ideal &I, const ring &r);
gfan::ZCone homogeneitySpace(ideal I, ring r);
void tropical_setup(SModulFunctions* p);

#endif
