/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: wrapper to try stdhild within std
*/


#include "kernel/mod2.h"

#include "coeffs/bigintmat.h"
#include "misc/options.h"
#include "misc/intvec.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "kernel/ideals.h"
#include "kernel/combinatorics/hilb.h"
#include "kernel/combinatorics/stairc.h"
#include "Singular/ipid.h"

static int kFindLuckyPrime(ideal F, ideal Q) // TODO
{
  int prim=32003;
  // assume coeff are in Q
  return prim;
}

poly kTryHC(ideal F, ideal Q)
{
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
  strat->kHomW=kHomW=NULL;
  strat->homog = (tHomog)idHomIdeal(F,Q);
  ideal res=mora(FF,QQ,NULL,NULL,strat);
  // clean
  idDelete(&FF);
  poly HC=NULL;
  if (strat->kNoether!=NULL) scComputeHC(res,QQ,0,HC);
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
      if (pGetExp(HC, i) > 0) pDecrExp(HC,i);
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

static ideal kTryHilbstd_homog(ideal F, ideal Q)
{
  // create Zp_ring
  ring save_ring=currRing;
  int prim=kFindLuckyPrime(F,Q);
  coeffs cf=nInitChar(n_Zp, (void*)(long)prim);
  ring Zp_ring=rDefault(cf,save_ring->N,save_ring->names,ringorder_dp);
  // map data
  nMapFunc nMap=n_SetMap(save_ring->cf,Zp_ring->cf);
  if (nMap==NULL) return NULL;
  rChangeCurrRing(Zp_ring);
  ideal FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  ideal QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  // compute GB in Zp_ring
  if(TEST_OPT_PROT) PrintS("std in charp  ------------------\n");
  ideal GB=kStd(FF,QQ,(tHomog)TRUE,NULL,NULL,0,0,NULL,NULL);
  // compute hilb
  bigintmat* hilb=hFirstSeries0b(GB,QQ,NULL,NULL,Zp_ring,coeffs_BIGINT);
  // clean up Zp_ring
  rChangeCurrRing(save_ring);
  id_Delete(&GB,Zp_ring);
  id_Delete(&FF,Zp_ring);
  if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
  rDelete(Zp_ring);
  // std with hilb
  intvec *w=NULL;
  if(TEST_OPT_PROT) PrintS("stdhilb in basering  ------------------\n");
  ideal result=kStd(F,Q,(tHomog)TRUE,&w,hilb);
  if (w!=NULL) delete w;
  delete hilb;
  return result;
}

static ideal kTryHilbstd_nonhomog(ideal F, ideal Q)
{
  if(TEST_OPT_PROT) PrintS("std in charp, homogenized ------------------\n");
  // create Zp_ring, need 1 more variable
  ring save_ring=currRing;
  coeffs cf=nInitChar(n_Zp, (void*)(long)32003);
  char **names=(char**)omAlloc0((currRing->N+1) * sizeof(char *));
  for(int i=0;i<currRing->N;i++)
  {
    names[i]=omStrDup(currRing->names[i]);
  }
  names[currRing->N]=omStrDup("@");
  ring Zp_ring=rDefault(cf,save_ring->N+1,names,ringorder_dp);
  // map data
  nMapFunc nMap=n_SetMap(save_ring->cf,Zp_ring->cf);
  if (nMap==NULL) return NULL;
  rChangeCurrRing(Zp_ring);
  ideal FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  ideal QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Zp_ring,nMap,NULL,0,0);
  // homogenize
  ideal tmp=id_HomogenDP(FF,Zp_ring->N,Zp_ring);
  id_Delete(&FF,Zp_ring);
  FF=tmp;
  if (QQ!=NULL)
  {
    tmp=id_HomogenDP(QQ,Zp_ring->N,Zp_ring);
    id_Delete(&QQ,Zp_ring);
    QQ=tmp;
  }
  // compute GB in Zp_ring
  ideal GB=kStd(FF,QQ,(tHomog)TRUE,NULL,NULL,0,0,NULL,NULL);
  // compute hilb
  bigintmat* hilb=hFirstSeries0b(GB,QQ,NULL,NULL,Zp_ring,coeffs_BIGINT);
  // clean up Zp_ring
  id_Delete(&GB,Zp_ring);
  id_Delete(&FF,Zp_ring);
  if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
  rChangeCurrRing(save_ring);
  rDelete(Zp_ring);
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

  ring Q_ring=rDefault(cf,save_ring->N+1,names,nblocks,order,block0,block1,wvhdl,save_ring->wanted_maxExp);
  // map data
  nMap=n_SetMap(save_ring->cf,Q_ring->cf);
  if (nMap==NULL) return NULL;
  rChangeCurrRing(Q_ring);
  FF=id_PermIdeal(F,1,IDELEMS(F),NULL,save_ring,Q_ring,nMap,NULL,0,0);
  QQ=NULL;
  if (Q!=NULL) QQ=id_PermIdeal(Q,1,IDELEMS(Q),NULL,save_ring,Q_ring,nMap,NULL,0,0);
  // homogenize
  if(TEST_OPT_PROT) PrintS("stdhilb in basering, homogenized ------------------\n");
  tmp=id_HomogenDP(FF,Q_ring->N,Q_ring);
  id_Delete(&FF,Q_ring);
  FF=tmp;
  if (QQ!=NULL)
  {
    tmp=id_HomogenDP(QQ,Q_ring->N,Q_ring);
    id_Delete(&QQ,Q_ring);
    QQ=tmp;
  }
  // std with hilb
  intvec *w=NULL;
  tmp=kStd(FF,QQ,testHomog,&w,hilb);
  if (w!=NULL) delete w;
  delete hilb;
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
 tHomog h = (tHomog)id_HomIdealDP(F,Q,currRing);
 if (h==(tHomog)TRUE) return kTryHilbstd_homog(F,Q);
 if (h==(tHomog)FALSE) return kTryHilbstd_nonhomog(F,Q);
 return NULL;
}
