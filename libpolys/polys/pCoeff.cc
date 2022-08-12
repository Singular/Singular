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
