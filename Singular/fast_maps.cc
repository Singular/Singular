/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    fast_maps.cc
 *  Purpose: implementation of fast maps
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 02/01
 *  Version: $Id: fast_maps.cc,v 1.9 2002-01-19 12:49:20 Singular Exp $
 *******************************************************************/
#include "mod2.h"
#include <omalloc.h>
#include "p_polys.h"
#include "prCopy.h"
#include "ideals.h"
#include "ring.h"
#include "febase.h"
#include "fast_maps.h"

#if 0
// paste into extra.cc
    if(strcmp(sys_cmd,"map")==0)
    {
      ring image_r = currRing;
      map theMap = (map)h->Data();
      ideal image_id = (ideal) theMap;
      ring map_r = IDRING(idroot->get(theMap->preimage, myynest));
      ideal map_id = IDIDEAL(map_r->idroot->get(h->Next()->Name(), myynest));

      ring src_r, dest_r;
      maMap_CreateRings(map_id, map_r, image_id, image_r, src_r, dest_r);
      mapoly mp;
      maideal mideal;
          
      maMap_CreatePolyIdeal(map_id, map_r, src_r, dest_r, mp, mideal);
      maPoly_Out(mp, src_r);
      return FALSE;
    }

// use as Singular source template
ring map_r = 32003, (a, b, c), Dp;
ideal map_id = a, ab, c3 + ab, a2b + ab;


ring image_r;
ideal image_id = x2y3, y, z+x6;

map Phi = map_r, image_id;


system("map", Phi, map_id);

#endif
/*******************************************************************************
**
*F  debugging stuff
*/
void maMonomial_Out(mapoly monomial, ring src_r, ring dest_r = NULL)
{
  p_wrp(monomial->src, src_r);
  printf(" ref:%d", monomial->ref);
  if (dest_r != NULL)
  {
    printf(" dest:");
    p_wrp(monomial->dest, dest_r);
  }
  printf("\n");
  fflush(stdout);
}

void maPoly_Out(mapoly mpoly, ring src_r, ring dest_r = NULL)
{
  while (mpoly != NULL)
  {
    maMonomial_Out(mpoly, src_r, dest_r);
    mpoly = mpoly->next;
  }
}


/*******************************************************************************
**
*F  mapolyCreate  . . . . . . . . . . . . . . .  Creates mapoly
*/
static omBin mapolyBin = omGetSpecBin(sizeof(mapoly_s));
static omBin macoeffBin = omGetSpecBin(sizeof(macoeff_s));

mapoly maMonomial_Create(poly p, ring r_p, sBucket_pt bucket = NULL)
{
  mapoly mp = (mapoly) omAlloc0Bin(mapolyBin);
  mp->src = p;
  p->next = NULL;

  if (bucket != NULL)
  {
    mp->coeff = (macoeff) omAlloc0Bin(macoeffBin);
    mp->coeff->bucket = bucket;
    mp->coeff->n = pGetCoeff(p);
  }
  mp->ref = 1;
  return mp;
}

void maMonomial_Destroy(mapoly mp, ring src_r, ring dest_r = NULL)
{
  if (mp != NULL)
  {
    p_LmFree(mp->src, src_r);
    if (mp->coeff != NULL)
    {
      macoeff coeff, next = mp->coeff;
      do
      {
        coeff = next;
        next = coeff->next;
        omFreeBin(coeff, macoeffBin);
      }
      while (next != NULL);
      if (mp->dest != NULL) 
      {
        assume(dest_r != NULL);
        p_Delete(&(mp->dest), dest_r);
      }
      omFreeBin(mp, mapolyBin);
    }
  }
}

/*******************************************************************************
**
*F  maPoly_InsertMonomial . . . . . . . . .insertion of a monomial into mapoly
*/
mapoly maPoly_InsertMonomial(mapoly into, mapoly what, ring src_r)
{
  if (into == NULL)
  {
    into = what;
    return what;
  }
  
  mapoly iter = into;
  mapoly prev = NULL;
  
  Top:
  p_LmCmpAction(iter->src, what->src, src_r, goto Equal, goto Greater, goto Smaller);
  

  Greater:
  if (iter->next == NULL)
  {
    iter->next = what;
    return into;
  }
  prev = iter;
  iter = iter->next;
  goto Top;
  
  Smaller:
  if (prev == NULL)
  {
    what->next = iter;
    return what;
  }
  prev->next = what;
  what->next = iter;
  return into;
  
  Equal:
  iter->ref += what->ref;
  macoeff coeff = what->coeff;
  if (coeff != NULL) 
  {
    while (coeff->next != NULL) coeff = coeff->next;
    coeff->next = iter->coeff;
    iter->coeff = what->coeff;
    what->coeff = NULL;
  }
  maMonomial_Free(what, src_r);
  return into;
}

mapoly maPoly_InsertMonomial(mapoly into, poly p, ring src_r, sBucket_pt bucket = NULL)
{
  return maPoly_InsertMonomial(into, maMonomial_Create(p, src_r, bucket), src_r);
}

static mapoly maPoly_InsertPoly(mapoly into, poly what, ring src_r, sBucket_pt bucket)
{
  poly next;
  
  while (what != NULL)
  {
    next = what->next;
    into = maPoly_InsertMonomial(into, what, src_r, bucket);
    what = next;
  }
  return into;
}

void maMap_CreatePolyIdeal(ideal map_id, ring map_r, ring src_r, ring dest_r,
                           mapoly &mp, maideal &mideal)
{
  mideal = (maideal) omAlloc0(sizeof(maideal_s));
  mideal->n = IDELEMS(map_id);
  mideal->buckets = (sBucket_pt*) omAlloc0(mideal->n*sizeof(sBucket_pt));
  int i;
  mp = NULL;
  
  for (i=0; i<mideal->n; i++)
  {
    if (map_id->m[i] != NULL)
    {
      mideal->buckets[i] = sBucketCreate(dest_r);
      mp = maPoly_InsertPoly(mp, 
                             prShallowCopyR_NoSort(map_id->m[i], map_r, src_r),
                             src_r,                     
                             mideal->buckets[i]);
    }
  }
}

void maMap_CreateRings(ideal map_id, ring map_r, 
                       ideal image_id, ring image_r, 
                       ring &src_r, ring &dest_r)
{
  src_r = map_r;
  dest_r = image_r;
}


ideal maIdeal_2_Ideal(maideal m_id, ring dest_r)
{
  ideal res = idInit(m_id->n,1);
  int l;
  
  for (int i= 0; i < m_id->n; i++)
  {
    sBucketDestroyMerge(m_id->buckets[i], &(res->m[i]), &l);
  }
  return res;
}

#if 0

/*******************************************************************************
**
*F  maMaxExp  . . . . . . . . . . . .  returns maximal exponent of result of map
*/

// return maximal monomial if max_map_monomials are substituted into pi_m
static poly maGetMaxExpP(poly* max_map_monomials, 
                         int n_max_map_monomials, ring map_r, 
                         poly pi_m, ring pi_r)
{
  int n = min(pi_r->N, n_max_map_monomials);
  int i, j;
  Exponent_t e_i, e_j;
  poly m_i, map_j = p_Init(map_r);
  
  for (i=1; i <= n; i++)
  {
    e_i = p_GetExp(pi_m, i, pi_r);
    m_i = max_map_monomials[i-1];
    if (e_i > 0 && m_i != NULL && ! p_IsConstantComp(m_i, map_r))
    {
      for (j = 1; j<= map_r->N; j++)
      {
        e_j = p_GetExp(m_i, j, map_r);
        if (e_j > 0)
        {
          p_AddExp(map_j, j, e_j*e_i, map_r);
        }
      }
    }
  }
  return map_j;
}

// returns maximal monomial if map_id is applied to pi_id
static poly maGetMaxExpP(ideal map_id, ring map_r, ideal pi_id, ring pi_r)
{
  poly* max_map_monomials = (poly*) omAlloc(IDELEMS(map_id)*sizeof(poly));
  poly max_pi_i, max_map_i;
  poly max_map = p_Init(map_r);
  
  int i, j;
  for (i=0; i<IDELEMS(map_id); i++)
  {
    max_map_monomials[i] = p_GetMaxExpP(map_id->m[i], map_r);
  }
  
  for (i=0; i<IDELEMS(pi_id); i++)
  {
    max_pi_i = p_GetMaxExpP(pi_id->m[i], pi_r);
    max_map_i = maGetMaxExpP(max_map_monomials, IDELEMS(map_id), map_r, 
                              max_pi_i, pi_r);
    // get maximum 
    for (j = 1; j<= map_r->N; j++)
    {
      if (p_GetExp(max_map, j, map_r) < p_GetExp(max_map_i, j, map_r))
        p_SetExp(max_map, j, p_GetExp(max_map_i, j, map_r), map_r);
    }
    p_LmFree(max_pi_i, pi_r);
    p_LmFree(max_map_i, map_r);
  }
  return max_map;
}

// returns maximal exponent if map_id is applied to pi_id
static Exponent_t maGetMaxExp(ideal map_id, ring map_r, ideal pi_id, ring pi_r)
{
  poly p = maGetMaxExpP(map_id, map_r, pi_id, pi_r);
  Exponent_t max = p_GetMaxExp(p, map_r);
  p_LmFree(p, map_r);
  return max;
}

// construct ring/map ideal  in/with which we perform computations
// return TRUE if ordering changed (not yet implemented), false, otherwise
static BOOLEAN maGetCompIdealRing(ideal map_id, ring map_r, ideal pi_id, 
                                   ring pi_r, ideal &comp_map_id, ring &comp_r)
{
  Exponent_t max_exp = maGetMaxExp(map_id, map_r, pi_id, pi_r);

  comp_r = rModifyRing(map_r, FALSE, !idIsModule(pi_id, pi_r), max_exp);
  if (comp_r != map_r)
  {
    if (TEST_OPT_PROT)
      Print("[%d:%d]", (long) comp_r->bitmask, comp_r->ExpL_Size);
    comp_map_id = idrShallowCopyR_NoSort(map_id, map_r, comp_r);
  }
  else
    comp_map_id = map_id;
  
  return FALSE;
}

static void maDestroyCompIdealRing(ideal map_id, ring map_r, 
                                    ideal comp_map_id, ring &comp_r,
                                    ideal &result, BOOLEAN sort=FALSE)
{
  if (map_r != comp_r)
  {
    result = idrMoveR(result, comp_r, map_r);
    id_ShallowDelete(&comp_map_id, comp_r);
    rKillModifiedRing(comp_r);
  }
}

/*******************************************************************************
**
*F  maEggt  . . . . . . . . . . . . . . . . . . . . . . . .  returns extended ggt 
*/
// return NULL if deg(ggt(m1, m2)) < 2
// else return m = ggT(m1, m2) and q1, q2 such that m1 = q1*m m2 = q2*m
static poly maEggT(const poly m1, const poly m2, poly &q1, poly &q2,const ring r)
{
  int i;
  int dg = 0;
  poly ggt = NULL;
  for (i=1; i<=r->N; i++)
  {
    Exponent_t e1 = p_GetExp(m1, i, r);
    Exponent_t e2 = p_GetExp(m2, i, r);
    if (e1 > 0 && e2 > 0)
    {
      Exponent_t em = (e1 > e2 ? e2 : e1);
      if (dg < 2)
      {
        ggt = p_Init(r);
        q1 = p_Init(r);
        q2 = p_Init(r);
      }
      dg += em;
      p_SetExp(ggt, i, em, r);
      p_SetExp(q1, i, e1 - em, r);
      p_SetExp(q2, i, e2 - em, r);
    }
  }
  if (ggt != NULL)
  {
    p_Setm(ggt, r);
    p_Setm(q1, r);
    p_Setm(q2, r);
  }
  return ggt;
}

/*******************************************************************************
**
*F  maGetMapRing  . . . . . . . . . . . . gets ring and ideal with which we work
*/
static ring maGetWeightedRing(ideal theMap, ring map_r)  
{
  // we work in a ring with ordering Deg,WeightedDegree,vars
  // First, construct weighted degrees
  int* weights = (int*) omAlloc0(map_r->N);
  int i;
  int n = min(map_r->N, IDELEMS(theMap));

  for (i=0; i<n; i++)
  {
    weights[i] = pLength(theMap->m[i]);
  }
  return rModifyRing_Wp(map_r, weights);
}
static void maDestroyWeightedRing(ring r)
{
  rKillModified_Wp_Ring(r);
}





static void maMap_2_maPoly(ideal source_id, ring source_r, ring weight_r, ring comp_r,
                           mapoly &mp, maideal &mideal)
{
  mideal = (maideal) omAlloc0(sizeof(maideal_s));
  mideal->n = IDELEMS(source_id);
  mideal->buckets = (sBucket_pt*) omAlloc0(mideal->n*sizeof(sBucket_pt));
  int i;
  mp = NULL;
  
  for (i=0; i<mideal->n; i++)
  {
    if (source_id->m[i] != NULL)
    {
      mideal->buckets[i] = sBucketCreate(comp_r);
      mapInsertPoly(mp, 
                    prShallowCopyR_NoSort(source_id->m[i], source_r, weight_r),
                    weight_r,                     
                    mideal->buckets[i]);
    }
    maPoly_Out(mp, source_r);
  }
}





static mapoly maFindBestggT(mapoly mp, mapoly in, poly ggT, poly fp, poly fq)
{
  int ggt_deg = 0;
  poly p = mp->src;
  mapoly mq = NULL;
  
  ggT = NULL;
  fp = NULL;
  fq = NULL;
  while (in != NULL && in->factors > 1 && pFDeg(in->src) > ggt_deg)
  {
    poly q1, q2, q;
//    q = maEggT(p, in->src, q1, q2);
    if (q != NULL)
    {
      if (pFDeg(q) > fft_deg)
      {
        if (ggT != NULL)
        {
          p_LmFree(ggT);
          p_LmFree(fp);
          p_LmFree(fq);
        }
        ggT = q;
        fp = q1;
        fq = q2;
      }
      else
      {
        p_LmFree(q);
        p_LmFree(q1);
        p_LmFree(q2);
      }
    }
  }
}

      
static mapoly maPrepareEval(mapoly mp, ring r)
{
  mapoly res = NULL;
  mapoly next = mp;
  
  while (mp != NULL)
  {
    next = mp->next;
    mp->next = res;
    res = mp;
    if (mp->factors > 1 && mp->f1 == NULL && mp->f2 == NULL)
    {
      poly fp, fq, ggT;
      mapoly mq = maFindBestggT(mp, next, ggT, fp, fq);
      if (mq != NULL)
      {
        assume(fp != NULL);
        mp->f1 = maInsertMonomial(next, fp, r, NULL);
        
        if (ggT != NULL)
        {
          assume(fq != NULL);
          mapoly mggT = maInsertMonomial(next, ggT, r, NULL);
          mq->f1 = maInsertMonomial(next, fq, r, NULL);
          mq->f2 = mggT;
          mp->f2 = mggT;
          mggT->ref++;
        }
        else
        {
          assume(fq == NULL);
          mp->f2 = mq;
          mq->ref++;
        }
      }
    }
    mp = next;
  }
  return res;
}
/*******************************************************************************
**
*F  ideal_2_maideal . . . . . . . . . . . . . . . . .  converts ideal to maideal
*/
mapoly ideal_2_maideal(ideal id, ring r, maideal mid, ring mr, ring comp_ring,
                       nMapFunc nMap)
{
  mid->n = IDELEMS(id);
  mid->buckets = (sBucket_pt*)omAlloc(mid->n*sizeof(sBucket_pt));
  mapoly mpoly = NULL;
  
  for (int i=0; i<mid->n; i++)
  {
    mid->buckets[i] = sBucketCreate(comp_ring);
    mapoly mpoly_i = poly_2_mapoly(id->m[i], mid, nMap, mid->buckets[i]);
    mpoly = mapAdd(mpoly, mpoly_i);
  }
  return mpoly;
}

ideal maideal_2_ideal(ideal orig_id, 
                      maideal mideal, ring comp_ring, ring dest_ring)
{
  ideal id = idInit(IDELEMS(orig_id), orig_id->rank);
  for (int i=0; i<mid->n; i++)
  { 
    sBucketDestroyAdd(mid->buckets[i], &(id->m[i]), &dummy);
  }
  
  if (comp_ring != dest_ring)
    id = idrMoveR_NoSort(id, comp_ring);
  
  return id;
}

#endif


    
    
                    
  
    








  
  
  
  

 
      
