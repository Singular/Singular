#ifndef BBFAN_H
#define BBFAN_H

#ifdef HAVE_FANS
#include <gfanlib/gfanlib.h>

extern int fanID;

void bbfan_setup();

int getAmbientDimension(gfan::ZFan* zf);
int getCodimension(gfan::ZFan* zf);
int getDimension(gfan::ZFan* zf);
int getLinealityDimension(gfan::ZFan* zf);

#endif
#endif
