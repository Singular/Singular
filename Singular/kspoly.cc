/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kspoly.cc,v 1.12 2000-10-16 12:06:34 obachman Exp $ */
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
  
  kTest_L(PR);
  k_Test_T(PW, PR->tailRing);
  
  poly p1 = PR->p;
  poly p2 = PW->p;
  poly t2 = pNext(p2), lm = p1;
  ring lmRing = PR->lmRing;
  ring tailRing = PR->tailRing;

  pAssume1(p2 != NULL && p1 != NULL && 
           p_DivisibleBy(p2,  currRing, p1, lmRing));

  pAssume1(p_GetComp(p1, lmRing) == p_GetComp(p2, currRing) ||
           (p_GetComp(p2, currRing) == 0 && 
            p_MaxComp(pNext(p2),tailRing) == 0));

  if (t2==NULL)
  {
    PR->Iter();
    p_LmDelete(lm, lmRing);
    if (coef != NULL) *coef = n_Init(1, currRing);
    return;
  }

  if (! n_IsOne(pGetCoeff(p2), currRing))
  {
    number bn = pGetCoeff(lm);
    number an = pGetCoeff(p2);
    int ct = ksCheckCoeff(&an, &bn);
    pSetCoeff(lm, bn);
    if ((ct == 0) || (ct == 2)) 
      PR->Tail_Mult_nn(an);
    if (coef != NULL) *coef = an;
    else n_Delete(&an, currRing);
  }
  else
  {
    if (coef != NULL) *coef = n_Init(1, currRing);
  }
  
  if (lmRing != tailRing)
    k_LmShallowCopyDelete_lmRing_2_tailRing(lm, currRing, tailRing);
  if (currRing != tailRing)
  {
    poly p_temp = k_LmInit_lmRing_2_tailRing(p2, currRing, tailRing);
    p_ExpVectorSub(lm, p_temp, tailRing);
    p_LmFree(p_temp, tailRing);
  }
  else
  {
    p_ExpVectorSub(lm, p2, tailRing);
  }

  if (PR->bucket != NULL && PW->pLength <= 0)
    PW->pLength = pLength(PW->p);
    
  PR->Tail_Minus_mm_Mult_qq(lm, t2, PW->pLength-1, spNoether);

  PR->Iter();
  
  p_LmDelete(lm, tailRing);
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
  Pair->lmRing = currRing;
  
  
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
  Pair->SetLmTail(m2, a2, use_buckets);

  // get m2*a2 - m1*a1
  Pair->Tail_Minus_mm_Mult_qq(m1, a1, 0, spNoether);
  Pair->Iter();
  
  // Clean-up time
  p_LmDelete(m1, tailRing);
  p_LmDelete(m2, tailRing);
  
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
void ksSpolyTail(LObject* PR, TObject* PW, poly Current, poly spNoether)
{
  poly Lp = PR->p;
  number coef;
  poly Save = PW->p;
  ring lmRing = PR->lmRing;
  
  assume(Lp != NULL && Current != NULL && pNext(Current) != NULL);
  assume(PR->bucket == NULL);
  pAssume(pIsMonomOf(Lp, Current));

  if (Lp == Save)
    PW->p = p_Copy(Save, currRing, PR->tailRing);

  
  PR->p = pNext(Current);
  PR->lmRing = PR->tailRing;
  ksReducePoly(PR, PW, spNoether, &coef);
  
  if (! n_IsOne(coef, currRing))
  {
    pNext(Current) = NULL;
    p_Mult_nn(Lp, coef, currRing, PR->tailRing);
  }
  n_Delete(&coef, currRing);
  pNext(Current) = PR->p;
  PR->p = Lp;
  PR->lmRing = lmRing;
  if (PW->p != Save)
  {
    p_Delete(&(PW->p), currRing, PR->tailRing);
    PW->p = Save; // == Lp
  }
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


/***************************************************************
 *
 * Routines for backwards-Compatibility
 * 
 * 
 ***************************************************************/
poly ksOldSpolyRed(poly p1, poly p2, poly spNoether)
{
  LObject L;
  TObject T;

  L.p = p2;
  T.p = p1;
  
  ksReducePoly(&L, &T, spNoether);
  
  return L.p;
}

poly ksOldSpolyRedNew(poly p1, poly p2, poly spNoether)
{
  LObject L;
  TObject T;

  L.p = pCopy(p2);
  T.p = p1;
  
  ksReducePoly(&L, &T, spNoether);
  
  return L.p;
}

poly ksOldCreateSpoly(poly p1, poly p2, poly spNoether)
{
  LObject L;
  L.p1 = p1;
  L.p2 = p2;
  
  ksCreateSpoly(&L, spNoether);
  return L.p;
}

void ksOldSpolyTail(poly p1, poly q, poly q2, poly spNoether)
{
  LObject L;
  TObject T;

  L.p = q;
  T.p = p1;
  
  ksSpolyTail(&L, &T, q2, spNoether);
  return;
}



  
  
