#include <polys/monomials/p_polys.h>
#include <Singular/ipid.h>

#include "singularWishlist.h"
#include "ppinitialReduction.h"

#include <map>
#include <set>
#include <exception>


#ifndef NDEBUG
bool isOrderingLocalInT(const ring r)
{
  poly one = p_One(r);
  poly t = p_One(r);
  p_SetExp(t,1,1,r);
  p_Setm(t,r);
  int s = p_LmCmp(one,t,r);
  p_Delete(&one,r);
  p_Delete(&t,r);
  return (s==1);
}
#endif

void divideByCommonGcd(poly &g, const ring r)
{
  number commonGcd = n_Copy(p_GetCoeff(g,r),r->cf);
  for (poly gCache=pNext(g); gCache; pIter(gCache))
  {
    number commonGcdCache = n_Gcd(commonGcd,p_GetCoeff(gCache,r),r->cf);
    n_Delete(&commonGcd,r->cf);
    commonGcd = commonGcdCache;
    if (n_IsOne(commonGcd,r->cf))
    {
      n_Delete(&commonGcd,r->cf);
      return;
    }
  }
  for (poly gCache=g; gCache; pIter(gCache))
  {
    number oldCoeff = p_GetCoeff(gCache,r);
    number newCoeff = n_Div(oldCoeff,commonGcd,r->cf);
    p_SetCoeff(gCache,newCoeff,r);
  }
  p_Test(g,r);
  n_Delete(&commonGcd,r->cf);
  return;
}

/***
 * changes a polynomial g with the help p-t such that
 * 1) each term of g has a distinct monomial in x
 * 2) no term of g has a coefficient divisible by p
 * in particular, this means that all g_\alpha can be obtained
 * by reading the coefficients and that g is initially reduced
 * with respect to p-t
 **/
void pReduce(poly &g, const number p, const ring r)
{
  if (g==NULL)
    return;
  p_Test(g,r);

  poly toBeChecked = pNext(g);
  pNext(g) = NULL; poly gEnd = g;
  poly gCache;

  number coeff, pPower; int power; poly subst;
  while(toBeChecked)
  {
    for (gCache = g; gCache; pIter(gCache))
      if (p_LeadmonomDivisibleBy(gCache,toBeChecked,r)) break;
    if (gCache)
    {
      n_Power(p,p_GetExp(toBeChecked,1,r)-p_GetExp(gCache,1,r),&pPower,r->cf);
      coeff = n_Mult(p_GetCoeff(toBeChecked,r),pPower,r->cf);
      p_SetCoeff(gCache,n_Add(p_GetCoeff(gCache,r),coeff,r->cf),r);
      n_Delete(&pPower,r->cf); n_Delete(&coeff,r->cf);
      toBeChecked=p_LmDeleteAndNext(toBeChecked,r);
    }
    else
    {
      if (n_DivBy(p_GetCoeff(toBeChecked,r),p,r->cf))
      {
        power=1;
        coeff=n_Div(p_GetCoeff(toBeChecked,r),p,r->cf);
        while (n_DivBy(coeff,p,r->cf))
        {
          power++;
          number coeff0 = n_Div(coeff,p,r->cf);
          n_Delete(&coeff,r->cf);
          coeff = coeff0;
          coeff0 = NULL;
          if (power<1)
          {
            WerrorS("pReduce: overflow in exponent");
            throw 0;
          }
        }
        subst=p_LmInit(toBeChecked,r);
        p_AddExp(subst,1,power,r);
        p_SetCoeff(subst,coeff,r);
        p_Setm(subst,r); p_Test(subst,r);
        toBeChecked=p_LmDeleteAndNext(toBeChecked,r);
        toBeChecked=p_Add_q(toBeChecked,subst,r);
        p_Test(toBeChecked,r);
      }
      else
      {
        pNext(gEnd)=toBeChecked;
        pIter(gEnd); pIter(toBeChecked);
        pNext(gEnd)=NULL;
        p_Test(g,r);
      }
    }
  }
  p_Test(g,r);
  divideByCommonGcd(g,r);
  return;
}

void ptNormalize(poly* gStar, const number p, const ring r)
{
  poly g = *gStar;
  if (g==NULL || n_DivBy(p_GetCoeff(g,r),p,r->cf))
    return;
  p_Test(g,r);

  // create p-t
  poly pt = p_Init(r);
  p_SetCoeff(pt,n_Copy(p,r->cf),r);

  pNext(pt) = p_Init(r);
  p_SetExp(pNext(pt),1,1,r);
  p_Setm(pNext(pt),r);
  p_SetCoeff(pNext(pt),n_Init(-1,r->cf),r);

  // make g monic with the help of p-t
  number a,b;
  number gcd = n_ExtGcd(p_GetCoeff(g,r),p,&a,&b,r->cf);
  assume(n_IsUnit(gcd,r->cf));
  // now a*leadcoef(g)+b*p = gcd with gcd being a unit
  // so a*g+b*(p-t)*leadmonom(g) should have a unit as leading coefficient
  // but first check whether b is 0,
  // since p_Mult_nn doesn't allow 0 as number input
  if (n_IsZero(b,r->cf))
  {
    n_Delete(&a,r->cf);
    n_Delete(&b,r->cf);
    n_Delete(&gcd,r->cf);
    p_Delete(&pt,r);
    return;
  }
  poly m = p_Head(g,r);
  p_SetCoeff(m,n_Init(1,r->cf),r);
  g = p_Add_q(p_Mult_nn(g,a,r),p_Mult_nn(p_Mult_mm(pt,m,r),b,r),r);
  n_Delete(&a,r->cf);
  n_Delete(&b,r->cf);
  n_Delete(&gcd,r->cf);
  p_Delete(&m,r);

  p_Test(g,r);
  return;
}

void ptNormalize(ideal I, const number p, const ring r)
{
  for (int i=0; i<idSize(I); i++)
    ptNormalize(&(I->m[i]),p,r);
  return;
}

#ifndef NDEBUG
BOOLEAN ptNormalize(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v!=NULL) && (v->Typ()==NUMBER_CMD))
    {
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
      ideal I = (ideal) u->CopyD();
      number p = (number) v->CopyD();
      ptNormalize(I,p,currRing);
      n_Delete(&p,currRing->cf);
      res->rtyp = IDEAL_CMD;
      res->data = (char*) I;
      return FALSE;
    }
  }
  return TRUE;
}
#endif //NDEBUG

#ifndef NDEBUG
BOOLEAN pReduceDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == POLY_CMD))
  {
    poly g; number p = n_Init(3,currRing->cf);
    omUpdateInfo();
    Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
    g = (poly) u->CopyD();
    (void) pReduce(g,p,currRing);
    p_Delete(&g,currRing);
    omUpdateInfo();
    Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
    g = (poly) u->CopyD();
    (void) pReduce(g,p,currRing);
    n_Delete(&p,currRing->cf);
    res->rtyp = POLY_CMD;
    res->data = (char*) g;
    return FALSE;
  }
  return TRUE;
}
#endif //NDEBUG

void pReduce(ideal &I, const number p, const ring r)
{
  int k = idSize(I);
  for (int i=0; i<k; i++)
  {
    if (I->m[i]!=NULL)
    {
      number c = p_GetCoeff(I->m[i],r);
      if (!n_Equal(p,c,r->cf))
        pReduce(I->m[i],p,r);
    }
  }
  return;
}


/**
 * reduces h initially with respect to g,
 * returns false if h was initially reduced in the first place,
 * returns true if reductions have taken place.
 * assumes that h and g are in pReduced form and homogeneous in x of the same degree
 */
bool ppreduceInitially(poly* hStar, const poly g, const ring r)
{
  poly h = *hStar;
  if (h==NULL || g==NULL)
    return false;
  p_Test(h,r);
  p_Test(g,r);
  poly hCache;
  for (hCache=h; hCache; pIter(hCache))
    if (p_LeadmonomDivisibleBy(g,hCache,r)) break;
  if (hCache)
  {
    number gAlpha = p_GetCoeff(g,r);
    poly hAlphaT = p_Init(r);
    p_SetCoeff(hAlphaT,n_Copy(p_GetCoeff(hCache,r),r->cf),r);
    p_SetExp(hAlphaT,1,p_GetExp(hCache,1,r)-p_GetExp(g,1,r),r);
    for (int i=2; i<=r->N; i++)
      p_SetExp(hAlphaT,i,0,r);
    p_Setm(hAlphaT,r); p_Test(hAlphaT,r);
    poly q1 = p_Mult_nn(h,gAlpha,r); p_Test(q1,r);
    poly q2 = p_Mult_q(p_Copy(g,r),hAlphaT,r); p_Test(q2,r);
    q2 = p_Neg(q2,r); p_Test(q2,r);
    h = p_Add_q(q1,q2,r);
    p_Test(h,r);
    p_Test(g,r);
    *hStar = h;
    return true;
  }
  p_Test(h,r);
  p_Test(g,r);
  return false;
}


#ifndef NDEBUG
BOOLEAN ppreduceInitially0(leftv res, leftv args)
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
      (void)ppreduceInitially(&h,g,currRing);
      p_Delete(&h,currRing);
      p_Delete(&g,currRing);
      omUpdateInfo();
      Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
      h = (poly) u->CopyD();
      g = (poly) v->CopyD();
      (void)ppreduceInitially(&h,g,currRing);
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
bool ppreduceInitially(ideal I, const number p, const ring r)
{
  int m=idSize(I),n=m; poly cache;
  do
  {
    int j=0;
    for (int i=1; i<n; i++)
    {
      if (p_LmCmp(I->m[i-1],I->m[i],r)<0)
      {
        cache=I->m[i-1];
        I->m[i-1]=I->m[i];
        I->m[i]=cache;
        j = i;
      }
    }
    n=j;
  } while(n);
  for (int i=0; i<m; i++)
    pReduce(I->m[i],p,r);

  /***
   * the first pass. removing terms with the same monomials in x as lt(g_i) out of g_j for i<j
   **/
  for (int i=0; i<m-1; i++)
    for (int j=i+1; j<m; j++)
      if (ppreduceInitially(&I->m[j], I->m[i], r))
        pReduce(I->m[j],p,r);

  /***
   * the second pass. removing terms divisible by lt(g_j) out of g_i for i<j
   **/
  for (int i=0; i<m-1; i++)
    for (int j=i+1; j<m; j++)
      if (ppreduceInitially(&I->m[i], I->m[j],r))
        pReduce(I->m[i],p,r);

  /***
   * removes the elements of I which have been reduced to 0 in the previous two passes
   **/
  idSkipZeroes(I);
  return false;
}


#ifndef NDEBUG
BOOLEAN ppreduceInitially1(leftv res, leftv args)
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
      (void) ppreduceInitially(I,p,currRing);
      id_Delete(&I,currRing);
      n_Delete(&p,currRing->cf);
      omUpdateInfo();
      Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
      I = (ideal) u->CopyD();
      p = (number) v->CopyD();
      (void) ppreduceInitially(I,p,currRing);
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
 * returns the position in I in which g was inserted
 * assumes that I was already sorted and initially reduced in the first place
 **/
int ppreduceInitially(ideal I, const number p, const poly g, const ring r)
{
  id_Test(I,r);
  p_Test(g,r);
  idInsertPoly(I,g);
  int n=idSize(I);
  int j;
  for (j=n-1; j>0; j--)
  {
    if (p_LmCmp(I->m[j], I->m[j-1],r)>0)
    {
      poly cache = I->m[j];
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
    if (ppreduceInitially(&I->m[j], I->m[i], r))
      pReduce(I->m[j],p,r);
  for (int k=j+1; k<n; k++)
    if (ppreduceInitially(&I->m[k], I->m[j], r))
    {
      pReduce(I->m[k],p,r);
      for (int l=j+1; l<k; l++)
        if (ppreduceInitially(&I->m[k], I->m[l], r))
          pReduce(I->m[k],p,r);
    }

  /***
   * the second pass. removing terms divisible by lt(g_j) and lt(g_k) out of g_i for i<j<k
   * removing terms divisible by lt(g_k) out of g_j for j<k
   **/
  for (int i=0; i<j; i++)
    for (int k=j; k<n; k++)
      if (ppreduceInitially(&I->m[i], I->m[k], r))
        pReduce(I->m[i],p,r);
  for (int k=j; k<n-1; k++)
    for (int l=k+1; l<n; l++)
      if (ppreduceInitially(&I->m[k], I->m[l], r))
        pReduce(I->m[k],p,r);

  /***
   * removes the elements of I which have been reduced to 0 in the previous two passes
   **/
  idSkipZeroes(I);
  id_Test(I,r);
  return j;
}


#ifndef NDEBUG
BOOLEAN ppreduceInitially2(leftv res, leftv args)
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
        (void) ppreduceInitially(I,p,g,currRing);
        id_Delete(&I,currRing);
        n_Delete(&p,currRing->cf);
        omUpdateInfo();
        Print("usedBytesAfter=%ld\n",om_Info.UsedBytes);
        I = (ideal) u->CopyD();
        p = (number) v->CopyD();
        g = (poly) w->CopyD();
        (void) ppreduceInitially(I,p,g,currRing);
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


static poly ppNext(poly p, int l)
{
  poly q = p;
  for (int i=0; i<l; i++)
  {
    if (q==NULL)
      break;
    pIter(q);
  }
  return q;
}


static void sortMarks(const ideal H, const ring r, std::vector<mark> &T)
{
  std::pair<int,int> pointerToTerm;
  int k=T.size();
  do
  {
    int j=0;
    for (int i=1; i<k-1; i++)
    {
      int generatorA = T[i-1].first;
      int termA = T[i-1].second;
      int generatorB = T[i].first;
      int termB = T[i].second;
      if (p_LmCmp(ppNext(H->m[generatorA],termA),ppNext(H->m[generatorB],termB),r)<0)
      {
        mark cache=T[i-1];
        T[i-1]=T[i];
        T[i]=cache;
        j = i;
      }
    }
    k=j;
  } while(k);
  return;
}


static poly getTerm(const ideal H, const mark ab)
{
  int a = ab.first;
  int b = ab.second;
  return ppNext(H->m[a],b);
}


static void adjustMarks(std::vector<mark> &T, const int newEntry)
{
  for (unsigned i=0; i<T.size(); i++)
  {
    if (T[i].first>=newEntry)
      T[i].first = T[i].first+1;
  }
  return;
}


static void cleanupMarks(const ideal H, std::vector<mark> &T)
{
  for (unsigned i=0; i<T.size();)
  {
    if (getTerm(H,T[i])==NULL)
      T.erase(T.begin()+i);
    else
      i++;
  }
  return;
}


/***
 * reduces H initially with respect to itself, with respect to p-t,
 * and with respect to G.
 * assumes that the generators of H are homogeneous in x of the same degree,
 * assumes that the generators of G are homogeneous in x of lesser degree.
 **/
bool ppreduceInitially(ideal &H, const number p, const ideal G, const ring r)
{
  /***
   * Step 1: reduce H initially with respect to itself and with respect to p-t
   **/
  if (ppreduceInitially(H,p,r)) return true;

  /***
   * Step 2: initialize an ideal I in which the reductions will take place-
   *   along the reduction it will be enlarged with elements that will be discarded at the end
   *   initialize a working list T which keeps track.
   *   the working list T is a vector of pairs of integer.
   *   if T contains a pair (i,j) then that means that in the i-th element of H
   *   term j and subsequent terms need to be checked for reduction.
   *   T is sorted by the ordering on the temrs the pairs correspond to.
   **/
  int m=idSize(H);
  ideal I = idInit(m);
  std::vector<mark> T;
  for (int i=0; i<m; i++)
  {
    I->m[i]=H->m[i];
    if (pNext(I->m[i])!=NULL)
      T.push_back(std::make_pair<int,int>(i,1));
  }

  /***
   * Step 3: as long as the working list is not empty, successively reduce terms in it
   *   by adding suitable elements to I and reducing it initially with respect to itself
   **/
  int k=idSize(G);
  while (T.size()>0)
  {
    sortMarks(I,r,T);
    int i=0; for (; i<k; i++)
      if (p_LeadmonomDivisibleBy(G->m[i],getTerm(I,T[0]),r)) break;
    if (i<k)
    {
      poly g = p_One(r); poly h0 = getTerm(I,T[0]);
      assume(h0!=NULL);
      for (int j=2; j<=r->N; j++)
        p_SetExp(g,j,p_GetExp(h0,j,r)-p_GetExp(G->m[i],j,r),r);
      p_Setm(g,r);
      g = p_Mult_q(g,p_Copy(G->m[i],r),r);
      int newEntry = ppreduceInitially(I,p,g,r);
      adjustMarks(T,newEntry);
    }
    else
      T[0].second = T[0].second+1;
    cleanupMarks(I,T);
  }

  /***
   * Step 4: cleanup, delete all polynomials in I which have been added in Step 3
   **/
  k=idSize(I);
  for (int i=0; i<k; i++)
  {
    for (int j=0; j<m; j++)
    {
      if (p_LeadmonomDivisibleBy(H->m[j],I->m[i],r))
      {
        I->m[i]=NULL;
        break;
      }
    }
  }
  id_Delete(&I,r);
  return false;
}


#ifndef NDEBUG
BOOLEAN ppreduceInitially3(leftv res, leftv args)
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
        (void) ppreduceInitially(H,p,G,currRing);
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

/**
 * reduces I initially with respect to itself.
 * assumes that the generators of I are homogeneous in x and that p-t is in I.
 */
bool ppreduceInitially(ideal I, const ring r, const number p)
{
  assume(!n_IsUnit(p,r->cf));

  /***
   * Step 1: split up I into components of same degree in x
   *  the lowest component should only contain p-t
   **/
  std::map<long,ideal> H; int n = idSize(I);
  for (int i=0; i<n; i++)
  {
    I->m[i] = p_Cleardenom(I->m[i],r);
    long d = 0;
    for (int j=2; j<=r->N; j++)
      d += p_GetExp(I->m[i],j,r);
    std::map<long,ideal>::iterator it = H.find(d);
    if (it != H.end())
      idInsertPoly(it->second,I->m[i]);
    else
    {
      std::pair<long,ideal> Hd(d,idInit(1));
      Hd.second->m[0] = I->m[i];
      H.insert(Hd);
    }
  }

  std::map<long,ideal>::iterator it=H.begin();
  ideal Hi = it->second;
  idShallowDelete(&Hi);
  it++;
  Hi = it->second;

  /***
   * Step 2: reduce each component initially with respect to itself
   *  and all lower components
   **/
  if (ppreduceInitially(Hi,p,r)) return true;
  id_Test(Hi,r);
  id_Test(I,r);

  ideal G = idInit(n); int m=0;
  ideal GG = (ideal) omAllocBin(sip_sideal_bin);
  GG->nrows = 1; GG->rank = 1; GG->m=NULL;

  for (it++; it!=H.end(); it++)
  {
    int l=idSize(Hi); int k=l; poly cache;
    /**
     * sorts Hi according to degree in t in descending order
     * (lowest first, highest last)
     */
    do
    {
      int j=0;
      for (int i=1; i<k; i++)
      {
        if (p_GetExp(Hi->m[i-1],1,r)<p_GetExp(Hi->m[i],1,r))
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
      if (kH==l)
        break;
      if (p_GetExp(G->m[kG],1,r)>p_GetExp(Hi->m[kH],1,r))
        G->m[i] = G->m[kG++];
      else
        G->m[i] = Hi->m[kH++];
    }
    m += l; IDELEMS(GG) = m; GG->m = &G->m[n-m];
    id_Test(it->second,r);
    id_Test(GG,r);
    if (ppreduceInitially(it->second,p,GG,r)) return true;
    id_Test(it->second,r);
    id_Test(GG,r);
    idShallowDelete(&Hi); Hi = it->second;
  }
  idShallowDelete(&Hi);

  ptNormalize(I,p,r);
  omFreeBin((ADDRESS)GG, sip_sideal_bin);
  idShallowDelete(&G);
  return false;
}


#ifndef NDEBUG
BOOLEAN reduceInitiallyDebug(leftv res, leftv args)
{
  leftv u = args;
  if ((u != NULL) && (u->Typ() == IDEAL_CMD))
  {
    leftv v = u->next;
    if ((v != NULL) && (v->Typ() == NUMBER_CMD))
    {
      omUpdateInfo();
      Print("usedBytesBefore=%ld\n",om_Info.UsedBytes);
      ideal I = (ideal) u->CopyD();
      number p = (number) v->Data();
      (void) ppreduceInitially(I,currRing,p);
      res->rtyp = IDEAL_CMD;
      res->data = (char*) I;
      return FALSE;
    }
  }
  return TRUE;
}
#endif


// BOOLEAN ppreduceInitially(leftv res, leftv args)
// {
//   leftv u = args;
//   if ((u != NULL) && (u->Typ() == IDEAL_CMD))
//   {
//     ideal I = (ideal) u->CopyD();
//     (void) ppreduceInitially(I,currRing);
//     res->rtyp = IDEAL_CMD;
//     res->data = (char*) I;
//     return FALSE;
//   }
//   return TRUE;
// }
