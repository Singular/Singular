#ifndef TROPICALDEBUG_H
#define TROPICALDEBUG_H

#ifndef SING_NDEBUG
#include "libpolys/polys/simpleideals.h"
#include "gfanlib/gfanlib.h"

bool checkForNonPositiveEntries(const gfan::ZVector &w);
bool checkForNonPositiveLaterEntries(const gfan::ZVector &w);

bool areIdealsEqual(ideal I, ring r, ideal J, ring s);
bool checkWeightVector(const ideal I, const ring r, const gfan::ZVector &weightVector, bool checkBorder=0);

bool checkOrderingAndCone(const ring r, const gfan::ZCone zc);
bool checkPolynomialInput(const ideal I, const ring r);
bool checkPolyhedralInput(const gfan::ZCone zc, const gfan::ZVector p);

#endif //SING_NDEBUG

#endif
