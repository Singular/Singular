#ifndef BBCONE_H
#define BBCONE_H

#ifdef HAVE_FANS
#include <gfanlib/gfanlib.h>
#include <kernel/intvec.h>

extern int coneID;

void bbcone_setup();
intvec* zVector2Intvec(const gfan::ZVector zv);
intvec* zMatrix2Intvec(const gfan::ZMatrix zm);
gfan::ZMatrix intmat2ZMatrix(const intvec* iMat);
gfan::ZVector intStar2ZVector(const int d, const int* i);
gfan::ZVector intvec2ZVector(const intvec* iVec);
std::string toString(gfan::ZMatrix const &m, char *tab=0);
int integerToInt(gfan::Integer const &V, bool &ok);

int getDimension(gfan::ZCone* zc);
int getCodimension(gfan::ZCone* zc);
int getLinealityDimension(gfan::ZCone* zc);
int isSimplicial(gfan::ZCone* zc);

bool containsInSupport(gfan::ZCone* zc, gfan::ZCone* zd);
bool containsInSupport(gfan::ZCone* zc, intvec* vec);

#endif
#endif
