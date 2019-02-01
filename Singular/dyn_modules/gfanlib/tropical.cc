#include "polys/monomials/p_polys.h"
#include "coeffs/coeffs.h"

#include "callgfanlib_conversion.h"
#include "bbcone.h"
#include "ppinitialReduction.h"
#include "containsMonomial.h"
#include "initial.h"
#include "witness.h"
#include "tropicalCurves.h"
#include "tropicalStrategy.h"
#include "startingCone.h"
#include "groebnerFan.h"
#include "groebnerComplex.h"
#include "tropicalVariety.h"

VAR int tropicalVerboseLevel = 0;

gfan::ZCone homogeneitySpace(ideal I, ring r)
{
  int n = rVar(r);
  poly g;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) I->m[i];
    if (g)
    {
      p_GetExpV(g,leadexpv,r);
      leadexpw = intStar2ZVector(n,leadexpv);
      pIter(g);
      while (g)
      {
        p_GetExpV(g,tailexpv,r);
        tailexpw = intStar2ZVector(n,tailexpv);
        equations.appendRow(leadexpw-tailexpw);
        pIter(g);
      }
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  return gfan::ZCone(gfan::ZMatrix(0, equations.getWidth()),equations);
}


BOOLEAN homogeneitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      poly g = (poly) u->Data();
      ideal I = idInit(1);
      I->m[0] = g;
      res->rtyp = coneID;
      res->data = (void*) new gfan::ZCone(homogeneitySpace(I,currRing));
      I->m[0] = NULL;
      id_Delete(&I,currRing);
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      res->rtyp = coneID;
      res->data = (void*) new gfan::ZCone(homogeneitySpace(I,currRing));
      return FALSE;
    }
  }
  WerrorS("homogeneitySpace: unexpected parameters");
  return TRUE;
}


gfan::ZCone lowerHomogeneitySpace(ideal I, ring r)
{
  int n = rVar(r);
  poly g;
  int* leadexpv = (int*) omAlloc((n+1)*sizeof(int));
  int* tailexpv = (int*) omAlloc((n+1)*sizeof(int));
  gfan::ZVector leadexpw = gfan::ZVector(n);
  gfan::ZVector tailexpw = gfan::ZVector(n);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) I->m[i];
    if (g)
    {
      p_GetExpV(g,leadexpv,r);
      leadexpw = intStar2ZVector(n,leadexpv);
      pIter(g);
      while (g)
      {
        p_GetExpV(g,tailexpv,r);
        tailexpw = intStar2ZVector(n,tailexpv);
        equations.appendRow(leadexpw-tailexpw);
        pIter(g);
      }
    }
  }
  gfan::ZMatrix inequalities = gfan::ZMatrix(0,n);
  gfan::ZVector lowerHalfSpaceCondition = gfan::ZVector(n);
  lowerHalfSpaceCondition[0] = -1;
  inequalities.appendRow(lowerHalfSpaceCondition);

  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  return gfan::ZCone(inequalities,equations);
}


BOOLEAN lowerHomogeneitySpace(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      poly g = (poly) u->Data();
      ideal I = idInit(1);
      I->m[0] = g;
      res->rtyp = coneID;
      res->data = (void*) new gfan::ZCone(lowerHomogeneitySpace(I,currRing));
      I->m[0] = NULL;
      id_Delete(&I,currRing);
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      ideal I = (ideal) u->Data();
      res->rtyp = coneID;
      res->data = (void*) new gfan::ZCone(lowerHomogeneitySpace(I,currRing));
      return FALSE;
    }
  }
  WerrorS("lowerHomogeneitySpace: unexpected parameters");
  return TRUE;
}


gfan::ZCone groebnerCone(const ideal I, const ring r, const gfan::ZVector &w)
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
    g = (poly) I->m[i];
    if (g!=NULL)
    {
      p_GetExpV(g,leadexpv,currRing);
      leadexpw = intStar2ZVector(n, leadexpv);
      pIter(g);
      while (g!=NULL)
      {
        p_GetExpV(g,tailexpv,currRing);
        tailexpw = intStar2ZVector(n, tailexpv);
        inequalities.appendRow(leadexpw-tailexpw);
        pIter(g);
      }
    }
  }

  ideal inI = initial(I,currRing,w);
  gfan::ZMatrix equations = gfan::ZMatrix(0,n);
  for (int i=0; i<IDELEMS(I); i++)
  {
    g = (poly) inI->m[i];
    if (g!=NULL)
    {
      p_GetExpV(g,leadexpv,currRing);
      leadexpw = intStar2ZVector(n, leadexpv);
      pIter(g);
      while (g!=NULL)
      {
        p_GetExpV(g,tailexpv,currRing);
        tailexpw = intStar2ZVector(n, tailexpv);
        equations.appendRow(leadexpw-tailexpw);
        pIter(g);
      }
    }
  }

  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  id_Delete(&inI,currRing);
  return gfan::ZCone(inequalities,equations);
}


BOOLEAN groebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if ((v !=NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      try
      {
        poly g = (poly) u->Data();
        ideal I = idInit(1);
        I->m[0] = g;
        gfan::ZVector* weightVector;
        if (v->Typ() == INTVEC_CMD)
        {
          intvec* w0 = (intvec*) v->Data();
          bigintmat* w1 = iv2bim(w0,coeffs_BIGINT);
          w1->inpTranspose();
          weightVector = bigintmatToZVector(*w1);
          delete w1;
        }
        else
        {
          bigintmat* w1 = (bigintmat*) v->Data();
          weightVector = bigintmatToZVector(*w1);
        }
        res->rtyp = coneID;
        res->data = (void*) new gfan::ZCone(groebnerCone(I,currRing,*weightVector));
        delete weightVector;
        I->m[0] = NULL;
        id_Delete(&I,currRing);
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v !=NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      try
      {
        ideal I = (ideal) u->Data();
        gfan::ZVector* weightVector;
        if (v->Typ() == INTVEC_CMD)
        {
          intvec* w0 = (intvec*) v->Data();
          bigintmat* w1 = iv2bim(w0,coeffs_BIGINT);
          w1->inpTranspose();
          weightVector = bigintmatToZVector(*w1);
          delete w1;
        }
        else
        {
          bigintmat* w1 = (bigintmat*) v->Data();
          weightVector = bigintmatToZVector(*w1);
        }
        res->rtyp = coneID;
        res->data = (void*) new gfan::ZCone(groebnerCone(I,currRing,*weightVector));
        delete weightVector;
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
  }
  WerrorS("groebnerCone: unexpected parameters");
  return TRUE;
}


gfan::ZCone maximalGroebnerCone(const ideal &I, const ring &r)
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
    g = (poly) I->m[i];
    if (g != NULL && pNext(g) != NULL)
    {
      p_GetExpV(g,leadexpv,r);
      leadexpw = intStar2ZVector(n, leadexpv);
      pIter(g);
      while (g != NULL)
      {
        p_GetExpV(g,tailexpv,r);
        tailexpw = intStar2ZVector(n, tailexpv);
        inequalities.appendRow(leadexpw-tailexpw);
        pIter(g);
      }
    }
  }
  omFreeSize(leadexpv,(n+1)*sizeof(int));
  omFreeSize(tailexpv,(n+1)*sizeof(int));
  return gfan::ZCone(inequalities,gfan::ZMatrix(0, inequalities.getWidth()));
}


BOOLEAN maximalGroebnerCone(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      try
      {
        poly g = (poly) u->Data();
        ideal I = idInit(1);
        I->m[0] = g;
        res->rtyp = coneID;
        res->data = (void*) new gfan::ZCone(maximalGroebnerCone(I,currRing));
        I->m[0] = NULL;
        id_Delete(&I,currRing);
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if (v == NULL)
    {
      try
      {
        ideal I = (ideal) u->Data();
        res->rtyp = coneID;
        res->data = (void*) new gfan::ZCone(maximalGroebnerCone(I,currRing));
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
  }
  WerrorS("maximalGroebnerCone: unexpected parameters");
  return TRUE;
}


BOOLEAN initial(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if ((v !=NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      poly p = (poly) u->Data();
      gfan::ZVector* weightVector;
      if (v->Typ() == INTVEC_CMD)
      {
        intvec* w0 = (intvec*) v->Data();
        bigintmat* w1 = iv2bim(w0,coeffs_BIGINT);
        w1->inpTranspose();
        weightVector = bigintmatToZVector(*w1);
        delete w1;
      }
      else
      {
        bigintmat* w1 = (bigintmat*) v->Data();
        weightVector = bigintmatToZVector(*w1);
      }
      res->rtyp = POLY_CMD;
      res->data = (void*) initial(p, currRing, *weightVector);
      delete weightVector;
      return FALSE;
    }
  }
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v !=NULL) && ((v->Typ() == BIGINTMAT_CMD) || (v->Typ() == INTVEC_CMD)))
    {
      try
      {
        ideal I = (ideal) u->Data();
        gfan::ZVector* weightVector;
        if (v->Typ() == INTVEC_CMD)
        {
          intvec* w0 = (intvec*) v->Data();
          bigintmat* w1 = iv2bim(w0,coeffs_BIGINT);
          w1->inpTranspose();
          weightVector = bigintmatToZVector(*w1);
          delete w1;
        }
        else
        {
          bigintmat* w1 = (bigintmat*) v->Data();
          weightVector = bigintmatToZVector(*w1);
        }
        res->rtyp = IDEAL_CMD;
        res->data = (void*) initial(I, currRing, *weightVector);
        delete weightVector;
        return FALSE;
      }
      catch (const std::exception& ex)
      {
        Werror("ERROR: %s",ex.what());
        return TRUE;
      }
    }
  }
  WerrorS("initial: unexpected parameters");
  return TRUE;
}


void tropical_setup(SModulFunctions* p)
{
  p->iiAddCproc("tropical.lib","groebnerCone",FALSE,groebnerCone);
  p->iiAddCproc("tropical.lib","maximalGroebnerCone",FALSE,maximalGroebnerCone);
  p->iiAddCproc("tropical.lib","homogeneitySpace",FALSE,homogeneitySpace);
  // p->iiAddCproc("","lowerHomogeneitySpace",FALSE,lowerHomogeneitySpace);
  p->iiAddCproc("tropical.lib","initial",FALSE,initial);
  p->iiAddCproc("tropical.lib","tropicalVariety",FALSE,tropicalVariety);
  p->iiAddCproc("tropical.lib","groebnerFan",FALSE,groebnerFan);
  p->iiAddCproc("tropical.lib","groebnerComplex",FALSE,groebnerComplex);
  // p->iiAddCproc("","ppreduceInitially",FALSE,ppreduceInitially);
  // p->iiAddCproc("","ttreduceInitially",FALSE,ttreduceInitially);
}
