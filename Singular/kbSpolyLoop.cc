#include "mod2.h"
#include "polys.h"
#include "febase.h"
#include "kutil.h"
#include "kbuckets.h"
#include "modulop.h"
#include "numbers.h"
#include "ipid.h"
#include "spolys0.h"
#include "polys-comp.h"


#ifdef BUCKETS_USE_SPOLYLOOP
extern poly  kbMult_General(poly a1, poly lm);

/***************************************************************
 *
 * General spoly loop which always works
 *
 * assume monom = L(monom)
 * pNext(monom) = result = a2-a1*monom
 * do not destroy a1, but a2
 *
 ***************************************************************/
unsigned int kbSpolyLoop_General 
(poly a1, const unsigned int l1, 
 poly a2, const unsigned int l2, 
 poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                        // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = nNeg(nCopy(tm)), // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                    // used as intermediate number
  unsigned int pl = l1 + l2;
  
  pTest(a1); kbassume(pLength(a1) == (int) l1);
  pTest(a2); kbassume(pLength(a2) == (int) l2);
  
  pSetComp(monom, 0);
  
  if (a2==NULL) goto Finish; // we are done if a2 is 0

  kb_pNew(b);
  b->Order = a1->Order + monom->Order;
  memadd((unsigned long*) &(b->exp[0]),
         (unsigned long*) &(a1->exp[0]),
         (unsigned long*) &(monom->exp[0]),
         pVariables1W);
  pSetCoeff0(b, pGetCoeff(a1));
  
  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    if ((d = pComp0(b, a2))) goto NotEqual; else goto Equal;

  Equal:   // b equals a2
    tb = nMult(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!nEqual(tc, tb))
    {
      pl--;
      tc = nSub(tc, tb);
      nDelete(&(pGetCoeff(a2)));
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      nDelete(&tc);
      kb_pFree1AndAdvance(a2);
      pl -= 2;
    }
    nDelete(&tb);
    pIter(a1);
    if (a1 == NULL || a2 == NULL) goto Finish; // are we done ?
    b->Order = a1->Order + monom->Order;
    memadd((unsigned long*) &(b->exp[0]),
           (unsigned long*) &(a1->exp[0]),
           (unsigned long*) &(monom->exp[0]),
           pVariables1W);
    pSetCoeff0(b, pGetCoeff(a1));
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      pSetCoeff0(b,nMult(pGetCoeff(a1), tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL) // are we done?
      {
        b = NULL;
        goto Finish; 
      }
      kb_pNew(b);
      b->Order = a1->Order + monom->Order;
      memadd((unsigned long*) &(b->exp[0]),
             (unsigned long*) &(a1->exp[0]),
             (unsigned long*) &(monom->exp[0]),
             pVariables1W);
      pSetCoeff0(b, pGetCoeff(a1));
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     pNext(a) = kbMult_General(a1, monom);
   
   nDelete(&tneg);
   if (b != NULL) kb_pFree1(b);
   pTest(pNext(monom));
   kbassume((unsigned int) pLength(pNext(monom)) == pl);
   return pl;
} 

/***************************************************************
 *
 * Fst spoly loops 
 *
 ***************************************************************/
#ifdef FAST_BUCKETS

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

#include "kbSpolyLoop.pin" // the loops and GetspSpolyLoop

#endif // FAST_BUCKETS

kbSpolyLoopProc GetkbSpolyLoopProc(ring r, rOrderType_t rot, BOOLEAN homog)
{
#ifdef FAST_BUCKETS
  Characteristics ch = chGEN;
  OrderingTypes ot = otGEN;
  Homogs hom = homGEN;
  NumWords nw = nwGEN;
  kbSpolyLoopProc spolyloop;
  int Variables1W;

  // set characterisic
  if (r->ch > 1) ch = chMODP;
  
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
  
  // GetspolyLoop   
  spolyloop = GetkbSpolyLoop(ch, ot, hom, nw);
  if (spolyloop != NULL) return  spolyloop;
  // still here? -- no special spolyloop found, return the general loop
#endif // FAST_BUCKETS
  return kbSpolyLoop_General;
}



#endif // BUCKETS_USE_SPOLYLOOP
