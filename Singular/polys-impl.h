#ifndef POLYS_IMPL_H
#define POLYS_IMPL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.h,v 1.49 2000-08-24 14:28:55 Singular Exp $ */

/***************************************************************
 *
 * File:       polys-impl.h
 * Purpose:    low-level definition and declarations for polys
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.h should be used, instead.
 *
 ***************************************************************/
#include "tok.h"
#include "structs.h"
#include <omalloc.h>

/***************************************************************
 *
 * definition of the poly structure and its fields
 *
 ***************************************************************/

#ifndef NDEBUG
#define VARS (10)   /*max. number of variables as constant*/
#else
#define VARS (0)
#endif
union s_exp
{
#ifdef HAVE_SHIFTED_EXPONENTS
   unsigned long  l[VARS +1];
#else
   Exponent_t     e[VARS +1];
   unsigned long  l[(VARS +1)/(sizeof(long)/sizeof(Exponent_t))];
#endif
};

typedef s_exp  monomial;
typedef Exponent_t* Exponent_pt;

typedef unsigned long Order_t;
struct  spolyrec
{
  poly      next; // next needs to be the first field
  number    coef; // and coef the second --- do not change this !!!
  monomial  exp; // make sure that exp is aligned
};

/***************************************************************
 *
 * variables/defines used for managment of monomials
 *
 ***************************************************************/

#define POLYSIZE (sizeof(poly) + sizeof(number))
#define POLYSIZEW (POLYSIZE / sizeof(long))
#define MAX_EXPONENT_NUMBER ((MAX_BLOCK_SIZE - POLYSIZE) / SIZEOF_EXPONENT)

// number of Variables
extern int pVariables;
// size of a monom in bytes - always a multiple of sizeof(void*)
extern int pMonomSize;
// size of a monom in units of sizeof(void*) -- i.e. in words
extern int pMonomSizeW;
// Ceiling((pVariables+1) / sizeof(void*)) == length of exp-vector in words
// extern int pVariables1W;
// Ceiling((pVariables) / sizeof(void*))
// extern int pVariablesW;
extern int *pVarOffset;
// extern int pVarLowIndex;
// extern int pVarHighIndex;
// extern int pVarCompIndex;
extern omBin currPolyBin;

/***************************************************************
 *
 * Primitives for determening/setting  the way exponents are arranged
 *
 ***************************************************************/
#define _pExpIndex(i) (currRing->VarOffset[(i)])
#define _pRingExpIndex(r, i)  (r)->VarOffset[(i)]

#define _pCompIndex        (currRing->pCompIndex)
#define _pRingCompIndex(r)  ((r)->pCompIndex)

/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 *
 ***************************************************************/
#define _pNext(p)           ((p)->next)
#define _pIter(p)           ((p) = (p)->next)

#define _pGetCoeff(p)       ((p)->coef)
#define _pSetCoeff(p,n)     {nDelete(&((p)->coef));(p)->coef=n;}
#define _pSetCoeff0(p,n)    (p)->coef=n

//#ifdef HAVE_SHIFTED_EXPONENTS
//#define _pGetOrder(p)       ((p)->exp.l[currRing->pOrdIndex]-0x40000000)
//#else
#define _pGetOrder(p)       ((p)->exp.l[currRing->pOrdIndex])
//#endif

#if defined(PDEBUG) && PDEBUG > 1
extern Exponent_t pPDSetExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDGetExp(poly p, int v, char* f, int l);
extern Exponent_t pPDIncrExp(poly p, int v, char* f, int l);
extern Exponent_t pPDDecrExp(poly p, int v, char* f, int l);
extern Exponent_t pPDAddExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDMultExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDSubExp(poly p, int v, Exponent_t e, char* f, int l);

extern Exponent_t pPDRingSetExp(ring r,poly p,int v,Exponent_t e,char* f,int l);
extern Exponent_t pPDRingGetExp(ring r,poly p, int v, char* f, int l);

extern Exponent_t pDBSetComp(poly p, Exponent_t k, int l, char* f, int l);
extern Exponent_t pDBDecrComp(poly p, char* f, int l);
extern Exponent_t pDBAddComp(poly p, Exponent_t k, int l, char* f, int l);
extern Exponent_t pDBSubComp(poly p, Exponent_t k, char* f, int l);
extern Exponent_t pDBRingSetComp(ring r, poly p, Exponent_t k, char* f, int l);


#define _pSetExp(p,v,e)     pPDSetExp(p,v,e,__FILE__,__LINE__)
#define _pGetExp(p,v)       pPDGetExp(p,v,__FILE__,__LINE__)
#define _pIncrExp(p,v)      pPDIncrExp(p,v,__FILE__,__LINE__)
#define _pDecrExp(p,v)      pPDDecrExp(p,v,__FILE__,__LINE__)
#define _pAddExp(p,i,v)     pPDAddExp(p,i,v,__FILE__,__LINE__)
#define _pSubExp(p,i,v)     pPDSubExp(p,i,v,__FILE__,__LINE__)
#define _pMultExp(p,i,v)    pPDMultExp(p,i,v,__FILE__,__LINE__)

#define _pRingSetExp(r,p,v,e)     pPDRingSetExp(r,p,v,e,__FILE__,__LINE__)
#define _pRingGetExp(r,p,v)       pPDRingGetExp(r,p,v,__FILE__,__LINE__)

#define _pSetComp(p,k)      pDBSetComp(p, k, 0, __FILE__, __LINE__)
#define _pDecrComp(p)       pDBDecrComp(p, __FILE__, __LINE__)
#define _pAddComp(p,v)      pDBAddComp(p,v, 0, __FILE__, __LINE__)
#define _pSubComp(p,v)      pDBSubComp(p,v, __FILE__, __LINE__)
#define _pRingSetComp(r,p,k)  pDBRingSetComp(r, p, k, __FILE__, __LINE__)

#define pSetCompS(p, k, l) pDBSetComp(p, k, l, __FILE__, __LINE__)

#else  // ! (defined(PDEBUG) && PDEBUG > 1)
#ifndef HAVE_SHIFTED_EXPONENTS

#define _pSetExp(p,v,E)     (p)->exp.e[_pExpIndex(v)]=(E)
#define _pGetExp(p,v)       (p)->exp.e[_pExpIndex(v)]
#define _pIncrExp(p,v)      ((p)->exp.e[_pExpIndex(v)])++
#define _pDecrExp(p,v)      ((p)->exp.e[_pExpIndex(v)])--
#define _pAddExp(p,i,v)     ((p)->exp.e[_pExpIndex(i)]) += (v)
#define _pSubExp(p,i,v)     ((p)->exp.e[_pExpIndex(i)]) -= (v)
#define _pMultExp(p,i,v)    ((p)->exp.e[_pExpIndex(i)]) *= (v)

#define _pRingSetExp(r,p,e_i,e_v)     (p)->exp.e[_pRingExpIndex(r,e_i)]=(e_v)
#define _pRingGetExp(r,p,v)       (p)->exp.e[_pRingExpIndex(r,v)]
#else
inline Exponent_t pSGetExp(poly p, int v, ring r)
{
  assume(v>0);
  assume(v<=r->N);
  return (p->exp.l[(r->VarOffset[v] & 0xffffff)] >> (r->VarOffset[v] >> 24))
          & r->bitmask;
}

inline Exponent_t pSSetExp(poly p, int v, int e, ring r)
{
  assume(e>=0);
  assume(v>0);
  assume(v<=r->N);
  assume(e<=((int)r->bitmask));

  // shift e to the left:
  register int shift = r->VarOffset[v] >> 24;
  unsigned long ee = ((unsigned long)e) << shift /*(r->VarOffset[v] >> 24)*/;
  // clear the bits in the exponent vector:
  register int offset = (r->VarOffset[v] & 0xffffff);
  p->exp.l[offset]  &=
    ~( r->bitmask << shift );
  // insert e with |
  p->exp.l[ offset ] |= ee;
  return e;
}

inline Exponent_t pSIncrExp(poly p, int v, ring r)
{
  assume(v>0);
  assume(v<=r->N);

  Exponent_t e=pSGetExp(p,v,r);
  e++;
  return pSSetExp(p,v,e,r);
}

inline Exponent_t pSDecrExp(poly p, int v, ring r)
{
  assume(v>0);
  assume(v<=r->N);

  Exponent_t e=pSGetExp(p,v,r);
  e--;
  return pSSetExp(p,v,e,r);
}

inline Exponent_t pSAddExp(poly p, int v, Exponent_t e, ring r)
{
  assume(v>0);
  assume(v<=r->N);

  Exponent_t ee=pSGetExp(p,v,r);
  ee+=e;
  return pSSetExp(p,v,ee,r);
}

inline Exponent_t pSSubExp(poly p, int v, Exponent_t e, ring r)
{
  assume(v>0);
  assume(v<=r->N);

  Exponent_t ee=pSGetExp(p,v,r);
  ee-=e;
  return pSSetExp(p,v,ee,r);
}

inline Exponent_t pSMultExp(poly p, int v, Exponent_t e, ring r)
{
  assume(v>0);
  assume(v<=r->N);

  Exponent_t ee=pSGetExp(p,v,r);
  ee*=e;
  return pSSetExp(p,v,ee,r);
}

#define _pSetExp(p,v,E)     pSSetExp(p,v,E,currRing)
#define _pGetExp(p,v)       pSGetExp(p,v,currRing)
#define _pIncrExp(p,v)      pSIncrExp(p,v,currRing)
#define _pDecrExp(p,v)      pSDecrExp(p,v,currRing)
#define _pAddExp(p,i,v)     pSAddExp(p,i,v,currRing)
#define _pSubExp(p,i,v)     pSSubExp(p,i,v,currRing)
#define _pMultExp(p,i,v)    pSMultExp(p,i,v,currRing)

#define _pRingSetExp(r,p,e_i,e_v)  pSSetExp(p,e_i,e_v,r)
#define _pRingGetExp(r,p,v)        pSGetExp(p,v,r)
#endif

#define _pSetComp(p,k)      _pGetComp(p) = (k)
#define _pDecrComp(p)       _pGetComp(p)--
#define _pAddComp(p,v)      _pGetComp(p) += (v)
#define _pSubComp(p,v)      _pGetComp(p) -= (v)
#define _pRingSetComp(r,p,k)      (_pRingGetComp(r, p) = (k))
#define pSetCompS(p, k,l)     _pSetComp(p, k)

#endif // defined(PDEBUG) && PDEBUG > 1

#ifndef HAVE_SHIFTED_EXPONENTS

#define _pGetComp(p)        ((p)->exp.e[_pCompIndex])
#define _pIncrComp(p)       _pGetComp(p)++
#define _pRingGetComp(r,p)        ((p)->exp.e[_pRingCompIndex(r)])

inline Exponent_t _pGetExpSum(poly p1, poly p2, int i)
{
  int index = _pExpIndex(i);
  return p1->exp.e[index] + p2->exp.e[index];
}
inline Exponent_t _pGetExpDiff(poly p1, poly p2, int i)
{
  int index = _pExpIndex(i);
  return p1->exp.e[index] - p2->exp.e[index];
}

#else //---------------------------------------------

#define _pGetComp(p)        ((p)->exp.l[_pCompIndex])
#define _pIncrComp(p)       _pGetComp(p)++
#define _pRingGetComp(r,p)        ((p)->exp.l[_pRingCompIndex(r)])

inline Exponent_t _pGetExpSum(poly p1, poly p2, int i)
{
  return _pGetExp(p1,i) + _pGetExp(p2,i);
}
inline Exponent_t _pGetExpDiff(poly p1, poly p2, int i)
{
  return _pGetExp(p1,i) - _pGetExp(p2,i);
}
#endif

inline void _pGetExpV(poly p, Exponent_t *ev)
{
  for (int j = pVariables; j; j--)
      ev[j] = _pGetExp(p, j);

  ev[0] = _pGetComp(p);
}

extern pSetmProc pSetm;
inline void _pSetExpV(poly p, Exponent_t *ev)
{
  for (int j = pVariables; j; j--)
      _pSetExp(p, j, ev[j]);

  _pSetComp(p, ev[0]);
  pSetm(p);
}

/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/
#define prAllocMonom(p, r)  omTypeAllocBin(poly, p, r->PolyBin)
#define _pNew(h)            (poly) omAllocBin(h)
#define _pInit(h)           (poly) omAlloc0Bin(h)
#define _pFree1(a, h)       omFreeBin((void*) a, h)
#define _pRingFree1(r, a)   omFreeBin((void*) a, r->PolyBin)

extern void    _pDelete(poly * a, omBin h);
extern void    _pDelete1(poly * a, omBin h);

extern poly    _pCopy(poly a);
extern poly    _pCopy(omBin h, poly a);
extern poly    _pCopy1(poly a);
extern poly    _pHead(omBin h, poly a);
extern poly    _pHead0(poly a);
extern poly    _pShallowCopyDeleteHead(omBin d_h, poly *s_p, omBin s_h);
extern poly    _pShallowCopyDelete(omBin d_h, poly *s_p, omBin s_h);
#define _pCopy2(p1, p2)     omMemcpyW(p1, p2, pMonomSizeW)


/***************************************************************
 *
 * Routines which work on vectors instead of single exponents
 *
 ***************************************************************/
// Here is a handy Macro which disables inlining when run with
// profiling and enables it otherwise

#ifdef DO_DEEP_PROFILE

#ifndef POLYS_IMPL_CC

#define DECLARE(type, arglist) type arglist; \
   static type dummy_##arglist
#else
#define DECLARE(type, arglist) type arglist
#endif // POLYS_IMPL_CC

#else //! DO_DEEP_PROFILE

#define DECLARE(type, arglist ) inline type arglist

#endif // DO_DEEP_PROFILE


#if defined(PDEBUG) && PDEBUG > 1
#define _pMonAddOn(p1, p2)  pDBMonAddOn(p1, p2, __FILE__, __LINE__)
extern  void pDBMonAddOn(poly p1, poly p2, char* f, int l);
inline void __pMonAddOn(poly p1, poly p2)
#else
  DECLARE(void, _pMonAddOn(poly p1, poly p2))
#endif // defined(PDEBUG) && PDEBUG > 1
{
  int i = currRing->ExpLSize;
  unsigned long* s1 = &(p1->exp.l[0]);
  const unsigned long* s2 = &(p2->exp.l[0]);
  for (;;)
  {
    *s1 += *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
}

#if defined(PDEBUG) && PDEBUG > 1
#define _pMonSubFrom(p1, p2)  pDBMonSubFrom(p1, p2, __FILE__, __LINE__)
extern  void pDBMonSubFrom(poly p1, poly p2, char* f, int l);
inline void __pMonSubFrom(poly p1, poly p2)
#else
  DECLARE(void, _pMonSubFrom(poly p1, poly p2))
#endif // defined(PDEBUG) && PDEBUG > 1
{
  int i = currRing->ExpLSize;
  unsigned long* s1 = &(p1->exp.l[0]);
  const unsigned long* s2 = &(p2->exp.l[0]);

  for (;;)
  {
    *s1 -= *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
}

// Makes p1 a copy of p2 and adds on exponents of p3
#if defined(PDEBUG) && PDEBUG > 1
#define _prMonAdd(p1, p2, p3, r)  prDBMonAdd(p1, p2, p3, r, __FILE__, __LINE__)
extern  void prDBMonAdd(poly p1, poly p2, poly p3, ring r, char* f, int l);
inline void __prMonAdd(poly p1, poly p2, poly p3, ring r)
#else
  DECLARE(void, _prMonAdd(poly p1, poly p2, poly p3, ring r))
#endif // defined(PDEBUG) && PDEBUG > 1
{
  unsigned long* s1 = &(p1->exp.l[0]);
  const unsigned long* s2 = &(p2->exp.l[0]);
  const unsigned long* s3 = &(p3->exp.l[0]);
  const unsigned long* const ub = s3 + r->ExpLSize;

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
}


#if SIZEOF_LONG == 4

#if SIZEOF_EXPONENT == 1
#define P_DIV_MASK 0x80808080
#define EXPONENT_MAX     0x7f
#else // SIZEOF_EXPONENT == 2
#define P_DIV_MASK 0x80008000
#define EXPONENT_MAX   0x7fff
#endif

#else // SIZEOF_LONG == 8

#if SIZEOF_EXPONENT == 1
#define P_DIV_MASK 0x8080808080808080
#define EXPONENT_MAX             0x7f
#elif  SIZEOF_EXPONENT == 2
#define P_DIV_MASK 0x8000800080008000
#define EXPONENT_MAX           0x7fff
#else // SIZEOF_EXPONENT == 4
#define P_DIV_MASK 0x8000000080000000
#define EXPONENT_MAX       0x7fffffff
#endif

#endif

// #define LONG_MONOMS

#ifdef LONG_MONOMS
DECLARE(BOOLEAN, __pDivisibleBy(poly a, poly b))
{
  const unsigned long* const lb = (unsigned long*) &(a->exp.l[currRing->pDivLow]);
  const unsigned long* s1 = (unsigned long*) &(a->exp.l[currRing->pDivHigh]);
  const unsigned long* s2 = (unsigned long*) &(b->exp.l[currRing->pDivHigh]);

  for (;;)
  {
    // Yes, the following is correct, provided that the exponents do
    // not have their first bit set
    if ((*s2 - *s1) & P_DIV_MASK) return FALSE;
    if (s1 == lb) return TRUE;
    s1--;
    s2--;
  }
}
#else
DECLARE(BOOLEAN, __pDivisibleBy(poly a, poly b))
{
  int i=pVariables; // assume i>0

  for (;;)
  {
    if (_pGetExp(a,i) > _pGetExp(b,i))
      return FALSE;
    i--;
    if (i==0)
      return TRUE;
  }
}
#endif

#if defined(PDEBUG) && PDEBUG > 1
#define _pDivisibleBy(a,b)   pDBDivisibleBy(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy(poly p1, poly p2, char* f, int l);
inline BOOLEAN _pDivisibleBy_orig(poly a, poly b)
#else
inline BOOLEAN _pDivisibleBy(poly a, poly b)
#endif // defined(PDEBUG) && PDEBUG > 1
{
  if ((a!=NULL)&&((_pGetComp(a)==0) || (_pGetComp(a) == _pGetComp(b))))
  {
    return __pDivisibleBy(a,b);
  }
  return FALSE;
}

#if defined(PDEBUG) && PDEBUG > 1
#define _pDivisibleBy1(a,b)   pDBDivisibleBy1(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy1(poly p1, poly p2, char* f, int l);
inline BOOLEAN _pDivisibleBy1_orig(poly a, poly b)
#else
inline BOOLEAN _pDivisibleBy1(poly a, poly b)
#endif // defined(PDEBUG) && PDEBUG > 1
{
  if (_pGetComp(a) == 0 || _pGetComp(a) == _pGetComp(b))
    return __pDivisibleBy(a,b);
  return FALSE;
}

#if defined(PDEBUG) && PDEBUG > 1
#define _pDivisibleBy2(a,b)   pDBDivisibleBy2(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy2(poly p1, poly p2, char* f, int l);
#else
#define _pDivisibleBy2(a,b) __pDivisibleBy(a,b)
#endif // defined(PDEBUG) && PDEBUG > 1

#ifdef PDEBUG
#define _pEqual(p, q)       mmDBEqual(p, q, __FILE__, __LINE__)
BOOLEAN mmDBEqual(poly p, poly q, char* file, int line);
#else
#define _pEqual __pEqual
#endif

DECLARE(BOOLEAN, __pEqual(poly p1, poly p2))
{
  const unsigned long *s1 = (unsigned long*) &(p1->exp.l[0]);
  const unsigned long *s2 = (unsigned long*) &(p2->exp.l[0]);
  const unsigned long* const lb = s1 + currRing->ExpLSize;

  for(;;)
  {
    if (*s1 != *s2) return FALSE;
    s1++;
    if (s1 == lb) return TRUE;
    s2++;
  }
}

/***************************************************************
 *
 * Misc. things
 *
 *
 ***************************************************************/
// Divisiblity tests based on Short Exponent Vectors
// define to enable debugging of this
// #define PDIV_DEBUG
#if defined(PDEBUG) && ! defined(PDIV_DEBUG)
#define PDIV_DEBUG
#endif

#ifdef PDIV_DEBUG
#define _pShortDivisibleBy(a, sev_a, b, not_sev_b) \
  pDBShortDivisibleBy(a, sev_a, b, not_sev_b, __FILE__, __LINE__)
BOOLEAN pDBShortDivisibleBy(poly p1, unsigned long sev_1,
                            poly p2, unsigned long not_sev_2,
                            char* f, int l);
#else
#define _pShortDivisibleBy(a, sev_a, b, not_sev_b) \
  ( ! (sev_a & not_sev_b) && pDivisibleBy(a, b))
#endif


/***************************************************************
 *
 * Routines which implement low-level manipulations/operations
 * on exponents and "are allowed" to access single exponetns
 *
 ***************************************************************/

#ifdef LONG_MONOMS
DECLARE(int, __pExpQuerSum2(poly p, int from, int to))
{
  int j = 0;
  int i = from ;

  for(;;)
  {
    if (i > to) break;
    j += p->exp.e[i];
    i++;
  }
  if (from <= _pCompIndex && to >= _pCompIndex)
    return j - _pGetComp(p);
  return j;
}

#define _pExpQuerSum(p)  __pExpQuerSum2(p, currRing->pVarLowIndex, currRing->pVarHighIndex)

inline int _pExpQuerSum2(poly p,int from,int to)
{
  int ei_to = _pExpIndex(to);
  int ei_from = _pExpIndex(from);

  if (ei_from > ei_to)
    return __pExpQuerSum2(p, ei_to, ei_from);
  else
    return __pExpQuerSum2(p, ei_from, ei_to);
}

#else
DECLARE(int, _pExpQuerSum(poly p))
{
  int s = 0;
  int i = pVariables;
  for (;;)
  {
    s += _pGetExp(p, i);
    i--;
    if (i==0) return s;
  }
}
#endif

#endif // POLYS_IMPL_H
