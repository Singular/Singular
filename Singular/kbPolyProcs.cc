#include "mod2.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kutil.h"
#include "kbuckets.h"
#include "ring.h"
#include "ipid.h"
#include "numbers.h"
#include "kbPolyProcs.h"
#include "modulop.h"
#include "spolys0.h"

/***************************************************************
 *
 * Reduces p1 with p2
 * Assumes p1 != NULL, p2 != NULL, Lm(p1) divides Lm(p2)
 *         pLength(p2) == l2
 * 
 ***************************************************************/
void kbReducePoly(poly *p1, int *l1,
                  poly p2, int l2, 
                  poly spNoether,
                  kbPolyProcs_pt pprocs,
                  memHeap heap)
{
  assume(p2 != NULL && 
         pDivisibleBy(p2,  *p1));
#ifdef HAVE_LENGTH
  assume(pLength(p2) == (int) l2);
  assume(pLength(p1) == (int) l1);
#endif  

  poly a2 = pNext(p2), lm = *p1;
  *p1 = pNext(*p1);
  *l1 = *l1 -1;
  BOOLEAN reset_vec=FALSE;
  
  if (a2==NULL)
  {
    kb_pDelete1(lm, heap);
    return;
  }

  if (! nIsOne(pGetCoeff(p2)))
  {
    number an = pGetCoeff(p2), bn = pGetCoeff(lm);
    int ct = spCheckCoeff(&an, &bn);
    pSetCoeff(lm, bn);
    if ((ct == 0) || (ct == 2)) kb_n_Mult_p_General(an, *p1);
    nDelete(&an);
  }

  
  if (pGetComp(p2) != pGetComp(lm))
  {
    pSetCompP(p2, pGetComp(lm));
    reset_vec = TRUE;
  }

  pMonSubFast(lm, p2);
  assume(pGetComp(lm) == 0);

  kb_p_Minus_m_Mult_q_General(p1, l1, lm, a2, l2-1, spNoether);
  
  kb_pDelete1(lm, heap);
  if (reset_vec) spModuleToPoly(p2);
}

/***************************************************************
 *
 * Creates S-Poly of p1 and p2
 * 
 *
 ***************************************************************/
void kbCreateSpoly(poly *sp, int *lsp, 
                   poly p1, int l1,
                   poly p2, int l2,
                   poly spNoether,
                   kbPolyProcs_pt pprocs,
                   memHeap heap)
{
#ifdef HAVE_LENGTH
  assume(p1 != NULL && (l1 <= 0 || pLength(p1) == l1));
  assume(p2 != NULL && (l2 <= 0 || pLength(p2) == l2));
#endif
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif  
  
  poly a1 = pNext(p1), a2 = pNext(p2);
  number lc1 = pGetCoeff(p1), lc2 = pGetCoeff(p2);
  poly m1, m2;
  int co=0, ct = spCheckCoeff(&lc1, &lc2);
  Exponent_t x;

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

  // get m1 = LCM(LM(p1), LM(p2))/LM(p1)
  //     m2 = LCM(LM(p1), LM(p2))/LM(p2)
  m1 = pInit();
  m2 = pInit();
  for (int i = pVariables; i; i--)
  {
    x = pGetExpDiff(p1, p2, i);
    if (x > 0)
    {
      pSetExp(m2,i,x);
      pSetExp(m1,i,0);
    }
    else
    {
      pSetExp(m1,i,-x);
      pSetExp(m2,i,0);
    }
  }
  pSetm(m1);
  pSetm(m2);           // now we have m1 * LM(p1) == m2 * LM(p2)
  pSetCoeff0(m1, lc2);
  pSetCoeff0(m2, lc1); // and now, m1 * LT(p1) == m2 * LT(p2)

  // get m2 * a2
  a2 = kb_p_Mult_m_General(a2, m2);
#ifdef HAVE_LENGTH
  if (l1 <= 0) l1 = pLength(a1); else l1--;
  if (l2 <= 0) l2 = pLength(a2); else l2--;
#endif
  // and, finally, the spoly
  kb_p_Minus_m_Mult_q_General(&a2, &l2, m1, a1, l1);
  
  // Clean-up time
  pDelete1(&m1);
  pDelete1(&m2);
  
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
  *sp = a2;
  *lsp = l2;
  pTest(*sp);
}

/***************************************************************
 *
 * General kb_n_Mult_p which always works
 * Realizes p = n*p
 *
 ***************************************************************/

void kb_n_Mult_p_General(number n, poly p)
{
  while (p != NULL)
  {
    number nc = pGetCoeff(p);
    pSetCoeff0(p, nMult(n, nc));
    nDelete(&nc);
    pIter(p);
  }
}

/***************************************************************
 *
 * General kb_p_Add_q which always works
 *
 * assume pLength(*p) == *lp && pLength(*q) == *q
 * *p and *q from heap 
 * Destroys *p and *q
 * On return, *p == *p + *q, *q == NULL, 
 *            *lp == pLength(*p + *q), *lq == 0
 *
 ***************************************************************/

void kb_p_Add_q_General(poly *p, int *lp,
                        poly *q, int *lq, 
                        memHeap heap)
{
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif
#ifdef HAVE_LENGTH
  assume(pLength(*p) == *lp && pLength(*q) == *lq);
#endif
  
  number t, n1, n2;
  unsigned int l = *lp + *lq;
  spolyrec rp;
  poly a = &rp, a1 = *p, a2 = *q;

  if (a2 == NULL) return;

  *q = NULL;
  *lq = 0;
  
  if (a1 == NULL) 
  {
    *p = a2;
    *lp = l;
    return;
  }

  Top:     // compare a1 and a2 w.r.t. monomial ordering
//  register long d;
  long d;
#ifdef HAVE_COMP_MACROS
  _pMonComp(a1, a2, d, goto NotEqual, goto Equal);
#else
  if ((d = pComp0(a1, a2))) goto NotEqual; else goto Equal;
#endif

  Equal:
  assume(pComp0(a1, a2) == 0);

  n1 = pGetCoeff(a1);
  n2 = pGetCoeff(a2);
  t = nAdd(n1,n2);
  nDelete(&n1);
  nDelete(&n2);
  kb_pFree1AndAdvance(a2, heap);
  
  if (nIsZero(t))
  {
    l -= 2;
    nDelete(&t);
    kb_pFree1AndAdvance(a1, heap);
  }
  else
  {
    l--;
    pSetCoeff0(a1,t);
    a = pNext(a) = a1;
    pIter(a1);
  }
  if (a1==NULL)
  {
    pNext(a) = a2;
    goto Finish;
  }
  else if (a2==NULL)
  {
    pNext(a) = a1;
    goto Finish;
  }
  goto Top;
     
  NotEqual:
  if (d < 0)
  {
    assume(pComp0(a1, a2) == -1);
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pNext(a) = a1;
      goto Finish;
    }
  }
  else 
  {
    assume(pComp0(a1, a2) == 1);
    a = pNext(a) = a1;
    pIter(a1);
    if (a1==NULL)
    {
      pNext(a) = a2;
      goto Finish;
    }
  }
  goto Top;
  

  Finish:
#ifdef HAVE_LENGTH  
  assume(pLength(pNext(&rp)) == (int) l);
#endif
  *lp = l;
  *p  = pNext(&rp);
}

/***************************************************************
 *
 * General kb_p_Mult_m which always works
 *
 * assume pLength(m) == 1
 * Returns m*a1, monoms are allocated from heap
 *
 ***************************************************************/
  
poly  kb_p_Mult_m_General(poly p,
                          poly m, 
                          poly spNoether,
                          memHeap heap)
{
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif
  assume(pGetComp(m) == 0);

  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp;
  number ln = pGetCoeff(m);

  while (p != NULL)
  {
    kb_pNew(pNext(q), heap);
    q = pNext(q);

    pSetCoeff0(q, nMult(ln, pGetCoeff(p)));

    memaddW((unsigned long*) &(q->exp.l[0]),
            (unsigned long*) &(p->exp.l[0]),
            (unsigned long*) &(m->exp.l[0]),
            currRing->ExpLSize);
    
    p = pNext(p);
  }
  pNext(q) = NULL;
  return rp.next;
}

/***************************************************************
 *
 * General spoly loop which always works
 *
 * assume(pLength(*pp) == *lpp)
 * assume(pLength(q) == lq)
 * assume(pLength(m) == NULL)
 * assume: Monoms of *pp are from heap
 *
 * Destroys *pp
 * Does not touch q
 * On return *pp == *pp - m*q
 *           *lpp == pLength(*pp - m*q)
 * New monoms are allocated from heap
 *
 ***************************************************************/
void kb_p_Minus_m_Mult_q_General (poly *pp, int *lpp, 
                                  poly m,
                                  poly q, int lq,
                                  poly spNoether,
                                  kb_p_Mult_m_Proc kb_p_Mult_m,
                                  memHeap heap)
{ 
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif
#ifdef HAVE_LENGTH
  assume(pLength(q) == lq);
  assume(pLength(*pp) == *lpp);
#endif
  assume(pGetComp(m) == 0);
  pTest(*pp);
  pTest(q);
  // we are done if q == NULL
  if (q == NULL || m == NULL) return;
  
  poly a = m,                         // collects the result
       qm = NULL,                     // stores q*m
       c,                             // used for temporary storage
       p = *pp;

  number tm   = pGetCoeff(m),       // coefficient of m
         tneg = nNeg(nCopy(tm)),    // - (coefficient of m)
         tb,                        // used for tm*coeff(a1)
         tc;                        // used as intermediate number

  unsigned int lp = *lpp + lq;

  if (p == NULL) goto Finish; // we are done if p is 0

  kb_pNew(qm, heap);
  memaddW((unsigned long*) &(qm->exp.l[0]),
          (unsigned long*) &(q->exp.l[0]),
          (unsigned long*) &(m->exp.l[0]),
          currRing->ExpLSize);
  
  // MAIN LOOP:
  Top:     // compare qm = m*q and p w.r.t. monomial ordering
//    register long d;
  long d;
#ifdef HAVE_COMP_MACROS
    _pMonComp(qm, p, d, goto NotEqual, goto Equal);
#else
    if ((d = pComp0(qm, p))) goto NotEqual; else goto Equal;
#endif

  Equal:   // qm equals p
    tb = nMult(pGetCoeff(q), tm);
    tc = pGetCoeff(p);
    if (!nEqual(tc, tb))
    {
      lp--;
      tc = nSub(tc, tb);
      nDelete(&(pGetCoeff(p)));
      pSetCoeff0(p,tc); // adjust coeff of p
      a = pNext(a) = p; // append p to result and advance p
      pIter(p);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      lp -= 2;
      nDelete(&tc);
      kb_pFree1AndAdvance(p, heap);
    }
    nDelete(&tb);
    pIter(q);
    if (q == NULL || p == NULL) goto Finish; // are we done ?
    // no, so update qm
    memaddW((unsigned long*) &(qm->exp.l[0]),
            (unsigned long*) &(q->exp.l[0]),
            (unsigned long*) &(m->exp.l[0]),
            currRing->ExpLSize);
    goto Top;

  NotEqual:     // qm != p 
    if (d < 0)  // qm < p: 
    {
      a = pNext(a) = p;// append p to result and advance p
      pIter(p);
      if (p == NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., qm > p
    {
      pSetCoeff0(qm,nMult(pGetCoeff(q), tneg));
      a = pNext(a) = qm;       // append qm to result and advance q
      pIter(q);
      if (q == NULL) // are we done?
      {
        qm = NULL;
        goto Finish; 
      }
      // construct new qm 
      kb_pNew(qm, heap);
      memaddW((unsigned long*) &(qm->exp.l[0]),
              (unsigned long*) &(q->exp.l[0]),
              (unsigned long*) &(m->exp.l[0]),
              currRing->ExpLSize);
      goto Top;
    }
 
 Finish: // q or p is NULL: Clean-up time
   if (q == NULL) // append rest of p to result
     pNext(a) = p;
   else  // append (- q*m) to result
   {
     pSetCoeff0(m, tneg);
     pNext(a) = kb_p_Mult_m_General(q, m, spNoether, heap);
     pSetCoeff0(m, tm);
   }
   
   nDelete(&tneg);
   if (qm != NULL) kb_pFree1(qm, heap);

   *pp = pNext(m);
   *lpp = lp;
   pNext(m) = NULL;

#ifdef HAVE_LENGTH   
   assume(pLength(*pp) == *lpp);
#endif
   pTest(*pp);
} 


/***************************************************************
 *
 * fast poly proc business
 *
 ***************************************************************/

#ifdef FAST_POLY_PROCS
#define NonZeroA(d, multiplier, actionE)        \
{                                               \
  d ^= multiplier;                              \
  actionE;                                      \
}                                               \

#define NonZeroTestA(d, multiplier, actionE)    \
do                                              \
{                                               \
  if (d)                                        \
  {                                             \
    d ^= multiplier;                            \
    actionE;                                    \
  }                                             \
}                                               \
while(0)

#include "kbPolyProcs.pin"
#endif

void kbSetPolyProcs(kbPolyProcs_pt pprocs,
                    ring r, rOrderType_t rot, BOOLEAN homog)
{
  assume(pprocs != NULL);
#ifdef FAST_POLY_PROCS
  Characteristics ch = chGEN;
  OrderingTypes ot = otGEN;
  Homogs hom = homGEN;
  NumWords nw = nwGEN;
  int Variables1W;

  // set characterisic
  if (rField_is_Zp(r)) ch = chMODP;
  
  // set Ordering Type
  switch (rot)
  {   
      case rOrderType_Exp:
        ot = otEXP;
        break;
    
      case rOrderType_CompExp:
        ot = otCOMPEXP;
        break;
    
      case rOrderType_ExpComp:
        ot = otEXPCOMP;
        break;

      case rOrderType_Syz2dpc:
        ot = otSYZDPC;
        break;
        
      case rOrderType_ExpNoComp:
        ot = otExpNoComp;
        
      default:
        ot = otGEN;
        break;
  }
  
  // set homogenous
  if (homog) hom = homYES;
  
  // set NumWords
  if ((((r->N+1)*sizeof(Exponent_t)) % sizeof(void*)) == 0)
    Variables1W = (r->N+1)*sizeof(Exponent_t) / sizeof(void*);
  else
    Variables1W = ((r->N+1)*sizeof(Exponent_t) / sizeof(void*)) + 1;
  if (Variables1W > 2)
  {
    if (Variables1W & 1) nw = nwODD;
    else nw = nwEVEN;
  }
  else 
  {
    if (Variables1W == 2) nw = nwTWO;
    else nw = nwONE;
  }
  
  // Get the nPoly Procs 
  pprocs->p_Add_q = Getkb_p_Add_q(ch, ot, hom, nw);
  if (pprocs->p_Add_q == NULL)
    pprocs->p_Add_q = kb_p_Add_q_General;

  pprocs->p_Mult_m = Getkb_p_Mult_m(ch, ot, hom, nw);
  if (pprocs->p_Mult_m == NULL)
    pprocs->p_Mult_m = kb_p_Mult_m_General;

  pprocs->p_Minus_m_Mult_q = Getkb_p_Minus_m_Mult_q(ch, ot, hom, nw);
  if (pprocs->p_Minus_m_Mult_q == NULL)
    pprocs->p_Minus_m_Mult_q = kb_p_Minus_m_Mult_q_General;

  pprocs->n_Mult_p = Getkb_n_Mult_p(ch, ot, hom, nw);
  if (pprocs->n_Mult_p == NULL)
    pprocs->n_Mult_p = kb_n_Mult_p_General;
#else // FAST_POLYPROCS
  pprocs->p_Add_q = kb_p_Add_q_General;
  pprocs->p_Mult_m = kb_p_Mult_m_General;
  pprocs->p_Minus_m_Mult_q = kb_p_Minus_m_Mult_q_General;
  pprocs->n_Mult_p = kb_n_Mult_p_General;
#endif  
}
