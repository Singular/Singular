/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: kernel: utils for shift GB and free GB
*/

#include "kernel/mod2.h"

#ifdef HAVE_SHIFTBBA
#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "coeffs/numbers.h"
#include "kernel/ideals.h"
#include "polys/matpol.h"
#include "polys/kbuckets.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/sbuckets.h"
#include "polys/operations/p_Mult_q.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/structs.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/structs.h"
#include "kernel/GBEngine/kInline.h"
#include "kernel/combinatorics/stairc.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/GBEngine/shiftgb.h"
#include "polys/nc/sca.h"


#define freeT(A,v) omFreeSize((ADDRESS)A,(v+1)*sizeof(int))

poly p_LPshift(poly p, int sh, const ring r)
{
  if (sh == 0 || p == NULL) return(p);

  poly q  = NULL;
  poly pp = p;
  while (pp!=NULL)
  {
    poly h=pp;
    pIter(pp);
    pNext(h)=NULL;
    h=p_mLPshift(h,sh,r);
    q = p_Add_q(q, h,r);
  }
  return(q);
}

poly p_mLPshift(poly p, int sh, const ring r)
{
  if (sh == 0 || p == NULL || p_LmIsConstantComp(p,r)) return(p);

  int lV = r->isLPring;

  int L = p_mLastVblock(p,r);
  assume(L+sh>=1);
  assume(L+sh<=r->N/lV);

  int *e=(int *)omAlloc0((r->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(p,e,r);

  int j;
  //  for (j=1; j<=r->N; j++)
  // L*lV gives the last position of the last block
  for (j=1; j<= L*lV ; j++)
  {
    assume(e[j]<=1);
    if (e[j]==1)
    {
      assume(j + (sh*lV)<=r->N);
      assume(j + (sh*lV)>=1);
      s[j + (sh*lV)] = e[j]; /* actually 1 */
    }
  }
  p_SetExpV(p,s,r);
  freeT(e, r->N);
  freeT(s, r->N);
  /*  pSetm(m); */ /* done in the pSetExpV */
  /* think on the component and coefficient */
  //  number c = pGetCoeff(p);
  //  p_SetCoeff0(m,p_GetCoeff(p,r),r);
  return(p);
}

poly p_LPCopyAndShiftLM(poly p, int sh, const ring r)
{
  if (sh == 0 || p == NULL) return p;
  poly q = p_mLPshift(p_Head(p, r), sh, r);
  pNext(q) = pNext(p);
  return q;
}

/* returns the number of maximal block */
/* appearing among the monomials of p */
/* the 0th block is the 1st one */
int p_LastVblock(poly p, const ring r)
{
  poly q = p;
  int ans = 0;
  while (q!=NULL)
  {
    int ansnew = p_mLastVblock(q, r);
    ans    = si_max(ans,ansnew);
    pIter(q);
  }
  return(ans);
}

/* for a monomial p, returns the number of the last block */
/* where a nonzero exponent is sitting */
int p_mLastVblock(poly p, const ring r)
{
  if (p == NULL || p_LmIsConstantComp(p,r))
  {
    return(0);
  }

  int lV = r->isLPring;
  int *e=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(p,e,r);
  int j,b;
  j = r->N;
  while ( (!e[j]) && (j>=1) ) j--;
  freeT(e, r->N);
  assume(j>0);
  b = (int)((j+lV-1)/lV); /* the number of the block, >=1 */
  return (b);
}

/* returns the number of maximal block */
/* appearing among the monomials of p */
/* the 0th block is the 1st one */
int p_FirstVblock(poly p, const ring r)
{
  if (p == NULL) {
    return 0;
  }

  poly q = p;
  int ans = p_mFirstVblock(q, r);
  while (q!=NULL)
  {
    int ansnew = p_mFirstVblock(q, r);
    if (ansnew > 0) { // don't count constants
      ans = si_min(ans,ansnew);
    }
    pIter(q);
  }
  /* do not need to delete q */
  return(ans);
}

/* for a monomial p, returns the number of the first block */
/* where a nonzero exponent is sitting */
int p_mFirstVblock(poly p, const ring r)
{
  if (p == NULL || p_LmIsConstantComp(p,r))
  {
    return(0);
  }

  int lV = r->isLPring;
  int *e=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(p,e,r);
  int j,b;
  j = 1;
  while ( (!e[j]) && (j<=r->N-1) ) j++;
  freeT(e, r->N);
  assume(j <= r->N);
  b = (int)(j+lV-1)/lV; /* the number of the block, 1<= b <= r->N  */
  return (b);
}

/* there should be two routines: */
/* 1. test place-squarefreeness: in homog this suffices: isInV */
/* 2. test the presence of a hole -> in the tail??? */

int isInV(poly p, const ring r)
{
  int lV = r->isLPring;
  /* investigate only the leading monomial of p in currRing */
  if ( pTotaldegree(p)==0 ) return(1);
  /* returns 1 iff p is in V */
  /* that is in each block up to a certain one there is only one nonzero exponent */
  /* lV = the length of V = the number of orig vars */
  int *e = (int *)omAlloc0((currRing->N+1)*sizeof(int));
  int  b = (int)((currRing->N +lV-1)/lV); /* the number of blocks */
  //int b  = (int)(currRing->N)/lV;
  int *B = (int *)omAlloc0((b+1)*sizeof(int)); /* the num of elements in a block */
  p_GetExpV(p,e,currRing);
  int i,j;
  for (j=1; j<=b; j++)
  {
    /* we go through all the vars */
    /* by blocks in lV vars */
    for (i=(j-1)*lV + 1; i<= j*lV; i++)
    {
      if (e[i]) B[j] = B[j]+1;
    }
  }
  //  j = b;
  //  while ( (!B[j]) && (j>=1)) j--;
  for (j=b; j>=1; j--)
  {
    if (B[j]!=0) break;
  }
  /* do not need e anymore */
  freeT(e, currRing->N);

  if (j==0) goto ret_true;
//   {
//     /* it is a zero exp vector, which is in V */
//     freeT(B, b);
//     return(1);
//   }
  /* now B[j] != 0 and we test place-squarefreeness */
  for (; j>=1; j--)
  {
    if (B[j]!=1)
    {
      freeT(B, b);
      return(0);
    }
  }
 ret_true:
  freeT(B, b);
  return(1);
}

/* tests whether the whole polynomial p in in V */
int poly_isInV(poly p, const ring r)
{
  poly q = p;
  while (q!=NULL)
  {
    if ( !isInV(q, r) )
    {
      return(0);
    }
    q = pNext(q);
  }
  return(1);
}

/* tests whether each polynomial of an ideal I lies in in V */
int ideal_isInV(ideal I, const ring r)
{
  int i;
  int s    = IDELEMS(I)-1;
  for(i = 0; i <= s; i++)
  {
    if ( !poly_isInV(I->m[i], r) )
    {
      return(0);
    }
  }
  return(1);
}


/* for poly in lmCR/tailTR presentation */
int itoInsert(poly p, const ring r)
{
  /* the below situation (commented out) might happen! */
//   if (r == currRing)
//   {
//     "Current ring is not expected in toInsert";
//     return(0);
//   }
  /* compute the number of insertions */
  int i = p_mLastVblock(p, currRing);
  if (pNext(p) != NULL)
  {
    i = si_max(i, p_LastVblock(pNext(p), r) );
  }
  //  i = uptodeg  - i +1;
  int uptodeg = r->N/r->isLPring;
  //  p_wrp(p,currRing,r); Print("----i:%d",i); PrintLn();
  return uptodeg - i;
}

// splits a frame (e.g. x(1)*y(5)) m1 into m1 and m2 (e.g. m1=x(1) and m2=y(1))
// according to p which is inside the frame
void k_SplitFrame(poly &m1, poly &m2, int at, const ring r) {
  int lV = r->isLPring;

  number m1Coeff = pGetCoeff(m1);

  int hole = lV * at;
  m2 = p_GetExp_k_n(m1, 1, hole, r);
  m1 = p_GetExp_k_n(m1, hole, r->N, r);

  p_mLPshift(m2, 1 - p_mFirstVblock(m2, r), r);
  p_SetCoeff(m1, m1Coeff, r);
}

#endif
