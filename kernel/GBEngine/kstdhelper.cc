/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: wrapper to try stdhild within std
*/


#include "kernel/mod2.h"

#include "coeffs/bigintmat.h"
#include "coeffs/longrat.h"
#include "misc/int64vec.h"
#include "misc/options.h"
#include "misc/intvec.h"
#include "reporter/si_signals.h"
#include "kernel/polys.h"
#define TRANSEXT_PRIVATES
#include "polys/ext_fields/transext.h"
#undef TRANSEXT_PRIVATES
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "kernel/ideals.h"
#include "kernel/combinatorics/hilb.h"
#include "kernel/combinatorics/stairc.h"
#include "Singular/ipid.h"
#include "Singular/cntrlc.h"
#include "Singular/links/ssiLink.h"
#include "Singular/feOpt.h"

#include <limits.h>
#include <limits>

static int kFindLuckyPrime(ideal F, ideal Q) // TODO
{
  int prim=32003;
  // assume coeff are in Q
  return prim;
}

poly kTryHC(ideal F, ideal Q, long* colength)
{
  assume(colength!=NULL);
  *colength=-1;
  if (Q!=NULL)
    return NULL;
  int prim=kFindLuckyPrime(F,Q);
  if (TEST_OPT_PROT) Print("try HC in ring over ZZ/%d\n",prim);
  // create Zp_ring
  ring save_ring=currRing;
  ring Zp_ring=rCopy0(save_ring);
  nKillChar(Zp_ring->cf);
  Zp_ring->cf=nInitChar(n_Zp, (void*)(long)prim);
  rComplete(Zp_ring);
  // map data
  rChangeCurrRing(Zp_ring);
  nMapFunc nMap=n_SetMap(save_ring->cf,Zp_ring->cf);
  if (nMap==NULL) return NULL;
  ideal FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  ideal QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  // call std
  kStrategy strat=new skStrategy;
  strat->LazyPass=20;
  strat->LazyDegree = 1;
  strat->kModW=kModW=NULL;
  strat->kHomW=NULL;
  strat->kHomW64=NULL;
  kHomW=NULL;
  kHomW64=NULL;
  strat->homog = (tHomog)idHomIdeal(F,Q);
  ideal res=mora(FF,QQ,NULL,NULL,strat);
  // clean
  idDelete(&FF);
  poly HC=NULL;
  if (strat->kNoether!=NULL)
  {
    scComputeHC(res,QQ,0,HC);
    *colength=scMult0Int(res,QQ);
  }
  delete strat;
  if (QQ!=NULL) idDelete(&QQ);
  idDelete(&res);
  // map back
  rChangeCurrRing(save_ring);
  if (HC!=NULL)
  {
    //p_IncrExp(HC,Zp_ring->N,Zp_ring);
    for (int i=rVar(Zp_ring)-1; i>0; i--)
    {
      int e;
      if ((e=pGetExp(HC, i)) > 0) pSetExp(HC,i,e-1);
    }
    p_Setm(HC,Zp_ring);
    if (TEST_OPT_PROT) Print("HC(%ld) found\n",pTotaldegree(HC));
    pSetCoeff0(HC,nInit(1));
  }
  else
  {
    if (TEST_OPT_PROT) PrintS("HC not found\n");
  }
  rDelete(Zp_ring);
  return HC;
}

// --------------------------------------------------------
#if 0
static number nMapQa2Zp(number a, const coeffs src, const coeffs dst)
{
  if (a==NULL) return a;
  fraction f=(fraction)a;
  poly p=NUM(f);
  while(pNext(p)!=NULL) pIter(p);
  return nlModP(pGetCoeff(p),src->extRing->cf,dst);
}

static number nMapZpa2Zp(number a, const coeffs src, const coeffs dst)
{
  if (a==NULL) return a;
  fraction f=(fraction)a;
  poly p=NUM(f);
  while(pNext(p)!=NULL) pIter(p);
  return pGetCoeff(p);
}
#endif

static int64 kHilbstdGcd(int64 a, int64 b)
{
  while (b!=0)
  {
    const int64 r=a%b;
    a=b;
    b=r;
  }
  return a;
}

static int64vec* kHilbstdUnitWeights(const int n)
{
  int64vec* w = new int64vec(n);
  for (int i=n-1; i>=0; i--) (*w)[i] = 1;
  return w;
}

static BOOLEAN kHilbstdWeightsAllOne(const int64vec* w)
{
  assume(w != NULL);
  for (int i=w->length()-1; i>=0; i--)
  {
    if ((*w)[i] != 1) return FALSE;
  }
  return TRUE;
}

static BOOLEAN kHilbstdSupportedFDeg(const ring r)
{
  return (r->pFDeg == p_Totaldegree)
      || (r->pFDeg == p_Deg)
      || (r->pFDeg == p_WTotaldegree)
      || (r->pFDeg == p_WFirstTotalDegree);
}

static int64vec* kHilbstdPositiveFDegWeights(const ring r)
{
  assume(r != NULL);

  // A 64-bit A block is used as a consistent grading by the walk code, but
  // rComplete may select the following dp block for pFDeg.  Read this grading
  // directly instead of losing it through pFDeg's total-degree fallback.
  for (int block=0; r->order[block]!=ringorder_no; block++)
  {
    if ((r->order[block]==ringorder_a64)
     && (r->block0[block]==1) && (r->block1[block]==rVar(r)))
    {
      const int64* rw=(const int64*)r->wvhdl[block];
      int64vec* w=new int64vec(rVar(r));
      int64 common=0;
      for (int i=0; i<rVar(r); i++)
      {
        if (rw[i]<=0)
        {
          delete w;
          return NULL;
        }
        (*w)[i]=rw[i];
        common=kHilbstdGcd(common,rw[i]);
      }
      for (int i=0; i<rVar(r); i++) (*w)[i]/=common;
      return w;
    }
  }

  if (!kHilbstdSupportedFDeg(r)) return NULL;

  int64vec* w = new int64vec(rVar(r));
  int64 common=0;
  for (int i=1; i<=rVar(r); i++)
  {
    poly x = p_One(r);
    p_SetExp(x, i, 1, r);
    p_Setm(x, r);
    const long d = r->pFDeg(x, r);
    p_Delete(&x, r);

    if (d <= 0)
    {
      delete w;
      return NULL;
    }
    (*w)[i-1]=(int64)d;
    common=kHilbstdGcd(common,(int64)d);
  }

  // Scaling every weight by the same positive factor does not change
  // homogeneity or the degree comparisons used by Hilbert-driven std.
  for (int i=0; i<rVar(r); i++)
    (*w)[i]/=common;
  return w;
}

static ring kHilbstdWideDpRing(const coeffs cf, const int n, char** names)
{
  rRingOrder_t* order=(rRingOrder_t*)omAlloc(2*sizeof(rRingOrder_t));
  int* block0=(int*)omAlloc0(2*sizeof(int));
  int* block1=(int*)omAlloc0(2*sizeof(int));
  order[0]=ringorder_dp;
  order[1]=ringorder_no;
  block0[0]=1;
  block1[0]=n;
  return rDefault(cf,n,names,2,order,block0,block1,NULL,
                  (unsigned long)LONG_MAX);
}

static int64vec* kHilbstdHomogenizingWeights(const int64vec* w)
{
  assume(w != NULL);

  int64vec* hw = new int64vec(w->length()+1);
  for (int i=w->length()-1; i>=0; i--) (*hw)[i] = (*w)[i];
  (*hw)[w->length()] = 1;
  return hw;
}

static BOOLEAN kHilbstdWeightedDeg(poly p, const int64vec* w, const ring r,
                                  int64* degree)
{
  assume(p != NULL);
  assume(w != NULL);
  assume(w->length() >= rVar(r));

  assume(degree != NULL);
  int64 d = 0;
  for (int i=rVar(r); i>0; i--)
  {
    const int64 e=(int64)p_GetExp(p,i,r);
    const int64 wi=(*w)[i-1];
    if ((e!=0) && (wi>std::numeric_limits<int64>::max()/e))
    {
      WerrorS("weighted degree does not fit into int64");
      return FALSE;
    }
    const int64 term=e*wi;
    if (d>std::numeric_limits<int64>::max()-term)
    {
      WerrorS("weighted degree does not fit into int64");
      return FALSE;
    }
    d+=term;
  }
  *degree=d;
  return TRUE;
}

static poly kHilbstdHomogenW(poly p, int varnum, const int64vec* w, const ring r)
{
  if (p == NULL) return NULL;
  if ((varnum < 1) || (varnum > rVar(r))) return NULL;
  assume(w != NULL);
  assume(w->length() >= rVar(r));
  assume((*w)[varnum-1] == 1);

  int64 maxdeg;
  if (!kHilbstdWeightedDeg(p,w,r,&maxdeg)) return NULL;
  for (poly q=pNext(p); q!=NULL; pIter(q))
  {
    int64 d;
    if (!kHilbstdWeightedDeg(q,w,r,&d)) return NULL;
    if (d > maxdeg) maxdeg = d;
  }

  poly q = p_Copy(p, r);
  poly result = NULL;
  while (q != NULL)
  {
    poly qn = pNext(q);
    pNext(q) = NULL;

    int64 d;
    if (!kHilbstdWeightedDeg(q,w,r,&d))
    {
      p_Delete(&q,r);
      p_Delete(&qn,r);
      p_Delete(&result,r);
      return NULL;
    }
    const int64 shift = maxdeg-d;
    if (shift != 0)
    {
      if (shift>std::numeric_limits<long>::max())
      {
        WerrorS("homogenizing exponent does not fit into the polynomial exponent type");
        p_Delete(&q,r);
        p_Delete(&qn,r);
        p_Delete(&result,r);
        return NULL;
      }
      p_AddExp(q,varnum,(long)shift,r);
      p_Setm(q, r);
    }
    result = p_Add_q(result, q, r);
    q = qn;
  }
  return result;
}

static ideal kHilbstdHomogenIdealW(ideal F, int varnum, const int64vec* w, const ring r)
{
  ideal H = idInit(IDELEMS(F), F->rank);
  for (int i=IDELEMS(F)-1; i>=0; i--)
  {
    H->m[i] = kHilbstdHomogenW(F->m[i], varnum, w, r);
    if ((F->m[i]!=NULL) && (H->m[i]==NULL))
    {
      id_Delete(&H,r);
      return NULL;
    }
  }
  return H;
}

static ideal kTryHilbstd_homog(ideal F, ideal Q, int64vec* hdegree)
{
  assume(hdegree != NULL);
  // create Zp_ring
  ring save_ring=currRing;
  BITSET save_opt;SI_SAVE_OPT1(save_opt);
  int prim=kFindLuckyPrime(F,Q);
  //if(nCoeff_is_transExt(save_ring->cf)
  //&&(nCoeff_is_Zp(save_ring->cf->extRing->cf)))
  //  prim=save_ring->cf->extRing->cf->ch;
  if(nCoeff_is_Zp(save_ring->cf))
    prim=save_ring->cf->ch;
  coeffs cf=nInitChar(n_Zp, (void*)(long)prim);
  ring Zp_ring=kHilbstdWideDpRing(cf,save_ring->N,save_ring->names);
  // map data
  nMapFunc nMap=n_SetMap(save_ring->cf,Zp_ring->cf);
  if (nMap==NULL)
  {
    /*
    if (nCoeff_is_transExt(save_ring->cf))
    {
      if (nCoeff_is_Q(save_ring->cf->extRing->cf))
        nMap=nMapQa2Zp;
      else if (nCoeff_is_Zp(save_ring->cf->extRing->cf))
        nMap=nMapZpa2Zp;
      else
      {
        SI_RESTORE_OPT1(save_opt);
        return NULL;
      }
    }
    else
    {
    */
      SI_RESTORE_OPT1(save_opt);
      rDelete(Zp_ring);
      return NULL;
  }
  rChangeCurrRing(Zp_ring);
  ideal FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  ideal QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  // compute GB in Zp_ring
  si_opt_1&= ~Sy_bit(OPT_REDSB);
  si_opt_1&= ~Sy_bit(OPT_REDTAIL);
  if(TEST_OPT_PROT) Print("std in char. %d ------------------\n",prim);
  ideal GB=kStd_internal64(FF,QQ,(tHomog)TRUE,NULL,NULL,0,0,hdegree,NULL);
  if (GB==NULL)
  {
    rChangeCurrRing(save_ring);
    id_Delete(&FF,Zp_ring);
    if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
    rDelete(Zp_ring);
    SI_RESTORE_OPT1(save_opt);
    return NULL;
  }
  // compute hilb
  ring hilbRing=hHilbertSeriesRing();
  poly hilb=hFirstSeries0p64(GB,QQ,hdegree,Zp_ring,hilbRing);
  // clean up Zp_ring
  rChangeCurrRing(save_ring);
  id_Delete(&GB,Zp_ring);
  id_Delete(&FF,Zp_ring);
  if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
  rDelete(Zp_ring);
  if (hilb==NULL)
  {
    SI_RESTORE_OPT1(save_opt);
    return NULL;
  }
  // std with hilb
  intvec *w=NULL;
  if(TEST_OPT_PROT) PrintS("stdhilb in basering  ------------------\n");
  SI_RESTORE_OPT1(save_opt);
  ideal result=kStdPoly64(F,Q,(tHomog)TRUE,&w,hilb,hilbRing,0,0,hdegree,NULL);
  if (w!=NULL) delete w;
  p_Delete(&hilb,hilbRing);
  return result;
}

static ideal kTryHilbstd_nonhomog(ideal F, ideal Q, int64vec* hdegree)
{
  assume(hdegree != NULL);
  int prim=kFindLuckyPrime(F,Q);
  //if(nCoeff_is_transExt(save_ring->cf)
  //&&(nCoeff_is_Zp(save_ring->cf->extRing->cf)))
  //  prim=save_ring->cf->extRing->cf->ch;
  if(nCoeff_is_Zp(currRing->cf))
    prim=currRing->cf->ch;
  if(TEST_OPT_PROT) Print("std in char. %d, homogenized ------------------\n",prim);
  // create Zp_ring, need 1 more variable
  ring save_ring=currRing;
  BITSET save_opt;SI_SAVE_OPT1(save_opt);
  coeffs cf=nInitChar(n_Zp, (void*)(long)prim);
  char **names=(char**)omAlloc0((currRing->N+1) * sizeof(char *));
  for(int i=0;i<currRing->N;i++)
  {
    names[i]=omStrDup(currRing->names[i]);
  }
  names[currRing->N]=omStrDup("@");
  ring Zp_ring=kHilbstdWideDpRing(cf,save_ring->N+1,names);
  int64vec* homDegree=kHilbstdHomogenizingWeights(hdegree);
  // map data
  nMapFunc nMap=n_SetMap(save_ring->cf,Zp_ring->cf);
  if (nMap==NULL)
  {
    delete homDegree;
    SI_RESTORE_OPT1(save_opt);
    rDelete(Zp_ring);
    return NULL;
  }
  rChangeCurrRing(Zp_ring);
  ideal FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  ideal QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  // homogenize
  ideal tmp=kHilbstdHomogenIdealW(FF,Zp_ring->N,homDegree,Zp_ring);
  id_Delete(&FF,Zp_ring);
  if (tmp==NULL)
  {
    if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
    rChangeCurrRing(save_ring);
    delete homDegree;
    SI_RESTORE_OPT1(save_opt);
    rDelete(Zp_ring);
    return NULL;
  }
  FF=tmp;
  if (QQ!=NULL)
  {
    tmp=kHilbstdHomogenIdealW(QQ,Zp_ring->N,homDegree,Zp_ring);
    id_Delete(&QQ,Zp_ring);
    if (tmp==NULL)
    {
      id_Delete(&FF,Zp_ring);
      rChangeCurrRing(save_ring);
      delete homDegree;
      SI_RESTORE_OPT1(save_opt);
      rDelete(Zp_ring);
      return NULL;
    }
    QQ=tmp;
  }
  // compute GB in Zp_ring
  si_opt_1&= ~Sy_bit(OPT_REDSB);
  si_opt_1&= ~Sy_bit(OPT_REDTAIL);
  ideal GB=kStd_internal64(FF,QQ,(tHomog)TRUE,NULL,NULL,0,0,homDegree,NULL);
  if (GB==NULL)
  {
    id_Delete(&FF,Zp_ring);
    if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
    rChangeCurrRing(save_ring);
    delete homDegree;
    SI_RESTORE_OPT1(save_opt);
    rDelete(Zp_ring);
    return NULL;
  }
  // compute hilb
  ring hilbRing=hHilbertSeriesRing();
  poly hilb=hFirstSeries0p64(GB,QQ,homDegree,Zp_ring,hilbRing);
  // clean up Zp_ring
  id_Delete(&GB,Zp_ring);
  id_Delete(&FF,Zp_ring);
  if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
  rChangeCurrRing(save_ring);
  rDelete(Zp_ring);
  if (hilb==NULL)
  {
    delete homDegree;
    SI_RESTORE_OPT1(save_opt);
    return NULL;
  }
  //omFreeBin(Zp_ring,sip_sring_bin);
  // create Q_ring
  cf=nCopyCoeff(save_ring->cf);
  int nblocks=rBlocks(save_ring)+1;
  names=(char**)omAlloc0((save_ring->N+1) * sizeof(char *));
  for(int i=0;i<save_ring->N;i++)
  {
    names[i]=omStrDup(save_ring->names[i]);
  }
  names[save_ring->N]=omStrDup("@");
  rRingOrder_t *order = (rRingOrder_t *) omAlloc(nblocks* sizeof(rRingOrder_t));
  int *block0 = (int *)omAlloc0(nblocks * sizeof(int));
  int *block1 = (int *)omAlloc0(nblocks * sizeof(int));
  int **wvhdl=(int**)omAlloc0(nblocks * sizeof(int *));
  for (int j=0; j<nblocks-1; j++)
  {
    if (save_ring->wvhdl[j]!=NULL)
    {
      #ifdef HAVE_OMALLOC
      wvhdl[j] = (int*) omMemDup(save_ring->wvhdl[j]);
      #else
      {
        int l=save_ring->block1[j]-save_ring->block0[j]+1;
        if (save_ring->order[j]==ringorder_a64) l*=2;
        else if (save_ring->order[j]==ringorder_M) l=l*l;
        else if (save_ring->order[j]==ringorder_am)
        {
          l+=save_ring->wvhdl[j][save_ring->block1[j]-save_ring->block0[j]+1]+1;
        }
        wvhdl[j]=(int*)omalloc(l*sizeof(int));
        memcpy(wvhdl[j],save_ring->wvhdl[j],l*sizeof(int));
      }
      #endif
    }
  }
  memcpy(order,save_ring->order,(nblocks-1) * sizeof(rRingOrder_t));
  memcpy(block0,save_ring->block0,(nblocks-1) * sizeof(int));
  memcpy(block1,save_ring->block1,(nblocks-1) * sizeof(int));
  order[nblocks-1]=ringorder_lp;
  block0[nblocks-1]=save_ring->N+1;
  block1[nblocks-1]=save_ring->N+1;

  ring Q_ring=rDefault(cf,save_ring->N+1,names,nblocks,order,block0,block1,
                       wvhdl,(unsigned long)LONG_MAX);
  // map data
  nMap=n_SetMap(save_ring->cf,Q_ring->cf);
  if (nMap==NULL)
  {
    delete homDegree;
    p_Delete(&hilb,hilbRing);
    SI_RESTORE_OPT1(save_opt);
    rDelete(Q_ring);
    return NULL;
  }
  rChangeCurrRing(Q_ring);
  FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Q_ring,nMap,NULL,0,0);
  QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Q_ring,nMap,NULL,0,0);
  // homogenize
  if(TEST_OPT_PROT) PrintS("stdhilb in basering, homogenized ------------------\n");
  tmp=kHilbstdHomogenIdealW(FF,Q_ring->N,homDegree,Q_ring);
  id_Delete(&FF,Q_ring);
  if (tmp==NULL)
  {
    if (QQ!=NULL) id_Delete(&QQ,Q_ring);
    rChangeCurrRing(save_ring);
    delete homDegree;
    p_Delete(&hilb,hilbRing);
    SI_RESTORE_OPT1(save_opt);
    rDelete(Q_ring);
    return NULL;
  }
  FF=tmp;
  if (QQ!=NULL)
  {
    tmp=kHilbstdHomogenIdealW(QQ,Q_ring->N,homDegree,Q_ring);
    id_Delete(&QQ,Q_ring);
    if (tmp==NULL)
    {
      id_Delete(&FF,Q_ring);
      rChangeCurrRing(save_ring);
      delete homDegree;
      p_Delete(&hilb,hilbRing);
      SI_RESTORE_OPT1(save_opt);
      rDelete(Q_ring);
      return NULL;
    }
    QQ=tmp;
  }
  // std with hilb
  intvec *w=NULL;
  tmp=kStdPoly64(FF,QQ,(tHomog)TRUE,&w,hilb,hilbRing,0,0,homDegree,NULL);
  if (w!=NULL) delete w;
  delete homDegree;
  p_Delete(&hilb,hilbRing);
  if (tmp==NULL)
  {
    id_Delete(&FF,Q_ring);
    if (QQ!=NULL) id_Delete(&QQ,Q_ring);
    rChangeCurrRing(save_ring);
    SI_RESTORE_OPT1(save_opt);
    rDelete(Q_ring);
    return NULL;
  }
  // dehomogenize
  if(TEST_OPT_PROT) PrintS("de-homogenize, interred ------------------\n");
  poly one=pOne();
  tmp=id_Subst(tmp,Q_ring->N,one,Q_ring);
  p_Delete(&one,Q_ring);
  // map back to save_ring
  rChangeCurrRing(save_ring);
  nMap=n_SetMap(Q_ring->cf,save_ring->cf);
  GB=id_PermIdeal(tmp,1,IDELEMS(tmp),NULL,Q_ring,save_ring,nMap,NULL,0,0);
  // clean up Q_ring
  id_Delete(&FF,Q_ring);
  if (QQ!=NULL) id_Delete(&QQ,Q_ring);
  id_Delete(&tmp,Q_ring);
  rDelete(Q_ring);
  //omFreeBin(Q_ring,sip_sring_bin);
  SI_RESTORE_OPT1(save_opt);
  int dummy;
  if (TEST_OPT_REDSB)
  {
    id_DelDiv(GB,currRing);
    idSkipZeroes(GB);
    ideal GB2=kInterRedBba(GB,currRing->qideal,dummy);
    idDelete(&GB);
    return GB2;
  }
  else
  {
    id_DelDiv(GB,currRing);
    idSkipZeroes(GB);
    return GB;
  }
}

ideal kTryHilbstd(ideal F, ideal Q)
{
 if (rField_is_Ring(currRing)) return NULL;
 if (!rHasGlobalOrdering(currRing)) return NULL;
 if ((!TEST_V_STDHILB) && (!TEST_V_PROBABILISTIC)) return NULL;
 if(TEST_V_PURE_GB) return NULL;

 int64vec* fdegree=kHilbstdPositiveFDegWeights(currRing);
 if (fdegree != NULL)
 {
   if (!kHilbstdWeightsAllOne(fdegree))
   {
     if (id_HomIdealW64(F,Q,fdegree,currRing))
     {
       ideal result=kTryHilbstd_homog(F,Q,fdegree);
       delete fdegree;
       return result;
     }
     if((rField_is_Q(currRing)) || (rField_is_Zp(currRing)))
     {
       ideal result=kTryHilbstd_nonhomog(F,Q,fdegree);
       delete fdegree;
       fdegree=NULL;
       if (result != NULL) return result;
     }
   }
   delete fdegree;
 }

 int64vec* totaldegree=kHilbstdUnitWeights(currRing->N);
 tHomog h = (tHomog)id_HomIdealDP(F,Q,currRing);
 if (h==(tHomog)TRUE)
 {
   ideal result=kTryHilbstd_homog(F,Q,totaldegree);
   delete totaldegree;
   return result;
 }
 if((!rField_is_Q(currRing))
 &&(!rField_is_Zp(currRing))
 )
 {
   delete totaldegree;
   return NULL;
 }
 if (h==(tHomog)FALSE)
 {
   ideal result=kTryHilbstd_nonhomog(F,Q,totaldegree);
   delete totaldegree;
   return result;
 }
 delete totaldegree;
 return NULL;
}

ideal kTryHilbstd_par(ideal F, ideal Q, tHomog h, intvec ** mw)
{
  int cpus = (long) feOptValue(FE_OPT_CPUS);
  if (cpus<1)
  {
    //WerrorS("no sub-processes allowed");
    return NULL;
  }
#if 0
  if(!TEST_V_PURE_GB)
  {
    int cp_std[2];
    int cp_hstd[2];
    int err1=pipe(cp_std);// [0] is read , [1] is write
    int err2=pipe(cp_hstd);
    if (err1||err2)
    {
      Werror("pipe failed with %d\n",errno);
      si_close(cp_std[0]);
      si_close(cp_std[1]);
      si_close(cp_hstd[0]);
      si_close(cp_hstd[1]);
      return NULL;
    }
    pid_t pid_std=fork();
    if (pid_std==0) /*child std*/
    {
      si_set_signal(SIGTERM,sig_term_hdl_child);
      si_close(cp_std[0]);
      si_close(cp_hstd[0]);
      si_close(cp_hstd[1]);
      ssiInfo d;
      memset(&d,0,sizeof(d));
      d.f_write=fdopen(cp_std[1],"w");
      d.fd_write=cp_std[1];
      d.r=currRing;
      si_opt_2|=Sy_bit(V_PURE_GB);
      ideal res=kStd_internal(F,Q,h,mw);
      ssiWriteIdeal(&d,IDEAL_CMD,res);
      fclose(d.f_write);
      _exit(0);
    }
    pid_t pid_hstd=fork();
    if (pid_hstd==0) /*child hstd*/
    {
      si_set_signal(SIGTERM,sig_term_hdl_child);
      si_close(cp_hstd[0]);
      si_close(cp_std[0]);
      si_close(cp_std[1]);
      ssiInfo d;
      memset(&d,0,sizeof(d));
      d.f_write=fdopen(cp_hstd[1],"w");
      d.fd_write=cp_hstd[1];
      d.r=currRing;

      si_opt_2|=Sy_bit(V_PURE_GB);
      ideal res=kTryHilbstd(F,Q);
      if (res!=NULL)
      {
        ssiWriteIdeal(&d,IDEAL_CMD,res);
      }
      fclose(d.f_write);
      _exit(0);
    }
    /*parent*/
    si_close(cp_std[1]);
    si_close(cp_hstd[1]);
  #ifdef HAVE_POLL
    pollfd pfd[2];
    pfd[0].fd=cp_std[0];
    pfd[0].events=POLLIN;
    pfd[1].fd=cp_hstd[0];
    pfd[1].events=POLLIN;
    int s=si_poll(pfd,2,-1); // wait infinite
    ideal res;
    ssiInfo d;
    memset(&d,0,sizeof(d));
    d.r=currRing;
    if (s==1) //std
    {
      d.f_read=s_open(cp_std[0]);
      d.fd_read=cp_std[0];
      res=ssiReadIdeal(&d);
      si_close(cp_hstd[0]);
      s_close(d.f_read);
      si_close(cp_std[0]);
      kill(pid_hstd,SIGTERM);
      si_waitpid(pid_std,NULL,0);
      si_waitpid(pid_hstd,NULL,0);
    }
    else if(s==2)
    {
      d.f_read=s_open(cp_hstd[0]);
      d.fd_read=cp_hstd[0];
      res=ssiReadIdeal(&d);
      si_close(cp_std[0]);
      s_close(d.f_read);
      si_close(cp_hstd[0]);
      kill(pid_std,SIGTERM);
      si_waitpid(pid_hstd,NULL,0);
      si_waitpid(pid_std,NULL,0);
    }
    return res;
  #endif
  }
#endif
  return NULL;
}
