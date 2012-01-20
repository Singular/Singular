#ifndef BBCONE_H
#define BBCONE_H

#ifdef HAVE_FANS
#include <gfanlib/gfanlib.h>

extern int coneID;

void bbcone_setup();
intvec* zVector2Intvec(const gfan::ZVector zv);
intvec* zMatrix2Intvec(const gfan::ZMatrix zm);
gfan::ZMatrix intmat2ZMatrix(const intvec* iMat);
gfan::ZVector intvec2ZVector(const intvec* iVec);
int integerToInt(gfan::Integer const &V, bool &ok);

int getAmbientDimension(gfan::ZCone* zc);
int getDimension(gfan::ZCone* zc);
int getCodimension(gfan::ZCone* zc);
int getLinealityDimension(gfan::ZCone* zc);
int isSimplicial(gfan::ZCone* zc);

bool containsInSupport(gfan::ZCone* zc, gfan::ZCone* zd);
bool containsInSupport(gfan::ZCone* zc, intvec* vec);

#endif
#endif
