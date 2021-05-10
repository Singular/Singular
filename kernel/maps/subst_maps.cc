#include "kernel/mod2.h"
#include "polys/monomials/p_polys.h"
#include "kernel/ideals.h"
#include "polys/monomials/ring.h"
#include "polys/monomials/maps.h"
#include "kernel/maps/gen_maps.h"

static poly p_SubstMon(poly p, int var, poly image, const ring preimage_r, const ring image_r, const nMapFunc nMap, matrix cache)
{
  assume(!rIsNCRing(image_r));
  poly q=p_NSet(nMap(pGetCoeff(p),preimage_r->cf,image_r->cf),image_r);
  int i;
  poly h=NULL;
  for(i=1;i<=preimage_r->N; i++)
  {
    int pExp=p_GetExp( p,i,preimage_r);
    if (i!=var)
    {
      p_SetExp(q,i,pExp,image_r);
    }
    else if (pExp!=0)
    {
      h=maEvalVariable(image,var,pExp,(ideal)cache,image_r);
      if (h==NULL)
      {
        p_LmDelete(q,image_r);
        return NULL;
      }
    }
    else
      h=p_One(image_r);
  }
  p_Setm(q,image_r);
  h=p_Mult_mm(h,q,image_r);
  p_LmDelete(q,image_r);
  return h;
}

poly p_SubstPoly (poly p, int var, poly image, const ring preimage_r, const ring image_r, const nMapFunc nMap, matrix cache)
{
  if (p==NULL) return NULL;

  if (rIsNCRing(image_r))
  {
    if((image_r!=preimage_r)
    ||(preimage_r!=currRing))
    {
      WerrorS("not implemented");
      return NULL;
    }
    return pSubst(pCopy(p),var,image);
  }
  matrix org_cache=cache;
  if (cache==NULL) cache=mpNew(preimage_r->N,maMaxDeg_P(p, preimage_r));

  poly result = NULL;
  sBucket_pt bucket = sBucketCreate(image_r);
  while (p != NULL)
  {
    poly q=p_SubstMon(p,var,image,preimage_r,image_r,nMap,cache);
    pIter(p);
    if (q!=NULL) sBucket_Add_p(bucket,q,pLength(q));
  }
  int l_dummy;
  sBucketDestroyAdd(bucket, &result, &l_dummy);
  /*if no cache was passed,it is a local cache: */
  if (org_cache==NULL) id_Delete((ideal*)&cache,image_r);
  return result;
}

ideal id_SubstPoly (ideal id, int var, poly image, const ring preimage_r, const ring image_r, const nMapFunc nMap)
{
  matrix cache=mpNew(preimage_r->N,maMaxDeg_Ma(id, preimage_r));
  int k=MATROWS((matrix)id)*MATCOLS((matrix)id);
  ideal res=(ideal)mpNew(MATROWS((matrix)id),MATCOLS((matrix)id));
  res->rank = id->rank;
  for(k--;k>=0;k--)
  {
    res->m[k]=p_SubstPoly(id->m[k],var,image,preimage_r,image_r,nMap,cache);
  }
  id_Delete((ideal*)&cache,image_r);
  return res;
}
