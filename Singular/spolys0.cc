/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: spolys0.cc,v 1.4 1997-08-01 10:53:08 Singular Exp $ */

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
  number an = *a, bn = *b, cn = nGcd(an, bn);

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
      spMemcpy(a,p);
      spMonAdd(a,m);
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
static void spGMultCopyX(poly p, poly m, poly n, number exp, poly spNoether)
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
static void spGSpolyLoop(poly a1, poly a2, poly m,poly spNoether)
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
poly spGSpolyRed(poly p1, poly p2,poly spNoether)
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
void spGSpolyTail(poly p1, poly q, poly q2, poly spNoether)
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
poly spGSpolyRedNew(poly p1, poly p2,poly spNoether)
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
  short x;
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
  b = pNew();
  m = pNew();
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

void spShort1(poly b, poly a, poly m)
{
  for(int i=pVariables; i; i--)
  {
    if(pGetExp(m,i)<0)
    {
      pSetExp(b,i,pGetExp(a,i));
    }
  }
  pFree1(m);
  pSetm(b);
}

void spShort2(poly b, poly a, poly m)
{
  short x;
  pSetComp(b,pGetComp(a));
  for(int i=pVariables; i; i--)
  {
    if(pGetExp(m,i)<0)
      x = pGetExp(a,i)-pGetExp(m,i);
    else
      x = pGetExp(a,i);
    pSetExp(b,i,x);
  }
  pFree1(m);
  pSetm(b);
}

/*2
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is 0 (nNew)
*   2. in a first step the monomials from p1 are
*      multiplied with L(p2-p1), hence the multiplication
*      must be additive and further the transformation
*      spShort1 and spShort2 are needed
*/
poly spGSpolyShortBba(poly p1, poly p2)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  poly m,b;
  number ta,tb,t;
  int c;
  int co=0;
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
  b = pNew();
  m = pNew();
  pSetComp(b,pGetComp(p1));
  spMemcpy(m,p2);
  spMonSub(m,p1);
  if(a1!=NULL)
  {
    spMemcpy(b,a1);
    spMonAdd(b,m);
    if(a2==NULL)
    {
      spShort1(b,a1,m);
      nNew(&(pGetCoeff(b)));
      if (co==1) spModuleToPoly(p1);
      else if (co==2) spModuleToPoly(p2);
      return b;
    }
  }
  else
  {
    if(a2!=NULL)
    {
      spShort2(b,a2,m);
      nNew(&(pGetCoeff(b)));
      if (co==1) spModuleToPoly(p1);
      else if (co==2) spModuleToPoly(p2);
      return b;
    }
    else
    {
      pFree1(m);
      pFree1(b);
      if (co==1) spModuleToPoly(p1);
      else if (co==2) spModuleToPoly(p2);
      return NULL;
    }
  }
  loop
  {
    c = pComp0(b,a2);
    if (c == 1)
    {
      spShort1(b,a1,m);
      nNew(&(pGetCoeff(b)));
      break;
    }
    else if (c == -1)
    {
      spShort2(b,a2,m);
      nNew(&(pGetCoeff(b)));
      break;
    }
    else
    {
      if (nIsOne(pGetCoeff(p1)))
      {
        if (nIsOne(pGetCoeff(p2)))
          t = nSub(pGetCoeff(a2), pGetCoeff(a1));
        else
        {
          ta = nMult(pGetCoeff(a1), pGetCoeff(p2));
          t = nSub(ta, pGetCoeff(a2));
          nDelete(&ta);
        }
      }
      else
      {
        if (nIsOne(pGetCoeff(p2)))
        {
          ta = nMult(pGetCoeff(a2), pGetCoeff(p1));
          t = nSub(ta, pGetCoeff(a1));
        }
        else
        {
          ta = nMult(pGetCoeff(a1), pGetCoeff(p2));
          tb = nMult(pGetCoeff(a2), pGetCoeff(p1));
          t = nSub(ta, tb);
          nDelete(&tb);
        }
        nDelete(&ta);
      }
      if (!nIsZero(t))
      {
        nDelete(&t);
        spShort1(b,a1,m);
        nNew(&(pGetCoeff(b)));
        break;
      }
      nDelete(&t);
      pIter(a2);
      pIter(a1);
      if(a1!=NULL)
      {
        spMemcpy(b,a1);
        spMonAdd(b,m);
        if(a2==NULL)
        {
          spShort1(b,a1,m);
          nNew(&(pGetCoeff(b)));
          break;
        }
      }
      else
      {
        if(a2!=NULL)
        {
          spShort2(b,a2,m);
          nNew(&(pGetCoeff(b)));
          break;
        }
        else
        {
          pFree1(m);
          pFree1(b);
          b= NULL; break;
        }
      }
    }
  }
  if (co==1) spModuleToPoly(p1);
  else if (co==2) spModuleToPoly(p2);
  return b;
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
