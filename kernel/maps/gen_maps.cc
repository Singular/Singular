/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include "kernel/mod2.h"
#include "misc/options.h"
#include "polys/monomials/p_polys.h"
#include "polys/prCopy.h"
#include "kernel/ideals.h"
#include "polys/monomials/ring.h"
#include "polys/monomials/maps.h"
#include "polys/sbuckets.h"
#include "kernel/maps/fast_maps.h"
#include "kernel/maps/find_perm.h"
#include "kernel/maps/gen_maps.h"

static void find_subst_for_map(const ring preimage_r, const ring image_r, const ideal image, int &var,poly &p)
{
  p=NULL;
  var=0;
  int i,v;
  for (i=si_min(IDELEMS(image),preimage_r->N)-1; i>=0; i--)
  {
    if (image->m[i]!=NULL)
    {
      if ((pNext(image->m[i])==NULL)
      && (n_IsOne(pGetCoeff(image->m[i]),image_r->cf)))
      {
        v=p_IsUnivariate(image->m[i],image_r);
        if ((v<=0) /*not univariate */
        || (v!=i+1) /* non-trivial */
	|| (p_GetExp(image->m[i],v,image_r)!=1))
        {
          if (var==0)
          {
            var=i+1;
            p=image->m[i];
          }
          else /* found second non-trivial entry */
          {
            goto non_trivial;
          }
        }
      }
      else
      {
        if (var==0)
        {
          var=i+1;
          p=image->m[i];
        }
        else /* found second non-trivial entry */
        {
          goto non_trivial;
        }
      }
    }
    else
    {
        if (var==0)
        {
          var=i+1;
          p=image->m[i];
        }
        else /* found second non-trivial entry */
        {
          goto non_trivial;
        }
    }
  }
  //Print("elms:%d, N:%d\n",IDELEMS(image),preimage_r->N);
  //iiWriteMatrix((matrix)image,"_",1,image_r,0);
  //PrintS("\npreimage:\n");rWrite(preimage_r);
  //PrintS("image:\n");rWrite(image_r);
  //Print("\nsusbt: v%d -> ",var);
  //p_wrp(p,image_r,image_r);
non_trivial:
  var=0;
  p=NULL;
}

// polynomial map for ideals/module/matrix
// map_id: the ideal to map
// preimage_r: the base ring for map_id
// image_id: the image of the variables
// image_r: the base ring for image_id
// nMap: map for coefficients
ideal maMapIdeal(const ideal map_id, const ring preimage_r,const ideal image_id, const ring image_r, const nMapFunc nMap)
{
  // handle special cases
  if (IDELEMS(map_id)<=0)
    return idInit(0,map_id->rank);
  if(!rIsNCRing(image_r))
  {
    // heuristic:
    // is the map a permutation ?
    matrix m=ma_ApplyPermForMap((matrix)map_id,preimage_r,image_id,image_r,nMap);
    if (m!=NULL)
    {
      if (TEST_OPT_PROT) PrintS("map is a permutation\n");
      return (ideal)m;
    }
    // ----------------------------------------------------------
    // is it a substitution of one variable ?
    {
      poly p;
      int var;
      find_subst_for_map(preimage_r,image_r,image_id,var,p);
      if (var!=0)
      {
        return id_SubstPoly(map_id,var,p,preimage_r,image_r,nMap);
      }
    }
    // ----------------------------------------------------------
    // long polys in the image ?: possibility of many common subexpressions
    if ((nMap==ndCopyMap) /* and !rIsPluralRing(image_r) */
    && (map_id->nrows==1) /* i.e. only for ideal/map */
    && (map_id->rank==1))
    {
      int sz=IDELEMS(map_id);
      int sz_l=0;
      int sz_more=0;
      int t,i;
      for(i=sz-1;i>=0;i--)
      {
        sz_l+=pLength(map_id->m[i]);
      }
      for(i=IDELEMS(image_id)-1;i>=0;i--)
      {
        t=pLength(image_id->m[i]);
        if ((t==0) || (t>1)) sz_more++;
      }
      if (((sz_l > sz*2) && (sz_more != 1))||(sz<5))
      {
        if (TEST_OPT_PROT) PrintS("map via common subexpressions\n");
        return fast_map_common_subexp(map_id, preimage_r, image_id, image_r);
      }
    }
  }
  // ----------------------------------------------------------
  // otherwise: try the generic method (generic map with cache)
  if (TEST_OPT_PROT) PrintS("map with cache\n");
  int C=((matrix)map_id)->cols();
  int R=((matrix)map_id)->rows();
  matrix m=mpNew(R,C);
  int N = preimage_r->N;
  matrix cache=mpNew(N,maMaxDeg_Ma(map_id,preimage_r));
  int i;
  for (i=R*C-1;i>=0;i--)
  {
    if (map_id->m[i]!=NULL)
    {
      m->m[i]=maEval((map)image_id, map_id->m[i], preimage_r, nMap, (ideal)cache, image_r);
      p_Test(m->m[i],image_r);
    }
  }
  idDelete((ideal *)&cache);
  ideal ii=(ideal)m;
  ii->rank=((ideal)map_id)->rank;
  return (ideal)m;
}

poly maMapPoly(const poly map_p, const ring map_r,const ideal image_id, const ring image_r, const nMapFunc nMap)
{
  matrix s=mpNew(map_r->N,maMaxDeg_P(map_p, map_r));
  poly p=maEval((map)image_id, map_p, map_r, nMap, (ideal)s, image_r);
  id_Delete((ideal*)&s,image_r);
  return p;
}

number maEvalAt(const poly p,const number* pt, const ring r)
{
  ideal map=idInit(r->N,1);
  for(int i=r->N-1;i>=0;i--)
  {
    map->m[i]=p_NSet(n_Copy(pt[i],r->cf),r);
  }
  poly v=maMapPoly(p,r,map,r,ndCopyMap);
  id_Delete(&map,r);
  number vv;
  if (v==NULL)
    vv=n_Init(0,r->cf);
  else
  {
    vv=pGetCoeff(v);
    p_LmFree(&v,r);
  }
  return vv;
}
