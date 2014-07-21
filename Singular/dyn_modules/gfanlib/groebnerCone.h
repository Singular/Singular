#ifndef CALLGFANLIB_GROEBNERCONE_H
#define CALLGFANLIB_GROEBNERCONE_H

#include <kernel/polys.h>
#include <Singular/ipid.h>

#include <libpolys/polys/monomials/ring.h>
#include <libpolys/polys/simpleideals.h>
#include <kernel/ideals.h>
#include <gfanlib/gfanlib.h>
#include <set>

#include <tropicalStrategy.h>

class groebnerCone;
struct groebnerCone_compare;
typedef std::set<groebnerCone,groebnerCone_compare> groebnerCones;


class groebnerCone
{

private:
  ideal polynomialIdeal;
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

  void deletePolynomialIdeal()
  {
    assume ((!polynomialIdeal) || (polynomialIdeal && polynomialRing));
    if (polynomialIdeal) id_Delete(&polynomialIdeal,polynomialRing);
  }

  void deletePolynomialRing()
  {
    assume ((!polynomialIdeal) || (polynomialIdeal && polynomialRing));
    if (polynomialRing) rDelete(polynomialRing);
    polynomialRing = NULL;
  }

  ideal getPolynomialIdeal() const { return polynomialIdeal; };
  ring getPolynomialRing() const { return polynomialRing; };
  gfan::ZCone getPolyhedralCone() const { return polyhedralCone; };
  gfan::ZVector getInteriorPoint() const { return interiorPoint; };
  const tropicalStrategy* getTropicalStrategy() const {return currentStrategy; };

  friend struct groebnerCone_compare;

  /***
   * Returns a point in the tropical variety, if the groebnerCone contains one.
   * Returns an empty vector otherwise.
   **/
  gfan::ZVector tropicalPoint() const;

  /***
   * Given an interior point on the facet and the outer normal factor on the facet,
   * returns the adjacent groebnerCone sharing that facet
   **/
  bool checkFlipConeInput(const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal) const;
  groebnerCone flipCone(const gfan::ZVector interiorPoint, const gfan::ZVector facetNormal) const;

  /***
   * Returns a complete list of neighboring Groebner cones.
   **/
  groebnerCones groebnerNeighbours() const;

  /***
   * Returns a complete list of neighboring Groebner cones in the tropical variety.
   **/
  groebnerCones tropicalNeighbours() const;
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
