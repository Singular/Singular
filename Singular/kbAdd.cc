#include "mod2.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kutil.h"
#include "kbuckets.h"
#include "ring.h"
#include "ipid.h"
#include "modulop.h"
#include "numbers.h"
#include "polys-comp.h"


#ifdef HAVE_BUCKETS


/***************************************************************
 *
 * General kbAdd which always works
 *
 * assume pLength(a1) == l1 && pLength(a2) == l2
 * Returns sum of a1 and a2, set length of sum
 * Destroys a1 and a2
 *
 ***************************************************************/

poly kbAdd_General(poly a1, const unsigned int l1, 
                   poly a2, const unsigned int l2, 
                   unsigned int *length)
{
  int c;
  number t, n1, n2;
  unsigned int l = l1 + l2;
  spolyrec rp;
  poly a = &rp;
  pNext(a) = NULL;

  kbassume((pLength(a1) == (int) l1) && (pLength(a2) == (int) l2));
  
  if (a1==NULL) 
  {
    *length = l;
    return a2;
  }
  
  if (a2==NULL) 
  {
    *length = l;
    return a1;
  }

  Top:     // compare a1 and a2 w.r.t. monomial ordering
  register long d;
  if ((d = pComp0(a1, a2))) goto NotEqual; else goto Equal;

  Equal:
  kbassume(pComp0(a1, a2) == 0);
  n1 = pGetCoeff(a1);
  n2 = pGetCoeff(a2);
  t = nAdd(pGetCoeff(a1),pGetCoeff(a2));

  nDelete(&n1);
  nDelete(&n2);
  kb_pFree1AndAdvance(a2);
  
  if (nIsZero(t))
  {
    l -= 2;
    nDelete(&t);
    kb_pFree1AndAdvance(a1);
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
    kbassume(pComp0(a1, a2) == -1);
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
    kbassume(pComp0(a1, a2) == 1);
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
  kbassume(pLength(pNext(&rp)) == (int) l);
  *length = l;
  
  return pNext(&rp);
}

/***************************************************************
 *
 * Fast kbAdd
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

#include "kbAdd.pin" // the procs and GetkbAddProc

#endif // FAST_BUCKETS

kbAddProc GetkbAddProc(ring r, rOrderType_t rot, BOOLEAN homog)
{
#ifdef FAST_BUCKETS
  Characteristics ch = chGEN;
  OrderingTypes ot = otGEN;
  Homogs hom = homGEN;
  NumWords nw = nwGEN;
  kbAddProc kbadd;
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
  
  // GetSpolyLoop   
  kbadd = GetkbAdd(ch, ot, hom, nw);
  if (kbadd != NULL) return  kbadd;
  // still here? -- no special spolyloop found, return the general loop
#endif // FAST_SPOLY_LOOP
  return kbAdd_General;
}

#endif // HAVE_BUCKETS

