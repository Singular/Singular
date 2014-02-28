#include <set>
#include <gfanlib/gfanlib.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <callgfanlib_conversion.h>

#ifndef NDEBUG
#include <Singular/subexpr.h> // for leftv
#include <bbfan.h>            // for fanID
#endif

/***
 * Returns the tropical variety of a polynomial g in ring r
 **/
std::set<gfan::ZCone> fullTropicalVarietyOfPolynomial(const poly &g, const ring &r)
{
  int n = r->N;
  std::set<gfan::ZCone> setOfCones;
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
    for (int i=0; i<l; i++)
    {
      for (int j=i+1; j<l; j++)
      {
        gfan::ZMatrix equation = gfan::ZMatrix(0,n);
        equation.appendRow(exponents[i]-exponents[j]);
        gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
        for (int k=0; k<l; k++)
          if (k!=i && k!=j) inequalities.appendRow(exponents[i]-exponents[k]);
        gfan::ZCone zc = gfan::ZCone(inequalities,equation);
        if (zc.dimension()>=n-1)
        {
          zc.canonicalize();
          setOfCones.insert(zc);
        }
      }
    }
  }
  return setOfCones;
}

/***
 * Returns the tropical variety of a polynomial g in ring r
 * intersected with the lower halfspace
 **/
std::set<gfan::ZCone> lowerTropicalVarietyOfPolynomial(const poly &g, const ring &r)
{
  int n = r->N;
  std::set<gfan::ZCone> setOfCones;
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
    for (int i=0; i<l; i++)
    {
      for (int j=i+1; j<l; j++)
      {
        gfan::ZMatrix equation = gfan::ZMatrix(0,n);
        equation.appendRow(exponents[i]-exponents[j]);
        gfan::ZMatrix inequalities = gfan::ZMatrix(1,n);
        inequalities[0][0] = -1;
        for (int k=0; k<l; k++)
          if (k!=i && k!=j) inequalities.appendRow(exponents[i]-exponents[k]);
        gfan::ZCone zc = gfan::ZCone(inequalities,equation);
        if (zc.dimension()>=n-1)
        {
          zc.canonicalize();
          setOfCones.insert(zc);
        }
      }
    }
  }
  return setOfCones;
}

#ifndef NDEBUG
BOOLEAN tropicalVariety00(leftv res, leftv args)
{
  leftv u = args;
  poly g = (poly) u->CopyD();
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  std::set<gfan::ZCone> C = fullTropicalVarietyOfPolynomial(g,currRing);
  p_Delete(&g,currRing);
  res->rtyp = fanID;
  res->data = (char*) toFanStar(C);
  return FALSE;
}
BOOLEAN tropicalVariety01(leftv res, leftv args)
{
  leftv u = args;
  poly g = (poly) u->CopyD();
  omUpdateInfo();
  Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
  std::set<gfan::ZCone> C = lowerTropicalVarietyOfPolynomial(g,currRing);
  p_Delete(&g,currRing);
  res->rtyp = fanID;
  res->data = (char*) toFanStar(C);
  return FALSE;
}
#endif
