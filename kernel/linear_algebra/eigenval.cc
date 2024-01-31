/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: eigenvalues of constant square matrices
*/




#include "kernel/mod2.h"

#ifdef HAVE_EIGENVAL

#include "kernel/structs.h"
#include "misc/intvec.h"
#include "coeffs/numbers.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "polys/matpol.h"
#include "polys/clapsing.h"
#include "kernel/linear_algebra/eigenval.h"


matrix evSwap(matrix M,int i,int j)
{
  if(i==j)
    return(M);

  for(int k=1;k<=MATROWS(M);k++)
  {
    poly p=MATELEM(M,i,k);
    MATELEM(M,i,k)=MATELEM(M,j,k);
    MATELEM(M,j,k)=p;
  }

  for(int k=1;k<=MATCOLS(M);k++)
  {
    poly p=MATELEM(M,k,i);
    MATELEM(M,k,i)=MATELEM(M,k,j);
    MATELEM(M,k,j)=p;
  }

  return(M);
}

matrix evRowElim(matrix M,int i,int j,int k)
{
  if(MATELEM(M,i,k)==NULL||MATELEM(M,j,k)==NULL)
    return(M);
  poly p1=pp_Jet0(MATELEM(M,i,k),currRing);
  poly p2=pp_Jet0(MATELEM(M,j,k),currRing);
  if ((p1==NULL)||(p2==NULL)) return (M);

  poly p=pNSet(nDiv(pGetCoeff(p1),pGetCoeff(p2)));
  pNormalize(p);

  for(int l=1;l<=MATCOLS(M);l++)
  {
    MATELEM(M,i,l)=pSub(MATELEM(M,i,l),ppMult_qq(p,MATELEM(M,j,l)));
    pNormalize(MATELEM(M,i,l));
  }
  for(int l=1;l<=MATROWS(M);l++)
  {
    MATELEM(M,l,j)=pAdd(MATELEM(M,l,j),ppMult_qq(p,MATELEM(M,l,i)));
    pNormalize(MATELEM(M,l,j));
  }

  pDelete(&p);
  pDelete(&p1);
  pDelete(&p2);

  return(M);
}

matrix evColElim(matrix M,int i,int j,int k)
{
  if(MATELEM(M,k,i)==0||MATELEM(M,k,j)==0)
    return(M);

  poly p=pNSet(nDiv(pGetCoeff(MATELEM(M,k,i)),pGetCoeff(MATELEM(M,k,j))));
  pNormalize(p);

  for(int l=1;l<=MATROWS(M);l++)
  {
    MATELEM(M,l,i)=pSub(MATELEM(M,l,i),ppMult_qq(p,MATELEM(M,l,j)));
    pNormalize(MATELEM(M,l,i));
  }
  for(int l=1;l<=MATCOLS(M);l++)
  {
    MATELEM(M,j,l)=pAdd(MATELEM(M,j,l),ppMult_qq(p,MATELEM(M,i,l)));
    pNormalize(MATELEM(M,j,l));
  }

  pDelete(&p);

  return(M);
}

matrix evHessenberg(matrix M)
{
  int n=MATROWS(M);
  if(n!=MATCOLS(M))
    return(M);

  for(int k=1,j=2;k<n-1;k++,j=k+1)
  {
    while((j<=n)
    &&((MATELEM(M,j,k)==NULL)
      || (p_Totaldegree(MATELEM(M,j,k),currRing)!=0)))
      j++;

    if(j<=n)
    {
      M=evSwap(M,j,k+1);

      for(int i=j+1;i<=n;i++)
        M=evRowElim(M,i,k+1,k);
    }
  }

  return(M);
}

#endif /* HAVE_EIGENVAL */
