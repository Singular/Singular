#ifndef CALLGFANLIB_GROEBNERCONE_H
#define CALLGFANLIB_GROEBNERCONE_H

#include <kernel/polys.h>
#include <Singular/ipid.h>

#include <libpolys/polys/monomials/ring.h>
#include <libpolys/polys/simpleideals.h>
#include <kernel/ideals.h>
#include <gfanlib/gfanlib.h>
#include <set>

gfan::ZCone sloppyGroebnerCone(const poly g, const ring r, const gfan::ZVector w);
gfan::ZCone sloppyGroebnerCone(const ideal I, const ring r, const gfan::ZVector w);
gfan::ZCone groebnerCone(const poly g, const ring r, const gfan::ZVector w);
gfan::ZCone groebnerCone(const ideal I, const ring r, const gfan::ZVector w);
gfan::ZCone fullGroebnerCone(const ideal &I, const ring &r);

class groebnerCone
{

private:

  ideal polynomialIdeal;
  ring polynomialRing;
  gfan::ZCone polyhedralCone;
  gfan::ZVector interiorPoint;

public:

  groebnerCone();
  groebnerCone(const groebnerCone &sigma);
  groebnerCone(const ideal I, const ring r, const gfan::ZCone c, const gfan::ZVector w);
  groebnerCone(const ideal I, const ring r, const gfan::ZVector w);
  groebnerCone(const ideal I, const ring r, const gfan::ZCone c);
  ~groebnerCone();

  ideal getPolynomialIdeal() const { return this->polynomialIdeal; };
  ring getPolynomialRing() const { return this->poynomialRing; };
  gfan::ZCone getPolyhedralCone() const { return this->polyhedralCone; };
  gfan::ZVector getInteriorPoint() const { return this->interiorPoint; };

  friend struct groebnerCone_compare;

  /***
   * Returns a point in the tropical variety, if the groebnerCone contains one.
   * Returns an empty vector otherwise.
   **/
  gfan::ZVector tropicalPoint() const;

  /***
   * Given a point w in the relative interior of a facet
   * and a direction u away from the groebnerCone,
   * computes the adjacent neighbour in direction u
   * sharing the facet that contains w.
   **/
  groebnerCone adjacentCone(const gfan::ZVector w, const gfan::ZVector u) const;
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

groebnerCone maximalGroebnerCone(ideal I, const ring r);

typedef std::set<groebnerCone,groebnerCone_compare> groebnerCones;

gfan::ZFan* toFanStar(groebnerCones setOfCones);

#endif
