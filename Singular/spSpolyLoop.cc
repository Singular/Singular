#include "mod2.h"
#include "polys.h"

#ifdef COMP_FAST
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

// used in routines where component is not handled as a special case
static int LoopVariablesW;

static void spPSpolyLoop_1(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_1(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_1_c(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2_c(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_c(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_1_c(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_1(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_2(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_2i(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_2i_1(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_1_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_1_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_2_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_2i_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_c_2i_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_1_homog(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2_homog(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_homog(poly p1, poly p2, poly m, poly spNoether);
static void spPSpolyLoop_2i_1_homog(poly p1, poly p2, poly m, poly spNoether);
#ifdef TEST_MAC_ORDER
static void spLoop_mac_1(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_1(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_1_c(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_1_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2_c(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_c(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_1_c(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_2i_1_c_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_1(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_1_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_2(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_2_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_2i(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_2i_Syz(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_2i_1(poly p1, poly p2, poly m, poly spNoether);
static void spLoop_mac_c_2i_1_Syz(poly p1, poly p2, poly m, poly spNoether);
#endif

extern int maxBound;

/***************************************************************
 *
 * Sets the used spoly loop in the strategy
 *
 ***************************************************************/
#ifndef DO_DEEP_PROFILE
#define FAST_SPOLY_LOOP
#endif

spSpolyLoopProc spSetPSpolyLoop(ring r, int syzComp, int ak, BOOLEAN homog)
{
#ifdef FAST_SPOLY_LOOP
  // check for oredering with one real block only
  if ((r->order[0] == ringorder_unspec)  ||
      (r->order[2] == 0 &&
       r->order[0] != ringorder_M && r->order[1] != ringorder_M))
  {
    // check whether components play a role
    if (ak == 0 && syzComp == 0)
    {
      int or;
      if (r->order[0] != ringorder_C && r->order[0] != ringorder_c)
        or = r->order[0];
      else
        or = r->order[1];

      LoopVariablesW = pVariablesW;
      if (homog || or == ringorder_lp || or == ringorder_ls)
      {
        // wee do not need to compare the order field for the
        // omogenous case and for non-degree orderings
        if (LoopVariablesW == 1)
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_1_homog;
            //else
              return spLoop_mac_1;
          }
          #endif
          return spPSpolyLoop_1_homog;
        }
        else if (LoopVariablesW == 2)
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_2_homog;
            //else
              return spLoop_mac_2;
          }
          #endif
          return spPSpolyLoop_2_homog;
        }
        else if (LoopVariablesW & 1)
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_2i_1_homog;
            //else
              return spLoop_mac_2i_1;
          }
          #endif
          return spPSpolyLoop_2i_1_homog;
        }
        else
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_2i_homog;
            //else
              return spLoop_mac_2i;
          }
          #endif
          return spPSpolyLoop_2i_homog;
        }
      }
      else
      {
        // no, components are not involved
        if (LoopVariablesW == 1)
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_1_homog;
            //else
              return spLoop_mac_1;
          }
          #endif
          return spPSpolyLoop_1;
        }
        else if (LoopVariablesW == 2)
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_2_homog;
            //else
              return spLoop_mac_2;
          }
          #endif
          return spPSpolyLoop_2;
        }
        else if (LoopVariablesW & 1)
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_2i_1_homog;
            //else
              return spLoop_mac_2i_1;
          }
          #endif
          return spPSpolyLoop_2i_1;
        }
        else
        {
          #ifdef TEST_MAC_ORDER
          if (or==ringorder_dp)
          {
            //if (homog)
            //  return spLoop_mac_2i_homog;
            //else
              return spLoop_mac_2i;
          }
          #endif
          return spPSpolyLoop_2i;
        }
      }
    }
    else
    {
      // components are invloved
      // check for priority to exponents
      LoopVariablesW = pVariables1W;
      if ((r->order[1]==ringorder_c)||(r->order[1]==ringorder_C) ||
          (r->order[0] == ringorder_unspec))
      {
        switch(r->order[0])
        {
            case ringorder_dp:
              #ifdef TEST_MAC_ORDER
              //LoopVariablesW = pVariables1W;
              if (r->order[1] == ringorder_C)
              {
                if (LoopVariablesW == 1)
                  return (syzComp ?
                          spLoop_mac_1_c_Syz :
                          spLoop_mac_1_c);
                else if (LoopVariablesW == 2)
                  return (syzComp ?
                          spLoop_mac_2_c_Syz :
                          spLoop_mac_2_c);
                else if (LoopVariablesW & 1)
                  return (syzComp ?
                          spLoop_mac_2i_1_c_Syz :
                          spLoop_mac_2i_1_c);
                else
                  return (syzComp ?
                          spLoop_mac_2i_c_Syz :
                          spLoop_mac_2i_c);
              }
              else
              {
                if (LoopVariablesW == 1)
                  return (syzComp ?
                          spLoop_mac_1_Syz :
                          spLoop_mac_1);
                else if (LoopVariablesW == 2)
                  return (syzComp ?
                          spLoop_mac_2_Syz :
                          spLoop_mac_2);
                else if (LoopVariablesW & 1)
                  return (syzComp ?
                          spLoop_mac_2i_1_Syz :
                          spLoop_mac_2i_1);
                else
                  return (syzComp ?
                          spLoop_mac_2i_Syz :
                          spLoop_mac_2i);
              }
              #endif
            case ringorder_wp:
            case ringorder_ds:
            case ringorder_ws:
            case ringorder_ls:
            case ringorder_unspec:

              if (r->order[1] == ringorder_C ||
                  r->order[0] == ringorder_unspec)
              {
                if (LoopVariablesW == 1)
                  return (syzComp ?
                          spPSpolyLoop_1_c_Syz :
                          spPSpolyLoop_1_c);
                else if (LoopVariablesW == 2)
                  return (syzComp ?
                          spPSpolyLoop_2_c_Syz :
                          spPSpolyLoop_2_c);
                else if (LoopVariablesW & 1)
                  return (syzComp ?
                          spPSpolyLoop_2i_1_c_Syz :
                          spPSpolyLoop_2i_1_c);
                else
                  return (syzComp ?
                          spPSpolyLoop_2i_c_Syz :
                          spPSpolyLoop_2i_c);
              }
              else
              {
                //LoopVariablesW = pVariables1W;
                if (LoopVariablesW == 1)
                  return (syzComp ?
                          spPSpolyLoop_1_Syz :
                          spPSpolyLoop_1);
                else if (LoopVariablesW == 2)
                  return (syzComp ?
                          spPSpolyLoop_2_Syz :
                          spPSpolyLoop_2);
                else if (LoopVariablesW & 1)
                  return (syzComp ?
                          spPSpolyLoop_2i_1_Syz :
                          spPSpolyLoop_2i_1);
                else
                  return (syzComp ?
                          spPSpolyLoop_2i_Syz :
                          spPSpolyLoop_2i);
              }
#ifdef PDEBUG
            case ringorder_lp:
            case ringorder_Dp:
            case ringorder_Wp:
            case ringorder_Ds:
            case ringorder_Ws:
#else
            default:
#endif
              if (r->order[1] == ringorder_c)
              {
                if (LoopVariablesW == 1)
                  return (syzComp ?
                          spPSpolyLoop_1_c_Syz :
                          spPSpolyLoop_1_c);
                else if (LoopVariablesW == 2)
                  return (syzComp ?
                          spPSpolyLoop_2_c_Syz :
                          spPSpolyLoop_2_c);
                else if (LoopVariablesW & 1)
                  return (syzComp ?
                          spPSpolyLoop_2i_1_c_Syz :
                          spPSpolyLoop_2i_1_c);
                else
                  return (syzComp ?
                          spPSpolyLoop_2i_c_Syz :
                          spPSpolyLoop_2i_c);
              }
              else
              {
                //LoopVariablesW = pVariables1W;
                if (LoopVariablesW == 1)
                  return (syzComp ?
                          spPSpolyLoop_1_Syz : spPSpolyLoop_1);
                else if (LoopVariablesW == 2)
                  return (syzComp ?
                          spPSpolyLoop_2_Syz : spPSpolyLoop_2);
                else if (LoopVariablesW & 1)
                  return (syzComp ?
                          spPSpolyLoop_2i_1_Syz :
                          spPSpolyLoop_2i_1);
                else
                  return (syzComp ?
                          spPSpolyLoop_2i_Syz :
                          spPSpolyLoop_2i);
              }
#ifdef PDEBUG
            default:
              Werror("wrong internal ordering:%d at %s, l:%d\n",r->order[0],__FILE__,__LINE__);
              return NULL;
#endif
        }
      }
      else if (r->order[0] == ringorder_c || r->order[0] == ringorder_C)
      {
        if (LoopVariablesW == 1)
        {
          #ifdef TEST_MAC_ORDER
          if (r->order[1] == ringorder_dp)
          {
            return (syzComp ?
                  spLoop_mac_c_1_Syz : spLoop_mac_c_1);
          }
          #endif
          return (syzComp ?
                  spPSpolyLoop_c_1_Syz : spPSpolyLoop_c_1);
        }
        else if (LoopVariablesW == 2)
        {
          #ifdef TEST_MAC_ORDER
          if (r->order[1] == ringorder_dp)
          {
            return (syzComp ?
                  spLoop_mac_c_2_Syz : spLoop_mac_c_2);
          }
          #endif
          return (syzComp ?
                  spPSpolyLoop_c_2_Syz : spPSpolyLoop_c_2);
        }
        else if (LoopVariablesW & 1)
        {
          #ifdef TEST_MAC_ORDER
          if (r->order[1] == ringorder_dp)
          {
            return (syzComp ?
                  spLoop_mac_c_2i_1_Syz : spLoop_mac_c_2i_1);
          }
          #endif
          return (syzComp ?
                  spPSpolyLoop_c_2i_1_Syz : spPSpolyLoop_c_2i_1);
        }
        else
        {
          #ifdef TEST_MAC_ORDER
          if (r->order[1] == ringorder_dp)
          {
            return (syzComp ?
                  spLoop_mac_c_2i_Syz : spLoop_mac_c_2i);
          }
          #endif
          return (syzComp ?
                  spPSpolyLoop_c_2i_Syz : spPSpolyLoop_c_2i);
        }
      }
      else
        return spPSpolyLoop_General;
    }
  }
  else
#endif // FAST_SPOLY_LOOP
    return spPSpolyLoop_General;
}

spSpolyLoopProc spSetSpolyLoop(ring r, int syzComp, int ak, BOOLEAN homog)
{
  if (r->ch > 0)
  {
    return spSetPSpolyLoop(r, syzComp, ak, homog);
  }
  else return NULL;
}

/***************************************************************
 *
 * The general case
 *
 ***************************************************************/
void spPSpolyLoop_General(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;

  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);
  loop
  {
    c = pComp0(b, a2);
    if (c == -1)
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        spMultCopyX(a1, m, a, tneg,spNoether);
        return;
      }
    }
    else if (c == 1)
    {
      pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        pCopyAddFast(b, a1, m);
      }
      else
      {
        pNext(a) = a2;
        return;
      }
    }
    else
    {
      tb = npMultM(pGetCoeff(a1),tm);
      if (!npEqualM(pGetCoeff(a2),tb))
      {
        pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
        spMultCopyX(a1, m, a, tneg,spNoether);
        return;
      }
      pCopyAddFast(b, a1, m);
    }
  }
}

/***************************************************************
 *
 * comp != 0 cases
 *
 ***************************************************************/
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

static void spPSpolyLoop_1(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_1(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_1_c(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_1_c(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

   NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_1(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetComp(a2) - pGetComp(b);
  NonZeroTestA(d, pComponentOrder, goto NotEqual);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_1(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
  a = pNext(a) = a2;
  pIter(a2);
  if (a2==NULL)
  {
    pFree1(b);
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  goto Top;
  }


  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

   NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2_c(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2_c(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

   NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_2(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetComp(a2) - pGetComp(b);
  NonZeroTestA(d, pComponentOrder, goto NotEqual);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
  a = pNext(a) = a2;
  pIter(a2);
  if (a2==NULL)
  {
    pFree1(b);
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  goto Top;
  }


  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i(b, a2, LoopVariablesW, d,
             NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

   NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_c(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_c(b, a2, pVariables1W, d,
               NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

   NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_2i(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetComp(a2) - pGetComp(b);
  NonZeroTestA(d, pComponentOrder, goto NotEqual);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i(b, a2, pVariablesW,
             d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
  a = pNext(a) = a2;
  pIter(a2);
  if (a2==NULL)
  {
    pFree1(b);
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  goto Top;
  }


  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_1(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_1(b, a2, LoopVariablesW, d,
               NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_1_c(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_1_c(b, a2, pVariables1W,
                 d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

   NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_2i_1(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  d = pGetComp(a2) - pGetComp(b);
  NonZeroTestA(d, pComponentOrder, goto NotEqual);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_1(b, a2, pVariablesW,
               d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
  a = pNext(a) = a2;
  pIter(a2);
  if (a2==NULL)
  {
    pFree1(b);
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  goto Top;
  }


  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}


/***************************************************************
 *
 * Syzygies orderings
 *
 ***************************************************************/
#define _SyzComp(p1, p2, actionG, actionS)      \
do                                              \
{                                               \
  if (pGetComp(p1) <= maxBound)                 \
  {                                             \
    if (pGetComp(p2) > maxBound) actionG;       \
  }                                             \
  else if (pGetComp(p2)<=maxBound) actionS;     \
}                                               \
while(0)

#define _SyzComp_d(p1, p2, d, actionG, actionS) \
do                                              \
{                                               \
  Exponent_t c1 = pGetComp(p1);                 \
  Exponent_t c2 = pGetComp(p2);                 \
  d = c2 -c1;                                   \
  if (d)                                        \
  {                                             \
    if (c1 <= maxBound)                         \
    {                                           \
      if (c2 > maxBound) actionG;               \
    }                                           \
    else                                        \
    {                                           \
      if (c2 <= maxBound) actionS;              \
    }                                           \
    d ^= pComponentOrder;                       \
    if (d < 0) actionS;                         \
    actionG;                                    \
  }                                             \
}                                               \
while(0)

static void spPSpolyLoop_1_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;


  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_1(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
     }
     goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;

}

static void spPSpolyLoop_1_c_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_1_c(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;
  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
     }
     goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;

}

static void spPSpolyLoop_c_1_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;


  _SyzComp_d(b, a2, d, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_1(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
     }
     goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2_c_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2_c(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_2_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp_d(b, a2, d, goto Greater, goto Smaller);

  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
     }
     goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i(b, a2, LoopVariablesW, d,
             NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_c_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_c(b, a2, pVariables1W, d,
               NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_2i_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp_d(b, a2, d, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i(b, a2, pVariablesW,
             d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
     }
     goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_1_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b, a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_1(b, a2, LoopVariablesW, d,
             NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_1_c_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp(b,a2, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_1_c(b, a2, pVariables1W, d,
               NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_c_2i_1_Syz(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long d;

  _SyzComp_d(b, a2, d, goto Greater, goto Smaller);
  d = pGetOrder(b) - pGetOrder(a2);
  NonZeroTestA(d, pOrdSgn, goto NotEqual);
  _pMonCmp_2i_1(b, a2, pVariablesW,
             d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    Smaller:
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >=  0
  Greater:
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

/***************************************************************
 *
 * Comp == 0, homogenous case (i.e. no order comparisons)
 *
 ***************************************************************/

static void spPSpolyLoop_1_homog(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast1(b, a1, m);

  Top:
  register long d;

  _pMonCmp_1(b, a2, d, goto NotEqual, goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast1(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  d ^= pLexSgn;
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast1(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2_homog(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast1(b, a1, m);

  Top:
  register long d;

  _pMonCmp_2(b, a2, d, goto NotEqual, goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast1(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  d ^= pLexSgn;
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast1(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_homog(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast1(b, a1, m);

  Top:
  register long d;

  _pMonCmp_2i(b, a2, pVariablesW, d,
             goto NotEqual, goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast1(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  d ^= pLexSgn;
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast1(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

static void spPSpolyLoop_2i_1_homog(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;


  tneg = npNegM(tm);
  if (a2==NULL)
  {
    spMultCopyX(a1, m, m, tneg,spNoether);
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast1(b, a1, m);

  Top:
  register long d;
  _pMonCmp_2i_1(b, a2, pVariablesW, d, goto NotEqual, goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    spMultCopyX(a1, m, a, tneg,spNoether);
    return;
  }
  pCopyAddFast1(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  d ^= pLexSgn;
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      spMultCopyX(a1, m, a, tneg,spNoether);
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast1(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    return;
  }
  goto Top;
}

/***************************************************************
 *
 * The Macaulay-order case
 *
 ***************************************************************/
#ifdef TEST_MAC_ORDER
/*2
* assume m = L(m) and Lc(m) = exp
* pNext(n) = result = p*m
* do not destroy p
*/
inline void bMultCopyX(poly p, poly m, poly n, number exp)
{
#ifdef PDEBUG
  poly pp=n;
#endif
  poly a = n;
  do
  {
    pNext(a) = pNew();
    a = pNext(a);
    spMemcpy(a,p);
    pbMonAddFast(a,m);
    pSetCoeff0(a,npMultM(pGetCoeff(p),exp));
    pIter(p);
  }
  while (p!=NULL);
  pNext(a) = NULL;
  pTest(pp);
}

/*2
* assume m = L(m) and Lc(m) = exp
* pNext(n) = result = p*m
* do not destroy p
*/
inline void bMultCopyX0(poly p, poly m, poly n, number exp)
{
  poly a = n;
  do
  {
    pNext(a) = pNew();
    a = pNext(a);
    spMemcpy(a,p);
    pbMonAddFast0(a,m);
    pSetCoeff0(a,npMultM(pGetCoeff(p),exp));
    pIter(p);
  }
  while (p!=NULL);
  pNext(a) = NULL;
}


// no components involved
void spLoop_mac_1(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;

  tneg = npNegM(tm);
  if (a2==NULL)
  {
    //bMultCopyX(a1, m, m, tneg);
    spMultCopyX(a1, m, m, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long long d;


  if ((pGetOrder(b)<0) && (pGetOrder(a2)<0)) goto Top0;

  //d = (long long)pGetOrder(b) - (long long)pGetOrder(a2);
  //NonZeroTestA(d, 1 /*pOrdSgn*/, goto NotEqual);
  if (pGetOrder(b) > pGetOrder(a2)) { d=1; goto NotEqual; }
  else if (pGetOrder(b) < pGetOrder(a2)) { d=-1; goto NotEqual; }

  // now pGetOrder(b)==pGetOrder(a2):
  _pMonCmp_1(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    pTest(pNext(m));
    return;
  }
  if (a2==NULL)
  {
    pFree1(b);
    //bMultCopyX(a1, m, a, tneg);
    spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      //bMultCopyX(a1, m, a, tneg);
      spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    pTest(pNext(m));
    return;
  }
  goto Top;


  // below degree limit =================================================
  Top0:
  if (pGetOrder(b) > pGetOrder(a2))      { goto C_1_0; }
  else if (pGetOrder(b) < pGetOrder(a2)) { goto C_M1_0; }


  //Equal0:
    {
      tb = npMultM(pGetCoeff(a1),tm);
      if (!npEqualM(pGetCoeff(a2),tb))
      {
        pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
        bMultCopyX(a1, m, a, tneg);
        return;
      }
      pbCopyAddFast0(b, a1, m);
    }
    goto Top0;

  C_M1_0:  // b < a2
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        bMultCopyX(a1, m, a, tneg);
        return;
      }
    }
    goto Top0;

  C_1_0:  // b >a2
    {
      pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        pbCopyAddFast0(b, a1, m);
      }
      else
      {
        pNext(a) = a2;
        return;
      }
    }
  goto Top0;
}
void spLoop_mac_2(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;

  tneg = npNegM(tm);
  if (a2==NULL)
  {
    bMultCopyX(a1, m, m, tneg);
    //spMultCopyX(a1, m, m, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long long d;


  if ((pGetOrder(b)<0) && (pGetOrder(a2)<0)) goto Top0;

  //d = (long long)pGetOrder(b) - (long long)pGetOrder(a2);
  //NonZeroTestA(d, 1 /*pOrdSgn*/, goto NotEqual);
  if (pGetOrder(b) > pGetOrder(a2)) { d=1; goto NotEqual; }
  else if (pGetOrder(b) < pGetOrder(a2)) { d=-1; goto NotEqual; }

  // now pGetOrder(b)==pGetOrder(a2):
  _pMonCmp_2(b, a2, d, NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    pTest(pNext(m));
    return;
  }
  if (a2==NULL)
  {
    pFree1(b);
    bMultCopyX(a1, m, a, tneg);
    //spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      bMultCopyX(a1, m, a, tneg);
      //spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    pTest(pNext(m));
    return;
  }
  goto Top;


  // below degree limit =================================================
  Top0:
  if (pGetOrder(b) > pGetOrder(a2))      { goto C_1_0; }
  else if (pGetOrder(b) < pGetOrder(a2)) { goto C_M1_0; }


  //Equal0:
    {
      tb = npMultM(pGetCoeff(a1),tm);
      if (!npEqualM(pGetCoeff(a2),tb))
      {
        pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
        bMultCopyX(a1, m, a, tneg);
        return;
      }
      pbCopyAddFast0(b, a1, m);
    }
    goto Top0;

  C_M1_0:  // b < a2
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        bMultCopyX(a1, m, a, tneg);
        return;
      }
    }
    goto Top0;

  C_1_0:  // b >a2
    {
      pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        pbCopyAddFast0(b, a1, m);
      }
      else
      {
        pNext(a) = a2;
        return;
      }
    }
  goto Top0;
}
void spLoop_mac_2i(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;

  tneg = npNegM(tm);
  if (a2==NULL)
  {
    bMultCopyX(a1, m, m, tneg);
    //spMultCopyX(a1, m, m, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long long d;


  if ((pGetOrder(b)<0) && (pGetOrder(a2)<0)) goto Top0;

  //d = (long long)pGetOrder(b) - (long long)pGetOrder(a2);
  //NonZeroTestA(d, 1 /*pOrdSgn*/, goto NotEqual);
  if (pGetOrder(b) > pGetOrder(a2)) { d=1; goto NotEqual; }
  else if (pGetOrder(b) < pGetOrder(a2)) { d=-1; goto NotEqual; }

  // now pGetOrder(b)==pGetOrder(a2):
  _pMonCmp_2i(b, a2, LoopVariablesW, d,
                 NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    pTest(pNext(m));
    return;
  }
  if (a2==NULL)
  {
    pFree1(b);
    bMultCopyX(a1, m, a, tneg);
    //spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      bMultCopyX(a1, m, a, tneg);
      //spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    pTest(pNext(m));
    return;
  }
  goto Top;


  // below degree limit =================================================
  Top0:
  if (pGetOrder(b) > pGetOrder(a2))      { goto C_1_0; }
  else if (pGetOrder(b) < pGetOrder(a2)) { goto C_M1_0; }


  //Equal0:
    {
      tb = npMultM(pGetCoeff(a1),tm);
      if (!npEqualM(pGetCoeff(a2),tb))
      {
        pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
        bMultCopyX(a1, m, a, tneg);
        return;
      }
      pbCopyAddFast0(b, a1, m);
    }
    goto Top0;

  C_M1_0:  // b < a2
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        bMultCopyX(a1, m, a, tneg);
    pTest(pNext(m));
        return;
      }
    }
    goto Top0;

  C_1_0:  // b >a2
    {
      pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        pbCopyAddFast0(b, a1, m);
      }
      else
      {
        pNext(a) = a2;
    pTest(pNext(m));
        return;
      }
    }
  goto Top0;
}
void spLoop_mac_2i_1(poly a1, poly a2, poly m,poly spNoether)
{
  poly a, b, s;
  number tm = pGetCoeff(m);
  number tneg,tb;
  int c;

  tneg = npNegM(tm);
  if (a2==NULL)
  {
    //bMultCopyX(a1, m, m, tneg);
    spMultCopyX(a1, m, m, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  a = m;
  b = pNew();
  pCopyAddFast(b, a1, m);

  Top:
  register long long d;


  if ((pGetOrder(b)<0) && (pGetOrder(a2)<0)) goto Top0;

  //d = (long long)pGetOrder(b) - (long long)pGetOrder(a2);
  //NonZeroTestA(d, 1 /*pOrdSgn*/, goto NotEqual);
  if (pGetOrder(b) > pGetOrder(a2)) { d=1; goto NotEqual; }
  else if (pGetOrder(b) < pGetOrder(a2)) { d=-1; goto NotEqual; }

  // now pGetOrder(b)==pGetOrder(a2):
  _pMonCmp_2i_1(b, a2, LoopVariablesW, d,
                 NonZeroA(d, pLexSgn, goto NotEqual ), goto Equal);

  Equal:
  tb = npMultM(pGetCoeff(a1),tm);
  if (!npEqualM(pGetCoeff(a2),tb))
  {
    pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
    pTest(pNext(m));
    return;
  }
  if (a2==NULL)
  {
    pFree1(b);
    //bMultCopyX(a1, m, a, tneg);
    spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
    return;
  }
  pCopyAddFast(b, a1, m);
  goto Top;

  NotEqual: // i.e., b smaller than a2
  if (d < 0)
  {
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pFree1(b);
      //bMultCopyX(a1, m, a, tneg);
      spMultCopyX(a1, m, a, tneg,NULL);
    pTest(pNext(m));
      return;
    }
    goto Top;
  }

  // now d >= 0, i.e., b greater than a2
  pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
  a = pNext(a) = b;
  pIter(a1);
  if (a1!=NULL)
  {
    b = pNew();
    pCopyAddFast(b, a1, m);
  }
  else
  {
    pNext(a) = a2;
    pTest(pNext(m));
    return;
  }
  goto Top;


  // below degree limit =================================================
  Top0:
  if (pGetOrder(b) > pGetOrder(a2))      { goto C_1_0; }
  else if (pGetOrder(b) < pGetOrder(a2)) { goto C_M1_0; }


  //Equal0:
    {
      tb = npMultM(pGetCoeff(a1),tm);
      if (!npEqualM(pGetCoeff(a2),tb))
      {
        pSetCoeff0(a2,npSubM(pGetCoeff(a2),tb));
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
        bMultCopyX(a1, m, a, tneg);
        return;
      }
      pbCopyAddFast0(b, a1, m);
    }
    goto Top0;

  C_M1_0:  // b < a2
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pFree1(b);
        bMultCopyX(a1, m, a, tneg);
        return;
      }
    }
    goto Top0;

  C_1_0:  // b >a2
    {
      pSetCoeff0(b,npMultM(pGetCoeff(a1),tneg));
      a = pNext(a) = b;
      pIter(a1);
      if (a1!=NULL)
      {
        b = pNew();
        pbCopyAddFast0(b, a1, m);
      }
      else
      {
        pNext(a) = a2;
        return;
      }
    }
  goto Top0;
}
static void spLoop_mac_1_c(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_1_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_1_c_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2_c(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2_c_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2i_c(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2i_c_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2i_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2i_1_c(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2i_1_c_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_2i_1_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_1(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_1_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_2(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_2_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_2i(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_2i_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_2i_1(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
static void spLoop_mac_c_2i_1_Syz(poly p1, poly p2, poly m, poly spNoether)
{
  spPSpolyLoop_General(p1,p2,m,NULL /*spNoether*/);
}
#endif // TEST_MAC_ORDER
#endif // COMP_FAST
