/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kspoly.cc,v 1.13 2000-10-19 15:00:14 obachman Exp $ */
/*
*  ABSTRACT -  Routines for Spoly creation and reductions
*/
#define PDEBUG 2
#include "mod2.h"
#include "kutil.h"
#include "polys.h"
#include "numbers.h"
#include "p_Procs.h"

// Define to enable tests in this file 
#define DEBUG_THIS

#if ! (defined(DEBUG_THIS) || (defined(KDEBUG) && (KDEBUG > 1)))
#undef assume
#define assume(x) 
#endif


/***************************************************************
 *
 * Reduces PR with PW
 * Assumes PR != NULL, PW != NULL, Lm(PR) divides Lm(PW)
 * 
 ***************************************************************/
void ksReducePoly(LObject* PR,
                  TObject* PW,
                  poly spNoether,
                  number *coef)
{
  ring tailRing = PR->tailRing;
  kTest_L(PR);
  kTest_T(PW);
  
  poly p1 = PR->GetLm(tailRing);
  poly p2 = PW->GetLm(tailRing);
  poly t2 = pNext(p2), lm = p1;
  p_CheckPolyRing(p1, tailRing);
  p_CheckPolyRing(p2, tailRing);

  pAssume1(p2 != NULL && p1 != NULL && 
           p_DivisibleBy(p2,  p1, tailRing));

  pAssume1(p_GetComp(p1, tailRing) == p_GetComp(p2, tailRing) ||
           (p_GetComp(p2, tailRing) == 0 && 
            p_MaxComp(pNext(p2),tailRing) == 0));

  if (t2==NULL)
  {
    PR->LmDeleteAndIter();
    if (coef != NULL) *coef = n_Init(1, tailRing);
    return;
  }

  if (! n_IsOne(pGetCoeff(p2), tailRing))
  {
    number bn = pGetCoeff(lm);
    number an = pGetCoeff(p2);
    int ct = ksCheckCoeff(&an, &bn);
    pSetCoeff(lm, bn);
    if ((ct == 0) || (ct == 2)) 
      PR->Tail_Mult_nn(an);
    if (coef != NULL) *coef = an;
    else n_Delete(&an, tailRing);
  }
  else
  {
    if (coef != NULL) *coef = n_Init(1, tailRing);
  }
  
  p_ExpVectorSub(lm, p2, tailRing);

  if (PR->bucket != NULL && PW->pLength <= 0)
    PW->pLength = pLength(p2);
    
  PR->Tail_Minus_mm_Mult_qq(lm, t2, PW->pLength-1, spNoether);
  PR->LmDeleteAndIter();
}

/***************************************************************
 *
 * Creates S-Poly of p1 and p2
 * 
 *
 ***************************************************************/
void ksCreateSpoly(LObject* Pair,poly spNoether, 
                   int use_buckets, ring tailRing)
{
  kTest_L(Pair);
  poly p1 = Pair->p1;
  poly p2 = Pair->p2;
  Pair->tailRing = tailRing;
  
  assume(p1 != NULL);
  assume(p2 != NULL);
  assume(tailRing != NULL);

  poly a1 = pNext(p1), a2 = pNext(p2);
  number lc1 = pGetCoeff(p1), lc2 = pGetCoeff(p2);
  poly m1, m2;
  int co=0, ct = ksCheckCoeff(&lc1, &lc2);
  int x, l1;

  if (pGetComp(p1)!=pGetComp(p2))
  {
    if (pGetComp(p1)==0)
    {
      co=1;
      p_SetCompP(p1,pGetComp(p2), currRing, tailRing);
    }
    else
    {
      co=2;
      p_SetCompP(p2, pGetComp(p1), currRing, tailRing);
    }
  }

  // get m1 = LCM(LM(p1), LM(p2))/LM(p1)
  //     m2 = LCM(LM(p1), LM(p2))/LM(p2)
  m1 = p_Init(tailRing);
  m2 = p_Init(tailRing);
  for (int i = pVariables; i; i--)
  {
    x = pGetExpDiff(p1, p2, i);
    if (x > 0)
    {
      p_SetExp(m2,i,x, tailRing);
      p_SetExp(m1,i,0, tailRing);
    }
    else
    {
      p_SetExp(m1,i,-x, tailRing);
      p_SetExp(m2,i,0, tailRing);
    }
  }
  p_Setm(m1, tailRing);
  p_Setm(m2, tailRing);           // now we have m1 * LM(p1) == m2 * LM(p2)
  pSetCoeff0(m1, lc2);
  pSetCoeff0(m2, lc1); // and now, m1 * LT(p1) == m2 * LT(p2)

  // get m2 * a2
  a2 = tailRing->p_Procs->pp_Mult_mm(a2, m2, spNoether, currRing);
  Pair->SetLmTail(m2, a2, use_buckets, tailRing);

  // get m2*a2 - m1*a1
  Pair->Tail_Minus_mm_Mult_qq(m1, a1, 0, spNoether);
  
  // Clean-up time
  Pair->LmDeleteAndIter();
  p_LmDelete(m1, tailRing);
  
  if (co != 0)
  {
    if (co==1)
    {
      p_SetCompP(p1,0, currRing, tailRing);
    }
    else
    {
      p_SetCompP(p2,0, currRing, tailRing);
    }
  }
}

//////////////////////////////////////////////////////////////////////////
// Reduces PR at Current->next with PW
// Assumes PR != NULL, Current contained in PR 
//         Current->next != NULL, LM(PW) devides LM(Current->next)
// Changes: PR
// Const:   PW
void ksReducePolyTail(LObject* PR, TObject* PW, poly Current, poly spNoether)
{
  poly Lp =     PR->GetLm();
  poly Save =   PW->GetLm();
  
  assume(Lp != NULL && Current != NULL && pNext(Current) != NULL);
  assume(PR->bucket == NULL);
  pAssume(pIsMonomOf(Lp, Current));

  LObject Red(pNext(Current), PR->tailRing);
  TObject With(PW, Lp == Save);
  number coef;

  ksReducePoly(&Red, &With, spNoether, &coef);
  
  if (! n_IsOne(coef, currRing))
  {
    pNext(Current) = NULL;
    PR->Mult_nn(coef);
  }

  n_Delete(&coef, currRing);
  pNext(Current) = Red.GetLm(PR->tailRing);

  if (Lp == Save) With.Delete();
}

/***************************************************************
 *
 * Auxillary Routines
 * 
 * 
 ***************************************************************/

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
int ksCheckCoeff(number *a, number *b)
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
* creates the leading term of the S-polynomial of p1 and p2
* do not destroy p1 and p2
* remarks:
*   1. the coefficient is 0 (nNew)
*   2. pNext is undefined
*/
//static void bbb() { int i=0; }
poly ksCreateShortSpoly(poly p1, poly p2)
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
  for(;;)
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
    cm = pLmCmp(m1, m2);
    if (cm!=0)
    {
      if(cm==1)
      {
        pLmFree(m2);
        nNew(&(pGetCoeff(m1)));
        return m1;
      }
      else
      {
        pLmFree(m1);
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
      pLmFree(m2);
      nNew(&(pGetCoeff(m1)));
      return m1;
    }
    pIter(a1);
    pIter(a2);
    if (a2==NULL)
    {
      pLmFree(m2);
      if (a1==NULL)
      {
        pLmFree(m1);
        return NULL;
      }
      goto x1;
    }
    if (a1==NULL)
    {
      pLmFree(m1);
      goto x2;
    }
  }
}





  
  
