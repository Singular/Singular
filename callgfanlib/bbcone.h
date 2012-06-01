#ifndef BBCONE_H
#define BBCONE_H

#ifdef HAVE_FANS
#include <gfanlib/gfanlib.h>
#include <kernel/intvec.h>

extern int coneID;

void bbcone_setup();
number integerToNumber(const gfan::Integer &I);
bigintmat* zVectorToBigintmat(const gfan::ZVector &zv);
bigintmat* zMatrixToBigintmat(const gfan::ZMatrix &zm);
gfan::ZMatrix bigintmatToZMatrix(const bigintmat* bim);
gfan::ZVector bigintmatToZVector(const bigintmat* bim);

char* toString(gfan::ZMatrix const &zm);
std::string toString(const gfan::ZCone* const zc);

int getDimension(gfan::ZCone* zc);
int getCodimension(gfan::ZCone* zc);
int getLinealityDimension(gfan::ZCone* zc);
int isSimplicial(gfan::ZCone* zc);

gfan::ZCone liftUp(const gfan::ZCone &zc);

 #endif
#endif
