/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: pProcs.cc,v 1.9 1999-10-19 17:19:38 obachman Exp $ */
/*
*  ABSTRACT -  Routines for primitive poly arithmetic
*/
#include "mod2.h"
#include "mmemory.h"
#include "polys.h"
#include "polys-comp.h"
#include "pProcs.h"
#include "numbers.h"

// Define to us COMP_MACROS
// #define HAVE_COMP_MACROS

// Define to enable tests in this file 
#define DEBUG_THIS

#if ! (defined(DEBUG_THIS) || (defined(PDEBUG) && PDEBUG > 0))
#undef assume
#define assume(x) 
#endif

#define pFree1AndAdvance(p, heap)               \
do                                              \
{                                               \
  poly __p = p;                                 \
  p = pNext(__p);                               \
  FreeHeap(__p, heap);                          \
}                                               \
while(0)
/***************************************************************
 *
 * General:  p_Mult_n which always works
 * Returns:  p*n
 * Destroys: p
 * Const:    n
 *
 ***************************************************************/

poly p_Mult_n_General(poly p, number n)
{
  poly q = p;
  assume(pHeapTest(p, MM_UNKNOWN_HEAP));
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
 * General:  p_Add_q which always works
 * Returns:  p + q, *lp == pLength(p+q), p+q are from heap
 * Destroys: p, q
 * Assume:   *lp == NULL || pLength(p) == *lp && pLength(q) == q
 *           p, q are from heap
 * 
 ***************************************************************/

poly p_Add_q_General(poly p, poly q, int *lp, int lq,
                     memHeap heap)
{
  assume(heap != NULL && heap != MM_UNKNOWN_HEAP);
  assume(pHeapTest(p, heap));
  assume(pHeapTest(q, heap));
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
  _pMonCmp(p, q, goto Equal, goto Greater , goto Smaller);

  Equal:
  assume(pComp0(p, q) == 0);

  n1 = pGetCoeff(p);
  n2 = pGetCoeff(q);
  t = nAdd(n1,n2);
  nDelete(&n1);
  nDelete(&n2);
  pFree1AndAdvance(q, heap);
  
  if (nIsZero(t))
  {
    l -= 2;
    nDelete(&t);
    pFree1AndAdvance(p, heap);
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
  assume(pComp0(p, q) == 1);
  a = pNext(a) = p;
  pIter(p);
  if (p==NULL) { pNext(a) = q; goto Finish;}
  goto Top;
    
  Smaller:
  assume(pComp0(p, q) == -1);
  a = pNext(a) = q;
  pIter(q);
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;
  

  Finish:
  if (lp != NULL) *lp = l;

  assume(pHeapTest(pNext(&rp), heap));
  assume(lp == NULL || pLength(pNext(&rp)) == *lp);
  return pNext(&rp);
}


/***************************************************************
 *
 * General: p_Mult_m which always works
 * Returns: m*a1, newly allocated from heap
 *          if spNoether != NULL, then monoms whioch are smaller
 *          then spNoether are cut
 * Assume:  m is Monom 
 * Const: p, m
 *
 ***************************************************************/
poly  p_Mult_m_General(poly p,
                       poly m, 
                       poly spNoether,
                       memHeap heap)
{
  assume(heap != NULL && heap != MM_UNKNOWN_HEAP);
  assume(pHeapTest(p, MM_UNKNOWN_HEAP));
  
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp, r;
  number ln = pGetCoeff(m);

  if (spNoether == NULL)
  {
    while (p != NULL)
    {
      pNext(q) = AllocHeap(heap);
      q = pNext(q);
      pSetCoeff0(q, nMult(ln, pGetCoeff(p)));
      pMonAdd(q, p, m);
      p = pNext(p);
    }
  }
  else
  {
    poly r;
    while (p != NULL)
    {
      r = AllocHeap(heap);
      pMonAdd(r, p, m);

      if (pComp0(r, spNoether) == -1)
      {
        FreeHeap(r, heap);
        break;
      }
      q = pNext(q) = r;
      pSetCoeff0(q, nMult(ln, pGetCoeff(p)));
      pIter(p);
    }
  }
  pNext(q) = NULL;

  assume(pHeapTest(pNext(&rp), heap));

  return pNext(&rp);
}

// #define TRACK_MEMORY

/***************************************************************
 *
 * General:  p_Minus_m_Mult_q which always works
 * Return :  p - m*q, allocated from heap
 * Assume:   p is from heap, m is Monom
 *           *lp == NULL || pLength(p) == *lp && pLenth(q) == lq
 * Destroy:  p
 * Const:    m, q
 *
 ***************************************************************/
poly p_Minus_m_Mult_q_General (poly p, 
                               poly m,
                               poly q, 
                               poly spNoether,
                               int *lp,  int lq,
                               memHeap heap)
{
#ifdef TRACK_MEMORY
  mmMarkCurrentUsageState();
  mmMarkCurrentUsageStop();
  poly ptemp = pCopy(p);
  pDelete(&p);
  p = ptemp;
#endif  
  assume(heap != NULL && heap != MM_UNKNOWN_HEAP);
  assume(pHeapTest(p, heap));
  assume(pHeapTest(q, MM_UNKNOWN_HEAP));
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
  
  qm = AllocHeap(heap);
  assume(pGetComp(q) == 0 || pGetComp(m) == 0);
  pMonAdd(qm, q, m);
  
  // MAIN LOOP:
  Top:     // compare qm = m*q and p w.r.t. monomial ordering
  _pMonCmp(qm, p, goto Equal, goto Greater, goto Smaller);
  
  Equal:   // qm equals p
  assume(pComp0(qm, p) == 0);
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
      pFree1AndAdvance(p, heap);
    }
    nDelete(&tb);
    pIter(q);
    if (q == NULL || p == NULL) goto Finish; // are we done ?
    // no, so update qm
    assume(pGetComp(q) == 0 || pGetComp(m) == 0);
    pMonAdd(qm, q, m);
    goto Top;


  Greater:
  assume(pComp0(qm, p) == 1);
      pSetCoeff0(qm,nMult(pGetCoeff(q), tneg));
      a = pNext(a) = qm;       // append qm to result and advance q
      pIter(q);
      if (q == NULL) // are we done?
      {
        qm = NULL;
        goto Finish; 
      }
      // construct new qm 
      qm = AllocHeap(heap);
      assume(pGetComp(q) == 0 || pGetComp(m) == 0);
      pMonAdd(qm, q, m);
      goto Top;
    
  Smaller:     
  assume(pComp0(qm, p) == -1);
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
     pNext(a) = p_Mult_m(q, m, spNoether, heap);
     pSetCoeff0(m, tm);
   }
   
   nDelete(&tneg);
   if (qm != NULL) FreeHeap(qm, heap);
   if (lp != NULL) *lp = l;
   
   assume(pHeapTest(pNext(&rp), heap));
   assume(lp == NULL || pLength(pNext(&rp)) == l);
   
#ifdef TRACK_MEMORY
   mmMarkCurrentUsageStart();
   assume(pHeapTest(pNext(&rp), heap));
   mmPrintUnMarkedBlocks();
   mmMarkCurrentUsageStop();
#endif
   
  return pNext(&rp);
} 

