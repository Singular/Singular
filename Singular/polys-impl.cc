/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.cc,v 1.18 1998-11-06 14:44:13 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-impl.cc
 * Purpose:    low-level procuders for polys.
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.[h,cc] should be used, instead.
 *
 ***************************************************************/
#ifndef POLYS_IMPL_CC
#define POLYS_IMPL_CC

#include <stdio.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "mmprivat.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "ipid.h"

/***************************************************************
 *
 * Low - level routines which deal with var indicies
 *
 ***************************************************************/
// And here is how we determine the way exponents are stored:
// There are the following four possibilities:
//
//
// BIGENDIAN -- lex order
// e_1, e_2, ... , e_n,..,comp:  pVarOffset = -1,
//                               pVarLowIndex = 0,
//                               pVarHighIndex = pVariables-1
//                               pVarCompIndex = pVariables + #(..)
// BIGENDIAN -- rev lex order
// e_n, ... , e_2, e_1,..,comp:  pVarOffset = pVariables,
//                               pVarLowIndex = 0,
//                               pVarHighIndex = pVariables-1
//                               pVarCompIndex = pVariables + #(..)
// LITTLEENDIAN -- rev lex order
// comp,.., e_1, e_2, ... , e_n : pVarOffset = #(..),
//                                pVarLowIndex = 1 + #(..),
//                                pVarHighIndex = #(..) + pVariables
//                                pVarCompIndex = 0
// LITTLEENDIAN -- lex order
// comp,..,e_n, .... , e_2, e_1 : pVarOffset = pVariables + 1 + #(..)
//                                pVarLowIndex = 1 + #(..)
//                                pVarHighIndex = #(..) + pVariables
//                                pVarCompIndex = 0
//
// Furthermore, the size of the exponent vector is always a multiple
// of the word size -- "empty exponents" (exactly #(..) ones) are
// filled in between comp and first/last exponent -- i.e. comp and
// first/last exponent might not be next to each other

void pGetVarIndicies_Lex(int nvars, int* VarOffset, int &VarCompIndex,
                                int &VarLowIndex, int &VarHighIndex)
{
  long temp = (nvars+1)*sizeof(Exponent_t);
  if ((temp % sizeof(long)) == 0)
    temp = temp / sizeof(long);
  else
    temp = (temp / sizeof(long)) + 1; // temp == pVariables1W
#ifdef WORDS_BIGENDIAN
  VarCompIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarOffset[0] = VarCompIndex;
  for (temp=1; temp<=nvars; temp++)
    VarOffset[temp] = temp - 1;
  VarLowIndex  = 0;
  VarHighIndex = nvars - 1;
#else //  ! WORDS_BIGENDIAN
  temp *= sizeof(long)/sizeof(Exponent_t);
  VarHighIndex = temp -1;
  VarLowIndex = temp - nvars;
  VarOffset[0] = 0;
  for (long i = 1; i<=nvars;i++)
    VarOffset[i] = temp - i;
  VarCompIndex = 0;
#endif // WORDS_BIGENDIAN
}

void pGetVarIndicies_RevLex(int nvars,int *VarOffset,int &VarCompIndex,
                            int &VarLowIndex, int &VarHighIndex)
{
  long temp = (nvars+1)*sizeof(Exponent_t);
  if ((temp % sizeof(long)) == 0)
    temp = temp / sizeof(long);
  else
    temp = (temp / sizeof(long)) + 1; // temp == pVariables1W
#ifdef WORDS_BIGENDIAN
  VarCompIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarOffset[0] = VarCompIndex;
  for (temp=1; temp <= nvars; temp++)
    VarOffset[temp] = nvars - temp;
  VarLowIndex  = 0;
  VarHighIndex = nvars-1;
#else //  ! WORDS_BIGENDIAN
  temp *= sizeof(long)/sizeof(Exponent_t);
  VarHighIndex = temp -1;
  VarLowIndex = temp - nvars;
  VarOffset[0] = 0;
  for (int i = 1; i<=nvars; i++)
    VarOffset[i] = temp - nvars - 1 + i;
  VarCompIndex = 0;
#endif // WORDS_BIGENDIAN
}

void pGetVarIndicies(int nvars, int *VarOffset, int &VarCompIndex,
                     int &VarLowIndex, int &VarHighIndex)
{
  pGetVarIndicies_Lex(nvars,VarOffset, VarCompIndex, VarLowIndex,VarHighIndex);
}

// gets var indicies w.r.t. the ring r
void pGetVarIndicies(ring r, int *VarOffset, int &VarCompIndex,
                     int &VarLowIndex, int &VarHighIndex)
{
  // at the moment, non-default var indicies are only used for simple orderings
  if (rHasSimpleOrder(r))
  {
    short s_order;
    if (r->order[0] == ringorder_c || r->order[0] == ringorder_C)
      s_order = r->order[1];
    else
      s_order = r->order[0];

    switch(s_order)
    {
        case ringorder_dp:
        case ringorder_wp:
        case ringorder_ds:
        case ringorder_ws:
        case ringorder_unspec:
          pGetVarIndicies_RevLex(r->N, VarOffset, VarCompIndex, 
                                 VarLowIndex, VarHighIndex);
          break;

#ifdef PDEBUG
        case ringorder_lp:
        case ringorder_Dp:
        case ringorder_Wp:
        case ringorder_Ds:
        case ringorder_Ws:
        case ringorder_ls:
#else
        default:
#endif
          pGetVarIndicies_Lex(r->N, VarOffset, VarCompIndex, 
                              VarLowIndex, VarHighIndex);
#ifdef PDEBUG
          break;
        default:
          Werror("wrong internal ordering:%d at %s, l:%d\n",
                 s_order,__FILE__,__LINE__);
#endif
    }
  }
  else
    // default var indicies are used
    pGetVarIndicies(r->N, VarOffset, VarCompIndex, VarLowIndex, VarHighIndex);
}


inline void RingCopy2ExpV(poly dest, poly src, ring src_r)
{
  for (int i=pVariables; i; i--)
    pSetExp(dest, i, pRingGetExp(src_r, src, i));
  pSetComp(dest, pRingGetComp(src_r, src));
}

// Returns a converted copy (in the sense that the returned poly is
// poly of currRing) of poly p which is from ring r -- assumes that
// currRing and r have the same number of variables, i.e. that polys
// from r can be "fetched" into currRing
#ifdef PDEBUG
poly pDBFetchCopy(ring r, poly p,char *f,int l)
#else
poly _pFetchCopy(ring r, poly p)
#endif
{
  poly res;
  poly a;
  if (p==NULL) return NULL;
#ifdef PDEBUG
  res = a = pDBNew(f,l);
#else
  res = a = pNew();
#endif
  if (r->VarOffset == pVarOffset)
  {
    pCopy2(a,p);
    a->coef=nCopy(p->coef);
    pSetm(a);
    if (pNext(p)!=NULL)
    {
      pIter(p);
      do
      {
#ifdef PDEBUG
        a = pNext(a) = pDBNew(f,l);
#else
        a = pNext(a) = pNew();
#endif
        pCopy2(a,p);
        a->coef=nCopy(p->coef);
        pSetm(a);
        pIter(p);
      }
      while (p!=NULL);
    }
    pNext(a) = NULL;
  }
  else
  {
#ifdef PDEBUG
    a = res = pDBInit(f,l);
#else
    a = res = pInit();
#endif
    res->coef = nCopy(p->coef);
    RingCopy2ExpV(res, p, r);
    pSetm(res);
    if (pNext(p) != NULL)
    {
      pIter(p);
      do
      {
        // the VarOffset's are different: Hence we
        // convert betweeen a lex order and a revlex order -- to speed
        // up the sorting, we assemble new poly in inverse order
#ifdef PDEBUG
        res = pDBInit(f,l);
#else
        res = pInit();
#endif
        pNext(res) = a;
        a = res;
        res->coef = nCopy(p->coef);
        RingCopy2ExpV(res, p, r);
        pSetm(res);
        pIter(p);
      }
      while (p != NULL);
    }
  }
#ifdef PDEBUG
  res = pOrdPolyMerge(res);
  pTest(res);
  return res;
#else
  return pOrdPolyMerge(res);
#endif
}


/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/

#ifdef PDEBUG
poly pDBNew(char *f, int l)
{
#ifdef MDEBUG
  poly p = (poly) mmDBAllocSpecialized(f,l);
#else
  poly p = (poly) mmAllocSpecialized();
#endif
  memset(p,0,pMonomSize);
  return p;
}
#endif

/*2
* create a new monomial and init
*/
#ifdef PDEBUG
poly pDBInit(char * f, int l)
{
  poly p=pDBNew(f,l);
  memset(p,0, pMonomSize);
  nNew(&(p->coef));
  return p;
}
#endif

/*2
* delete a poly, resets pointer
* put the monomials in the freelist
*/
#ifdef PDEBUG
void pDBDelete(poly * p, char * f, int l)
{
  poly h = *p;

  while (h!=NULL)
  {
#ifdef LDEBUG
    nDBDelete(&(h->coef),f,l);
#else
    nDelete(&(h->coef));
#endif
    pIter(h);
#ifdef MDEBUG
    mmDBFreeSpecialized((ADDRESS)*p,f,l);
#else
    mmFreeSpecialized((ADDRESS)*p);
#endif
    *p=h;
    if (l>0) l= -l;
  }
  *p = NULL;
}
#else
void _pDelete(poly* p)
{
  poly h = *p;
  poly pp;

  while (h!=NULL)
  {
    nDelete(&(h->coef));
    pp=h;
    pIter(h);
    mmFreeSpecialized((ADDRESS)pp);
  }
  *p = NULL;
}
#endif

/*2
* remove first monom
*/
#ifdef PDEBUG
void pDBDelete1(poly * p, char * f, int l)
{
  poly h = *p;

  if (h==NULL) return;
  nDelete(&(h->coef));
  *p = pNext(h);
#ifdef MDEBUG
  mmDBFreeSpecialized((ADDRESS)h,f,l);
#else
  mmFreeSpecialized((ADDRESS)h);
#endif
}
#else
void _pDelete1(poly* p)
{
  poly h = *p;

  if (h==NULL) return;
  nDelete(&(h->coef));
  *p = pNext(h);
  mmFreeSpecialized((ADDRESS)h);
}
#endif

void ppDelete(poly* p, ring rg)
{
  ring origRing = currRing;
  rChangeCurrRing(rg, FALSE);
  pDelete(p);
  rChangeCurrRing(origRing, FALSE);
}

/*2
* remove first monom
*/
#ifdef PDEBUG
void pDBFree1(poly p, char * f, int l)
{
  if (p!=NULL)
  {
    p->coef=NULL;//nDelete(&(p->coef));
#ifdef MDEBUG
    mmDBFreeSpecialized((ADDRESS)p,f,l);
#else
    mmFreeSpecialized((ADDRESS)p);
#endif
  }
}
#endif

/*2
* creates a copy of p
*/
#ifdef PDEBUG
poly pDBCopy(poly p,char *f,int l)
#else
poly _pCopy(poly p)
#endif
{
  poly w, a;

  if (p==NULL) return NULL;
  pDBTest(p,f,l);
#ifdef PDEBUG
  w = a = pDBNew(f,l);
#else
  w = a = pNew();
#endif
  memcpy(w,p,pMonomSize);
  w->coef=nCopy(p->coef);
  if (pNext(p)!=NULL)
  {
    pIter(p);
    do
    {
#ifdef PDEBUG
      a = pNext(a) = pDBNew(f,l);
#else
      a = pNext(a) = pNew();
#endif
      memcpy(a,p,pMonomSize);
      a->coef=nCopy(p->coef);
      pIter(p);
    }
    while (p!=NULL);
  }
  pNext(a) = NULL;
  return w;
}


/*2
* creates a copy of the initial monomial of p
* sets the coeff of the copy to a defined value
*/
#ifdef PDEBUG
poly pDBCopy1(poly p,char *f,int l)
#else
poly _pCopy1(poly p)
#endif
{
  poly w;
#ifdef PDEBUG
  w = pDBNew(f,l);
#else
  w = pNew();
#endif
  pCopy2(w,p);
  nNew(&(w->coef));
  pNext(w) = NULL;
  return w;
}

/*2
* returns (a copy of) the head term of a
*/
#ifdef PDEBUG
poly pDBHead(poly p,char *f, int l)
#else
poly _pHead(poly p)
#endif
{
  poly w=NULL;

  if (p!=NULL)
  {
#ifdef PDEBUG
    w = pDBNew(f,l);
#else
    w = pNew();
#endif
    pCopy2(w,p);
    pSetCoeff0(w,nCopy(pGetCoeff(p)));
    pNext(w) = NULL;
  }
  return w;
}

poly pHeadProc(poly p)
{
  return pHead(p);
}

/*2
* returns (a copy of) the head term of a without the coef
*/
#ifdef PDEBUG
poly pDBHead0(poly p,char *f, int l)
#else
poly _pHead0(poly p)
#endif
{
  poly w=NULL;

  if (p!=NULL)
  {
#ifdef PDEBUG
    w = pDBNew(f,l);
#else
    w = pNew();
#endif
    pCopy2(w,p);
    pSetCoeff0(w,NULL);
    pNext(w) = NULL;
  }
  return w;
}


/***************************************************************
 *
 * Routines for turned on debugging
 *
 ***************************************************************/

#ifdef PDEBUG

#if PDEBUG != 0
Exponent_t pPDSetExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp[_pExpIndex(v)]=(e);
}

Exponent_t pPDGetExp(poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp[_pExpIndex(v)];
}

Exponent_t pPDRingSetExp(ring r, poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > r->N)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp[_pRingExpIndex(r, v)]=(e);
}

Exponent_t pPDRingGetExp(ring r, poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > r->N)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp[_pRingExpIndex(r,v)];
}

Exponent_t pPDIncrExp(poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp[_pExpIndex(v)])++;
}

Exponent_t pPDDecrExp(poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp[_pExpIndex(v)])--;
}

Exponent_t pPDAddExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp[_pExpIndex(v)]) += (e);
}

Exponent_t pPDSubExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp[_pExpIndex(v)]) -= (e);
}

Exponent_t pPDMultExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Print("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Print("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp[_pExpIndex(v)]) *= (e);
}

// checks whether fast monom add did not overflow
void pDBMonAddFast(poly p1, poly p2, char* f, int l)
{
  poly ptemp = pNew();
  pCopy2(ptemp, p1);

  __pMonAddFast(p1, p2);

  for (int i=1; i<=pVariables; i++)
  {
    pAddExp(ptemp, i, pGetExp(p2, i));
  }
  pGetOrder(ptemp) += pGetOrder(p2);

  if (! pEqual(ptemp, p1))
  {
    Print("Error in pMonAddFast in %s:%d\n", f, l);
  }

  pFree1(ptemp);
}

void pDBCopyAddFast(poly p1, poly p2, poly p3, char* f, int l)
{
  if (p2 == p1 || p3 == p1)
  {
    Print("Error in pCopyAddFast: Destination equals source in %s:%d\n", f, l);
  }
  poly ptemp = pNew();
  __pCopyAddFast(ptemp, p2, p3);

  pCopy2(p1, p2);
  pDBMonAddFast(p1, p3, f, l);

  if (! pEqual(ptemp, p1))
    Print("Error in pCopyMonAddFast in %s:%d\n", f, l);
  pFree1(ptemp);
}

void pDBCopyAddFastHomog(poly p1, poly p2, poly p3, Order_t Order,
                         char* f, int l)
{
  pDBCopyAddFast(p1, p2, p3, f, l);
  if (p1->Order != Order)
    Print("Error in pCopyAddFastHomog: Order is different from sum\n");
}

    

static BOOLEAN OldpDivisibleBy(poly a, poly b)
{
  if ((a!=NULL)&&((pGetComp(a)==0) || (pGetComp(a) == pGetComp(b))))
  {
    for (int i = 1; i<=pVariables; i++)
      if (pGetExp(a, i) > pGetExp(b,i)) return FALSE;
    return TRUE;
  }
  return FALSE;
}


BOOLEAN pDBDivisibleBy(poly a, poly b, char* f, int l)
{
  BOOLEAN istrue = OldpDivisibleBy(a,b);
  BOOLEAN f_istrue = _pDivisibleBy_orig(a, b);

  if (istrue != f_istrue)
  {
    Print("Error in pDivisibleBy in %s:%d\n", f, l);
    _pDivisibleBy_orig(a, b);
  }
  return f_istrue;
}

BOOLEAN pDBDivisibleBy1(poly a, poly b, char* f, int l)
{
  BOOLEAN istrue = OldpDivisibleBy(a,b);
  BOOLEAN f_istrue = _pDivisibleBy1_orig(a, b);

  if (istrue != f_istrue)
  {
    Print("Error in pDivisibleBy1 in %s:%d\n", f, l);
    _pDivisibleBy1_orig(a, b);
  }
  return f_istrue;
}

BOOLEAN pDBDivisibleBy2(poly a, poly b, char* f, int l)
{
  BOOLEAN istrue = OldpDivisibleBy(a,b);
  BOOLEAN f_istrue = __pDivisibleBy(a, b);

  if (istrue != f_istrue)
  {
    Print("Error in pDivisibleBy2 in %s:%d\n", f, l);
    __pDivisibleBy(a, b);
  }
  return f_istrue;
}

#endif // PDEBUG != 0


BOOLEAN pDBTest(poly p, char *f, int l)
{
  poly old=NULL;
  BOOLEAN ismod=FALSE;
  while (p!=NULL)
  {
#ifdef MDEBUG
    if (!mmDBTestBlock(p,mm_specSize,f,l))
      return FALSE;
#endif
#ifdef LDEBUG
    if (!nDBTest(p->coef,f,l))
      return FALSE;
#endif
    if ((p->coef==NULL)&&(nGetChar()<2))
    {
      Print("NULL coef in poly in %s:%d\n",f,l);
      return FALSE;
    }
    if (nIsZero(p->coef))
    {
      Print("zero coef in poly in %s:%d\n",f,l);
      return FALSE;
    }
    int i=pVariables;
#ifndef DRING
    for(;i;i--)
    {
      if (pGetExp(p,i)<0)
      {
        Print("neg. Exponent in %s:%d\n",f,l);
        return FALSE;
      }
    }
    if (pGetComp(p)<0)
    {
      Print("neg Component in %s:%d\n",f,l);
      return FALSE;
    }
#endif
    if (ismod==0)
    {
      if (pGetComp(p)>0) ismod=1;
      else               ismod=2;
    }
    else if (ismod==1)
    {
      if (pGetComp(p)==0)
      {
        Print("mix vec./poly in %s:%d\n",f,l);
        return FALSE;
      }
    }
    else if (ismod==2)
    {
      if (pGetComp(p)!=0)
      {
        Print("mix poly/vec. in %s:%d\n",f,l);
        return FALSE;
      }
    }
    i=p->Order;
    pSetm(p);
    if(i!=p->Order)
    {
      Print("wrong ord-field in %s:%d\n",f,l);
      return FALSE;
    }
    old=p;
    pIter(p);
    if (pComp(old,p)!=1)
    {
      PrintS("wrong order (");
      wrp(old);
      Print(") in %s:%d\n",f,l);
      return FALSE;
    }
  }
  return TRUE;
}
#endif // PDEBUG


#endif // POLYS_IMPL_CC
