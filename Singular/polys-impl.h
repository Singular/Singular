#ifndef POLYS_IMPL_H
#define POLYS_IMPL_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.h,v 1.1 1997-12-03 16:58:57 obachman Exp $ */

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

#define VARS (100)   /*max. number of variables as constant*/

typedef Exponent_t  monomial[VARS + 1];
typedef Exponent_t* Exponent_pt;

typedef long Order_t;
// make sure that exp is aligned
struct  spolyrec
{
  poly      next;
  number    coef;
  Order_t   Order;
  monomial  exp;
};

/***************************************************************
 * MACROS CONTROLING MONOMIAL COMPARIONS:

 * COMP_TRADITIONAL 
     Keeps the traditional comparison routines
     defined -- needed as long as their might be comparisons with
     negativ components.
     All the traditional routines are prefixed by t_
     
 * COMP_FAST 
     Implements monomial comparisons using the fast
     techniques. Undefine in case there are problems. All the fast
     routines are prefixed by f_
     
 * COMP_STATISTIC  
     Provides several routines for accumulating statistics on monomial
     comparisons
     
 * COMP_DEBUG 
     Turns on debugging of COMP_FAST by comparing the resutsl of fast
     comparison with traditional comparison

 ***************************************************************/
#define COMP_FAST
#define COMP_TRADITIONAL

#if defined(COMP_FAST) && ! defined(NDEBUG)
#define COMP_DEBUG
#endif

// some relations between these flags
#ifdef COMP_DEBUG
#define COMP_TRADITIONAL
#define COMP_FAST
#undef  COMP_PROFILE
#undef  COMP_STATISTICS
#endif // COMP_DEBUG

#ifdef COMP_STATISTICS
#undef COMP_FAST
#endif // COMP_STATISTICS

// for the time being COMP_TRADITIONAL always has to be defined, since
// traditional routines are needed in spolys.cc -- monomials with
// negative exponents are compared there!
#define COMP_TRADITIONAL

/***************************************************************
 *
 * variables used for storage management and monomial traversions
 *
 ***************************************************************/

// size of poly without exponents
#define POLYSIZE (sizeof(poly) + sizeof(number) + sizeof(Order_t))
#define POLYSIZEW (POLYSIZE / sizeof(long))
// number of Variables
extern int pVariables;
// size of a monom in bytes - always a multiple of sizeof(void*)
extern int pMonomSize;
// size of a monom in units of sizeof(void*) -- i.e. in words
extern int pMonomSizeW;
#ifdef COMP_FAST
// Ceiling((pVariables+1) / sizeof(void*))
extern int pVariables1W;
// Ceiling((pVariables) / sizeof(void*))
extern int pVariablesW;
extern int pCompIndex;
extern int pVarOffset;
extern int pVarLowIndex;
extern int pVarHighIndex;
#else
#define pCompIndex 0
#endif

/***************************************************************
 *
 * Primitives for determening/setting  the way exponents are arranged
 *
 ***************************************************************/
// And here is how we determine the way exponents are stored:
// There are the following four possibilities:
// comp, e_1, e_2, ... , e_n : pVarOffset = 0,
//                             pCompIndex = 0,
//                             pVarLowIndex = 1, pVarHighIndex = pVariables
// e_1, e_2, ... , e_n, comp : pVarOffset = -1,
//                             pCompIndex = pVariables
//                             pVarLowIndex = 0, pVarHighIndex = pVariables-1
// comp, e_n, .... , e_2, e_1: pVarOffset = pVariables + 1,
//                             pCompIndex = 0
//                             pVarLowIndex = 1, pVarHighIndex = pVariables
// e_n, ... , e_2, e_1, comp : pVarOffset = pVariables,
//                             pCompIndex = pVariables
//                             pVarLowIndex = 0, pVarHighIndex = pVariables-1
// Furthermore, the last field is always at an anignment border, and
// there might be "empty" fields between comp and e_1 (resp. e_n).
// Which one of these variants is used depends on the byte ordering of
// the machine and on the choosen variable ordering (see below).

#ifdef COMP_FAST

#ifdef WORDS_BIGENDIAN
#define _pReverseExp    (pVarOffset == -1)
#define _pExpIndex(i)                           \
  (pVarOffset == -1 ? (i) - 1 : pVarOffset - (i))
#else
#define _pReverseExp    (pVarOffset > (SIZEOF_LONG / SIZEOF_EXPONENT) - 1)
#define _pExpIndex(i) \
  (pVarOffset > (SIZEOF_LONG / SIZEOF_EXPONENT) - 1 ? pVarOffset - (i) : pVarOffset + (i))
#endif
    
// The default settings:
inline void pGetVarIndicies(int nvars,
                            int &VarOffset, int &VarCompIndex,
                            int &VarLowIndex, int &VarHighIndex)
{   
  VarOffset    = 0;
  VarCompIndex = 0;
  VarLowIndex  = 1;
  VarHighIndex = nvars;
}
#define pSetVarIndicies(nvars) \
  pGetVarIndicies(nvars, pVarOffset, pCompIndex, pVarLowIndex, pVarHighIndex)

// For (Lex, Comp) Comparisons
inline void pGetVarIndicies_Lex(int nvars,
                                int &VarOffset, int &VarCompIndex,
                                int &VarLowIndex, int &VarHighIndex)
{
  long temp = (nvars+1)*sizeof(Exponent_t);
  if ((temp % sizeof(long)) == 0)
    temp = temp / sizeof(long);
  else
    temp = (temp / sizeof(long)) + 1; // now temp == nvars1W
#ifdef WORDS_BIGENDIAN
  // e_1, e_2, ..., e_n, ..., comp : comp NEEDS TO BE be in last available slot
  VarCompIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarOffset    = -1;
  VarLowIndex  = 0;
  VarHighIndex = nvars - 1;
#else //  ! WORDS_BIGENDIAN
  // comp, ..., e_n, .... , e_2, e_1
  VarHighIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarCompIndex = 0;
  VarOffset    = VarHighIndex + 1;
  VarLowIndex  = VarOffset - nvars;
#endif // WORDS_BIGENDIAN
}
#define pSetVarIndicies_Lex(nvars) \
  pGetVarIndicies_Lex(nvars, pVarOffset, pCompIndex, pVarLowIndex, pVarHighIndex)

inline void pGetVarIndicies_RevLex(int nvars,
                                   int &VarOffset, int &VarCompIndex,
                                   int &VarLowIndex, int &VarHighIndex)
{
  long temp = (nvars+1)*sizeof(Exponent_t);
  if ((temp % sizeof(long)) == 0)
    temp = temp / sizeof(long);
  else
    temp = (temp / sizeof(long)) + 1;
#ifdef WORDS_BIGENDIAN
  // e_n, .... , e_2, e_1, ..., comp
  VarCompIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarOffset    = nvars;
  VarLowIndex  = 0;
  VarHighIndex = nvars-1;
#else //  ! WORDS_BIGENDIAN
  // comp, ..., e_1, e_2, ... , e_n
  VarHighIndex = temp * sizeof(long)/sizeof(Exponent_t) - 1;
  VarCompIndex = 0;
  VarLowIndex  = pVarHighIndex - nvars + 1;
  VarOffset    = VarLowIndex - 1;
#endif // WORDS_BIGENDIAN
}
#define pSetVarIndicies_RevLex(nvars) \
  pGetVarIndicies_RevLex(nvars, pVarOffset, pCompIndex, pVarLowIndex, pVarHighIndex)

#else  // ! COMP_FAST
#define _pExpIndex(i)       (i)
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

#define _pGetOrder(p)       ((p)->Order)

#if defined(PDEBUG) && PDEBUG != 0
extern Exponent_t pPDSetExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDGetExp(poly p, int v, char* f, int l);
extern Exponent_t pPDIncrExp(poly p, int v, char* f, int l);
extern Exponent_t pPDDecrExp(poly p, int v, char* f, int l);
extern Exponent_t pPDAddExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDMultExp(poly p, int v, Exponent_t e, char* f, int l);
extern Exponent_t pPDSubExp(poly p, int v, Exponent_t e, char* f, int l);

#define _pSetExp(p,v,e)     pPDSetExp(p,v,e,__FILE__,__LINE__)
#define _pGetExp(p,v)       pPDGetExp(p,v,__FILE__,__LINE__)
#define _pIncrExp(p,v)      pPDIncrExp(p,v,__FILE__,__LINE__)
#define _pDecrExp(p,v)      pPDDecrExp(p,v,__FILE__,__LINE__)
#define _pAddExp(p,i,v)     pPDAddExp(p,i,v,__FILE__,__LINE__)
#define _pSubExp(p,i,v)     pPDSubExp(p,i,v,__FILE__,__LINE__)
#define _pMultExp(p,i,v)    pPDMultExp(p,i,v,__FILE__,__LINE__)

#else  // ! (defined(PDEBUG) && PDEBUG != 0)

#define _pSetExp(p,v,e)     (p)->exp[_pExpIndex(v)]=(e)
#define _pGetExp(p,v)       (p)->exp[_pExpIndex(v)]
#define _pIncrExp(p,v)      ((p)->exp[_pExpIndex(v)])++
#define _pDecrExp(p,v)      ((p)->exp[_pExpIndex(v)])--
#define _pAddExp(p,i,v)     ((p)->exp[_pExpIndex(i)]) += (v)
#define _pSubExp(p,i,v)     ((p)->exp[_pExpIndex(i)]) -= (v)
#define _pMultExp(p,i,v)    ((p)->exp[_pExpIndex(i)]) *= (v)

#endif // defined(PDEBUG) && PDEBUG != 0

inline Exponent_t _pGetExpSum(poly p1, poly p2, int i)
{
  int index = _pExpIndex(i);
  return (long) p1->exp[index] + (long) p2->exp[index];
}
inline Exponent_t _pGetExpDiff(poly p1, poly p2, int i)
{
  int index = _pExpIndex(i);
  return (long) p1->exp[index] - (long) p2->exp[index];
}

#define _pSetComp(p,k)      (p)->exp[pCompIndex] = (k)
#define _pGetComp(p)        (p)->exp[pCompIndex]
#define _pIncrComp(p)       (p)->exp[pCompIndex]++
#define _pDecrComp(p)       (p)->exp[pCompIndex]--
#define _pAddComp(p,v)      (p)->exp[pCompIndex] += (v)
#define _pSubComp(p,v)      (p)->exp[pCompIndex] -= (v)

/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/
#ifdef MDEBUG

poly    pDBNew(char *f, int l);
poly    pDBInit(char * f,int l);

void    pDBDelete(poly * a, char * f, int l);
void    pDBDelete1(poly * a, char * f, int l);
void    pDBFree1(poly a, char * f, int l);

poly    pDBCopy(poly a, char *f, int l);
poly    pDBCopy1(poly a, char *f, int l);
poly    pDBHead(poly a, char *f, int l);
poly    pDBHead0(poly a, char *f, int l);


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

#else // ! MDEBUG

#define _pNew()         (poly) mmAllocSpecialized()

poly    _pInit(void);

extern void    _pDelete(poly * a);
extern void    _pDelete1(poly * a);
#define _pFree1(a)       mmFreeSpecialized((ADDRESS)a)

extern poly    _pCopy(poly a);
extern poly    _pCopy1(poly a);
extern poly    _pHead(poly a);
extern poly    _pHead0(poly a);

#endif // MDEBUG

#define _pCopy2(p1, p2)     memcpyW(p1, p2, pMonomSizeW)

// Here is a handy Macro which disables inlining when run with
// profiling and enables it otherwise

#ifdef DO_PROFIL

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
  Exponent_t c2 = _pGetComp(p2);
  int i = pVariables1W;
  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  _pSetComp(p2, 0);
  for (;;)
  {
    *s1 += *s2;
    i--;
    if (i==0) break;
    s1++;
    s2++;
  }
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
  p1->next = p2->next;
  p1->coef = p2->coef;

  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;

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
  p1->next = p2->next;
  p1->coef = p2->coef;

  unsigned long* s1 = (unsigned long*) &(p1->exp[0]);
  const unsigned long* s2 = (unsigned long*) &(p2->exp[0]);
  const unsigned long* s3 = (unsigned long*) &(p3->exp[0]);
  const unsigned long* const ub = s3 + pVariables1W;

  for (;;)
  {
    *s1 = *s2 + *s3;
    s3++;
    if (s3 == ub) break;
    s1++;
    s2++;
  }
  _pGetOrder(p1) = _pGetOrder(p2) + _pGetOrder(p3);
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


inline BOOLEAN __pDivisibleBy(poly a, poly b)
{
  const unsigned long* s1;
  const unsigned long* s2;
  const unsigned long* lb;
  
#ifdef WORDS_BIGENDIAN
  lb = (unsigned long*) &(a->exp[0]);
  if (pVariables & ((SIZEOF_LONG / SIZEOF_EXPONENT) - 1))
  {
    // now pVariables == pVariables1W, i.e. there are exponents in the
    // "first" word of exponentvector
    s1 = ((unsigned long*) a) + pMonomSizeW -1;
    s2 = ((unsigned long*) b) + pMonomSizeW -1;
  }
  else
  {
    // first exponent word has only component as significant field --
    // Hence, do not bother
    s1 = ((unsigned long*) a) + pMonomSizeW -2;
    s2 = ((unsigned long*) b) + pMonomSizeW -2;
  }
#else // !WORDS_BIGENDIAN 
  lb = ((unsigned long*) a) + pMonomSizeW;
  if (pVariables & ((SIZEOF_LONG / SIZEOF_EXPONENT) - 1))
  {
    s1 = (unsigned long*) &(a->exp[0]);
    s2 = (unsigned long*) &(b->exp[0]);
  }
  else
  {
    s1 = (unsigned long*) &(a->exp[0]) + 1;
    s2 = (unsigned long*) &(b->exp[0]) + 1;
  }
#endif  
  for (;;)
  {
// O.K. -- and now comes a bit of magic. The following _really_
// works. Think about it! If you can prove it, please tell me, for I
// did not bother to prove it formally (Hint: We can assume that our
// exponents are always positive).
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
  DECLARE(BOOLEAN, _pDivisibleBy(poly a, poly b))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  if ((a!=NULL)&&((_pGetComp(a)==0) || (_pGetComp(a) == _pGetComp(b))))
  {
    return __pDivisibleBy(a,b);
  }
  return FALSE;
}

#if defined(PDEBUG) && PDEBUG == 1
#define _pDivisibleBy1(a,b)   pDBDivisibleBy(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy(poly p1, poly p2, char* f, int l);
inline BOOLEAN _pDivisibleBy1_orig(poly a, poly b)
#else
  DECLARE(BOOLEAN, _pDivisibleBy1(poly a, poly b))
#endif // defined(PDEBUG) && PDEBUG == 1
{
  if (_pGetComp(a) == _pGetComp(b)) return __pDivisibleBy(a,b);
  return FALSE;
}
#if defined(PDEBUG) && PDEBUG == 1
#define _pDivisibleBy2(a,b)   pDBDivisibleBy(a, b, __FILE__, __LINE__)
extern  BOOLEAN pDBDivisibleBy(poly p1, poly p2, char* f, int l);
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

inline void _pGetExpV(poly p, Exponent_t *ev)
{
  if (_pReverseExp)
  {
    for (int i = pVarLowIndex, j = pVariables; j; i++, j--)
      ev[j] = p->exp[i];
  }
  else
    memcpy(ev, &(p->exp[pVarLowIndex]), pVariables*sizeof(Exponent_t));
  ev[0] = _pGetComp(p);
}

extern pSetmProc pSetm;
inline void _pSetExpV(poly p, Exponent_t *ev)
{
  if (_pReverseExp)
  {
    for (int i = pVarLowIndex, j = pVariables; j; i++, j--)
      p->exp[i] = ev[j];
  }
  else
    memcpy(&(p->exp[pVarLowIndex]), ev, pVariables*sizeof(Exponent_t));
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

DECLARE(void, pMonAddFast(poly a, poly m))
{
  for(int ii =pVariables; ii; ii--) (a)->exp[ii] += (m)->exp[ii];\
  _pGetOrder(a) += _pGetOrder(m);
}

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
 * Routines which implement low-level manipulations/operations on exponets
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
#define _pExpQuerSum(p)     __pExpQuerSum2(p, pVarLowIndex, pVarHighIndex)
#define _pExpQuerSum1(p,to) __pExpQuerSum2(p, pVarLowIndex, pVarLowIndex+to-1) 
#define _pExpQuerSum2(p,from, to)     \
  __pQuerSum2(p, pVarLowIndex + from -1 ,pVarLowIndex + to -1)
#else
#define _pExpQuerSum(p)             __pExpQuerSum2(p, 1, pVariables)
#define _pExpQuerSum1(p, to)        __pExpQuerSum2(p, 1, to)
#define _pExpQuerSum2(p, from, to)  __pExpQuerSum2(p, from, to)
#endif

#endif // POLYS_IMPL_H

