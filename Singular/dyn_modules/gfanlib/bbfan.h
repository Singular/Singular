#ifndef BBFAN_H
#define BBFAN_H

#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "Singular/ipid.h"

#include "gfanlib/gfanlib.h"

EXTERN_VAR int fanID;

void bbfan_setup(SModulFunctions* p);

bool isCompatible(const gfan::ZFan* zf, const gfan::ZCone* zc);

int getAmbientDimension(gfan::ZFan* zf);
int getCodimension(gfan::ZFan* zf);
int getDimension(gfan::ZFan* zf);
int getLinealityDimension(gfan::ZFan* zf);
int isSimplicial(gfan::ZFan* zf);
gfan::Matrix<gfan::Integer> rays(const gfan::ZFan* const zf);

gfan::ZFan commonRefinement(gfan::ZFan zf, gfan::ZFan zg);
#endif
#endif
