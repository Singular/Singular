#ifndef GFANLIB_TROPICALVARIETYOFPOLYNOMIALS_H
#define GFANLIB_TROPICALVARIETYOFPOLYNOMIALS_H

#include <gfanlib/gfanlib.h>
#include <polys/monomials/p_polys.h>
#include <set>

#include <tropicalStrategy.h>

#ifndef NDEBUG
#include <Singular/subexpr.h> // for leftv
#include <bbfan.h>            // for fanID
#endif

struct ZConeCompareDimensionFirst
{
  bool operator() (const gfan::ZCone &zc, const gfan::ZCone &zd) const
    {
      int n = zc.dimension();
      int m = zd.dimension();
      if (n==m)
        return zc<zd;
      else
        return n>m;
    }
};

typedef std::set<gfan::ZCone,ZConeCompareDimensionFirst> ZConesSortedByDimension;

std::set<gfan::ZCone> tropicalVariety(const poly g, const ring r, const tropicalStrategy* currentCase);
ZConesSortedByDimension tropicalVarietySortedByDimension(const poly g, const ring r, const tropicalStrategy* currentCase);

#ifndef NDEBUG
BOOLEAN tropicalVariety00(leftv res, leftv args);
BOOLEAN tropicalVariety01(leftv res, leftv args);
#endif

#endif
