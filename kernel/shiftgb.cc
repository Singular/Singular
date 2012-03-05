/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: kernel: utils for shift GB and free GB
*/

#include <kernel/mod2.h>

#ifdef HAVE_SHIFTBBA
#include <kernel/febase.h>
#include <kernel/ring.h>
#include <kernel/polys.h>
#include <kernel/numbers.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <kernel/kbuckets.h>
#include <kernel/kstd1.h>
#include <kernel/sbuckets.h>
#include <kernel/p_Mult_q.h>
#include <kernel/kutil.h>
#include <kernel/structs.h>
#include <omalloc/omalloc.h>
#include <kernel/khstd.h>
#include <kernel/kbuckets.h>
#include <kernel/weight.h>
#include <kernel/intvec.h>
#include <kernel/structs.h>
#include <kernel/kInline.cc>
#include <kernel/stairc.h>
#include <kernel/weight.h>
#include <kernel/intvec.h>
#include <kernel/timer.h>
#include <kernel/shiftgb.h>
#include <kernel/sca.h>


#define freeT(A,v) omFreeSize((ADDRESS)A,(v+1)*sizeof(int))


/* TODO: write p* stuff as instances of p_* for all the functions */
/* p_* functions are new, p* are old */

poly p_LPshiftT(poly p, int sh, int uptodeg, int lV, kStrategy strat, const ring r)
{
  /* assume shift takes place, shifts the poly p by sh */
  /* p is like TObject: lm in currRing = r, tail in tailRing  */

  if (p==NULL) return(p);

  assume(p_LmCheckIsFromRing(p,r));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));

  /* assume sh and uptodeg agree  TODO check */

  if (sh == 0) return(p); /* the zero shift */

  poly q   = NULL;
  poly s   = p_mLPshift(p, sh, uptodeg, lV, r); // lm in currRing
  poly pp = pNext(p);

  while (pp != NULL)
  {
    q = p_Add_q(q, p_mLPshift(pp,sh,uptodeg,lV,strat->tailRing),strat->tailRing);
    pIter(pp);
  }
  pNext(s) = q;
  /* int version: returns TRUE if it was successful */
  return(s);
}


poly p_LPshift(poly p, int sh, int uptodeg, int lV, const ring r)
{
  /* assume shift takes place */
  /* shifts the poly p from the ring r by sh */

  /* assume sh and uptodeg agree TODO check */

  if (p==NULL) return(p);
  if (sh == 0) return(p); /* the zero shift */

  poly q  = NULL;
  poly pp = p; // do not take copies
  while (pp!=NULL)
  {
    q = p_Add_q(q, p_mLPshift(pp,sh,uptodeg,lV,r),r);
    pIter(pp);
  }
  return(q);
}

poly p_mLPshift(poly p, int sh, int uptodeg, int lV, const ring r)
{
  /* p is a monomial from the ring r */

  if (sh == 0) return(p); /* the zero shift */

  if (sh < 0 )
  {
#ifdef PDEBUG
    PrintS("pmLPshift: negative shift requested\n");
#endif
    return(NULL); /* violation, 2check */
  }

  int L = p_mLastVblock(p,lV,r);
  if (L+sh-1 > uptodeg)
  {
#ifdef PDEBUG
    PrintS("p_mLPshift: too big shift requested\n");
#endif
    return(NULL); /* violation, 2check */
  }
  int *e=(int *)omAlloc0((r->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(p,e,r);

  int j;
  //  for (j=1; j<=r->N; j++)
  // L*lV gives the last position of the last block
  for (j=1; j<= L*lV ; j++)
  {
    if (e[j]==1)
    {
      s[j + (sh*lV)] = e[j]; /* actually 1 */
#ifdef PDEBUG
      omCheckAddr(s);
#endif
    }
#ifdef PDEBUG
    else
    {
      if (e[j]!=0)
      {
         //         Print("p_mLPshift: ex[%d]=%d\n",j,e[j]);
      }
    }
#endif
  }
  poly m = p_One(r);
  p_SetExpV(m,s,r);
  freeT(e, r->N);
  freeT(s, r->N);
  /*  pSetm(m); */ /* done in the pSetExpV */
  /* think on the component and coefficient */
  //  number c = pGetCoeff(p);
  //  p_SetCoeff0(m,p_GetCoeff(p,r),r);
  p_SetComp(m,p_GetComp(p,r),r); // component is preserved
  p_SetCoeff0(m,n_Copy(p_GetCoeff(p,r),r),r);  // coeff is preserved
  return(m);
}

poly pLPshift(poly p, int sh, int uptodeg, int lV)
{
  /* assume shift takes place */
  /* shifts the poly p by sh */
  /* deletes p */

  /* assume sh and uptodeg agree */

  if (sh == 0) return(p); /* the zero shift */

  poly q  = NULL;
  poly pp = p; // pCopy(p);
  while (pp!=NULL)
  {
    q = p_Add_q(q, pmLPshift(pp,sh,uptodeg,lV),currRing);
    pIter(pp);
  }
  /* delete pp? */
  /* int version: returns TRUE if it was successful */
  return(q);
}

poly pmLPshift(poly p, int sh, int uptodeg, int lV)
{
  /* TODO: use a shortcut with p_ version */
  /* pm is a monomial */

  if (sh == 0) return(p); /* the zero shift */

  if (sh < 0 )
  {
#ifdef PDEBUG
    PrintS("pmLPshift: negative shift requested\n");
#endif
    return(NULL); /* violation, 2check */
  }

  int L = pmLastVblock(p,lV);
  if (L+sh-1 > uptodeg)
  {
#ifdef PDEBUG
    PrintS("pmLPshift: too big shift requested\n");
#endif
    return(NULL); /* violation, 2check */
  }
  int *e=(int *)omAlloc0((currRing->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((currRing->N+1)*sizeof(int));
  pGetExpV(p,e);
  number c = pGetCoeff(p);
  int j;
  for (j=1; j<=currRing->N; j++)
  {
    if (e[j]==1)
    {
      s[j + (sh*lV)] = e[j]; /* actually 1 */
    }
  }
  poly m = pOne();
  pSetExpV(m,s);
  /*  pSetm(m); */ /* done in the pSetExpV */
  /* think on the component */
  pSetCoeff0(m,c);
  freeT(e, currRing->N);
  freeT(s, currRing->N);
  return(m);
}

int pLastVblock(poly p, int lV)
{
  /* returns the number of maximal block */
  /* appearing among the monomials of p */
  /* the 0th block is the 1st one */
  poly q = p; //p_Copy(p,currRing); /* need it ? */
  int ans = 0;
  int ansnew = 0;
  while (q!=NULL)
  {
    ansnew = pmLastVblock(q,lV);
    ans    = si_max(ans,ansnew);
    pIter(q);
  }
  /* do not need to delete q */
  return(ans);
}

int pmLastVblock(poly p, int lV)
{
  /* for a monomial p, returns the number of the last block */
  /* where a nonzero exponent is sitting */
  if (pIsConstantPoly(p))
  {
    return(int(0));
  }
  int *e=(int *)omAlloc0((currRing->N+1)*sizeof(int));
  pGetExpV(p,e);
  int j,b;
  j = currRing->N;
  while ( (!e[j]) && (j>=1) ) j--;
  if (j==0)
  {
#ifdef PDEBUG
    PrintS("pmLastVblock: unexpected zero exponent vector\n");
#endif
    freeT(e, currRing->N);
    return(j);
  }
  b = (int)(j/lV) + 1; /* the number of the block, >=1 */
  freeT(e, currRing->N);
  return (b);
}

int p_LastVblockT(poly p, int lV, kStrategy strat, const ring r)
{
  /* returns the number of maximal block */
  /* appearing among the monomials of p */
  /* the 0th block is the 1st one */

  /* p is like TObject: lm in currRing = r, tail in tailRing  */
  assume(p_LmCheckIsFromRing(p,r));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));

  int ans = p_mLastVblock(p, lV, r); // Block of LM
  poly q = pNext(p);
  int ansnew = 0;
  while (q != NULL)
  {
    ansnew = p_mLastVblock(q, lV, strat->tailRing);
    ans       = si_max(ans,ansnew);
    pIter(q);
  }
  /* do not need to delete q */
  return(ans);
}

int p_LastVblock(poly p, int lV, const ring r)
{
  /* returns the number of maximal block */
  /* appearing among the monomials of p */
  /* the 0th block is the 1st one */
  poly q = p; //p_Copy(p,currRing); /* need it ? */
  int ans = 0;
  int ansnew = 0;
  while (q!=NULL)
  {
    ansnew = p_mLastVblock(q, lV, r);
    ans    = si_max(ans,ansnew);
    pIter(q);
  }
  /* do not need to delete q */
  return(ans);
}

int p_mLastVblock(poly p, int lV, const ring r)
{
  /* for a monomial p, returns the number of the last block */
  /* where a nonzero exponent is sitting */
  if (p_LmIsConstant(p,r))
  {
    return(0);
  }
  int *e=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(p,e,r);
  int j,b;
  j = r->N;
  while ( (!e[j]) && (j>=1) ) j--;
  if (j==0)
  {
#ifdef PDEBUG
    PrintS("pmLastVblock: unexpected zero exponent vector\n");
#endif
    return(j);
  }
  b = (int)((j+lV-1)/lV); /* the number of the block, >=1 */
  freeT(e,r->N);
  return (b);
}

int pFirstVblock(poly p, int lV)
{
  /* returns the number of maximal block */
  /* appearing among the monomials of p */
  /* the 0th block is the 1st one */
  poly q = p; //p_Copy(p,currRing); /* need it ? */
  int ans = 0;
  int ansnew = 0;
  while (q!=NULL)
  {
    ansnew = pmFirstVblock(q,lV);
    ans    = si_min(ans,ansnew);
    pIter(q);
  }
  /* do not need to delete q */
  return(ans);
}

int pmFirstVblock(poly p, int lV)
{
  if (pIsConstantPoly(p))
  {
    return(int(0));
  }
  /* for a monomial p, returns the number of the first block */
  /* where a nonzero exponent is sitting */
  int *e=(int *)omAlloc0((currRing->N+1)*sizeof(int));
  pGetExpV(p,e);
  int j,b;
  j = 1;
  while ( (!e[j]) && (j<=currRing->N-1) ) j++;
  if (j==currRing->N + 1)
  {
#ifdef PDEBUG
    PrintS("pmFirstVblock: unexpected zero exponent vector\n");
#endif
    return(j);
  }
  b = (int)(j/lV)+1; /* the number of the block, 1<= N <= currRing->N  */
  return (b);
}

  /* there should be two routines: */
  /* 1. test place-squarefreeness: in homog this suffices: isInV */
  /* 2. test the presence of a hole -> in the tail??? */

int isInV(poly p, int lV)
{
  /* investigate only the leading monomial of p in currRing */
  if ( pIsConstant(p) ) return(1);
  if (lV <= 0) return(0);
  /* returns 1 iff p is in V */
  /* that is in each block up to a certain one there is only one nonzero exponent */
  /* lV = the length of V = the number of orig vars */
  int *e = (int *)omAlloc0((currRing->N+1)*sizeof(int));
  int  b = (int)((currRing->N +lV-1)/lV); /* the number of blocks */
  //int b  = (int)(currRing->N)/lV;
  int *B = (int *)omAlloc0((b+1)*sizeof(int)); /* the num of elements in a block */
  pGetExpV(p,e);
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

int poly_isInV(poly p, int lV)
{
  /* tests whether the whole polynomial p in in V */
  poly q = p;
  while (q!=NULL)
  {
    if ( !isInV(q,lV) )
    {
      return(0);
    }
    q = pNext(q);
  }
  return(1);
}

int ideal_isInV(ideal I, int lV)
{
  /* tests whether each polynomial of an ideal I lies in in V */
  int i;
  int s    = IDELEMS(I)-1;
  for(i = 0; i <= s; i++)
  {
    if ( !poly_isInV(I->m[i],lV) )
    {
      return(0);
    }
  }
  return(1);
}


int itoInsert(poly p, int uptodeg, int lV, const ring r)
{
  /* for poly in lmCR/tailTR presentation */
  /* the below situation (commented out) might happen! */
//   if (r == currRing)
//   {
//     "Current ring is not expected in toInsert";
//     return(0);
//   }
  /* compute the number of insertions */
  int i = p_mLastVblock(p, lV, currRing);
  if (pNext(p) != NULL)
  {
    i = si_max(i, p_LastVblock(pNext(p), lV, r) );
  }
  //  i = uptodeg  - i +1;
  i = uptodeg  - i;
  //  p_wrp(p,currRing,r); Print("----i:%d",i); PrintLn();
  return(i);
}

poly p_ShrinkT(poly p, int lV, kStrategy strat, const ring r)
//poly p_Shrink(poly p, int uptodeg, int lV, kStrategy strat, const ring r)
{
  /* p is like TObject: lm in currRing = r, tail in tailRing  */
  /* proc shrinks the poly p in ring r */
  /* lV = the length of V = the number of orig vars */
  /* check assumes/exceptions */
  /* r->N is a multiple of lV */

  if (p==NULL) return(p);

  assume(p_LmCheckIsFromRing(p,r));
  assume(p_CheckIsFromRing(pNext(p),strat->tailRing));

  poly q   = NULL;
  poly s   = p_mShrink(p, lV, r); // lm in currRing
  poly pp = pNext(p);

  while (pp != NULL)
  {
    //    q = p_Add_q(q, p_mShrink(pp,uptodeg,lV,strat->tailRing),strat->tailRing);
    q = p_Add_q(q, p_mShrink(pp,lV,strat->tailRing),strat->tailRing);
    pIter(pp);
  }
  pNext(s) = q;
  return(s);
}

poly p_Shrink(poly p, int lV, const ring r)
{
  /* proc shrinks the poly p in ring r */
  /* lV = the length of V = the number of orig vars */
  /* check assumes/exceptions */
  /* r->N is a multiple of lV */

  if (p==NULL) return(p);
  assume(p_CheckIsFromRing(p,r));
  poly q = NULL;
  poly pp = p;

  while (pp != NULL)
  {
    q = p_Add_q(q, p_mShrink(pp,lV,r),r);
    pIter(pp);
  }
  return(q);
}

poly p_mShrink(poly p, int lV, const ring r)
{
  /* shrinks the monomial p in ring r */
  /* lV = the length of V = the number of orig vars */

  /* check assumes/exceptions */
  /* r->N is a multiple of lV */

  int *e = (int *)omAlloc0((r->N+1)*sizeof(int));
  int  b = (int)((r->N +lV-1)/lV); /* the number of blocks */
  //  int *B = (int *)omAlloc0((b+1)*sizeof(int)); /* the num of elements in a block */
  int *S = (int *)omAlloc0((r->N+1)*sizeof(int)); /* the shrinked exponent */
  p_GetExpV(p,e,r);
  int i,j; int cnt = 1; //counter for blocks in S
  for (j=1; j<=b; j++)
  {
    /* we go through all the vars */
    /* by blocks in lV vars */
    for (i=(j-1)*lV + 1; i<= j*lV; i++)
    {
      if (e[i]==1)
      {
         //      B[j] = B[j]+1; // for control in V?
         S[(cnt-1)*lV + (i - (j-1)*lV)] = e[i];
         /* assuming we are in V, can interrupt here */
         cnt++;
         //  break; //results in incomplete shrink!
         i = j*lV; // manual break under assumption p is in V
      }
    }
  }
#ifdef PDEBUG
  //  Print("p_mShrink: cnt = [%d], b = %d\n",cnt,b);
#endif
  // cnt -1 <= b  must hold!
  //  freeT(B, b);
  poly s = p_One(r);
  p_SetExpV(s,S,r);
  freeT(e, r->N);
  freeT(S, r->N);
  /*  p_Setm(s,r); // done by p_SetExpV */
  p_SetComp(s,p_GetComp(p,r),r); // component is preserved
  p_SetCoeff(s,p_GetCoeff(p,r),r);  // coeff is preserved
#ifdef PDEBUG
  //  Print("p_mShrink: from "); p_wrp(p,r); Print(" to "); p_wrp(s,r); PrintLn();
#endif
  return(s);
}

/* shiftgb stuff */


/*2
 *if the leading term of p
 *divides the leading term of some T[i] it will be canceled
 */
// static inline void clearSShift (poly p, unsigned long p_sev,int l, int* at, int* k,
//                            kStrategy strat)
// {
//   assume(p_sev == pGetShortExpVector(p));
//   if (!pLmShortDivisibleBy(p,p_sev, strat->T[*at].p, ~ strat->sevT[*at])) return;
//   //  if (l>=strat->lenS[*at]) return;
//   if (TEST_OPT_PROT)
//     PrintS("!");
//   mflush();
//   //pDelete(&strat->S[*at]);
//   deleteInS((*at),strat);
//   (*at)--;
//   (*k)--;
// //  assume(lenS_correct(strat));
// }

/* remarks: cleanT : just deletion
enlargeT: just reallocation */

#endif
