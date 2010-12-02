/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT - all basic methods to manipulate polynomials
*/

/* includes */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <kernel/mod2.h>
#include <kernel/options.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ring.h>
#include <kernel/sbuckets.h>

#ifdef HAVE_PLURAL
#include <kernel/gring.h>
#include <kernel/sca.h>
#endif

/* ----------- global variables, set by pSetGlobals --------------------- */
/* computes length and maximal degree of a POLYnomial */
pLDegProc pLDeg;
/* computes the degree of the initial term, used for std */
pFDegProc pFDeg;
/* the monomial ordering of the head monomials a and b */
/* returns -1 if a comes before b, 0 if a=b, 1 otherwise */

int pVariables;     // number of variables

/* 1 for polynomial ring, -1 otherwise */
int     pOrdSgn;
// it is of type int, not BOOLEAN because it is also in ip
/* TRUE if the monomial ordering is not compatible with pFDeg */
BOOLEAN pLexOrder;

/* ----------- global variables, set by procedures from hecke/kstd1 ----- */
/* the highest monomial below pHEdge */
poly      ppNoether = NULL;

/* -------------------------------------------------------- */
/*2
* change all global variables to fit the description of the new ring
*/


void pSetGlobals(const ring r, BOOLEAN complete)
{
  int i;
  if (ppNoether!=NULL) pDelete(&ppNoether);
  pVariables = r->N;
  pOrdSgn = r->OrdSgn;
  pFDeg=r->pFDeg;
  pLDeg=r->pLDeg;
  pLexOrder=r->LexOrder;

  if (complete)
  {
    test &= ~ TEST_RINGDEP_OPTS;
    test |= r->options;
  }
}

// resets the pFDeg and pLDeg: if pLDeg is not given, it is
// set to currRing->pLDegOrig, i.e. to the respective LDegProc which
// only uses pFDeg (and not pDeg, or pTotalDegree, etc)
void pSetDegProcs(pFDegProc new_FDeg, pLDegProc new_lDeg)
{
  assume(new_FDeg != NULL);
  pFDeg = new_FDeg;
  currRing->pFDeg = new_FDeg;

  if (new_lDeg == NULL)
    new_lDeg = currRing->pLDegOrig;

  pLDeg = new_lDeg;
  currRing->pLDeg = new_lDeg;
}


// restores pFDeg and pLDeg:
extern void pRestoreDegProcs(pFDegProc old_FDeg, pLDegProc old_lDeg)
{
  assume(old_FDeg != NULL && old_lDeg != NULL);
  pFDeg = old_FDeg;
  currRing->pFDeg = old_FDeg;
  pLDeg = old_lDeg;
  currRing->pLDeg = old_lDeg;
}

/*2
* assumes that the head term of b is a multiple of the head term of a
* and return the multiplicant *m
*/
poly pDivide(poly a, poly b)
{
  int i;
  poly result = pInit();

  for(i=(int)pVariables; i; i--)
    pSetExp(result,i, pGetExp(a,i)- pGetExp(b,i));
  pSetComp(result, pGetComp(a) - pGetComp(b));
  pSetm(result);
  return result;
}

#ifdef HAVE_RINGS   //TODO Oliver
#define pDiv_nn(p, n)              p_Div_nn(p, n, currRing)

poly p_Div_nn(poly p, const number n, const ring r)
{
  pAssume(!n_IsZero(n,r));
  p_Test(p, r);

  poly q = p;
  while (p != NULL)
  {
    number nc = pGetCoeff(p);
    pSetCoeff0(p, n_Div(nc, n, r));
    n_Delete(&nc, r);
    pIter(p);
  }
  p_Test(q, r);
  return q;
}
#endif

/*2
* divides a by the monomial b, ignores monomials which are not divisible
* assumes that b is not NULL
*/
poly pDivideM(poly a, poly b)
{
  if (a==NULL) return NULL;
  poly result=a;
  poly prev=NULL;
  int i;
#ifdef HAVE_RINGS
  number inv=pGetCoeff(b);
#else
  number inv=nInvers(pGetCoeff(b));
#endif

  while (a!=NULL)
  {
    if (pDivisibleBy(b,a))
    {
      for(i=(int)pVariables; i; i--)
         pSubExp(a,i, pGetExp(b,i));
      pSubComp(a, pGetComp(b));
      pSetm(a);
      prev=a;
      pIter(a);
    }
    else
    {
      if (prev==NULL)
      {
        pLmDelete(&result);
        a=result;
      }
      else
      {
        pLmDelete(&pNext(prev));
        a=pNext(prev);
      }
    }
  }
#ifdef HAVE_RINGS
  if (nIsUnit(inv))
  {
    inv = nInvers(inv);
    pMult_nn(result,inv);
    nDelete(&inv);
  }
  else
  {
    pDiv_nn(result,inv);
  }
#else
  pMult_nn(result,inv);
  nDelete(&inv);
#endif
  pDelete(&b);
  return result;
}

/*2
* returns the LCM of the head terms of a and b in *m
*/
void pLcm(poly a, poly b, poly m)
{
  int i;
  for (i=pVariables; i; i--)
  {
    pSetExp(m,i, si_max( pGetExp(a,i), pGetExp(b,i)));
  }
  pSetComp(m, si_max(pGetComp(a), pGetComp(b)));
  /* Don't do a pSetm here, otherwise hres/lres chockes */
}

/*2
* convert monomial given as string to poly, e.g. 1x3y5z
*/
const char * p_Read(const char *st, poly &rc, const ring r)
{
  if (r==NULL) { rc=NULL;return st;}
  int i,j;
  rc = p_Init(r);
  const char *s = r->cf->nRead(st,&(rc->coef));
  if (s==st)
  /* i.e. it does not start with a coeff: test if it is a ringvar*/
  {
    j = r_IsRingVar(s,r);
    if (j >= 0)
    {
      p_IncrExp(rc,1+j,r);
      while (*s!='\0') s++;
      goto done;
    }
  }
  while (*s!='\0')
  {
    char ss[2];
    ss[0] = *s++;
    ss[1] = '\0';
    j = r_IsRingVar(ss,r);
    if (j >= 0)
    {
      const char *s_save=s;
      s = eati(s,&i);
      if (((unsigned long)i) >  r->bitmask)
      {
        // exponent to large: it is not a monomial
        p_LmDelete(&rc,r);
        return s_save;
      }
      p_AddExp(rc,1+j, (long)i, r);
    }
    else
    {
      // 1st char of is not a varname
      p_LmDelete(&rc,r);
      s--;
      return s;
    }
  }
done:
  if (r->cf->nIsZero(pGetCoeff(rc))) p_LmDelete(&rc,r);
  else
  {
#ifdef HAVE_PLURAL
    // in super-commutative ring
    // squares of anti-commutative variables are zeroes!
    if(rIsSCA(r))
    {
      const unsigned int iFirstAltVar = scaFirstAltVar(r);
      const unsigned int iLastAltVar  = scaLastAltVar(r);

      assume(rc != NULL);

      for(unsigned int k = iFirstAltVar; k <= iLastAltVar; k++)
        if( p_GetExp(rc, k, r) > 1 )
        {
          p_LmDelete(&rc, r);
          goto finish;
        }
    }
#endif
    p_Setm(rc,r);
  }
finish:
  return s;
}

BOOLEAN _p_Test(poly p, ring r, int level);
poly pmInit(const char *st, BOOLEAN &ok)
{
  poly p;
  const char *s=p_Read(st,p,currRing);
  if (*s!='\0')
  {
    if ((s!=st)&&isdigit(st[0]))
    {
      errorreported=TRUE;
    }
    ok=FALSE;
    pDelete(&p);
    return NULL;
  }
  #ifdef PDEBUG
  _p_Test(p,currRing,PDEBUG);
  #endif
  ok=!errorreported;
  return p;
}

/*2
*make p homogeneous by multiplying the monomials by powers of x_varnum
*assume: deg(var(varnum))==1
*/
poly pHomogen (poly p, int varnum)
{
  pFDegProc deg;
  if (pLexOrder && (currRing->order[0]==ringorder_lp))
    deg=p_Totaldegree;
  else
    deg=pFDeg;

  poly q=NULL, qn;
  int  o,ii;
  sBucket_pt bp;

  if (p!=NULL)
  {
    if ((varnum < 1) || (varnum > pVariables))
    {
      return NULL;
    }
    o=deg(p,currRing);
    q=pNext(p);
    while (q != NULL)
    {
      ii=deg(q,currRing);
      if (ii>o) o=ii;
      pIter(q);
    }
    q = pCopy(p);
    bp = sBucketCreate(currRing);
    while (q != NULL)
    {
      ii = o-deg(q,currRing);
      if (ii!=0)
      {
        pAddExp(q,varnum, (long)ii);
        pSetm(q);
      }
      qn = pNext(q);
      pNext(q) = NULL;
      sBucket_Add_p(bp, q, 1);
      q = qn;
    }
    sBucketDestroyAdd(bp, &q, &ii);
  }
  return q;
}

/*4
*Returns the exponent of the maximal power of the leading monomial of
*p2 in that of p1
*/
/*----------utilities for syzygies--------------*/
poly pTakeOutComp(poly * p, int k)
{
  poly q = *p,qq=NULL,result = NULL;

  if (q==NULL) return NULL;
  BOOLEAN use_setmcomp=rOrd_SetCompRequiresSetm(currRing);
  if (pGetComp(q)==k)
  {
    result = q;
    do
    {
      pSetComp(q,0);
      if (use_setmcomp) pSetmComp(q);
      qq = q;
      pIter(q);
    }
    while ((q!=NULL) && (pGetComp(q)==k));
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
  if (pGetComp(q) > k)
  {
    pSubComp(q,1);
    if (use_setmcomp) pSetmComp(q);
  }
  poly pNext_q;
  while ((pNext_q=pNext(q))!=NULL)
  {
    if (pGetComp(pNext_q)==k)
    {
      if (result==NULL)
      {
        result = pNext_q;
        qq = result;
      }
      else
      {
        pNext(qq) = pNext_q;
        pIter(qq);
      }
      pNext(q) = pNext(pNext_q);
      pNext(qq) =NULL;
      pSetComp(qq,0);
      if (use_setmcomp) pSetmComp(qq);
    }
    else
    {
      /*pIter(q);*/ q=pNext_q;
      if (pGetComp(q) > k)
      {
        pSubComp(q,1);
        if (use_setmcomp) pSetmComp(q);
      }
    }
  }
  return result;
}

// Splits *p into two polys: *q which consists of all monoms with
// component == comp and *p of all other monoms *lq == pLength(*q)
void pTakeOutComp(poly *r_p, long comp, poly *r_q, int *lq)
{
  spolyrec pp, qq;
  poly p, q, p_prev;
  int l = 0;

#ifdef HAVE_ASSUME
  int lp = pLength(*r_p);
#endif

  pNext(&pp) = *r_p;
  p = *r_p;
  p_prev = &pp;
  q = &qq;

  while(p != NULL)
  {
    while (pGetComp(p) == comp)
    {
      pNext(q) = p;
      pIter(q);
      pSetComp(p, 0);
      pSetmComp(p);
      pIter(p);
      l++;
      if (p == NULL)
      {
        pNext(p_prev) = NULL;
        goto Finish;
      }
    }
    pNext(p_prev) = p;
    p_prev = p;
    pIter(p);
  }

  Finish:
  pNext(q) = NULL;
  *r_p = pNext(&pp);
  *r_q = pNext(&qq);
  *lq = l;
#ifdef HAVE_ASSUME
  assume(pLength(*r_p) + pLength(*r_q) == lp);
#endif
  pTest(*r_p);
  pTest(*r_q);
}

#if 1
poly pTakeOutComp1(poly * p, int k)
{
  poly q = *p;

  if (q==NULL) return NULL;

  poly qq=NULL,result = NULL;

  if (pGetComp(q)==k)
  {
    result = q; /* *p */
    while ((q!=NULL) && (pGetComp(q)==k))
    {
      pSetComp(q,0);
      pSetmComp(q);
      qq = q;
      pIter(q);
    }
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
//  if (pGetComp(q) > k) pGetComp(q)--;
  while (pNext(q)!=NULL)
  {
    if (pGetComp(pNext(q))==k)
    {
      if (result==NULL)
      {
        result = pNext(q);
        qq = result;
      }
      else
      {
        pNext(qq) = pNext(q);
        pIter(qq);
      }
      pNext(q) = pNext(pNext(q));
      pNext(qq) =NULL;
      pSetComp(qq,0);
      pSetmComp(qq);
    }
    else
    {
      pIter(q);
//      if (pGetComp(q) > k) pGetComp(q)--;
    }
  }
  return result;
}
#endif

void pDeleteComp(poly * p,int k)
{
  poly q;

  while ((*p!=NULL) && (pGetComp(*p)==k)) pLmDelete(p);
  if (*p==NULL) return;
  q = *p;
  if (pGetComp(q)>k)
  {
    pSubComp(q,1);
    pSetmComp(q);
  }
  while (pNext(q)!=NULL)
  {
    if (pGetComp(pNext(q))==k)
      pLmDelete(&(pNext(q)));
    else
    {
      pIter(q);
      if (pGetComp(q)>k)
      {
        pSubComp(q,1);
        pSetmComp(q);
      }
    }
  }
}
/*----------end of utilities for syzygies--------------*/

/*2
* pair has no common factor ? or is no polynomial
*/
BOOLEAN pHasNotCF(poly p1, poly p2)
{

  if (pGetComp(p1) > 0 || pGetComp(p2) > 0)
    return FALSE;
  int i = pVariables;
  loop
  {
    if ((pGetExp(p1, i) > 0) && (pGetExp(p2, i) > 0))   return FALSE;
    i--;
    if (i == 0)                                         return TRUE;
  }
}

/*2
*divides p1 by its leading coefficient
*/
void pNorm(poly p1)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring(currRing))
  {
    Werror("pNorm not possible in the case of coefficient rings.");
  }
  else
#endif
  if (p1!=NULL)
  {
    if (pNext(p1)==NULL)
    {
      pSetCoeff(p1,nInit(1));
      return;
    }
    poly h;
    if (!nIsOne(pGetCoeff(p1)))
    {
      number k, c;
      nNormalize(pGetCoeff(p1));
      k = pGetCoeff(p1);
      c = nInit(1);
      pSetCoeff0(p1,c);
      h = pNext(p1);
      while (h!=NULL)
      {
        c=nDiv(pGetCoeff(h),k);
        // no need to normalize: Z/p, R
        // normalize already in nDiv: Q_a, Z/p_a
        // remains: Q
        if (rField_is_Q() && (!nIsOne(c))) nNormalize(c);
        pSetCoeff(h,c);
        pIter(h);
      }
      nDelete(&k);
    }
    else
    {
      if (nNormalize != nDummy2)
      {
        h = pNext(p1);
        while (h!=NULL)
        {
          nNormalize(pGetCoeff(h));
          pIter(h);
        }
      }
    }
  }
}

/*2
*normalize all coefficients
*/
void p_Normalize(poly p,const ring r)
{
  if (rField_has_simple_inverse(r)) return; /* Z/p, GF(p,n), R, long R/C */
  while (p!=NULL)
  {
#ifdef LDEBUG
    if (currRing==r) {nTest(pGetCoeff(p));}
#endif
    n_Normalize(pGetCoeff(p),r);
    pIter(p);
  }
}

// splits p into polys with Exp(n) == 0 and Exp(n) != 0
// Poly with Exp(n) != 0 is reversed
static void pSplitAndReversePoly(poly p, int n, poly *non_zero, poly *zero)
{
  if (p == NULL)
  {
    *non_zero = NULL;
    *zero = NULL;
    return;
  }
  spolyrec sz;
  poly z, n_z, next;
  z = &sz;
  n_z = NULL;

  while(p != NULL)
  {
    next = pNext(p);
    if (pGetExp(p, n) == 0)
    {
      pNext(z) = p;
      pIter(z);
    }
    else
    {
      pNext(p) = n_z;
      n_z = p;
    }
    p = next;
  }
  pNext(z) = NULL;
  *zero = pNext(&sz);
  *non_zero = n_z;
  return;
}

/*3
* substitute the n-th variable by 1 in p
* destroy p
*/
static poly pSubst1 (poly p,int n)
{
  poly qq=NULL, result = NULL;
  poly zero=NULL, non_zero=NULL;

  // reverse, so that add is likely to be linear
  pSplitAndReversePoly(p, n, &non_zero, &zero);

  while (non_zero != NULL)
  {
    assume(pGetExp(non_zero, n) != 0);
    qq = non_zero;
    pIter(non_zero);
    qq->next = NULL;
    pSetExp(qq,n,0);
    pSetm(qq);
    result = pAdd(result,qq);
  }
  p = pAdd(result, zero);
  pTest(p);
  return p;
}

/*3
* substitute the n-th variable by number e in p
* destroy p
*/
static poly pSubst2 (poly p,int n, number e)
{
  assume( ! nIsZero(e) );
  poly qq,result = NULL;
  number nn, nm;
  poly zero, non_zero;

  // reverse, so that add is likely to be linear
  pSplitAndReversePoly(p, n, &non_zero, &zero);

  while (non_zero != NULL)
  {
    assume(pGetExp(non_zero, n) != 0);
    qq = non_zero;
    pIter(non_zero);
    qq->next = NULL;
    nPower(e, pGetExp(qq, n), &nn);
    nm = nMult(nn, pGetCoeff(qq));
#ifdef HAVE_RINGS
    if (nIsZero(nm))
    {
      pLmFree(&qq);
      nDelete(&nm);
    }
    else
#endif
    {
      pSetCoeff(qq, nm);
      pSetExp(qq, n, 0);
      pSetm(qq);
      result = pAdd(result,qq);
    }
    nDelete(&nn);
  }
  p = pAdd(result, zero);
  pTest(p);
  return p;
}


/* delete monoms whose n-th exponent is different from zero */
poly pSubst0(poly p, int n)
{
  spolyrec res;
  poly h = &res;
  pNext(h) = p;

  while (pNext(h)!=NULL)
  {
    if (pGetExp(pNext(h),n)!=0)
    {
      pLmDelete(&pNext(h));
    }
    else
    {
      pIter(h);
    }
  }
  pTest(pNext(&res));
  return pNext(&res);
}

/*2
* substitute the n-th variable by e in p
* destroy p
*/
poly pSubst(poly p, int n, poly e)
{
  if (e == NULL) return pSubst0(p, n);

  if (pIsConstant(e))
  {
    if (nIsOne(pGetCoeff(e))) return pSubst1(p,n);
    else return pSubst2(p, n, pGetCoeff(e));
  }

#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing))
  {
    return nc_pSubst(p,n,e);
  }
#endif

  int exponent,i;
  poly h, res, m;
  int *me,*ee;
  number nu,nu1;

  me=(int *)omAlloc((pVariables+1)*sizeof(int));
  ee=(int *)omAlloc((pVariables+1)*sizeof(int));
  if (e!=NULL) pGetExpV(e,ee);
  res=NULL;
  h=p;
  while (h!=NULL)
  {
    if ((e!=NULL) || (pGetExp(h,n)==0))
    {
      m=pHead(h);
      pGetExpV(m,me);
      exponent=me[n];
      me[n]=0;
      for(i=pVariables;i>0;i--)
        me[i]+=exponent*ee[i];
      pSetExpV(m,me);
      if (e!=NULL)
      {
        nPower(pGetCoeff(e),exponent,&nu);
        nu1=nMult(pGetCoeff(m),nu);
        nDelete(&nu);
        pSetCoeff(m,nu1);
      }
      res=pAdd(res,m);
    }
    pLmDelete(&h);
  }
  omFreeSize((ADDRESS)me,(pVariables+1)*sizeof(int));
  omFreeSize((ADDRESS)ee,(pVariables+1)*sizeof(int));
  return res;
}

/* Returns TRUE if
     * LM(p) | LM(lcm)
     * LC(p) | LC(lcm) only if ring
     * Exists i, j:
         * LE(p, i)  != LE(lcm, i)
         * LE(p1, i) != LE(lcm, i)   ==> LCM(p1, p) != lcm
         * LE(p, j)  != LE(lcm, j)
         * LE(p2, j) != LE(lcm, j)   ==> LCM(p2, p) != lcm
*/
BOOLEAN pCompareChain (poly p,poly p1,poly p2,poly lcm)
{
  int k, j;

  if (lcm==NULL) return FALSE;

  for (j=pVariables; j; j--)
    if ( pGetExp(p,j) >  pGetExp(lcm,j)) return FALSE;
  if ( pGetComp(p) !=  pGetComp(lcm)) return FALSE;
  for (j=pVariables; j; j--)
  {
    if (pGetExp(p1,j)!=pGetExp(lcm,j))
    {
      if (pGetExp(p,j)!=pGetExp(lcm,j))
      {
        for (k=pVariables; k>j; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p2,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        for (k=j-1; k; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p2,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        return FALSE;
      }
    }
    else if (pGetExp(p2,j)!=pGetExp(lcm,j))
    {
      if (pGetExp(p,j)!=pGetExp(lcm,j))
      {
        for (k=pVariables; k>j; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p1,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        for (k=j-1; k!=0 ; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p1,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        return FALSE;
      }
    }
  }
  return FALSE;
}
#ifdef HAVE_RATGRING
BOOLEAN pCompareChainPart (poly p,poly p1,poly p2,poly lcm)
{
  int k, j;

  if (lcm==NULL) return FALSE;

  for (j=currRing->real_var_end; j>=currRing->real_var_start; j--)
    if ( pGetExp(p,j) >  pGetExp(lcm,j)) return FALSE;
  if ( pGetComp(p) !=  pGetComp(lcm)) return FALSE;
  for (j=currRing->real_var_end; j>=currRing->real_var_start; j--)
  {
    if (pGetExp(p1,j)!=pGetExp(lcm,j))
    {
      if (pGetExp(p,j)!=pGetExp(lcm,j))
      {
        for (k=pVariables; k>j; k--)
        for (k=currRing->real_var_end; k>j; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p2,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        for (k=j-1; k>=currRing->real_var_start; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p2,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        return FALSE;
      }
    }
    else if (pGetExp(p2,j)!=pGetExp(lcm,j))
    {
      if (pGetExp(p,j)!=pGetExp(lcm,j))
      {
        for (k=currRing->real_var_end; k>j; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p1,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        for (k=j-1; k>=currRing->real_var_start; k--)
        {
          if ((pGetExp(p,k)!=pGetExp(lcm,k))
          && (pGetExp(p1,k)!=pGetExp(lcm,k)))
            return TRUE;
        }
        return FALSE;
      }
    }
  }
  return FALSE;
}
#endif

int pSize(poly p)
{
  int count = 0;
  while ( p != NULL )
  {
    count+= nSize( pGetCoeff( p ) );
    pIter( p );
  }
  return count;
}

/*2
* returns the length of a (numbers of monomials)
* respect syzComp
*/
poly pLast(poly a, int &l)
{
  if (a == NULL)
  {
    l = 0;
    return NULL;
  }
  l = 1;
  if (! rIsSyzIndexRing(currRing))
  {
    while (pNext(a)!=NULL)
    {
      pIter(a);
      l++;
    }
  }
  else
  {
    int curr_limit = rGetCurrSyzLimit(currRing);
    poly pp = a;
    while ((a=pNext(a))!=NULL)
    {
      if (pGetComp(a)<=curr_limit/*syzComp*/)
        l++;
      else break;
      pp = a;
    }
    a=pp;
  }
  return a;
}

