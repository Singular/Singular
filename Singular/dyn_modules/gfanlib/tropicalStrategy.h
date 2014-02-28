#ifndef GFANLIB_TROPICALSTRATEGY_H
#define GFANLIB_TROPICALSTRATEGY_H

#include <gfanlib/gfanlib_vector.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/simpleideals.h>
#include <set>

class tropicalStrategy
{
public:
  std::set<gfan::ZCone> (*tropicalVarietyOfPolynomial)(const poly &g, const ring &r);
  gfan::ZVector (*adjustWeightForHomogeneity)(gfan::ZVector w);
  gfan::ZVector (*adjustWeightUnderHomogeneity)(gfan::ZVector v, gfan::ZVector w);
  bool (*reduce)(ideal I, ring r);
};

extern tropicalStrategy valuedCase;
extern tropicalStrategy nonValuedCase;

tropicalStrategy initializeValuedCase();
tropicalStrategy initializeNonValuedCase();

#endif
