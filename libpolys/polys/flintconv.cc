// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/



#include "misc/auxiliary.h"
#include "flintconv.h"

#ifdef HAVE_FLINT
#if __FLINT_RELEASE >= 20500

#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "coeffs/rintegers.h"
#include "polys/monomials/p_polys.h"

#include "polys/sbuckets.h"
//#include "polys/clapconv.h"

#include "simpleideals.h"


int convFlintISingI (fmpz_t f)
{
  //return fmpz_get_si(f);
  return (int)*f;
}

void convSingIFlintI(fmpz_t f, int p)
{
  fmpz_init(f);
  *f=p;
  //fmpz_set_si(f,p);
  return;
}

void convFlintNSingN (mpz_t z, fmpz_t f)
{
  mpz_init(z);
  fmpz_get_mpz(z,f);
}

number convFlintNSingN (fmpz_t f)
{
#if __FLINT_RELEASE > 20502
  number z;
  if(COEFF_IS_MPZ(*f))
    nlMPZ(COEFF_TO_PTR(*f),z,NULL);
  else
  {
    mpz_t a;
    mpz_init(a);
    fmpz_get_mpz(a,f);
    nlMPZ(a,z,NULL);
    mpz_clear(a);
  }
  return z;
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

number convFlintNSingN (fmpq_t f, const coeffs cf)
{
#if __FLINT_RELEASE > 20502
  if (getCoeffType(cf)==n_Q) /* QQ, bigint */
  {
    return convFlintNSingN_QQ(f,cf);
  }
  else
  {
    number z;
    mpz_t a,b;
    mpz_init(a);
    mpz_init(b);
    fmpq_get_mpz_frac(a,b,f);
    if (mpz_cmp_si(b,1L)!=0)
    {
      number na=n_InitMPZ(a,cf);
      number nb=n_InitMPZ(b,cf);
      z=n_Div(na,nb,cf);
      n_Delete(&nb,cf);
      n_Delete(&na,cf);
      n_Normalize(z,cf);
    }
    else
    {
      z=n_InitMPZ(a,cf);
    }
    mpz_clear(a);
    mpz_clear(b);
    n_Test(z,cf);
    return z;
  }
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

number convFlintNSingN (fmpz_t f, const coeffs cf)
{
#if __FLINT_RELEASE > 20502
  number z;
  if(COEFF_IS_MPZ(*f))
    z=n_InitMPZ(COEFF_TO_PTR(*f),cf);
  else if (cf->rep==n_rep_gmp)
  {
    z=nrzInit(1,NULL); // alloc and initialization
    fmpz_get_mpz((mpz_ptr)z,f);
  }
  else
  {
    mpz_t a;
    mpz_init(a);
    fmpz_get_mpz(a,f);
    z=n_InitMPZ(a,cf);
    mpz_clear(a);
  }
  return z;
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

number convFlintNSingN_QQ (fmpq_t f, const coeffs cf)
{
#if __FLINT_RELEASE > 20502
  if (fmpz_is_one(fmpq_denref(f)))
  {
    if (fmpz_fits_si(fmpq_numref(f)))
    {
      long i=fmpz_get_si(fmpq_numref(f));
      return n_Init(i,cf);
    }
  }
  number z=ALLOC_RNUMBER();
  #if defined(LDEBUG)
  z->debug=123456;
  #endif
  mpz_init(z->z);
  if (fmpz_is_one(fmpq_denref(f)))
  {
    z->s=3;
    fmpz_get_mpz(z->z,fmpq_numref(f));
  }
  else
  {
    z->s=0;
    mpz_init(z->n);
    fmpq_get_mpz_frac(z->z,z->n,f);
  }
  return z;
#else
  WerrorS("not implemented");
  return NULL;
#endif
}

void convSingNFlintN(fmpz_t f, mpz_t n)
{
  fmpz_init(f);
  fmpz_set_mpz(f,n);
}

void convSingNFlintN(fmpz_t f, number n)
{
  fmpz_init(f);
  fmpz_set_mpz(f,(mpz_ptr)n);
}

void convSingNFlintN(fmpq_t f, number n, const coeffs cf)
{
  if (LIKELY(getCoeffType(cf)==n_Q)) /* QQ, bigint */
  {
    fmpq_init(f);
    if (SR_HDL(n)&SR_INT)
      fmpq_set_si(f,SR_TO_INT(n),1);
    else if (n->s<3)
    {
      fmpz_set_mpz(fmpq_numref(f), n->z);
      fmpz_set_mpz(fmpq_denref(f), n->n);
    }
    else
    {
      mpz_t one;
      mpz_init_set_si(one,1);
      fmpz_set_mpz(fmpq_numref(f), n->z);
      fmpz_set_mpz(fmpq_denref(f), one);
      mpz_clear(one);
    }
  }
  else
  {
    coeffs QQ=nInitChar(n_Q,NULL);
    nMapFunc nMap=n_SetMap(cf,QQ);
    if (nMap!=NULL)
    {
      number nn=nMap(n,cf,QQ);
      convSingNFlintN(f,nn,QQ);
    }
    nKillChar(QQ);
  }
}

void convSingNFlintN_QQ(fmpq_t f, number n)
{
  fmpq_init(f);
  if (SR_HDL(n)&SR_INT)
    fmpq_set_si(f,SR_TO_INT(n),1);
  else if (n->s<3)
  {
    fmpz_set_mpz(fmpq_numref(f), n->z);
    fmpz_set_mpz(fmpq_denref(f), n->n);
  }
  else
  {
    mpz_t one;
    mpz_init_set_si(one,1);
    fmpz_set_mpz(fmpq_numref(f), n->z);
    fmpz_set_mpz(fmpq_denref(f), one);
    mpz_clear(one);
  }
}

void convSingNFlintNN(fmpq_t re, fmpq_t im, number n, const coeffs cf)
{
  number n_2=n_RePart(n,cf);
  convSingNFlintN(re,n_2,cf);
  n_Delete(&n_2,cf);
  n_2=n_ImPart(n,cf);
  convSingNFlintN(im,n_2,cf);
  n_Delete(&n_2,cf);
}

void convSingPFlintP(fmpq_poly_t res, poly p, const ring r)
{
  if (p==NULL)
  {
    fmpq_poly_init(res);
    return;
  }
  int d=p_GetExp(p,1,r);
  fmpq_poly_init2(res,d+1);
  _fmpq_poly_set_length (res, d + 1);
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpq_t c;
    convSingNFlintN(c,n,r->cf);
    fmpq_poly_set_coeff_fmpq(res,p_GetExp(p,1,r),c);
    fmpq_clear(c);
    pIter(p);
  }
}

void convSingImPFlintP(fmpq_poly_t res, poly p, const ring r)
{
  int d=p_GetExp(p,1,r);
  fmpq_poly_init2(res,d+1);
  _fmpq_poly_set_length (res, d + 1);
  while(p!=NULL)
  {
    number n=n_ImPart(pGetCoeff(p),r->cf);
    fmpq_t c;
    convSingNFlintN(c,n,r->cf);
    fmpq_poly_set_coeff_fmpq(res,p_GetExp(p,1,r),c);
    fmpq_clear(c);
    n_Delete(&n,r->cf);
    pIter(p);
  }
}

poly convFlintPSingP(fmpq_poly_t f, const ring r)
{
  if (fmpq_poly_is_zero(f)) return NULL;
  int d=fmpq_poly_length(f);
  poly p=NULL;
  fmpq_t c;
  fmpq_init(c);
  for(int i=0; i<=d; i++)
  {
    fmpq_poly_get_coeff_fmpq(c,f,i);
    number n=convFlintNSingN(c,r->cf);
    if(!n_IsZero(n,r->cf))
    {
      poly pp=p_Init(r);
      pSetCoeff0(pp,n);
      p_SetExp(pp,1,i,r);
      p_Setm(pp,r);
      p=p_Add_q(p,pp,r);
    }
  }
  fmpq_clear(c);
  p_Test(p,r);
  return p;
}

void convSingPFlintnmod_poly_t(nmod_poly_t result, const poly p, const ring r)
{
  // assume univariate, r->cf=Z/p
  nmod_poly_init2 (result,rChar(r),p_Deg(p,r));
  poly h=p;
  while(h!=NULL)
  {
    if (h==NULL)
      nmod_poly_set_coeff_ui(result,0,0);
    else
      nmod_poly_set_coeff_ui(result,p_GetExp(h,1,r),n_Int(pGetCoeff(h),r->cf)+rChar(r));
    pIter(h);
  }
}

void convSingMFlintFq_nmod_mat(matrix m, fq_nmod_mat_t M, const fq_nmod_ctx_t fq_con, const ring r)
{
  fq_nmod_mat_init (M, (long)MATROWS(m), (long) MATCOLS(m), fq_con);
  int i,j;
  for(i=MATROWS(m);i>0;i--)
  {
    for(j=MATCOLS(m);j>0;j--)
    {
      convSingPFlintnmod_poly_t (M->rows[i-1]+j-1, MATELEM(m,i,j),r);
    }
  }
}

poly convFlintFq_nmodSingP(const fq_nmod_t Fp, const fq_nmod_ctx_t ctx, const ring r)
{
  poly p=NULL;
  poly h;
  for (int i= 0; i < nmod_poly_length (Fp); i++)
  {
    ulong coeff= nmod_poly_get_coeff_ui (Fp, i);
    if (coeff != 0)
    h=p_NSet(n_Init(coeff,r->cf),r);
    if (h!=NULL)
    {
      p_SetExp(h,1,i,r);
      p_Setm(h,r);
      p=p_Add_q(p,h,r);
    }
  }
  return p;
}

matrix convFlintFq_nmod_matSingM(fq_nmod_mat_t m, const fq_nmod_ctx_t fq_con, const ring r)
{
  matrix M=mpNew(fq_nmod_mat_nrows (m, fq_con),fq_nmod_mat_ncols (m, fq_con));
   int i,j;
  for(i=MATROWS(M);i>0;i--)
  {
    for(j=MATCOLS(M);j>0;j--)
    {
      MATELEM(M,i,j)=convFlintFq_nmodSingP(fq_nmod_mat_entry (m, i-1, j-1),
                                          fq_con, r);
    }
  }
  return M;
}

void convSingMFlintNmod_mat(matrix m, nmod_mat_t M, const ring r)
{
  nmod_mat_init (M, (long)MATROWS(m), (long) MATCOLS(m), rChar(r));
  int i,j;
  for(i=MATROWS(m);i>0;i--)
  {
    for(j=MATCOLS(m);j>0;j--)
    {
      poly h=MATELEM(m,i,j);
      if (h!=NULL)
        nmod_mat_entry(M,i-1,j-1)=(long)pGetCoeff(h);
    }
  }
}

matrix convFlintNmod_matSingM(nmod_mat_t m, const ring r)
{
  matrix M=mpNew(nmod_mat_nrows (m),nmod_mat_ncols (m));
   int i,j;
  for(i=MATROWS(M);i>0;i--)
  {
    for(j=MATCOLS(M);j>0;j--)
    {
      MATELEM(M,i,j)=p_ISet(nmod_mat_entry (m, i-1, j-1),r);
    }
  }
  return M;
}

matrix singflint_rref(matrix  m, const ring R)
{
  int r=m->rows();
  int c=m->cols();
  int i,j;
  matrix M=NULL;
  if (rField_is_Q(R))
  {
    fmpq_mat_t FLINTM;
    fmpq_mat_init(FLINTM,r,c);
    M=mpNew(r,c);
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        poly h=MATELEM(m,i,j);
        if (h!=NULL)
        {
          if (p_Totaldegree(h,R)==0)
            convSingNFlintN(fmpq_mat_entry(FLINTM,i-1,j-1),pGetCoeff(h),R->cf);
          else
          {
            WerrorS("matrix for rref is not constant");
            return M;
          }
        }
      }
    }
    fmpq_mat_rref(FLINTM,FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        number n=convFlintNSingN(fmpq_mat_entry(FLINTM,i-1,j-1),R->cf);
        MATELEM(M,i,j)=p_NSet(n,R);
      }
    }
    fmpq_mat_clear(FLINTM);
  }
  else if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    // convert matrix
    convSingMFlintNmod_mat(m,FLINTM,R);
    // rank
    long rk= nmod_mat_rref (FLINTM);
    M=convFlintNmod_matSingM(FLINTM,R);
    // clean up
    nmod_mat_clear(FLINTM);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

ideal singflint_rref(ideal  m, const ring R) /*assume smatrix m*/
{
  int r=m->rank;
  int c=m->ncols;
  int i,j;
  ideal M=idInit(c,r);
  if (rField_is_Q(R))
  {
    fmpq_mat_t FLINTM;
    fmpq_mat_init(FLINTM,r,c);
    for(j=c-1;j>=0;j--)
    {
      poly h=m->m[j];
      while(h!=NULL)
      {
        i=p_GetComp(h,R);
        if (p_Totaldegree(h,R)==0)
          convSingNFlintN(fmpq_mat_entry(FLINTM,i-1,j),p_GetCoeff(h,R),R->cf);
        else
        {
          WerrorS("smatrix for rref is not constant");
          return M;
        }
        pIter(h);
      }
    }
    fmpq_mat_rref(FLINTM,FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c-1;j>=0;j--)
      {
        number n=convFlintNSingN(fmpq_mat_entry(FLINTM,i-1,j),R->cf);
        if(!n_IsZero(n,R->cf))
        {
          poly p=p_NSet(n,R);
          p_SetComp(p,i,R);
          M->m[j]=p_Add_q(M->m[j],p,R);
        }
      }
    }
    fmpq_mat_clear(FLINTM);
  }
  else if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    nmod_mat_init(FLINTM,r,c,rChar(R));
    for(j=c-1;j>=0;j--)
    {
      poly h=m->m[j];
      while(h!=NULL)
      {
        i=p_GetComp(h,R);
        if (p_Totaldegree(h,R)==0)
          nmod_mat_entry(FLINTM,i-1,j)=(long)p_GetCoeff(h,R);
        else
        {
          WerrorS("smatrix for rref is not constant");
          return M;
        }
        pIter(h);
      }
    }
    nmod_mat_rref(FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c-1;j>=0;j--)
      {
        number n=n_Init(nmod_mat_entry(FLINTM,i-1,j),R->cf);
        if(!n_IsZero(n,R->cf))
        {
          poly p=p_NSet(n,R);
          p_SetComp(p,i,R);
          M->m[j]=p_Add_q(M->m[j],p,R);
        }
      }
    }
    nmod_mat_clear(FLINTM);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

matrix singflint_kernel(matrix  m, const ring R)
{
  matrix M=NULL;
  if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    nmod_mat_t FLINTX;
    nmod_mat_init (FLINTX, (long)MATROWS(m), (long) MATCOLS(m), rChar(R));
    // convert matrix
    convSingMFlintNmod_mat(m,FLINTM,R);
    // rank
    long rk= nmod_mat_nullspace(FLINTX,FLINTM);
    nmod_mat_clear(FLINTM);
    M=convFlintNmod_matSingM(FLINTX,R);
    // clean up
    nmod_mat_clear(FLINTX);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

ideal singflint_kernel(ideal  m, const ring R) /*assume smatrix m*/
{
  int r=m->rank;
  int c=m->ncols;
  int i,j;
  ideal M=idInit(c,r);
  if (rField_is_Zp(R))
  {
    nmod_mat_t FLINTM;
    nmod_mat_t FLINTX;
    nmod_mat_init(FLINTM,r,c,rChar(R));
    nmod_mat_init(FLINTX,r,c,rChar(R));
    for(j=c-1;j>=0;j--)
    {
      poly h=m->m[j];
      while(h!=NULL)
      {
        i=p_GetComp(h,R);
        if (p_Totaldegree(h,R)==0)
          nmod_mat_entry(FLINTM,i-1,j)=(long)p_GetCoeff(h,R);
        else
        {
          WerrorS("smatrix for rref is not constant");
          return M;
        }
        pIter(h);
      }
    }
    nmod_mat_nullspace(FLINTX,FLINTM);
    nmod_mat_clear(FLINTM);
    for(i=r;i>0;i--)
    {
      for(j=c-1;j>=0;j--)
      {
        number n=n_Init(nmod_mat_entry(FLINTX,i-1,j),R->cf);
        if(!n_IsZero(n,R->cf))
        {
          poly p=p_NSet(n,R);
          p_SetComp(p,i,R);
          M->m[j]=p_Add_q(M->m[j],p,R);
        }
      }
    }
    nmod_mat_clear(FLINTX);
  }
  else
  {
    WerrorS("not implemented for these coefficients");
  }
  return M;
}

bigintmat* singflint_LLL(bigintmat*  m, bigintmat* T)
{
  int r=m->rows();
  int c=m->cols();
  bigintmat* res=new bigintmat(r,c,m->basecoeffs());
  fmpz_mat_t M, Transf;
  fmpz_mat_init(M, r, c);
  if(T != NULL)
  {
    fmpz_mat_init(Transf, T->rows(), T->rows());
  }
  fmpz_t dummy;
  mpz_t n;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      n_MPZ(n, BIMATELEM(*m, i, j),m->basecoeffs());
      convSingNFlintN(dummy,n);
      mpz_clear(n);
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
      fmpz_clear(dummy);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->rows();j>0;j--)
      {
        n_MPZ(n, BIMATELEM(*T, i, j),T->basecoeffs());
        convSingNFlintN(dummy,n);
        mpz_clear(n);
        fmpz_set(fmpz_mat_entry(Transf, i-1, j-1), dummy);
        fmpz_clear(dummy);
      }
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  if(T != NULL)
    fmpz_lll(M, Transf, fl);
  else
    fmpz_lll(M, NULL, fl);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convFlintNSingN(n, fmpz_mat_entry(M, i-1, j-1));
      n_Delete(&(BIMATELEM(*res,i,j)),res->basecoeffs());
      BIMATELEM(*res,i,j)=n_InitMPZ(n,res->basecoeffs());
      mpz_clear(n);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->cols();j>0;j--)
      {
        convFlintNSingN(n, fmpz_mat_entry(Transf, i-1, j-1));
        n_Delete(&(BIMATELEM(*T,i,j)),T->basecoeffs());
        BIMATELEM(*T,i,j)=n_InitMPZ(n,T->basecoeffs());
        mpz_clear(n);
      }
    }
  }
  return res;
}

intvec* singflint_LLL(intvec*  m, intvec* T)
{
  int r=m->rows();
  int c=m->cols();
  intvec* res = new intvec(r,c,(int)0);
  fmpz_mat_t M,Transf;
  fmpz_mat_init(M, r, c);
  if(T != NULL)
    fmpz_mat_init(Transf, r, r);
  fmpz_t dummy;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convSingIFlintI(dummy,IMATELEM(*m,i,j));
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
      fmpz_clear(dummy);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->rows();j>0;j--)
      {
        convSingIFlintI(dummy,IMATELEM(*T,i,j));
        fmpz_set(fmpz_mat_entry(Transf, i-1, j-1), dummy);
        fmpz_clear(dummy);
      }
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  if(T != NULL)
    fmpz_lll(M, Transf, fl);
  else
    fmpz_lll(M, NULL, fl);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      IMATELEM(*res,i,j)=convFlintISingI(fmpz_mat_entry(M, i-1, j-1));
    }
  }
  if(T != NULL)
  {
    for(i=Transf->r;i>0;i--)
    {
      for(j=Transf->r;j>0;j--)
      {
        IMATELEM(*T,i,j)=convFlintISingI(fmpz_mat_entry(Transf, i-1, j-1));
      }
    }
  }
  return res;
}
#endif
#endif
