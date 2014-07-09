#include <set>
#include <gfanlib/gfanlib.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>

#include <tropicalStrategy.h>

#ifndef NDEBUG
#include <Singular/subexpr.h> // for leftv
#include <bbfan.h>            // for fanID
#endif

/***
 * Returns the tropical variety of a polynomial g in ring r.
 * Depending on the current case, it will compute either the whole tropical variety
 * or just the part in the first lower half-space.
 * An empty set means that the tropical Variety is the whole space,
 * i.e. g is either 0 or a non-zero monomial.
 **/
std::set<gfan::ZCone> tropicalVariety(const poly g, const ring r, const tropicalStrategy &currentCase)
{
  int n = rVar(r);
  std::set<gfan::ZCone> tropVar;
  if (g && g->next)
  {
    int* expv = (int*) omAlloc((n+1)*sizeof(int));
    gfan::ZMatrix exponents = gfan::ZMatrix(0,n);
    for (poly s=g; s; pIter(s))
    {
      p_GetExpV(s,expv,r);
      gfan::ZVector zv = intStar2ZVector(n,expv);
      exponents.appendRow(intStar2ZVector(n,expv));
    }
    omFreeSize(expv,(n+1)*sizeof(int));

    int l = exponents.getHeight();
    gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
    lowerHalfSpaceCondition[0] = -1;
    for (int i=0; i<l; i++)
    {
      for (int j=i+1; j<l; j++)
      {
        gfan::ZMatrix equation = gfan::ZMatrix(0,n);
        equation.appendRow(exponents[i]-exponents[j]);
        gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
        if (currentCase.restrictToLowerHalfSpace())
          inequalities.appendRow(lowerHalfSpaceCondition);
        for (int k=0; k<l; k++)
          if (k!=i && k!=j) inequalities.appendRow(exponents[i]-exponents[k]);
        gfan::ZCone zc = gfan::ZCone(inequalities,equation);
        if (zc.dimension()>=n-1)
        {
          zc.canonicalize();
          tropVar.insert(zc);
        }
      }
    }
  }
  return tropVar;
}


// #ifndef NDEBUG
// BOOLEAN tropicalVariety00(leftv res, leftv args)
// {
//   leftv u = args;
//   poly g = (poly) u->CopyD();
//   omUpdateInfo();
//   Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
//   std::set<gfan::ZCone> C = fullTropicalVarietyOfPolynomial(g,currRing);
//   p_Delete(&g,currRing);
//   res->rtyp = fanID;
//   res->data = (char*) toFanStar(C);
//   return FALSE;
// }
// BOOLEAN tropicalVariety01(leftv res, leftv args)
// {
//   leftv u = args;
//   poly g = (poly) u->CopyD();
//   omUpdateInfo();
//   Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
//   std::set<gfan::ZCone> C = lowerTropicalVarietyOfPolynomial(g,currRing);
//   p_Delete(&g,currRing);
//   res->rtyp = fanID;
//   res->data = (char*) toFanStar(C);
//   return FALSE;
// }
// #endif
