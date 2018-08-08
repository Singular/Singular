/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    find_perm.cc
 *  Purpose: is a map a permuation ?
 *  Author:  hannes
 *  Created: 16/01
 *******************************************************************/



#include "kernel/mod2.h"
#include "polys/monomials/p_polys.h"
#include "kernel/ideals.h"
#include "polys/monomials/ring.h"
#include "kernel/maps/find_perm.h"

static int* find_perm_for_map(const ring preimage_r, const ring image_r, const ideal image)
{
  int i;
  int *perm=(int *)omAlloc0((preimage_r->N+1)*sizeof(int));
  for (i=si_min(IDELEMS(image),preimage_r->N)-1; i>=0; i--)
  {
    if (image->m[i]!=NULL)
    {
      if((pNext(image->m[i])==NULL)
      && (n_IsOne(pGetCoeff(image->m[i]),image_r->cf)))
      {
        int v=p_IsUnivariate(image->m[i],image_r);
        if (v<=0) /*not univariate */
        {
          omFreeSize(perm,(preimage_r->N+1)*sizeof(int));
          return NULL;
        }
        else if (v>0) /* image is univaritate */
        {
          if (p_GetExp(image->m[i],v,image_r)==1)
          {
            perm[i+1]=v; /* and of exp 1 */
          }
          else
          {
            omFreeSize(perm,(preimage_r->N+1)*sizeof(int));
            return NULL;
          }
        }
      }
      else /* image is not a monomial */
      {
        omFreeSize(perm,(preimage_r->N+1)*sizeof(int));
        return NULL;
      }
    }
  }
  //Print("elms:%d, N:%d\n",IDELEMS(image),preimage_r->N);
  //iiWriteMatrix((matrix)image,"_",1,image_r,0);
  //PrintS("\npreimage:\n");rWrite(preimage_r);
  //PrintS("image:\n");rWrite(image_r);
  //PrintS("\nperm:");
  //for (i=1; i<=preimage_r->N; i++)
  //{
  //  Print(" %d",perm[i]);
  //}
  //PrintLn();
  return perm;
}

matrix ma_ApplyPermForMap(const matrix to_map, const ring preimage_r,
         const ideal image, const ring image_r, const nMapFunc nMap)
{
  if ((rPar(preimage_r)>0)||(rPar(image_r)>0)) return NULL; /* not applicable */
  int *perm=find_perm_for_map(preimage_r,image_r,image);
  if (perm==NULL) return NULL; /* could not find permutation */
  int C=to_map->cols();
  int R=to_map->rows();
  matrix m=mpNew(R,C);
  for (int i=R*C-1;i>=0;i--)
  {
    if (to_map->m[i]!=NULL)
    {
      m->m[i]=p_PermPoly(to_map->m[i],perm,preimage_r,image_r, nMap,NULL,0);
      p_Test(m->m[i],image_r);
    }
  }
  ideal ii=(ideal)m;
  ii->rank=((ideal)to_map)->rank;
  omFreeSize(perm,(preimage_r->N+1)*sizeof(int));
  return m;
}

