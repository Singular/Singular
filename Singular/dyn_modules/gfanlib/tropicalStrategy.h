#ifndef GFANLIB_TROPICALSTRATEGY_H
#define GFANLIB_TROPICALSTRATEGY_H

#include <gfanlib/gfanlib_vector.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/simpleideals.h>
#include <set>

typedef gfan::ZVector (*wAdjAlg1)(gfan::ZVector);
typedef gfan::ZVector (*wAdjAlg2)(gfan::ZVector,gfan::ZVector);
typedef bool (*redAlg)(ideal,ring,number);

class tropicalStrategy
{
private:
  ring originalRing;
  ideal originalIdeal;
  int dimensionOfIdeal;
  ring startingRing;
  ideal startingIdeal;
  number uniformizingParameter;
  gfan::ZCone linealitySpace;
  bool onlyLowerHalfSpace;

  gfan::ZVector (*weightAdjustingAlgorithm1) (gfan::ZVector w);
  gfan::ZVector (*weightAdjustingAlgorithm2) (gfan::ZVector v, gfan::ZVector w);
  bool (*extraReductionAlgorithm) (ideal I, ring r, number p);

public:

  tropicalStrategy(const ideal I, const ring r);
  tropicalStrategy(const ideal J, const number p, const ring s);
  tropicalStrategy(const tropicalStrategy& currentStrategy);
  ~tropicalStrategy();
  tropicalStrategy& operator=(const tropicalStrategy& currentStrategy);


  ring getOriginalRing() const
  {
    rTest(originalRing);
    return originalRing;
  }

  ring getStartingRing() const
  {
    rTest(startingRing);
    return startingRing;
  }

  number getUniformizingParameter() const
  {
    return uniformizingParameter;
  }

  ideal getStartingIdeal() const
  {
    id_Test(startingIdeal,startingRing);
    return startingIdeal;
  }

  int getDimensionOfIdeal() const
  {
    return dimensionOfIdeal;
  }

  gfan::ZCone getHomogeneitySpace() const
  {
    return linealitySpace;
  }

  int getDimensionOfHomogeneitySpace() const
  {
    return linealitySpace.dimension();
  }

  bool restrictToLowerHalfSpace() const
  {
    return onlyLowerHalfSpace;
  }

  wAdjAlg1 getWeightAdjustingAlgorithm1() const
  {
    return weightAdjustingAlgorithm1;
  }

  wAdjAlg2 getWeightAdjustingAlgorithm2() const
  {
    return weightAdjustingAlgorithm2;
  }

  redAlg getExtraReductionAlgorithm() const
  {
    return extraReductionAlgorithm;
  }

  gfan::ZVector adjustWeightForHomogeneity(gfan::ZVector w) const
  {
    return this->weightAdjustingAlgorithm1(w);
  }

  gfan::ZVector adjustWeightUnderHomogeneity(gfan::ZVector v, gfan::ZVector w) const
  {
    return this->weightAdjustingAlgorithm2(v,w);
  }

  bool reduce(ideal I, ring r) const
  {
    rTest(r);  id_Test(I,r);
    return this->extraReductionAlgorithm(I,r,uniformizingParameter);
  }
};

int dim(ideal I, ring r);

#endif
