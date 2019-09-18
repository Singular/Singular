#ifndef BBCONE_H
#define BBCONE_H

#include "kernel/mod2.h"

#if HAVE_GFANLIB

#include "misc/intvec.h"
#include "coeffs/bigintmat.h"
#include "Singular/ipid.h"

#include "gfanlib/gfanlib.h"

THREAD_VAR extern int coneID;

void bbcone_setup(SModulFunctions* p);

std::string toString(const gfan::ZCone* const c);

gfan::ZVector randomPoint(const gfan::ZCone* zc, const int b=0);
gfan::ZCone liftUp(const gfan::ZCone &zc);
gfan::ZMatrix interiorPointsOfFacets(const gfan::ZCone &zc, const std::set<gfan::ZVector> &exceptThese=std::set<gfan::ZVector>());
std::pair<gfan::ZMatrix,gfan::ZMatrix> interiorPointsAndNormalsOfFacets(const gfan::ZCone zc, const std::set<gfan::ZVector> &exceptThesePoints=std::set<gfan::ZVector>(), const bool onlyLowerHalfSpace=false);

#endif
#endif
