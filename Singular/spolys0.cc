/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: spolys0.cc,v 1.9 1998-03-16 14:56:46 obachman Exp $ */

/*
* ABSTRACT - s-polynomials and reduction in general
*/

#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "numbers.h"
#include "polys.h"
#include "spolys0.h"

/*0 implementation*/

/*
* input - output: a, b
* returns:
*   a := a/gcd(a,b), b := b/gcd(a,b)
*   and return value
*       0  ->  a != 1,  b != 1
*       1  ->  a == 1,  b != 1
*       2  ->  a != 1,  b == 1
*       3  ->  a == 1,  b == 1
*   this value is used to control the spolys
*/
static int spCheckCoeff(number *a, number *b)
{
  int c = 0;
  number an = *a, bn = *b;
  nTest(an);
  nTest(bn);
  number cn = nGcd(an, bn);

  if(nIsOne(cn))
  {
    an = nCopy(an);
    bn = nCopy(bn);
  }
  else
  {
    an = nIntDiv(an, cn);
    bn = nIntDiv(bn, cn);
  }
  nDelete(&cn);
  if (nIsOne(an))
  {
    c = 1;
  }
  if (nIsOne(bn))
  {
    c += 2;
  }
  *a = an;
  *b = bn;
  return c;
}

/*2
* transforms a1 to a polynomial
*/
void spModuleToPoly(poly a1)
{
#ifdef PDEBUG
  poly t=a1;
  pTest(t);
#endif
  do
  {
    pSetComp(a1,0);
    pIter(a1);
  } while (a1!=NULL);
#ifdef PDEBUG
  pTest(t);
#endif
}

/*2
* assume m = L(m) and Lc(m) = 1
* pNext(m) = result = p*m
* do not destroy p
*/
static void spGMultCopy0(poly p, poly m, poly spNoether)
{
  poly a, b;

  a = m;
  if (spNoether==NULL)
  {
    do
    {
      a = pNext(a) = pNew();
      pCopyAddFast(a,p,m);
      pSetCoeff0(a,nCopy(pGetCoeff(a)));
      pIter(p);
    }
    while (p!=NULL);
    pNext(a) = NULL;
    return;
  }
  else
  {
    do
    {
      b = pNext(a) = pNew();
      spMemcpy(b,p);
      spMonAdd(b,m);
      if (pComp0(b, spNoether) == -1)
      {
        pFree1(b);
        pNext(a) = NULL;
        return;
      }
      a = b;
      pSetCoeff0(a,nCopy(pGetCoeff(a)));
      pIter(p);
    } while (p!=NULL);
    pNext(a) = NULL;
  }
}

/*2
* assume m = L(m) and Lc(m) = -1
* pNext(n) = result = p*m
* do not destroy p
*/
static void spGMultCopy1(poly p, poly m, poly n,poly spNoether)
{
  poly a, b;

  a = n;
  if (spNoether==NULL)
  {
    do
    {
      number tmp;
      a = pNext(a) = pNew();
      spMemcpy(a,p);
      spMonAdd(a,m);
      tmp=nCopy(pGetCoeff(a));
      tmp=nNeg(tmp);
      pSetCoeff0(a,tmp);
      pIter(p);
    }
    while (p!=NULL);
    pNext(a) = NULL;
    return;
  }
  else
  {
    do
    {
      b = pNext(a) = pNew();
      spMemcpy(b,p);
      spMonAdd(b,m);
      if (pComp(b, spNoether) == -1)
      {
        pFree1(b);
        pNext(a) = NULL;
        return;
      }
      a = b;
      number tmp;
      tmp=nCopy(pGetCoeff(a));
      tmp=nNeg(tmp);
      pSetCoeff0(a,tmp);
      pIter(p);
    }
    while (p!=NULL);
    pNext(a) = NULL;
  }
}

/*2
* assume m = L(m) and Lc(m) = 1
* pNext(m) = result = a2-a1*m
* do not destroy a1, but a2
*/
static void spGSpolyLoop1(poly a1, poly a2, poly m, poly spNoether)
{
  poly a, b, s;
  number tb;
  int c;

  if (a2==NULL)
  {
    spGMultCopy1(a1, m, m,spNoether);
    return;
  }
  a = m;
  b = pNew();
  spMemcpy(b,a1);
  spMonAdd(b,m);
  loop
  {
    c = pComp0(b, a2);
    if (c == 1)
    {
      number tmp=nCopy(pGetCoeff(b));
      tmp=nNeg(tmp);
      pSetCoeff0(b,tmp);
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        spMemcpy(b,a1);
        spMonAdd(b,m);
      }
      else
      {
        pNext(a) = a2;
        return;
      }
    }
    else if (c == -1)
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        spGMultCopy1(a1, m, a,spNoether);
        return;
      }
    }
    else
    {
      tb = pGetCoeff(a2);
      if (!nEqual(tb,pGetCoeff(a1)))
      {
        tb = nSub(tb,pGetCoeff(a1));
        pSetCoeff(a2,tb);
        a = pNext(a) = a2;
        pIter(a2);
      }
      else
      {
        s = a2;
        pIter(a2);
        nDelete(&tb);
        pFree1(s);
      }
      pIter(a1);
      if (a1==NULL)
      {
        pFree1(b);
        pNext(a) = a2;
        return;
      }
      if (a2==NULL)
      {
        pFree1(b);
        spGMultCopy1(a1, m, a,spNoether);
        return;
      }
      spMemcpy(b,a1);
      spMonAdd(b,m);
    }
  }
}

/*2
* assume m = L(m) and Lc(m) = exp
* pNext(n) = result = p*m
* do not destroy p
*/
void spGMultCopyX(poly p, poly m, poly n, number exp, poly spNoether)
{
  poly a, b;

  a = n;
  if (spNoether==NULL)
  {
    do
    {
      a = pNext(a) = pNew();
      spMemcpy(a,p);
      spMonAdd(a,m);
      pSetCoeff0(a,nMult(pGetCoeff(p),exp));
      pIter(p);
    } while (p!=NULL);
    pNext(a) = NULL;
    return;
  }
  else
  {
    do
    {
      b = pNext(a) = pNew();
      spMemcpy(b,p);
      spMonAdd(b,m);
      if (pComp(b, spNoether) == -1)
      {
        pFree1(b);
        pNext(a) = NULL;
        return;
      }
      a = b;
      pSetCoeff0(a,nMult(pGetCoeff(p),exp));
      pIter(p);
    } while (p!=NULL);
    pNext(a) = NULL;
  }
}

/*2
* assume m = L(m)
* pNext(m) = result = a2-a1*m
* do not destroy a1, but a2
*/
void spGSpolyLoop(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb,t;
  int c;

  tneg = nCopy(tm);
  tneg = nNeg(tneg);
  if (a2==NULL)
  {
    spGMultCopyX(a1, m, m, tneg,spNoether);
    nDelete(&tneg);
    return;
  }
  a = m;
  b = pNew();
  spMemcpy(b,a1);
  spMonAdd(b,m);
  loop
  {
    c = pComp0(b, a2);
    if (c == 1)
    {
      pSetCoeff0(b,nMult(pGetCoeff(a1),tneg));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        spMemcpy(b,a1);
        spMonAdd(b,m);
      }
      else
      {
        pNext(a) = a2;
        nDelete(&tneg);
        return;
      }
    }
    else if (c == -1)
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        spGMultCopyX(a1, m, a, tneg,spNoether);
        nDelete(&tneg);
        return;
      }
    }
    else
    {
      t = pGetCoeff(a2);
      tb = nMult(pGetCoeff(a1),tm);
      if (!nEqual(t,tb))
      {
        t = nSub(t,tb);
        pSetCoeff(a2,t);
        a = pNext(a) = a2;
        pIter(a2);
      }
      else
      {
        s = a2;
        pIter(a2);
        nDelete(&t);
        pFree1(s);
      }
      nDelete(&tb);
      pIter(a1);
      if (a1==NULL)
      {
        pFree1(b);
        pNext(a) = a2;
        nDelete(&tneg);
        return;
      }
      if (a2==NULL)
      {
        pFree1(b);
        spGMultCopyX(a1, m, a, tneg,spNoether);
        nDelete(&tneg);
        return;
      }
      spMemcpy(b,a1);
      spMonAdd(b,m);
    }
  }
}

/*2
* reduction of p2 with p1
* do not destroy p1, but p2
*/
poly spGSpolyRed(poly p1, poly p2,poly spNoether, spSpolyLoopProc spSpolyLoop)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  number an = pGetCoeff(p1), bn = pGetCoeff(p2);
  int ct = spCheckCoeff(&an, &bn);

  pSetCoeff(p2,bn);
  if(a1==NULL)
  {
    nDelete(&an);
    nDelete(&bn);
    pFree1(p2);
    return a2;
  }
  if ((ct == 0) || (ct == 2))
  {
    pMultN(a2, an);
  }
  nDelete(&an);
  BOOLEAN reset_vec=FALSE;
  if (pGetComp(p1) != pGetComp(p2))
  {
    pSetCompP(a1,pGetComp(p2));
    reset_vec=TRUE;
  }
  spMonSub(p2,p1);
  if (ct < 2)
  {
    spGSpolyLoop(a1, a2, p2,spNoether);
  }
  else
  {
    spGSpolyLoop1(a1, a2, p2,spNoether);
  }
  a2 = pNext(p2);
  if (reset_vec)
  {
    spModuleToPoly(a1);
  }
  nDelete(&bn);
  pFree1(p2);
  return a2;
}

/*2
* reduction of tail(q) with p1
* lead(p1) divides lead(pNext(q2)) and pNext(q2) is reduced
* do not destroy p1, but tail(q)
*/
void spGSpolyTail(poly p1, poly q, poly q2, poly spNoether,
                  spSpolyLoopProc spSpolyLoop)
{
  number t;
  poly m, h;
  poly a1 = pNext(p1), p2 = pNext(q2), a2 = pNext(p2);
  number an = pGetCoeff(p1), bn = pGetCoeff(p2);
  int ct = spCheckCoeff(&an, &bn);
  BOOLEAN reset_vec=FALSE;

  if(a1==NULL)
  {
    nDelete(&an);
    nDelete(&bn);
    nDelete(&pGetCoeff(p2));
    pFree1(p2);
    pNext(q2) = a2;
    return;
  }
  if (p1 != q)
  {
    m = p2;
    pSetCoeff(m,bn);
  }
  else
  {
    m = pNew();
    spMemcpy(m,p2);
    pSetCoeff0(m,bn);
    a2 = pCopy(a2);
  }
  if ((ct == 0) || (ct == 2))
  {
    pMultN(a2, an);
  }
  if ((pGetComp(p1) != pGetComp(p2))
  && (pGetComp(p1)==0))
  {
    pSetCompP(a1,pGetComp(p2));
    reset_vec=TRUE;
  }
  spMonSub(m,p1);
  if (ct < 2)
  {
    spGSpolyLoop(a1, a2, m,spNoether);
  }
  else
  {
    spGSpolyLoop1(a1, a2, m,spNoether);
  }
  if ((ct == 0) || (ct == 2))
  {
    h = q;
    do
    {
      t = nMult(pGetCoeff(h),an);
      pSetCoeff(h,t);
      pIter(h);
    }
    while (h != p2);
  }
  h = pNext(m);
  nDelete(&an);
  nDelete(&bn);
  pFree1(m);
  pNext(q2) = h;
  if (reset_vec)
    spModuleToPoly(a1);
  if (p1 == q)
  {
    pDelete(&p2);
  }
}

/*2
* reduction of p2 with p1
* do not destroy p1 and p2
*/
poly spGSpolyRedNew(poly p1, poly p2,poly spNoether,
                    spSpolyLoopProc spSpolyLoop)
{
  poly m;
  poly a1 = pNext(p1), a2 = pNext(p2);
  number an = pGetCoeff(p1), bn = pGetCoeff(p2);
  int ct = spCheckCoeff(&an, &bn);

  if(a1==NULL)
  {
    nDelete(&an);
    nDelete(&bn);
    return pCopy(a2);
  }
  if ((ct == 0) || (ct == 2))
  {
    a2 = pMultCopyN(a2,an);
  }
  else
  {
    a2 = pCopy(a2);
  }
  pTest(a2);
  pTest(p2);
  nDelete(&an);
  BOOLEAN reset_vec=FALSE;
  if (pGetComp(p1) != pGetComp(p2))
  {
    pSetCompP(a1,pGetComp(p2));
    reset_vec=TRUE;
  }
  m = pNew();
  spMemcpy(m,p2);
  spMonSub(m,p1);
  if (ct < 2)
  {
    pSetCoeff0(m,bn);
    spGSpolyLoop(a1, a2, m,spNoether);
  }
  else
  {
    spGSpolyLoop1(a1, a2, m,spNoether);
  }
  a2 = pNext(m);
  pTest(a2);
  pTest(p2);
  if (reset_vec)
  {
    spModuleToPoly(a1);
  }
  nDelete(&bn);
  pFree1(m);
  pTest(a2);
  pTest(p2);
  return a2;
}

/*2
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
poly spGSpolyCreate(poly p1, poly p2,poly spNoether)
{
  Exponent_t x;
  poly m, b;
  poly a1 = pNext(p1), a2 = pNext(p2);
  number an = pGetCoeff(p1), bn = pGetCoeff(p2);
  int co=0, ct = spCheckCoeff(&an, &bn);

  if (pGetComp(p1)!=pGetComp(p2))
  {
    if (pGetComp(p1)==0)
    {
      co=1;
      pSetCompP(p1,pGetComp(p2));
    }
    else
    {
      co=2;
      pSetCompP(p2,pGetComp(p1));
    }
  }
  b = pInit();
  m = pInit();
  for (int i = pVariables; i; i--)
  {
    x = pGetExp(p1,i) - pGetExp(p2,i);
    if (x > 0)
    {
      pSetExp(b,i,x);
      pSetExp(m,i,0);
    }
    else
    {
      pSetExp(m,i,-x);
      pSetExp(b,i,0);
    }
  }
  pSetm(m);
  pSetm(b);
  if (a2!=NULL)
  {
    if ((ct == 0) || (ct == 2))
    {
      spGMultCopyX(a2, b, b, an,spNoether);
    }
    else
    {
      spGMultCopy0(a2, b,spNoether);
    }
    a2 = pNext(b);
  }
  nDelete(&an);
  pFree1(b);
  if (a1!=NULL)
  {
    if (ct < 2)
    {
      pSetCoeff0(m,bn);
      spGSpolyLoop(a1, a2, m,spNoether);
    }
    else
    {
      spGSpolyLoop1(a1, a2, m,spNoether);
    }
    a2 = pNext(m);
  }
  nDelete(&bn);
  pFree1(m);
  if (co != 0)
  {
    if (co==1)
    {
      spModuleToPoly(p1);
    }
    else
    {
      spModuleToPoly(p2);
    }
  }
  return a2;
}

/*2
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is 0 (nNew)
*   2. pNext is undefined
*/
static void bbb() { int i=0; }
poly spGSpolyShortBba(poly p1, poly p2)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  Exponent_t c1=pGetComp(p1),c2=pGetComp(p2);
  Exponent_t c;
  poly m1,m2;
  number t1,t2;
  int cm,i;
  BOOLEAN equal;

  if (a1==NULL)
  {
    if(a2!=NULL)
    {
      m2=pInit();
x2:
      for (i = pVariables; i; i--)
      {
        c = pGetExpDiff(p1, p2,i);
        if (c>0)
        {
          pSetExp(m2,i,(c+pGetExp(a2,i)));
        }
        else
        {
          pSetExp(m2,i,pGetExp(a2,i));
        }
      }
      if ((c1==c2)||(c2!=0))
      {
        pSetComp(m2,pGetComp(a2));
      }
      else
      {
        pSetComp(m2,c1);
      }
      pSetm(m2);
      nNew(&(pGetCoeff(m2)));
      return m2;
    }
    else
      return NULL;
  }
  if (a2==NULL)
  {
    m1=pInit();
x1:
    for (i = pVariables; i; i--)
    {
      c = pGetExpDiff(p2, p1,i);
      if (c>0)
      {
        pSetExp(m1,i,(c+pGetExp(a1,i)));
      }
      else
      {
        pSetExp(m1,i,pGetExp(a1,i));
      }
    }
    if ((c1==c2)||(c1!=0))
    {
      pSetComp(m1,pGetComp(a1));
    }
    else
    {
      pSetComp(m1,c2);
    }
    pSetm(m1);
    nNew(&(pGetCoeff(m1)));
    return m1;
  }
  m1 = pInit();
  m2 = pInit();
  loop
  {
    for (i = pVariables; i; i--)
    {
      c = pGetExpDiff(p1, p2,i);
      if (c > 0)
      {
        pSetExp(m2,i,(c+pGetExp(a2,i)));
        pSetExp(m1,i,pGetExp(a1,i));
      }
      else
      {
        pSetExp(m1,i,(pGetExp(a1,i)-c));
        pSetExp(m2,i,pGetExp(a2,i));
      }
    }
    if(c1==c2)
    {
      pSetComp(m1,pGetComp(a1));
      pSetComp(m2,pGetComp(a2));
    }
    else
    {
      if(c1!=0)
      {
        pSetComp(m1,pGetComp(a1));
        pSetComp(m2,c1);
      }
      else
      {
        pSetComp(m2,pGetComp(a2));
        pSetComp(m1,c2);
      }
    }
    pSetm(m1);
    pSetm(m2);
    cm = t_pComp0(m1, m2);
    if (cm!=0)
    {
      if(cm==1)
      {
        pFree1(m2);
        nNew(&(pGetCoeff(m1)));
        return m1;
      }
      else
      {
        pFree1(m1);
        nNew(&(pGetCoeff(m2)));
        return m2;
      }
    }
    t1 = nMult(pGetCoeff(a2),pGetCoeff(p1));
    t2 = nMult(pGetCoeff(a1),pGetCoeff(p2));
    equal = nEqual(t1,t2);
    nDelete(&t2);
    nDelete(&t1);
    if (!equal)
    {
      pFree1(m2);
      nNew(&(pGetCoeff(m1)));
      return m1;
    }
    pIter(a1);
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(m2);
      if (a1==NULL)
      {
        pFree1(m1);
        return NULL;
      }
      goto x1;
    }
    if (a1==NULL)
    {
      pFree1(m1);
      goto x2;
    }
  }
}

/*2
* creates the last term of the S-polynomial of p1 and p2
* in order to compute the ecart
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is undefined
*   2. see above
*/
/*
*static poly spGSpolyLast(poly p1, poly p2)
*{
*  poly a1 = pNext(p1), a2 = pNext(p2);
*  poly m, b, L1, L2;
*  number ta,tb,t;
*  int c;
*
*  m = pNew();
*  b = pNew();
*  spMemcpy(m,p2);
*  spMonSub(m,p1);
*  pSetComp(b,pGetComp(p1));
*  if (a2)
*  {
*    while (pNext(a2))
*      pIter(a2);
*  }
*  if (a1)
*  {
*    while (pNext(a1))
*      pIter(a1);
*    spMemcpy(b,a1);
*    spMonAdd(b,m);
*  }
*  else
*  {
*    spShort2(b,a2,m);
*    return b;
*  }
*  if(!a2)
*  {
*    spShort1(b,a1,m);
*    return b;
*  }
*  for (; ; )
*  {
*    c = pComp0(b, a2);
*    if (c == -1)
*    {
*      spShort1(b,a1,m);
*      return b;
*    }
*    else if (c == 1)
*    {
*      spShort2(b,a2,m);
*      return b;
*    }
*    else
*    {
*      if (nIsOne(pGetCoeff(p1)))
*      {
*        if (nIsOne(pGetCoeff(p2)))
*          t = nSub(pGetCoeff(a2), pGetCoeff(a1));
*        else
*        {
*          ta = nMult(pGetCoeff(a1), pGetCoeff(p2));
*          t = nSub(ta, pGetCoeff(a2));
*          nDelete(&ta);
*        }
*      }
*      else
*      {
*        if (nIsOne(pGetCoeff(p2)))
*        {
*          ta = nMult(pGetCoeff(a2), pGetCoeff(p1));
*          t = nSub(ta, pGetCoeff(a1));
*        }
*        else
*        {
*          ta = nMult(pGetCoeff(a1), pGetCoeff(p2));
*          tb = nMult(pGetCoeff(a2), pGetCoeff(p1));
*          t = nSub(ta, tb);
*          nDelete(&tb);
*        }
*        nDelete(&ta);
*      }
*      if (!nIsZero(t))
*      {
*        nDelete(&t);
*        spShort1(b,a1,m);
*        return b;
*      }
*      nDelete(&t);
*      if (a1 != pNext(p1))
*      {
*        L1 = a1;
*        a1 = pNext(p1);
*        while (pNext(a1) != L1)
*          pIter(a1);
*        spMemcpy(b,a1);
*        spMonAdd(b,m);
*      }
*      if (a2 != pNext(p2))
*      {
*        L2 = a2;
*        a2 = pNext(p2);
*        while (pNext(a2) != L2)
*          pIter(a2);
*      }
*    }
*  }
*}
*/
/*2
* creates the leading term of the S-polynomial of p1 and p2
* and computes the ecart under the assumtion that
* the last term of the S-polynomial is the greatest
* do not destroy p1 and p2
*/
/*
*poly spGSpolyShortMora(poly p1, poly p2, int *ecart)
*{
*  poly p, q;
*
*  p = spGSpolyShortBba(p1, p2, ecart);
*  *ecart = 0;
*  if(p)
*  {
*    if(spNoether==NULL)
*    {
*      q = spGSpolyLast(p1, p2);
*      *ecart = pFDeg(q) - pFDeg(p);
*      pFree1(q);
*      return p;
*    }
*    if (pComp(p, spNoether) == -1)
*    {
*      pFree1(p);
*      return NULL;
*    }
*    q = spGSpolyLast(p1, p2);
*    if (pComp(q, spNoether) == -1)
*      *ecart = pFDeg(spNoether) - pFDeg(p);
*    else
*      *ecart = pFDeg(q) - pFDeg(p);
*    pFree1(q);
*  }
*  return p;
*}
*/
