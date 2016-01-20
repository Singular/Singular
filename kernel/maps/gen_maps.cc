/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include <kernel/mod2.h>
#include <omalloc/omalloc.h>
#include <misc/options.h>
#include <polys/monomials/p_polys.h>
#include <polys/prCopy.h>
#include <kernel/ideals.h>
#include <polys/monomials/ring.h>
#include <polys/monomials/maps.h>
#include <polys/sbuckets.h>
#include <kernel/maps/fast_maps.h>
#include <kernel/maps/find_perm.h>

// polynomial map for ideals/module/matrix
// map_id: the ideal to map
// preimage_r: the base ring for map_id
// image_id: the image of the variables
// image_r: the base ring for image_id
// nMap: map for coefficients
ideal maMapIdeal(const ideal map_id, const ring preimage_r,const ideal image_id, const ring image_r, const nMapFunc nMap)
{
  if(!rIsPluralRing(image_r))
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
    // long polys in the image ?: possiblity of many common subexpressions
    if (nMap==ndCopyMap) /* and !rIsPluralRing(image_r) */
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
  int R=1; //((matrix)map_id)->rows();
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
  return (ideal)m;
}



