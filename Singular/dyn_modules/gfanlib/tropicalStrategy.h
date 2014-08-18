#ifndef GFANLIB_TROPICALSTRATEGY_H
#define GFANLIB_TROPICALSTRATEGY_H

#include <gfanlib/gfanlib_vector.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/simpleideals.h>
#include <set>

/** \file
 * implementation of the class tropicalStrategy
 *
 * tropicalStrategy is a class that contains information relevant for
 * computing tropical varieties that is dependent on the valuation of the coefficient field.
 * It represents the mutable part of an overall framework that is capable of
 * computing tropical varieties both over coefficient fields without valuation and
 * with valuation (currently: only p-adic valuation over rational numbers)
 */

typedef gfan::ZVector (*wAdjAlg1)(gfan::ZVector);
typedef gfan::ZVector (*wAdjAlg2)(gfan::ZVector,gfan::ZVector);
typedef bool (*redAlg)(ideal,ring,number);

class tropicalStrategy
{
private:
  /**
   * polynomial ring over a field with valuation
   */
  ring originalRing;
  /**
   * input ideal, assumed to be a homogeneous prime ideal
   */
  ideal originalIdeal;
  /**
   * dimension of the input ideal
   */
  int dimensionOfIdeal;
  /**
   * the homogeneity space of the Grobner fan
   */
  gfan::ZCone linealitySpace;
  /**
   * polynomial ring over the valuation ring extended by one extra variable t
   */
  ring startingRing;
  /**
   * preimage of the input ideal under the map that sends t to the uniformizing parameter
   */
  ideal startingIdeal;
  /**
   * uniformizing parameter in the valuation ring
   */
  number uniformizingParameter;
  /**
   * polynomial ring over the residue field
   */
  ring shortcutRing;

  /**
   * true if valuation non-trivial, false otherwise
   */
  bool onlyLowerHalfSpace;

  /**
   * A function such that:
   * Given weight w, returns a strictly positive weight u such that an ideal satisfying
   * the valuation-sepcific homogeneity conditions is weighted homogeneous with respect to w
   * if and only if it is homogeneous with respect to u
   */
  gfan::ZVector (*weightAdjustingAlgorithm1) (gfan::ZVector w);
  /**
   * A function such that:
   * Given strictly positive weight w and weight v,
   * returns a strictly positive weight u such that on an ideal that is weighted homogeneous
   * with respect to w the weights u and v coincide
   */
  gfan::ZVector (*weightAdjustingAlgorithm2) (gfan::ZVector v, gfan::ZVector w);
  /**
   * A function that reduces the generators of an ideal I so that
   * the inequalities and equations of the Groebner cone can be read off.
   */
  bool (*extraReductionAlgorithm) (ideal I, ring r, number p);

public:

  /**
   * Constructor for the trivial valuation case
   */
  tropicalStrategy(const ideal I, const ring r);
  /**
   * Constructor for the non-trivial valuation case
   * p is the uniformizing parameter of the valuation
   */
  tropicalStrategy(const ideal J, const number p, const ring s);
  /**
   * copy constructor
   */
  tropicalStrategy(const tropicalStrategy& currentStrategy);
  /**
   * destructor
   */
  ~tropicalStrategy();
  /**
   * assignment operator
   **/
  tropicalStrategy& operator=(const tropicalStrategy& currentStrategy);


  /**
   * returns the polynomial ring over the field with valuation
   */
  ring getOriginalRing() const
  {
    rTest(originalRing);
    return originalRing;
  }

  /**
   * returns the polynomial ring over the valuation ring
   */
  ring getStartingRing() const
  {
    rTest(startingRing);
    return startingRing;
  }

  /**
   * returns the input ideal
   */
  ideal getStartingIdeal() const
  {
    id_Test(startingIdeal,startingRing);
    return startingIdeal;
  }

  /**
   * returns the dimension of the input ideal
   */
  int getDimensionOfIdeal() const
  {
    return dimensionOfIdeal;
  }

  /**
   * returns the uniformizing parameter in the valuation ring
   */
  number getUniformizingParameter() const
  {
    return uniformizingParameter;
  }

  /**
   * returns the homogeneity space of the preimage ideal
   */
  gfan::ZCone getHomogeneitySpace() const
  {
    return linealitySpace;
  }

  /**
   * returns the dimension of the homogeneity space
   */
  int getDimensionOfHomogeneitySpace() const
  {
    return linealitySpace.dimension();
  }

  /**
   * returns true, if valuation non-trivial, false otherwise
   */
  bool restrictToLowerHalfSpace() const
  {
    return onlyLowerHalfSpace;
  }

  /**
   * Given weight w, returns a strictly positive weight u such that an ideal satisfying
   * the valuation-sepcific homogeneity conditions is weighted homogeneous with respect to w
   * if and only if it is homogeneous with respect to u
   */
  gfan::ZVector adjustWeightForHomogeneity(gfan::ZVector w) const
  {
    return this->weightAdjustingAlgorithm1(w);
  }

  /**
   * Given strictly positive weight w and weight v,
   * returns a strictly positive weight u such that on an ideal that is weighted homogeneous
   * with respect to w the weights u and v coincide
   */
  gfan::ZVector adjustWeightUnderHomogeneity(gfan::ZVector v, gfan::ZVector w) const
  {
    return this->weightAdjustingAlgorithm2(v,w);
  }

  /**
   * reduces the generators of an ideal I so that
   * the inequalities and equations of the Groebner cone can be read off.
   */
  bool reduce(ideal I, ring r) const
  {
    rTest(r);  id_Test(I,r);
    nMapFunc nMap = n_SetMap(startingRing->cf,r->cf);
    number p = nMap(uniformizingParameter,startingRing->cf,r->cf);
    bool b = this->extraReductionAlgorithm(I,r,p);
    n_Delete(&p,r->cf);
    return b;
  }

  /**
   * returns true, if I contains a monomial.
   * returns false otherwise.
   **/
  bool containsMonomial(ideal I, ring r) const
  {
    ring rFinite = rCopy0(r);
    nKillChar(rFinite.coeffs());
    rFinite->cf =
  }
};

int dim(ideal I, ring r);

#endif
