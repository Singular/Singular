#ifndef POLYS_IMPL_H
#define POLYS_IMPL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.h,v 1.19 1998-03-17 10:59:57 obachman Exp $ */

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
#include "mmemory.h"

/***************************************************************
 *
 * definition of the poly structure and its fields
 *
 ***************************************************************/

// EXPONENT_TYPE is determined by configure und defined in mod2.h
typedef EXPONENT_TYPE Exponent_t;

#define VARS (100)   /*max. number of variables as constant*/

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
extern int pVarOffset;
extern int pVarLowIndex;
extern int pVarHighIndex;

/***************************************************************
 *
 * Primitives for determening/setting  the way exponents are arranged
 *
 ***************************************************************/
// And here is how we determine the way exponents are stored:
// There are the following four possibilities:
//
//
// BIGENDIAN -- lex order
// e_1, e_2, ... , e_n,comp,.. : pVarOffset = -1,
//                               pVarLowIndex = 0,
//                               pVarHighIndex = pVariables-1
//                               P_COMP_INDEX = pVariables
// BIGENDIAN -- rev lex order
// e_n, ... , e_2, e_1,comp,.. : pVarOffset = pVariables,
//                               pVarLowIndex = 0,
//                               pVarHighIndex = pVariables-1
//                               P_COMP_INDEX = pVariables
// LITTLEENDIAN -- rev lex order
// comp,.., e_1, e_2, ... , e_n : pVarOffset = #(..),
//                                pVarLowIndex = 1 + #(..),
//                                pVarHighIndex = #(..) + pVariables
//                                P_COMP_INDEX = pVariables
// LITTLEENDIAN -- lex order
// comp,..,e_n, .... , e_2, e_1 : pVarOffset = pVariables + 1 + #(..)
//                                pVarLowIndex = 1 + #(..)
//                                pVarHighIndex = #(..) + pVariables
//                                P_COMP_INDEX = pVariables
//
// Furthermore, the size of the exponent vector is always a multiple
// of the word size -- "empty exponents" (exactly #(..) ones) are
// filled in between comp and first/last exponent -- i.e. comp and
// first/last exponent might not be next to each other

#ifdef WORDS_BIGENDIAN

#define _pHasReverseExp    (pVarOffset != -1)
#define _pExpIndex(i)                           \
  (pVarOffset == -1 ? (i) - 1 : pVarOffset - (i))
#define _pRingExpIndex(r, i)                           \
  ((r)->VarOffset == -1 ? (i) - 1 : (r)->VarOffset - (i))

#else // ! WORDS_BIGENDIAN

#define _pHasReverseExp    (pVarOffset > (SIZEOF_LONG / SIZEOF_EXPONENT) - 1)
#define _pExpIndex(i)                                   \
  (pVarOffset > (SIZEOF_LONG / SIZEOF_EXPONENT) - 1 ?   \
   pVarOffset - (i) : pVarOffset + (i))
#define _pRingExpIndex(r, i)                                \
  ((r)->VarOffset > (SIZEOF_LONG / SIZEOF_EXPONENT) - 1 ?   \
   (r)->VarOffset - (i) : (r)->VarOffset + (i))

#endif // WORDS_BIGENDIAN

inline void pGetVarIndicies_Lex(int nvars, int &VarOffset,
                                int &VarLowIndex, int &VarHighIndex)
{
#ifdef WORDS_BIGENDIAN
  VarOffset    = -1;
  VarLowIndex  = 0;
  VarHighIndex = nvars - 1;
#else //  ! WORDS_BIGENDIAN
  long temp = (nvars+1)*sizeof(Exponent_t);
  if ((temp % sizeof(long)) == 0)
    temp = temp / sizeof(long);
  else
    temp = (temp / sizeof(long)) + 1; // now temp == nvars1W
  VarHighIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarLowIndex  = VarHighIndex - nvars + 1;
  VarOffset    = VarHighIndex + 1;
#endif // WORDS_BIGENDIAN
}

#define pSetVarIndicies_Lex(nvars) \
  pGetVarIndicies_Lex(nvars,pVarOffset,pVarLowIndex,pVarHighIndex)
  

inline void pGetVarIndicies_RevLex(int nvars, int &VarOffset, 
                                   int &VarLowIndex, int &VarHighIndex)
{
#ifdef WORDS_BIGENDIAN
  VarOffset    = nvars;
  VarLowIndex  = 0;
  VarHighIndex = nvars-1;
#else //  ! WORDS_BIGENDIAN
  long temp = (nvars+1)*sizeof(Exponent_t);
  if ((temp % sizeof(long)) == 0)
    temp = temp / sizeof(long);
  else
    temp = (temp / sizeof(long)) + 1;
  VarHighIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarLowIndex  = VarHighIndex - nvars + 1;
  VarOffset    = VarLowIndex - 1;
#endif // WORDS_BIGENDIAN
}
#ifdef WORDS_BIGENDIAN
#define pSetVarIndicies_RevLex(nvars) \
 pGetVarIndicies_RevLex(nvars,pVarOffset,pCompIndex,pVarLowIndex,pVarHighIndex)
#else
#define pSetVarIndicies_RevLex(nvars) \
 pGetVarIndicies_RevLex(nvars,pVarOffset,pVarLowIndex, pVarLowIndex,pVarHighIndex)
#endif

// The default settings:
inline void pGetVarIndicies(int nvars,
                            int &VarOffset, int &VarCompIndex,
                            int &VarLowIndex, int &VarHighIndex)
{
  pGetVarIndicies_Lex(nvars,VarOffset,VarCompIndex,VarLowIndex,VarHighIndex);
}

// gets var indicies w.r.t. the ring r
extern void pGetVarIndicies(ring r,
                            int &VarOffset, int &VarCompIndex,
                            int &VarLowIndex, int &VarHighIndex);

#define pSetVarIndicies(nvars) \
  pGetVarIndicies(nvars, pVarOffset, pCompIndex, pVarLowIndex, pVarHighIndex)


#else  // ! COMP_FAST
#define _pExpIndex(i)       (i)
#define _pRingExpIndex(r,i) (i)
#endif // COMP_FAST

/***************************************************************
 *
 * Primitives for accessing and seeting fields of a poly
 *
 ***************************************************************/
#define _pNext(p)           ((p)->next)
#define _pIter(p)           ((p) = (p)->next)

#define _pGetCoeff(p)       ((p)->coef)
#define _pSetCoeff(p,n)     {nDelete(&((p)->coef));(p)->coef=n;}
#define _pSetCoeff0(p,n)    (p)->coef=n

#ifndef TEST_MAC_DEBUG
#define _pGetOrder(p)       ((p)->Order)
#else
#define _pGetOrder(p)       ((p)->MOrder)
#endif

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

#define _pSetComp(p,k)      (p)->exp[pCompIndex] = (k)
#define _pGetComp(p)        (p)->exp[pCompIndex]
#define _pIncrComp(p)       (p)->exp[pCompIndex]++
#define _pDecrComp(p)       (p)->exp[pCompIndex]--
#define _pAddComp(p,v)      (p)->exp[pCompIndex] += (v)
#define _pSubComp(p,v)      (p)->exp[pCompIndex] -= (v)

#ifdef COMP_FAST
#define _pRingSetComp(r,p,k)      (p)->exp[(r)->CompIndex] = (k)
#define _pRingGetComp(r,p)        (p)->exp[(r)->CompIndex]
#else
#define _pRingSetComp(r,p,k)      _pSetComp(p,k)
#define _pRingGetComp(r,p)        _pGetComp(p)
#endif


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
#ifdef COMP_FAST
#define _pFetchCopy(r,A)    pDBFetchCopy(r, A,__FILE__,__LINE__)
#else
#define _pFetchCopy(r,p)    pOrdPolyInsertSetm(pCopy(p))
#endif

#else // ! PDEBUG

#ifdef MDEBUG
#define _pNew()         (poly) mmDBAllocSpecialized(__FILE__,__LINE__)
#else
#define _pNew()         (poly) mmAllocSpecialized()
// #define _pNew() _pInit()
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
#ifdef COMP_FAST
extern poly    _pFetchCopy(ring r,poly a);
#else
#define _pFetchCopy(r,p)  pOrdPolyInsertSetm(pCopy(p))
#endif

#endif // PDEBUG

#define _pCopy2(p1, p2)     memcpyW(p1, p2, pMonomSizeW)

// Here is a handy Macro which disables inlining when run with
// profiling and enables it otherwise

#ifdef DO_PROFILE

#ifndef POLYS_IMPL_CC
#define DECLARE(type, arglist) type arglist; \
   static type dummy_##arglist
#else
#define DECLARE(type, arglist) type arglist
#endif // POLYS_IMPL_CC

#else //! DO_PROFILE

#define DECLARE(type, arglist ) inline type arglist

#endif // DO_PROFILE


/***************************************************************
 *
 * Routines which work on vectors instead of single exponents
 *
 ***************************************************************/

#ifdef COMP_FAST

// nice declaration isn't it ??
#if defined(PDEBUG) && PDEBUG == 1
#define pMonAddFast(p1, p2)  pDBMonAddFast(p1, p2, __FILE__, __LINE__)
extern  void pDBMonAddFast(poly p1, poly p2, char* f, int l);
inline void _pMonAddFast(poly p1, poly p2)
#else
  DECLARE(void, pMonAddFast(poly p1, poly p2))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  // OK -- this might be the only place where we are kind of quick and
  // dirty: the following only works correctly if all exponents are
  // positive and the sum of two exponents does not exceed
  // EXPONENT_MAX
#ifndef COMP_NO_EXP_VECTOR_OPS
  Exponent_t c2 = _pGetComp(p2);
  int i = pVariables1W;
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  // set comp of p2 temporarily to 0, so that nothing is added to comp of p1
  _pSetComp(p2, 0);
#else
  int i = pVariables;
  Exponent_pt s1 = &(p1->exp[pVarLowIndex]);
  Exponent_pt s2 = &(p2->exp[pVarLowIndex]);
#endif

  for (;;)
  {
    *s1 += *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
#ifndef COMP_NO_EXP_VECTOR_OPS
  // reset comp of p2
  _pSetComp(p2, c2);
#endif
#ifdef TEST_MAC_ORDER
  if (bNoAdd) bSetm(p1);else
#endif
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
  p1->next = p2->next;
  p1->coef = p2->coef;
//  memset(p1, 0, pMonomSize);

  //memset(p1,0,pMonomSize);
#ifndef COMP_NO_EXP_VECTOR_OPS
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;
#else
  Exponent_t* s1 = (Exponent_t*) &(p1->exp[pVarLowIndex]);
  const Exponent_t* s2 = (Exponent_t*) &(p2->exp[pVarLowIndex]);
  const Exponent_t* s3 = (Exponent_t*) &(p3->exp[pVarLowIndex]);
  const Exponent_t* const ub = s3 + pVariables;
// need to zero the "fill in" slots (i.e., empty exponents)
#ifdef  WORDS_BIGENDIAN
  *((unsigned long *) ((unsigned long*) p1) + pMonomSizeW -1) = 0;
#else
  *((unsigned long *) p1->exp) = 0;
#endif
#endif

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
  #ifdef TEST_MAC_ORDER
  if (bNoAdd) bSetm(p1);else
  #endif
  _pGetOrder(p1) = _pGetOrder(p2) + _pGetOrder(p3);
}

// Similar to pCopyAddFast, except that we assume that the component
// of p2 and p3 is zero component
#if defined(PDEBUG) && PDEBUG == 1
#define _pCopyAddFast1(p1, p2, p3)  pDBCopyAddFast(p1, p2, p3, __FILE__, __LINE__)
extern  void pDBCopyAddFast(poly p1, poly p2, poly p3, char* f, int l);
inline void __pCopyAddFast1(poly p1, poly p2, poly p3)
#else
  DECLARE(void, _pCopyAddFast1(poly p1, poly p2, poly p3))
#endif // defined(PDEBUG) && PDEBUG == 1
{
#ifndef COMP_NO_EXP_VECTOR_OPS
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;
#else
  Exponent_t* s1 = (Exponent_t*) &(p1->exp[pVarLowIndex]);
  const Exponent_t* s2 = (Exponent_t*) &(p2->exp[pVarLowIndex]);
  const Exponent_t* s3 = (Exponent_t*) &(p3->exp[pVarLowIndex]);
  const Exponent_t* const ub = s3 + pVariables;
#ifdef  WORDS_BIGENDIAN
  *((unsigned long *) ((unsigned long*) p1) + pMonomSizeW -1) = 0;
#else
  *((unsigned long *) p1->exp) = 0;
#endif
#endif

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
  #ifdef TEST_MAC_ORDER
  if (bNoAdd) bSetm(p1);else
  #endif
  _pGetOrder(p1) = _pGetOrder(p2) + _pGetOrder(p3);
}


#ifndef COMP_NO_EXP_VECTOR_OPS

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

#else //  ! COMP_NO_EXP_VECTOR_OPS

DECLARE(BOOLEAN, __pDivisibleBy(poly a, poly b))
{
#ifdef WORDS_BIGENDIAN
  const Exponent_t* s1 = &(a->exp[pVarHighIndex]);
  const Exponent_t* s2 = &(b->exp[pVarHighIndex]);
  const Exponent_t* lb = s1 - pVariables;

  for (;;)
  {
    if (*s1 > *s2) return FALSE;
    s1--;
    if (s1 == lb) return TRUE;
    s2--;
  }

#else // !WORDS_BIGENDIAN
  const Exponent_t* s1 = &(a->exp[pVarLowIndex]);
  const Exponent_t* s2 = &(b->exp[pVarLowIndex]);
  const Exponent_t* lb = s1 + pVariables;

  for (;;)
  {
   if (*s1 > *s2) return FALSE;
   s1++;
   if (s1 == lb) return TRUE;
   s2++;
  }
#endif  // WORDS_BIGENDIAN
}

#endif //  COMP_NO_EXP_VECTOR_OPS

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
#ifndef COMP_NO_EXP_VECTOR_OPS
  const long *s1 = (long*) &(p1->exp[0]);
  const long *s2 = (long*) &(p2->exp[0]);
  const long* const lb = s1 + pVariables1W;
#else
  const Exponent_t *s1 = (Exponent_t*) &(p1->exp[pVarLowIndex]);
  const Exponent_t *s2 = (Exponent_t*) &(p2->exp[pVarLowIndex]);
  const Exponent_t* const lb = s1 + pVariables;
  if (_pGetComp(p1) != _pGetComp(p2)) return FALSE;
#endif

  for(;;)
  {
    if (*s1 != *s2) return FALSE;
    s1++;
    if (s1 == lb) return TRUE;
    s2++;
  }
}

inline void _pGetExpV(poly p, Exponent_t *ev)
{
  if (_pHasReverseExp)
  {
    for (int i = pVarLowIndex, j = pVariables; j; i++, j--)
      ev[j] = p->exp[i];
  }
  else
    memcpy(&ev[1], &(p->exp[pVarLowIndex]), pVariables*sizeof(Exponent_t));
  ev[0] = _pGetComp(p);
}

extern pSetmProc pSetm;
inline void _pSetExpV(poly p, Exponent_t *ev)
{
  if (_pHasReverseExp)
  {
    for (int i = pVarLowIndex, j = pVariables; j; i++, j--)
      p->exp[i] = ev[j];
  }
  else
    memcpy(&(p->exp[pVarLowIndex]), &ev[1], pVariables*sizeof(Exponent_t));
  _pSetComp(p, ev[0]);
  pSetm(p);
}

#else // ! COMP_FAST

DECLARE(BOOLEAN, _pDivisibleBy(poly a, poly b))
{
  if ((a!=NULL)&&((a->exp[0]==0) || (a->exp[0] == b->exp[0])))
  {
    int i=pVariables;
    short *e1=&(a->exp[1]);
    short *e2=&(b->exp[1]);
    if ((*e1) > (*e2)) return FALSE;
    do
    {
      i--;
      if (i == 0) return TRUE;
      e1++;
      e2++;
    } while ((*e1) <= (*e2));
  }
  return FALSE;
}

#define _pDivisibleBy1(a,b) _pDivisibleBy(a,b)
#define _pDivisibleBy2(a,b) _pDivisibleBy(a,b)

#ifdef TEST_MAC_ORDER
DECLARE(void, pMonAddFast(poly a, poly m))
{
  for(int ii =pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];\
  if (bNoAdd) bSetm(a); else
  _pGetOrder(a) += _pGetOrder(m);
}
#else
DECLARE(void, pMonAddFast(poly a, poly m))
{
  for(int ii =pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];\
  _pGetOrder(a) += _pGetOrder(m);
}
#endif

DECLARE(BOOLEAN, _pEqual(poly p1, poly p2))
{
  int i;
  short *e1=p1->exp;
  short *e2=p2->exp;

  if (p1->Order != p2->Order) return FALSE;
  for (i=pVariables; i>=0; i--,e1++,e2++)
    if (*e1 != *e2) return FALSE;
  return TRUE;
}

#define _pGetExpV(p,e)    memcpy((e),(p)->exp,(pVariables+1)*sizeof(short));
//void    pSetExpV(poly p, short * exp);
#define _pSetExpV(p,e) {memcpy((p)->exp,(e),(pVariables+1)*sizeof(short));pSetm(p);}

#endif // COMP_FAST

/***************************************************************
 *
 * Routines which implement low-level manipulations/operations on exponents
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

#ifdef COMP_FAST
#define _pExpQuerSum(p)  __pExpQuerSum2(p, pVarLowIndex, pVarHighIndex)

#define _pExpQuerSum1(p,to) \
 (_pHasReverseExp ? \
    __pExpQuerSum2(p, _pExpIndex(to), _pExpIndex(1)) : \
    __pExpQuerSum2(p, _pExpIndex(1), _pExpIndex(to)))

#define _pExpQuerSum2(p,from,to) \
 (_pHasReverseExp ? \
    __pExpQuerSum2(p, _pExpIndex(to), _pExpIndex(from)) : \
    __pExpQuerSum2(p, _pExpIndex(from), _pExpIndex(to)))
#else

#define _pExpQuerSum(p)             __pExpQuerSum2(p, 1, pVariables)
#define _pExpQuerSum1(p, to)        __pExpQuerSum2(p, 1, to)
#define _pExpQuerSum2(p, from, to)  __pExpQuerSum2(p, from, to)

#endif

/***************************************************************
 *
 * Routines which implement macaulay ordering routines
 *
 ***************************************************************/
#ifdef TEST_MAC_ORDER

DECLARE(void, _bSetm0(poly p))
{

  int i=1;
  int ord = -INT_MAX;
  Exponent_t *ep;

#ifdef COMP_FAST
  if(_pHasReverseExp)
  {
    ep=&(p->exp[pVarHighIndex]);
    int *ip=bBinomials+(*ep); /*_pGetExp(p,1);*/
    loop
    {
      ord += (*ip);
      if (i==pVariables) break;
      i++;
      //ip+=bHighdeg_1+_pGetExp(p,i);
      ep--;
      ip+=bHighdeg_1+(*ep);
    }
  }
  else
  {
    ep=&(p->exp[pVarLowIndex]);
    int *ip=bBinomials+(*ep); /*_pGetExp(p,1);*/
    loop
    {
      ord += (*ip);
      if (i==pVariables) break;
      i++;
      //ip+=bHighdeg_1+_pGetExp(p,i);
      ep++;
      ip+=bHighdeg_1+(*ep);
    }
  }
  #ifdef TEST_MAC_DEBUG
  p->Order=_pExpQuerSum(p);
  p->MOrder=ord;
  #else
  p->Order=ord;
  #endif
#else
    int *ip=bBinomials+_pGetExp(p,1);
    loop
    {
      ord += (*ip);
      if (i==pVariables) break;
      i++;
      ip+=bHighdeg_1+_pGetExp(p,i);
    }
  #ifdef TEST_MAC_DEBUG
  p->Order=pTotaldegree(p);
  p->MOrder=ord;
  #else
  p->Order=ord;
  #endif
#endif
}

DECLARE(void, _bSetm(poly p))
{
  int ord = _pExpQuerSum(p);

  if (ord<bHighdeg)
    _bSetm0(p);
  else
  {
  #ifdef TEST_MAC_DEBUG
    p->MOrder=ord;
  #endif
    p->Order=ord;
  }
}

#ifdef COMP_FAST
// ordering dp,c or c,dp, general case
#if defined(PDEBUG) && PDEBUG == 1
#define pbMonAddFast(p1, p2)  pDBMonAddFast(p1, p2, __FILE__, __LINE__)
extern  void pbDBMonAddFast(poly p1, poly p2, char* f, int l);
inline void _pbMonAddFast(poly p1, poly p2)
#else
#define pbMonAddFast(p1, p2)  _pbMonAddFast(p1, p2)
DECLARE(void, _pbMonAddFast(poly p1, poly p2))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  // OK -- this might be the only place where we are kind of quick and
  // dirty: the following only works correctly if all exponents are
  // positive and the sum of two exponents does not exceed
  // EXPONENT_MAX
#ifndef COMP_NO_EXP_VECTOR_OPS
  Exponent_t c2 = _pGetComp(p2);
  int i = pVariables1W;
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  // set comp of p2 temporarily to 0, so that nothing is added to comp of p1
  _pSetComp(p2, 0);
#else
  int i = pVariables;
  Exponent_pt s1 = &(p1->exp[pVarLowIndex]);
  Exponent_pt s2 = &(p2->exp[pVarLowIndex]);
#endif

  for (;;)
  {
    *s1 += *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
#ifndef COMP_NO_EXP_VECTOR_OPS
  // reset comp of p2
  _pSetComp(p2, c2);
#endif
  if ((_pGetOrder(p1)|_pGetOrder(p2))>0) // i.e. overflow of mac order
    _pGetOrder(p1) += _pGetOrder(p2);
  else
    _bSetm(p1);
}

// ordering dp,c or c,dp, below degree limit
#if defined(PDEBUG) && PDEBUG == 1
#define pbMonAddFast0(p1, p2)  pbDBMonAddFast0(p1, p2, __FILE__, __LINE__)
extern  void pbDBMonAddFast0(poly p1, poly p2, char* f, int l);
inline void _pbMonAddFast0(poly p1, poly p2)
#else
  DECLARE(void, pbMonAddFast0(poly p1, poly p2))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  // OK -- this might be the only place where we are kind of quick and
  // dirty: the following only works correctly if all exponents are
  // positive and the sum of two exponents does not exceed
  // EXPONENT_MAX
#ifndef COMP_NO_EXP_VECTOR_OPS
  Exponent_t c2 = _pGetComp(p2);
  int i = pVariables1W;
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  // set comp of p2 temporarily to 0, so that nothing is added to comp of p1
  _pSetComp(p2, 0);
#else
  int i = pVariables;
  Exponent_pt s1 = &(p1->exp[pVarLowIndex]);
  Exponent_pt s2 = &(p2->exp[pVarLowIndex]);
#endif

  for (;;)
  {
    *s1 += *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
#ifndef COMP_NO_EXP_VECTOR_OPS
  // reset comp of p2
  _pSetComp(p2, c2);
#endif
  _bSetm0(p1);
}

// ordering dp,c or c,dp, below degree limit
// Makes p1 a copy of p2 and adds on exponets of p3
#if defined(PDEBUG) && PDEBUG == 1
#define _pbCopyAddFast0(p1, p2, p3)  pDBCopyAddFast(p1, p2, p3, __FILE__, __LINE__)
inline void __pbCopyAddFast0(poly p1, poly p2, poly p3)
#else
  DECLARE(void, _pbCopyAddFast0(poly p1, poly p2, poly p3))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  p1->next = p2->next;
  p1->coef = p2->coef;

#ifndef COMP_NO_EXP_VECTOR_OPS
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;
#else
  Exponent_t* s1 = (Exponent_t*) &(p1->exp[pVarLowIndex]);
  const Exponent_t* s2 = (Exponent_t*) &(p2->exp[pVarLowIndex]);
  const Exponent_t* s3 = (Exponent_t*) &(p3->exp[pVarLowIndex]);
  const Exponent_t* const ub = s3 + pVariables;
// need to zero the "fill in" slots (i.e., empty exponents)
#ifdef  WORDS_BIGENDIAN
  *((unsigned long*) p1 + pMonomSizeW -1) = 0;
#else
  *((unsigned long *) p1->exp) = 0;
#endif
#endif

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
  _bSetm0(p1);
}

// Similar to pCopyAddFast, except that we assume that the component
// of p2 and p3 is zero component
#if defined(PDEBUG) && PDEBUG == 1
#define _pbCopyAddFast10(p1, p2, p3)  pbDBCopyAddFast0(p1, p2, p3, __FILE__, __LINE__)
extern  void pbDBCopyAddFast0(poly p1, poly p2, poly p3, char* f, int l);
inline void __pbCopyAddFast10(poly p1, poly p2, poly p3)
#else
  DECLARE(void, _pbCopyAddFast10(poly p1, poly p2, poly p3))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  p1->next = p2->next;
  p1->coef = p2->coef;

#ifndef COMP_NO_EXP_VECTOR_OPS
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;
#else
  Exponent_t* s1 = (Exponent_t*) &(p1->exp[pVarLowIndex]);
  const Exponent_t* s2 = (Exponent_t*) &(p2->exp[pVarLowIndex]);
  const Exponent_t* s3 = (Exponent_t*) &(p3->exp[pVarLowIndex]);
  const Exponent_t* const ub = s3 + pVariables;
#ifdef  WORDS_BIGENDIAN
  *((unsigned long*) p1 + pMonomSizeW -1) = 0;
#else
  *((unsigned long *) p1->exp) = 0;
#endif
#endif

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
  _bSetm0(p1);
}

#endif
#endif

#endif // POLYS_IMPL_H

