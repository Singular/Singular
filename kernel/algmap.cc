/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: algmap.cc,v 1.1.1.1 2003-10-06 12:15:50 Singular Exp $ */
/*
* ABSTRACT - the mapping of polynomials from rings with
* 'alg' numbers
*/

#include "mod2.h"
#include "omalloc.h"
#include "polys.h"
#include "numbers.h"
#include "longalg.h"
#include "ring.h"
#include "febase.h"
#include "maps.h"
#include "algmap.h"


static poly maLongalg1Fetch(poly res, poly p0, ring r0, int n,
                            int t, BOOLEAN *nom)
{
  napoly a0, b0;
  poly q0, q1 = NULL;
  int i, j, m = r0->N;

  if (naGetDenom0(pGetCoeff(p0)) != NULL)
  {
    *nom = TRUE;
    return res;
  }
  a0 = naGetNom0(pGetCoeff(p0));
  do
  {
    q0 = pInit();
    pSetComp(q0,p_GetComp(p0, r0));
    if (t!=0)
    {
      pGetCoeff(q0) = (number)omAlloc0Bin(rnumber_bin);
      b0 = naGetNom0(pGetCoeff(q0)) = napNew();
      napGetCoeff(b0) = nacCopy(napGetCoeff(a0));
      for (i=1; i<=t; i++)
      {
        napSetExp(b0,i,napGetExpFrom(a0,i,r0));
      }
    }
    else
    {
      pGetCoeff(q0) = nCopy(napGetCoeff(a0));
    }
    for (i=m; i>0; i--)
    {
      pSetExp(q0,i, p_GetExp( p0,i,r0));
    }
    j = t;
    for (i=m+1; i<=n; i++)
    {
      j++;
      pSetExp(q0,i, napGetExpFrom(a0,j,r0));
    }
    pSetm(q0);
    q1 = pAdd(q1, q0);
    a0 = napNext(a0);
  }
  while (a0 != NULL);
  return pAdd(res, q1);
}

static poly maLongalg2Fetch(poly res, poly p0, ring r0, int n, int s,
                            int t, BOOLEAN *nom)
{
  poly q0;
  int i, j;
  napoly b0;
  napoly a0 = NULL, b1 = NULL;

  if (s!=0)
  {
    if (naGetDenom0(pGetCoeff(p0)) != NULL)
    {
      *nom = TRUE;
      return res;
    }
    a0 = naGetNom0(pGetCoeff(p0));
  }
  q0 = pInit();
  pSetComp(q0,p_GetComp(p0, r0));
  for (i=n; i>0; i--)
  {
    pSetExp(q0,i, p_GetExp(p0,i,r0));
  }
  pSetm(q0);
  do
  {
    b0 = napNew();
    if (s!=0)
    {
      napGetCoeff(b0) = nacCopy(napGetCoeff(a0));
      for (i=1; i<=s; i++)
      {
        napSetExp(b0,i, napGetExp(a0,i));
      }
    }
    else
    {
      napGetCoeff(b0) = nacCopy(pGetCoeff(p0));
    }
    j = n;
    for (i=s+1; i<=t; i++)
    {
      j++;
      napSetExp(b0,i, p_GetExp(p0,j,r0));
    }
    if (s==0)
    {
      pGetCoeff(q0)=(number)omAlloc0Bin(rnumber_bin);
      naGetNom0(pGetCoeff(q0)) = b0;
      return pAdd(res, q0);
    }
    b1 = napAdd(b1, b0);
    a0 = napNext(a0);
  }
  while (a0 != NULL);
  if (pGetCoeff(q0)==NULL)
    pGetCoeff(q0) = (number)omAlloc0Bin(rnumber_bin);
  naGetNom0(pGetCoeff(q0)) = b1;
  return pAdd(res, q0);
}

/*2
* return Fe(preimage)
* Fe : k(y(1),..,y(s))[x(1),..,x(m)] -> k(y(1),..,y(t))[x(1),..,x(n)]
*      with:
*      s+m = t+n,
*      Fe(y(i)) = y(i), i = 1,..,min(s,t),
*      Fe(x(i)) = x(i), i = 1,..,min(m,n),
*      and
*      1. for s>t: Fe(y(i)) = x(i+n-t), i = t+1,..,s
*      2. for m>n: Fe(x(i)) = y(i+t-n), i = n+1,..,m
*/
poly maAlgpolyFetch(ring R, poly preimage)
{
  BOOLEAN nom=FALSE;
  int m, n, s, t;
  poly p0, result=NULL;

  if (preimage == NULL)
  {
    return NULL;
  }
  m = R->N;
  n = currRing->N;
  s = rPar(R);
  t = rPar(currRing);
  if ((m+s) != (n+t))
  {
    WerrorS("no algfetch possible");
    return NULL;
  }
  if (n == m)
  {
    return pCopy(preimage);
  }
  p0 = preimage;
  if (s > t)
  {
    while (p0!=NULL)
    {
      result = maLongalg1Fetch(result, p0, R, n, t, &nom);
      if (nom)
      {
        goto err_algfetch;
      }
      pIter(p0);
    }
  }
  else
  {
    while (p0!=NULL)
    {
      result = maLongalg2Fetch(result, p0, R, n, s, t, &nom);
      if (nom)
      {
        goto err_algfetch;
      }
      pIter(p0);
    }
  }
  return result;

err_algfetch:
  pDelete(&result);
  WerrorS("denominator in algnumber");
  return NULL;
}

static poly maLongalgMap(poly res, ring r, poly p0, int s, int t,
                         BOOLEAN *nom, poly monpart, ideal F)
{
  number cc;
  napoly a0, b0;
  poly q, q0, q1 = NULL;
  int i;

  if (s == 0)
  {
    if (t!=0)
    {
      nNew(&cc);
      b0 = napNew();
      napGetCoeff(b0) = pGetCoeff(p0);
      naGetNom0(cc) = b0;
      pMult_nn(monpart,cc);
      napGetCoeff(b0) = NULL;
      nDelete(&cc);
    }
    else
    {
      pMult_nn(monpart,pGetCoeff(p0));
    }
    pSetCompP(monpart, p_GetComp(p0,r));
    return pAdd(res, monpart);
  }
  if (naGetDenom0(pGetCoeff(p0)) != NULL)
  {
    *nom = TRUE;
    WerrorS("denominator in algnumber");
    pDelete(&monpart);
    pDelete(&res);
    return NULL;
  }
  a0 = naGetNom0(pGetCoeff(p0));
  do
  {
    q = pInit();
    if (t!=0)
    {
      pGetCoeff(q) = (number)omAlloc0Bin(rnumber_bin);
      b0 = naGetNom0(pGetCoeff(q)) = napNew();
      napGetCoeff(b0) = nacCopy(napGetCoeff(a0));
    }
    else
    {
      pGetCoeff(q) = nCopy(napGetCoeff(a0));
    }
    for(i=0; i<s; i++)
    {
      if (napGetExpFrom(a0,i+1,r) != 0)
      {
        if (F->m[i]!=NULL)
        {
          q0 = pPower(pCopy(F->m[i]),napGetExpFrom(a0,i+1,r));
          q = pMult(q, q0);
        }
        else
        {
          pDelete(&q);
          break;
        }
      }
    }
    q1 = pAdd(q1, q);
    a0 = napNext(a0);
  }
  while (a0 != NULL);
  q1 = pMult(q1,monpart);
  pSetCompP(q1,p_GetComp(p0,r));
  return pAdd(res, q1);
}

number maNumberOne(number x)
{
  return nInit(1);
}

/*2
* return Ma(preimage)
* Ma : k(y(1),..,y(s))[x(1),..,x(m)] -> k(y(1),..,y(t))[z(1),..,z(n)]
* the ideals F = f_1,..,f_s and G = g_1,..,g_m in k(y(1),..,y(t))[z(1),..,z(n)]
* are as follows:
* f_i = Ma(y(i)), g_i = Ma(x(i))
*/
poly maAlgpolyMap(ring R, poly preimage, ideal F, ideal G)
{
  BOOLEAN nom=FALSE;
  int m, s, t;
  poly p0, monpart, result = NULL;

  if (preimage == NULL)
  {
    return NULL;
  }
  m = R->N;
  if (m != IDELEMS(G))
  {
    WerrorS("error 1 in algmap");
    return NULL;
  }
  s = rPar(R);
  if ((s!=0) && (s != IDELEMS(F)))
  {
    WerrorS("error 2 in algmap");
    return NULL;
  }
  t = rPar(currRing);
  p0 = preimage;
  poly pr=NULL;
  while (p0!=NULL)
  {
    poly pr=pNext(p0);
    p0->next=NULL;
    monpart = maEval((map)G, p0, R, maNumberOne);
    result = maLongalgMap(result, R, p0, s, t, &nom, monpart, F);
    pTest(result);
    if (nom)
    {
      return NULL;
    }
    p0->next = pr;
    p0=pr;
  }
  return result;
}
