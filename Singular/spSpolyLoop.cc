#include "mod2.h"
#include "polys.h"
#include "tok.h"
#include "kstd1.h"
#include "numbers.h"
#include "ipid.h"
#include "febase.h"
#include "spolys0.h"
#include "spolys.h"
#include "modulop.h"
#include "polys-comp.h"
#include "kutil.h"
#include "ring.h"
#include "spSpolyLoop.h"


#ifndef DO_DEEP_PROFILE
// undefine to disable fast spoly loops
// #define FAST_SPOLY_LOOP 
#endif

/***************************************************************
 *
 * General spoly loop which always works
 *
 * assume monom = L(monom)
 * pNext(monom) = result = a2-a1*monom
 * do not destroy a1, but a2
 *
 ***************************************************************/
void spSpolyLoop_General 
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                        // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = nNeg(nCopy(tm)), // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                    // used as intermediate number
  
  if (a2==NULL) goto Finish; // we are done if a2 is 0

  b = pNew();
  pCopyAddFast0(b, a1, monom);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    if ((d = pComp0(b, a2))) goto NotEqual; else goto Equal;;

  Equal:   // b equals a2
    tb = nMult(pGetCoeff(a1), tm);
    tc = pGetCoeff(a2);
    if (!nEqual(tc, tb))
    {
      tc = nSub(tc, tb);
      pSetCoeff(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      nDelete(&tc);
      pFree1(c);
    }
    nDelete(&tb);
    pIter(a1);
    if (a1 == NULL || a2 == NULL) goto Finish; // are we done ?
    pCopyAddFast0(b, a1, monom); // No! So, get new b = a1*monom
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
      b = pNew();
      pCopyAddFast0(b, a1, monom); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     spGMultCopyX(a1, monom, a, tneg, spNoether);
   nDelete(&tneg);
   if (b != NULL) pFree1(b);
} 

/***************************************************************
 *
 * Fst spoly loops 
 *
 ***************************************************************/
#ifdef FAST_SPOLY_LOOP

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
#include "spSpolyLoop.inc" // the loops and spGetSpolyLoop

#endif // FAST_SPOLY_LOOP


spSpolyLoopProc spGetSpolyLoop(ring r, rOrderType_t rot, BOOLEAN homog)
{
#ifdef FAST_SPOLY_LOOP
  Characteristics ch = chGEN;
  OrderingTypes ot = otGEN;
  Homogs hom = homGEN;
  NumWords nw = nwGEN;
  spSpolyLoopProc spolyloop;
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
  
  // GetSpolyLoop   
  spolyloop = spGetSpolyLoop(ch, ot, hom, nw);
  if (spolyloop != NULL) return  spolyloop;
  // still here? -- no special spolyloop found, return the general loop
#endif // FAST_SPOLY_LOOP
  return spSpolyLoop_General;
}



