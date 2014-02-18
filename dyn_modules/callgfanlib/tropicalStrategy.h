#ifndef GFANLIB_TROPICALSTRATEGY_H
#define GFANLIB_TROPICALSTRATEGY_H

#include <gfanlib/gfanlib_vector.h>
#include <libpolys/polys/simpleideals.h>

class tropicalStrategy
{
public:
  gfan::ZVector (*adjustWeightForHomogeneity)(gfan::ZVector w);
  gfan::ZVector (*adjustWeightUnderHomogeneity)(gfan::ZVector v, gfan::ZVector w);
};

extern tropicalStrategy valuedCase;
extern tropicalStrategy nonValuedCase;

tropicalStrategy initializeValuedCase();
tropicalStrategy initializeNonValuedCase();

#endif
