#ifndef BBFAN_H
#define BBFAN_H

#include <Singular/mod2.h>
#ifdef HAVE_FANS

#include <gfanlib/gfanlib.h>

extern int fanID;

void bbfan_setup();

int getAmbientDimension(gfan::ZFan* zf);
int getCodimension(gfan::ZFan* zf);
int getDimension(gfan::ZFan* zf);
int getLinealityDimension(gfan::ZFan* zf);
int isSimplicial(gfan::ZFan* zf);
gfan::Matrix<gfan::Integer> rays(const gfan::ZFan* const zf);

#endif
#endif
