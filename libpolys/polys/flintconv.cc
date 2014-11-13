// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/





#include <misc/auxiliary.h>
#include "flintconv.h"
#include <coeffs/coeffs.h>
#include <polys/monomials/p_polys.h>

#include <omalloc/omalloc.h>
#include <coeffs/longrat.h>
#include <coeffs/modulop.h>
#include <polys/sbuckets.h>
#include <polys/clapconv.h>

#include "simpleideals.h"

#ifdef HAVE_FLINT
#ifdef FLINT_VER_2_4_5
int convFlintISingI (fmpz_t f)
{
  int res;
  res = fmpz_get_si(f);
  return res;
}

void convSingIFlintI(fmpz_t f, int p)
{
  fmpz_set_si(f,p);
  return;
}

void convFlintNSingN (mpz_t z, fmpz_t f)
{
  fmpz_get_mpz(z,f);
}

void convSingNFlintN(fmpz_t f, mpz_t z)
{
  fmpz_set_mpz(f,z);
}


bigintmat* singflint_LLL(bigintmat*  m)
{
  int r=m->rows();
  int c=m->cols();
  bigintmat* res=new bigintmat(r,c,m->basecoeffs());
  fmpz_mat_t M;
  fmpz_mat_init(M, r, c);
  fmpz_t dummy;
  mpz_t n;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      n_MPZ(n, BIMATELEM(*m, i, j),m->basecoeffs());
      convSingNFlintN(dummy,n);
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  //printf("\nBefore LLL (in Flint)\n");fmpz_mat_print(M);
  fmpz_lll(M, NULL, fl);
  //printf("\nAfter LLL (in Flint)\n");fmpz_mat_print(M);printf("\n\n");
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      //printf("\nThis is here:\n");fmpz_print(fmpz_mat_entry(M, i-1, j-1));
      convFlintNSingN(n, fmpz_mat_entry(M, i-1, j-1));
      BIMATELEM(*res,i,j)=n_InitMPZ(n,m->basecoeffs());
      //printf("\nThis is res[%i] = %i\n", i+j, *res[i+j]);
    }
  }
  return res;
}

intvec* singflint_LLL(intvec*  m)
{
  int r=m->rows();
  int c=m->cols();
  intvec* res = new intvec(r,c,(int)0);
  fmpz_mat_t M;
  fmpz_mat_init(M, r, c);
  fmpz_t dummy;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convSingIFlintI(dummy,IMATELEM(*m,i,j));
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  //printf("\nBefore LLL (in Flint)\n");fmpz_mat_print(M);
  fmpz_lll(M, NULL, fl);
  //printf("\nAfter LLL (in Flint)\n");fmpz_mat_print(M);printf("\n\n");
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      //printf("\nThis is here:\n");fmpz_print(fmpz_mat_entry(M, i-1, j-1));
      IMATELEM(*res,i,j)=convFlintISingI(fmpz_mat_entry(M, i-1, j-1));
      //printf("\nThis is res[%i] = %i\n", i+j, *res[i+j]);
    }
  }
  return res;
}
#endif
#endif
