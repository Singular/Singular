// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: accces single (coeffs of) monoms
*/

#include "misc/auxiliary.h"

#include "coeffs/coeffs.h"

#include "coeffs/longrat.h" // snumber is necessary

#include "polys/monomials/p_polys.h"
#include "polys/pCoeff.h"

#include "simpleideals.h"

/// find coeff of (polynomial) m in polynomial p
/// find coeff of (vector) m in vector p
number p_CoeffTerm(poly p, poly m, const ring r)
{
  if (m!=NULL)
  {
    while (p!=NULL)
    {
      if (p_LmCmp(p,m,r)==0)
      {
        return n_Copy(p_GetCoeff(p,r),r->cf);
      }
      pIter(p);
    }
  }
  return n_Init(0,r->cf);
}

/// find vector of coeffs of (polynomial) m in vector v
poly p_CoeffTermV(poly v, poly m, const ring r)
{
  poly res=NULL;
  if (m!=NULL)
  {
    while (v!=NULL)
    {
      p_SetComp(m,p_GetComp(v,r),r);
      if (p_LmCmp(v,m,r)==0)
      {
        p_SetComp(m,0,r);
        poly p=p_Init(r);
        p_SetCoeff0(p,p_GetCoeff(v,r),r);
        p_SetComp(p,p_GetComp(v,r),r);
        res=p_Add_q(res,p,r);
      }
      pIter(v);
    }
  }
  return res;
}

/// find coeffs of (polynomial) m in all polynomials from I
/// find coeffs of (vector) m in all vectors from I
ideal id_CoeffTerm(ideal I, poly m, const ring r)
{
  ideal res=idInit(IDELEMS(I),I->rank);
  for(int i=IDELEMS(I)-1;i>=0;i--)
  {
    number n=p_CoeffTerm(I->m[i],m,r);
    res->m[i]=p_NSet(n,r);
  }
  return res;
}

/// find coeffs of (polynomial) m in all vectors from I
ideal id_CoeffTermV(ideal M, poly m, const ring r)
{
  ideal res=idInit(IDELEMS(M),M->rank);
  for(int i=IDELEMS(M)-1;i>=0;i--)
  {
    res->m[i]=p_CoeffTermV(M->m[i],m,r);
  }
  return res;
}

/// find coeffs of a vector of a list of given monomials, n>=max_comp(v)
poly p_CoeffTermId(poly v, ideal m, int n, const ring r)
{
  if ((n<=0)||(v==NULL)) return NULL;
  poly q;
  poly u=NULL;
  int dummy;
  const int ncols_m=IDELEMS(m);
  v=p_Copy(v,r);
  for(int i=1;i<=n;i++)
  {
    p_TakeOutComp(&v,i,&q,&dummy,r);
    for(int j=0;j<ncols_m;j++)
    {
      number n=p_CoeffTerm(q,m->m[j],r);
      poly uu=p_NSet(n,r);
      if (uu!=NULL)
      {
        p_SetComp(uu,(i-1)*ncols_m+j+1,r);
        u=p_Add_q(u,uu,r);
      }
    }
    if (v==NULL) break;
  }
  return u;
}

/// find coeffs of a vector of a matrix(module) of given monomials, n>=max_comp(v)
poly p_CoeffTermMo(poly v, ideal m, int n, const ring r)
{
  poly res=NULL;
  for(int i=0;i<IDELEMS(m);i++)
  {
    ideal mm=id_Vec2Ideal(m->m[i],r);
    poly tmp=p_CoeffTermId(v,mm,n,r);
    if (i>0)
    {
      p_Shift(&tmp,m->rank,r); // m->rank >= p_MaxComp(m->m[i-1],r)
      res=p_Add_q(res,tmp,r);
    }
    else
      res=tmp;
  }
  return res;
}
