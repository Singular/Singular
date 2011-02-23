#ifndef BBCONE_H
#define BBCONE_H

#include <gfanlib/gfanlib.h>

extern int coneID;

void bbcone_setup();
intvec* zVector2Intvec(const gfan::ZVector zv);
intvec* zMatrix2Intvec(const gfan::ZMatrix zm);
gfan::ZMatrix intmat2ZMatrix(const intvec* iMat);
gfan::ZVector intvec2ZVector(const intvec* iVec);
int integerToInt(gfan::Integer const &V, bool &ok);

#endif
