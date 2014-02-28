#ifndef CALLGFANLIB_CONVERSION_H
#define CALLGFANLIB_CONVERSION_H

#include <gfanlib/gfanlib.h>
#include <libpolys/coeffs/bigintmat.h>
#include <libpolys/coeffs/longrat.h>
#include <libpolys/coeffs/numbers.h>

gfan::Integer* numberToInteger(const number &n);
number integerToNumber(const gfan::Integer &I);
bigintmat* zVectorToBigintmat(const gfan::ZVector &zv);
bigintmat* zMatrixToBigintmat(const gfan::ZMatrix &zm);
gfan::ZMatrix* bigintmatToZMatrix(const bigintmat &bim);
gfan::ZVector* bigintmatToZVector(const bigintmat &bim);

gfan::ZVector intStar2ZVector(const int d, const int* i);
int* ZVectorToIntStar(const gfan::ZVector &v, bool &overflow);
char* toString(gfan::ZMatrix const &m);

gfan::ZFan* toFanStar(std::set<gfan::ZCone> setOfCones);
std::set<gfan::ZVector> rays(std::set<gfan::ZCone> setOfCones);

#endif
