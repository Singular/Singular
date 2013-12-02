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
    (void) pReduce(g,p);
    p_Delete(&g,currRing);
    omUpdateInfo();
    Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
    g = (poly) u->CopyD();
    (void) pReduce(g,p);
    n_Delete(&p,currRing->cf);
    res->rtyp = POLY_CMD;
    res->data = (char*) g;
    return FALSE;
  }
  return TRUE;
}
#endif //NDEBUG


/***
 * returns, if it exists, a pointer to the first term in g
 * that is divisible by (the leading monomial of) m or, if it does not exist, the NULL pointer.
 * if g is homogeneous in x with the same degree as m,
 * then it returns the first term with the same monomial in x as m,
 * if the t-degree of the term is higher than the t-degree of m, or NULL otherwise.
 **/
static inline poly firstTermDivisibleBy(const poly g, const poly m)
{
  poly gCache = NULL;
  for (gCache=g; gCache; pIter(gCache))
    if (p_LeadmonomDivisibleBy(m,gCache,currRing)) break;
  return gCache;
}


/***
 * reduces h initially with respect to g,
 * returns false if h was initially reduced in the first place,
 * returns true if reductions have taken place.
 * assumes that h and g are in pReduced form and homogeneous in x of the same degree
 **/
bool reduceInitially(poly &h, const poly g)
{
  poly hCache;
  for (hCache=h; hCache; pIter(hCache))
    if (p_LeadmonomDivisibleBy(g,hCache,currRing)) break;
  if (hCache)
  {
    number gAlpha = p_GetCoeff(g,currRing);
    poly hAlphaT = p_Init(currRing);
    p_SetCoeff(hAlphaT,n_Copy(p_GetCoeff(hCache,currRing),currRing->cf),currRing);
    p_SetExp(hAlphaT,1,p_GetExp(hCache,1,currRing)-p_GetExp(g,1,currRing),currRing);
    for (int i=2; i<=currRing->N; i++)
      p_SetExp(hAlphaT,i,0,currRing);
    p_Setm(hAlphaT,currRing); pTest(hAlphaT);
    h = p_Add_q(p_Mult_nn(h,gAlpha,currRing),
                p_Neg(p_Mult_q(p_Copy(g,currRing),hAlphaT,currRing),currRing),
                currRing);
    pTest(h);
    return true;
  }
  return false;
}


#ifndef NDEBUG
BOOLEAN reduceInitially0(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == POLY_CMD))
    {
      poly g,h;
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
      h = (poly) u->CopyD();
      g = (poly) v->CopyD();
      (void)reduceInitially(h,g);
      p_Delete(&h,currRing);
      p_Delete(&g,currRing);
      omUpdateInfo();
      Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
      h = (poly) u->CopyD();
      g = (poly) v->CopyD();
      (void)reduceInitially(h,g);
      p_Delete(&g,currRing);
      res->rtyp = POLY_CMD;
      res->data = (char*) h;
      return FALSE;
    }
  }
  return TRUE;
}
#endif //NDEBUG


static inline void sortByLeadmonom(ideal I)
{
  poly cache; int i, n=IDELEMS(I), newn;
  do
  {
    newn=0;
    for (i=1; i<n; i++)
    {
      if (pLmCmp(I->m[i-1],I->m[i])<0)
      {
        cache=I->m[i-1];
        I->m[i-1]=I->m[i];
        I->m[i]=cache;
        newn = i;
      }
    }
    n=newn;
  } while(n);
}


/***
 * reduces I initially with respect to itself and with respect to p-t.
 * also sorts the generators of I with respect to the leading monomials in descending order.
 * assumes that I is generated by elements which are homogeneous in x of the same degree.
 **/
bool reduceInitially(ideal I, const number p)
{
  poly cache; int i,j,n=IDELEMS(I);
  do
  {
    j=0;
    for (i=1; i<n; i++)
    {
      if (pLmCmp(I->m[i-1],I->m[i])<0)
      {
        cache=I->m[i-1];
        I->m[i-1]=I->m[i];
        I->m[i]=cache;
        j = i;
      }
    }
    n=j;
  } while(n);
  for (i=1; i<IDELEMS(I); i++)
    if (pReduce(I->m[i],p)) return true;

  /***
   * the first pass. removing terms with the same monomials in x as lt(g_i) out of g_j for i<j
   **/
  for (i=0; i<IDELEMS(I)-1; i++)
    for (j=i+1; j<IDELEMS(I); j++)
      if (reduceInitially(I->m[j], I->m[i]) && pReduce(I->m[j],p)) return true;

  /***
   * the second pass. removing terms divisible by lt(g_j) out of g_i for i<j
   **/
  for (i=0; i<IDELEMS(I)-1; i++)
    for (j=i+1; j<IDELEMS(I); j++)
      if (reduceInitially(I->m[i], I->m[j]) && pReduce(I->m[i],p)) return true;
  return false;
}


#ifndef NDEBUG
BOOLEAN reduceInitially1(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == NUMBER_CMD))
    {
      ideal I; number p;
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
      I = (ideal) u->CopyD();
      p = (number) v->CopyD();
      (void) reduceInitially(I,p);
      id_Delete(&I,currRing);
      n_Delete(&p,currRing->cf);
      omUpdateInfo();
      Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
      I = (ideal) u->CopyD();
      p = (number) v->CopyD();
      (void) reduceInitially(I,p);
      n_Delete(&p,currRing->cf);
      res->rtyp = IDEAL_CMD;
      res->data = (char*) I;
      return FALSE;
    }
  }
  return TRUE;
}
#endif //NDEBUG


/***
 * inserts g into I and reduces I with respect to itself and p-t
 * assumes that I was already sorted and initially reduced in the first place
 **/
bool reduceInitially(ideal I, const number p, poly g)
{
  pEnlargeSet(&(I->m),IDELEMS(I),1);
  IDELEMS(I)++; int i,j,k;
  for (j=IDELEMS(I)-1; j>0; j--)
  {
    if (pLmCmp(I->m[j-1],g)<0)
      I->m[j] = I->m[j-1];
    else
    {
      I->m[j] = g;
      break;
    }
  }
  if (j<1) I->m[0] = g;

  /***
   * the first pass. removing terms with the same monomials in x as lt(g_i) out of g_j for i<j
   * removing terms with the same monomials in x as lt(g_j) out of g_k for j<k
   **/
  for (i=0; i<j; i++)
    if (reduceInitially(I->m[j], I->m[i]) && pReduce(I->m[j],p)) return true;
  for (k=j+1; k<IDELEMS(I); k++)
    if (reduceInitially(I->m[k], I->m[j]) && pReduce(I->m[k],p)) return true;

  /***
   * the second pass. removing terms divisible by lt(g_j) and lt(g_k) out of g_i for i<j<k
   * removing terms divisible by lt(g_k) out of g_j for j<k
   **/
  for (i=0; i<j; i++)
    for (k=j; k<IDELEMS(I); k++)
      if (reduceInitially(I->m[i], I->m[k]) && pReduce(I->m[i],p)) return true;
  for (k=j+1; k<IDELEMS(I); k++)
    if (reduceInitially(I->m[j],I->m[k]) && pReduce(I->m[j],p)) return true;

  return false;
}


#ifndef NDEBUG
BOOLEAN reduceInitially2(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == NUMBER_CMD))
    {
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == POLY_CMD))
      {
        ideal I; number p; poly g;
        omUpdateInfo();
        Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
        I = (ideal) u->CopyD();
        p = (number) v->CopyD();
        g = (poly) w->CopyD();
        (void) reduceInitially(I,p,g);
        id_Delete(&I,currRing);
        n_Delete(&p,currRing->cf);
        omUpdateInfo();
        Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
        I = (ideal) u->CopyD();
        p = (number) v->CopyD();
        g = (poly) w->CopyD();
        (void) reduceInitially(I,p,g);
        n_Delete(&p,currRing->cf);
        res->rtyp = IDEAL_CMD;
        res->data = (char*) I;
        return FALSE;
      }
    }
  }
  return TRUE;
}
#endif //NDEBUG
