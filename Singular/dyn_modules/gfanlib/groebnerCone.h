#ifndef CALLGFANLIB_GROEBNERCONE_H
#define CALLGFANLIB_GROEBNERCONE_H

#include "kernel/polys.h"
#include "Singular/ipid.h"

#include "polys/monomials/ring.h"
#include "polys/simpleideals.h"
#include "kernel/ideals.h"
#include "gfanlib/gfanlib.h"

#include "tropicalStrategy.h"

/** \file
 * implementation of the class groebnerCone
 *
 * groebnerCone is a class that encapsulates relevant (possibly redundant) information about a groebnerCone.
 * Moreover, it contains implrementation of several highly non-trivial algorithms, such as computing its neighbours
 * in the Groebner fan or computing its neighbours in the tropical variety.
 */

class groebnerCone;
struct groebnerCone_compare;
typedef std::set<groebnerCone,groebnerCone_compare> groebnerCones;


class groebnerCone
{

private:
  /**
   * ideal to which this Groebner cone belongs to
   */
  ideal polynomialIdeal;
  /**
   * ring in which the ideal exists
   */
  ring polynomialRing;
  gfan::ZCone polyhedralCone;
  gfan::ZVector interiorPoint;
  const tropicalStrategy* currentStrategy;

public:
  groebnerCone();
  groebnerCone(const ideal I, const ring r, const tropicalStrategy& currentCase);
  groebnerCone(const ideal I, const ring r, const gfan::ZVector& w, const tropicalStrategy& currentCase);
  groebnerCone(const ideal I, const ring r, const gfan::ZVector& u, const gfan::ZVector& w, const tropicalStrategy& currentCase);
  groebnerCone(const ideal I, const ideal inI, const ring r, const tropicalStrategy& currentCase);
  groebnerCone(const groebnerCone& sigma);
  ~groebnerCone();
  groebnerCone& operator=(const groebnerCone& sigma);

  void deletePolynomialData()
  {
    assume ((!polynomialIdeal) || (polynomialIdeal && polynomialRing));
    if (polynomialIdeal) id_Delete(&polynomialIdeal,polynomialRing);
    if (polynomialRing) rDelete(polynomialRing);
    polynomialIdeal = NULL;
    polynomialRing = NULL;
  }

  ideal getPolynomialIdeal() const { return polynomialIdeal; };
  ring getPolynomialRing() const { return polynomialRing; };
  gfan::ZCone getPolyhedralCone() const { return polyhedralCone; };
  gfan::ZVector getInteriorPoint() const { return interiorPoint; };
  const tropicalStrategy* getTropicalStrategy() const { return currentStrategy; };
  friend struct groebnerCone_compare;

  bool isTrivial() const
  {
    bool b = (polynomialRing==NULL);
    return b;
  }

  /**
   * Returns true if Groebner cone contains w, false otherwise
   */
  bool contains(const gfan::ZVector &w) const;

  /**
   * Returns a point in the tropical variety, if the groebnerCone contains one.
   * Returns an empty vector otherwise.
   */
  gfan::ZVector tropicalPoint() const;

  /**
   * Given an interior point on the facet and the outer normal factor on the facet,
   * returns the adjacent groebnerCone sharing that facet
   */
  groebnerCone flipCone(const gfan::ZVector &interiorPoint, const gfan::ZVector &facetNormal) const;

  /**
   * Returns a complete list of neighboring Groebner cones.
   */
  groebnerCones groebnerNeighbours() const;

  /**
   * Returns a complete list of neighboring Groebner cones in the tropical variety.
   */
  groebnerCones tropicalNeighbours() const;

  /**
   * Return 1 if w points is in the dual of the polyhedral cone, 0 otherwise
   */
  bool pointsOutwards(const gfan::ZVector w) const;

  /**
   * Debug tools.
   */
  #ifndef NDEBUG
  bool checkFlipConeInput(const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal) const;
  #endif
};

struct groebnerCone_compare
{
  bool operator()(const groebnerCone &sigma, const groebnerCone &theta) const
  {
    const gfan::ZVector p1 = sigma.getInteriorPoint();
    const gfan::ZVector p2 = theta.getInteriorPoint();
    assume (p1.size() == p2.size());
    return p1 < p2;
  }
};

gfan::ZFan* toFanStar(groebnerCones setOfCones);

#endif
