#include <libpolys/polys/monomials/p_polys.h>
#include <libpolys/coeffs/coeffs.h>

#include <bbcone.h>
#include <ppinitialReduction.h>
#include <ttinitialReduction.h>
#include <containsMonomial.h>

poly initial(poly p)
{
  poly g = p;
  poly h = p_Head(g,currRing);
  poly f = h;
  long d = p_Deg(g,currRing);
  pIter(g);
  while ((g != NULL) && (p_Deg(g,currRing) == d))
  {
    pNext(h) = p_Head(g,currRing);
    pIter(h);
    pIter(g);
  }
  return(f);
}


BOOLEAN initial(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      poly p = (poly) u->Data();
      res->rtyp = POLY_CMD;
      res->data = (void*) initial(p);
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      ideal inI = idInit(IDELEMS(I));
      for (int i=0; i<IDELEMS(I); i++)
        inI->m[i]=initial(I->m[i]);
      res->rtyp = IDEAL_CMD;
      res->data = (void*) inI;
      return FALSE;
    }
  }
  WerrorS("initial: unexpected parameters");
  return TRUE;
}


BOOLEAN homogeneitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix equations = gfan::ZMatrix(0,n);
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        while (g != NULL)
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          equations.appendRow(leadexpw-tailexpw);
          pIter(g);
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(gfan::ZMatrix(0, equations.getWidth()),equations);
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("homogeneitySpace: unexpected parameters");
  return TRUE;
}


BOOLEAN groebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      int n = currRing->N;
      ideal I = (ideal) u->Data();
      poly g = NULL;
      int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
      int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
      gfan::ZVector leadexpw = gfan::ZVector(n);
      gfan::ZVector tailexpw = gfan::ZVector(n);
      gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
      gfan::ZMatrix equations = gfan::ZMatrix(0,n);
      long d;
      for (int i=0; i<IDELEMS(I); i++)
      {
        g = (poly) I->m[i]; pGetExpV(g,leadexpv);
        leadexpw = intStar2ZVector(n, leadexpv);
        pIter(g);
        d = p_Deg(g,currRing);
        while ((g != NULL) && (p_Deg(g,currRing) == d))
        {
          pGetExpV(g,tailexpv);
          tailexpw = intStar2ZVector(n, tailexpv);
          equations.appendRow(leadexpw-tailexpw);
          pIter(g);
        }

        if (g != NULL)
        {
          while (g != NULL)
          {
            pGetExpV(g,tailexpv);
            tailexpw = intStar2ZVector(n, tailexpv);
            inequalities.appendRow(leadexpw-tailexpw);
            pIter(g);
          }
        }
      }
      gfan::ZCone* gCone = new gfan::ZCone(inequalities,equations);
      omFreeSize(leadexpv,(n+1)*sizeof(int));
      omFreeSize(tailexpv,(n+1)*sizeof(int));

      res->rtyp = coneID;
      res->data = (void*) gCone;
      return FALSE;
    }
  }
  WerrorS("groebnerCone: unexpected parameters");
  return TRUE;
}


gfan::ZCone* maximalGroebnerCone(const ring &r, const ideal &I)
{
  int n = rVar(r);
  poly g = NULL;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) I->m[i]; pGetExpV(g,leadexpv);
    leadexpw = intStar2ZVector(n, leadexpv);
    pIter(g);
    while (g != NULL)
    {
      pGetExpV(g,tailexpv);
      tailexpw = intStar2ZVector(n, tailexpv);
      inequalities.appendRow(leadexpw-tailexpw);
      pIter(g);
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  return new gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
}


BOOLEAN maximalGroebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      res->rtyp = coneID;
      res->data = (void*) maximalGroebnerCone(currRing, I);
      return FALSE;
    }
  }
  WerrorS("maximalGroebnerCone: unexpected parameters");
  return TRUE;
}


// gfan::ZCone* startingCone(ideal I)
// {
//   I = kStd(I,NULL,isNotHomog,NULL);
//   gfan::ZCone* zc = maximalGroebnerCone(currRing,I);
//   gfan::ZMatrix rays = zc->extremeRays();
//   gfan::ZVector v;
//   for (int i=0; i<rays.getHeight(); i++)
//   {
//     v = rays[i];
//   }
//   return zc;
// }


void tropical_setup(SModulFunctions* p)
{
  p->iiAddCproc("","groebnerCone",FALSE,groebnerCone);
  p->iiAddCproc("","maximalGroebnerCone",FALSE,maximalGroebnerCone);
  p->iiAddCproc("","initial",FALSE,initial);
#ifndef NDEBUG
  p->iiAddCproc("","pppReduce",FALSE,pppReduce);
  p->iiAddCproc("","ppreduceInitially0",FALSE,ppreduceInitially0);
  p->iiAddCproc("","ppreduceInitially1",FALSE,ppreduceInitially1);
  p->iiAddCproc("","ppreduceInitially2",FALSE,ppreduceInitially2);
  p->iiAddCproc("","ppreduceInitially3",FALSE,ppreduceInitially3);
  p->iiAddCproc("","ppreduceInitially4",FALSE,ppreduceInitially4);
  p->iiAddCproc("","ttpReduce",FALSE,ttpReduce);
  p->iiAddCproc("","ttreduceInitially0",FALSE,ttreduceInitially0);
  p->iiAddCproc("","ttreduceInitially1",FALSE,ttreduceInitially1);
  p->iiAddCproc("","ttreduceInitially2",FALSE,ttreduceInitially2);
  p->iiAddCproc("","ttreduceInitially3",FALSE,ttreduceInitially3);
  p->iiAddCproc("","ttreduceInitially4",FALSE,ttreduceInitially4);
#endif //NDEBUG
  p->iiAddCproc("","ppreduceInitially",FALSE,ppreduceInitially);
  p->iiAddCproc("","ttreduceInitially",FALSE,ttreduceInitially);
  p->iiAddCproc("","homogeneitySpace",FALSE,homogeneitySpace);
  p->iiAddCproc("","containsMonomial",FALSE,containsMonomial);
}
