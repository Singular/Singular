/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: wrapper to try stdhild within std
*/


#include "kernel/mod2.h"

#include "coeffs/bigintmat.h"
#include "coeffs/longrat.h"
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
  strat->kHomW=kHomW=NULL;
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

static intvec* kHilbstdDeleteWeights(intvec* w)
{
  delete w;
  return NULL;
}

static intvec* kHilbstdUnitWeights(const int n)
{
  intvec* w = new intvec(n);
  for (int i=n-1; i>=0; i--) (*w)[i] = 1;
  return w;
}

static BOOLEAN kHilbstdWeightsAllOne(const intvec* w)
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

static intvec* kHilbstdPositiveFDegWeights(const ring r)
{
  assume(r != NULL);

  if (!kHilbstdSupportedFDeg(r)) return NULL;

  intvec* w = new intvec(rVar(r));
  for (int i=1; i<=rVar(r); i++)
  {
    poly x = p_One(r);
    p_SetExp(x, i, 1, r);
    p_Setm(x, r);
    const long d = r->pFDeg(x, r);
    p_Delete(&x, r);

    if ((d <= 0) || (d > INT_MAX)) return kHilbstdDeleteWeights(w);
    (*w)[i-1] = (int)d;
  }

  return w;
}

static intvec* kHilbstdHomogenizingWeights(const intvec* w)
{
  assume(w != NULL);

  intvec* hw = new intvec(w->length()+1);
  for (int i=w->length()-1; i>=0; i--) (*hw)[i] = (*w)[i];
  (*hw)[w->length()] = 1;
  return hw;
}

static long kHilbstdWeightedDeg(poly p, const intvec* w, const ring r)
{
  assume(p != NULL);
  assume(w != NULL);
  assume(w->length() >= rVar(r));

  long d = 0;
  for (int i=rVar(r); i>0; i--)
  {
    d += (long)p_GetExp(p, i, r) * (long)(*w)[i-1];
  }
  return d;
}

static poly kHilbstdHomogenW(poly p, int varnum, const intvec* w, const ring r)
{
  if (p == NULL) return NULL;
  if ((varnum < 1) || (varnum > rVar(r))) return NULL;
  assume(w != NULL);
  assume(w->length() >= rVar(r));
  assume((*w)[varnum-1] == 1);

  long maxdeg = kHilbstdWeightedDeg(p, w, r);
  for (poly q=pNext(p); q!=NULL; pIter(q))
  {
    const long d = kHilbstdWeightedDeg(q, w, r);
    if (d > maxdeg) maxdeg = d;
  }

  poly q = p_Copy(p, r);
  poly result = NULL;
  while (q != NULL)
  {
    poly qn = pNext(q);
    pNext(q) = NULL;

    const long shift = maxdeg - kHilbstdWeightedDeg(q, w, r);
    if (shift != 0)
    {
      p_AddExp(q, varnum, shift, r);
      p_Setm(q, r);
    }
    result = p_Add_q(result, q, r);
    q = qn;
  }
  return result;
}

static ideal kHilbstdHomogenIdealW(ideal F, int varnum, const intvec* w, const ring r)
{
  ideal H = idInit(IDELEMS(F), F->rank);
  for (int i=IDELEMS(F)-1; i>=0; i--)
  {
    H->m[i] = kHilbstdHomogenW(F->m[i], varnum, w, r);
  }
  return H;
}

static ideal kTryHilbstd_homog(ideal F, ideal Q, intvec* hdegree)
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
  ring Zp_ring=rDefault(cf,save_ring->N,save_ring->names,ringorder_dp);
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
  ideal GB=kStd_internal(FF,QQ,(tHomog)TRUE,NULL,NULL,0,0,hdegree,NULL);
  // compute hilb
  bigintmat* hilb=hFirstSeries0b(GB,QQ,hdegree,NULL,Zp_ring,coeffs_BIGINT);
  // clean up Zp_ring
  rChangeCurrRing(save_ring);
  id_Delete(&GB,Zp_ring);
  id_Delete(&FF,Zp_ring);
  if (QQ!=NULL) id_Delete(&QQ,Zp_ring);
  rDelete(Zp_ring);
  // std with hilb
  intvec *w=NULL;
  if(TEST_OPT_PROT) PrintS("stdhilb in basering  ------------------\n");
  SI_RESTORE_OPT1(save_opt);
  ideal result=kStd_internal(F,Q,(tHomog)TRUE,&w,hilb,0,0,hdegree,NULL);
  if (w!=NULL) delete w;
  delete hilb;
  return result;
}

static ideal kTryHilbstd_nonhomog(ideal F, ideal Q, intvec* hdegree)
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
  ring Zp_ring=rDefault(cf,save_ring->N+1,names,ringorder_dp);
  intvec* homDegree=kHilbstdHomogenizingWeights(hdegree);
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
  FF=tmp;
  if (QQ!=NULL)
  {
    tmp=kHilbstdHomogenIdealW(QQ,Zp_ring->N,homDegree,Zp_ring);
    id_Delete(&QQ,Zp_ring);
    QQ=tmp;
  }
  // compute GB in Zp_ring
  si_opt_1&= ~Sy_bit(OPT_REDSB);
  si_opt_1&= ~Sy_bit(OPT_REDTAIL);
  ideal GB=kStd_internal(FF,QQ,(tHomog)TRUE,NULL,NULL,0,0,homDegree,NULL);
  // compute hilb
  bigintmat* hilb=hFirstSeries0b(GB,QQ,homDegree,NULL,Zp_ring,coeffs_BIGINT);
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
  if (nMap==NULL)
  {
    delete homDegree;
    delete hilb;
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
  FF=tmp;
  if (QQ!=NULL)
  {
    tmp=kHilbstdHomogenIdealW(QQ,Q_ring->N,homDegree,Q_ring);
    id_Delete(&QQ,Q_ring);
    QQ=tmp;
  }
  // std with hilb
  intvec *w=NULL;
  tmp=kStd_internal(FF,QQ,testHomog,&w,hilb,0,0,homDegree,NULL);
  if (w!=NULL) delete w;
  delete homDegree;
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

 intvec* fdegree=kHilbstdPositiveFDegWeights(currRing);
 if (fdegree != NULL)
 {
   if (!kHilbstdWeightsAllOne(fdegree))
   {
     if (id_HomIdealW(F,Q,fdegree,currRing))
     {
       ideal result=kTryHilbstd_homog(F,Q,fdegree);
       delete fdegree;
       return result;
     }
     if((rField_is_Q(currRing)) || (rField_is_Zp(currRing)))
     {
       ideal result=kTryHilbstd_nonhomog(F,Q,fdegree);
       delete fdegree;
       if (result != NULL) return result;
     }
   }
   delete fdegree;
 }

 intvec* totaldegree=kHilbstdUnitWeights(currRing->N);
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
