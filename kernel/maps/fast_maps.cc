/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    fast_maps.cc
 *  Purpose: implementation of fast maps
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 02/01
 *******************************************************************/



#include "kernel/mod2.h"
#include "misc/options.h"
#include "polys/monomials/p_polys.h"
#include "polys/prCopy.h"
#include "kernel/ideals.h"
#include "polys/monomials/ring.h"
#include "polys/sbuckets.h"
#include "kernel/maps/fast_maps.h"

// define if you want to use special dest_ring
#define HAVE_DEST_R 1
// define if you want to use special src_ring
#define HAVE_SRC_R 1
// define if you want to use optimization step
#define HAVE_MAP_OPTIMIZE 1

/*******************************************************************************
**
*F  maMaxExp  . . . . . . . .  returns bound on maximal exponent of result of map
*/
// return maximal monomial if max_map_monomials are substituted into pi_m
static poly maGetMaxExpP(poly* max_map_monomials,
                         int n_max_map_monomials, ring map_r,
                         poly pi_m, ring pi_r)
{
  int n = si_min(pi_r->N, n_max_map_monomials);
  int i, j;
  unsigned long e_i, e_j;
  poly m_i=NULL;
  poly map_j = p_Init(map_r);

  for (i=1; i <= n; i++)
  {
    e_i = p_GetExp(pi_m, i, pi_r);
    if (e_i==0) e_i=1;
    m_i = max_map_monomials[i-1];
    if (m_i != NULL && ! p_IsConstantComp(m_i, map_r))
    {
      for (j = 1; j<= map_r->N; j++)
      {
        e_j = p_GetExp(m_i, j, map_r);
        if (e_j == 0) e_j=1;
        p_AddExp(map_j, j, e_j*e_i, map_r);
      }
    }
  }
  return map_j;
}

// returns maximal exponent if map_id is applied to pi_id
static unsigned long maGetMaxExp(ideal pi_id, ring pi_r, ideal map_id, ring map_r)
{
  unsigned long max=0;
  poly* max_map_monomials = (poly*) omAlloc(IDELEMS(map_id)*sizeof(poly));
  poly max_pi_i, max_map_i;

  int i;
  for (i=0; i<IDELEMS(map_id); i++)
  {
    max_map_monomials[i] = p_GetMaxExpP(map_id->m[i], map_r);
  }

  for (i=0; i<IDELEMS(pi_id); i++)
  {
    max_pi_i = p_GetMaxExpP(pi_id->m[i], pi_r);
    max_map_i = maGetMaxExpP(max_map_monomials, IDELEMS(map_id), map_r,
                              max_pi_i, pi_r);
    unsigned long temp = p_GetMaxExp(max_map_i, map_r);
    if (temp > max){ max=temp; }

    p_LmFree(max_pi_i, pi_r);
    p_LmFree(max_map_i, map_r);
  }
  for (i=0; i<IDELEMS(map_id); i++)
  {
    p_Delete(&max_map_monomials[i], map_r);
  }
  omFreeSize(max_map_monomials,IDELEMS(map_id)*sizeof(poly));

  return max;
}


/*******************************************************************************
**
*F  debugging stuff
*/
#ifndef SING_NDEBUG
void maMonomial_Out(mapoly monomial, ring src_r, ring dest_r)
{
  p_wrp(monomial->src, src_r);
  printf(" ref:%d", monomial->ref);
  if (dest_r != NULL)
  {
    printf(" dest:");
    p_wrp(monomial->dest, dest_r);
  }
  if (monomial->f1!=NULL) { printf(" f1:%lx", (long)monomial->f1->src);
                            // p_wrp(monomial->f1->src, src_r);
                          }
  if (monomial->f2!=NULL) { printf(" f2:%lx",(long)monomial->f2->src);
                            // p_wrp(monomial->f2->src, src_r);
                          }
  printf("\n");
  fflush(stdout);
}

void maPoly_Out(mapoly mpoly, ring src_r, ring dest_r)
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
STATIC_VAR omBin mapolyBin = omGetSpecBin(sizeof(mapoly_s));
STATIC_VAR omBin macoeffBin = omGetSpecBin(sizeof(macoeff_s));

mapoly maMonomial_Create(poly p, ring /*r_p*/, sBucket_pt bucket)
{
  mapoly mp = (mapoly) omAlloc0Bin(mapolyBin);
  //p_wrp(p,r_p);printf(" (%x) created\n",mp);
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

void maMonomial_Destroy(mapoly mp, ring src_r, ring dest_r)
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
    }
  }
  omFreeBin(mp, mapolyBin);
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

mapoly maPoly_InsertMonomial(mapoly &into, poly p, ring src_r, sBucket_pt bucket)
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
#ifdef PDEBUG
                        prShallowCopyR(map_id->m[i], map_r, src_r),
#else
                        prShallowCopyR_NoSort(map_id->m[i], map_r, src_r),
#endif
                        src_r,
                        mideal->buckets[i]);
    }
  }
}

void maMap_CreateRings(ideal map_id, ring map_r,
                       ideal image_id, ring image_r,
                       ring &src_r, ring &dest_r, BOOLEAN &simple)
{
#if HAVE_SRC_R > 0
  int* weights = (int*) omAlloc0(map_r->N*sizeof(int));
  int i;
  int n = si_min(map_r->N, IDELEMS(image_id));

  for (i=0; i<n; i++)
  {
    weights[i] = pLength(image_id->m[i])+1;
  }
  src_r = rModifyRing_Wp(map_r, weights);
#else
  src_r = map_r;
#endif

#if HAVE_DEST_R > 0
  unsigned long maxExp = maGetMaxExp(map_id, map_r, image_id, image_r);
  if (maxExp <=  1) maxExp = 2;
  else if (maxExp > (unsigned long) image_r->bitmask)
    maxExp = (unsigned long) image_r->bitmask;
  dest_r = rModifyRing_Simple(image_r, TRUE, TRUE, maxExp,  simple);
#else
  dest_r = image_r;
#endif
}

static void maMap_KillRings(ring map_r, ring image_r, ring src_r, ring dest_r)
{
  if (map_r != src_r)
    rKillModified_Wp_Ring(src_r);
  if (image_r != dest_r)
    rKillModifiedRing(dest_r);
}

/*******************************************************************************
**
*F  misc  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  misc  stuff
*/

ideal maIdeal_2_Ideal(maideal m_id, ring /*dest_r*/)
{
  ideal res = idInit(m_id->n, 1);
  int l;

  for (int i= 0; i < m_id->n; i++)
  {
    if (m_id->buckets[i]!=NULL)
      sBucketDestroyAdd(m_id->buckets[i], &(res->m[i]), &l);
  }
  omFreeSize(m_id->buckets,m_id->n*sizeof(sBucket_pt));
  omFree(m_id);
  return res;
}

void maPoly_GetLength(mapoly mp, int &length)
{
  length = 0;
  while (mp != NULL)
  {
    length++;
    mp = mp->next;
  }
}


/*******************************************************************************
**
*F  fast_map_common_subexp   . . . . . . . . . . . . . . . . . .the real thing
*/

ideal fast_map_common_subexp(const ideal map_id,const ring map_r,const ideal image_id,const ring image_r)
{
  ring src_r, dest_r;
  ideal dest_id/*, res_id*/;
  int length = 0;
  BOOLEAN no_sort;

  // construct rings we work in:
  // src_r: Wp with Weights set to length of poly in image_id
  // dest_r: Simple ring without degree ordering and short exponents
  maMap_CreateRings(map_id, map_r, image_id, image_r, src_r, dest_r, no_sort);

  // construct dest_id
  if (dest_r != image_r)
  {
    dest_id = idrShallowCopyR(image_id, image_r, dest_r);
  }
  else
    dest_id = image_id;

  // construct mpoly and mideal
  mapoly mp;
  maideal mideal;
  maMap_CreatePolyIdeal(map_id, map_r, src_r, dest_r, mp, mideal);

  if (TEST_OPT_PROT)
  {
    maPoly_GetLength(mp, length);
    Print("map[%ld:%d]{%d:", dest_r->bitmask, dest_r->ExpL_Size, length);
  }

  // do the optimization step
#if HAVE_MAP_OPTIMIZE > 0
  if (mp!=NULL) maPoly_Optimize(mp, src_r);
#endif
  if (TEST_OPT_PROT)
  {
    maPoly_GetLength(mp, length);
    Print("%d}", length);
  }

  // do the actual evaluation
  maPoly_Eval(mp, src_r, dest_id, dest_r, length);
  if (TEST_OPT_PROT) PrintS(".");

  // collect the results back into an ideal
  ideal res_dest_id = maIdeal_2_Ideal(mideal, dest_r);
  if (TEST_OPT_PROT) PrintS(".");

  // convert result back to image_r
  ideal res_image_id;
  if (dest_r != image_r)
  {
    //if (no_sort) see Old/m134si.tst
    //  res_image_id = idrShallowCopyR_NoSort(res_dest_id, dest_r, image_r);
    //else
      res_image_id = idrShallowCopyR(res_dest_id, dest_r, image_r);
    id_ShallowDelete(&res_dest_id, dest_r);
    id_ShallowDelete(&dest_id,dest_r);
  }
  else
    res_image_id = res_dest_id;

  if (TEST_OPT_PROT) PrintS(".");

  // clean-up the rings
  maMap_KillRings(map_r, image_r, src_r, dest_r);

  if (TEST_OPT_PROT)
    PrintLn();

  idTest(res_image_id);
  return res_image_id;
}


/**********************************************************************
* Evaluation stuff                                                    *
**********************************************************************/

// substitute p everywhere the monomial occours,
// return the number of substitutions
static int maPoly_Substitute(macoeff c, poly p, ring dest_r)
{
  // substitute the monomial: go through macoeff
  int len;
  BOOLEAN zero_div= (rField_is_Ring(dest_r) && !rField_is_Domain(dest_r));
  if (!zero_div) len=pLength(p);
  int done=0;
  while (c!=NULL)
  {
    done++;
    poly t=__pp_Mult_nn(p,c->n,dest_r);
    #ifdef HAVE_RINGS
    if (zero_div) len=pLength(t);
    #endif
    sBucket_Add_p(c->bucket, t, len);
    c=c->next;
  }
  return done;
}

static poly maPoly_EvalMon(poly src, ring src_r, poly* dest_id, ring dest_r)
{
  int i;
  int e;
  poly p=NULL;
  poly pp;
  BOOLEAN is_const=TRUE; // to check for zero-div in p_Mult_q
  for(i=1;i<=src_r->N;i++)
  {
    e=p_GetExp(src,i,src_r);
    if (e>0)
    {
      is_const=FALSE;
      pp=dest_id[i-1];
      if (pp==NULL)
      {
        p_Delete(&p,dest_r);
        return NULL;
      }
      if (/*(*/ p==NULL /*)*/) /* && (e>0)*/
      {
        p=p_Copy(pp /*dest_id[i-1]*/,dest_r);
        e--;
      }
      while (e>0)
      {
        p=p_Mult_q(p,p_Copy(pp /*dest_id[i-1]*/,dest_r),dest_r);
        e--;
      }
    }
  }
  if (is_const)
  {
    assume(p==NULL);
    p=p_ISet(1,dest_r);
  }
  return p;
}

void maPoly_Eval(mapoly root, ring src_r, ideal dest_id, ring dest_r, int total_cost)
{
  // invert the list rooted at root:
  if ((root!=NULL) && (root->next!=NULL))
  {
    mapoly q=root->next;
    mapoly qn;
    root->next=NULL;
    do
    {
      qn=q->next;
      q->next=root;
      root=q;
      q=qn;
    }
    while (qn !=NULL);
  }

  total_cost /= 10;
  int next_print_cost = total_cost;

  // the evaluation -----------------------------------------
  mapoly p=root;
  int cost = 0;

  while (p!=NULL)
  {
    // look at each mapoly: compute its value in ->dest
    assume (p->dest==NULL);
    {
      if ((p->f1!=NULL)&&(p->f2!=NULL))
      {
        poly f1=p->f1->dest;
        poly f2=p->f2->dest;
        if (p->f1->ref>0) f1=p_Copy(f1,dest_r);
        else
        {
          // we own p->f1->dest now (in f1)
          p->f1->dest=NULL;
        }
        if (p->f2->ref>0) f2=p_Copy(f2,dest_r);
        else
        {
          // we own p->f2->dest now (in f2)
          p->f2->dest=NULL;
        }
        maMonomial_Free(p->f1,src_r, dest_r);
        maMonomial_Free(p->f2,src_r, dest_r);
        p->dest=p_Mult_q(f1,f2,dest_r);
      } /* factors : 2 */
      else
      {
        assume((p->f1==NULL) && (p->f2==NULL));
        // no factorization provided, use the classical method:
        p->dest=maPoly_EvalMon(p->src,src_r,dest_id->m,dest_r);
      }
    } /* p->dest==NULL */
    // substitute the monomial: go through macoeff
    p->ref -= maPoly_Substitute(p->coeff, p->dest, dest_r);
    //printf("subst done\n");
    if (total_cost)
    {
      assume(TEST_OPT_PROT);
      cost++;
      if (cost > next_print_cost)
      {
        PrintS("-");
        next_print_cost += total_cost;
      }
    }

    mapoly pp=p;
    p=p->next;
    //p_wrp(pp->src, src_r);
    if (pp->ref<=0)
    {
      //printf(" (%x) killed\n",pp);
      maMonomial_Destroy(pp, src_r, dest_r);
    }
    //else
    //  printf(" (%x) not killed, ref=%d\n",pp,pp->ref);
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
  poly ggt = p_Init(r);
  q1 = p_Init(r);
  q2 = p_Init(r);

  for (i=1;i<=r->N;i++)
  {
    unsigned long e1 = p_GetExp(m1, i, r);
    unsigned long e2 = p_GetExp(m2, i, r);
    if (e1 > 0 && e2 > 0)
    {
      unsigned long em = (e1 > e2 ? e2 : e1);
      dg += em;
      p_SetExp(ggt, i, em, r);
      p_SetExp(q1, i, e1 - em, r);
      p_SetExp(q2, i, e2 - em, r);
    }
    else
    {
      p_SetExp(q1, i, e1, r);
      p_SetExp(q2, i, e2, r);
    }
  }
  if (dg>1)
  {
    p_Setm(ggt, r);
    p_Setm(q1, r);
    p_Setm(q2, r);
  }
  else
  {
    p_LmFree(ggt, r);
    p_LmFree(q1, r);
    p_LmFree(q2, r);
    ggt = NULL;
  }
  return ggt;
}

/********************************************************************
 **                                                                 *
 * maFindBestggT                                                    *
 * finds ggT with the highest cost                                  *
 *******************************************************************/

static mapoly maFindBestggT(mapoly mp, mapoly & choice, mapoly & fp, mapoly & fq,const ring r)
{
  int ggt_deg = 0;
  poly p = mp->src;
  mapoly iter = choice;
  poly ggT = NULL;
  fp = NULL;
  fq = NULL;
  poly fp_p=NULL;
  poly fq_p=NULL;
  choice=NULL;
  while ((iter != NULL) && (p_Deg(iter->src, r) > ggt_deg))
  {
    //    maMonomial_Out(iter, r, NULL);
    poly q1, q2, q;

    q = maEggT(p, iter->src, q1, q2,r);
    if (q != NULL)
    {
      int tmp_deg;
      assume((q1!=NULL)&&(q2!=NULL));
      if ((tmp_deg=p_Deg(q,r)) > ggt_deg)
      {
        choice=iter;
        if (ggT != NULL)
        {
          p_LmFree(ggT, r);
          p_LmFree(fp_p, r);
          p_LmFree(fq_p, r);
        }
        ggt_deg = tmp_deg ; /*p_Deg(q, r);*/
        ggT = q;
        fp_p = q1;
        fq_p = q2;
      }
      else
      {
        p_LmFree(q, r);
        p_LmFree(q1, r);
        p_LmFree(q2, r);
      }
    }
    iter=iter->next;
  }
  if(ggT!=NULL)
  {
    int dq =p_Totaldegree(fq_p,r);
    if (dq!=0)
    {
      fq=maPoly_InsertMonomial(mp, fq_p, r, NULL);
      fp=maPoly_InsertMonomial(mp, fp_p, r, NULL);
      return maPoly_InsertMonomial(mp, ggT, r, NULL);
    }
    else
    {
      fq=NULL;
      p_LmFree(fq_p, r);
      p_LmFree(ggT, r);
      fp=maPoly_InsertMonomial(mp, fp_p, r, NULL);
      choice->ref++;
      return choice;
    }
  }
  else
  {
    return NULL;
  }
}

/********************************************************************
 **                                                                 *
 * maPoly_Optimize                                                  *
 * adds and integrates subexpressions                               *
 *******************************************************************/

void maPoly_Optimize(mapoly mpoly, ring src_r)
{
  assume(mpoly!=NULL && mpoly->src!=NULL);
  mapoly iter = mpoly;
  mapoly choice;
  mapoly ggT=NULL;
  mapoly fp=NULL;
  mapoly fq=NULL;
  while (iter->next!=NULL)
  {
    choice=iter->next;
    if ( /*(*/ iter->f1==NULL /*)*/ )
    {
      ggT=maFindBestggT(iter, choice, fp, fq,src_r);
      if (choice!=NULL)
      {
        assume(iter->f1==NULL);
        assume(iter->f2==NULL);
        iter->f1=fp;
        iter->f2=ggT;
        if (fq!=NULL)
        {
          ggT->ref++;
          choice->f1=fq;
          choice->f2=ggT;
        }
      }
      else assume(ggT==NULL);
    }
    iter=iter->next;
  }
}
