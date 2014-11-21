#ifndef GFANLIB_TROPICALSTRATEGY_H
#define GFANLIB_TROPICALSTRATEGY_H

#include <gfanlib/gfanlib_vector.h>
#include <gfanlib/gfanlib_zcone.h>
#include <libpolys/polys/simpleideals.h>
#include <kernel/ideals.h> // for idSize
#include <set>
#include <callgfanlib_conversion.h>
#include <containsMonomial.h>
#include <flip.h>
#include <initial.h>
#include <witness.h>

#ifndef NDEBUG

#include <Singular/ipshell.h> // for isPrime(int i)
#include <adjustWeights.h>
#include <ppinitialReduction.h>

#endif

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
   * the expected Dimension of the polyhedral output,
   * i.e. the dimension of the ideal if valuation trivial
   * or the dimension of the ideal plus one if valuation non-trivial
   * (as the output is supposed to be intersected with a hyperplane)
   */
  int expectedDimension;
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

  ring copyAndChangeCoefficientRing(const ring r) const;
  ring copyAndChangeOrderingWP(const ring r, const gfan::ZVector w, const gfan::ZVector v) const;
  ring copyAndChangeOrderingLS(const ring r, const gfan::ZVector w, const gfan::ZVector v) const;

  /**
   * if valuation non-trivial, checks whether the generating system contains p-t
   * otherwise returns true
   */
  bool checkForUniformizingBinomial(const ideal I, const ring r) const;

  /**
   * if valuation non-trivial, checks whether the genearting system contains p
   * otherwise returns true
   */
  bool checkForUniformizingParameter(const ideal inI, const ring r) const;
  int findPositionOfUniformizingBinomial(const ideal I, const ring r) const;
  void putUniformizingBinomialInFront(ideal I, const ring r, const number q) const;

public:

  /**
   * Constructor for the trivial valuation case
   */
  tropicalStrategy(const ideal I, const ring r, const bool completelyHomogeneous=true, const bool completeSpace=true);
  /**
   * Constructor for the non-trivial valuation case
   * p is the uniformizing parameter of the valuation
   */
  tropicalStrategy(const ideal J, const number p, const ring s);
  /**
   * copy constructor
   */
  tropicalStrategy(const tropicalStrategy& currentStrategy);


#ifndef NDEBUG
  tropicalStrategy();

  static tropicalStrategy debugStrategy(const ideal startIdeal, number unifParameter, ring startRing);
#endif

  /**
   * destructor
   */
  ~tropicalStrategy();
  /**
   * assignment operator
   **/
  tropicalStrategy& operator=(const tropicalStrategy& currentStrategy);

  bool isConstantCoefficientCase() const
  {
    bool b = (uniformizingParameter==NULL);
    return b;
  }
  bool isValuationTrivial() const
  {
    bool b = (uniformizingParameter==NULL);
    return b;
  }
  bool isValuationNonTrivial() const
  {
    bool b = (uniformizingParameter!=NULL);
    return b;
  }

  /**
   * returns the polynomial ring over the field with valuation
   */
  ring getOriginalRing() const
  {
    rTest(originalRing);
    return originalRing;
  }

  /**
   * returns the input ideal over the field with valuation
   */
  ideal getOriginalIdeal() const
  {
    if (originalIdeal) id_Test(originalIdeal,originalRing);
    return originalIdeal;
  }

  /**
   * returns the polynomial ring over the valuation ring
   */
  ring getStartingRing() const
  {
    if (startingRing) rTest(startingRing);
    return startingRing;
  }

  /**
   * returns the input ideal
   */
  ideal getStartingIdeal() const
  {
    if (startingIdeal) id_Test(startingIdeal,startingRing);
    return startingIdeal;
  }

  /**
   * returns the expected Dimension of the polyhedral output
   */
  int getExpectedDimension() const
  {
    return expectedDimension;
  }

  /**
   * returns the uniformizing parameter in the valuation ring
   */
  number getUniformizingParameter() const
  {
    if (uniformizingParameter) n_Test(uniformizingParameter,startingRing->cf);
    return uniformizingParameter;
  }

  ring getShortcutRing() const
  {
    if (shortcutRing) rTest(shortcutRing);
    return shortcutRing;
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
   * If valuation trivial, returns a copy of r with a positive weight prepended,
   * such that any ideal homogeneous with respect to w is homogeneous with respect to that weight.
   * If valuation non-trivial, changes the coefficient ring to the residue field.
   */
  ring getShortcutRingPrependingWeight(const ring r, const gfan::ZVector w) const;

  /**
   * Returns a copy of the shortcutRing which is weighted with respect to w first and v second.
   * If valuation trivial, the coefficient ring will be the field with valuation.
   * If valuation non-trivial, the coefficient ring will be the residue field.
   */
  ring getShortcutRingDoublyWeighted(const gfan::ZVector w, const gfan::ZVector v) const;

  /**
   * copies r and prepends extra weight w
   */
  ring copyAndPrependWeight(const ring r, const gfan::ZVector w) const;

  /**
   * changes the coefficient ring of r to be the residue field
   */
  void changeCoefficientToResidueField(ring r) const;

  /**
   * reduces the generators of an ideal I so that
   * the inequalities and equations of the Groebner cone can be read off.
   */
  bool reduce(ideal I, const ring r) const;

  void pReduce(ideal I, const ring r) const;

  /**
   * returns true, if I contains a monomial.
   * returns false otherwise.
   **/
  poly checkInitialIdealForMonomial(const ideal I, const ring r, const gfan::ZVector w) const;

  /**
   * suppose w a weight in maximal groebner cone of >
   * suppose I (initially) reduced standard basis w.r.t. > and inI initial forms of its elements w.r.t. w
   * suppose inJ elements of initial ideal that are homogeneous w.r.t w
   * returns J elements of ideal whose initial form w.r.t. w are inI
   * in particular, if w lies also inthe maximal groebner cone of another ordering >'
   * and inJ is a standard basis of the initial ideal w.r.t. >'
   * then the returned J will be a standard baiss of the ideal w.r.t. >'
   */
  ideal getWitness(const ideal inJ, const ideal inI, const ideal I, const ring r) const;

  ideal computeLift(const ideal inJs, const ring s, const ideal inIr, const ideal Ir, const ring r) const;

  /**
   * given generators of the initial ideal, computes its standard basis
   */
  ideal getStdOfInitialIdeal(const ideal inI, const ring r) const;

  /**
   * given an interior point of a groebner cone
   * computes the groebner cone adjacent to it
   */
  std::pair<ideal,ring> getFlip(const ideal Ir, const ring r, const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal) const;
};

int dim(ideal I, ring r);

#ifndef NDEBUG
BOOLEAN getWitnessDebug(leftv res, leftv args);
BOOLEAN getFlipDebug(leftv res, leftv args);
#endif

#endif
