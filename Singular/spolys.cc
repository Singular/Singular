/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: spolys.cc,v 1.23 2000-08-14 12:56:52 obachman Exp $ */

/*
* ABSTRACT - s-polynomials and reduction for char p
*/

#include <string.h>
#include "mod2.h"
#include <ommemory
#include "tok.h"
#include "kstd1.h"
#include "numbers.h"
#include "modulop.h"
#include "polys.h"
//#include "ipid.h"
#include "febase.h"
#include "spolys0.h"
#include "spolys.h"
#include "spSpolyLoop.h"


/*0 implementation*/

#ifdef SDRING
poly spDSpolyRed(poly p1,poly p2,poly spNoether, spSpolyLoopProc dummy);
poly spDSpolyCreate(poly p1,poly p2,poly spNoether, spSpolyLoopProc dummy);
#endif

poly (*spSpolyRed)(poly p1, poly p2,poly spNoether,
                   spSpolyLoopProc spSpolyLoop);
void (*spSpolyTail)(poly p1, poly q, poly q2, poly spNoether,
                    spSpolyLoopProc spSpolyLoop);
poly (*spSpolyRedNew)(poly p1, poly p2,poly spNoether,
                      spSpolyLoopProc spSpolyLoop);
poly (*spSpolyCreate)(poly p1, poly p2,poly spNoether,
                      spSpolyLoopProc SpolyLoop);
poly (*spSpolyShortBba)(poly p1, poly p2);

/*2
* assume m = L(m) and Lc(m) = 1
* pNext(m) = result = p*m
* do not destroy p
*/
static void spMultCopy0(poly p, poly m, poly spNoether)
{
  poly a, b;

  a = m;
  if (spNoether==NULL)
  {
    do
    {
      a = pNext(a) = pNew();
      pCopyAddFast(a,p,m);
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
      pCopyAddFast(b,p,m);
      if (pComp0(b, spNoether) == -1)
      {
        pFree1(b);
        pNext(a) = NULL;
        return;
      }
      a = b;
      pIter(p);
    }
    while (p!=NULL);
    pNext(a) = NULL;
  }
}

/*2
* assume m = L(m) and Lc(m) = -1
* pNext(n) = result = p*m
* do not destroy p
*/
static void spMultCopy1(poly p, poly m, poly n, poly spNoether)
{
  poly a, b;

  a = n;
  if (spNoether==NULL)
  {
    do
    {
      a = pNext(a) = pNew();
      pCopyAddFast(a,p,m);
      pSetCoeff0(a,npNegM(pGetCoeff(a)));
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
      pCopyAddFast(b,p,m);
      spMemcpy(b,p);
      spMonAdd(b,m);
      if (pComp0(b, spNoether) == -1)
      {
        pFree1(b);
        pNext(a) = NULL;
        return;
      }
      a = b;
      pSetCoeff0(a,npNegM(pGetCoeff(a)));
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
static void spSpolyLoop1(poly a1, poly a2, poly m, poly spNoether)
{
  poly a, b, s;
  int c;

  if (a2==NULL)
  {
    spMultCopy1(a1, m, m, spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b,a1,m);
  loop
  {
    c = pComp0(b, a2);
    if (c == 1)
    {
      pSetCoeff0(b,npNegM(pGetCoeff(b)));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        pCopyAddFast(b,a1,m);
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
        spMultCopy1(a1, m, a,spNoether);
        return;
      }
    }
    else
    {
      if (!npEqualM(pGetCoeff(a2),pGetCoeff(a1)))
      {
        pSetCoeff0(a2,npSubM(pGetCoeff(a2),pGetCoeff(a1)));
        a = pNext(a) = a2;
        pIter(a2);
      }
      else
      {
        s = a2;
        pIter(a2);
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
        spMultCopy1(a1, m, a,spNoether);
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
void spMultCopyX(poly p, poly m, poly n, number exp, poly spNoether)
{
  poly a, b;

  a = n;
  if (spNoether==NULL)
  {
    do
    {
      a = pNext(a) = pNew();
      pCopyAddFast(a,p,m);
      pSetCoeff0(a,npMultM(pGetCoeff(p),exp));
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
      pCopyAddFast(b,p,m);
      if (pComp0(b, spNoether) == -1)
      {
        pFree1(b);
        pNext(a) = NULL;
        return;
      }
      a = b;
      pSetCoeff0(a,npMultM(pGetCoeff(p),exp));
      pIter(p);
    }
    while (p!=NULL);
    pNext(a) = NULL;
  }
}

/*2
* reduction of p2 with p1
* do not destroy p1, but p2
*/
static poly spPSpolyRed(poly p1, poly p2,poly spNoether,
                        spSpolyLoopProc SpolyLoop)
{
  pTest(p1);
  pTest(p2);
  poly a1 = pNext(p1), a2 = pNext(p2);
  if(a1==NULL)
  {
    pFree1(p2);
    return a2;
  }
  BOOLEAN reset_vec=FALSE;
  if (pGetComp(p1) != pGetComp(p2))
  {
    pSetCompP(a1,pGetComp(p2));
    reset_vec=TRUE;
  }
  pTest(p1);
  pTest(p2);
  spMonSub(p2,p1);
  if (1!=(int)pGetCoeff(p2))
  {
    if (SpolyLoop != NULL)
      SpolyLoop(a1, a2, p2,spNoether);
    else
      spSpolyLoop_General(a1, a2, p2,spNoether);
  }
  else
    spSpolyLoop1(a1, a2, p2,spNoether);
  a2 = pNext(p2);
  if (reset_vec)
    spModuleToPoly(a1);
  pFree1(p2);
  return a2;
}

/*
* reduction of tail(q) with p1
* lead(p1) divides lead(pNext(q2)) and pNext(q2) is reduced
* do not destroy p1, but tail(q)
*/
static void spPSpolyTail(poly p1, poly q, poly q2, poly spNoether,
                         spSpolyLoopProc spSpolyLoop)
{
  poly h = pNext(q2);
  if (p1 != q)
  {
    pNext(q2) = spSpolyRed(p1, h, spNoether, spSpolyLoop);
  }
  else
  {
    pNext(q2) = spSpolyRedNew(p1, h, spNoether, spSpolyLoop);
    pDelete(&h);
  }
}

/*2
* reduction of p2 with p1
* do not destroy p1 and p2
*/
static poly spPSpolyRedNew(poly p1, poly p2,poly spNoether,
                           spSpolyLoopProc SpolyLoop)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  poly m;
  if (a2!=NULL)
    a2 = pCopy(a2);
  if (a1==NULL)
    return a2;
  BOOLEAN reset_vec=FALSE;
  if (pGetComp(p1) != pGetComp(p2))
  {
    pSetCompP(a1,pGetComp(p2));
    reset_vec=TRUE;
  }
  m = pNew();
  spMemcpy(m,p2);
  spMonSub(m,p1);
  if (1!=(int)pGetCoeff(m))
  {
    if (SpolyLoop != NULL)
      SpolyLoop(a1, a2, m,spNoether);
    else
      spSpolyLoop_General(a1, a2, m,spNoether);
  }
  else
    spSpolyLoop1(a1, a2, m,spNoether);
  a2 = pNext(m);
  if (reset_vec)
    spModuleToPoly(a1);
  pFree1(m);
  return a2;
}

/*2
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
static poly spPSpolyCreate(poly p1, poly p2,poly spNoether,
                           spSpolyLoopProc SpolyLoop)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  poly m, b;
  int co=0;
  Exponent_t c;
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
    c = pGetExpDiff(p1, p2,i);
    if (c > 0)
    {
      pSetExp(b,i,c);
      pSetExp(m,i,0);
    }
    else
    {
      pSetExp(m,i,-c);
      pSetExp(b,i,0);
    }
  }
  pSetm(m);
  pSetm(b);
  if (a2!=NULL)
  {
    spMultCopy0(a2, b,spNoether);
    a2 = pNext(b);
  }
  pFree1(b);
  if (a1!=NULL)
  {
    spSpolyLoop1(a1, a2, m,spNoether);
    a2 = pNext(m);
  }
  pFree1(m);
  if (co==1) spModuleToPoly(p1);
  else if (co==2) spModuleToPoly(p2);
  return a2;
}

/*2
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is undefined
*   2. pNext is undefined
*/
//#ifndef TEST_MAC_ORDER
#if 1
static poly spPSpolyShortBba(poly p1, poly p2)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  Exponent_t c1=pGetComp(p1),c2=pGetComp(p2);
  Exponent_t c;
  poly m1,m2;
  int cm,i;

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
      return m2;
    }
    else
      return NULL;
  }
  else if (a2==NULL)
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
    cm = pComp0(m1, m2);
    if (cm!=0)
    {
      if(cm==1)
      {
        pFree1(m2);
        return m1;
      }
      else
      {
        pFree1(m1);
        return m2;
      }
    }
    if (!npEqualM(pGetCoeff(a2),pGetCoeff(a1)))
    {
      pFree1(m2);
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
#else
static poly spPSpolyShortBba(poly p1, poly p2)
{
  poly a1 = pNext(p1), a2 = pNext(p2);
  int co=0;
  // adjust components, store change poly-nr in co
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
  //------------------------------------------------------------
  poly b = pInit(); /* the result */
  poly lcm=pInit();
  pLcm(p1,p2,lcm);
  pSetComp(b,pGetComp(p1));
  loop
  {
    if(a1==NULL)
    {
      if(a2==NULL)
      {
        pFree1(b);
        b=NULL;
        break;
      }
      else
      {
        spMemcpy(b,a2);
        MyspMonAdd0(b,lcm);
        spMonSub(b,p2);
        break;
      }
    }
    else /*a1!=NULL*/
    {
      if(a2==NULL)
      {
        spMemcpy(b,a1);
        MyspMonAdd0(b,lcm);
        spMonSub(b,p1);
        break;
      }
    }
    /* now a1!=NULL and a2!=NULL */
    {
      poly n1=pNew();
      poly n2=pNew();
      spMemcpy(n1,a1);
      spMemcpy(n2,a2);
      spMonAdd(n1,p2);
      spMonAdd(n2,p1);
      int c=pComp0(n1,n2);
      pFree1(n2);
      pFree1(n1);
      if(c==1)
      {
        spMemcpy(b,a1);
        MyspMonAdd0(b,lcm);
        spMonSub(b,p1);
        break;
      }
      else if(c==-1)
      {
        spMemcpy(b,a2);
        MyspMonAdd0(b,lcm);
        spMonSub(b,p2);
        break;
      }
      else
      {
        if (!npEqualM(pGetCoeff(a2),pGetCoeff(a1)))
        {
          spMemcpy(b,a1);
          MyspMonAdd0(b,lcm);
          spMonSub(b,p1);
          break;
        }
        pIter(a1);
        pIter(a2);
      }
    }
  }
  if (co==1) spModuleToPoly(p1);
  else if (co==2) spModuleToPoly(p2);
  pFree1(lcm);
  return b;
}
#endif

#ifdef SDRING
poly spDSpolyRed(poly p1,poly p2,poly spNoether, spSpolyLoopProc dummy)
{
  poly m=pOne();
  poly tp1;
  number n1,n2;

  int i;

  pTest(p1);
  for (i=1;i<=pVariables;i++)
  {
    pSetExp(m,i,pGetExp(p2,i)-pGetExp(p1,i));
  }
#ifdef DRING
  if (pDRING)
  {
    if (pdDFlag(p1)==0) /* && (pdDFlag(p2==0) */
    {
      for (i=pdN+1;i<=2*pdN;i++)
      {
        if (pGetExp(m,i)!=0)
        {
          pDelete1(&m);
          pTest(p2);
          return p2;
        }
      }
    }
    pdSetDFlag(m,1);
  }
#endif
  pSetm(m);
  pTest(p1);
  tp1=pMult(m,pCopy(p1));
  pTest(tp1);
  if (tp1!=NULL)
  {
    n2=nCopy(pGetCoeff(p2));
    n2=nNeg(n2);
    n1=pGetCoeff(tp1);
    pMultN(tp1,n2);
    pMultN(p2,n1);
    nDelete(&n2);
    //pDelete1(&p2);
    //pDelete1(&tp1);
    p2=pAdd(tp1,p2);
  }
  pTest(p2);
  return p2;
}

poly spDSpolyRedNew(poly p1,poly p2,poly spNoether, spSpolyLoopProc dummy)
{
  return spDSpolyRed(p1,pCopy(p2),spNoether, dummy);
}

poly spDSpolyCreate(poly p1,poly p2,poly spNoether, spSpolyLoopProc dummy)
{
  poly m1=pOne();
  poly m2=pOne();
  poly pp1=p1,pp2=p2;
  number n1,n2;
  int i,j;
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

  for (i=1;i<=pVariables;i++)
  {
    j=pGetExp(p2,i)-pGetExp(p1,i);
    if (j>0)
    {
      pSetExp(m1,i,j);
    }
    else
    {
      pSetExp(m2,i,-j);
    }
  }
#ifdef DRING
  if (pDRING)
  {
    if (pdDFlag(p1)==0) /* && (pdDFlag(p2==0) */
    {
      for (i=pdN+1;i<=2*pdN;i++)
      {
        if ((pGetExp(m1,i)!=0) || (pGetExp(m2,i)!=0))
        {
         pDelete1(&m1);
         pDelete1(&m2);
         return NULL;
        }
      }
    }
    pdSetDFlag(m1,1);
    pdSetDFlag(m2,1);
  }
#endif
  pSetm(m1);
  pSetm(m2);
  p1=pMult(m1,pCopy(p1));
  p2=pMult(m2,pCopy(p2));

  n1=nCopy(pGetCoeff(p1));
  n2=nCopy(pGetCoeff(p2));

  pDelete1(&p1);
  pDelete1(&p2);

  n1=nNeg(n1);
  pMultN(p1,n2);
  nDelete(&n2);
  pMultN(p2,n1);
  nDelete(&n1);

  m1=pAdd(p1,p2);
  if (co==1) spModuleToPoly(pp1);
  else if (co==2) spModuleToPoly(pp2);
  pTest(m1);
  return m1;
}

#endif

void spSet(ring r)
{
  if ((rField_is_Q())
  || (rField_is_Extension()) /* Q(a), Fp(a) */
  || (rField_is_numeric()) /* R, long R, long C*/
#ifdef SRING
  && (pSRING==0)
#endif
#ifdef DRING
  && (pDRING==0)
#endif
  )
  {
    spSpolyRed=spGSpolyRed;
    spSpolyTail=spGSpolyTail;
    spSpolyRedNew=spGSpolyRedNew;
    spSpolyCreate=spGSpolyCreate;
    spSpolyShortBba=spGSpolyShortBba;
    return;
  }
  if (rField_is_GF(r) /* finite fields GF(p,n) */
#ifdef SRING
  && (pSRING==0)
#endif
#ifdef DRING
  && (pDRING==0)
#endif
  )
  {
    spSpolyRed=spGSpolyRed;
    spSpolyTail=spGSpolyTail;
    spSpolyRedNew=spGSpolyRedNew;
    spSpolyCreate=spGSpolyCreate;
    spSpolyShortBba=spGSpolyShortBba;
    return;
  }
  if (rField_is_Zp(r) /* Fp */
#ifdef SRING
  && (pSRING==0)
#endif
#ifdef DRING
  && (pDRING==0)
#endif
  )
  {
    spSpolyRed=spPSpolyRed;
    spSpolyTail=spPSpolyTail;
    spSpolyRedNew=spPSpolyRedNew;
    spSpolyCreate=spPSpolyCreate;
    spSpolyShortBba=spPSpolyShortBba;
    return;
  }
#ifdef SRING
  if (pSRING!=0)
  {
    spSpolyRed=spDSpolyRed;
    spSpolyRedNew=spDSpolyRedNew;
    spSpolyCreate=spDSpolyCreate;
    spSpolyShortBba=NULL;/*spGSpolyShortBba;*/
    test&=~Sy_bit(OPT_REDTAIL); // no redtail
    return;
  }
#endif
#ifdef DRING
  if (pDRING!=0)
  {
    spSpolyRed=spDSpolyRed;
    spSpolyRedNew=spDSpolyRedNew;
    spSpolyCreate=spDSpolyCreate;
    spSpolyShortBba=NULL;/*spGSpolyShortBba;*/
    test&=~Sy_bit(OPT_REDTAIL); // no redtail
    return;
  }
#endif
  WarnS("spoly init err");
}
