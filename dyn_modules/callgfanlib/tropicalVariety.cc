#include <callgfanlib_conversion.h>
#include <groebnerCone.h>
#include <tropicalVarietyOfPolynomials.h>
#include <tropicalVarietyOfIdeals.h>
#include <libpolys/coeffs/numbers.h>
#include <kernel/structs.h>

static poly adjustPoly(const poly g, const number p, const ring r)
{
  poly currentTerm = g;
  poly fBeginning = p_Head(currentTerm,r);
  poly f = fBeginning;
  number coeff0 = p_GetCoeff(currentTerm,r);
  unsigned power = 0;
  while (n_DivBy(coeff0,p,r->cf))
  {
    power++;
    number coeff1 = n_Div(coeff0,p,r->cf);
    n_Delete(&coeff0,r->cf);
    coeff0 = coeff1;
  }
  p_SetCoeff(f,coeff0,r);
  p_AddExp(f,1,power,r); p_Setm(f,r);

  for (pIter(currentTerm); currentTerm; pIter(currentTerm))
  {
    f->next = p_Head(currentTerm,r);
    pIter(f);
    coeff0 = p_GetCoeff(currentTerm,r);
    power = 0;
    while (n_DivBy(coeff0,p,r->cf))
    {
      power++;
      number coeff1 = n_Div(coeff0,p,r->cf);
      n_Delete(&coeff0,r->cf);
      coeff0 = coeff1;
    }
    p_SetCoeff(f,coeff0,r);
    p_AddExp(f,1,power,r); p_Setm(f,r);
  }
  return fBeginning;
}

static ideal adjustIdeal(const ideal I, const number p, const ring r)
{
  int k = idSize(I);
  ideal J = idInit(k+1);
  for (int i = 0; i<k; i++)
    J->m[i+1] = adjustPoly(I->m[i],p,r);
  poly h0 = p_One(r);
  p_SetCoeff(h0,n_Copy(p,r->cf),r);
  poly h1 = p_One(r);
  p_SetExp(h1,1,1,r);
  p_Setm(h1,r);
  J->m[0] = p_Add_q(h1,p_Neg(h0,r),r);
  return J;
}

BOOLEAN tropicalVariety(leftv res, leftv args)
{
  ring origin = currRing;
  ring r = rCopy(currRing);
  rChangeCurrRing(r);
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==POLY_CMD))
  {
    poly g = (poly) u->Data();
    leftv v = u->next;
    if (v==NULL)
    {
      std::set<gfan::ZCone> setOfCones = fullTropicalVarietyOfPolynomial(g,currRing);
      gfan::ZFan* tropG = toFanStar(setOfCones);
      res->rtyp = fanID;
      res->data = (char*) tropG;
      rChangeCurrRing(origin);
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      number p = (number) v->CopyD();
      poly f = adjustPoly(g,p,currRing);
      std::set<gfan::ZCone> setOfCones = lowerTropicalVarietyOfPolynomial(f,currRing);
      gfan::ZFan* tropG = toFanStar(setOfCones);
      res->rtyp = fanID;
      res->data = (char*) tropG;
      rChangeCurrRing(origin);
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==POLY_CMD))
    {
      std::set<gfan::ZCone> setOfCones = lowerTropicalVarietyOfPolynomial(g,currRing);
      gfan::ZFan* tropG = toFanStar(setOfCones);
      res->rtyp = fanID;
      res->data = (char*) tropG;
      rChangeCurrRing(origin);
      return FALSE;
    }
  }
  if ((u!=NULL) && (u->Typ()==IDEAL_CMD))
  {
    ideal I = (ideal) u->CopyD();
    leftv v = u->next;
    if (v==NULL)
    {
      setOfGroebnerConeData TropI = fullTropicalVariety(I,currRing);
      gfan::ZFan* tropI = toFanStar(TropI);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      rChangeCurrRing(origin);
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      number p = (number) v->CopyD();
      ideal J = adjustIdeal(I,p,currRing);
      setOfGroebnerConeData TropI = lowerTropicalVariety(J,currRing);
      gfan::ZFan* tropI = toFanStar(TropI);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      rChangeCurrRing(origin);
      return FALSE;
    }
    if ((v!=NULL) && (v->Typ()==POLY_CMD))
    {
      setOfGroebnerConeData TropI = lowerTropicalVariety(I,currRing);
      gfan::ZFan* tropI = toFanStar(TropI);
      res->rtyp = fanID;
      res->data = (char*) tropI;
      rChangeCurrRing(origin);
      return FALSE;
    }
    rChangeCurrRing(origin);
    return FALSE;
  }
  rChangeCurrRing(origin);
  WerrorS("tropicalVariety: unexpected parameters");
  return TRUE;
}
