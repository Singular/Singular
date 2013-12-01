#include <kernel/polys.h>
#include <libpolys/polys/monomials/p_polys.h>
#include <singularWishlist.h>

#include <Singular/ipid.h>

/***
 * changes a polynomial g with the help p-t such that
 * 1) each term of g has a distinct monomial in x
 * 2) no term of g has a coefficient divisible by p
 * in particular, this means that all g_\alpha can be obtained
 * by reading the coefficients and that g is initially reduced
 * with respect to p-t
 **/
static bool pReduce(poly g, const number p)
{
  poly toBeChecked = pNext(g);
  pNext(g) = NULL; poly gEnd = g;
  poly gCache;

  number coeff, pPower; int power; poly subst;
  while(toBeChecked)
  {
    for (gCache = g; gCache; pIter(gCache))
      if (p_LeadmonomDivisibleBy(gCache,toBeChecked,currRing)) break;
    if (gCache)
    {
      n_Power(p,p_GetExp(toBeChecked,1,currRing)-p_GetExp(gCache,1,currRing),&pPower,currRing->cf);
      coeff = n_Mult(p_GetCoeff(toBeChecked,currRing),pPower,currRing->cf);
      p_SetCoeff(gCache,n_Add(p_GetCoeff(gCache,currRing),coeff,currRing->cf),currRing);
      n_Delete(&pPower,currRing->cf); n_Delete(&coeff,currRing->cf);
      toBeChecked=p_LmDeleteAndNext(toBeChecked,currRing);
    }
    else
    {
      if (n_DivBy(p_GetCoeff(toBeChecked,currRing),p,currRing->cf))
      {
        coeff=n_Div(p_GetCoeff(toBeChecked,currRing),p,currRing->cf);
        power=1;
        while (n_DivBy(coeff,p,currRing->cf))
        {
          coeff=n_Div(p_GetCoeff(pNext(g),currRing),p,currRing->cf);
          power++;
          if (power<1)
          {
            WerrorS("pReduce: overflow in exponent");
            return true;
          }
        }
        subst=p_LmInit(toBeChecked,currRing);
        p_AddExp(subst,1,power,currRing);
        p_SetCoeff(subst,coeff,currRing);
        p_Setm(subst,currRing); pTest(subst);
        toBeChecked=p_LmDeleteAndNext(toBeChecked,currRing);
        toBeChecked=p_Add_q(toBeChecked,subst,currRing);
        pTest(toBeChecked);
      }
      else
      {
        pNext(gEnd)=toBeChecked;
        pIter(gEnd); pIter(toBeChecked);
        pNext(gEnd)=NULL;
      }
    }
  }
  return false;
}

#ifndef NDEBUG
BOOLEAN pReduce(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    poly g; number p = n_Init(3,currRing->cf);
    omUpdateInfo();
    Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
    g = (poly) u->CopyD();
    pReduce(g,p);
    p_Delete(&g,currRing);
    omUpdateInfo();
    Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
    g = (poly) u->CopyD();
    pReduce(g,p);
    n_Delete(&p,currRing->cf);
    res->rtyp = POLY_CMD;
    res->data = (char*) g;
    return FALSE;
  }
  return TRUE;
}
#endif //NDEBUG



