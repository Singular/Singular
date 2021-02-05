/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - all basic methods to manipulate ideals
*/

/* includes */

#include "kernel/mod2.h"

#include "misc/options.h"
#include "misc/intvec.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"
// #include "coeffs/longrat.h"


#include "polys/monomials/ring.h"
#include "polys/matpol.h"
#include "polys/weight.h"
#include "polys/sparsmat.h"
#include "polys/prCopy.h"
#include "polys/nc/nc.h"


#include "kernel/ideals.h"

#include "kernel/polys.h"

#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/tgb.h"
#include "kernel/GBEngine/syz.h"
#include "Singular/ipshell.h" // iiCallLibProc1
#include "Singular/ipid.h" // ggetid


/* #define WITH_OLD_MINOR */

/*0 implementation*/

/*2
*returns a minimized set of generators of h1
*/
ideal idMinBase (ideal h1)
{
  ideal h2, h3,h4,e;
  int j,k;
  int i,l,ll;
  intvec * wth;
  BOOLEAN homog;
  if(rField_is_Ring(currRing))
  {
      WarnS("minbase applies only to the local or homogeneous case over coefficient fields");
      e=idCopy(h1);
      return e;
  }
  homog = idHomModule(h1,currRing->qideal,&wth);
  if (rHasGlobalOrdering(currRing))
  {
    if(!homog)
    {
      WarnS("minbase applies only to the local or homogeneous case over coefficient fields");
      e=idCopy(h1);
      return e;
    }
    else
    {
      ideal re=kMin_std(h1,currRing->qideal,(tHomog)homog,&wth,h2,NULL,0,3);
      idDelete(&re);
      return h2;
    }
  }
  e=idInit(1,h1->rank);
  if (idIs0(h1))
  {
    return e;
  }
  pEnlargeSet(&(e->m),IDELEMS(e),15);
  IDELEMS(e) = 16;
  h2 = kStd(h1,currRing->qideal,isNotHomog,NULL);
  h3 = idMaxIdeal(1);
  h4=idMult(h2,h3);
  idDelete(&h3);
  h3=kStd(h4,currRing->qideal,isNotHomog,NULL);
  k = IDELEMS(h3);
  while ((k > 0) && (h3->m[k-1] == NULL)) k--;
  j = -1;
  l = IDELEMS(h2);
  while ((l > 0) && (h2->m[l-1] == NULL)) l--;
  for (i=l-1; i>=0; i--)
  {
    if (h2->m[i] != NULL)
    {
      ll = 0;
      while ((ll < k) && ((h3->m[ll] == NULL)
      || !pDivisibleBy(h3->m[ll],h2->m[i])))
        ll++;
      if (ll >= k)
      {
        j++;
        if (j > IDELEMS(e)-1)
        {
          pEnlargeSet(&(e->m),IDELEMS(e),16);
          IDELEMS(e) += 16;
        }
        e->m[j] = pCopy(h2->m[i]);
      }
    }
  }
  idDelete(&h2);
  idDelete(&h3);
  idDelete(&h4);
  if (currRing->qideal!=NULL)
  {
    h3=idInit(1,e->rank);
    h2=kNF(h3,currRing->qideal,e);
    idDelete(&h3);
    idDelete(&e);
    e=h2;
  }
  idSkipZeroes(e);
  return e;
}


ideal idSectWithElim (ideal h1,ideal h2)
// does not destroy h1,h2
{
  if (TEST_OPT_PROT) PrintS("intersect by elimination method\n");
  assume(!idIs0(h1));
  assume(!idIs0(h2));
  assume(IDELEMS(h1)<=IDELEMS(h2));
  assume(id_RankFreeModule(h1,currRing)==0);
  assume(id_RankFreeModule(h2,currRing)==0);
  // add a new variable:
  int j;
  ring origRing=currRing;
  ring r=rCopy0(origRing);
  r->N++;
  r->block0[0]=1;
  r->block1[0]= r->N;
  omFree(r->order);
  r->order=(rRingOrder_t*)omAlloc0(3*sizeof(rRingOrder_t));
  r->order[0]=ringorder_dp;
  r->order[1]=ringorder_C;
  char **names=(char**)omAlloc0(rVar(r) * sizeof(char_ptr));
  for (j=0;j<r->N-1;j++) names[j]=r->names[j];
  names[r->N-1]=omStrDup("@");
  omFree(r->names);
  r->names=names;
  rComplete(r,TRUE);
  // fetch h1, h2
  ideal h;
  h1=idrCopyR(h1,origRing,r);
  h2=idrCopyR(h2,origRing,r);
  // switch to temp. ring r
  rChangeCurrRing(r);
  // create 1-t, t
  poly omt=p_One(currRing);
  p_SetExp(omt,r->N,1,currRing);
  p_Setm(omt,currRing);
  poly t=p_Copy(omt,currRing);
  omt=p_Neg(omt,currRing);
  omt=p_Add_q(omt,pOne(),currRing);
  // compute (1-t)*h1
  h1=(ideal)mp_MultP((matrix)h1,omt,currRing);
  // compute t*h2
  h2=(ideal)mp_MultP((matrix)h2,pCopy(t),currRing);
  // (1-t)h1 + t*h2
  h=idInit(IDELEMS(h1)+IDELEMS(h2),1);
  int l;
  for (l=IDELEMS(h1)-1; l>=0; l--)
  {
    h->m[l] = h1->m[l];  h1->m[l]=NULL;
  }
  j=IDELEMS(h1);
  for (l=IDELEMS(h2)-1; l>=0; l--)
  {
    h->m[l+j] = h2->m[l];  h2->m[l]=NULL;
  }
  idDelete(&h1);
  idDelete(&h2);
  // eliminate t:
  ideal res=idElimination(h,t);
  // cleanup
  idDelete(&h);
  pDelete(&t);
  if (res!=NULL) res=idrMoveR(res,r,origRing);
  rChangeCurrRing(origRing);
  rDelete(r);
  return res;
}
/*2
* h3 := h1 intersect h2
*/
ideal idSect (ideal h1,ideal h2, GbVariant alg)
{
  int i,j,k;
  unsigned length;
  int flength = id_RankFreeModule(h1,currRing);
  int slength = id_RankFreeModule(h2,currRing);
  int rank=si_max(h1->rank,h2->rank);
  if ((idIs0(h1)) || (idIs0(h2)))  return idInit(1,rank);

  BITSET save_opt;
  SI_SAVE_OPT1(save_opt);
  si_opt_1 |= Sy_bit(OPT_REDTAIL_SYZ);

  ideal first,second,temp,temp1,result;
  poly p,q;

  if (IDELEMS(h1)<IDELEMS(h2))
  {
    first = h1;
    second = h2;
  }
  else
  {
    first = h2;
    second = h1;
    int t=flength; flength=slength; slength=t;
  }
  length  = si_max(flength,slength);
  if (length==0)
  {
    if ((currRing->qideal==NULL)
    && (currRing->OrdSgn==1)
    && (!rIsPluralRing(currRing))
    && ((TEST_V_INTERSECT_ELIM) || (!TEST_V_INTERSECT_SYZ)))
      return idSectWithElim(first,second);
    else length = 1;
  }
  if (TEST_OPT_PROT) PrintS("intersect by syzygy methods\n");
  j = IDELEMS(first);

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzOrder(orig_ring,TRUE);
  rSetSyzComp(length,syz_ring);
  rChangeCurrRing(syz_ring);

  while ((j>0) && (first->m[j-1]==NULL)) j--;
  temp = idInit(j /*IDELEMS(first)*/+IDELEMS(second),length+j);
  k = 0;
  for (i=0;i<j;i++)
  {
    if (first->m[i]!=NULL)
    {
      if (syz_ring==orig_ring)
        temp->m[k] = pCopy(first->m[i]);
      else
        temp->m[k] = prCopyR(first->m[i], orig_ring, syz_ring);
      q = pOne();
      pSetComp(q,i+1+length);
      pSetmComp(q);
      if (flength==0) p_Shift(&(temp->m[k]),1,currRing);
      p = temp->m[k];
      while (pNext(p)!=NULL) pIter(p);
      pNext(p) = q;
      k++;
    }
  }
  for (i=0;i<IDELEMS(second);i++)
  {
    if (second->m[i]!=NULL)
    {
      if (syz_ring==orig_ring)
        temp->m[k] = pCopy(second->m[i]);
      else
        temp->m[k] = prCopyR(second->m[i], orig_ring,currRing);
      if (slength==0) p_Shift(&(temp->m[k]),1,currRing);
      k++;
    }
  }
  intvec *w=NULL;
  if (alg==GbDefault) alg=GbStd;
  if (alg==GbStd)
  {
    if (TEST_OPT_PROT) { PrintS("std:"); mflush(); }
    temp1 = kStd(temp,currRing->qideal,testHomog,&w,NULL,length);
    if (w!=NULL) delete w;
    idDelete(&temp);
  }
  else if (alg==GbSlimgb)
  {
    if (TEST_OPT_PROT) { PrintS("slimgb:"); mflush(); }
    temp1 = t_rep_gb(currRing, temp, temp->rank);
    idDelete(&temp);
  }
  else if (alg==GbGroebner)
  {
    if (TEST_OPT_PROT) { PrintS("groebner:"); mflush(); }
    BOOLEAN err;
    temp1=(ideal)iiCallLibProc1("groebner",temp,MODUL_CMD,err);
    if (err)
    {
      Werror("error %d in >>groebner<<",err);
      temp1=idInit(1,1);
    }
  }
  else if (alg==GbModstd)
  {
    if (TEST_OPT_PROT) { PrintS("modStd:"); mflush(); }
    BOOLEAN err;
    void *args[]={temp,(void*)1,NULL};
    int arg_t[]={MODUL_CMD,INT_CMD,0};
    leftv temp0=ii_CallLibProcM("modStd",args,arg_t,currRing,err);
    temp1=(ideal)temp0->data;
    omFreeBin((ADDRESS)temp0,sleftv_bin);
    if (err)
    {
      Werror("error %d in >>modStd<<",err);
      temp1=idInit(1,1);
    }
  }
  else if (alg==GbStdSat)
  {
    if (TEST_OPT_PROT) { PrintS("std:sat:"); mflush(); }
    BOOLEAN err;
    // search for 2nd block of vars
    int i=0;
    int block=-1;
    loop
    {
      if ((currRing->order[i]!=ringorder_c)
      && (currRing->order[i]!=ringorder_C)
      && (currRing->order[i]!=ringorder_s))
      {
        if (currRing->order[i]==0) { err=TRUE;break;}
        block++;
        if (block==1) { block=i; break;}
      }
      i++;
    }
    if (block>0)
    {
      if (TEST_OPT_PROT)
      {
        Print("sat(%d..%d)\n",currRing->block0[block],currRing->block1[block]);
        mflush();
      }
      ideal v=idInit(currRing->block1[block]-currRing->block0[block]+1,1);
      for(i=currRing->block0[block];i<=currRing->block1[block];i++)
      {
        v->m[i-currRing->block0[block]]=pOne();
        pSetExp(v->m[i-currRing->block0[block]],i,1);
        pSetm(v->m[i-currRing->block0[block]]);
      }
      void *args[]={temp,v,NULL};
      int arg_t[]={MODUL_CMD,IDEAL_CMD,0};
      leftv temp0=ii_CallLibProcM("satstd",args,arg_t,currRing,err);
      temp1=(ideal)temp0->data;
      omFreeBin((ADDRESS)temp0, sleftv_bin);
    }
    if (err)
    {
      Werror("error %d in >>satstd<<",err);
      temp1=idInit(1,1);
    }
  }

  if(syz_ring!=orig_ring)
    rChangeCurrRing(orig_ring);

  result = idInit(IDELEMS(temp1),rank);
  j = 0;
  for (i=0;i<IDELEMS(temp1);i++)
  {
    if ((temp1->m[i]!=NULL)
    && (__p_GetComp(temp1->m[i],syz_ring)>length))
    {
      if(syz_ring==orig_ring)
      {
        p = temp1->m[i];
      }
      else
      {
        p = prMoveR(temp1->m[i], syz_ring,orig_ring);
      }
      temp1->m[i]=NULL;
      while (p!=NULL)
      {
        q = pNext(p);
        pNext(p) = NULL;
        k = pGetComp(p)-1-length;
        pSetComp(p,0);
        pSetmComp(p);
        /* Warning! multiply only from the left! it's very important for Plural */
        result->m[j] = pAdd(result->m[j],pMult(p,pCopy(first->m[k])));
        p = q;
      }
      j++;
    }
  }
  if(syz_ring!=orig_ring)
  {
    rChangeCurrRing(syz_ring);
    idDelete(&temp1);
    rChangeCurrRing(orig_ring);
    rDelete(syz_ring);
  }
  else
  {
    idDelete(&temp1);
  }

  idSkipZeroes(result);
  SI_RESTORE_OPT1(save_opt);
  if (TEST_OPT_RETURN_SB)
  {
     w=NULL;
     temp1=kStd(result,currRing->qideal,testHomog,&w);
     if (w!=NULL) delete w;
     idDelete(&result);
     idSkipZeroes(temp1);
     return temp1;
  }
  else //temp1=kInterRed(result,currRing->qideal);
    return result;
}

/*2
* ideal/module intersection for a list of objects
* given as 'resolvente'
*/
ideal idMultSect(resolvente arg, int length, GbVariant alg)
{
  int i,j=0,k=0,l,maxrk=-1,realrki;
  unsigned syzComp;
  ideal bigmat,tempstd,result;
  poly p;
  int isIdeal=0;
  intvec * w=NULL;

  /* find 0-ideals and max rank -----------------------------------*/
  for (i=0;i<length;i++)
  {
    if (!idIs0(arg[i]))
    {
      realrki=id_RankFreeModule(arg[i],currRing);
      k++;
      j += IDELEMS(arg[i]);
      if (realrki>maxrk) maxrk = realrki;
    }
    else
    {
      if (arg[i]!=NULL)
      {
        return idInit(1,arg[i]->rank);
      }
    }
  }
  if (maxrk == 0)
  {
    isIdeal = 1;
    maxrk = 1;
  }
  /* init -----------------------------------------------------------*/
  j += maxrk;
  syzComp = k*maxrk;

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzOrder(orig_ring,TRUE);
  rSetSyzComp(syzComp,syz_ring);
  rChangeCurrRing(syz_ring);

  bigmat = idInit(j,(k+1)*maxrk);
  /* create unit matrices ------------------------------------------*/
  for (i=0;i<maxrk;i++)
  {
    for (j=0;j<=k;j++)
    {
      p = pOne();
      pSetComp(p,i+1+j*maxrk);
      pSetmComp(p);
      bigmat->m[i] = pAdd(bigmat->m[i],p);
    }
  }
  /* enter given ideals ------------------------------------------*/
  i = maxrk;
  k = 0;
  for (j=0;j<length;j++)
  {
    if (arg[j]!=NULL)
    {
      for (l=0;l<IDELEMS(arg[j]);l++)
      {
        if (arg[j]->m[l]!=NULL)
        {
          if (syz_ring==orig_ring)
            bigmat->m[i] = pCopy(arg[j]->m[l]);
          else
            bigmat->m[i] = prCopyR(arg[j]->m[l], orig_ring,currRing);
          p_Shift(&(bigmat->m[i]),k*maxrk+isIdeal,currRing);
          i++;
        }
      }
      k++;
    }
  }
  /* std computation --------------------------------------------*/
  if (alg==GbDefault) alg=GbStd;
  if (alg==GbStd)
  {
    if (TEST_OPT_PROT) { PrintS("std:"); mflush(); }
    tempstd = kStd(bigmat,currRing->qideal,testHomog,&w,NULL,syzComp);
    if (w!=NULL) delete w;
    idDelete(&bigmat);
  }
  else if (alg==GbSlimgb)
  {
    if (TEST_OPT_PROT) { PrintS("slimgb:"); mflush(); }
    tempstd = t_rep_gb(currRing, bigmat, syzComp);
    idDelete(&bigmat);
  }
  else if (alg==GbGroebner)
  {
    if (TEST_OPT_PROT) { PrintS("groebner:"); mflush(); }
    BOOLEAN err;
    tempstd=(ideal)iiCallLibProc1("groebner",bigmat,MODUL_CMD,err);
    if (err)
    {
      Werror("error %d in >>groebner<<",err);
      tempstd=idInit(1,1);
    }
  }
//  else if (alg==GbModstd): requires ideal, not module
//  {
//    if (TEST_OPT_PROT) { PrintS("modstd:"); mflush(); }
//    BOOLEAN err;
//    tempstd=(ideal)iiCallLibProc1("modStd",bigmat,MODUL_CMD,err);
//    if (err)
//    {
//      Werror("error %d in >>modStd<<",err);
//      tempstd=idInit(1,1);
//    }
//  }
  //else if (alg==GbSba): requires order C,...
  //{
  //  if (TEST_OPT_PROT) { PrintS("sba:"); mflush(); }
  //  tempstd = kSba(bigmat,currRing->qideal,hom,w,1,0,NULL,syzComp);
  //  idDelete(&bigmat);
  //}
  else
  {
    tempstd=idInit(1,1);
    Werror("wrong algorithm %d for SB",(int)alg);
  }

  if(syz_ring!=orig_ring)
    rChangeCurrRing(orig_ring);

  /* interprete result ----------------------------------------*/
  result = idInit(IDELEMS(tempstd),maxrk);
  k = 0;
  for (j=0;j<IDELEMS(tempstd);j++)
  {
    if ((tempstd->m[j]!=NULL) && (__p_GetComp(tempstd->m[j],syz_ring)>syzComp))
    {
      if (syz_ring==orig_ring)
        p = pCopy(tempstd->m[j]);
      else
        p = prCopyR(tempstd->m[j], syz_ring,currRing);
      p_Shift(&p,-syzComp-isIdeal,currRing);
      result->m[k] = p;
      k++;
    }
  }
  /* clean up ----------------------------------------------------*/
  if(syz_ring!=orig_ring)
    rChangeCurrRing(syz_ring);
  idDelete(&tempstd);
  if(syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    rDelete(syz_ring);
  }
  idSkipZeroes(result);
  return result;
}

/*2
*computes syzygies of h1,
*if quot != NULL it computes in the quotient ring modulo "quot"
*works always in a ring with ringorder_s
*/
static ideal idPrepare (ideal  h1, tHomog hom, int syzcomp, intvec **w, GbVariant alg)
{
  ideal   h2;
  int     j,k;
  poly    p,q;

  if (idIs0(h1)) return NULL;
  k = id_RankFreeModule(h1,currRing);
  h2=idCopy(h1);
  int i = IDELEMS(h2);
  if (k == 0)
  {
    id_Shift(h2,1,currRing);
    k = 1;
  }
  if (syzcomp<k)
  {
    Warn("syzcomp too low, should be %d instead of %d",k,syzcomp);
    syzcomp = k;
    rSetSyzComp(k,currRing);
  }
  h2->rank = syzcomp+i;

  //if (hom==testHomog)
  //{
  //  if(idHomIdeal(h1,currRing->qideal))
  //  {
  //    hom=TRUE;
  //  }
  //}

  for (j=0; j<i; j++)
  {
    p = h2->m[j];
    q = pOne();
#ifdef HAVE_SHIFTBBA
    // non multiplicative variable
    if (rIsLPRing(currRing))
    {
      pSetExp(q, currRing->isLPring - currRing->LPncGenCount + j + 1, 1);
      p_Setm(q, currRing);
    }
#endif
    pSetComp(q,syzcomp+1+j);
    pSetmComp(q);
    if (p!=NULL)
    {
#ifdef HAVE_SHIFTBBA
      if (rIsLPRing(currRing))
      {
        h2->m[j] = pAdd(p, q);
      }
      else
#endif
      {
        while (pNext(p)) pIter(p);
        p->next = q;
      }
    }
    else
      h2->m[j]=q;
  }

  idTest(h2);

#ifdef HAVE_SHIFTBBA
  if (rIsLPRing(currRing)) alg = GbStd;
#endif
  ideal h3=NULL;
  if (alg==GbDefault) alg=GbStd;
  if (alg==GbStd)
  {
    if (TEST_OPT_PROT) { PrintS("std:"); mflush(); }
    h3 = kStd(h2,currRing->qideal,hom,w,NULL,syzcomp);
  }
  else if (alg==GbSlimgb)
  {
    if (TEST_OPT_PROT) { PrintS("slimgb:"); mflush(); }
    h3 = t_rep_gb(currRing, h2, syzcomp);
  }
  else if (alg==GbGroebner)
  {
    if (TEST_OPT_PROT) { PrintS("groebner:"); mflush(); }
    BOOLEAN err;
    h3=(ideal)iiCallLibProc1("groebner",idCopy(h2),MODUL_CMD,err);
    if (err)
    {
      Werror("error %d in >>groebner<<",err);
      h3=idInit(1,1);
    }
  }
  else if (alg==GbModstd)
  {
    if (TEST_OPT_PROT) { PrintS("modstd:"); mflush(); }
    BOOLEAN err;
    void *args[]={idCopy(h2),(void*)1,NULL};
    int arg_t[]={MODUL_CMD,INT_CMD,0};
    leftv temp0=ii_CallLibProcM("modStd",args,arg_t,currRing,err);
    h3=(ideal)temp0->data;
    omFreeBin((ADDRESS)temp0,sleftv_bin);
    if (err)
    {
      Werror("error %d in >>modStd<<",err);
      h3=idInit(1,1);
    }
  }
  else if (alg==GbStdSat)
  {
    if (TEST_OPT_PROT) { PrintS("std:sat:"); mflush(); }
    BOOLEAN err;
    // search for 2nd block of vars
    int i=0;
    int block=-1;
    loop
    {
      if ((currRing->order[i]!=ringorder_c)
      && (currRing->order[i]!=ringorder_C)
      && (currRing->order[i]!=ringorder_s))
      {
        if (currRing->order[i]==0) { err=TRUE;break;}
        block++;
        if (block==1) { block=i; break;}
      }
      i++;
    }
    if (block>0)
    {
      if (TEST_OPT_PROT)
      {
        Print("sat(%d..%d)\n",currRing->block0[block],currRing->block1[block]);
        mflush();
      }
      ideal v=idInit(currRing->block1[block]-currRing->block0[block]+1,1);
      for(i=currRing->block0[block];i<=currRing->block1[block];i++)
      {
        v->m[i-currRing->block0[block]]=pOne();
        pSetExp(v->m[i-currRing->block0[block]],i,1);
        pSetm(v->m[i-currRing->block0[block]]);
      }
      void *args[]={idCopy(h2),v,NULL};
      int arg_t[]={MODUL_CMD,IDEAL_CMD,0};
      leftv temp0=ii_CallLibProcM("satstd",args,arg_t,currRing,err);
      h3=(ideal)temp0->data;
      omFreeBin((ADDRESS)temp0,sleftv_bin);
    }
    if (err)
    {
      Werror("error %d in >>satstd<<",err);
      h3=idInit(1,1);
    }
  }
  //else if (alg==GbSba): requires order C,...
  //{
  //  if (TEST_OPT_PROT) { PrintS("sba:"); mflush(); }
  //  h3 = kSba(h2,currRing->qideal,hom,w,1,0,NULL,syzcomp);
  //}
  else
  {
    h3=idInit(1,1);
    Werror("wrong algorithm %d for SB",(int)alg);
  }

  idDelete(&h2);
  return h3;
}

/*2
* compute the syzygies of h1 in R/quot,
* weights of components are in w
* if setRegularity, return the regularity in deg
* do not change h1,  w
*/
ideal idSyzygies (ideal  h1, tHomog h,intvec **w, BOOLEAN setSyzComp,
                  BOOLEAN setRegularity, int *deg, GbVariant alg)
{
  ideal s_h1;
  int   j, k, length=0,reg;
  BOOLEAN isMonomial=TRUE;
  int ii, idElemens_h1;

  assume(h1 != NULL);

  idElemens_h1=IDELEMS(h1);
#ifdef PDEBUG
  for(ii=0;ii<idElemens_h1 /*IDELEMS(h1)*/;ii++) pTest(h1->m[ii]);
#endif
  if (idIs0(h1))
  {
    ideal result=idFreeModule(idElemens_h1/*IDELEMS(h1)*/);
    return result;
  }
  int slength=(int)id_RankFreeModule(h1,currRing);
  k=si_max(1,slength /*id_RankFreeModule(h1)*/);

  assume(currRing != NULL);
  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  if (setSyzComp) rSetSyzComp(k,syz_ring);

  if (orig_ring != syz_ring)
  {
    rChangeCurrRing(syz_ring);
    s_h1=idrCopyR_NoSort(h1,orig_ring,syz_ring);
  }
  else
  {
    s_h1 = h1;
  }

  idTest(s_h1);

  BITSET save_opt;
  SI_SAVE_OPT1(save_opt);
  si_opt_1|=Sy_bit(OPT_REDTAIL_SYZ);

  ideal s_h3=idPrepare(s_h1,h,k,w,alg); // main (syz) GB computation

  SI_RESTORE_OPT1(save_opt);

  if (s_h3==NULL)
  {
    if (orig_ring != syz_ring)
    {
      rChangeCurrRing(orig_ring);
      rDelete(syz_ring);
    }
    return idFreeModule( idElemens_h1 /*IDELEMS(h1)*/);
  }

  if (orig_ring != syz_ring)
  {
    idDelete(&s_h1);
    for (j=0; j<IDELEMS(s_h3); j++)
    {
      if (s_h3->m[j] != NULL)
      {
        if (p_MinComp(s_h3->m[j],syz_ring) > k)
          p_Shift(&s_h3->m[j], -k,syz_ring);
        else
          p_Delete(&s_h3->m[j],syz_ring);
      }
    }
    idSkipZeroes(s_h3);
    s_h3->rank -= k;
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rDelete(syz_ring);
    #ifdef HAVE_PLURAL
    if (rIsPluralRing(orig_ring))
    {
      id_DelMultiples(s_h3,orig_ring);
      idSkipZeroes(s_h3);
    }
    #endif
    idTest(s_h3);
    return s_h3;
  }

  ideal e = idInit(IDELEMS(s_h3), s_h3->rank);

  for (j=IDELEMS(s_h3)-1; j>=0; j--)
  {
    if (s_h3->m[j] != NULL)
    {
      if (p_MinComp(s_h3->m[j],syz_ring) <= k)
      {
        e->m[j] = s_h3->m[j];
        isMonomial=isMonomial && (pNext(s_h3->m[j])==NULL);
        p_Delete(&pNext(s_h3->m[j]),syz_ring);
        s_h3->m[j] = NULL;
      }
    }
  }

  idSkipZeroes(s_h3);
  idSkipZeroes(e);

  if ((deg != NULL)
  && (!isMonomial)
  && (!TEST_OPT_NOTREGULARITY)
  && (setRegularity)
  && (h==isHomog)
  && (!rIsPluralRing(currRing))
  && (!rField_is_Ring(currRing))
  )
  {
    assume(orig_ring==syz_ring);
    ring dp_C_ring = rAssure_dp_C(syz_ring); // will do rChangeCurrRing later
    if (dp_C_ring != syz_ring)
    {
      rChangeCurrRing(dp_C_ring);
      e = idrMoveR_NoSort(e, syz_ring, dp_C_ring);
    }
    resolvente res = sySchreyerResolvente(e,-1,&length,TRUE, TRUE);
    intvec * dummy = syBetti(res,length,&reg, *w);
    *deg = reg+2;
    delete dummy;
    for (j=0;j<length;j++)
    {
      if (res[j]!=NULL) idDelete(&(res[j]));
    }
    omFreeSize((ADDRESS)res,length*sizeof(ideal));
    idDelete(&e);
    if (dp_C_ring != orig_ring)
    {
      rChangeCurrRing(orig_ring);
      rDelete(dp_C_ring);
    }
  }
  else
  {
    idDelete(&e);
  }
  assume(orig_ring==currRing);
  idTest(s_h3);
  if (currRing->qideal != NULL)
  {
    ideal ts_h3=kStd(s_h3,currRing->qideal,h,w);
    idDelete(&s_h3);
    s_h3 = ts_h3;
  }
  return s_h3;
}

/*
*computes a standard basis for h1 and stores the transformation matrix
* in ma
*/
ideal idLiftStd (ideal  h1, matrix* ma, tHomog hi, ideal * syz, GbVariant alg)
{
  int  i, j, t, inputIsIdeal=id_RankFreeModule(h1,currRing);
  long k;
  poly  p=NULL, q;
  intvec *w=NULL;

  idDelete((ideal*)ma);
  BOOLEAN lift3=FALSE;
  if (syz!=NULL) { lift3=TRUE; idDelete(syz); }
  if (idIs0(h1))
  {
    *ma=mpNew(1,0);
    if (lift3)
    {
      *syz=idFreeModule(IDELEMS(h1));
    }
    return idInit(1,h1->rank);
  }

  BITSET save2;
  SI_SAVE_OPT2(save2);

  k=si_max(1,inputIsIdeal);

  if ((!lift3)&&(!TEST_OPT_RETURN_SB)) si_opt_2 |=Sy_bit(V_IDLIFT);

  ring orig_ring = currRing;
  ring syz_ring = rAssure_SyzOrder(orig_ring,TRUE);
  rSetSyzComp(k,syz_ring);
  rChangeCurrRing(syz_ring);

  ideal s_h1=h1;

  if (orig_ring != syz_ring)
    s_h1 = idrCopyR_NoSort(h1,orig_ring,syz_ring);
  else
    s_h1 = h1;

  ideal s_h3=idPrepare(s_h1,hi,k,&w,alg); // main (syz) GB computation

  ideal s_h2 = idInit(IDELEMS(s_h3), s_h3->rank);

  if (lift3) (*syz)=idInit(IDELEMS(s_h3),IDELEMS(h1));

  if (w!=NULL) delete w;
  i = 0;

  // now sort the result, SB : leave in s_h3
  //                      T:  put in s_h2
  //                      syz: put in *syz
  for (j=0; j<IDELEMS(s_h3); j++)
  {
    if (s_h3->m[j] != NULL)
    {
      //if (p_MinComp(s_h3->m[j],syz_ring) <= k)
      if (pGetComp(s_h3->m[j]) <= k) // syz_ring == currRing
      {
        i++;
        q = s_h3->m[j];
        while (pNext(q) != NULL)
        {
          if (pGetComp(pNext(q)) > k)
          {
            s_h2->m[j] = pNext(q);
            pNext(q) = NULL;
          }
          else
          {
            pIter(q);
          }
        }
        if (!inputIsIdeal) p_Shift(&(s_h3->m[j]), -1,currRing);
      }
      else
      {
        // we a syzygy here:
        if (lift3)
        {
          p_Shift(&s_h3->m[j], -k,currRing);
          (*syz)->m[j]=s_h3->m[j];
          s_h3->m[j]=NULL;
        }
        else
          p_Delete(&(s_h3->m[j]),currRing);
      }
    }
  }
  idSkipZeroes(s_h3);
  //extern char * iiStringMatrix(matrix im, int dim,char ch);
  //PrintS("SB: ----------------------------------------\n");
  //PrintS(iiStringMatrix((matrix)s_h3,k,'\n'));
  //PrintLn();
  //PrintS("T: ----------------------------------------\n");
  //PrintS(iiStringMatrix((matrix)s_h2,h1->rank,'\n'));
  //PrintLn();

  if (lift3) idSkipZeroes(*syz);

  j = IDELEMS(s_h1);


  if (syz_ring!=orig_ring)
  {
    idDelete(&s_h1);
    rChangeCurrRing(orig_ring);
  }

  *ma = mpNew(j,i);

  i = 1;
  for (j=0; j<IDELEMS(s_h2); j++)
  {
    if (s_h2->m[j] != NULL)
    {
      q = prMoveR( s_h2->m[j], syz_ring,orig_ring);
      s_h2->m[j] = NULL;

      if (q!=NULL)
      {
        q=pReverse(q);
        while (q != NULL)
        {
          p = q;
          pIter(q);
          pNext(p) = NULL;
          t=pGetComp(p);
          pSetComp(p,0);
          pSetmComp(p);
          MATELEM(*ma,t-k,i) = pAdd(MATELEM(*ma,t-k,i),p);
        }
      }
      i++;
    }
  }
  idDelete(&s_h2);

  for (i=0; i<IDELEMS(s_h3); i++)
  {
    s_h3->m[i] = prMoveR_NoSort(s_h3->m[i], syz_ring,orig_ring);
  }
  if (lift3)
  {
    for (i=0; i<IDELEMS(*syz); i++)
    {
      (*syz)->m[i] = prMoveR_NoSort((*syz)->m[i], syz_ring,orig_ring);
    }
  }

  if (syz_ring!=orig_ring) rDelete(syz_ring);
  s_h3->rank=h1->rank;
  SI_RESTORE_OPT2(save2);
  return s_h3;
}

static void idPrepareStd(ideal s_temp, int k)
{
  int j,rk=id_RankFreeModule(s_temp,currRing);
  poly p,q;

  if (rk == 0)
  {
    for (j=0; j<IDELEMS(s_temp); j++)
    {
      if (s_temp->m[j]!=NULL) pSetCompP(s_temp->m[j],1);
    }
    k = si_max(k,1);
  }
  for (j=0; j<IDELEMS(s_temp); j++)
  {
    if (s_temp->m[j]!=NULL)
    {
      p = s_temp->m[j];
      q = pOne();
      //pGetCoeff(q)=nInpNeg(pGetCoeff(q));   //set q to -1
      pSetComp(q,k+1+j);
      pSetmComp(q);
#ifdef HAVE_SHIFTBBA
      // non multiplicative variable
      if (rIsLPRing(currRing))
      {
        pSetExp(q, currRing->isLPring - currRing->LPncGenCount + j + 1, 1);
        p_Setm(q, currRing);
        s_temp->m[j] = pAdd(p, q);
      }
      else
#endif
      {
        while (pNext(p)) pIter(p);
        pNext(p) = q;
      }
    }
  }
  s_temp->rank = k+IDELEMS(s_temp);
}

static void idLift_setUnit(int e_mod, matrix *unit)
{
  if (unit!=NULL)
  {
    *unit=mpNew(e_mod,e_mod);
    // make sure that U is a diagonal matrix of units
    for(int i=e_mod;i>0;i--)
    {
      MATELEM(*unit,i,i)=pOne();
    }
  }
}
/*2
*computes a representation of the generators of submod with respect to those
* of mod
*/

ideal idLift(ideal mod, ideal submod,ideal *rest, BOOLEAN goodShape,
             BOOLEAN isSB, BOOLEAN divide, matrix *unit, GbVariant alg)
{
  int lsmod =id_RankFreeModule(submod,currRing), j, k;
  int comps_to_add=0;
  int idelems_mod=IDELEMS(mod);
  int idelems_submod=IDELEMS(submod);
  poly p;

  if (idIs0(submod))
  {
    if (rest!=NULL)
    {
      *rest=idInit(1,mod->rank);
    }
    idLift_setUnit(idelems_submod,unit);
    return idInit(1,idelems_mod);
  }
  if (idIs0(mod)) /* and not idIs0(submod) */
  {
    if (rest!=NULL)
    {
      *rest=idCopy(submod);
      idLift_setUnit(idelems_submod,unit);
      return idInit(1,idelems_mod);
    }
    else
    {
      WerrorS("2nd module does not lie in the first");
      return NULL;
    }
  }
  if (unit!=NULL)
  {
    comps_to_add = idelems_submod;
    while ((comps_to_add>0) && (submod->m[comps_to_add-1]==NULL))
      comps_to_add--;
  }
  k=si_max(id_RankFreeModule(mod,currRing),id_RankFreeModule(submod,currRing));
  if  ((k!=0) && (lsmod==0)) lsmod=1;
  k=si_max(k,(int)mod->rank);
  if (k<submod->rank) { WarnS("rk(submod) > rk(mod) ?");k=submod->rank; }

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzOrder(orig_ring,TRUE);
  rSetSyzComp(k,syz_ring);
  rChangeCurrRing(syz_ring);

  ideal s_mod, s_temp;
  if (orig_ring != syz_ring)
  {
    s_mod = idrCopyR_NoSort(mod,orig_ring,syz_ring);
    s_temp = idrCopyR_NoSort(submod,orig_ring,syz_ring);
  }
  else
  {
    s_mod = mod;
    s_temp = idCopy(submod);
  }
  ideal s_h3;
  if (isSB)
  {
    s_h3 = idCopy(s_mod);
    idPrepareStd(s_h3, k+comps_to_add);
  }
  else
  {
    s_h3 = idPrepare(s_mod,(tHomog)FALSE,k+comps_to_add,NULL,alg);
  }
  if (!goodShape)
  {
    for (j=0;j<IDELEMS(s_h3);j++)
    {
      if ((s_h3->m[j] != NULL) && (pMinComp(s_h3->m[j]) > k))
        p_Delete(&(s_h3->m[j]),currRing);
    }
  }
  idSkipZeroes(s_h3);
  if (lsmod==0)
  {
    id_Shift(s_temp,1,currRing);
  }
  if (unit!=NULL)
  {
    for(j = 0;j<comps_to_add;j++)
    {
      p = s_temp->m[j];
      if (p!=NULL)
      {
        while (pNext(p)!=NULL) pIter(p);
        pNext(p) = pOne();
        pIter(p);
        pSetComp(p,1+j+k);
        pSetmComp(p);
        p = pNeg(p);
      }
    }
    s_temp->rank += (k+comps_to_add);
  }
  ideal s_result = kNF(s_h3,currRing->qideal,s_temp,k);
  s_result->rank = s_h3->rank;
  ideal s_rest = idInit(IDELEMS(s_result),k);
  idDelete(&s_h3);
  idDelete(&s_temp);

  for (j=0;j<IDELEMS(s_result);j++)
  {
    if (s_result->m[j]!=NULL)
    {
      if (pGetComp(s_result->m[j])<=k)
      {
        if (!divide)
        {
          if (rest==NULL)
          {
            if (isSB)
            {
              WarnS("first module not a standardbasis\n"
              "// ** or second not a proper submodule");
            }
            else
              WerrorS("2nd module does not lie in the first");
          }
          idDelete(&s_result);
          idDelete(&s_rest);
          if(syz_ring!=orig_ring)
          {
            idDelete(&s_mod);
            rChangeCurrRing(orig_ring);
            rDelete(syz_ring);
          }
          if (unit!=NULL)
          {
            idLift_setUnit(idelems_submod,unit);
          }
          if (rest!=NULL) *rest=idCopy(submod);
          s_result=idInit(idelems_submod,idelems_mod);
          return s_result;
        }
        else
        {
          p = s_rest->m[j] = s_result->m[j];
          while ((pNext(p)!=NULL) && (pGetComp(pNext(p))<=k)) pIter(p);
          s_result->m[j] = pNext(p);
          pNext(p) = NULL;
        }
      }
      p_Shift(&(s_result->m[j]),-k,currRing);
      pNeg(s_result->m[j]);
    }
  }
  if ((lsmod==0) && (s_rest!=NULL))
  {
    for (j=IDELEMS(s_rest);j>0;j--)
    {
      if (s_rest->m[j-1]!=NULL)
      {
        p_Shift(&(s_rest->m[j-1]),-1,currRing);
      }
    }
  }
  if(syz_ring!=orig_ring)
  {
    idDelete(&s_mod);
    rChangeCurrRing(orig_ring);
    s_result = idrMoveR_NoSort(s_result, syz_ring, orig_ring);
    s_rest = idrMoveR_NoSort(s_rest, syz_ring, orig_ring);
    rDelete(syz_ring);
  }
  if (rest!=NULL)
  {
    s_rest->rank=mod->rank;
    *rest = s_rest;
  }
  else
    idDelete(&s_rest);
//idPrint(s_result);
  if (unit!=NULL)
  {
    *unit=mpNew(idelems_submod,idelems_submod);
    int i;
    for(i=0;i<IDELEMS(s_result);i++)
    {
      poly p=s_result->m[i];
      poly q=NULL;
      while(p!=NULL)
      {
        if(pGetComp(p)<=comps_to_add)
        {
          pSetComp(p,0);
          if (q!=NULL)
          {
            pNext(q)=pNext(p);
          }
          else
          {
            pIter(s_result->m[i]);
          }
          pNext(p)=NULL;
          MATELEM(*unit,i+1,i+1)=pAdd(MATELEM(*unit,i+1,i+1),p);
          if(q!=NULL)   p=pNext(q);
          else          p=s_result->m[i];
        }
        else
        {
          q=p;
          pIter(p);
        }
      }
      p_Shift(&s_result->m[i],-comps_to_add,currRing);
    }
  }
  s_result->rank=idelems_mod;
  return s_result;
}

/*2
*computes division of P by Q with remainder up to (w-weighted) degree n
*P, Q, and w are not changed
*/
void idLiftW(ideal P,ideal Q,int n,matrix &T, ideal &R,short *w)
{
  long N=0;
  int i;
  for(i=IDELEMS(Q)-1;i>=0;i--)
    if(w==NULL)
      N=si_max(N,p_Deg(Q->m[i],currRing));
    else
      N=si_max(N,p_DegW(Q->m[i],w,currRing));
  N+=n;

  T=mpNew(IDELEMS(Q),IDELEMS(P));
  R=idInit(IDELEMS(P),P->rank);

  for(i=IDELEMS(P)-1;i>=0;i--)
  {
    poly p;
    if(w==NULL)
      p=ppJet(P->m[i],N);
    else
      p=ppJetW(P->m[i],N,w);

    int j=IDELEMS(Q)-1;
    while(p!=NULL)
    {
      if(pDivisibleBy(Q->m[j],p))
      {
        poly p0=p_DivideM(pHead(p),pHead(Q->m[j]),currRing);
        if(w==NULL)
          p=pJet(pSub(p,ppMult_mm(Q->m[j],p0)),N);
        else
          p=pJetW(pSub(p,ppMult_mm(Q->m[j],p0)),N,w);
        pNormalize(p);
        if(((w==NULL)&&(p_Deg(p0,currRing)>n))||((w!=NULL)&&(p_DegW(p0,w,currRing)>n)))
          p_Delete(&p0,currRing);
        else
          MATELEM(T,j+1,i+1)=pAdd(MATELEM(T,j+1,i+1),p0);
        j=IDELEMS(Q)-1;
      }
      else
      {
        if(j==0)
        {
          poly p0=p;
          pIter(p);
          pNext(p0)=NULL;
          if(((w==NULL)&&(p_Deg(p0,currRing)>n))
          ||((w!=NULL)&&(p_DegW(p0,w,currRing)>n)))
            p_Delete(&p0,currRing);
          else
            R->m[i]=pAdd(R->m[i],p0);
          j=IDELEMS(Q)-1;
        }
        else
          j--;
      }
    }
  }
}

/*2
*computes the quotient of h1,h2 : internal routine for idQuot
*BEWARE: the returned ideals may contain incorrectly ordered polys !
*
*/
static ideal idInitializeQuot (ideal  h1, ideal h2, BOOLEAN h1IsStb, BOOLEAN *addOnlyOne, int *kkmax)
{
  idTest(h1);
  idTest(h2);

  ideal temph1;
  poly     p,q = NULL;
  int i,l,ll,k,kkk,kmax;
  int j = 0;
  int k1 = id_RankFreeModule(h1,currRing);
  int k2 = id_RankFreeModule(h2,currRing);
  tHomog   hom=isNotHomog;
  k=si_max(k1,k2);
  if (k==0)
    k = 1;
  if ((k2==0) && (k>1)) *addOnlyOne = FALSE;
  intvec * weights;
  hom = (tHomog)idHomModule(h1,currRing->qideal,&weights);
  if /**addOnlyOne &&*/ (/*(*/ !h1IsStb /*)*/)
    temph1 = kStd(h1,currRing->qideal,hom,&weights,NULL);
  else
    temph1 = idCopy(h1);
  if (weights!=NULL) delete weights;
  idTest(temph1);
/*--- making a single vector from h2 ---------------------*/
  for (i=0; i<IDELEMS(h2); i++)
  {
    if (h2->m[i] != NULL)
    {
      p = pCopy(h2->m[i]);
      if (k2 == 0)
        p_Shift(&p,j*k+1,currRing);
      else
        p_Shift(&p,j*k,currRing);
      q = pAdd(q,p);
      j++;
    }
  }
  *kkmax = kmax = j*k+1;
/*--- adding a monomial for the result (syzygy) ----------*/
  p = q;
  while (pNext(p)!=NULL) pIter(p);
  pNext(p) = pOne();
  pIter(p);
  pSetComp(p,kmax);
  pSetmComp(p);
/*--- constructing the big matrix ------------------------*/
  ideal h4 = idInit(k,kmax+k-1);
  h4->m[0] = q;
  if (k2 == 0)
  {
    for (i=1; i<k; i++)
    {
      if (h4->m[i-1]!=NULL)
      {
        p = p_Copy_noCheck(h4->m[i-1], currRing); /*h4->m[i-1]!=NULL*/
        p_Shift(&p,1,currRing);
        h4->m[i] = p;
      }
      else break;
    }
  }
  idSkipZeroes(h4);
  kkk = IDELEMS(h4);
  i = IDELEMS(temph1);
  for (l=0; l<i; l++)
  {
    if(temph1->m[l]!=NULL)
    {
      for (ll=0; ll<j; ll++)
      {
        p = pCopy(temph1->m[l]);
        if (k1 == 0)
          p_Shift(&p,ll*k+1,currRing);
        else
          p_Shift(&p,ll*k,currRing);
        if (kkk >= IDELEMS(h4))
        {
          pEnlargeSet(&(h4->m),IDELEMS(h4),16);
          IDELEMS(h4) += 16;
        }
        h4->m[kkk] = p;
        kkk++;
      }
    }
  }
/*--- if h2 goes in as single vector - the h1-part is just SB ---*/
  if (*addOnlyOne)
  {
    idSkipZeroes(h4);
    p = h4->m[0];
    for (i=0;i<IDELEMS(h4)-1;i++)
    {
      h4->m[i] = h4->m[i+1];
    }
    h4->m[IDELEMS(h4)-1] = p;
  }
  idDelete(&temph1);
  //idTest(h4);//see remark at the beginning
  return h4;
}

/*2
*computes the quotient of h1,h2
*/
ideal idQuot (ideal  h1, ideal h2, BOOLEAN h1IsStb, BOOLEAN resultIsIdeal)
{
  // first check for special case h1:(0)
  if (idIs0(h2))
  {
    ideal res;
    if (resultIsIdeal)
    {
      res = idInit(1,1);
      res->m[0] = pOne();
    }
    else
      res = idFreeModule(h1->rank);
    return res;
  }
  int i, kmax;
  BOOLEAN  addOnlyOne=TRUE;
  tHomog   hom=isNotHomog;
  intvec * weights1;

  ideal s_h4 = idInitializeQuot (h1,h2,h1IsStb,&addOnlyOne,&kmax);

  hom = (tHomog)idHomModule(s_h4,currRing->qideal,&weights1);

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzOrder(orig_ring,TRUE);
  rSetSyzComp(kmax-1,syz_ring);
  rChangeCurrRing(syz_ring);
  if (orig_ring!=syz_ring)
  //  s_h4 = idrMoveR_NoSort(s_h4,orig_ring, syz_ring);
    s_h4 = idrMoveR(s_h4,orig_ring, syz_ring);
  idTest(s_h4);
  #if 0
  void ipPrint_MA0(matrix m, const char *name);
  matrix m=idModule2Matrix(idCopy(s_h4));
  PrintS("start:\n");
  ipPrint_MA0(m,"Q");
  idDelete((ideal *)&m);
  PrintS("last elem:");wrp(s_h4->m[IDELEMS(s_h4)-1]);PrintLn();
  #endif
  ideal s_h3;
  BITSET old_test1;
  SI_SAVE_OPT1(old_test1);
  if (TEST_OPT_RETURN_SB) si_opt_1 |= Sy_bit(OPT_REDTAIL_SYZ);
  if (addOnlyOne)
  {
    if(!rField_is_Ring(currRing)) si_opt_1 |= Sy_bit(OPT_SB_1);
    s_h3 = kStd(s_h4,currRing->qideal,hom,&weights1,NULL,0/*kmax-1*/,IDELEMS(s_h4)-1);
  }
  else
  {
    s_h3 = kStd(s_h4,currRing->qideal,hom,&weights1,NULL,kmax-1);
  }
  SI_RESTORE_OPT1(old_test1);
  #if 0
  // only together with the above debug stuff
  idSkipZeroes(s_h3);
  m=idModule2Matrix(idCopy(s_h3));
  Print("result, kmax=%d:\n",kmax);
  ipPrint_MA0(m,"S");
  idDelete((ideal *)&m);
  #endif
  idTest(s_h3);
  if (weights1!=NULL) delete weights1;
  idDelete(&s_h4);

  for (i=0;i<IDELEMS(s_h3);i++)
  {
    if ((s_h3->m[i]!=NULL) && (pGetComp(s_h3->m[i])>=kmax))
    {
      if (resultIsIdeal)
        p_Shift(&s_h3->m[i],-kmax,currRing);
      else
        p_Shift(&s_h3->m[i],-kmax+1,currRing);
    }
    else
      p_Delete(&s_h3->m[i],currRing);
  }
  if (resultIsIdeal)
    s_h3->rank = 1;
  else
    s_h3->rank = h1->rank;
  if(syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rDelete(syz_ring);
  }
  idSkipZeroes(s_h3);
  idTest(s_h3);
  return s_h3;
}

/*2
* eliminate delVar (product of vars) in h1
*/
ideal idElimination (ideal h1,poly delVar,intvec *hilb, GbVariant alg)
{
  int    i,j=0,k,l;
  ideal  h,hh, h3;
  rRingOrder_t    *ord;
  int    *block0,*block1;
  int    ordersize=2;
  int    **wv;
  tHomog hom;
  intvec * w;
  ring tmpR;
  ring origR = currRing;

  if (delVar==NULL)
  {
    return idCopy(h1);
  }
  if ((currRing->qideal!=NULL) && rIsPluralRing(origR))
  {
    WerrorS("cannot eliminate in a qring");
    return NULL;
  }
  if (idIs0(h1)) return idInit(1,h1->rank);
#ifdef HAVE_PLURAL
  if (rIsPluralRing(origR))
    /* in the NC case, we have to check the admissibility of */
    /* the subalgebra to be intersected with */
  {
    if ((ncRingType(origR) != nc_skew) && (ncRingType(origR) != nc_exterior)) /* in (quasi)-commutative algebras every subalgebra is admissible */
    {
      if (nc_CheckSubalgebra(delVar,origR))
      {
        WerrorS("no elimination is possible: subalgebra is not admissible");
        return NULL;
      }
    }
  }
#endif
  hom=(tHomog)idHomModule(h1,NULL,&w); //sets w to weight vector or NULL
  h3=idInit(16,h1->rank);
  for (k=0;; k++)
  {
    if (origR->order[k]!=0) ordersize++;
    else break;
  }
#if 0
  if (rIsPluralRing(origR)) // we have too keep the odering: it may be needed
                            // for G-algebra
  {
    for (k=0;k<ordersize-1; k++)
    {
      block0[k+1] = origR->block0[k];
      block1[k+1] = origR->block1[k];
      ord[k+1] = origR->order[k];
      if (origR->wvhdl[k]!=NULL) wv[k+1] = (int*) omMemDup(origR->wvhdl[k]);
    }
  }
  else
  {
    block0[1] = 1;
    block1[1] = (currRing->N);
    if (origR->OrdSgn==1) ord[1] = ringorder_wp;
    else                  ord[1] = ringorder_ws;
    wv[1]=(int*)omAlloc0((currRing->N)*sizeof(int));
    double wNsqr = (double)2.0 / (double)(currRing->N);
    wFunctional = wFunctionalBuch;
    int  *x= (int * )omAlloc(2 * ((currRing->N) + 1) * sizeof(int));
    int sl=IDELEMS(h1) - 1;
    wCall(h1->m, sl, x, wNsqr);
    for (sl = (currRing->N); sl!=0; sl--)
      wv[1][sl-1] = x[sl + (currRing->N) + 1];
    omFreeSize((ADDRESS)x, 2 * ((currRing->N) + 1) * sizeof(int));

    ord[2]=ringorder_C;
    ord[3]=0;
  }
#else
#endif
  if ((hom==TRUE) && (origR->OrdSgn==1) && (!rIsPluralRing(origR)))
  {
    #if 1
    // we change to an ordering:
    // aa(1,1,1,...,0,0,0),wp(...),C
    // this seems to be better than version 2 below,
    // according to Tst/../elimiate_[3568].tat (- 17 %)
    ord=(rRingOrder_t*)omAlloc0(4*sizeof(rRingOrder_t));
    block0=(int*)omAlloc0(4*sizeof(int));
    block1=(int*)omAlloc0(4*sizeof(int));
    wv=(int**) omAlloc0(4*sizeof(int**));
    block0[0] = block0[1] = 1;
    block1[0] = block1[1] = rVar(origR);
    wv[0]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    // use this special ordering: like ringorder_a, except that pFDeg, pWeights
    // ignore it
    ord[0] = ringorder_aa;
    for (j=0;j<rVar(origR);j++)
      if (pGetExp(delVar,j+1)!=0) wv[0][j]=1;
    BOOLEAN wp=FALSE;
    for (j=0;j<rVar(origR);j++)
      if (p_Weight(j+1,origR)!=1) { wp=TRUE;break; }
    if (wp)
    {
      wv[1]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
      for (j=0;j<rVar(origR);j++)
        wv[1][j]=p_Weight(j+1,origR);
      ord[1] = ringorder_wp;
    }
    else
      ord[1] = ringorder_dp;
    #else
    // we change to an ordering:
    // a(w1,...wn),wp(1,...0.....),C
    ord=(int*)omAlloc0(4*sizeof(int));
    block0=(int*)omAlloc0(4*sizeof(int));
    block1=(int*)omAlloc0(4*sizeof(int));
    wv=(int**) omAlloc0(4*sizeof(int**));
    block0[0] = block0[1] = 1;
    block1[0] = block1[1] = rVar(origR);
    wv[0]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    wv[1]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    ord[0] = ringorder_a;
    for (j=0;j<rVar(origR);j++)
      wv[0][j]=pWeight(j+1,origR);
    ord[1] = ringorder_wp;
    for (j=0;j<rVar(origR);j++)
      if (pGetExp(delVar,j+1)!=0) wv[1][j]=1;
    #endif
    ord[2] = ringorder_C;
    ord[3] = (rRingOrder_t)0;
  }
  else
  {
    // we change to an ordering:
    // aa(....),orig_ordering
    ord=(rRingOrder_t*)omAlloc0(ordersize*sizeof(rRingOrder_t));
    block0=(int*)omAlloc0(ordersize*sizeof(int));
    block1=(int*)omAlloc0(ordersize*sizeof(int));
    wv=(int**) omAlloc0(ordersize*sizeof(int**));
    for (k=0;k<ordersize-1; k++)
    {
      block0[k+1] = origR->block0[k];
      block1[k+1] = origR->block1[k];
      ord[k+1] = origR->order[k];
      if (origR->wvhdl[k]!=NULL) wv[k+1] = (int*) omMemDup(origR->wvhdl[k]);
    }
    block0[0] = 1;
    block1[0] = rVar(origR);
    wv[0]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    for (j=0;j<rVar(origR);j++)
      if (pGetExp(delVar,j+1)!=0) wv[0][j]=1;
    // use this special ordering: like ringorder_a, except that pFDeg, pWeights
    // ignore it
    ord[0] = ringorder_aa;
  }
  // fill in tmp ring to get back the data later on
  tmpR  = rCopy0(origR,FALSE,FALSE); // qring==NULL
  //rUnComplete(tmpR);
  tmpR->p_Procs=NULL;
  tmpR->order = ord;
  tmpR->block0 = block0;
  tmpR->block1 = block1;
  tmpR->wvhdl = wv;
  rComplete(tmpR, 1);

#ifdef HAVE_PLURAL
  /* update nc structure on tmpR */
  if (rIsPluralRing(origR))
  {
    if ( nc_rComplete(origR, tmpR, false) ) // no quotient ideal!
    {
      WerrorS("no elimination is possible: ordering condition is violated");
      // cleanup
      rDelete(tmpR);
      if (w!=NULL)
        delete w;
      return NULL;
    }
  }
#endif
  // change into the new ring
  //pChangeRing((currRing->N),currRing->OrdSgn,ord,block0,block1,wv);
  rChangeCurrRing(tmpR);

  //h = idInit(IDELEMS(h1),h1->rank);
  // fetch data from the old ring
  //for (k=0;k<IDELEMS(h1);k++) h->m[k] = prCopyR( h1->m[k], origR);
  h=idrCopyR(h1,origR,currRing);
  if (origR->qideal!=NULL)
  {
    WarnS("eliminate in q-ring: experimental");
    ideal q=idrCopyR(origR->qideal,origR,currRing);
    ideal s=idSimpleAdd(h,q);
    idDelete(&h);
    idDelete(&q);
    h=s;
  }
  // compute kStd
#if 1
  //rWrite(tmpR);PrintLn();
  //BITSET save1;
  //SI_SAVE_OPT1(save1);
  //si_opt_1 |=1;
  //Print("h: %d gen, rk=%d\n",IDELEMS(h),h->rank);
  //extern char * showOption();
  //Print("%s\n",showOption());
  if (alg==GbDefault) alg=GbStd;
  if (alg==GbStd)
  {
    if (TEST_OPT_PROT) { PrintS("std:"); mflush(); }
    hh = kStd(h,NULL,hom,&w,hilb);
  }
  else if (alg==GbSlimgb)
  {
    if (TEST_OPT_PROT) { PrintS("slimgb:"); mflush(); }
    hh = t_rep_gb(currRing, h, 0);
  }
  else if (alg==GbGroebner)
  {
    if (TEST_OPT_PROT) { PrintS("groebner:"); mflush(); }
    BOOLEAN err;
    hh=(ideal)iiCallLibProc1("groebner",idCopy(h),MODUL_CMD,err);
    if (err)
    {
      Werror("error %d in >>groebner<<",err);
      hh=idInit(1,1);
    }
  }
  else if (alg==GbModstd)
  {
    if (TEST_OPT_PROT) { PrintS("modstd:"); mflush(); }
    BOOLEAN err;
    void *args[]={idCopy(h),(void*)1,NULL};
    int arg_t[]={IDEAL_CMD,INT_CMD,0};
    leftv temp0=ii_CallLibProcM("modStd",args,arg_t,currRing,err);
    hh=(ideal)temp0->data;
    omFreeBin((ADDRESS)temp0,sleftv_bin);
    if (err)
    {
      Werror("error %d in >>modStd<<",err);
      hh=idInit(1,1);
    }
  }
  else if (alg==GbSba)
  {
    if (TEST_OPT_PROT) { PrintS("sba:"); mflush(); }
    hh = kSba(h,currRing->qideal,hom,&w,1,0,NULL);
  }
  else
  {
    hh=idInit(1,1);
    Werror("wrong algorithm %d for SB",(int)alg);
  }
  //SI_RESTORE_OPT1(save1);
  idDelete(&h);
#else
  extern ideal kGroebner(ideal F, ideal Q);
  hh=kGroebner(h,NULL);
#endif
  // go back to the original ring
  rChangeCurrRing(origR);
  i = IDELEMS(hh)-1;
  while ((i >= 0) && (hh->m[i] == NULL)) i--;
  j = -1;
  // fetch data from temp ring
  for (k=0; k<=i; k++)
  {
    l=(currRing->N);
    while ((l>0) && (p_GetExp( hh->m[k],l,tmpR)*pGetExp(delVar,l)==0)) l--;
    if (l==0)
    {
      j++;
      if (j >= IDELEMS(h3))
      {
        pEnlargeSet(&(h3->m),IDELEMS(h3),16);
        IDELEMS(h3) += 16;
      }
      h3->m[j] = prMoveR( hh->m[k], tmpR,origR);
      hh->m[k] = NULL;
    }
  }
  id_Delete(&hh, tmpR);
  idSkipZeroes(h3);
  rDelete(tmpR);
  if (w!=NULL)
    delete w;
  return h3;
}

#ifdef WITH_OLD_MINOR
/*2
* compute the which-th ar-minor of the matrix a
*/
poly idMinor(matrix a, int ar, unsigned long which, ideal R)
{
  int     i,j/*,k,size*/;
  unsigned long curr;
  int *rowchoise,*colchoise;
  BOOLEAN rowch,colch;
  // ideal result;
  matrix tmp;
  poly p,q;

  rowchoise=(int *)omAlloc(ar*sizeof(int));
  colchoise=(int *)omAlloc(ar*sizeof(int));
  tmp=mpNew(ar,ar);
  curr = 0; /* index of current minor */
  idInitChoise(ar,1,a->rows(),&rowch,rowchoise);
  while (!rowch)
  {
    idInitChoise(ar,1,a->cols(),&colch,colchoise);
    while (!colch)
    {
      if (curr == which)
      {
        for (i=1; i<=ar; i++)
        {
          for (j=1; j<=ar; j++)
          {
            MATELEM(tmp,i,j) = MATELEM(a,rowchoise[i-1],colchoise[j-1]);
          }
        }
        p = mp_DetBareiss(tmp,currRing);
        if (p!=NULL)
        {
          if (R!=NULL)
          {
            q = p;
            p = kNF(R,currRing->qideal,q);
            p_Delete(&q,currRing);
          }
        }
        /*delete the matrix tmp*/
        for (i=1; i<=ar; i++)
        {
          for (j=1; j<=ar; j++) MATELEM(tmp,i,j) = NULL;
        }
        idDelete((ideal*)&tmp);
        omFreeSize((ADDRESS)rowchoise,ar*sizeof(int));
        omFreeSize((ADDRESS)colchoise,ar*sizeof(int));
        return (p);
      }
      curr++;
      idGetNextChoise(ar,a->cols(),&colch,colchoise);
    }
    idGetNextChoise(ar,a->rows(),&rowch,rowchoise);
  }
  return (poly) 1;
}

/*2
* compute all ar-minors of the matrix a
*/
ideal idMinors(matrix a, int ar, ideal R)
{
  int     i,j,/*k,*/size;
  int *rowchoise,*colchoise;
  BOOLEAN rowch,colch;
  ideal result;
  matrix tmp;
  poly p,q;

  i = binom(a->rows(),ar);
  j = binom(a->cols(),ar);
  size=i*j;

  rowchoise=(int *)omAlloc(ar*sizeof(int));
  colchoise=(int *)omAlloc(ar*sizeof(int));
  result=idInit(size,1);
  tmp=mpNew(ar,ar);
  // k = 0; /* the index in result*/
  idInitChoise(ar,1,a->rows(),&rowch,rowchoise);
  while (!rowch)
  {
    idInitChoise(ar,1,a->cols(),&colch,colchoise);
    while (!colch)
    {
      for (i=1; i<=ar; i++)
      {
        for (j=1; j<=ar; j++)
        {
          MATELEM(tmp,i,j) = MATELEM(a,rowchoise[i-1],colchoise[j-1]);
        }
      }
      p = mp_DetBareiss(tmp,currRing);
      if (p!=NULL)
      {
        if (R!=NULL)
        {
          q = p;
          p = kNF(R,currRing->qideal,q);
          p_Delete(&q,currRing);
        }
      }
      if (k>=size)
      {
        pEnlargeSet(&result->m,size,32);
        size += 32;
      }
      result->m[k] = p;
      k++;
      idGetNextChoise(ar,a->cols(),&colch,colchoise);
    }
    idGetNextChoise(ar,a->rows(),&rowch,rowchoise);
  }
  /*delete the matrix tmp*/
  for (i=1; i<=ar; i++)
  {
    for (j=1; j<=ar; j++) MATELEM(tmp,i,j) = NULL;
  }
  idDelete((ideal*)&tmp);
  if (k==0)
  {
    k=1;
    result->m[0]=NULL;
  }
  omFreeSize((ADDRESS)rowchoise,ar*sizeof(int));
  omFreeSize((ADDRESS)colchoise,ar*sizeof(int));
  pEnlargeSet(&result->m,size,k-size);
  IDELEMS(result) = k;
  return (result);
}
#else


/// compute all ar-minors of the matrix a
/// the caller of mpRecMin
/// the elements of the result are not in R (if R!=NULL)
ideal idMinors(matrix a, int ar, ideal R)
{

  const ring origR=currRing;
  id_Test((ideal)a, origR);

  const int r = a->nrows;
  const int c = a->ncols;

  if((ar<=0) || (ar>r) || (ar>c))
  {
    Werror("%d-th minor, matrix is %dx%d",ar,r,c);
    return NULL;
  }

  ideal h = id_Matrix2Module(mp_Copy(a,origR),origR);
  long bound = sm_ExpBound(h,c,r,ar,origR);
  id_Delete(&h, origR);

  ring tmpR = sm_RingChange(origR,bound);

  matrix b = mpNew(r,c);

  for (int i=r*c-1;i>=0;i--)
    if (a->m[i] != NULL)
      b->m[i] = prCopyR(a->m[i],origR,tmpR);

  id_Test( (ideal)b, tmpR);

  if (R!=NULL)
  {
    R = idrCopyR(R,origR,tmpR); // TODO: overwrites R? memory leak?
    //if (ar>1) // otherwise done in mpMinorToResult
    //{
    //  matrix bb=(matrix)kNF(R,currRing->qideal,(ideal)b);
    //  bb->rank=b->rank; bb->nrows=b->nrows; bb->ncols=b->ncols;
    //  idDelete((ideal*)&b); b=bb;
    //}
    id_Test( R, tmpR);
  }

  int size=binom(r,ar)*binom(c,ar);
  ideal result = idInit(size,1);

  int elems = 0;

  if(ar>1)
    mp_RecMin(ar-1,result,elems,b,r,c,NULL,R,tmpR);
  else
    mp_MinorToResult(result,elems,b,r,c,R,tmpR);

  id_Test( (ideal)b, tmpR);

  id_Delete((ideal *)&b, tmpR);

  if (R!=NULL) id_Delete(&R,tmpR);

  rChangeCurrRing(origR);
  result = idrMoveR(result,tmpR,origR);
  sm_KillModifiedRing(tmpR);
  idTest(result);
  return result;
}
#endif

/*2
*returns TRUE if id1 is a submodule of id2
*/
BOOLEAN idIsSubModule(ideal id1,ideal id2)
{
  int i;
  poly p;

  if (idIs0(id1)) return TRUE;
  for (i=0;i<IDELEMS(id1);i++)
  {
    if (id1->m[i] != NULL)
    {
      p = kNF(id2,currRing->qideal,id1->m[i]);
      if (p != NULL)
      {
        p_Delete(&p,currRing);
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOLEAN idTestHomModule(ideal m, ideal Q, intvec *w)
{
  if ((Q!=NULL) && (!idHomIdeal(Q,NULL)))  { PrintS(" Q not hom\n"); return FALSE;}
  if (idIs0(m)) return TRUE;

  int cmax=-1;
  int i;
  poly p=NULL;
  int length=IDELEMS(m);
  polyset P=m->m;
  for (i=length-1;i>=0;i--)
  {
    p=P[i];
    if (p!=NULL) cmax=si_max(cmax,(int)pMaxComp(p)+1);
  }
  if (w != NULL)
  if (w->length()+1 < cmax)
  {
    // Print("length: %d - %d \n", w->length(),cmax);
    return FALSE;
  }

  if(w!=NULL)
    p_SetModDeg(w, currRing);

  for (i=length-1;i>=0;i--)
  {
    p=P[i];
    if (p!=NULL)
    {
      int d=currRing->pFDeg(p,currRing);
      loop
      {
        pIter(p);
        if (p==NULL) break;
        if (d!=currRing->pFDeg(p,currRing))
        {
          //pWrite(q); wrp(p); Print(" -> %d - %d\n",d,pFDeg(p,currRing));
          if(w!=NULL)
            p_SetModDeg(NULL, currRing);
          return FALSE;
        }
      }
    }
  }

  if(w!=NULL)
    p_SetModDeg(NULL, currRing);

  return TRUE;
}

ideal idSeries(int n,ideal M,matrix U,intvec *w)
{
  for(int i=IDELEMS(M)-1;i>=0;i--)
  {
    if(U==NULL)
      M->m[i]=pSeries(n,M->m[i],NULL,w);
    else
    {
      M->m[i]=pSeries(n,M->m[i],MATELEM(U,i+1,i+1),w);
      MATELEM(U,i+1,i+1)=NULL;
    }
  }
  if(U!=NULL)
    idDelete((ideal*)&U);
  return M;
}

matrix idDiff(matrix i, int k)
{
  int e=MATCOLS(i)*MATROWS(i);
  matrix r=mpNew(MATROWS(i),MATCOLS(i));
  r->rank=i->rank;
  int j;
  for(j=0; j<e; j++)
  {
    r->m[j]=pDiff(i->m[j],k);
  }
  return r;
}

matrix idDiffOp(ideal I, ideal J,BOOLEAN multiply)
{
  matrix r=mpNew(IDELEMS(I),IDELEMS(J));
  int i,j;
  for(i=0; i<IDELEMS(I); i++)
  {
    for(j=0; j<IDELEMS(J); j++)
    {
      MATELEM(r,i+1,j+1)=pDiffOp(I->m[i],J->m[j],multiply);
    }
  }
  return r;
}

/*3
*handles for some ideal operations the ring/syzcomp managment
*returns all syzygies (componentwise-)shifted by -syzcomp
*or -syzcomp-1 (in case of ideals as input)
static ideal idHandleIdealOp(ideal arg,int syzcomp,int isIdeal=FALSE)
{
  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzOrder(orig_ring, TRUE); rChangeCurrRing(syz_ring);
  rSetSyzComp(length, syz_ring);

  ideal s_temp;
  if (orig_ring!=syz_ring)
    s_temp=idrMoveR_NoSort(arg,orig_ring, syz_ring);
  else
    s_temp=arg;

  ideal s_temp1 = kStd(s_temp,currRing->qideal,testHomog,&w,NULL,length);
  if (w!=NULL) delete w;

  if (syz_ring!=orig_ring)
  {
    idDelete(&s_temp);
    rChangeCurrRing(orig_ring);
  }

  idDelete(&temp);
  ideal temp1=idRingCopy(s_temp1,syz_ring);

  if (syz_ring!=orig_ring)
  {
    rChangeCurrRing(syz_ring);
    idDelete(&s_temp1);
    rChangeCurrRing(orig_ring);
    rDelete(syz_ring);
  }

  for (i=0;i<IDELEMS(temp1);i++)
  {
    if ((temp1->m[i]!=NULL)
    && (pGetComp(temp1->m[i])<=length))
    {
      pDelete(&(temp1->m[i]));
    }
    else
    {
      p_Shift(&(temp1->m[i]),-length,currRing);
    }
  }
  temp1->rank = rk;
  idSkipZeroes(temp1);

  return temp1;
}
*/
/*2
* represents (h1+h2)/h2=h1/(h1 intersect h2)
*/
//ideal idModulo (ideal h2,ideal h1)
ideal idModulo (ideal h2,ideal h1, tHomog hom, intvec ** w)
{
  intvec *wtmp=NULL;

  int i,k,rk,flength=0,slength,length;
  poly p,q;

  if (idIs0(h2))
    return idFreeModule(si_max(1,h2->ncols));
  if (!idIs0(h1))
    flength = id_RankFreeModule(h1,currRing);
  slength = id_RankFreeModule(h2,currRing);
  length  = si_max(flength,slength);
  if (length==0)
  {
    length = 1;
  }
  ideal temp = idInit(IDELEMS(h2),length+IDELEMS(h2));
  if ((w!=NULL)&&((*w)!=NULL))
  {
    //Print("input weights:");(*w)->show(1);PrintLn();
    int d;
    int k;
    wtmp=new intvec(length+IDELEMS(h2));
    for (i=0;i<length;i++)
      ((*wtmp)[i])=(**w)[i];
    for (i=0;i<IDELEMS(h2);i++)
    {
      poly p=h2->m[i];
      if (p!=NULL)
      {
        d = p_Deg(p,currRing);
        k= pGetComp(p);
        if (slength>0) k--;
        d +=((**w)[k]);
        ((*wtmp)[i+length]) = d;
      }
    }
    //Print("weights:");wtmp->show(1);PrintLn();
  }
#ifdef HAVE_SHIFTBBA
  if (rIsLPRing(currRing))
  {
    for (i=0;i<IDELEMS(h2);i++)
    {
      temp->m[i] = pCopy(h2->m[i]);
      q = pOne();
      // non multiplicative variable
      pSetExp(q, currRing->isLPring - currRing->LPncGenCount + i + 1, 1);
      p_Setm(q, currRing);
      pSetComp(q,i+1+length);
      pSetmComp(q);
      if(temp->m[i]!=NULL)
      {
        if (slength==0) p_Shift(&(temp->m[i]),1,currRing);
        p = temp->m[i];
        temp->m[i] = pAdd(p, q);
      }
      else
        temp->m[i]=q;
    }
  }
  else
#endif
  {
    for (i=0;i<IDELEMS(h2);i++)
    {
      temp->m[i] = pCopy(h2->m[i]);
      q = pOne();
      pSetComp(q,i+1+length);
      pSetmComp(q);
      if(temp->m[i]!=NULL)
      {
        if (slength==0) p_Shift(&(temp->m[i]),1,currRing);
        p = temp->m[i];
        while (pNext(p)!=NULL) pIter(p);
        pNext(p) = q; // will be sorted later correctly
      }
      else
        temp->m[i]=q;
    }
  }
  rk = k = IDELEMS(h2);
  if (!idIs0(h1))
  {
    pEnlargeSet(&(temp->m),IDELEMS(temp),IDELEMS(h1));
    IDELEMS(temp) += IDELEMS(h1);
    for (i=0;i<IDELEMS(h1);i++)
    {
      if (h1->m[i]!=NULL)
      {
        temp->m[k] = pCopy(h1->m[i]);
        if (flength==0) p_Shift(&(temp->m[k]),1,currRing);
        k++;
      }
    }
  }

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzOrder(orig_ring, TRUE);
  rSetSyzComp(length,syz_ring);
  rChangeCurrRing(syz_ring);
  // we can use OPT_RETURN_SB only, if syz_ring==orig_ring,
  // therefore we disable OPT_RETURN_SB for modulo:
  // (see tr. #701)
  //if (TEST_OPT_RETURN_SB)
  //  rSetSyzComp(IDELEMS(h2)+length, syz_ring);
  //else
  //  rSetSyzComp(length, syz_ring);
  ideal s_temp;

  if (syz_ring != orig_ring)
  {
    s_temp = idrMoveR_NoSort(temp, orig_ring, syz_ring);
  }
  else
  {
    s_temp = temp;
  }

  idTest(s_temp);
  unsigned save_opt;
  SI_SAVE_OPT1(save_opt);
  si_opt_1 |= Sy_bit(OPT_REDTAIL_SYZ);
  ideal s_temp1 = kStd(s_temp,currRing->qideal,hom,&wtmp,NULL,length);
  SI_RESTORE_OPT1(save_opt);

  //if (wtmp!=NULL)  Print("output weights:");wtmp->show(1);PrintLn();
  if ((w!=NULL) && (*w !=NULL) && (wtmp!=NULL))
  {
    delete *w;
    *w=new intvec(IDELEMS(h2));
    for (i=0;i<IDELEMS(h2);i++)
      ((**w)[i])=(*wtmp)[i+length];
  }
  if (wtmp!=NULL) delete wtmp;

  for (i=0;i<IDELEMS(s_temp1);i++)
  {
    if ((s_temp1->m[i]!=NULL)
    && (((int)pGetComp(s_temp1->m[i]))<=length))
    {
      p_Delete(&(s_temp1->m[i]),currRing);
    }
    else
    {
      p_Shift(&(s_temp1->m[i]),-length,currRing);
    }
  }
  s_temp1->rank = rk;
  idSkipZeroes(s_temp1);

  if (syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    s_temp1 = idrMoveR_NoSort(s_temp1, syz_ring, orig_ring);
    rDelete(syz_ring);
    // Hmm ... here seems to be a memory leak
    // However, simply deleting it causes memory trouble
    // idDelete(&s_temp);
  }
  else
  {
    idDelete(&temp);
  }
  idTest(s_temp1);
  return s_temp1;
}

/*
*computes module-weights for liftings of homogeneous modules
*/
intvec * idMWLift(ideal mod,intvec * weights)
{
  if (idIs0(mod)) return new intvec(2);
  int i=IDELEMS(mod);
  while ((i>0) && (mod->m[i-1]==NULL)) i--;
  intvec *result = new intvec(i+1);
  while (i>0)
  {
    (*result)[i]=currRing->pFDeg(mod->m[i],currRing)+(*weights)[pGetComp(mod->m[i])];
  }
  return result;
}

/*2
*sorts the kbase for idCoef* in a special way (lexicographically
*with x_max,...,x_1)
*/
ideal idCreateSpecialKbase(ideal kBase,intvec ** convert)
{
  int i;
  ideal result;

  if (idIs0(kBase)) return NULL;
  result = idInit(IDELEMS(kBase),kBase->rank);
  *convert = idSort(kBase,FALSE);
  for (i=0;i<(*convert)->length();i++)
  {
    result->m[i] = pCopy(kBase->m[(**convert)[i]-1]);
  }
  return result;
}

/*2
*returns the index of a given monom in the list of the special kbase
*/
int idIndexOfKBase(poly monom, ideal kbase)
{
  int j=IDELEMS(kbase);

  while ((j>0) && (kbase->m[j-1]==NULL)) j--;
  if (j==0) return -1;
  int i=(currRing->N);
  while (i>0)
  {
    loop
    {
      if (pGetExp(monom,i)>pGetExp(kbase->m[j-1],i)) return -1;
      if (pGetExp(monom,i)==pGetExp(kbase->m[j-1],i)) break;
      j--;
      if (j==0) return -1;
    }
    if (i==1)
    {
      while(j>0)
      {
        if (pGetComp(monom)==pGetComp(kbase->m[j-1])) return j-1;
        if (pGetComp(monom)>pGetComp(kbase->m[j-1])) return -1;
        j--;
      }
    }
    i--;
  }
  return -1;
}

/*2
*decomposes the monom in a part of coefficients described by the
*complement of how and a monom in variables occuring in how, the
*index of which in kbase is returned as integer pos (-1 if it don't
*exists)
*/
poly idDecompose(poly monom, poly how, ideal kbase, int * pos)
{
  int i;
  poly coeff=pOne(), base=pOne();

  for (i=1;i<=(currRing->N);i++)
  {
    if (pGetExp(how,i)>0)
    {
      pSetExp(base,i,pGetExp(monom,i));
    }
    else
    {
      pSetExp(coeff,i,pGetExp(monom,i));
    }
  }
  pSetComp(base,pGetComp(monom));
  pSetm(base);
  pSetCoeff(coeff,nCopy(pGetCoeff(monom)));
  pSetm(coeff);
  *pos = idIndexOfKBase(base,kbase);
  if (*pos<0)
    p_Delete(&coeff,currRing);
  p_Delete(&base,currRing);
  return coeff;
}

/*2
*returns a matrix A of coefficients with kbase*A=arg
*if all monomials in variables of how occur in kbase
*the other are deleted
*/
matrix idCoeffOfKBase(ideal arg, ideal kbase, poly how)
{
  matrix result;
  ideal tempKbase;
  poly p,q;
  intvec * convert;
  int i=IDELEMS(kbase),j=IDELEMS(arg),k,pos;
#if 0
  while ((i>0) && (kbase->m[i-1]==NULL)) i--;
  if (idIs0(arg))
    return mpNew(i,1);
  while ((j>0) && (arg->m[j-1]==NULL)) j--;
  result = mpNew(i,j);
#else
  result = mpNew(i, j);
  while ((j>0) && (arg->m[j-1]==NULL)) j--;
#endif

  tempKbase = idCreateSpecialKbase(kbase,&convert);
  for (k=0;k<j;k++)
  {
    p = arg->m[k];
    while (p!=NULL)
    {
      q = idDecompose(p,how,tempKbase,&pos);
      if (pos>=0)
      {
        MATELEM(result,(*convert)[pos],k+1) =
            pAdd(MATELEM(result,(*convert)[pos],k+1),q);
      }
      else
        p_Delete(&q,currRing);
      pIter(p);
    }
  }
  idDelete(&tempKbase);
  return result;
}

static void idDeleteComps(ideal arg,int* red_comp,int del)
// red_comp is an array [0..args->rank]
{
  int i,j;
  poly p;

  for (i=IDELEMS(arg)-1;i>=0;i--)
  {
    p = arg->m[i];
    while (p!=NULL)
    {
      j = pGetComp(p);
      if (red_comp[j]!=j)
      {
        pSetComp(p,red_comp[j]);
        pSetmComp(p);
      }
      pIter(p);
    }
  }
  (arg->rank) -= del;
}

/*2
* returns the presentation of an isomorphic, minimally
* embedded  module (arg represents the quotient!)
*/
ideal idMinEmbedding(ideal arg,BOOLEAN inPlace, intvec **w)
{
  if (idIs0(arg)) return idInit(1,arg->rank);
  int i,next_gen,next_comp;
  ideal res=arg;
  if (!inPlace) res = idCopy(arg);
  res->rank=si_max(res->rank,id_RankFreeModule(res,currRing));
  int *red_comp=(int*)omAlloc((res->rank+1)*sizeof(int));
  for (i=res->rank;i>=0;i--) red_comp[i]=i;

  int del=0;
  loop
  {
    next_gen = id_ReadOutPivot(res, &next_comp, currRing);
    if (next_gen<0) break;
    del++;
    syGaussForOne(res,next_gen,next_comp,0,IDELEMS(res));
    for(i=next_comp+1;i<=arg->rank;i++) red_comp[i]--;
    if ((w !=NULL)&&(*w!=NULL))
    {
      for(i=next_comp;i<(*w)->length();i++) (**w)[i-1]=(**w)[i];
    }
  }

  idDeleteComps(res,red_comp,del);
  idSkipZeroes(res);
  omFree(red_comp);

  if ((w !=NULL)&&(*w!=NULL) &&(del>0))
  {
    int nl=si_max((*w)->length()-del,1);
    intvec *wtmp=new intvec(nl);
    for(i=0;i<res->rank;i++) (*wtmp)[i]=(**w)[i];
    delete *w;
    *w=wtmp;
  }
  return res;
}

#include "polys/clapsing.h"

#if 0
poly id_GCD(poly f, poly g, const ring r)
{
  ring save_r=currRing;
  rChangeCurrRing(r);
  ideal I=idInit(2,1); I->m[0]=f; I->m[1]=g;
  intvec *w = NULL;
  ideal S=idSyzygies(I,testHomog,&w);
  if (w!=NULL) delete w;
  poly gg=pTakeOutComp(&(S->m[0]),2);
  idDelete(&S);
  poly gcd_p=singclap_pdivide(f,gg,r);
  p_Delete(&gg,r);
  rChangeCurrRing(save_r);
  return gcd_p;
}
#else
poly id_GCD(poly f, poly g, const ring r)
{
  ideal I=idInit(2,1); I->m[0]=f; I->m[1]=g;
  intvec *w = NULL;

  ring save_r = currRing;
  rChangeCurrRing(r);
  ideal S=idSyzygies(I,testHomog,&w);
  rChangeCurrRing(save_r);

  if (w!=NULL) delete w;
  poly gg=p_TakeOutComp(&(S->m[0]), 2, r);
  id_Delete(&S, r);
  poly gcd_p=singclap_pdivide(f,gg, r);
  p_Delete(&gg, r);

  return gcd_p;
}
#endif

#if 0
/*2
* xx,q: arrays of length 0..rl-1
* xx[i]: SB mod q[i]
* assume: char=0
* assume: q[i]!=0
* destroys xx
*/
ideal id_ChineseRemainder(ideal *xx, number *q, int rl, const ring R)
{
  int cnt=IDELEMS(xx[0])*xx[0]->nrows;
  ideal result=idInit(cnt,xx[0]->rank);
  result->nrows=xx[0]->nrows; // for lifting matrices
  result->ncols=xx[0]->ncols; // for lifting matrices
  int i,j;
  poly r,h,hh,res_p;
  number *x=(number *)omAlloc(rl*sizeof(number));
  for(i=cnt-1;i>=0;i--)
  {
    res_p=NULL;
    loop
    {
      r=NULL;
      for(j=rl-1;j>=0;j--)
      {
        h=xx[j]->m[i];
        if ((h!=NULL)
        &&((r==NULL)||(p_LmCmp(r,h,R)==-1)))
          r=h;
      }
      if (r==NULL) break;
      h=p_Head(r, R);
      for(j=rl-1;j>=0;j--)
      {
        hh=xx[j]->m[i];
        if ((hh!=NULL) && (p_LmCmp(r,hh, R)==0))
        {
          x[j]=p_GetCoeff(hh, R);
          hh=p_LmFreeAndNext(hh, R);
          xx[j]->m[i]=hh;
        }
        else
          x[j]=n_Init(0, R->cf); // is R->cf really n_Q???, yes!
      }

      number n=n_ChineseRemainder(x,q,rl, R->cf);

      for(j=rl-1;j>=0;j--)
      {
        x[j]=NULL; // nlInit(0...) takes no memory
      }
      if (n_IsZero(n, R->cf)) p_Delete(&h, R);
      else
      {
        p_SetCoeff(h,n, R);
        //Print("new mon:");pWrite(h);
        res_p=p_Add_q(res_p, h, R);
      }
    }
    result->m[i]=res_p;
  }
  omFree(x);
  for(i=rl-1;i>=0;i--) id_Delete(&(xx[i]), R);
  omFree(xx);
  return result;
}
#endif
/* currently unused:
ideal idChineseRemainder(ideal *xx, intvec *iv)
{
  int rl=iv->length();
  number *q=(number *)omAlloc(rl*sizeof(number));
  int i;
  for(i=0; i<rl; i++)
  {
    q[i]=nInit((*iv)[i]);
  }
  return idChineseRemainder(xx,q,rl);
}
*/
/*
 * lift ideal with coeffs over Z (mod N) to Q via Farey
 */
ideal id_Farey(ideal x, number N, const ring r)
{
  int cnt=IDELEMS(x)*x->nrows;
  ideal result=idInit(cnt,x->rank);
  result->nrows=x->nrows; // for lifting matrices
  result->ncols=x->ncols; // for lifting matrices

  int i;
  for(i=cnt-1;i>=0;i--)
  {
    result->m[i]=p_Farey(x->m[i],N,r);
  }
  return result;
}




// uses glabl vars via pSetModDeg
/*
BOOLEAN idTestHomModule(ideal m, ideal Q, intvec *w)
{
  if ((Q!=NULL) && (!idHomIdeal(Q,NULL)))  { PrintS(" Q not hom\n"); return FALSE;}
  if (idIs0(m)) return TRUE;

  int cmax=-1;
  int i;
  poly p=NULL;
  int length=IDELEMS(m);
  poly* P=m->m;
  for (i=length-1;i>=0;i--)
  {
    p=P[i];
    if (p!=NULL) cmax=si_max(cmax,(int)pMaxComp(p)+1);
  }
  if (w != NULL)
  if (w->length()+1 < cmax)
  {
    // Print("length: %d - %d \n", w->length(),cmax);
    return FALSE;
  }

  if(w!=NULL)
    p_SetModDeg(w, currRing);

  for (i=length-1;i>=0;i--)
  {
    p=P[i];
    poly q=p;
    if (p!=NULL)
    {
      int d=p_FDeg(p,currRing);
      loop
      {
        pIter(p);
        if (p==NULL) break;
        if (d!=p_FDeg(p,currRing))
        {
          //pWrite(q); wrp(p); Print(" -> %d - %d\n",d,pFDeg(p,currRing));
          if(w!=NULL)
            p_SetModDeg(NULL, currRing);
          return FALSE;
        }
      }
    }
  }

  if(w!=NULL)
    p_SetModDeg(NULL, currRing);

  return TRUE;
}
*/

/// keeps the first k (>= 1) entries of the given ideal
/// (Note that the kept polynomials may be zero.)
void idKeepFirstK(ideal id, const int k)
{
   for (int i = IDELEMS(id)-1; i >= k; i--)
   {
      if (id->m[i] != NULL) pDelete(&id->m[i]);
   }
   int kk=k;
   if (k==0) kk=1; /* ideals must have at least one element(0)*/
   pEnlargeSet(&(id->m), IDELEMS(id), kk-IDELEMS(id));
   IDELEMS(id) = kk;
}

typedef struct
{
  poly p;
  int index;
} poly_sort;

int pCompare_qsort(const void *a, const void *b)
{
  return (p_Compare(((poly_sort *)a)->p, ((poly_sort *)b)->p,currRing));
}

void idSort_qsort(poly_sort *id_sort, int idsize)
{
  qsort(id_sort, idsize, sizeof(poly_sort), pCompare_qsort);
}

/*2
* ideal id = (id[i])
* if id[i] = id[j] then id[j] is deleted for j > i
*/
void idDelEquals(ideal id)
{
  int idsize = IDELEMS(id);
  poly_sort *id_sort = (poly_sort *)omAlloc0(idsize*sizeof(poly_sort));
  for (int i = 0; i < idsize; i++)
  {
    id_sort[i].p = id->m[i];
    id_sort[i].index = i;
  }
  idSort_qsort(id_sort, idsize);
  int index, index_i, index_j;
  int i = 0;
  for (int j = 1; j < idsize; j++)
  {
    if (id_sort[i].p != NULL && pEqualPolys(id_sort[i].p, id_sort[j].p))
    {
      index_i = id_sort[i].index;
      index_j = id_sort[j].index;
      if (index_j > index_i)
      {
        index = index_j;
      }
      else
      {
        index = index_i;
        i = j;
      }
      pDelete(&id->m[index]);
    }
    else
    {
      i = j;
    }
  }
  omFreeSize((ADDRESS)(id_sort), idsize*sizeof(poly_sort));
}

STATIC_VAR int * id_satstdSaturatingVariables=NULL;

static BOOLEAN id_sat_vars_sp(kStrategy strat)
{
  BOOLEAN b = FALSE; // set b to TRUE, if spoly was changed,
                     // let it remain FALSE otherwise
  if (strat->P.t_p==NULL)
  {
    poly p=strat->P.p;

    // iterate over all terms of p and
    // compute the minimum mm of all exponent vectors
    int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    int *m0=(int*)omAlloc0((1+rVar(currRing))*sizeof(int));
    p_GetExpV(p,mm,currRing);
    bool nonTrivialSaturationToBeDone=true;
    for (p=pNext(p); p!=NULL; pIter(p))
    {
      nonTrivialSaturationToBeDone=false;
      p_GetExpV(p,m0,currRing);
      for (int i=rVar(currRing); i>0; i--)
      {
        if (id_satstdSaturatingVariables[i]!=0)
        {
          mm[i]=si_min(mm[i],m0[i]);
          if (mm[i]>0) nonTrivialSaturationToBeDone=true;
        }
        else mm[i]=0;
      }
      // abort if the minimum is zero in each component
      if (!nonTrivialSaturationToBeDone) break;
    }
    if (nonTrivialSaturationToBeDone)
    {
      // std::cout << "simplifying!" << std::endl;
      if (TEST_OPT_PROT) { PrintS("S"); mflush(); }
      p=p_Copy(strat->P.p,currRing);
      //pWrite(p);
      //  for (int i=rVar(currRing); i>0; i--)
      //    if (mm[i]!=0) Print("x_%d:%d ",i,mm[i]);
      //PrintLn();
      strat->P.Init(currRing);
      //memset(&strat->P,0,sizeof(strat->P));
      strat->P.tailRing = strat->tailRing;
      strat->P.p=p;
      while(p!=NULL)
      {
        for (int i=rVar(currRing); i>0; i--)
        {
          p_SubExp(p,i,mm[i],currRing);
        }
        p_Setm(p,currRing);
        pIter(p);
      }
      b = TRUE;
    }
    omFree(mm);
    omFree(m0);
  }
  else
  {
    poly p=strat->P.t_p;

    // iterate over all terms of p and
    // compute the minimum mm of all exponent vectors
    int *mm=(int*)omAlloc((1+rVar(currRing))*sizeof(int));
    int *m0=(int*)omAlloc0((1+rVar(currRing))*sizeof(int));
    p_GetExpV(p,mm,strat->tailRing);
    bool nonTrivialSaturationToBeDone=true;
    for (p = pNext(p); p!=NULL; pIter(p))
    {
      nonTrivialSaturationToBeDone=false;
      p_GetExpV(p,m0,strat->tailRing);
      for(int i=rVar(currRing); i>0; i--)
      {
        if(id_satstdSaturatingVariables[i]!=0)
        {
          mm[i]=si_min(mm[i],m0[i]);
          if (mm[i]>0) nonTrivialSaturationToBeDone = true;
        }
        else mm[i]=0;
      }
      // abort if the minimum is zero in each component
      if (!nonTrivialSaturationToBeDone) break;
    }
    if (nonTrivialSaturationToBeDone)
    {
      if (TEST_OPT_PROT) { PrintS("S"); mflush(); }
      p=p_Copy(strat->P.t_p,strat->tailRing);
      //p_Write(p,strat->tailRing);
      //  for (int i=rVar(currRing); i>0; i--)
      //    if (mm[i]!=0) Print("x_%d:%d ",i,mm[i]);
      //PrintLn();
      strat->P.Init(currRing);
      //memset(&strat->P,0,sizeof(strat->P));
      strat->P.tailRing = strat->tailRing;
      strat->P.t_p=p;
      while(p!=NULL)
      {
        for(int i=rVar(currRing); i>0; i--)
        {
          p_SubExp(p,i,mm[i],strat->tailRing);
        }
        p_Setm(p,strat->tailRing);
        pIter(p);
      }
      strat->P.GetP();
      b = TRUE;
    }
    omFree(mm);
    omFree(m0);
  }
  return b; // return TRUE if sp was changed, FALSE if not
}

ideal id_Satstd(const ideal I, ideal J, const ring r)
{
  ring save=currRing;
  if (currRing!=r) rChangeCurrRing(r);
  idSkipZeroes(J);
  id_satstdSaturatingVariables=(int*)omAlloc0((1+rVar(currRing))*sizeof(int));
  int k=IDELEMS(J);
  if (k>1)
  {
    for (int i=0; i<k; i++)
    {
      poly x = J->m[i];
      int li = p_Var(x,r);
      if (li>0)
        id_satstdSaturatingVariables[li]=1;
      else
      {
        if (currRing!=save) rChangeCurrRing(save);
        WerrorS("ideal generators must be variables");
        return NULL;
      }
    }
  }
  else
  {
    poly x = J->m[0];
    for (int i=1; i<=r->N; i++)
    {
      int li = p_GetExp(x,i,r);
      if (li==1)
        id_satstdSaturatingVariables[i]=1;
      else if (li>1)
      {
        if (currRing!=save) rChangeCurrRing(save);
        Werror("exponent(x(%d)^%d) must be 0 or 1",i,li);
        return NULL;
      }
    }
  }
  ideal res=kStd(I,r->qideal,testHomog,NULL,NULL,0,0,NULL,id_sat_vars_sp);
  omFreeSize(id_satstdSaturatingVariables,(1+rVar(currRing))*sizeof(int));
  id_satstdSaturatingVariables=NULL;
  if (currRing!=save) rChangeCurrRing(save);
  return res;
}

GbVariant syGetAlgorithm(char *n, const ring r, const ideal /*M*/)
{
  GbVariant alg=GbDefault;
  if (strcmp(n,"slimgb")==0) alg=GbSlimgb;
  else if (strcmp(n,"std")==0) alg=GbStd;
  else if (strcmp(n,"sba")==0) alg=GbSba;
  else if (strcmp(n,"singmatic")==0) alg=GbSingmatic;
  else if (strcmp(n,"groebner")==0) alg=GbGroebner;
  else if (strcmp(n,"modstd")==0) alg=GbModstd;
  else if (strcmp(n,"ffmod")==0) alg=GbFfmod;
  else if (strcmp(n,"nfmod")==0) alg=GbNfmod;
  else if (strcmp(n,"std:sat")==0) alg=GbStdSat;
  else Warn(">>%s<< is an unknown algorithm",n);

  if (alg==GbSlimgb) // test conditions for slimgb
  {
    if(rHasGlobalOrdering(r)
    &&(!rIsPluralRing(r))
    &&(r->qideal==NULL)
    &&(!rField_is_Ring(r)))
    {
       return GbSlimgb;
    }
    if (TEST_OPT_PROT)
      WarnS("requires: coef:field, commutative, global ordering, not qring");
  }
  else if (alg==GbSba) // cond. for sba
  {
    if(rField_is_Domain(r)
    &&(!rIsPluralRing(r))
    &&(rHasGlobalOrdering(r)))
    {
      return GbSba;
    }
    if (TEST_OPT_PROT)
      WarnS("requires: coef:domain, commutative, global ordering");
  }
  else if (alg==GbGroebner) // cond. for groebner
  {
    return GbGroebner;
  }
  else if(alg==GbModstd)  // cond for modstd: Q or Q(a)
  {
    if(ggetid("modStd")==NULL)
    {
      WarnS(">>modStd<< not found");
    }
    else if(rField_is_Q(r)
    &&(!rIsPluralRing(r))
    &&(rHasGlobalOrdering(r)))
    {
      return GbModstd;
    }
    if (TEST_OPT_PROT)
      WarnS("requires: coef:QQ, commutative, global ordering");
  }
  else if(alg==GbStdSat)  // cond for std:sat: 2 blocks of variables
  {
    if(ggetid("satstd")==NULL)
    {
      WarnS(">>satstd<< not found");
    }
    else
    {
      return GbStdSat;
    }
  }

  return GbStd; // no conditions for std
}
//----------------------------------------------------------------------------
// GB-algorithms and their pre-conditions
// std   slimgb  sba singmatic modstd ffmod nfmod groebner
// +     +       +   -         +      -     -     + coeffs: QQ
// +     +       +   +         -      -     -     + coeffs: ZZ/p
// +     +       +   -         ?      -     +     + coeffs: K[a]/f
// +     +       +   -         ?      +     -     + coeffs: K(a)
// +     -       +   -         -      -     -     + coeffs: domain, not field
// +     -       -   -         -      -     -     + coeffs: zero-divisors
// +     +       +   +         -      ?     ?     + also for modules: C
// +     +       -   +         -      ?     ?     + also for modules: all orderings
// +     +       -   -         -      -     -     + exterior algebra
// +     +       -   -         -      -     -     + G-algebra
// +     +       +   +         +      +     +     + degree ordering
// +     -       +   +         +      +     +     + non-degree ordering
// -     -       -   +         +      +     +     + parallel
