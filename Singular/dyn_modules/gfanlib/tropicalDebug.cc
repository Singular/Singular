#ifndef SING_NDEBUG

#include "libpolys/polys/simpleideals.h"
#include "kernel/GBEngine/kstd1.h"

#include "gfanlib/gfanlib.h"

#include "std_wrapper.h"
#include "callgfanlib_conversion.h"
#include "tropical.h"


bool checkForNonPositiveEntries(const gfan::ZVector &w)
{
  for (unsigned i=0; i<w.size(); i++)
  {
    if (w[i].sign()<=0)
    {
      std::cout << "ERROR: non-positive weight in weight vector" << std::endl
                << "weight: " << w << std::endl;
      return false;
    }
  }
  return true;
}

bool checkForNonPositiveLaterEntries(const gfan::ZVector &w)
{
  for (unsigned i=1; i<w.size(); i++)
  {
    if (w[i].sign()<=0)
    {
      std::cout << "ERROR: non-positive weight in weight vector later entries" << std::endl
                << "weight: " << w << std::endl;
      return false;
    }
  }
  return true;
}

bool areIdealsEqual(ideal I, ring r, ideal J, ring s)
{
  ideal Is = idInit(IDELEMS(I));
  nMapFunc identity = n_SetMap(r->cf,s->cf);
  for (int i=0; i<IDELEMS(I); i++)
    Is->m[i] = p_PermPoly(I->m[i],NULL,r,s,identity,NULL,0);

  ring origin = currRing;
  if (origin!=s)
    rChangeCurrRing(s);
  ideal stdI = gfanlib_kStd_wrapper(Is,s);
  ideal stdJ = gfanlib_kStd_wrapper(J,s);
  ideal NF1 = kNF(stdI,s->qideal,stdJ);
  ideal NF2 = kNF(stdJ,s->qideal,stdI);
  if (origin!=s)
    rChangeCurrRing(origin);

  bool b = true;
  if (NF1 != NULL)
    b = b && idIs0(NF1);
  if (NF2 != NULL)
    b = b && idIs0(NF2);
  if (!b)
    std::cout << "ERROR: input ideals not equal!" << std::endl;

  id_Delete(&stdI,s);
  id_Delete(&stdJ,s);
  id_Delete(&NF1,s);
  id_Delete(&NF2,s);
  return (b);
}


bool checkWeightVector(const ideal I, const ring r, const gfan::ZVector &weightVector, bool checkBorder)
{
  gfan::ZCone cI = maximalGroebnerCone(I,r);
  if (!cI.contains(weightVector))
  {
    std::cout << "ERROR: weight vector not inside maximal Gröbner cone" << std::endl;
    return false;
  }
  if (checkBorder && cI.containsRelatively(weightVector))
  {
    std::cout << "ERROR: weight vector in the relative interior of maximal Gröbner cone" << std::endl;
    return false;
  }
  return true;
}

bool checkOrderingAndCone(const ring r, const gfan::ZCone zc)
{
  if (r)
  {
    if (r->order[0]==ringorder_dp)
      return true;
    int n = rVar(r);
    int* w = r->wvhdl[0];
    gfan::ZVector v = wvhdlEntryToZVector(n,w);
    if (r->order[0]==ringorder_ws)
      v = gfan::Integer((long)-1)*v;
    if (!zc.contains(v))
    {
      std::cout << "ERROR: weight of ordering not inside Groebner cone!" << std::endl;
      return false;
    }
  }
  return true;
}

bool checkPolynomialInput(const ideal I, const ring r)
{
  if (r) rTest(r);
  if (I && r) id_Test(I,r);
  return ((!I) || (I && r));
}

bool checkPolyhedralInput(const gfan::ZCone zc, const gfan::ZVector p)
{
  return zc.containsRelatively(p);
}

#endif
