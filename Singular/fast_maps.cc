/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    fast_maps.cc
 *  Purpose: implementation of fast maps
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 02/01
 *  Version: $Id: fast_maps.cc,v 1.19 2002-01-19 18:03:38 obachman Exp $
 *******************************************************************/
#include "mod2.h"
#include <omalloc.h>
#include "p_polys.h"
#include "prCopy.h"
#include "ideals.h"
#include "ring.h"
#include "febase.h"
#include "fast_maps.h"

// define if you want to use special dest_ring
#define HAVE_DEST_R 1
// define if you want to use special src_ring
#define HAVE_SRC_R 0
// define if you want to use optimization step
#define HAVE_MAP_OPTIMIZATION 1

/*******************************************************************************
**
*F  maMaxExp  . . . . . . . .  returns bound on maximal exponent of result of map
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

// returns maximal exponent if map_id is applied to pi_id
static Exponent_t maGetMaxExp(ideal map_id, ring map_r, ideal pi_id, ring pi_r)
{
  Exponent_t max=0;
  poly* max_map_monomials = (poly*) omAlloc(IDELEMS(map_id)*sizeof(poly));
  poly max_pi_i, max_map_i;
  
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
    Exponent_t temp = p_GetMaxExp(max_map_i, map_r);
    if (temp > max){
      max=temp;
    }

    p_LmFree(max_pi_i, pi_r);
    p_LmFree(max_map_i, map_r);
  }
  return max;
}


/*******************************************************************************
**
*F  debugging stuff
*/
#ifndef NDEBUG
void maMonomial_Out(mapoly monomial, ring src_r, ring dest_r = NULL)
{
  p_wrp(monomial->src, src_r);
  printf(" ref:%d", monomial->ref);
  if (dest_r != NULL)
  {
    printf(" dest:");
    p_wrp(monomial->dest, dest_r);
  }
  if (monomial->f1!=NULL) { printf(" f1:"); p_wrp(monomial->f1->src, src_r); }
  if (monomial->f2!=NULL) { printf(" f2:"); p_wrp(monomial->f1->src, src_r); }
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
#endif

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
mapoly maPoly_InsertMonomial(mapoly &into, mapoly what, ring src_r)
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
    return what;
  }
  prev = iter;
  iter = iter->next;
  goto Top;
  
  Smaller:
  if (prev == NULL)
  {
    into = what;
    what->next = iter;
    return what;
  }
  prev->next = what;
  what->next = iter;
  return what;
  
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
  return iter;
}

mapoly maPoly_InsertMonomial(mapoly &into, poly p, ring src_r, sBucket_pt bucket = NULL)
{
  return maPoly_InsertMonomial(into, maMonomial_Create(p, src_r, bucket), src_r);
}

static void maPoly_InsertPoly(mapoly &into, poly what, ring src_r, sBucket_pt bucket)
{
  poly next;
  
  while (what != NULL)
  {
    next = what->next;
    maPoly_InsertMonomial(into, what, src_r, bucket);
    what = next;
  }
}

/*******************************************************************************
**
*F  maMap_Create . . . . . . . . . . . . . . . . . . . . create stuff
*/

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
      maPoly_InsertPoly(mp, 
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
#if HAVE_SRC_R > 0
  int* weights = (int*) omAlloc0(map_r->N);
  int i;
  int n = min(map_r->N, IDELEMS(theMap));

  for (i=0; i<n; i++)
  {
    weights[i] = pLength(theMap->m[i]);
  }
  src_r = rModifyRing_Wp(map_r, weights);
#else
  src_r = map_r;
#endif

#if HAVE_DEST_R > 0
  Exponent_t maxExp = maGetMaxExp(map_id, map_r, image_id, image_r);
  dest_r = rModifyRing_Simple(image_r, TRUE, TRUE, maxExp);
#else
  dest_r = image_r;
#endif
}

static void maMap_KillRings(ring map_r, ring image_r, ring src_r, ring dest_r)
{
  if (map_r != src_r)
    rKillModified_Wp_Ring(src_r);
  if (image_r != dest_r)
    rKillModifiedRing_Simple(dest_r);
}

/*******************************************************************************
**
*F  misc  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  misc  stuff
*/
    
ideal maIdeal_2_Ideal(maideal m_id, ring dest_r)
{
  ideal res = idInit(m_id->n, 1);
  int l;
  
  for (int i= 0; i < m_id->n; i++)
  {
    sBucketDestroyAdd(m_id->buckets[i], &(res->m[i]), &l);
  }
  omFree(m_id);
  return res;
}

void maPoly_GetLength(mapoly mp, ring src_r, int length, int total_cost)
{
  length = 0;
  total_cost = 0;
  while (mp != NULL)
  {
    length++;
    if (mp->src != NULL)
      total_cost += pDeg(mp->src, src_r);
    mp = mp->next;
  }
}

  
/*******************************************************************************
**
*F  fast_map  . . . . . . . . . . . . . . . . . . . . . . . . . .the real thing
*/

ideal fast_map(ideal map_id, ring map_r, ideal image_id, ring image_r)
{
  ring src_r, dest_r;
  ideal dest_id, res_id;
  int length, total_cost = 0;

  // construct rings we work in: 
  // src_r: Wp with Weights set to length of poly in image_id
  // dest_r: Simple ring without degree ordering and short exponents
  maMap_CreateRings(map_id, map_r, image_id, image_r, src_r, dest_r);
  
  // construct dest_id
  if (dest_r != image_r)
    dest_id = idrShallowCopyR(image_id, image_r, dest_r);
  else
    dest_id = image_id;

  // construct mpoly and mideal
  mapoly mp;
  maideal mideal;
  maMap_CreatePolyIdeal(map_id, map_r, src_r, dest_r, mp, mideal);
  
  if (TEST_OPT_PROT)
  {
    maPoly_GetLength(mp, src_r, length, total_cost);
    Print("map[%d:%d]{%d:%d}", src_r->bitmask, src_r->ExpL_Size, length, total_cost);
  }
  
  // do the optimization step
#if HAVE_MAP_OPTIMIZE > 0
  maPoly_Optimize(mp, src_r);
#endif
  if (TEST_OPT_PROT)
  {
    maPoly_GetLength(mp, src_r, length, total_cost);
    Print("{%d:%d}", length, total_cost);
  }
      
  // do the actual evaluation
  maPoly_Eval(mp, src_r, dest_id, dest_r, total_cost);

  // collect the results back into an ideal
  ideal res_dest_id = maIdeal_2_Ideal(mideal, dest_r);

  // convert result back to image_r
  ideal res_image_id;
  if (dest_r != image_r)
  {
    res_image_id = idrShallowCopyR(res_dest_id, dest_r, image_r);
    id_ShallowDelete(&res_dest_id, dest_r);
  }
  else
    res_image_id = res_dest_id;
  
  // clean-up the rings
  maMap_KillRings(map_r, image_r, src_r, dest_r);

  return res_image_id;
}




    
    
                    
  
    








  
  
  
  

 
      
