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

class groebnerConeData
{

private:

  ideal I;
  ring r;
  gfan::ZCone c;
  gfan::ZVector p;

public:

  groebnerConeData();
  groebnerConeData(const groebnerConeData &sigma);
  groebnerConeData(const ideal &J, const ring &s, const gfan::ZCone &d, const gfan::ZVector &q);
  groebnerConeData(const ideal J, const ring s, const gfan::ZVector w);
  groebnerConeData(const ideal J, const ring s, const gfan::ZCone d);
  ~groebnerConeData();

  friend struct groebnerConeData_compare;

  ideal getIdeal() const { return this->I; };
  ring getRing() const { return this->r; };
  gfan::ZCone getCone() const { return this->c; };
  gfan::ZVector getInteriorPoint() const { return this->p; };

};

struct groebnerConeData_compare
{
  bool operator()(const groebnerConeData &sigma, const groebnerConeData &theta) const
  {
    const gfan::ZVector p1 = sigma.getInteriorPoint();
    const gfan::ZVector p2 = theta.getInteriorPoint();
    assume (p1.size() == p2.size());
    return p1 < p2;
  }
};

groebnerConeData maximalGroebnerConeData(ideal I, const ring r);

typedef std::set<groebnerConeData,groebnerConeData_compare> setOfGroebnerConeData;

gfan::ZFan* toFanStar(setOfGroebnerConeData setOfCones);

#endif
