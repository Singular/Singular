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
   * i.e. the dimension of the ideal if trivial valuation
   * or the dimension of the ideal plus one if non-trivial valuation
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
    rTest(startingRing);
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
    rTest(shortcutRing);
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
   * reduces the generators of an ideal I so that
   * the inequalities and equations of the Groebner cone can be read off.
   */
  bool reduce(ideal I, const ring r) const
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
  poly checkInitialIdealForMonomial(const ideal I, const ring r, const gfan::ZVector w) const
  {
    gfan::ZVector v = adjustWeightForHomogeneity(w);
    if (isConstantCoefficientCase())
    {
      ring rShortcut = rCopy0(r);
      bool overflow;
      /**
       * prepend extra weight vector for homogeneity
       */
      int* order = rShortcut->order;
      int* block0 = rShortcut->block0;
      int* block1 = rShortcut->block1;
      int** wvhdl = rShortcut->wvhdl;
      int h = rBlocks(r); int n = rVar(r);
      rShortcut->order = (int*) omAlloc0((h+1)*sizeof(int));
      rShortcut->block0 = (int*) omAlloc0((h+1)*sizeof(int));
      rShortcut->block1 = (int*) omAlloc0((h+1)*sizeof(int));
      rShortcut->wvhdl = (int**) omAlloc0((h+1)*sizeof(int*));
      rShortcut->order[0] = ringorder_a;
      rShortcut->block0[0] = 1;
      rShortcut->block1[0] = n;
      rShortcut->wvhdl[0] = ZVectorToIntStar(v,overflow);
      for (int i=1; i<=h; i++)
      {
        rShortcut->order[i] = order[i-1];
        rShortcut->block0[i] = block0[i-1];
        rShortcut->block1[i] = block1[i-1];
        rShortcut->wvhdl[i] = wvhdl[i-1];
      }
      rComplete(rShortcut);
      rTest(rShortcut);
      omFree(order);
      omFree(block0);
      omFree(block1);
      omFree(wvhdl);

      ideal inI = initial(I,r,w);
      int k = idSize(inI);
      ideal inIShortcut = idInit(k);
      nMapFunc identity = n_SetMap(r->cf,rShortcut->cf);
      for (int i=0; i<k; i++)
        inIShortcut->m[i] = p_PermPoly(inI->m[i],NULL,r,rShortcut,identity,NULL,0);

      poly p = checkForMonomialViaSuddenSaturation(inIShortcut,rShortcut);
      poly monomial = NULL;
      if (p!=NULL)
      {
        monomial=p_One(r);
        for (int i=1; i<n; i++)
          p_SetExp(monomial,i,p_GetExp(p,i,rShortcut),r);
        p_Delete(&p,rShortcut);
      }
      id_Delete(&inI,r);
      id_Delete(&inIShortcut,rShortcut);
      rDelete(rShortcut);
      return monomial;
    }
    else
    {
      ring rShortcut = rCopy0(r);
      bool overflow;
      /**
       * prepend extra weight vector for homogeneity
       */
      int* order = rShortcut->order;
      int* block0 = rShortcut->block0;
      int* block1 = rShortcut->block1;
      int** wvhdl = rShortcut->wvhdl;
      int h = rBlocks(r); int n = rVar(r);
      rShortcut->order = (int*) omAlloc0((h+1)*sizeof(int));
      rShortcut->block0 = (int*) omAlloc0((h+1)*sizeof(int));
      rShortcut->block1 = (int*) omAlloc0((h+1)*sizeof(int));
      rShortcut->wvhdl = (int**) omAlloc0((h+1)*sizeof(int*));
      rShortcut->order[0] = ringorder_a;
      rShortcut->block0[0] = 1;
      rShortcut->block1[0] = n;
      rShortcut->wvhdl[0] = ZVectorToIntStar(v,overflow);
      for (int i=1; i<=h; i++)
      {
        rShortcut->order[i] = order[i-1];
        rShortcut->block0[i] = block0[i-1];
        rShortcut->block1[i] = block1[i-1];
        rShortcut->wvhdl[i] = wvhdl[i-1];
      }
      omFree(order);
      omFree(block0);
      omFree(block1);
      omFree(wvhdl);
      /**
       * change ground domain into finite field
       */
      nKillChar(rShortcut->cf);
      rShortcut->cf = nCopyCoeff(shortcutRing->cf);
      rComplete(rShortcut);
      rTest(rShortcut);

      ideal inI = initial(I,r,w);
      int k = idSize(inI);
      ideal inIShortcut = idInit(k);
      nMapFunc takingResidues = n_SetMap(r->cf,rShortcut->cf);
      for (int i=0; i<k; i++)
        inIShortcut->m[i] = p_PermPoly(inI->m[i],NULL,r,rShortcut,takingResidues,NULL,0);

      idSkipZeroes(inIShortcut);
      poly p = checkForMonomialViaSuddenSaturation(inIShortcut,rShortcut);
      poly monomial = NULL;
      if (p!=NULL)
      {
        monomial=p_One(r);
        for (int i=1; i<n; i++)
          p_SetExp(monomial,i,p_GetExp(p,i,rShortcut),r);
        p_Delete(&p,rShortcut);
      }
      id_Delete(&inI,r);
      id_Delete(&inIShortcut,rShortcut);
      rDelete(rShortcut);
      return monomial;
    }
  }
  std::pair<ideal,ring> flip(const ideal I, const ring r,
                             const gfan::ZVector interiorPoint,
                             const gfan::ZVector facetNormal) const
  {
    gfan::ZVector adjustedInteriorPoint = adjustWeightForHomogeneity(interiorPoint);
    gfan::ZVector adjustedFacetNormal = adjustWeightUnderHomogeneity(facetNormal,adjustedInteriorPoint);
    return flip0(I,r,interiorPoint,facetNormal,adjustedInteriorPoint,adjustedFacetNormal);
  }
};

int dim(ideal I, ring r);

#endif
