#ifndef POLYS_IMPL_H
#define POLYS_IMPL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.h,v 1.25 1998-11-04 15:55:32 obachman Exp $ */

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
#include "structs.h"
#include "mmemory.h"

/***************************************************************
 *
 * definition of the poly structure and its fields
 *
 ***************************************************************/

// EXPONENT_TYPE is determined by configure und defined in mod2.h
typedef EXPONENT_TYPE Exponent_t;

#ifndef NDEBUG
#define VARS (10)   /*max. number of variables as constant*/
#else
#define VARS (0)
#endif

typedef Exponent_t  monomial[VARS + 1];
typedef Exponent_t* Exponent_pt;

typedef long Order_t;
struct  spolyrec
{
  poly      next;
  number    coef;
  Order_t   Order;
  monomial  exp; // make sure that exp is aligned
};

/***************************************************************
 *
 * variables/defines used for managment of monomials
 *
 ***************************************************************/

#define POLYSIZE (sizeof(poly) + sizeof(number) + sizeof(Order_t))
#define POLYSIZEW (POLYSIZE / sizeof(long))
// number of Variables
extern int pVariables;
// size of a monom in bytes - always a multiple of sizeof(void*)
extern int pMonomSize;
// size of a monom in units of sizeof(void*) -- i.e. in words
extern int pMonomSizeW;
// Ceiling((pVariables+1) / sizeof(void*)) == length of exp-vector in words
extern int pVariables1W;
// Ceiling((pVariables) / sizeof(void*))
extern int pVariablesW;
extern int *pVarOffset;
extern int pVarLowIndex;
extern int pVarHighIndex;
extern int pVarCompIndex;

/***************************************************************
 *
 * Primitives for determening/setting  the way exponents are arranged
 *
 ***************************************************************/
#define _pExpIndex(i) pVarOffset[(i)]
#define _pRingExpIndex(r, i)  (r)->VarOffset[(i)]

#define _pCompIndex        pVarOffset[0]
#define _pRingCompIndex(r)  ((r)->VarOffset[0])

// for simple, lex orderings  
extern void pGetVarIndicies_Lex(int nvars, int* VarOffset, int &VarCompIndex,
                                int &VarLowIndex, int &VarHighIndex);
// for simple, revlex orderings
extern void pGetVarIndicies_RevLex(int nvars,int *VarOffset,int &VarCompIndex,
                                   int &VarLowIndex, int &VarHighIndex);
// for all non-simple orderings
extern void pGetVarIndicies(int nvars, int *VarOffset, int &VarCompIndex,
                            int &VarLowIndex, int &VarHighIndex);
// gets var indicies w.r.t. the ring r -- 
// determines which one of three pGetVarIndicies((int nvars, ...) to use
// based on properties of r
extern void pGetVarIndicies(ring r, int *VarOffset, int &VarCompIndex,
                            int &VarLowIndex, int &VarHighIndex);

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

#define _pGetOrder(p)       ((p)->Order)

#if defined(PDEBUG) && PDEBUG != 0
extern Exponent_t pPDSetExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDGetExp(poly p, int v, char* f, int l);
extern Exponent_t pPDIncrExp(poly p, int v, char* f, int l);
extern Exponent_t pPDDecrExp(poly p, int v, char* f, int l);
extern Exponent_t pPDAddExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDMultExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDSubExp(poly p, int v, Exponent_t e, char* f, int l);

extern Exponent_t pPDRingSetExp(ring r,poly p,int v,Exponent_t e,char* f,int l);
extern Exponent_t pPDRingGetExp(ring r,poly p, int v, char* f, int l);

#define _pSetExp(p,v,e)     pPDSetExp(p,v,e,__FILE__,__LINE__)
#define _pGetExp(p,v)       pPDGetExp(p,v,__FILE__,__LINE__)
#define _pIncrExp(p,v)      pPDIncrExp(p,v,__FILE__,__LINE__)
#define _pDecrExp(p,v)      pPDDecrExp(p,v,__FILE__,__LINE__)
#define _pAddExp(p,i,v)     pPDAddExp(p,i,v,__FILE__,__LINE__)
#define _pSubExp(p,i,v)     pPDSubExp(p,i,v,__FILE__,__LINE__)
#define _pMultExp(p,i,v)    pPDMultExp(p,i,v,__FILE__,__LINE__)

#define _pRingSetExp(r,p,v,e)     pPDRingSetExp(r,p,v,e,__FILE__,__LINE__)
#define _pRingGetExp(r,p,v)       pPDRingGetExp(r,p,v,__FILE__,__LINE__)

#else  // ! (defined(PDEBUG) && PDEBUG != 0)

#define _pSetExp(p,v,e)     (p)->exp[_pExpIndex(v)]=(e)
#define _pGetExp(p,v)       (p)->exp[_pExpIndex(v)]
#define _pIncrExp(p,v)      ((p)->exp[_pExpIndex(v)])++
#define _pDecrExp(p,v)      ((p)->exp[_pExpIndex(v)])--
#define _pAddExp(p,i,v)     ((p)->exp[_pExpIndex(i)]) += (v)
#define _pSubExp(p,i,v)     ((p)->exp[_pExpIndex(i)]) -= (v)
#define _pMultExp(p,i,v)    ((p)->exp[_pExpIndex(i)]) *= (v)

#define _pRingSetExp(r,p,v,e)     (p)->exp[_pRingExpIndex(r,v)]=(e)
#define _pRingGetExp(r,p,v)       (p)->exp[_pRingExpIndex(r,v)]

#endif // defined(PDEBUG) && PDEBUG != 0

inline Exponent_t _pGetExpSum(poly p1, poly p2, int i)
{
  int index = _pExpIndex(i);
  return p1->exp[index] + p2->exp[index];
}
inline Exponent_t _pGetExpDiff(poly p1, poly p2, int i)
{
  int index = _pExpIndex(i);
  return p1->exp[index] - p2->exp[index];
}

#define _pGetComp(p)        ((p)->exp[_pCompIndex])
#define _pSetComp(p,k)      _pGetComp(p) = (k)
#define _pIncrComp(p)       _pGetComp(p)++
#define _pDecrComp(p)       _pGetComp(p)--
#define _pAddComp(p,v)      _pGetComp(p) += (v)
#define _pSubComp(p,v)      _pGetComp(p) -= (v)

#define _pRingGetComp(r,p)        ((p)->exp[_pRingCompIndex(r)])
#define _pRingSetComp(r,p,k)      (_pRingGetComp(p) = (k))

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
#ifdef PDEBUG

poly    pDBNew(char *f, int l);
poly    pDBInit(char * f,int l);

void    pDBDelete(poly * a, char * f, int l);
void    pDBDelete1(poly * a, char * f, int l);
void    pDBFree1(poly a, char * f, int l);

poly    pDBCopy(poly a, char *f, int l);
poly    pDBCopy1(poly a, char *f, int l);
poly    pDBHead(poly a, char *f, int l);
poly    pDBHead0(poly a, char *f, int l);
poly    pDBFetchCopy(ring r, poly a, char *f, int l);

void    pDBDelete(poly * a, char * f, int l);
void    pDBDelete1(poly * a, char * f, int l);
void    pDBFree1(poly a, char * f, int l);

#define _pNew()         pDBNew(__FILE__,__LINE__)
#define _pInit()        pDBInit(__FILE__,__LINE__)

#define _pDelete(a)     pDBDelete((a),__FILE__,__LINE__)
#define _pDelete1(a)    pDBDelete1((a),__FILE__,__LINE__)
#define _pFree1(a)                              \
do                                              \
{                                               \
  pDBFree1((a),__FILE__,__LINE__);              \
  (a)=NULL;                                     \
}                                               \
while(0)

#define _pCopy(A)       pDBCopy(A,__FILE__,__LINE__)
#define _pCopy1(A)      pDBCopy1(A, __FILE__,__LINE__)
#define _pHead(A)       pDBHead(A,__FILE__,__LINE__)
#define _pHead0(A)      pDBHead0(A, __FILE__,__LINE__)
#define _pFetchCopy(r,A)    pDBFetchCopy(r, A,__FILE__,__LINE__)

#else // ! PDEBUG

#ifdef MDEBUG
#define _pNew()         (poly) mmDBAllocSpecialized(__FILE__,__LINE__)
#else
#define _pNew()         (poly) mmAllocSpecialized()
#endif

#include <string.h>
inline poly    _pInit(void)
{
#ifdef MDEBUG
  poly p=(poly)mmDBAllocSpecialized(__FILE__,__LINE__);
#else
  poly p=(poly)mmAllocSpecialized();
#endif
  memset(p,0, pMonomSize);
  return p;
}

extern void    _pDelete(poly * a);
extern void    _pDelete1(poly * a);
#ifdef MDEBUG
#define _pFree1(a)       mmDBFreeSpecialized((ADDRESS)a,__FILE__,__LINE__)
#else
#define _pFree1(a)       mmFreeSpecialized((ADDRESS)a)
#endif

extern poly    _pCopy(poly a);
extern poly    _pCopy1(poly a);
extern poly    _pHead(poly a);
extern poly    _pHead0(poly a);
extern poly    _pFetchCopy(ring r,poly a);
#endif // PDEBUG

#define _pCopy2(p1, p2)     memcpyW(p1, p2, pMonomSizeW)

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


#if defined(PDEBUG) && PDEBUG == 1
#define _pMonAddFast(p1, p2)  pDBMonAddFast(p1, p2, __FILE__, __LINE__)
extern  void pDBMonAddFast(poly p1, poly p2, char* f, int l);
inline void __pMonAddFast(poly p1, poly p2)
#else
  DECLARE(void, _pMonAddFast(poly p1, poly p2))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  // OK -- this might be the only place where we are kind of quick and
  // dirty: the following only works correctly if all exponents are
  // positive and the sum of two exponents does not exceed
  // EXPONENT_MAX
  Exponent_t c2 = _pGetComp(p2);
  int i = pVariables1W;
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  // set comp of p2 temporarily to 0, so that nothing is added to comp of p1
  _pSetComp(p2, 0);

  for (;;)
  {
    *s1 += *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
  // reset comp of p2
  _pSetComp(p2, c2);
  _pGetOrder(p1) += _pGetOrder(p2);
}

// Makes p1 a copy of p2 and adds on exponets of p3
#if defined(PDEBUG) && PDEBUG == 1
#define _pCopyAddFast(p1, p2, p3)  pDBCopyAddFast(p1, p2, p3, __FILE__, __LINE__)
extern  void pDBCopyAddFast(poly p1, poly p2, poly p3, char* f, int l);
inline void __pCopyAddFast(poly p1, poly p2, poly p3)
#else
  DECLARE(void, _pCopyAddFast(poly p1, poly p2, poly p3))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;

  p1->next = p2->next;
  p1->coef = p2->coef;

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
  // we first are supposed to do a copy from p2 to p1 -- therefore,
  // component of p1 is set to comp of p2
  _pSetComp(p1, _pGetComp(p2));
  _pGetOrder(p1) = _pGetOrder(p2) + _pGetOrder(p3);
}

// Similar to pCopyAddFast, except that we do not care about the "next" field
#if defined(PDEBUG) && PDEBUG == 1
#define _pCopyAddFast0(p1, p2, p3)  pDBCopyAddFast(p1, p2, p3, __FILE__, __LINE__)
extern  void pDBCopyAddFast(poly p1, poly p2, poly p3, char* f, int l);
inline void __pCopyAddFast0(poly p1, poly p2, poly p3)
#else
  DECLARE(void, _pCopyAddFast0(poly p1, poly p2, poly p3))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;

  p1->coef = p2->coef;

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
  _pSetComp(p1, _pGetComp(p2));
  _pGetOrder(p1) = _pGetOrder(p2) + _pGetOrder(p3);
}

// Similar to pCopyAddFast0, except that we do not recompute the Order,
// but assume that it is the sum of the Order of p2 and p3
#if defined(PDEBUG) && PDEBUG == 1
#define _pCopyAddFastHomog(p1, p2, p3, Order)  \
   pDBCopyAddFastHomog(p1, p2, p3, Order, __FILE__, __LINE__)
extern  void pDBCopyAddFastHomog(poly p1, poly p2, poly p3, Order_t Order,
                                 char* f, int l);
inline void __pCopyAddFastHomog(poly p1, poly p2, poly p3, Order_t Order)
#else
  DECLARE(void, _pCopyAddFastHomog(poly p1, poly p2, poly p3, Order_t Order))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;

  p1->coef = p2->coef;
  p1->Order = Order;

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
  _pSetComp(p1, _pGetComp(p2));
}

#if SIZEOF_LONG == 4

#if SIZEOF_EXPONENT == 1
#define P_DIV_MASK 0x80808080
#else // SIZEOF_EXPONENT == 2
#define P_DIV_MASK 0x80008000
#endif

#else // SIZEOF_LONG == 8

#if SIZEOF_EXPONENT == 1
#define P_DIV_MASK 0x8080808080808080
#elif  SIZEOF_EXPONENT == 2
#define P_DIV_MASK 0x8000800080008000
#else // SIZEOF_EXPONENT == 4
#define P_DIV_MASK 0x8000000080000000
#endif

#endif

DECLARE(BOOLEAN, __pDivisibleBy(poly a, poly b))
{
#ifdef WORDS_BIGENDIAN
  const unsigned long* const lb = (unsigned long*) &(a->exp[0]);;
  const unsigned long* s1 = ((unsigned long*) a) + pMonomSizeW -1;
  const unsigned long* s2 = ((unsigned long*) b) + pMonomSizeW -1;
#else
  const unsigned long* const lb = ((unsigned long*) a) + pMonomSizeW;
  const unsigned long* s1 = (unsigned long*) &(a->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(b->exp[0]);
#endif

  for (;;)
  {
    // Yes, the following is correct, provided that the exponents do
    // not have their first bit set
    if ((*s2 - *s1) & P_DIV_MASK) return FALSE;
#ifdef WORDS_BIGENDIAN
    if (s1 == lb) return TRUE;
    s1--;
    s2--;
#else
    s1++;
    if (s1 == lb) return TRUE;
    s2++;
#endif
  }
}

#if defined(PDEBUG) && PDEBUG == 1
#define _pDivisibleBy(a,b)   pDBDivisibleBy(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy(poly p1, poly p2, char* f, int l);
inline BOOLEAN _pDivisibleBy_orig(poly a, poly b)
#else
inline BOOLEAN _pDivisibleBy(poly a, poly b)
#endif // defined(PDEBUG) && PDEBUG == 1
{
  if ((a!=NULL)&&((_pGetComp(a)==0) || (_pGetComp(a) == _pGetComp(b))))
  {
    return __pDivisibleBy(a,b);
  }
  return FALSE;
}

#if defined(PDEBUG) && PDEBUG == 1
#define _pDivisibleBy1(a,b)   pDBDivisibleBy1(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy1(poly p1, poly p2, char* f, int l);
inline BOOLEAN _pDivisibleBy1_orig(poly a, poly b)
#else
inline BOOLEAN _pDivisibleBy1(poly a, poly b)
#endif // defined(PDEBUG) && PDEBUG == 1
{
  if (_pGetComp(a) == 0 || _pGetComp(a) == _pGetComp(b))
    return __pDivisibleBy(a,b);
  return FALSE;
}

#if defined(PDEBUG) && PDEBUG == 1
#define _pDivisibleBy2(a,b)   pDBDivisibleBy2(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy2(poly p1, poly p2, char* f, int l);
#else
#define _pDivisibleBy2(a,b) __pDivisibleBy(a,b)
#endif // defined(PDEBUG) && PDEBUG == 1


DECLARE(BOOLEAN, _pEqual(poly p1, poly p2))
{
  const long *s1 = (long*) &(p1->exp[0]);
  const long *s2 = (long*) &(p2->exp[0]);
  const long* const lb = s1 + pVariables1W;

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
 * Routines which implement low-level manipulations/operations
 * on exponents and "are allowed" to access single exponetns
 *
 ***************************************************************/

DECLARE(int, __pExpQuerSum2(poly p, int from, int to))
{
  int j = p->exp[from];
  int i = from + 1;

  for(;;)
  {
    if (i > to) return j;
    j += p->exp[i];
    i++;
  }
}

#define _pExpQuerSum(p)  __pExpQuerSum2(p, pVarLowIndex, pVarHighIndex)

inline int _pExpQuerSum1(poly p, int to)
{
  int ei_to = _pExpIndex(to);
  int ei_1 = _pExpIndex(1);
  
  if (ei_1 > ei_to)
    return __pExpQuerSum2(p, ei_to, ei_1);
  else
    return __pExpQuerSum2(p, ei_1, ei_to);
}


inline int _pExpQuerSum2(poly p,int from,int to)
{
  int ei_to = _pExpIndex(to);
  int ei_from = _pExpIndex(from);
  
  if (ei_from > ei_to)
    return __pExpQuerSum2(p, ei_to, ei_from);
  else
    return __pExpQuerSum2(p, ei_from, ei_to);
}

#endif // POLYS_IMPL_H
