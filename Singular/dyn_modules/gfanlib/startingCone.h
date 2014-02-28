#ifndef STARTING_CONE_H
#define STARTING_CONE_H

#include <groebnerCone.h>
/* namespace tropical */
/* { */

/*   class facet */
/*   { */
/*     gfan::ZVector interiorPoint; */
/*     gfan::ZVector facetNormal; */

/*   public: */

/*     facet(); */
/*     facet(const facet &f); */
/*     facet(const gfan::ZCone &c, const gfan::ZVector &v, const gfan::ZVector &w); */
/*     ~facet(); */

/*     gfan::ZCone getEta() { return this->eta; }; */
/*     gfan::ZVector getInteriorPoint() { return this->interiorPoint; }; */
/*     gfan::ZVector getFacetNormal() { return this->facetNormal; }; */

/*     friend struct facet_compare; */
/*   }; */

/*   struct facet_compare */
/*   { */
/*     bool operator()(const facet &f, const facet &g) const */
/*     { */
/*       const gfan::ZVector v1 = f.interiorPoint; */
/*       const gfan::ZVector v2 = g.interiorPoint; */
/* #ifndef NDEBUG */
/*       assume(v1.size() == v2.size()); */
/* #endif */
/*       return v1 < v2; */
/*     } */
/*   }; */

/*   typedef std::set<facet,facet_compare> facets; */

/*   void mergeFacets(facets &F, const facets &newFacets); */

/* } */

gfan::ZVector containsTropicalPoint(const groebnerConeData sigma);
gfan::ZVector tropicalStartingPoint(ideal I, ring r);

BOOLEAN tropicalStartingPoint0(leftv res, leftv args);
BOOLEAN tropicalStartingPoint1(leftv res, leftv args);

#endif
