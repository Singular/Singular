#include <Singular/ipid.h>

#include <libpolys/polys/monomials/p_polys.h>
#include <libpolys/polys/clapsing.h>
#include <singularWishlist.h>

#include <map>
#include <set>

#define KEEP_COEFF_SMALL 0

/***
 * if KEEP_COEFF_SMALL is set, alters all coefficients of a polynomial g
 * to be in range between 0 and p-1 with the help of p-t.
 * In particular the result will be reduced with respect to p-t.
 * if KEEP_COEFF_SMALL is not set, reduces g initially with respect to p-t.
 **/
static bool pReduce(poly g, const number p)
{
  poly toBeChecked = pNext(g);
  pNext(g) = NULL; poly gEnd = g;
  poly gCache;

  number coeff; int power; poly subst;
  while(toBeChecked)
  {
#if KEEP_COEFF_SMALL
    if (n_Greater(p_GetCoeff(toBeChecked,currRing),p,currRing->cf) ||
        n_Equal(p_GetCoeff(toBeChecked,currRing),p,currRing->cf))
    {
      coeff = n_IntDiv(p_GetCoeff(toBeChecked,currRing),p,currRing->cf);
      p_SetCoeff(toBeChecked,
                 n_IntMod(p_GetCoeff(toBeChecked,currRing),p,currRing->cf),
                 currRing);
      subst = p_LmInit(toBeChecked,currRing);
      p_AddExp(subst,1,1,currRing);
      p_SetCoeff(subst,coeff,currRing);
      p_Setm(subst,currRing); pTest(subst);
      pNext(toBeChecked)=p_Add_q(pNext(toBeChecked),subst,currRing);
      pTest(toBeChecked);
    }
    else
    {
      number negcoeff = n_Neg(p_GetCoeff(toBeChecked,currRing),currRing->cf);
      if (n_GreaterZero(negcoeff,currRing->cf))
      {
        number coeffdiv = n_IntDiv(p_GetCoeff(toBeChecked,currRing),p,currRing->cf);
        number coeffmod = n_IntMod(p_GetCoeff(toBeChecked,currRing),p,currRing->cf);
        if (n_IsZero(coeffmod,currRing->cf))
        {
          poly subst = p_LmInit(toBeChecked,currRing);
        }
      }
      n_Delete(negcoeff,currRing->cf);
    }
#else
    for (gCache = g; gCache; pIter(gCache))
      if (p_LeadmonomDivisibleBy(gCache,toBeChecked,currRing)) break;
    if (!gCache)
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
    else
    {
      pNext(gEnd)=toBeChecked;
      pIter(gEnd); pIter(toBeChecked);
      pNext(gEnd)=NULL;
    }
  }
#endif
  return false;
}


#ifndef NDEBUG
BOOLEAN ttpReduce(leftv res, leftv args)
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
 * reduces h initially with respect to g,
 * returns false if h was initially reduced in the first place,
 * returns true if reductions have taken place.
 * assumes that h and g are in pReduced form and homogeneous in x of the same degree
 **/
bool ttreduceInitially(poly &h, const poly g)
{
  pTest(h); pTest(g); poly hCache;
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
    poly q1 = p_Mult_nn(h,gAlpha,currRing); pTest(q1);
    poly q2 = p_Mult_q(p_Copy(g,currRing),hAlphaT,currRing); pTest(q2);
    q2 = p_Neg(q2,currRing); pTest(q2);
    h = p_Add_q(q1,q2,currRing);
    pTest(h);
    return true;
  }
  return false;
}


#ifndef NDEBUG
BOOLEAN ttreduceInitially0(leftv res, leftv args)
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
      (void)ttreduceInitially(h,g);
      p_Delete(&h,currRing);
      p_Delete(&g,currRing);
      omUpdateInfo();
      Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
      h = (poly) u->CopyD();
      g = (poly) v->CopyD();
      (void)ttreduceInitially(h,g);
      p_Delete(&g,currRing);
      res->rtyp = POLY_CMD;
      res->data = (char*) h;
      return FALSE;
    }
  }
  return TRUE;
}
#endif //NDEBUG


/***
 * reduces I initially with respect to itself and with respect to p-t.
 * also sorts the generators of I with respect to the leading monomials in descending order.
 * assumes that I is generated by elements which are homogeneous in x of the same degree.
 **/
bool ttreduceInitially(ideal I, const number p)
{
  int m=idSize(I),n=m; poly cache;
  do
  {
    int j=0;
    for (int i=1; i<n; i++)
    {
      if (p_LmCmp(I->m[i-1],I->m[i],currRing)<0)
      {
        cache=I->m[i-1];
        I->m[i-1]=I->m[i];
        I->m[i]=cache;
        j = i;
      }
    }
    n=j;
  } while(n);
  for (int i=1; i<m; i++)
    if (pReduce(I->m[i],p)) return true;

  /***
   * the first pass. removing terms with the same monomials in x as lt(g_i) out of g_j for i<j
   **/
  for (int i=0; i<m-1; i++)
    for (int j=i+1; j<m; j++)
      if (ttreduceInitially(I->m[j], I->m[i]) && pReduce(I->m[j],p)) return true;

  /***
   * the second pass. removing terms divisible by lt(g_j) out of g_i for i<j
   **/
  for (int i=0; i<m-1; i++)
    for (int j=i+1; j<m; j++)
      if (ttreduceInitially(I->m[i], I->m[j]) && pReduce(I->m[i],p)) return true;
  return false;
}


#ifndef NDEBUG
BOOLEAN ttreduceInitially1(leftv res, leftv args)
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
      (void) ttreduceInitially(I,p);
      id_Delete(&I,currRing);
      n_Delete(&p,currRing->cf);
      omUpdateInfo();
      Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
      I = (ideal) u->CopyD();
      p = (number) v->CopyD();
      (void) ttreduceInitially(I,p);
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
bool ttreduceInitially(ideal I, const number p, const poly g)
{
  int n=idSize(I);
  idInsertPoly(I,g);
  poly cache; n++; int j;
  for (j=n-1; j>0; j--)
  {
    if (p_LmCmp(I->m[j], I->m[j-1],currRing)>0)
    {
      cache = I->m[j];
      I->m[j] = I->m[j-1];
      I->m[j-1] = cache;
    }
    else
      break;
  }

  /***
   * the first pass. removing terms with the same monomials in x as lt(g_i) out of g_j for i<j
   * removing terms with the same monomials in x as lt(g_j) out of g_k for j<k
   **/
  for (int i=0; i<j; i++)
    if (ttreduceInitially(I->m[j], I->m[i]) && pReduce(I->m[j],p)) return true;
  for (int k=j+1; k<n; k++)
    if (ttreduceInitially(I->m[k], I->m[j]) && pReduce(I->m[k],p)) return true;

  /***
   * the second pass. removing terms divisible by lt(g_j) and lt(g_k) out of g_i for i<j<k
   * removing terms divisible by lt(g_k) out of g_j for j<k
   **/
  for (int i=0; i<j; i++)
    for (int k=j; k<n; k++)
      if (ttreduceInitially(I->m[i], I->m[k]) && pReduce(I->m[i],p)) return true;
  for (int k=j+1; k<n; k++)
    if (ttreduceInitially(I->m[j],I->m[k]) && pReduce(I->m[j],p)) return true;

  return false;
}


#ifndef NDEBUG
BOOLEAN ttreduceInitially2(leftv res, leftv args)
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
        (void) ttreduceInitially(I,p,g);
        id_Delete(&I,currRing);
        n_Delete(&p,currRing->cf);
        omUpdateInfo();
        Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
        I = (ideal) u->CopyD();
        p = (number) v->CopyD();
        g = (poly) w->CopyD();
        (void) ttreduceInitially(I,p,g);
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


/***
 * reduces H initially with respect to itself, with respect to p-t,
 * and with respect to G.
 * assumes that the generators of H are homogeneous in x of the same degree,
 * assumes that the generators of G are homogeneous in x of lesser degree.
 **/
bool ttreduceInitially(ideal H, const number p, const ideal G)
{
  /***
   * Step 1: reduce H initially with respect to itself and with respect to p-t
   **/
  if (ttreduceInitially(H,p)) return true;

  /***
   * Step 2: initialize a working list T and an ideal I in which the reductions will take place
   **/
  int m=idSize(H),n=0;
  ideal I = idInit(m), T = idInit(m);
  for (int i=0; i<m; i++)
  {
    I->m[i]=H->m[i];
    if (pNext(H->m[i])) T->m[n++]=H->m[i];
  }
  poly g; int k=n;
  do
  {
    int j=0;
    for (int i=1; i<k; i++)
    {
      if (p_LmCmp(pNext(T->m[i-1]),pNext(T->m[i]),currRing)<0)
      {
        g=T->m[i-1];
        T->m[i-1]=I->m[i];
        T->m[i]=g;
        j = i;
      }
    }
    k=j;
  } while(k);

  /***
   * Step 3: as long as the working list is not empty, successively reduce terms in it
   *   by adding suitable elements to I and reducing it initially with respect to itself
   **/
  k=idSize(G);
  while (n)
  {
    int i=0; for (; i<k; i++)
      if (p_LeadmonomDivisibleBy(G->m[i],pNext(T->m[0]),currRing)) break;
    if (i<k)
    {
      g = p_Init(currRing);
      for (int j=2; j<=currRing->N; j++)
        p_SetExp(g,j,p_GetExp(pNext(T->m[0]),j,currRing)-p_GetExp(G->m[i],j,currRing),currRing);
      p_SetCoeff(g,n_Init(1,currRing->cf),currRing); p_Setm(g,currRing);
      g = p_Mult_q(g,p_Copy(G->m[i],currRing),currRing);
      ttreduceInitially(I,p,g);
    }
    else
      pIter(T->m[0]);
    for (int i=0; i<n;)
    {
      if (!pNext(T->m[i]))
      {
        for (int j=i; j<n-1; j++)
          T->m[j]=T->m[j+1];
        T->m[--n]=NULL;
      }
      else
        i++;
    }
    int l = n;
    do
    {
      int j=0;
      for (int i=1; i<l; i++)
      {
        if (p_LmCmp(pNext(T->m[i-1]),pNext(T->m[i]),currRing)<0)
        {
          g=T->m[i-1];
          T->m[i-1]=I->m[i];
          T->m[i]=g;
          j = i;
        }
      }
      l=j;
    } while(l);
  }

  /***
   * Step 4: cleanup, delete all polynomials in I which have been added in Step 3
   **/
  k=idSize(I);
  for (int i=0; i<k; i++)
  {
    for (int j=0; j<m; j++)
    {
      if (p_LeadmonomDivisibleBy(H->m[j],I->m[i],currRing))
      {
        I->m[i]=NULL;
        break;
      }
    }
  }
  id_Delete(&I,currRing);
  id_Delete(&T,currRing);
  return false;
}


#ifndef NDEBUG
BOOLEAN ttreduceInitially3(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == NUMBER_CMD))
    {
      leftv w = v->next;
      if ((w != NULL) && (w->Typ() == IDEAL_CMD))
      {
        ideal H,G; number p;
        omUpdateInfo();
        Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
        H = (ideal) u->CopyD();
        p = (number) v->CopyD();
        G = (ideal) w->CopyD();
        (void) ttreduceInitially(H,p,G);
        id_Delete(&H,currRing);
        id_Delete(&G,currRing);
        n_Delete(&p,currRing->cf);
        omUpdateInfo();
        Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
        H = (ideal) u->CopyD();
        p = (number) v->CopyD();
        G = (ideal) w->CopyD();
        (void) ttreduceInitially(H,p,G);
        n_Delete(&p,currRing->cf);
        id_Delete(&G,currRing);
        res->rtyp = IDEAL_CMD;
        res->data = (char*) H;
        return FALSE;
      }
    }
  }
  return TRUE;
}
#endif //NDEBUG


/***
 * reduces I initially with respect to itself.
 * assumes that the generators of I are homogeneous in x and that p-t is in I.
 **/
bool ttreduceInitially(ideal I)
{
  /***
   * Step 1: split up I into components of same degree in x
   *  the lowest component should only contain p-t
   **/
  std::map<long,ideal> H; int n = idSize(I);
  for (int i=0; i<n; i++)
  {
    long d = 0;
    for (int j=2; j<=currRing->N; j++)
      d += p_GetExp(I->m[i],j,currRing);
    std::map<long,ideal>::iterator it = H.find(d);
    if (it != H.end())
      idInsertPoly(it->second,I->m[i]);
    else
    {
      std::pair<std::map<long,ideal>::iterator,bool> ret;
      ret = H.insert(std::pair<long,ideal>(d,idInit(16)));
      idInsertPoly(ret.first->second,I->m[i]);
    }
  }

  std::map<long,ideal>::iterator it=H.begin();
  ideal Hi = it->second;
  assume(idSize(Hi)==1);
  assume(pLength(Hi->m[0])==2 && p_GetExp(Hi->m[0],1,currRing)==0
           && p_GetExp(Hi->m[0]->next,1,currRing)==1);
  number p = p_GetCoeff(Hi->m[0],currRing);
  assume(!n_IsUnit(p,currRing->cf));
  idShallowDelete(&it->second);

  /***
   * Step 2: reduce each component initially with respect to itself
   *  and all lower components
   **/
  it++; Hi = it->second; n--;
  if (ttreduceInitially(Hi,p)) return true;

  ideal G = idInit(n); int m=0;
  ideal GG = (ideal) omAllocBin(sip_sideal_bin);
  GG->nrows = 1; GG->rank = 1; GG->m=NULL;

  for (it++; it!=H.end(); it++)
  {
    int l=idSize(Hi); int k=l; poly cache;
    do
    {
      int j=0;
      for (int i=1; i<k; i++)
      {
        if (p_GetExp(Hi->m[i-1],1,currRing)<p_GetExp(Hi->m[i],1,currRing))
        {
          cache=Hi->m[i-1];
          Hi->m[i-1]=Hi->m[i];
          Hi->m[i]=cache;
          j = i;
        }
      }
      k=j;
    } while(k);
    int kG=n-m, kH=0;
    for (int i=n-m-l; i<n; i++)
    {
      if (kG==n)
      {
        memcpy(&(G->m[i]),&(Hi->m[kH]),(n-i)*sizeof(poly));
        break;
      }
      if (p_GetExp(G->m[kG],1,currRing)>p_GetExp(Hi->m[kH],1,currRing))
        G->m[i] = G->m[kG++];
      else
        G->m[i] = Hi->m[kH++];
    }
    m += l; IDELEMS(GG) = m; GG->m = &G->m[n-m];
    if (ttreduceInitially(it->second,p,GG)) return true;
    idShallowDelete(&Hi); Hi = it->second;
  }
  idShallowDelete(&Hi);

  omFreeBin((ADDRESS)GG, sip_sideal_bin); idShallowDelete(&G);
  return false;
}


#ifndef NDEBUG
BOOLEAN ttreduceInitially4(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    ideal I;
    omUpdateInfo();
    Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
    I = (ideal) u->CopyD();
    (void) ttreduceInitially(I);
    id_Delete(&I,currRing);
    omUpdateInfo();
    Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
    I = (ideal) u->CopyD();
    (void) ttreduceInitially(I);
    res->rtyp = IDEAL_CMD;
    res->data = (char*) I;
    return FALSE;
  }
  return TRUE;
}
#endif


BOOLEAN ttreduceInitially(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    ideal I = (ideal) u->CopyD();
    (void) ttreduceInitially(I);
    res->rtyp = IDEAL_CMD;
    res->data = (char*) I;
    return FALSE;
  }
  return TRUE;
}