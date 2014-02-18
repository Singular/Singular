#ifndef TROPICAL_CURVE_H
#define TROPICAL_CURVE_H

#include <gfanlib/gfanlib_zcone.h>
#include <gfanlib/gfanlib_zfan.h>
#include <libpolys/polys/monomials/p_polys.h>

typedef std::set<gfan::ZCone> gfan::ZCones;

class tropicalCurve
{
private:
  std::set<gfan::ZCone> cones;
  int expectedDimension;
public:
  tropicalCurve();
  tropicalCurve(const int d);
  tropicalCurve(const tropicalCurve &C);
  tropicalCurve(const poly &g, const ring &r);

  std::set<gfan::ZCone>* getCones();
  void intersect(const tropicalCurve &C);
};

gfan::ZFan tropicalCurve(const ideal I, const gfan::ZVector w, const ring r);

#ifndef NDEBUG
BOOLEAN tropicalCurve0(leftv res, leftv args);
BOOLEAN tropicalCurve1(leftv res, leftv args);
BOOLEAN tropicalCurve2(leftv res, leftv args);
#endif

#endif
