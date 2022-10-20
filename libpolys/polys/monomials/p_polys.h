/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.h
 *  Purpose: declaration of poly stuf which are independent of
 *           currRing
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 9/00
 *******************************************************************/
/***************************************************************
 *  Purpose: implementation of poly procs which iter over ExpVector
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/
#ifndef P_POLYS_H
#define P_POLYS_H

#include "misc/mylimits.h"
#include "misc/intvec.h"
#include "coeffs/coeffs.h"

#include "polys/monomials/monomials.h"
#include "polys/monomials/ring.h"

#include "polys/templates/p_MemAdd.h"
#include "polys/templates/p_MemCmp.h"
#include "polys/templates/p_Procs.h"

#include "polys/sbuckets.h"

#ifdef HAVE_PLURAL
#include "polys/nc/nc.h"
#endif

poly p_Farey(poly p, number N, const ring r);
/*
* xx,q: arrays of length 0..rl-1
* xx[i]: SB mod q[i]
* assume: char=0
* assume: q[i]!=0
* destroys xx
*/
poly p_ChineseRemainder(poly *xx, number *x,number *q, int rl, CFArray &inv_cache, const ring R);
/***************************************************************
 *
 * Divisiblity tests, args must be != NULL, except for
 * pDivisbleBy
 *
 ***************************************************************/
unsigned long p_GetShortExpVector(const poly a, const ring r);

/// p_GetShortExpVector of p * pp
unsigned long p_GetShortExpVector(const poly p, const poly pp, const ring r);

#ifdef HAVE_RINGS
/*! divisibility check over ground ring (which may contain zero divisors);
   TRUE iff LT(f) divides LT(g), i.e., LT(f)*c*m = LT(g), for some
   coefficient c and some monomial m;
   does not take components into account
 */
BOOLEAN p_DivisibleByRingCase(poly f, poly g, const ring r);
#endif

/***************************************************************
 *
 * Misc things on polys
 *
 ***************************************************************/

poly p_One(const ring r);

int p_MinDeg(poly p,intvec *w, const ring R);

long p_DegW(poly p, const int *w, const ring R);

/// return TRUE if all monoms have the same component
BOOLEAN   p_OneComp(poly p, const ring r);

/// return i, if head depends only on var(i)
int       p_IsPurePower(const poly p, const ring r);

/// return i, if poly depends only on var(i)
int       p_IsUnivariate(poly p, const ring r);

/// set entry e[i] to 1 if var(i) occurs in p, ignore var(j) if e[j]>0
/// return #(e[i]>0)
int      p_GetVariables(poly p, int * e, const ring r);

/// returns the poly representing the integer i
poly      p_ISet(long i, const ring r);

/// returns the poly representing the number n, destroys n
poly      p_NSet(number n, const ring r);

void  p_Vec2Polys(poly v, poly**p, int *len, const ring r);
poly  p_Vec2Poly(poly v, int k, const ring r);

/// julia: vector to already allocated array (len=p_MaxComp(v,r))
void  p_Vec2Array(poly v, poly *p, int len, const ring r);

/***************************************************************
 *
 * Copying/Deletion of polys: args may be NULL
 *
 ***************************************************************/

// simply deletes monomials, does not free coeffs
void p_ShallowDelete(poly *p, const ring r);



/***************************************************************
 *
 * Copying/Deleteion of polys: args may be NULL
 *  - p/q as arg mean a poly
 *  - m a monomial
 *  - n a number
 *  - pp (resp. qq, mm, nn) means arg is constant
 *  - p (resp, q, m, n)     means arg is destroyed
 *
 ***************************************************************/

poly      p_Sub(poly a, poly b, const ring r);

poly      p_Power(poly p, int i, const ring r);


/***************************************************************
 *
 * PDEBUG stuff
 *
 ***************************************************************/
#ifdef PDEBUG
// Returns TRUE if m is monom of p, FALSE otherwise
BOOLEAN pIsMonomOf(poly p, poly m);
// Returns TRUE if p and q have common monoms
BOOLEAN pHaveCommonMonoms(poly p, poly q);

// p_Check* routines return TRUE if everything is ok,
// else, they report error message and return false

// check if Lm(p) is from ring r
BOOLEAN p_LmCheckIsFromRing(poly p, ring r);
// check if Lm(p) != NULL, r != NULL and initialized && Lm(p) is from r
BOOLEAN p_LmCheckPolyRing(poly p, ring r);
// check if all monoms of p are from ring r
BOOLEAN p_CheckIsFromRing(poly p, ring r);
// check r != NULL and initialized && all monoms of p are from r
BOOLEAN p_CheckPolyRing(poly p, ring r);
// check if r != NULL and initialized
BOOLEAN p_CheckRing(ring r);
// only do check if cond


#define pIfThen(cond, check) do {if (cond) {check;}} while (0)

BOOLEAN _p_Test(poly p, ring r, int level);
BOOLEAN _p_LmTest(poly p, ring r, int level);
BOOLEAN _pp_Test(poly p, ring lmRing, ring tailRing, int level);

#define p_Test(p,r)     _p_Test(p, r, PDEBUG)
#define p_LmTest(p,r)   _p_LmTest(p, r, PDEBUG)
#define pp_Test(p, lmRing, tailRing)    _pp_Test(p, lmRing, tailRing, PDEBUG)

#else // ! PDEBUG

#define pIsMonomOf(p, q)          (TRUE)
#define pHaveCommonMonoms(p, q)   (TRUE)
#define p_LmCheckIsFromRing(p,r)  (TRUE)
#define p_LmCheckPolyRing(p,r)    (TRUE)
#define p_CheckIsFromRing(p,r)    (TRUE)
#define p_CheckPolyRing(p,r)      (TRUE)
#define p_CheckRing(r)            (TRUE)
#define P_CheckIf(cond, check)    (TRUE)

#define p_Test(p,r)               (TRUE)
#define p_LmTest(p,r)             (TRUE)
#define pp_Test(p, lmRing, tailRing) (TRUE)

#endif

/***************************************************************
 *
 * Misc stuff
 *
 ***************************************************************/
/*2
* returns the length of a polynomial (numbers of monomials)
*/
static inline unsigned pLength(poly a)
{
  unsigned l = 0;
  while (a!=NULL)
  {
    pIter(a);
    l++;
  }
  return l;
}

// returns the length of a polynomial (numbers of monomials) and the last mon.
// respect syzComp
poly p_Last(const poly a, int &l, const ring r);

/*----------------------------------------------------*/

void      p_Norm(poly p1, const ring r);
void      p_Normalize(poly p,const ring r);
void      p_ProjectiveUnique(poly p,const ring r);

void      p_ContentForGB(poly p, const ring r);
void      p_Content(poly p, const ring r);
void      p_Content_n(poly p, number &c,const ring r);
#if 1
// currently only used by Singular/janet
void      p_SimpleContent(poly p, int s, const ring r);
number    p_InitContent(poly ph, const ring r);
#endif

poly      p_Cleardenom(poly p, const ring r);
void      p_Cleardenom_n(poly p, const ring r,number &c);
//number    p_GetAllDenom(poly ph, const ring r);// unused

int       p_Size( poly p, const ring r );

// homogenizes p by multiplying certain powers of the varnum-th variable
poly      p_Homogen (poly p, int varnum, const ring r);

BOOLEAN   p_IsHomogeneous (poly p, const ring r);
BOOLEAN   p_IsHomogeneousW (poly p, const intvec *w, const ring r);
BOOLEAN   p_IsHomogeneousW (poly p, const intvec *w, const intvec *module_w,const ring r);

// Setm
static inline void p_Setm(poly p, const ring r)
{
  p_CheckRing2(r);
  r->p_Setm(p, r);
}

p_SetmProc p_GetSetmProc(const ring r);

poly      p_Subst(poly p, int n, poly e, const ring r);

// TODO:
#define p_SetmComp  p_Setm

// component
static inline  unsigned long p_SetComp(poly p, unsigned long c, ring r)
{
  p_LmCheckPolyRing2(p, r);
  if (r->pCompIndex>=0) __p_GetComp(p,r) = c;
  return c;
}
// sets component of poly a to i
static inline   void p_SetCompP(poly p, int i, ring r)
{
  if (p != NULL)
  {
    p_Test(p, r);
    if (rOrd_SetCompRequiresSetm(r))
    {
      do
      {
        p_SetComp(p, i, r);
        p_SetmComp(p, r);
        pIter(p);
      }
      while (p != NULL);
    }
    else
    {
      do
      {
        p_SetComp(p, i, r);
        pIter(p);
      }
      while(p != NULL);
    }
  }
}

static inline   void p_SetCompP(poly p, int i, ring lmRing, ring tailRing)
{
  if (p != NULL)
  {
    p_SetComp(p, i, lmRing);
    p_SetmComp(p, lmRing);
    p_SetCompP(pNext(p), i, tailRing);
  }
}

// returns maximal column number in the modul element a (or 0)
static inline long p_MaxComp(poly p, ring lmRing, ring tailRing)
{
  long result,i;

  if(p==NULL) return 0;
  result = p_GetComp(p, lmRing);
  if (result != 0)
  {
    loop
    {
      pIter(p);
      if(p==NULL) break;
      i = p_GetComp(p, tailRing);
      if (i>result) result = i;
    }
  }
  return result;
}

static inline long p_MaxComp(poly p,ring lmRing) {return p_MaxComp(p,lmRing,lmRing);}

static inline   long p_MinComp(poly p, ring lmRing, ring tailRing)
{
  long result,i;

  if(p==NULL) return 0;
  result = p_GetComp(p,lmRing);
  if (result != 0)
  {
    loop
    {
      pIter(p);
      if(p==NULL) break;
      i = p_GetComp(p,tailRing);
      if (i<result) result = i;
    }
  }
  return result;
}

static inline long p_MinComp(poly p,ring lmRing) {return p_MinComp(p,lmRing,lmRing);}


static inline poly pReverse(poly p)
{
  if (p == NULL || pNext(p) == NULL) return p;

  poly q = pNext(p), // == pNext(p)
    qn;
  pNext(p) = NULL;
  do
  {
    qn = pNext(q);
    pNext(q) = p;
    p = q;
    q = qn;
  }
  while (qn != NULL);
  return p;
}
void      pEnlargeSet(poly**p, int length, int increment);


/***************************************************************
 *
 * I/O
 *
 ***************************************************************/
/// print p according to ShortOut in lmRing & tailRing
void      p_String0(poly p, ring lmRing, ring tailRing);
char*     p_String(poly p, ring lmRing, ring tailRing);
void      p_Write(poly p, ring lmRing, ring tailRing);
void      p_Write0(poly p, ring lmRing, ring tailRing);
void      p_wrp(poly p, ring lmRing, ring tailRing);

/// print p in a short way, if possible
void  p_String0Short(const poly p, ring lmRing, ring tailRing);

/// print p in a long way
void   p_String0Long(const poly p, ring lmRing, ring tailRing);


/***************************************************************
 *
 * Degree stuff -- see p_polys.cc for explainations
 *
 ***************************************************************/

static inline long  p_FDeg(const poly p, const ring r)  { return r->pFDeg(p,r); }
static inline long  p_LDeg(const poly p, int *l, const ring r)  { return r->pLDeg(p,l,r); }

long p_WFirstTotalDegree(poly p, ring r);
long p_WTotaldegree(poly p, const ring r);
long p_WDegree(poly p,const ring r);
long pLDeg0(poly p,int *l, ring r);
long pLDeg0c(poly p,int *l, ring r);
long pLDegb(poly p,int *l, ring r);
long pLDeg1(poly p,int *l, ring r);
long pLDeg1c(poly p,int *l, ring r);
long pLDeg1_Deg(poly p,int *l, ring r);
long pLDeg1c_Deg(poly p,int *l, ring r);
long pLDeg1_Totaldegree(poly p,int *l, ring r);
long pLDeg1c_Totaldegree(poly p,int *l, ring r);
long pLDeg1_WFirstTotalDegree(poly p,int *l, ring r);
long pLDeg1c_WFirstTotalDegree(poly p,int *l, ring r);

BOOLEAN p_EqualPolys(poly p1, poly p2, const ring r);

/// same as the usual p_EqualPolys for polys belonging to *equal* rings
BOOLEAN p_EqualPolys(poly p1, poly p2, const ring r1, const ring r2);

long p_Deg(poly a, const ring r);


/***************************************************************
 *
 * Primitives for accessing and setting fields of a poly
 *
 ***************************************************************/

static inline number p_SetCoeff(poly p, number n, ring r)
{
  p_LmCheckPolyRing2(p, r);
  n_Delete(&(p->coef), r->cf);
  (p)->coef=n;
  return n;
}

// order
static inline long p_GetOrder(poly p, ring r)
{
  p_LmCheckPolyRing2(p, r);
  if (r->typ==NULL) return ((p)->exp[r->pOrdIndex]);
  int i=0;
  loop
  {
    switch(r->typ[i].ord_typ)
    {
      case ro_am:
      case ro_wp_neg:
        return ((p->exp[r->pOrdIndex])-POLY_NEGWEIGHT_OFFSET);
      case ro_syzcomp:
      case ro_syz:
      case ro_cp:
        i++;
        break;
      //case ro_dp:
      //case ro_wp:
      default:
        return ((p)->exp[r->pOrdIndex]);
    }
  }
}


static inline unsigned long p_AddComp(poly p, unsigned long v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(rRing_has_Comp(r));
  return __p_GetComp(p,r) += v;
}
static inline unsigned long p_SubComp(poly p, unsigned long v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(rRing_has_Comp(r));
  _pPolyAssume2(__p_GetComp(p,r) >= v,p,r);
  return __p_GetComp(p,r) -= v;
}

#ifndef HAVE_EXPSIZES

/// get a single variable exponent
/// @Note:
/// the integer VarOffset encodes:
/// 1. the position of a variable in the exponent vector p->exp (lower 24 bits)
/// 2. number of bits to shift to the right in the upper 8 bits (which takes at most 6 bits for 64 bit)
/// Thus VarOffset always has 2 zero higher bits!
static inline long p_GetExp(const poly p, const unsigned long iBitmask, const int VarOffset)
{
  pAssume2((VarOffset >> (24 + 6)) == 0);
#if 0
  int pos=(VarOffset & 0xffffff);
  int bitpos=(VarOffset >> 24);
  unsigned long exp=(p->exp[pos] >> bitmask) & iBitmask;
  return exp;
#else
  return (long)
         ((p->exp[(VarOffset & 0xffffff)] >> (VarOffset >> 24))
          & iBitmask);
#endif
}


/// set a single variable exponent
/// @Note:
/// VarOffset encodes the position in p->exp @see p_GetExp
static inline unsigned long p_SetExp(poly p, const unsigned long e, const unsigned long iBitmask, const int VarOffset)
{
  pAssume2(e>=0);
  pAssume2(e<=iBitmask);
  pAssume2((VarOffset >> (24 + 6)) == 0);

  // shift e to the left:
  REGISTER int shift = VarOffset >> 24;
  unsigned long ee = e << shift /*(VarOffset >> 24)*/;
  // find the bits in the exponent vector
  REGISTER int offset = (VarOffset & 0xffffff);
  // clear the bits in the exponent vector:
  p->exp[offset]  &= ~( iBitmask << shift );
  // insert e with |
  p->exp[ offset ] |= ee;
  return e;
}


#else // #ifdef HAVE_EXPSIZES // EXPERIMENTAL!!!

static inline unsigned long BitMask(unsigned long bitmask, int twobits)
{
  // bitmask = 00000111111111111
  // 0 must give bitmask!
  // 1, 2, 3 - anything like 00011..11
  pAssume2((twobits >> 2) == 0);
  static const unsigned long _bitmasks[4] = {-1, 0x7fff, 0x7f, 0x3};
  return bitmask & _bitmasks[twobits];
}


/// @Note: we may add some more info (6 ) into VarOffset and thus encode
static inline long p_GetExp(const poly p, const unsigned long iBitmask, const int VarOffset)
{
  int pos  =(VarOffset & 0xffffff);
  int hbyte= (VarOffset >> 24); // the highest byte
  int bitpos = hbyte & 0x3f; // last 6 bits
  long bitmask = BitMask(iBitmask, hbyte >> 6);

  long exp=(p->exp[pos] >> bitpos) & bitmask;
  return exp;

}

static inline long p_SetExp(poly p, const long e, const unsigned long iBitmask, const int VarOffset)
{
  pAssume2(e>=0);
  pAssume2(e <= BitMask(iBitmask, VarOffset >> 30));

  // shift e to the left:
  REGISTER int hbyte = VarOffset >> 24;
  int bitmask = BitMask(iBitmask, hbyte >> 6);
  REGISTER int shift = hbyte & 0x3f;
  long ee = e << shift;
  // find the bits in the exponent vector
  REGISTER int offset = (VarOffset & 0xffffff);
  // clear the bits in the exponent vector:
  p->exp[offset]  &= ~( bitmask << shift );
  // insert e with |
  p->exp[ offset ] |= ee;
  return e;
}

#endif // #ifndef HAVE_EXPSIZES


static inline long p_GetExp(const poly p, const ring r, const int VarOffset)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(VarOffset != -1);
  return p_GetExp(p, r->bitmask, VarOffset);
}

static inline long p_SetExp(poly p, const long e, const ring r, const int VarOffset)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(VarOffset != -1);
  return p_SetExp(p, e, r->bitmask, VarOffset);
}



/// get v^th exponent for a monomial
static inline long p_GetExp(const poly p, const int v, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(v>0 && v <= r->N);
  pAssume2(r->VarOffset[v] != -1);
  return p_GetExp(p, r->bitmask, r->VarOffset[v]);
}


/// set v^th exponent for a monomial
static inline long p_SetExp(poly p, const int v, const long e, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(v>0 && v <= r->N);
  pAssume2(r->VarOffset[v] != -1);
  return p_SetExp(p, e, r->bitmask, r->VarOffset[v]);
}

// the following should be implemented more efficiently
static inline  long p_IncrExp(poly p, int v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  int e = p_GetExp(p,v,r);
  e++;
  return p_SetExp(p,v,e,r);
}
static inline  long p_DecrExp(poly p, int v, ring r)
{
  p_LmCheckPolyRing2(p, r);
  int e = p_GetExp(p,v,r);
  pAssume2(e > 0);
  e--;
  return p_SetExp(p,v,e,r);
}
static inline  long p_AddExp(poly p, int v, long ee, ring r)
{
  p_LmCheckPolyRing2(p, r);
  int e = p_GetExp(p,v,r);
  e += ee;
  return p_SetExp(p,v,e,r);
}
static inline  long p_SubExp(poly p, int v, long ee, ring r)
{
  p_LmCheckPolyRing2(p, r);
  long e = p_GetExp(p,v,r);
  pAssume2(e >= ee);
  e -= ee;
  return p_SetExp(p,v,e,r);
}
static inline  long p_MultExp(poly p, int v, long ee, ring r)
{
  p_LmCheckPolyRing2(p, r);
  long e = p_GetExp(p,v,r);
  e *= ee;
  return p_SetExp(p,v,e,r);
}

static inline long p_GetExpSum(poly p1, poly p2, int i, ring r)
{
  p_LmCheckPolyRing2(p1, r);
  p_LmCheckPolyRing2(p2, r);
  return p_GetExp(p1,i,r) + p_GetExp(p2,i,r);
}
static inline long p_GetExpDiff(poly p1, poly p2, int i, ring r)
{
  return p_GetExp(p1,i,r) - p_GetExp(p2,i,r);
}

static inline int p_Comp_k_n(poly a, poly b, int k, ring r)
{
  if ((a==NULL) || (b==NULL) ) return FALSE;
  p_LmCheckPolyRing2(a, r);
  p_LmCheckPolyRing2(b, r);
  pAssume2(k > 0 && k <= r->N);
  int i=k;
  for(;i<=r->N;i++)
  {
    if (p_GetExp(a,i,r) != p_GetExp(b,i,r)) return FALSE;
    //    if (a->exp[(r->VarOffset[i] & 0xffffff)] != b->exp[(r->VarOffset[i] & 0xffffff)]) return FALSE;
  }
  return TRUE;
}


/***************************************************************
 *
 * Allocation/Initalization/Deletion
 *
 ***************************************************************/
#if (OM_TRACK > 2) && defined(OM_TRACK_CUSTOM)
static inline poly p_New(const ring r, omBin bin)
#else
static inline poly p_New(const ring /*r*/, omBin bin)
#endif
{
  p_CheckRing2(r);
  pAssume2(bin != NULL && omSizeWOfBin(r->PolyBin) == omSizeWOfBin(bin));
  poly p;
  omTypeAllocBin(poly, p, bin);
  p_SetRingOfLm(p, r);
  return p;
}

static inline poly p_New(ring r)
{
  return p_New(r, r->PolyBin);
}

#if (PDEBUG > 2) || defined(XALLOC_BIN)
static inline void p_LmFree(poly p, ring r)
#else
static inline void p_LmFree(poly p, ring)
#endif
{
  p_LmCheckPolyRing2(p, r);
  #ifdef XALLOC_BIN
  omFreeBin(p,r->PolyBin);
  #else
  omFreeBinAddr(p);
  #endif
}
#if (PDEBUG > 2) || defined(XALLOC_BIN)
static inline void p_LmFree(poly *p, ring r)
#else
static inline void p_LmFree(poly *p, ring)
#endif
{
  p_LmCheckPolyRing2(*p, r);
  poly h = *p;
  *p = pNext(h);
  #ifdef XALLOC_BIN
  omFreeBin(h,r->PolyBin);
  #else
  omFreeBinAddr(h);
  #endif
}
#if (PDEBUG > 2) || defined(XALLOC_BIN)
static inline poly p_LmFreeAndNext(poly p, ring r)
#else
static inline poly p_LmFreeAndNext(poly p, ring)
#endif
{
  p_LmCheckPolyRing2(p, r);
  poly pnext = pNext(p);
  #ifdef XALLOC_BIN
  omFreeBin(p,r->PolyBin);
  #else
  omFreeBinAddr(p);
  #endif
  return pnext;
}
static inline void p_LmDelete(poly p, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  n_Delete(&pGetCoeff(p), r->cf);
  #ifdef XALLOC_BIN
  omFreeBin(p,r->PolyBin);
  #else
  omFreeBinAddr(p);
  #endif
}
static inline void p_LmDelete0(poly p, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  if (pGetCoeff(p)!=NULL) n_Delete(&pGetCoeff(p), r->cf);
  #ifdef XALLOC_BIN
  omFreeBin(p,r->PolyBin);
  #else
  omFreeBinAddr(p);
  #endif
}
static inline void p_LmDelete(poly *p, const ring r)
{
  p_LmCheckPolyRing2(*p, r);
  poly h = *p;
  *p = pNext(h);
  n_Delete(&pGetCoeff(h), r->cf);
  #ifdef XALLOC_BIN
  omFreeBin(h,r->PolyBin);
  #else
  omFreeBinAddr(h);
  #endif
}
static inline poly p_LmDeleteAndNext(poly p, const ring r)
{
  p_LmCheckPolyRing2(p, r);
  poly pnext = pNext(p);
  n_Delete(&pGetCoeff(p), r->cf);
  #ifdef XALLOC_BIN
  omFreeBin(p,r->PolyBin);
  #else
  omFreeBinAddr(p);
  #endif
  return pnext;
}

/***************************************************************
 *
 * Misc routines
 *
 ***************************************************************/

/// return the maximal exponent of p in form of the maximal long var
unsigned long p_GetMaxExpL(poly p, const ring r, unsigned long l_max = 0);

/// return monomial r such that GetExp(r,i) is maximum of all
/// monomials in p; coeff == 0, next == NULL, ord is not set
poly p_GetMaxExpP(poly p, ring r);

static inline unsigned long p_GetMaxExp(const unsigned long l, const ring r)
{
  unsigned long bitmask = r->bitmask;
  unsigned long max = (l & bitmask);
  unsigned long j = r->ExpPerLong - 1;

  if (j > 0)
  {
    unsigned long i = r->BitsPerExp;
    long e;
    loop
    {
      e = ((l >> i) & bitmask);
      if ((unsigned long) e > max)
        max = e;
      j--;
      if (j==0) break;
      i += r->BitsPerExp;
    }
  }
  return max;
}

static inline unsigned long p_GetMaxExp(const poly p, const ring r)
{
  return p_GetMaxExp(p_GetMaxExpL(p, r), r);
}

static inline unsigned long
p_GetTotalDegree(const unsigned long l, const ring r, const int number_of_exps)
{
  const unsigned long bitmask = r->bitmask;
  unsigned long sum = (l & bitmask);
  unsigned long j = number_of_exps - 1;

  if (j > 0)
  {
    unsigned long i = r->BitsPerExp;
    loop
    {
      sum += ((l >> i) & bitmask);
      j--;
      if (j==0) break;
      i += r->BitsPerExp;
    }
  }
  return sum;
}

/***************************************************************
 *
 * Dispatcher to r->p_Procs, they do the tests/checks
 *
 ***************************************************************/
/// returns a copy of p (without any additional testing)
static inline poly p_Copy_noCheck(poly p, const ring r)
{
  /*assume(p!=NULL);*/
  assume(r != NULL);
  assume(r->p_Procs != NULL);
  assume(r->p_Procs->p_Copy != NULL);
  return r->p_Procs->p_Copy(p, r);
}

/// returns a copy of p
static inline poly p_Copy(poly p, const ring r)
{
  if (p!=NULL)
  {
    p_Test(p,r);
    const poly pp = p_Copy_noCheck(p, r);
    p_Test(pp,r);
    return pp;
  }
  else
    return NULL;
}

/// copy the (leading) term of p
static inline poly p_Head(const poly p, const ring r)
{
  if (p == NULL) return NULL;
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  memcpy(np->exp, p->exp, r->ExpL_Size*sizeof(long));
  pNext(np) = NULL;
  pSetCoeff0(np, n_Copy(pGetCoeff(p), r->cf));
  return np;
}

/// like p_Head, but allow NULL coeff
poly p_Head0(const poly p, const ring r);

/// like p_Head, but with coefficient 1
poly p_CopyPowerProduct(const poly p, const ring r);

/// like p_Head, but with coefficient n
poly p_CopyPowerProduct0(const poly p, const number n, const ring r);

/// returns a copy of p with Lm(p) from lmRing and Tail(p) from tailRing
static inline poly p_Copy(poly p, const ring lmRing, const ring tailRing)
{
  if (p != NULL)
  {
#ifndef PDEBUG
    if (tailRing == lmRing)
      return p_Copy_noCheck(p, tailRing);
#endif
    poly pres = p_Head(p, lmRing);
    if (pNext(p)!=NULL)
      pNext(pres) = p_Copy_noCheck(pNext(p), tailRing);
    return pres;
  }
  else
    return NULL;
}

// deletes *p, and sets *p to NULL
static inline void p_Delete(poly *p, const ring r)
{
  assume( p!= NULL );
  assume( r!= NULL );
  if ((*p)!=NULL) r->p_Procs->p_Delete(p, r);
}

static inline void p_Delete(poly *p,  const ring lmRing, const ring tailRing)
{
  assume( p!= NULL );
  if (*p != NULL)
  {
#ifndef PDEBUG
    if (tailRing == lmRing)
    {
      p_Delete(p, tailRing);
      return;
    }
#endif
    if (pNext(*p) != NULL)
      p_Delete(&pNext(*p), tailRing);
    p_LmDelete(p, lmRing);
  }
}

// copys monomials of p, allocates new monomials from bin,
// deletes monomials of p
static inline poly p_ShallowCopyDelete(poly p, const ring r, omBin bin)
{
  p_LmCheckPolyRing2(p, r);
  pAssume2(omSizeWOfBin(r->PolyBin) == omSizeWOfBin(bin));
  return r->p_Procs->p_ShallowCopyDelete(p, r, bin);
}

// returns p+q, destroys p and q
static inline poly p_Add_q(poly p, poly q, const ring r)
{
  assume( (p != q) || (p == NULL && q == NULL) );
  if (q==NULL) return p;
  if (p==NULL) return q;
  int shorter;
  return r->p_Procs->p_Add_q(p, q, shorter, r);
}

/// like p_Add_q, except that if lp == pLength(lp) lq == pLength(lq) then lp == pLength(p+q)
static inline poly p_Add_q(poly p, poly q, int &lp, int lq, const ring r)
{
  assume( (p != q) || (p == NULL && q == NULL) );
  if (q==NULL) return p;
  if (p==NULL) { lp=lq; return q; }
  int shorter;
  poly res = r->p_Procs->p_Add_q(p, q, shorter, r);
  lp += lq - shorter;
  return res;
}

// returns p*n, destroys p
static inline poly p_Mult_nn(poly p, number n, const ring r)
{
  if (p==NULL) return NULL;
  if (n_IsOne(n, r->cf))
    return p;
  else if (n_IsZero(n, r->cf))
  {
    p_Delete(&p, r); // NOTE: without p_Delete - memory leak!
    return NULL;
  }
  else
    return r->p_Procs->p_Mult_nn(p, n, r);
}
#define __p_Mult_nn(p,n,r) r->p_Procs->p_Mult_nn(p, n, r)

static inline poly p_Mult_nn(poly p, number n, const ring lmRing,
                        const ring tailRing)
{
  assume(p!=NULL);
#ifndef PDEBUG
  if (lmRing == tailRing)
    return p_Mult_nn(p, n, tailRing);
#endif
  poly pnext = pNext(p);
  pNext(p) = NULL;
  p = lmRing->p_Procs->p_Mult_nn(p, n, lmRing);
  if (pnext!=NULL)
  {
    pNext(p) = tailRing->p_Procs->p_Mult_nn(pnext, n, tailRing);
  }
  return p;
}

// returns p*n, does not destroy p
static inline poly pp_Mult_nn(poly p, number n, const ring r)
{
  if (p==NULL) return NULL;
  if (n_IsOne(n, r->cf))
    return p_Copy(p, r);
  else if (n_IsZero(n, r->cf))
    return NULL;
  else
    return r->p_Procs->pp_Mult_nn(p, n, r);
}
#define __pp_Mult_nn(p,n,r) r->p_Procs->pp_Mult_nn(p, n, r)

// test if the monomial is a constant as a vector component
// i.e., test if all exponents are zero
static inline BOOLEAN p_LmIsConstantComp(const poly p, const ring r)
{
  //p_LmCheckPolyRing(p, r);
  int i = r->VarL_Size - 1;

  do
  {
    if (p->exp[r->VarL_Offset[i]] != 0)
      return FALSE;
    i--;
  }
  while (i >= 0);
  return TRUE;
}

// test if monomial is a constant, i.e. if all exponents and the component
// is zero
static inline BOOLEAN p_LmIsConstant(const poly p, const ring r)
{
  if (p_LmIsConstantComp(p, r))
    return (p_GetComp(p, r) == 0);
  return FALSE;
}

// returns Copy(p)*m, does neither destroy p nor m
static inline poly pp_Mult_mm(poly p, poly m, const ring r)
{
  if (p==NULL) return NULL;
  if (p_LmIsConstant(m, r))
    return __pp_Mult_nn(p, pGetCoeff(m), r);
  else
    return r->p_Procs->pp_Mult_mm(p, m, r);
}

// returns m*Copy(p), does neither destroy p nor m
static inline poly pp_mm_Mult(poly p, poly m, const ring r)
{
  if (p==NULL) return NULL;
  if (p_LmIsConstant(m, r))
    return __pp_Mult_nn(p, pGetCoeff(m), r);
  else
    return r->p_Procs->pp_mm_Mult(p, m, r);
}

// returns p*m, destroys p, const: m
static inline poly p_Mult_mm(poly p, poly m, const ring r)
{
  if (p==NULL) return NULL;
  if (p_LmIsConstant(m, r))
    return __p_Mult_nn(p, pGetCoeff(m), r);
  else
    return r->p_Procs->p_Mult_mm(p, m, r);
}

// returns m*p, destroys p, const: m
static inline poly p_mm_Mult(poly p, poly m, const ring r)
{
  if (p==NULL) return NULL;
  if (p_LmIsConstant(m, r))
    return __p_Mult_nn(p, pGetCoeff(m), r);
  else
    return r->p_Procs->p_mm_Mult(p, m, r);
}

static inline poly p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp, int lq,
                                      const poly spNoether, const ring r)
{
  int shorter;
  const poly res = r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, spNoether, r);
  lp += lq - shorter;
//  assume( lp == pLength(res) );
  return res;
}

// return p - m*Copy(q), destroys p; const: p,m
static inline poly p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, const ring r)
{
  int shorter;

  return r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r);
}


// returns p*Coeff(m) for such monomials pm of p, for which m is divisble by pm
static inline poly pp_Mult_Coeff_mm_DivSelect(poly p, const poly m, const ring r)
{
  int shorter;
  return r->p_Procs->pp_Mult_Coeff_mm_DivSelect(p, m, shorter, r);
}

// returns p*Coeff(m) for such monomials pm of p, for which m is divisble by pm
// if lp is length of p on input then lp is length of returned poly on output
static inline poly pp_Mult_Coeff_mm_DivSelect(poly p, int &lp, const poly m, const ring r)
{
  int shorter;
  poly pp = r->p_Procs->pp_Mult_Coeff_mm_DivSelect(p, m, shorter, r);
  lp -= shorter;
  return pp;
}

// returns -p, destroys p
static inline poly p_Neg(poly p, const ring r)
{
  return r->p_Procs->p_Neg(p, r);
}

extern poly  _p_Mult_q(poly p, poly q, const int copy, const ring r);
// returns p*q, destroys p and q
static inline poly p_Mult_q(poly p, poly q, const ring r)
{
  assume( (p != q) || (p == NULL && q == NULL) );

  if (p == NULL)
  {
    p_Delete(&q, r);
    return NULL;
  }
  if (q == NULL)
  {
    p_Delete(&p, r);
    return NULL;
  }

  if (pNext(p) == NULL)
  {
    q = r->p_Procs->p_mm_Mult(q, p, r);
    p_LmDelete(&p, r);
    return q;
  }

  if (pNext(q) == NULL)
  {
    p = r->p_Procs->p_Mult_mm(p, q, r);
    p_LmDelete(&q, r);
    return p;
  }
#if defined(HAVE_PLURAL) || defined(HAVE_SHIFTBBA)
  if (rIsNCRing(r))
    return _nc_p_Mult_q(p, q, r);
  else
#endif
  return _p_Mult_q(p, q, 0, r);
}

// returns p*q, does neither destroy p nor q
static inline poly pp_Mult_qq(poly p, poly q, const ring r)
{
  if (p == NULL || q == NULL) return NULL;

  if (pNext(p) == NULL)
  {
    return r->p_Procs->pp_mm_Mult(q, p, r);
  }

  if (pNext(q) == NULL)
  {
    return r->p_Procs->pp_Mult_mm(p, q, r);
  }

  poly qq = q;
  if (p == q)
    qq = p_Copy(q, r);

  poly res;
#if defined(HAVE_PLURAL) || defined(HAVE_SHIFTBBA)
  if (rIsNCRing(r))
    res = _nc_pp_Mult_qq(p, qq, r);
  else
#endif
    res = _p_Mult_q(p, qq, 1, r);

  if (qq != q)
    p_Delete(&qq, r);
  return res;
}

// returns p + m*q destroys p, const: q, m
static inline poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, int &lp, int lq,
                                const ring r)
{
#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
    return nc_p_Plus_mm_Mult_qq(p, m, q, lp, lq, r);
#endif

// this should be implemented more efficiently
  poly res;
  int shorter;
  number n_old = pGetCoeff(m);
  number n_neg = n_Copy(n_old, r->cf);
  n_neg = n_InpNeg(n_neg, r->cf);
  pSetCoeff0(m, n_neg);
  res = r->p_Procs->p_Minus_mm_Mult_qq(p, m, q, shorter, NULL, r);
  lp = (lp + lq) - shorter;
  pSetCoeff0(m, n_old);
  n_Delete(&n_neg, r->cf);
  return res;
}

static inline poly p_Plus_mm_Mult_qq(poly p, poly m, poly q, const ring r)
{
  int lp = 0, lq = 0;
  return p_Plus_mm_Mult_qq(p, m, q, lp, lq, r);
}

// returns merged p and q, assumes p and q have no monomials which are equal
static inline poly p_Merge_q(poly p, poly q, const ring r)
{
  assume( (p != q) || (p == NULL && q == NULL) );
  return r->p_Procs->p_Merge_q(p, q, r);
}

// like p_SortMerge, except that p may have equal monimals
static inline poly p_SortAdd(poly p, const ring r, BOOLEAN revert= FALSE)
{
  if (revert) p = pReverse(p);
  return sBucketSortAdd(p, r);
}

// sorts p using bucket sort: returns sorted poly
// assumes that monomials of p are all different
// reverses it first, if revert == TRUE, use this if input p is "almost" sorted
// correctly
static inline poly p_SortMerge(poly p, const ring r, BOOLEAN revert= FALSE)
{
  if (revert) p = pReverse(p);
  return sBucketSortMerge(p, r);
}

/***************************************************************
 *
 * I/O
 *
 ***************************************************************/
static inline char*     p_String(poly p, ring p_ring)
{
  return p_String(p, p_ring, p_ring);
}
static inline void     p_String0(poly p, ring p_ring)
{
  p_String0(p, p_ring, p_ring);
}
static inline void      p_Write(poly p, ring p_ring)
{
  p_Write(p, p_ring, p_ring);
}
static inline void      p_Write0(poly p, ring p_ring)
{
  p_Write0(p, p_ring, p_ring);
}
static inline void      p_wrp(poly p, ring p_ring)
{
  p_wrp(p, p_ring, p_ring);
}


#if PDEBUG > 0

#define _p_LmCmpAction(p, q, r, actionE, actionG, actionS)  \
do                                                          \
{                                                           \
  int _cmp = p_LmCmp(p,q,r);                                \
  if (_cmp == 0) actionE;                                   \
  if (_cmp == 1) actionG;                                   \
  actionS;                                                  \
}                                                           \
while(0)

#else

#define _p_LmCmpAction(p, q, r, actionE, actionG, actionS)                      \
 p_MemCmp_LengthGeneral_OrdGeneral(p->exp, q->exp, r->CmpL_Size, r->ordsgn,    \
                                   actionE, actionG, actionS)

#endif

#define pDivAssume(x)   do {} while (0)



/***************************************************************
 *
 * Allocation/Initalization/Deletion
 *
 ***************************************************************/
// adjustments for negative weights
static inline void p_MemAdd_NegWeightAdjust(poly p, const ring r)
{
  if (r->NegWeightL_Offset != NULL)
  {
    for (int i=r->NegWeightL_Size-1; i>=0; i--)
    {
      p->exp[r->NegWeightL_Offset[i]] -= POLY_NEGWEIGHT_OFFSET;
    }
  }
}
static inline void p_MemSub_NegWeightAdjust(poly p, const ring r)
{
  if (r->NegWeightL_Offset != NULL)
  {
    for (int i=r->NegWeightL_Size-1; i>=0; i--)
    {
      p->exp[r->NegWeightL_Offset[i]] += POLY_NEGWEIGHT_OFFSET;
    }
  }
}
// ExpVextor(d_p) = ExpVector(s_p)
static inline void p_ExpVectorCopy(poly d_p, poly s_p, const ring r)
{
  p_LmCheckPolyRing1(d_p, r);
  p_LmCheckPolyRing1(s_p, r);
  memcpy(d_p->exp, s_p->exp, r->ExpL_Size*sizeof(long));
}

static inline poly p_Init(const ring r, omBin bin)
{
  p_CheckRing1(r);
  pAssume1(bin != NULL && omSizeWOfBin(r->PolyBin) == omSizeWOfBin(bin));
  poly p;
  omTypeAlloc0Bin(poly, p, bin);
  p_MemAdd_NegWeightAdjust(p, r);
  p_SetRingOfLm(p, r);
  return p;
}
static inline poly p_Init(const ring r)
{
  return p_Init(r, r->PolyBin);
}

static inline poly p_LmInit(poly p, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  memcpy(np->exp, p->exp, r->ExpL_Size*sizeof(long));
  pNext(np) = NULL;
  pSetCoeff0(np, NULL);
  return np;
}
static inline poly p_LmInit(poly s_p, const ring s_r, const ring d_r, omBin d_bin)
{
  p_LmCheckPolyRing1(s_p, s_r);
  p_CheckRing(d_r);
  pAssume1(d_r->N <= s_r->N);
  poly d_p = p_Init(d_r, d_bin);
  for (unsigned i=d_r->N; i!=0; i--)
  {
    p_SetExp(d_p, i, p_GetExp(s_p, i,s_r), d_r);
  }
  if (rRing_has_Comp(d_r))
  {
    p_SetComp(d_p, p_GetComp(s_p,s_r), d_r);
  }
  p_Setm(d_p, d_r);
  return d_p;
}
static inline poly p_LmInit(poly s_p, const ring s_r, const ring d_r)
{
  pAssume1(d_r != NULL);
  return p_LmInit(s_p, s_r, d_r, d_r->PolyBin);
}

// set all exponents l..k to 0, assume exp. k+1..n and 1..l-1 are in
// different blocks
// set coeff to 1
static inline poly p_GetExp_k_n(poly p, int l, int k, const ring r)
{
  if (p == NULL) return NULL;
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  memcpy(np->exp, p->exp, r->ExpL_Size*sizeof(long));
  pNext(np) = NULL;
  pSetCoeff0(np, n_Init(1, r->cf));
  int i;
  for(i=l;i<=k;i++)
  {
    //np->exp[(r->VarOffset[i] & 0xffffff)] =0;
    p_SetExp(np,i,0,r);
  }
  p_Setm(np,r);
  return np;
}

// simialar to p_ShallowCopyDelete but does it only for leading monomial
static inline poly p_LmShallowCopyDelete(poly p, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  pAssume1(omSizeWOfBin(bin) == omSizeWOfBin(r->PolyBin));
  poly new_p = p_New(r);
  memcpy(new_p->exp, p->exp, r->ExpL_Size*sizeof(long));
  pSetCoeff0(new_p, pGetCoeff(p));
  pNext(new_p) = pNext(p);
  omFreeBinAddr(p);
  return new_p;
}

/***************************************************************
 *
 * Operation on ExpVectors
 *
 ***************************************************************/
// ExpVector(p1) += ExpVector(p2)
static inline void p_ExpVectorAdd(poly p1, poly p2, const ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif

  p_MemAdd_LengthGeneral(p1->exp, p2->exp, r->ExpL_Size);
  p_MemAdd_NegWeightAdjust(p1, r);
}
// ExpVector(pr) = ExpVector(p1) + ExpVector(p2)
static inline void p_ExpVectorSum(poly pr, poly p1, poly p2, const ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(pr, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1((unsigned long) (p_GetExp(p1, i, r) + p_GetExp(p2, i, r)) <= r->bitmask);
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0);
#endif

  p_MemSum_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpL_Size);
  p_MemAdd_NegWeightAdjust(pr, r);
}
// ExpVector(p1) -= ExpVector(p2)
static inline void p_ExpVectorSub(poly p1, poly p2, const ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  pAssume1(p_GetComp(p1, r) == 0 || p_GetComp(p2, r) == 0 ||
          p_GetComp(p1, r) == p_GetComp(p2, r));
#endif

  p_MemSub_LengthGeneral(p1->exp, p2->exp, r->ExpL_Size);
  p_MemSub_NegWeightAdjust(p1, r);
}

// ExpVector(p1) += ExpVector(p2) - ExpVector(p3)
static inline void p_ExpVectorAddSub(poly p1, poly p2, poly p3, const ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(p3, r);
#if PDEBUG >= 1
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) + p_GetExp(p2, i, r) >= p_GetExp(p3, i, r));
  pAssume1(p_GetComp(p1, r) == 0 ||
           (p_GetComp(p2, r) - p_GetComp(p3, r) == 0) ||
           (p_GetComp(p1, r) == p_GetComp(p2, r) - p_GetComp(p3, r)));
#endif

  p_MemAddSub_LengthGeneral(p1->exp, p2->exp, p3->exp, r->ExpL_Size);
  // no need to adjust in case of NegWeights
}

// ExpVector(pr) = ExpVector(p1) - ExpVector(p2)
static inline void p_ExpVectorDiff(poly pr, poly p1, poly p2, const ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(pr, r);
#if PDEBUG >= 2
  for (int i=1; i<=r->N; i++)
    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
  pAssume1(!rRing_has_Comp(r) || p_GetComp(p1, r) == p_GetComp(p2, r));
#endif

  p_MemDiff_LengthGeneral(pr->exp, p1->exp, p2->exp, r->ExpL_Size);
  p_MemSub_NegWeightAdjust(pr, r);
}

static inline BOOLEAN p_ExpVectorEqual(poly p1, poly p2, const ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);

  unsigned i = r->ExpL_Size;
  unsigned long *ep = p1->exp;
  unsigned long *eq = p2->exp;

  do
  {
    i--;
    if (ep[i] != eq[i]) return FALSE;
  }
  while (i!=0);
  return TRUE;
}

static inline long p_Totaldegree(poly p, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  unsigned long s = p_GetTotalDegree(p->exp[r->VarL_Offset[0]],
                                     r,
                                     r->ExpPerLong);
  for (unsigned i=r->VarL_Size-1; i!=0; i--)
  {
    s += p_GetTotalDegree(p->exp[r->VarL_Offset[i]], r,r->ExpPerLong);
  }
  return (long)s;
}

static inline void p_GetExpV(poly p, int *ev, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (unsigned j = r->N; j!=0; j--)
      ev[j] = p_GetExp(p, j, r);

  ev[0] = p_GetComp(p, r);
}
// p_GetExpVL is used in Singular,jl
static inline void p_GetExpVL(poly p, int64 *ev, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (unsigned j = r->N; j!=0; j--)
      ev[j-1] = p_GetExp(p, j, r);
}
// p_GetExpVLV is used in Singular,jl
static inline int64 p_GetExpVLV(poly p, int64 *ev, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (unsigned j = r->N; j!=0; j--)
      ev[j-1] = p_GetExp(p, j, r);
  return (int64)p_GetComp(p,r);
}
// p_GetExpVL is used in Singular,jl
static inline void p_SetExpV(poly p, int *ev, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (unsigned j = r->N; j!=0; j--)
      p_SetExp(p, j, ev[j], r);

  if(ev[0]!=0) p_SetComp(p, ev[0],r);
  p_Setm(p, r);
}
static inline void p_SetExpVL(poly p, int64 *ev, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (unsigned j = r->N; j!=0; j--)
      p_SetExp(p, j, ev[j-1], r);
  p_SetComp(p, 0,r);

  p_Setm(p, r);
}

// p_SetExpVLV is used in Singular,jl
static inline void p_SetExpVLV(poly p, int64 *ev, int64 comp, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  for (unsigned j = r->N; j!=0; j--)
      p_SetExp(p, j, ev[j-1], r);
  p_SetComp(p, comp,r);

  p_Setm(p, r);
}

/***************************************************************
 *
 * Comparison w.r.t. monomial ordering
 *
 ***************************************************************/

static inline int p_LmCmp(poly p, poly q, const ring r)
{
  p_LmCheckPolyRing1(p, r);
  p_LmCheckPolyRing1(q, r);

  const unsigned long* _s1 = ((unsigned long*) p->exp);
  const unsigned long* _s2 = ((unsigned long*) q->exp);
  REGISTER unsigned long _v1;
  REGISTER unsigned long _v2;
  const unsigned long _l = r->CmpL_Size;

  REGISTER unsigned long _i=0;

  LengthGeneral_OrdGeneral_LoopTop:
  _v1 = _s1[_i];
  _v2 = _s2[_i];
  if (_v1 == _v2)
  {
    _i++;
    if (_i == _l) return 0;
    goto LengthGeneral_OrdGeneral_LoopTop;
  }
  const long* _ordsgn = (long*) r->ordsgn;
#if 1 /* two variants*/
  if (_v1 > _v2)
  {
    return _ordsgn[_i];
  }
  return -(_ordsgn[_i]);
#else
   if (_v1 > _v2)
   {
     if (_ordsgn[_i] == 1) return 1;
     return -1;
   }
   if (_ordsgn[_i] == 1) return -1;
   return 1;
#endif
}

// The coefficient will be compared in absolute value
static inline int p_LtCmp(poly p, poly q, const ring r)
{
  int res = p_LmCmp(p,q,r);
  if(res == 0)
  {
    if(p_GetCoeff(p,r) == NULL || p_GetCoeff(q,r) == NULL)
      return res;
    number pc = n_Copy(p_GetCoeff(p,r),r->cf);
    number qc = n_Copy(p_GetCoeff(q,r),r->cf);
    if(!n_GreaterZero(pc,r->cf))
      pc = n_InpNeg(pc,r->cf);
    if(!n_GreaterZero(qc,r->cf))
      qc = n_InpNeg(qc,r->cf);
    if(n_Greater(pc,qc,r->cf))
      res = 1;
    else if(n_Greater(qc,pc,r->cf))
      res = -1;
    else if(n_Equal(pc,qc,r->cf))
      res = 0;
    n_Delete(&pc,r->cf);
    n_Delete(&qc,r->cf);
  }
  return res;
}

// The coefficient will be compared in absolute value
static inline int p_LtCmpNoAbs(poly p, poly q, const ring r)
{
  int res = p_LmCmp(p,q,r);
  if(res == 0)
  {
    if(p_GetCoeff(p,r) == NULL || p_GetCoeff(q,r) == NULL)
      return res;
    number pc = p_GetCoeff(p,r);
    number qc = p_GetCoeff(q,r);
    if(n_Greater(pc,qc,r->cf))
      res = 1;
    if(n_Greater(qc,pc,r->cf))
      res = -1;
    if(n_Equal(pc,qc,r->cf))
      res = 0;
  }
  return res;
}

#ifdef HAVE_RINGS
// This is the equivalent of pLmCmp(p,q) != -currRing->OrdSgn for rings
// It is used in posInLRing and posInTRing
static inline int p_LtCmpOrdSgnDiffM(poly p, poly q, const ring r)
{
  return(p_LtCmp(p,q,r) == r->OrdSgn);
}
#endif

#ifdef HAVE_RINGS
// This is the equivalent of pLmCmp(p,q) != currRing->OrdSgn for rings
// It is used in posInLRing and posInTRing
static inline int p_LtCmpOrdSgnDiffP(poly p, poly q, const ring r)
{
  if(r->OrdSgn == 1)
  {
    return(p_LmCmp(p,q,r) == -1);
  }
  else
  {
    return(p_LtCmp(p,q,r) != -1);
  }
}
#endif

#ifdef HAVE_RINGS
// This is the equivalent of pLmCmp(p,q) == -currRing->OrdSgn for rings
// It is used in posInLRing and posInTRing
static inline int p_LtCmpOrdSgnEqM(poly p, poly q, const ring r)
{
  return(p_LtCmp(p,q,r) == -r->OrdSgn);
}
#endif

#ifdef HAVE_RINGS
// This is the equivalent of pLmCmp(p,q) == currRing->OrdSgn for rings
// It is used in posInLRing and posInTRing
static inline int p_LtCmpOrdSgnEqP(poly p, poly q, const ring r)
{
  return(p_LtCmp(p,q,r) == r->OrdSgn);
}
#endif

/// returns TRUE if p1 is a skalar multiple of p2
/// assume p1 != NULL and p2 != NULL
BOOLEAN p_ComparePolys(poly p1,poly p2, const ring r);


/***************************************************************
 *
 * Comparisons: they are all done without regarding coeffs
 *
 ***************************************************************/
#define p_LmCmpAction(p, q, r, actionE, actionG, actionS) \
  _p_LmCmpAction(p, q, r, actionE, actionG, actionS)

// returns 1 if ExpVector(p)==ExpVector(q): does not compare numbers !!
#define p_LmEqual(p1, p2, r) p_ExpVectorEqual(p1, p2, r)

// pCmp: args may be NULL
// returns: (p2==NULL ? 1 : (p1 == NULL ? -1 : p_LmCmp(p1, p2)))
static inline int p_Cmp(poly p1, poly p2, ring r)
{
  if (p2==NULL)
  {
    if (p1==NULL) return 0;
    return 1;
  }
  if (p1==NULL)
    return -1;
  return p_LmCmp(p1,p2,r);
}

static inline int p_CmpPolys(poly p1, poly p2, ring r)
{
  if (p2==NULL)
  {
    if (p1==NULL) return 0;
    return 1;
  }
  if (p1==NULL)
    return -1;
  return p_ComparePolys(p1,p2,r);
}


/***************************************************************
 *
 * divisibility
 *
 ***************************************************************/
/// return: FALSE, if there exists i, such that a->exp[i] > b->exp[i]
///         TRUE, otherwise
/// (1) Consider long vars, instead of single exponents
/// (2) Clearly, if la > lb, then FALSE
/// (3) Suppose la <= lb, and consider first bits of single exponents in l:
///     if TRUE, then value of these bits is la ^ lb
///     if FALSE, then la-lb causes an "overflow" into one of those bits, i.e.,
///               la ^ lb != la - lb
static inline BOOLEAN _p_LmDivisibleByNoComp(poly a, poly b, const ring r)
{
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;

  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = a->exp[i];
      lb = b->exp[i];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = a->exp[r->VarL_Offset[i]];
      lb = b->exp[r->VarL_Offset[i]];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == FALSE);
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
/*#ifdef HAVE_RINGS
  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == n_DivBy(p_GetCoeff(b, r), p_GetCoeff(a, r), r->cf));
  return (!rField_is_Ring(r)) || n_DivBy(p_GetCoeff(b, r), p_GetCoeff(a, r), r->cf);
#else
*/
  pDivAssume(p_DebugLmDivisibleByNoComp(a, b, r) == TRUE);
  return TRUE;
//#endif
}

static inline BOOLEAN _p_LmDivisibleByNoComp(poly a, const ring r_a, poly b, const ring r_b)
{
  int i=r_a->N;
  pAssume1(r_a->N == r_b->N);

  do
  {
    if (p_GetExp(a,i,r_a) > p_GetExp(b,i,r_b))
    {
      return FALSE;
    }
    i--;
  }
  while (i);
/*#ifdef HAVE_RINGS
  return n_DivBy(p_GetCoeff(b, r_b), p_GetCoeff(a, r_a), r_a->cf);
#else
*/
  return TRUE;
//#endif
}

#ifdef HAVE_RATGRING
static inline BOOLEAN _p_LmDivisibleByNoCompPart(poly a, const ring r_a, poly b, const ring r_b,const int start, const int end)
{
  int i=end;
  pAssume1(r_a->N == r_b->N);

  do
  {
    if (p_GetExp(a,i,r_a) > p_GetExp(b,i,r_b))
      return FALSE;
    i--;
  }
  while (i>=start);
/*#ifdef HAVE_RINGS
  return n_DivBy(p_GetCoeff(b, r_b), p_GetCoeff(a, r_a), r_a->cf);
#else
*/
  return TRUE;
//#endif
}
static inline BOOLEAN _p_LmDivisibleByPart(poly a, const ring r_a, poly b, const ring r_b,const int start, const int end)
{
  if (p_GetComp(a, r_a) == 0 || p_GetComp(a,r_a) == p_GetComp(b,r_b))
    return _p_LmDivisibleByNoCompPart(a, r_a, b, r_b,start,end);
  return FALSE;
}
static inline BOOLEAN p_LmDivisibleByPart(poly a, poly b, const ring r,const int start, const int end)
{
  p_LmCheckPolyRing1(b, r);
  pIfThen1(a != NULL, p_LmCheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoCompPart(a, r, b, r,start, end);
  return FALSE;
}
#endif
static inline BOOLEAN _p_LmDivisibleBy(poly a, poly b, const ring r)
{
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoComp(a, b, r);
  return FALSE;
}
static inline BOOLEAN p_LmDivisibleByNoComp(poly a, poly b, const ring r)
{
  p_LmCheckPolyRing1(a, r);
  p_LmCheckPolyRing1(b, r);
  return _p_LmDivisibleByNoComp(a, b, r);
}

static inline BOOLEAN p_LmDivisibleByNoComp(poly a, const ring ra, poly b, const ring rb)
{
  p_LmCheckPolyRing1(a, ra);
  p_LmCheckPolyRing1(b, rb);
  return _p_LmDivisibleByNoComp(a, ra, b, rb);
}

static inline BOOLEAN p_LmDivisibleBy(poly a, poly b, const ring r)
{
  p_LmCheckPolyRing1(b, r);
  pIfThen1(a != NULL, p_LmCheckPolyRing1(b, r));
  if (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r))
    return _p_LmDivisibleByNoComp(a, b, r);
  return FALSE;
}

static inline BOOLEAN p_DivisibleBy(poly a, poly b, const ring r)
{
  pIfThen1(b!=NULL, p_LmCheckPolyRing1(b, r));
  pIfThen1(a!=NULL, p_LmCheckPolyRing1(a, r));

  if (a != NULL && (p_GetComp(a, r) == 0 || p_GetComp(a,r) == p_GetComp(b,r)))
      return _p_LmDivisibleByNoComp(a,b,r);
  return FALSE;
}

static inline BOOLEAN p_LmShortDivisibleBy(poly a, unsigned long sev_a,
                                    poly b, unsigned long not_sev_b, const ring r)
{
  p_LmCheckPolyRing1(a, r);
  p_LmCheckPolyRing1(b, r);
#ifndef PDIV_DEBUG
  _pPolyAssume2(p_GetShortExpVector(a, r) == sev_a, a, r);
  _pPolyAssume2(p_GetShortExpVector(b, r) == ~ not_sev_b, b, r);

  if (sev_a & not_sev_b)
  {
    pAssume1(p_LmDivisibleByNoComp(a, b, r) == FALSE);
    return FALSE;
  }
  return p_LmDivisibleBy(a, b, r);
#else
  return pDebugLmShortDivisibleBy(a, sev_a, r, b, not_sev_b, r);
#endif
}

static inline BOOLEAN p_LmShortDivisibleByNoComp(poly a, unsigned long sev_a,
                                           poly b, unsigned long not_sev_b, const ring r)
{
  p_LmCheckPolyRing1(a, r);
  p_LmCheckPolyRing1(b, r);
#ifndef PDIV_DEBUG
  _pPolyAssume2(p_GetShortExpVector(a, r) == sev_a, a, r);
  _pPolyAssume2(p_GetShortExpVector(b, r) == ~ not_sev_b, b, r);

  if (sev_a & not_sev_b)
  {
    pAssume1(p_LmDivisibleByNoComp(a, b, r) == FALSE);
    return FALSE;
  }
  return p_LmDivisibleByNoComp(a, b, r);
#else
  return pDebugLmShortDivisibleByNoComp(a, sev_a, r, b, not_sev_b, r);
#endif
}

/***************************************************************
 *
 * Misc things on Lm
 *
 ***************************************************************/


/// like the respective p_LmIs* routines, except that p might be empty
static inline BOOLEAN p_IsConstantComp(const poly p, const ring r)
{
  if (p == NULL) return TRUE;
  return (pNext(p)==NULL) && p_LmIsConstantComp(p, r);
}

static inline BOOLEAN p_IsConstant(const poly p, const ring r)
{
  if (p == NULL) return TRUE;
  return (pNext(p)==NULL) && p_LmIsConstant(p, r);
}

/// either poly(1)  or gen(k)?!
static inline BOOLEAN p_IsOne(const poly p, const ring R)
{
  if (p == NULL) return FALSE; /* TODO check if 0 == 1 */
  p_Test(p, R);
  return (p_IsConstant(p, R) && n_IsOne(p_GetCoeff(p, R), R->cf));
}

static inline BOOLEAN p_IsConstantPoly(const poly p, const ring r)
{
  p_Test(p, r);
  poly pp=p;
  while(pp!=NULL)
  {
    if (! p_LmIsConstantComp(pp, r))
      return FALSE;
    pIter(pp);
  }
  return TRUE;
}

static inline BOOLEAN p_IsUnit(const poly p, const ring r)
{
  if (p == NULL) return FALSE;
  if (rField_is_Ring(r))
    return (p_LmIsConstant(p, r) && n_IsUnit(pGetCoeff(p),r->cf));
  return p_LmIsConstant(p, r);
}

static inline BOOLEAN p_LmExpVectorAddIsOk(const poly p1, const poly p2,
                                      const ring r)
{
  p_LmCheckPolyRing(p1, r);
  p_LmCheckPolyRing(p2, r);
  unsigned long l1, l2, divmask = r->divmask;
  int i;

  for (i=0; i<r->VarL_Size; i++)
  {
    l1 = p1->exp[r->VarL_Offset[i]];
    l2 = p2->exp[r->VarL_Offset[i]];
    // do the divisiblity trick
    if ( (l1 > ULONG_MAX - l2) ||
         (((l1 & divmask) ^ (l2 & divmask)) != ((l1 + l2) & divmask)))
      return FALSE;
  }
  return TRUE;
}
void      p_Split(poly p, poly * r);   /*p => IN(p), r => REST(p) */
BOOLEAN p_HasNotCF(poly p1, poly p2, const ring r);
BOOLEAN p_HasNotCFRing(poly p1, poly p2, const ring r);
poly      p_mInit(const char *s, BOOLEAN &ok, const ring r); /* monom s -> poly, interpreter */
const char *    p_Read(const char *s, poly &p,const ring r); /* monom -> poly */
poly      p_MDivide(poly a, poly b, const ring r);
poly      p_DivideM(poly a, poly b, const ring r);
poly      pp_DivideM(poly a, poly b, const ring r);
poly      p_Div_nn(poly p, const number n, const ring r);

// returns the LCM of the head terms of a and b in *m, does not p_Setm
void p_Lcm(const poly a, const poly b, poly m, const ring r);
// returns the LCM of the head terms of a and b, does p_Setm
poly p_Lcm(const poly a, const poly b, const ring r);

#ifdef HAVE_RATGRING
poly p_LcmRat(const poly a, const poly b, const long lCompM, const ring r);
poly p_GetCoeffRat(poly p, int ishift, ring r);
void p_LmDeleteAndNextRat(poly *p, int ishift, ring r);
void p_ContentRat(poly &ph, const ring r);
#endif /* ifdef HAVE_RATGRING */


poly      p_Diff(poly a, int k, const ring r);
poly      p_DiffOp(poly a, poly b,BOOLEAN multiply, const ring r);
int       p_Weight(int c, const ring r);

///   assumes that p and divisor are univariate polynomials in r,
///   mentioning the same variable;
///   assumes divisor != NULL;
///   p may be NULL;
///   assumes a global monomial ordering in r;
///   performs polynomial division of p by divisor:
///     - afterwards p contains the remainder of the division, i.e.,
///       p_before = result * divisor + p_afterwards;
///     - if needResult == TRUE, then the method computes and returns 'result',
///       otherwise NULL is returned (This parametrization can be used when
///       one is only interested in the remainder of the division. In this
///       case, the method will be slightly faster.)
///   leaves divisor unmodified
poly      p_PolyDiv(poly &p, const poly divisor, const BOOLEAN needResult, const ring r);

/* syszygy stuff */
BOOLEAN   p_VectorHasUnitB(poly p, int * k, const ring r);
void      p_VectorHasUnit(poly p, int * k, int * len, const ring r);
poly      p_TakeOutComp1(poly * p, int k, const ring r);
// Splits *p into two polys: *q which consists of all monoms with
// component == comp and *p of all other monoms *lq == pLength(*q)
// On return all components pf *q == 0
void p_TakeOutComp(poly *p, long comp, poly *q, int *lq, const ring r);

// This is something weird -- Don't use it, unless you know what you are doing
poly      p_TakeOutComp(poly * p, int k, const ring r);

void      p_DeleteComp(poly * p,int k, const ring r);

/*-------------ring management:----------------------*/

// resets the pFDeg and pLDeg: if pLDeg is not given, it is
// set to currRing->pLDegOrig, i.e. to the respective LDegProc which
// only uses pFDeg (and not pDeg, or pTotalDegree, etc).
// If you use this, make sure your procs does not make any assumptions
// on ordering and/or OrdIndex -- otherwise they might return wrong results
// on strat->tailRing
void pSetDegProcs(ring r, pFDegProc new_FDeg, pLDegProc new_lDeg = NULL);
// restores pFDeg and pLDeg:
void pRestoreDegProcs(ring r, pFDegProc old_FDeg, pLDegProc old_lDeg);

/*-------------pComp for syzygies:-------------------*/
void p_SetModDeg(intvec *w, ring r);

/*------------ Jet ----------------------------------*/
poly pp_Jet(poly p, int m, const ring R);
poly p_Jet(poly p, int m,const ring R);
poly pp_JetW(poly p, int m, int *w, const ring R);
poly p_JetW(poly p, int m, int *w, const ring R);

poly n_PermNumber(const number z, const int *par_perm, const int OldPar, const ring src, const ring dst);

poly p_PermPoly (poly p, const int * perm,const ring OldRing, const ring dst,
                     nMapFunc nMap, const int *par_perm=NULL, int OldPar=0,
                     BOOLEAN use_mult=FALSE);

/*----------------------------------------------------*/
poly p_Series(int n,poly p,poly u, intvec *w, const ring R);

/*----------------------------------------------------*/
int   p_Var(poly mi, const ring r);
/// the minimal index of used variables - 1
int   p_LowVar (poly p, const ring r);

/*----------------------------------------------------*/
/// shifts components of the vector p by i
void p_Shift (poly * p,int i, const ring r);
/*----------------------------------------------------*/

int p_Compare(const poly a, const poly b, const ring R);

/// polynomial gcd for f=mon
poly p_GcdMon(poly f, poly g, const ring r);

/// divide polynomial by monomial
poly p_Div_mm(poly p, const poly m, const ring r);


/// max exponent of variable x_i in p
int p_MaxExpPerVar(poly p, int i, const ring r);
#endif // P_POLYS_H

