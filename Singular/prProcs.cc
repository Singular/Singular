/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: prProcs.cc,v 1.5 2000-09-04 13:39:05 obachman Exp $ */
/*
*  ABSTRACT -  Routines for primitive poly arithmetic
*/
#include "mod2.h"
#include <omalloc.h>
#include "polys.h"

#ifndef HAVE_P_PROCS
#include "polys-comp.h"
#include "prProcs.h"
#include "numbers.h"

// #define HAVE_CHAR_P

#ifdef HAVE_CHAR_P
#include "modulop.h"

#undef nDelete
#define nDelete(n) 
#define nMult(n1, n2)   npMultM(n1, n2)
#define nAdd(n1, n2)    npAddM(n1, n2)
#define nSub(n1, n2)    npSubM(n1, n2)
#define nEqual(n1, n2)  npEqualM(n1, n2)
#define nIsZero(n)      npIsZeroM(n)
#endif

// Define to enable tests in this file 
// #define DEBUG_THIS

#if ! (defined(DEBUG_THIS) || (defined(PDEBUG) && PDEBUG > 0))
#undef assume
#define assume(x) 
#endif

#define prFree1AndAdvance(p, r)                  \
do                                              \
{                                               \
  poly __p = p;                                 \
  p = pNext(__p);                               \
  omFreeBin(__p, r->PolyBin);                \
}                                               \
while(0)

poly pr_Mult_n_General(poly p, number n)
{
  poly q = p;
  while (p != NULL)
  {
    number nc = pGetCoeff(p);
    pSetCoeff0(p, nMult(n, nc));
    nDelete(&nc);
    pIter(p);
  }
  return q;
}


/***************************************************************
 *
 * General:  pr_Add_q which always works
 * Returns:  p + q, *lp == pLength(p+q), p+q are from heap
 * Destroys: p, q
 * Assume:   *lp == NULL || pLength(p) == *lp && pLength(q) == q
 *           p, q are from heap
 * 
 ***************************************************************/

poly pr_Add_q_General(poly p, poly q, int *lp, int lq,
                     const ring r)
{
  assume(prTest(p, r));
  assume(prTest(q, r));
  assume(lp == NULL || pLength(p) == *lp);
  assume(lp == NULL || pLength(q) == lq);

  // test for trivial cases
  if (q == NULL) return p;
  if (p == NULL)
  {
    if (lp != NULL) *lp = lq;
    return q;
  }
  
  number t, n1, n2;
  int l;
  spolyrec rp;
  poly a = &rp;

  if (lp != NULL) l = *lp + lq;

  Top:     // compare p and q w.r.t. monomial ordering
  _prMonCmp(p, q, r, goto Equal, goto Greater , goto Smaller);

  Equal:
  assume(prComp0(p, q, r) == 0);

  n1 = pGetCoeff(p);
  n2 = pGetCoeff(q);
  t = nAdd(n1,n2);
  nDelete(&n1);
  nDelete(&n2);
  prFree1AndAdvance(q, r);
  
  if (nIsZero(t))
  {
    l -= 2;
    nDelete(&t);
    prFree1AndAdvance(p, r);
  }
  else
  {
    l--;
    pSetCoeff0(p,t);
    a = pNext(a) = p;
    pIter(p);
  }
  if (p==NULL) { pNext(a) = q; goto Finish;}
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;
     
  Greater:
  assume(prComp0(p, q, r) == 1);
  a = pNext(a) = p;
  pIter(p);
  if (p==NULL) { pNext(a) = q; goto Finish;}
  goto Top;
    
  Smaller:
  assume(prComp0(p, q, r) == -1);
  a = pNext(a) = q;
  pIter(q);
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;
  

  Finish:
  if (lp != NULL) *lp = l;

  assume(prTest(pNext(&rp), r));
  assume(lp == NULL || pLength(pNext(&rp)) == *lp);
  return pNext(&rp);
}


/***************************************************************
 *
 * General: pr_Mult_m which always works
 * Returns: m*a1, newly allocated from r
 *          if spNoether != NULL, then monoms whioch are smaller
 *          then spNoether are cut
 * Assume:  m is Monom 
 * Const: p, m
 *
 ***************************************************************/
poly  pr_Mult_m_General(poly p,
                       poly m, 
                       poly spNoether,
                       const ring ri)
{
  assume(ri != NULL);
  assume(prTest(p, ri));
  
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);

  if (spNoether == NULL)
  {
    while (p != NULL)
    {
      prAllocMonom(pNext(q), ri);
      q = pNext(q);
      pSetCoeff0(q, nMult(ln, pGetCoeff(p)));
      prMonAdd(q, p, m, ri);
      p = pNext(p);
    }
  }
  else
  {
    poly r;
    while (p != NULL)
    {
      prAllocMonom(r, ri);
      prMonAdd(r, p, m, ri);

      if (prComp0(r, spNoether, ri) == -1)
      {
        omFreeBin(r, ri->PolyBin);
        break;
      }
      q = pNext(q) = r;
      pSetCoeff0(q, nMult(ln, pGetCoeff(p)));
      pIter(p);
    }
  }
  pNext(q) = NULL;

  assume(prTest(pNext(&rp), ri));

  return pNext(&rp);
}

// #define TRACK_MEMORY

/***************************************************************
 *
 * General:  pr_Minus_m_Mult_q which always works
 * Return :  p - m*q, allocated from heap
 * Assume:   p is from heap, m is Monom
 *           *lp == NULL || pLength(p) == *lp && pLenth(q) == lq
 * Destroy:  p
 * Const:    m, q
 *
 ***************************************************************/
poly pr_Minus_m_Mult_q_General (poly p, 
                               poly m,
                               poly q, 
                               poly spNoether,
                               int *lp,  int lq,
                               const ring r)
{
#ifdef TRACK_MEMORY
  mmMarkCurrentUsageState();
  mmMarkCurrentUsageStop();
  poly ptemp = pCopy(p);
  pDelete(&p);
  p = ptemp;
#endif  
  assume(prTest(p, r));
  assume(prTest(q, r));
  assume(lp == NULL || (pLength(p) == *lp && pLength(q) == lq));

  // we are done if q == NULL || m == NULL
  if (q == NULL || m == NULL) return p;
  
  spolyrec rp;
  poly a = &rp,                       // collects the result
       qm = NULL,                     // stores q*m
       c;                             // used for temporary storage


  number tm   = pGetCoeff(m),       // coefficient of m
         tneg = nNeg(nCopy(tm)),    // - (coefficient of m)
         tb,                        // used for tm*coeff(a1)
         tc;                        // used as intermediate number


  int l;
  if (lp != NULL) l = *lp + lq;

  if (p == NULL) goto Finish;       // we are done if p is 0
  
  prAllocMonom(qm, r);
  assume(pGetComp(q) == 0 || pGetComp(m) == 0);
  prMonAdd(qm, q, m, r);
  
  // MAIN LOOP:
  Top:     // compare qm = m*q and p w.r.t. monomial ordering
  _prMonCmp(qm, p, r, goto Equal, goto Greater, goto Smaller);
  
  Equal:   // qm equals p
  assume(prComp0(qm, p,r) == 0);
    tb = nMult(pGetCoeff(q), tm);
    tc = pGetCoeff(p);
    if (!nEqual(tc, tb))
    {
      l--;
      tc = nSub(tc, tb);
      nDelete(&(pGetCoeff(p)));
      pSetCoeff0(p,tc); // adjust coeff of p
      a = pNext(a) = p; // append p to result and advance p
      pIter(p);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      l -= 2;
      nDelete(&tc);
      prFree1AndAdvance(p, r);
    }
    nDelete(&tb);
    pIter(q);
    if (q == NULL || p == NULL) goto Finish; // are we done ?
    // no, so update qm
    assume(pGetComp(q) == 0 || pGetComp(m) == 0);
    prMonAdd(qm, q, m, r);
    goto Top;


  Greater:
    assume(prComp0(qm, p,r) == 1);
      pSetCoeff0(qm,nMult(pGetCoeff(q), tneg));
      a = pNext(a) = qm;       // append qm to result and advance q
      pIter(q);
      if (q == NULL) // are we done?
      {
        qm = NULL;
        goto Finish; 
      }
      // construct new qm 
      prAllocMonom(qm, r);
      assume(pGetComp(q) == 0 || pGetComp(m) == 0);
      prMonAdd(qm, q, m, r);
      goto Top;
    
  Smaller:     
      assume(prComp0(qm, p,r) == -1);
      a = pNext(a) = p;// append p to result and advance p
      pIter(p);
      if (p == NULL) goto Finish;;
      goto Top;
 

  Finish: // q or p is NULL: Clean-up time

   if (q == NULL) // append rest of p to result
     pNext(a) = p;
   else  // append (- m*q) to result
   {
     pSetCoeff0(m, tneg);
     pNext(a) = pr_Mult_m(q, m, spNoether, r);
     pSetCoeff0(m, tm);
   }
   
   nDelete(&tneg);
   if (qm != NULL) omFreeBin(qm, r->PolyBin);
   if (lp != NULL) *lp = l;
   
   assume(prTest(pNext(&rp), r));
   assume(lp == NULL || pLength(pNext(&rp)) == l);
   
#ifdef TRACK_MEMORY
   mmMarkCurrentUsageStart();
   assume(prTest((pNext(&rp), heap));
   mmPrintUnMarkedBlocks();
   mmMarkCurrentUsageStop();
#endif
   
  return pNext(&rp);
} 

#endif // HAVE_P_PROCS
