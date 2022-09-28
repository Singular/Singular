/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: resolutions
*/

#include "kernel/mod2.h"

#include "misc/mylimits.h"
#include "misc/options.h"
#include "misc/intvec.h"

#include "coeffs/numbers.h"

#include "polys/monomials/ring.h"
#include "polys/kbuckets.h"
#include "polys/prCopy.h"
#include "polys/matpol.h"

#include "kernel/combinatorics/stairc.h"
#include "kernel/combinatorics/hilb.h"

#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/syz.h"

#include "kernel/ideals.h"
#include "kernel/polys.h"

//#define SHOW_PROT
//#define SHOW_RED
//#define SHOW_Kosz
//#define SHOW_RESULT
//#define INVERT_PAIRS
//#define ONLY_STD
//#define EXPERIMENT1    //Hier stimmt was mit der Anzahl der Erzeuger in xyz11 nicht!!
#define EXPERIMENT2
#define EXPERIMENT3
#define WITH_BUCKET     //Use of buckets in EXPERIMENT3 (Product criterion)
#define WITH_SCHREYER_ORD
#define USE_CHAINCRIT
#define USE_CHAINCRIT0
#define USE_PROD_CRIT
#define USE_REGULARITY
#define WITH_SORT
//#define FULL_TOTAKE
VAR int discard_pairs;
VAR int short_pairs;

/*3
* assumes the ideals old_ideal and new_ideal to be homogeneous
* tests wether the new_ideal is a regular extension of the old_ideal
*/
static BOOLEAN syIsRegular(ideal old_ideal,ideal new_ideal,int deg)
{
  intvec * old_hilbs=hFirstSeries(old_ideal,NULL,NULL,NULL);
  intvec * new_hilbs=hFirstSeries(new_ideal,NULL,NULL,NULL);
  int biggest_length=si_max(old_hilbs->length()+deg,new_hilbs->length());
  intvec * shifted_old_hilbs=new intvec(biggest_length);
  intvec * old_hilb1=new intvec(biggest_length);
  intvec * new_hilb1=new intvec(biggest_length);
  int i;
  BOOLEAN isRegular=TRUE;

  for (i=old_hilbs->length()+deg-1;i>=deg;i--)
    (*shifted_old_hilbs)[i] = (*old_hilbs)[i-deg];
  for (i=old_hilbs->length()-1;i>=0;i--)
    (*old_hilb1)[i] = (*old_hilbs)[i]-(*shifted_old_hilbs)[i];
  for (i=old_hilbs->length()+deg-1;i>=old_hilbs->length();i--)
    (*old_hilb1)[i] = -(*shifted_old_hilbs)[i];
  for (i=new_hilbs->length()-1;i>=0;i--)
    (*new_hilb1)[i] = (*new_hilbs)[i];
  i = 0;
  while ((i<biggest_length) && isRegular)
  {
    isRegular = isRegular && ((*old_hilb1)[i] == (*new_hilb1)[i]);
    i++;
  }
  delete old_hilbs;
  delete new_hilbs;
  delete old_hilb1;
  delete new_hilb1;
  delete shifted_old_hilbs;
  return isRegular;
}

/*3
* shows the resolution stored in syzstr->orderedRes
*/
#if 0 /* unused*/
static void syShowRes(syStrategy syzstr)
{
  int i=0;

  while ((i<syzstr->length) && (!idIs0(syzstr->res[i])))
  {
    Print("aktueller hoechster index ist: %d\n",(*syzstr->Tl)[i]);
    Print("der %d-te modul ist:\n",i);
    idPrint(syzstr->res[i]);
    PrintS("Seine Darstellung:\n");
    idPrint(syzstr->orderedRes[i]);
    i++;
  }
}
#endif

/*3
* produces the next subresolution for a regular extension
*/
static void syCreateRegularExtension(syStrategy syzstr,ideal old_ideal,
            ideal old_repr,int old_tl, poly next_generator,resolvente totake)
{
  int index=syzstr->length-1,i,j,start,start_ttk/*,new_tl*/;
  poly gen=pCopy(next_generator),p;
  poly neg_gen=pCopy(next_generator);
  ideal current_ideal,current_repr;
  int current_tl;
  poly w_gen=pHead(next_generator);
  pSetComp(w_gen,0);
  pSetmComp(w_gen);

  //syShowRes(syzstr);
  neg_gen = pNeg(neg_gen);
  if (pGetComp(gen)>0)
  {
    p_Shift(&gen,-1,currRing);
    p_Shift(&neg_gen,-1,currRing);
  }
  while (index>0)
  {
    if (index%2==0)
      p = gen;
    else
      p = neg_gen;
    if (index>1)
    {
      current_ideal = syzstr->res[index-1];
      current_repr = syzstr->orderedRes[index-1];
      current_tl = (*syzstr->Tl)[index-1];
    }
    else
    {
      current_ideal = old_ideal;
      current_repr = old_repr;
      current_tl = old_tl;
    }
    if (!idIs0(current_ideal))
    {
      if (idIs0(syzstr->res[index]))
      {
        syzstr->res[index] = idInit(IDELEMS(current_ideal),
          current_ideal->rank+current_tl);
        syzstr->orderedRes[index] = idInit(IDELEMS(current_ideal),
          current_ideal->rank);
        start = 0;
      }
      else
      {
        start = IDELEMS(syzstr->res[index]);
        while ((start>0) && (syzstr->res[index]->m[start-1]==NULL)) start--;
        if (IDELEMS(syzstr->res[index])<start+IDELEMS(current_ideal))
        {
          pEnlargeSet(&syzstr->res[index]->m,IDELEMS(syzstr->res[index]),
                   IDELEMS(current_ideal));
          IDELEMS(syzstr->res[index]) += IDELEMS(current_ideal);
          pEnlargeSet(&syzstr->orderedRes[index]->m,IDELEMS(syzstr->orderedRes[index]),
                   IDELEMS(current_ideal));
          IDELEMS(syzstr->orderedRes[index]) += IDELEMS(current_ideal);
        }
      }
      if (idIs0(totake[index]))
      {
        totake[index] = idInit(IDELEMS(current_ideal),
          current_ideal->rank+current_tl);
        start_ttk = 0;
      }
      else
      {
        start_ttk = IDELEMS(totake[index]);
        while ((start_ttk>0) && (totake[index]->m[start_ttk-1]==NULL)) start_ttk--;
        if (IDELEMS(totake[index])<start_ttk+IDELEMS(current_ideal))
        {
          pEnlargeSet(&totake[index]->m,IDELEMS(totake[index]),
                   IDELEMS(current_ideal));
          for (j=IDELEMS(totake[index]);j<IDELEMS(totake[index])+
                                  IDELEMS(current_ideal);j++)
            totake[index]->m[j] = NULL;
          IDELEMS(totake[index]) += IDELEMS(current_ideal);
        }
      }
      for (i=0;i<IDELEMS(current_ideal);i++)
      {
        if (current_ideal->m[i]!=NULL)
        {
          syzstr->res[index]->m[i+start] = pCopy(current_ideal->m[i]);
          syzstr->res[index]->m[i+start] = pMult_mm(syzstr->res[index]->m[i+start],w_gen);
          p_Shift(&syzstr->res[index]->m[i+start],current_tl,currRing);
          syzstr->res[index]->m[i+start] = pAdd(syzstr->res[index]->m[i+start],
            ppMult_qq(current_repr->m[i],p));
          syzstr->orderedRes[index]->m[i+start] = pCopy(current_repr->m[i]);
          syzstr->orderedRes[index]->m[i+start] =
            pMult_mm(syzstr->orderedRes[index]->m[i+start],w_gen);
          if ((*syzstr->Tl)[index]!=0)
            p_Shift(&syzstr->orderedRes[index]->m[i+start],(*syzstr->Tl)[index],currRing);
        }
      }
      for (i=0;i<IDELEMS(totake[index-1]);i++)
      {
        if (totake[index-1]->m[i]!=NULL)
        {
          if ((index==1) && ((i==IDELEMS(current_ideal) ||
               (totake[index-1]->m[i+1]==NULL)))) break;
          totake[index]->m[i+start_ttk] =
            pMult_mm(pCopy(totake[index-1]->m[i]),w_gen);
          p_Shift(&totake[index]->m[i+start_ttk],current_tl,currRing);
#ifdef FULL_TOTAKE
          poly pp=pCopy(p);
          p_Shift(&pp,i+1,currRing);
          totake[index]->m[i+start_ttk] = pAdd(totake[index]->m[i+start_ttk],pp);
#endif
        }
      }
      (*syzstr->Tl)[index] += current_tl;
    }
    index--;
  }
  pDelete(&gen);
  pDelete(&neg_gen);
  pDelete(&w_gen);
  //syShowRes(syzstr);
}

/*3
* proves the consistence of the pairset resPairs with the corresponding
* set of generators;
* only for tests
*/
static void syTestPairs(SSet resPairs,int length,ideal old_generators)
{
  int i=0;

  while (i<length)
  {
    if (resPairs[i].lcm!=NULL)
    {
      if (resPairs[i].p1!=NULL)
        assume(resPairs[i].p1==old_generators->m[resPairs[i].ind1]);
      if (resPairs[i].p2!=NULL)
        assume(resPairs[i].p2==old_generators->m[resPairs[i].ind2]);
    }
    i++;
  }
}

/*3
* cancels the weight monomials given by the leading terms of totake
* from the resolution res;
* works in place on res, but reads only from totake
*/
void syReorder_Kosz(syStrategy syzstr)
{
  int length=syzstr->length;
  int syzIndex=length-1,i,j;
  resolvente res=syzstr->fullres;
  poly p;

  while ((syzIndex!=0) && (res[syzIndex]==NULL)) syzIndex--;
  while (syzIndex>0)
  {
    for(i=0;i<IDELEMS(res[syzIndex]);i++)
    {
#ifdef USE_REGULARITY
      if ((syzstr->regularity>0) && (res[syzIndex]->m[i]!=NULL))
      {
        if (p_FDeg(res[syzIndex]->m[i],currRing)>=syzstr->regularity+syzIndex)
          pDelete(&res[syzIndex]->m[i]);
      }
#endif
      p = res[syzIndex]->m[i];
      while (p!=NULL)
      {
        if (res[syzIndex-1]->m[pGetComp(p)-1]!=NULL)
        {
          for(j=1;j<=(currRing->N);j++)
          {
            pSetExp(p,j,pGetExp(p,j)
                        -pGetExp(res[syzIndex-1]->m[pGetComp(p)-1],j));
          }
        }
        else
          PrintS("error in the resolvent\n");
        pSetm(p);
        pIter(p);
      }
    }
    syzIndex--;
  }
}

/*3
* updates the pairset resPairs by generating all pairs including the
* new_generators in the 0-th modul;
* the new_generators are inserted in the old_generators;
* new_generators is empty after the procedure;
*/
static void updatePairs(SSet *resPairs,int *l_pairs,syStrategy syzstr,
       int index,ideal new_generators,ideal new_repr,int crit_comp)
{
  if (idIs0(new_generators)) return;
  ideal old_generators=syzstr->res[index];
  ideal old_repr=syzstr->orderedRes[index];
  int i=0,j,k,kk,og_elem=0,og_idel=IDELEMS(old_generators),l=*l_pairs,jj,ll,j1;
  int og_ini=0;
  ideal pairs=idInit(og_idel+IDELEMS(new_generators),old_generators->rank);
  polyset prs=pairs->m;
  poly p=NULL;
  SObject tso;

  syInitializePair(&tso);
  while ((og_elem<og_idel) && (old_generators->m[og_elem]!=NULL))
  {
    if ((index>0) && (pGetComp(old_generators->m[og_elem])<=crit_comp))
      og_ini = og_elem;
    og_elem++;
  }
  while ((l>0) && ((*resPairs)[l-1].lcm==NULL)) l--;
  while ((i<IDELEMS(new_generators)) && (new_generators->m[i]!=NULL))
  {
    syTestPairs(*resPairs,*l_pairs,old_generators);
    if (IDELEMS(old_generators)==og_elem)
    {
      pEnlargeSet(&old_generators->m,IDELEMS(old_generators),16);
      IDELEMS(old_generators) += 16;
      pEnlargeSet(&old_repr->m,IDELEMS(old_repr),16);
      IDELEMS(old_repr) += 16;
    }
    k = p_FDeg(new_generators->m[i],currRing);
    kk = pGetComp(new_generators->m[i]);
    j = og_ini;
    while ((j<og_elem) && (old_generators->m[j]!=NULL) &&
           (pGetComp(old_generators->m[j])<kk)) j++;
    while ((j<og_elem) && (old_generators->m[j]!=NULL) &&
           (p_FDeg(old_generators->m[j],currRing)<=k)) j++;
    for (jj=og_elem;jj>j;jj--)
    {
      old_generators->m[jj] = old_generators->m[jj-1];
      old_repr->m[jj] = old_repr->m[jj-1];
    }
    old_generators->m[j] = new_generators->m[i];
    new_generators->m[i] = NULL;
    old_repr->m[j] = new_repr->m[i];
    new_repr->m[i] = NULL;
    og_elem++;
    for (jj=0;jj<*l_pairs;jj++)
    {
      if ((*resPairs)[jj].lcm!=NULL)
      {
        if ((*resPairs)[jj].ind1>=j) (*resPairs)[jj].ind1++;
        if ((*resPairs)[jj].ind2>=j) (*resPairs)[jj].ind2++;
      }
    }
    syTestPairs(*resPairs,*l_pairs,old_generators);
    for (jj=og_ini;jj<og_elem;jj++)
    {
      if ((j!=jj) && (pGetComp(old_generators->m[jj])==pGetComp(old_generators->m[j])))
      {
        p = pOne();
        pLcm(old_generators->m[jj],old_generators->m[j],p);
        pSetComp(p,j+1);
        pSetm(p);
        j1 = 0;
        while (j1<jj)
        {
          if (prs[j1]!=NULL)
          {
            if (pLmDivisibleByNoComp(prs[j1],p))
            {
              pDelete(&p);
              break;
            }
            else if (pLmDivisibleByNoComp(p,prs[j1]))
            {
              pDelete(&(prs[j1]));
            }
#ifdef USE_CHAINCRIT0
            else
            {
              poly p1,p2;
              int ip=(currRing->N);
              p1 = pMDivide(p,old_generators->m[jj]);
              p2 = pMDivide(prs[j1],old_generators->m[j1]);
              while ((ip>0) && (pGetExp(p1,ip)*pGetExp(p2,ip)==0)) ip--;
              if (ip==0)
              {
                int ti=0;
                while ((ti<l) && (((*resPairs)[ti].ind1!=j1)|| ((*resPairs)[ti].ind2!=jj))) ti++;
                if (ti<l)
                {
                  if (TEST_OPT_PROT) PrintS("cc");
                  syDeletePair(&(*resPairs)[ti]);
                  syCompactifyPairSet(*resPairs,*l_pairs,ti);
                  l--;
                }
              }
              pDelete(&p1);
              pDelete(&p2);
            }
#endif
          }
          j1++;
        }
        if (p!=NULL)
          prs[jj] = p;
      }
    }
    for (jj=og_ini;jj<og_elem;jj++)
    {
      if (prs[jj] !=NULL)
      {
        if (l>=*l_pairs)
        {
          SSet temp = (SSet)omAlloc0((*l_pairs+16)*sizeof(SObject));
          for (ll=0;ll<*l_pairs;ll++)
          {
            temp[ll].p = (*resPairs)[ll].p;
            temp[ll].p1 = (*resPairs)[ll].p1;
            temp[ll].p2 = (*resPairs)[ll].p2;
            temp[ll].syz = (*resPairs)[ll].syz;
            temp[ll].lcm = (*resPairs)[ll].lcm;
            temp[ll].ind1 = (*resPairs)[ll].ind1;
            temp[ll].ind2 = (*resPairs)[ll].ind2;
            temp[ll].syzind = (*resPairs)[ll].syzind;
            temp[ll].order = (*resPairs)[ll].order;
            temp[ll].isNotMinimal = (*resPairs)[ll].isNotMinimal;
          }
          omFreeSize((ADDRESS)(*resPairs),*l_pairs*sizeof(SObject));
          *l_pairs += 16;
          (*resPairs) = temp;
        }
        tso.lcm = prs[jj];
        prs[jj] = NULL;
        tso.order = p_FDeg(tso.lcm,currRing);
        tso.p1 = old_generators->m[jj];
        tso.p2 = old_generators->m[j];
        tso.ind1 = jj;
        tso.ind2 = j;
        tso.syzind = -1;
        tso.isNotMinimal = NULL;
        tso.p = NULL;
        tso.syz = NULL;
        SSet rP=*resPairs;
#ifdef SHOW_PROT
Print("erzeuge Paar im Modul %d,%d mit: \n",index,tso.order);
PrintS("poly1: ");pWrite(tso.p1);
PrintS("poly2: ");pWrite(tso.p2);
PrintS("syz: ");pWrite(tso.syz);
PrintS("sPoly: ");pWrite(tso.p);
PrintLn();
#endif
        syEnterPair(rP,&tso,&l,index);
        syInitializePair(&tso);
      }
    }
    i++;
  }
  idDelete(&pairs);
}

/*3
* performs the modification of a single reduction on the syzygy-level
*/
inline void sySPRedSyz_Kosz(syStrategy syzstr,poly redWith,poly syz,poly q=NULL,int l_syz=-1)
{
  poly p=pMDivide(q,redWith);
  pSetCoeff(p,nDiv(pGetCoeff(q),pGetCoeff(redWith)));
  kBucket_Minus_m_Mult_p(syzstr->syz_bucket,p,syz,&l_syz,NULL);
  pDelete(&p);
}

/*3
* normalizes the poly bucket by the ideal;
* stops the reduction whenever the leading component is less than the
* crit_comp;
* returns the changing status
*/
static BOOLEAN syRedSyz(kBucket_pt bucket,ideal red,int crit_comp,int* g_l)
{
  poly p = kBucketGetLm(bucket);
  int j = 0,i=IDELEMS(red)-1;
  number n;
  BOOLEAN isChanged=FALSE;

  loop
  {
    if ((j>=i) || (p==NULL) || (pGetComp(p)<=crit_comp)) break;
    if ((red->m[j]!=NULL) && (pDivisibleBy(red->m[j],p)))
    {
      n = kBucketPolyRed(bucket,red->m[j], g_l[j], NULL, FALSE);
      nDelete(&n);
      p = kBucketGetLm(bucket);
      isChanged = TRUE;
      j = 0;
    }
    else
      j++;
  }
  return isChanged;
}

/*3
* a tail reduction for the syzygies yielding new generators
*/
static poly syRedTailSyz(poly tored,ideal red,ideal sec_red,int crit_comp,syStrategy syzstr,
            int * gen_length,int * secgen_length,int * tored_length)
{
  int i=IDELEMS(red)-1,num_mon,num_tail;
  poly h,hn;
  // BOOLEAN dummy;

  while ((i>0) && (red->m[i-1]==NULL)) i--;
  i--;
  h = tored;
  if ((h!=NULL) && (pGetComp(h)>crit_comp))
  {
    num_mon = 1;
    hn = pNext(h);
    num_tail = *tored_length-1;
    while (hn!=NULL)
    {
      kBucketInit(syzstr->syz_bucket,hn,num_tail);
      /*dummy =*/ (void) syRedSyz(syzstr->syz_bucket,red,crit_comp,gen_length);
      kBucketClear(syzstr->syz_bucket,&hn,&num_tail);
      pNext(h) = hn;
      if ((hn==NULL) || (pGetComp(hn)<=crit_comp))
        break;
      else
      {
        pIter(h);
        pIter(hn);
        num_mon++;
        num_tail--;
      }
    }
    if (sec_red!=NULL)
    {
      while (hn!=NULL)
      {
        kBucketInit(syzstr->syz_bucket,hn,num_tail);
        /*dummy =*/ (void) syRedSyz(syzstr->syz_bucket,sec_red,crit_comp,secgen_length);
        kBucketClear(syzstr->syz_bucket,&hn,&num_tail);
        pNext(h) = hn;
        if (hn==NULL)
          break;
        else
        {
          pIter(h);
          pIter(hn);
          num_mon++;
          num_tail--;
        }
      }
    }
    *tored_length = num_mon+num_tail;
  }
  assume(pLength(tored)==*tored_length);
  return tored;
}

#if 0
// unused
/*3
* the complete reduction of a single pair which is just stored
* in bucket and syz_bucket
*/
static BOOLEAN syRedSyzPair(syStrategy syzstr,int index,int* g_l,int* orp_l)
{
  kBucket_pt bucket=syzstr->bucket;
  poly p = kBucketGetLm(bucket);
  ideal red=syzstr->res[index],repr=syzstr->orderedRes[index];
  int j = 0,i=IDELEMS(red)-1;
  number n;
  BOOLEAN isChanged=FALSE;

  loop
  {
    if ((j>=i) || (p==NULL)) break;
    if ((red->m[j]!=NULL) && (pDivisibleBy(red->m[j],p)))
    {
      sySPRedSyz_Kosz(syzstr,red->m[j],repr->m[j],p,orp_l[j]);
      n = kBucketPolyRed(bucket,red->m[j], g_l[j], NULL);
      nDelete(&n);
      p = kBucketGetLm(bucket);
      isChanged = TRUE;
      j = 0;
    }
    else
      j++;
  }
  return isChanged;
}
#endif

/*3
* the tailreduction for generators (which includes the correction of
* the corresponding representation)
*/
#if 0 /*unused*/
static void syRedTailSyzPair(SObject tso,syStrategy syzstr,int index,
            int * gen_length,int* orp_l,int * tored_l,int * syzred_l)
{
  int num_mon,num_tail,syz_l;
  poly h,hn;
  BOOLEAN dummy;

  h = tso.p;
  kBucketInit(syzstr->syz_bucket,tso.syz,*syzred_l);
  if (h!=NULL)
  {
    num_mon = 1;
    hn = pNext(h);
    num_tail = *tored_l-1;
    while (hn!=NULL)
    {
      kBucketInit(syzstr->bucket,hn,num_tail);
      dummy = syRedSyzPair(syzstr,index,gen_length,orp_l);
      kBucketClear(syzstr->bucket,&hn,&num_tail);
      pNext(h) = hn;
      if (hn==NULL)
        break;
      else
      {
        pIter(h);
        pIter(hn);
        num_mon++;
        num_tail--;
      }
    }
    *tored_l = num_mon+num_tail;
  }
  kBucketClear(syzstr->syz_bucket,&tso.syz,&syz_l);
  assume(pLength(tso.syz)==syz_l);
  assume(pLength(tso.p)==*tored_l);
}
#endif

/*3
* the reduction of a pair in the 0-th module
*/
static void redOnePair(SSet resPairs,int itso,int l, ideal syzygies,
            int crit_comp, syStrategy syzstr,int index,ideal new_generators,
            ideal new_repr,int * ogm_l,int * orp_l)
{
  SObject tso = resPairs[itso];
  assume (tso.lcm!=NULL);
  ideal old_generators=syzstr->res[index];
  ideal old_repr=syzstr->orderedRes[index];
  int og_idel=IDELEMS(old_generators),ng_place=IDELEMS(new_generators);
  int toReplace=0;
  int i,j,syz_l;
  number /*coefgcd,*/n;
  polyset ogm=old_generators->m;
  poly p;
  BOOLEAN deleteP=FALSE;
#ifdef EXPERIMENT1
  poly syzp;
#endif
  int syz_place=IDELEMS(syzygies);

  while ((syz_place>0) && (syzygies->m[syz_place-1]==NULL)) syz_place--;
  while ((ng_place>0) && (new_generators->m[ng_place-1]==NULL)) ng_place--;
  while ((og_idel>0) && (old_generators->m[og_idel-1]==NULL)) og_idel--;
  assume (tso.ind1<og_idel);
  assume (tso.ind2<og_idel);
  assume (tso.ind1!=tso.ind2);
  assume (tso.p1 == old_generators->m[tso.ind1]);
  assume (tso.p2 == old_generators->m[tso.ind2]);
  tso.p1 = old_generators->m[tso.ind1];
  tso.p2 = old_generators->m[tso.ind2];
  if ((tso.p1!=NULL) && (tso.p2!=NULL))
  {
    if (TEST_OPT_PROT)
      PrintS(".");
    if (index==0)
    {
/*--- tests wether a generator must be replaced (lt(f1)|lt(f2)!)--*/
      if (p_FDeg(tso.p1,currRing)==p_FDeg(tso.lcm,currRing))
        toReplace = tso.ind1+1;
      else if (p_FDeg(tso.p2,currRing)==p_FDeg(tso.lcm,currRing))
        toReplace = tso.ind2+1;
    }
#ifdef EXPERIMENT3
/*--- tests wether the product criterion applies --------------*/
    if ((index==0) && (old_generators->rank==1) &&
        (p_FDeg(tso.p1,currRing)+p_FDeg(tso.p2,currRing)==tso.order))
    {
      tso.p = NULL;
      p = pCopy(tso.p1);
      p_Shift(&p,-1,currRing);
#ifdef WITH_BUCKET
      poly pp;
      pp = pMult_mm(pCopy(old_repr->m[tso.ind2]),p);
      kBucketInit(syzstr->syz_bucket,pp,-1);
      pLmDelete(&p);
      p = pNeg(p);
      pp = pCopy(old_repr->m[tso.ind2]);
      int il=-1;
      while (p!=NULL)
      {
        kBucket_Minus_m_Mult_p(syzstr->syz_bucket,p,pp,&il,NULL);
        pLmDelete(&p);
      }
      pDelete(&pp);
      p = pCopy(tso.p2);
      p_Shift(&p,-1,currRing);
      pp = pCopy(old_repr->m[tso.ind1]);
      il=-1;
      while (p!=NULL)
      {
        kBucket_Minus_m_Mult_p(syzstr->syz_bucket,p,pp,&il,NULL);
        pLmDelete(&p);
      }
      pDelete(&pp);
      kBucketClear(syzstr->syz_bucket,&tso.syz,&j);
#else
      tso.syz = pMult(p,pCopy(old_repr->m[tso.ind2]));
      p = pCopy(tso.p2);
      p_Shift(&p,-1,currRing);
      tso.syz = pSub(tso.syz,pMult(p,pCopy(old_repr->m[tso.ind1])));
#endif
    }
    else
#endif
/*--- the product criterion does not apply --------------------*/
    {
      tso.p = ksOldCreateSpoly(tso.p2,tso.p1);
      number coefgcd = n_Gcd(pGetCoeff(tso.p1),pGetCoeff(tso.p2),currRing->cf);
      assume (old_repr->m[tso.ind1]!=NULL);
      tso.syz = pCopy(old_repr->m[tso.ind1]);
      poly tt = pMDivide(tso.lcm,tso.p1);
      pSetComp(tt,0);
      pSetmComp(tt);
      pSetCoeff(tt,nDiv(pGetCoeff(tso.p1),coefgcd));
      tso.syz = pMult_mm(tso.syz,tt);
      pDelete(&tt);
      coefgcd = nInpNeg(coefgcd);
      assume (old_repr->m[tso.ind2]!=NULL);
      p = pCopy(old_repr->m[tso.ind2]);
      tt = pMDivide(tso.lcm,tso.p2);
      pSetComp(tt,0);
      pSetmComp(tt);
      pSetCoeff(tt,nDiv(pGetCoeff(tso.p2),coefgcd));
      p = pMult_mm(p,tt);
      pDelete(&tt);
      tso.syz = pAdd(p,tso.syz);
#ifdef EXPERIMENT2
      if ((tso.syz!=NULL) && (pGetComp(tso.syz)<=crit_comp))
      {
/*--- breaks when the leading component is less than crit_comp ------*/
        deleteP = TRUE;
        discard_pairs++;
      }
#endif
      nDelete(&coefgcd);
    }                             //End of the else-part of EXPERIMENT3
#ifdef SHOW_PROT
Print("reduziere Paar im Module %d mit: \n",index);
PrintS("poly1: ");pWrite(tso.p1);
PrintS("poly2: ");pWrite(tso.p2);
PrintS("syz: ");pWrite(tso.syz);
PrintS("sPoly: ");pWrite(tso.p);
#endif
    assume(tso.syz!=NULL);
    kBucketInit(syzstr->syz_bucket,tso.syz,-1);
    if ((tso.p!=NULL) && (!deleteP))
    {
      kBucketInit(syzstr->bucket,tso.p,-1);
      p = kBucketGetLm(syzstr->bucket);
      j = 0;
      loop
      {
        if (j>=og_idel)
        {
/*--- reduction with generators computed in this procedure ---*/
          j = 0;
          while ((j<ng_place) && (!pDivisibleBy(new_generators->m[j],p))) j++;
          if (j>=ng_place) break;
          assume (new_repr->m[j]!=NULL);
          sySPRedSyz_Kosz(syzstr,new_generators->m[j],new_repr->m[j],p);
          n = kBucketPolyRed(syzstr->bucket,new_generators->m[j],
                             pLength(new_generators->m[j]), NULL, FALSE);
          p = kBucketGetLm(syzstr->bucket);
#ifdef EXPERIMENT1
          syzp = kBucketGetLm(syzstr->syz_bucket);
          if ((syzp!=NULL) && (pGetComp(syzp)<=crit_comp))
          {
            deleteP =TRUE;
            break;
          }
          //if (syzp==NULL)
            //assume(p==NULL);
          //else
            //if (pGetComp(syzp)<=crit_comp) short_pairs++;
#endif
          if (p==NULL) break;
          j = 0;
        }
        if (pDivisibleBy(ogm[j],p))
        {
/*--- reduction with general old generators ---------------------*/
          assume (old_repr->m[j]!=NULL);
          sySPRedSyz_Kosz(syzstr,ogm[j],old_repr->m[j],p,orp_l[j]);
          n = kBucketPolyRed(syzstr->bucket,ogm[j],ogm_l[j], NULL,FALSE);
          p = kBucketGetLm(syzstr->bucket);
#ifdef EXPERIMENT1
          syzp = kBucketGetLm(syzstr->syz_bucket);
          if ((syzp!=NULL) && (pGetComp(syzp)<=crit_comp))
          {
            break;
            deleteP =TRUE;
          }
          //if (syzp==NULL)
            //assume(p==NULL);
          //else
            //if ((pGetComp(syzp)<=crit_comp) && (p!=NULL)) short_pairs++;
#endif
          if (p==NULL) break;
          j = 0;
        }
        else
          j++;
      }
      kBucketClear(syzstr->bucket,&tso.p,&tso.length);
    }
    kBucketClear(syzstr->syz_bucket,&tso.syz,&syz_l);
    if (deleteP)
    {
      pDelete(&tso.p);
      pDelete(&tso.syz);
    }
  }
  else
  {
    PrintS("Shit happens!\n");
  }
#ifdef SHOW_PROT
Print("erhalte Paar im Module %d mit: \n",index);
PrintS("syz: ");pWrite(tso.syz);
PrintS("sPoly: ");pWrite(tso.p);
PrintLn();
#endif
  if (toReplace)
  {
/*-- replaces the generator if neccesary ------------------*/
    pDelete(&old_generators->m[toReplace-1]);
    pDelete(&old_repr->m[toReplace-1]);
    for (i=toReplace-1;i<og_idel-1;i++)
    {
      old_generators->m[i] = old_generators->m[i+1];
      old_repr->m[i] = old_repr->m[i+1];
    }
    old_generators->m[og_idel-1] = NULL;
    old_repr->m[og_idel-1] = NULL;
    for (i=itso+1;i<l;i++)
    {
      if (resPairs[i].lcm!=NULL)
      {
        if ((resPairs[i].ind1==toReplace-1)||(resPairs[i].ind2==toReplace-1))
          syDeletePair(&resPairs[i]);
        else
        {
          if (resPairs[i].ind1>=toReplace)
            (resPairs[i].ind1)--;
          if (resPairs[i].ind2>=toReplace)
            (resPairs[i].ind2)--;
        }
      }
    }
    syCompactifyPairSet(resPairs,l,itso+1);
  }
  if (tso.p!=NULL)
  {
/*-- stores the new generator ---------------------------------*/
    //syRedTailSyzPair(tso,syzstr,index,ogm_l,orp_l,&tso.length,&syz_l);
    if (ng_place>=IDELEMS(new_generators))
    {
      pEnlargeSet(&new_generators->m,IDELEMS(new_generators),16);
      IDELEMS(new_generators) += 16;
      pEnlargeSet(&new_repr->m,IDELEMS(new_repr),16);
      IDELEMS(new_repr) += 16;
    }
    if (!nIsOne(pGetCoeff(tso.p)))
    {
      n=nInvers(pGetCoeff(tso.p));
      pNorm(tso.p);
      tso.syz=__p_Mult_nn(tso.syz,n,currRing);
      nDelete(&n);
    }
    new_generators->m[ng_place] = tso.p;
    tso.p = NULL;
    new_repr->m[ng_place] = tso.syz;
    tso.syz = NULL;
  }
  else
  {
/*--- takes the syzygy as new generator of the next module ---*/
    if (tso.syz==NULL)
    {
#ifndef EXPERIMENT2
#ifdef EXPERIMENT3
      short_pairs++;
#endif
#endif
    }
    else if (pGetComp(tso.syz)<=crit_comp)
    {
      pDelete(&tso.syz);
    }
    else
    {
      if (syz_place>=IDELEMS(syzygies))
      {
        pEnlargeSet(&syzygies->m,IDELEMS(syzygies),16);
        IDELEMS(syzygies) += 16;
      }
      syzygies->m[syz_place] = tso.syz;
      tso.syz = NULL;
      pNorm(syzygies->m[syz_place]);
    }
  }
  resPairs[itso] = tso;
  syDeletePair(&resPairs[itso]);
  syTestPairs(resPairs,l,old_generators);
}

/*3
* reduction of all pairs of a fixed degree of the 0-th module
*/
static BOOLEAN redPairs(SSet resPairs,int l_pairs, ideal syzygies,
  ideal new_generators,ideal new_repr, int crit_comp,syStrategy syzstr,
  int index)
{
  if (resPairs[0].lcm==NULL) return TRUE;
  int i,j,actdeg=resPairs[0].order;
  int * ogm_l=(int*)omAlloc0(IDELEMS(syzstr->res[index])*sizeof(int));
  int * orp_l=(int*)omAlloc0(IDELEMS(syzstr->orderedRes[index])*sizeof(int));
  // int t1=IDELEMS(syzstr->res[index]),t2=IDELEMS(syzstr->orderedRes[index]);

  for (j=IDELEMS(syzstr->res[index])-1;j>=0;j--)
  {
    if (syzstr->res[index]->m[j]!=NULL)
      ogm_l[j] = pLength(syzstr->res[index]->m[j]);
  }
  for (j=IDELEMS(syzstr->orderedRes[index])-1;j>=0;j--)
  {
    if (syzstr->orderedRes[index]->m[j]!=NULL)
      orp_l[j] = pLength(syzstr->orderedRes[index]->m[j]);
  }
  loop
  {
    i = 0;
    if (TEST_OPT_PROT)
      Print("(%d,%d)",index,resPairs[0].order);
    while (resPairs[i].order==actdeg)
    {
      syTestPairs(resPairs,l_pairs,syzstr->res[index]);
      redOnePair(resPairs,i,l_pairs,syzygies,crit_comp,syzstr,index,
                 new_generators, new_repr,ogm_l,orp_l);
      i++;
      syTestPairs(resPairs,l_pairs,syzstr->res[index]);
    }
    syTestPairs(resPairs,l_pairs,syzstr->res[index]);
    syCompactifyPairSet(resPairs,l_pairs,0);
    syTestPairs(resPairs,l_pairs,syzstr->res[index]);
    if (!idIs0(new_generators))
      break;
    else if (resPairs[0].lcm==NULL)  //there are no pairs left and no new_gens
    {
      omFreeSize((ADDRESS)ogm_l,IDELEMS(syzstr->res[index])*sizeof(int));
      omFreeSize((ADDRESS)orp_l,IDELEMS(syzstr->orderedRes[index])*sizeof(int));
      return TRUE;
    }
    else
      actdeg = resPairs[0].order;
  }
  syTestPairs(resPairs,l_pairs,syzstr->res[index]);
  omFreeSize((ADDRESS)ogm_l,IDELEMS(syzstr->res[index])*sizeof(int));
  omFreeSize((ADDRESS)orp_l,IDELEMS(syzstr->orderedRes[index])*sizeof(int));
  return FALSE;
}

/*3
* extends the standard basis old_generators with new_generators;
* returns the syzygies which involve the new elements;
* assumes that the components of the new_generators are sperated
* from those of old_generators, i.e. whenever the leading term
* of a syzygy lies in the part of the old_generators, the syzygy
* lie just in the module old_generators
* assumes that the new_generators are reduced w.r.t. old_generators
*/
static ideal kosz_std(ideal new_generators,ideal new_repr,syStrategy syzstr,
                      int index,int next_comp)
{
  int og_idel=IDELEMS(syzstr->res[index]);
  int l_pairs=2*og_idel;
  ideal syzygies=idInit(16,syzstr->res[index]->rank+1);
  if ((idIs0(new_generators)) || (new_generators->m[0]==NULL))
  {
    WerrorS("Hier ist was faul!\n");
    return NULL;
  }
  SSet resPairs=(SSet)omAlloc0(l_pairs*sizeof(SObject));
  loop
  {
    updatePairs(&resPairs,&l_pairs,syzstr,index,
                new_generators,new_repr,next_comp);
    if (redPairs(resPairs,l_pairs,syzygies, new_generators,new_repr,
                 next_comp,syzstr,index)) break;
  }
  omFreeSize((SSet)resPairs,l_pairs*sizeof(SObject));
  return syzygies;
}

/*3
* normalizes the incoming generators
*/
static poly normalize(poly next_p,ideal add_generators, syStrategy syzstr,
                      int * g_l,int * p_l,int crit_comp)
{
  int j=0,i=IDELEMS(add_generators);
  kBucketInit(syzstr->bucket,next_p,pLength(next_p));
  poly p = kBucketGetLm(syzstr->bucket),result;
  number n;

  loop
  {
    if ((j>=i) || (p==NULL) || (pGetComp(p)<=crit_comp)) break;
    if ((add_generators->m[j]!=NULL) && (pDivisibleBy(add_generators->m[j],p)))
    {
      n = kBucketPolyRed(syzstr->bucket,add_generators->m[j], g_l[j],
                         NULL, FALSE);
      nDelete(&n);
      p = kBucketGetLm(syzstr->bucket);
      j = 0;
    }
    else
      j++;
  }
  kBucketClear(syzstr->bucket,&result,p_l);
  return result;
}

/*3
* updates the pairs inthe higher modules
*/
static void updatePairsHIndex(SSet *resPairs,int *l_pairs,syStrategy /*syzstr*/,
       int index,ideal add_generators,ideal /*add_repr*/,ideal /*new_generators*/,
       ideal /*new_repr*/,int /*crit_comp*/,int* first_new)
{
  int i=*first_new,l=*l_pairs,j,ll,j1,add_idel=IDELEMS(add_generators);
  ideal pairs=idInit(add_idel,add_generators->rank);
  polyset prs=pairs->m;
  poly p=NULL;
  SObject tso;

  syInitializePair(&tso);
  while ((l>0) && ((*resPairs)[l-1].lcm==NULL)) l--;
  while ((i<add_idel) && (add_generators->m[i]!=NULL))
  {
    for (j=0;j<i;j++)
    {
      if (pGetComp(add_generators->m[j]) == pGetComp(add_generators->m[i]))
      {
        p = pOne();
        pLcm(add_generators->m[j],add_generators->m[i],p);
        pSetComp(p,i+1);
        pSetm(p);
        j1 = 0;
        while (j1<j)
        {
          if (prs[j1]!=NULL)
          {
            if (pLmDivisibleByNoComp(prs[j1],p))
            {
              pDelete(&p);
              break;
            }
            else if (pLmDivisibleByNoComp(p,prs[j1]))
            {
              pDelete(&(prs[j1]));
            }
#ifdef USE_CHAINCRIT
            else
            {
              poly p1,p2;
              int ip=(currRing->N);
              p1 = pMDivide(p,add_generators->m[j]);
              p2 = pMDivide(prs[j1],add_generators->m[j1]);
              while ((ip>0) && (pGetExp(p1,ip)*pGetExp(p2,ip)==0)) ip--;
              if (ip==0)
              {
                int ti=0;
                while ((ti<l) && (((*resPairs)[ti].ind1!=j1)|| ((*resPairs)[ti].ind2!=j))) ti++;
                if (ti<l)
                {
                  if (TEST_OPT_PROT) PrintS("cc");
                  syDeletePair(&(*resPairs)[ti]);
                  syCompactifyPairSet(*resPairs,*l_pairs,ti);
                  l--;
                }
              }
              pDelete(&p1);
              pDelete(&p2);
            }
#endif
          }
          j1++;
        }
        if (p!=NULL)
          prs[j] = p;
      }
    }
    for (j=0;j<i;j++)
    {
      if (prs[j] !=NULL)
      {
        if (l>=*l_pairs)
        {
          SSet temp = (SSet)omAlloc0((*l_pairs+16)*sizeof(SObject));
          for (ll=0;ll<*l_pairs;ll++)
          {
            temp[ll].p = (*resPairs)[ll].p;
            temp[ll].p1 = (*resPairs)[ll].p1;
            temp[ll].p2 = (*resPairs)[ll].p2;
            temp[ll].syz = (*resPairs)[ll].syz;
            temp[ll].lcm = (*resPairs)[ll].lcm;
            temp[ll].ind1 = (*resPairs)[ll].ind1;
            temp[ll].ind2 = (*resPairs)[ll].ind2;
            temp[ll].syzind = (*resPairs)[ll].syzind;
            temp[ll].order = (*resPairs)[ll].order;
            temp[ll].isNotMinimal = (*resPairs)[ll].isNotMinimal;
          }
          omFreeSize((ADDRESS)(*resPairs),*l_pairs*sizeof(SObject));
          *l_pairs += 16;
          (*resPairs) = temp;
        }
        tso.lcm = prs[j];
        prs[j] = NULL;
        tso.order = p_FDeg(tso.lcm,currRing);
        tso.p1 = add_generators->m[j];
        tso.p2 = add_generators->m[i];
        tso.ind1 = j;
        tso.ind2 = i;
        tso.syzind = -1;
        tso.isNotMinimal = NULL;
        tso.p = NULL;
        tso.syz = NULL;
        SSet rP=*resPairs;
#ifdef SHOW_PROT
Print("erzeuge Paar im Modul %d,%d mit: \n",index,tso.order);
PrintS("poly1: ");pWrite(tso.p1);
PrintS("poly2: ");pWrite(tso.p2);
PrintS("syz: ");pWrite(tso.syz);
PrintS("sPoly: ");pWrite(tso.p);
PrintLn();
#endif
        syEnterPair(rP,&tso,&l,index);
        syInitializePair(&tso);
      }
    }
    i++;
  }
  *first_new = i;
  idDelete(&pairs);
}

/*3
* reduction of a single pair in the higher moduls
*/
#ifdef SHOW_PROT
static void redOnePairHIndex(SSet resPairs,int itso, int crit_comp,
            syStrategy syzstr,int index,ideal add_generators, ideal add_repr,
            ideal new_generators, ideal new_repr,int * next_place_add,int ** g_l,
            poly deg_soc)
#else
static void redOnePairHIndex(SSet resPairs,int itso, int crit_comp,
            syStrategy syzstr,int /*index*/,ideal add_generators, ideal add_repr,
            ideal new_generators, ideal new_repr,int * next_place_add,int ** g_l,
            poly deg_soc)
#endif
{
  SObject tso = resPairs[itso];
  assume (tso.lcm!=NULL);
  int ng_place=IDELEMS(new_generators);
  int i,j;
  number n;
  poly p;
#ifdef EXPERIMENT1
  poly syzp;
#endif

  assume (tso.ind1<*next_place_add);
  assume (tso.ind2<*next_place_add);
  assume (tso.ind1!=tso.ind2);
  assume (tso.p1 == add_generators->m[tso.ind1]);
  assume (tso.p2 == add_generators->m[tso.ind2]);
  tso.p1 = add_generators->m[tso.ind1];
  tso.p2 = add_generators->m[tso.ind2];
  if ((tso.p1!=NULL) && (tso.p2!=NULL))
  {
    if (TEST_OPT_PROT)
      PrintS(".");
#ifdef USE_PROD_CRIT
    if (p_FDeg(tso.p1,currRing)+p_FDeg(tso.p2,currRing)==tso.order+p_FDeg(deg_soc,currRing))
    {
      if (TEST_OPT_PROT) PrintS("pc");
      int ac=pGetComp(tso.p1);
      assume(ac=pGetComp(tso.p2));
      poly p1=pCopy(tso.p1);
      poly p2=pCopy(tso.p2);
      poly pp1,pp2,tp1,tp2;
      poly sp1=pCopy(add_repr->m[tso.ind1]),sp2=pCopy(add_repr->m[tso.ind2]);
      pp1 = p1;
      pp2 = p2;
      loop
      {
        assume(pp1!=NULL);
        for(i=(int)(currRing->N); i; i--)
          pSetExp(pp1,i, pGetExp(pp1,i)- pGetExp(deg_soc,i));
        pSetComp(pp1, 0);
        pSetm(pp1);
        if ((pNext(pp1)!=NULL) && (pGetComp(pNext(pp1))!=ac))  break;
        pIter(pp1);
      }
      loop
      {
        assume(pp2!=NULL);
        for(i=(int)(currRing->N); i; i--)
          pSetExp(pp2,i, pGetExp(pp2,i)- pGetExp(deg_soc,i));
        pSetComp(pp2, 0);
        pSetm(pp2);
        if ((pNext(pp2)!=NULL) && (pGetComp(pNext(pp2))!=ac)) break;
        pIter(pp2);
      }
      tp1 = pNext(pp1);
      tp2 = pNext(pp2);
      pNext(pp1) = NULL;
      pNext(pp2) = NULL;
      //p_Shift(&p1,-ac,currRing);
      //p_Shift(&p2,-ac,currRing);
      tp1 = pMult(tp1,pCopy(p2));
      tp2 = pMult(tp2,pCopy(p1));
      sp1 = pMult(p2,sp1);
      sp2 = pMult(p1,sp2);
      tso.p = pSub(tp1,tp2);
      tso.syz = pSub(sp1,sp2);
    }
    else
#endif
    {
      tso.p = ksOldCreateSpoly(tso.p2,tso.p1);
      number coefgcd = n_Gcd(pGetCoeff(tso.p1),pGetCoeff(tso.p2),currRing->cf);
      assume (add_repr->m[tso.ind1]!=NULL);
      tso.syz = pCopy(add_repr->m[tso.ind1]);
      poly tt = pMDivide(tso.lcm,tso.p1);
      pSetComp(tt,0);
      pSetmComp(tt);
      pSetCoeff(tt,nDiv(pGetCoeff(tso.p1),coefgcd));
      tso.syz = pMult_mm(tso.syz,tt);
      pDelete(&tt);
      coefgcd = nInpNeg(coefgcd);
      assume (add_repr->m[tso.ind2]!=NULL);
      p = pCopy(add_repr->m[tso.ind2]);
      tt = pMDivide(tso.lcm,tso.p2);
      pSetComp(tt,0);
      pSetmComp(tt);
      pSetCoeff(tt,nDiv(pGetCoeff(tso.p2),coefgcd));
      p = pMult_mm(p,tt);
      pDelete(&tt);
      tso.syz = pAdd(p,tso.syz);
      nDelete(&coefgcd);
    }
#ifdef SHOW_PROT
Print("reduziere Paar im Module %d mit: \n",index);
PrintS("poly1: ");pWrite(tso.p1);
PrintS("poly2: ");pWrite(tso.p2);
PrintS("syz: ");pWrite(tso.syz);
PrintS("sPoly: ");pWrite(tso.p);
#endif
    assume(tso.syz!=NULL);
    kBucketInit(syzstr->syz_bucket,tso.syz,-1);
    if (tso.p!=NULL)
    {
      kBucketInit(syzstr->bucket,tso.p,-1);
      p = kBucketGetLm(syzstr->bucket);
      j = 0;
      loop
      {
        if (j>=*next_place_add) break;
        if (pDivisibleBy(add_generators->m[j],p))
        {
          assume (add_repr->m[j]!=NULL);
          sySPRedSyz_Kosz(syzstr,add_generators->m[j],add_repr->m[j],p);
          n = kBucketPolyRed(syzstr->bucket,add_generators->m[j],
                   pLength(add_generators->m[j]), NULL, FALSE);
          p = kBucketGetLm(syzstr->bucket);
          if ((p==NULL) || (pGetComp(p)<=crit_comp)) break;
          j = 0;
        }
        else
          j++;
      }
      kBucketClear(syzstr->bucket,&tso.p,&tso.length);
    }
    kBucketClear(syzstr->syz_bucket,&tso.syz,&j);
  }
  else
  {
    PrintS("Shit happens!\n");
  }
#ifdef SHOW_PROT
Print("erhalte Paar im Module %d mit: \n",index);
PrintS("syz: ");pWrite(tso.syz);
PrintS("sPoly: ");pWrite(tso.p);
PrintLn();
#endif
  if (tso.p!=NULL)
  {
    if (!nIsOne(pGetCoeff(tso.p)))
    {
      n=nInvers(pGetCoeff(tso.p));
      pNorm(tso.p);
      tso.syz=__p_Mult_nn(tso.syz,n,currRing);
      nDelete(&n);
    }
  }
  if ((TEST_OPT_PROT) && (tso.syz==NULL)) PrintS("null");
  if ((tso.p!=NULL) && (pGetComp(tso.p)>crit_comp))
  {
    if (*next_place_add>=IDELEMS(add_generators))
    {
      pEnlargeSet(&add_generators->m,IDELEMS(add_generators),16);
      pEnlargeSet(&add_repr->m,IDELEMS(add_repr),16);
      *g_l = (int*)omRealloc0Size((ADDRESS)*g_l, IDELEMS(add_generators)*sizeof(int),
                            (IDELEMS(add_generators)+16)*sizeof(int));
      IDELEMS(add_generators) += 16;
      IDELEMS(add_repr) += 16;
    }
    assume(add_repr->m[*next_place_add]==NULL);
    add_generators->m[*next_place_add] = tso.p;
    add_repr->m[*next_place_add] = tso.syz;
    (*g_l)[*next_place_add] = tso.length;
    (*next_place_add)++;
  }
  else
  {
    while ((ng_place>0) && (new_generators->m[ng_place-1]==NULL) &&
          (new_repr->m[ng_place-1]==NULL)) ng_place--;
    if (ng_place>=IDELEMS(new_generators))
    {
      pEnlargeSet(&new_generators->m,IDELEMS(new_generators),16);
      IDELEMS(new_generators) += 16;
      pEnlargeSet(&new_repr->m,IDELEMS(new_repr),16);
      IDELEMS(new_repr) += 16;
    }
    new_generators->m[ng_place] = tso.p;
    new_repr->m[ng_place] = tso.syz;
  }
  tso.p = NULL;
  tso.syz = NULL;
  resPairs[itso] = tso;
  syDeletePair(&resPairs[itso]);
}

/*3
* reduction of all pairs of a fixed degree of a fixed module
*/
static BOOLEAN reducePairsHIndex(SSet resPairs,int l_pairs,syStrategy syzstr,
       int index,ideal add_generators,ideal add_repr,ideal new_generators,
       ideal new_repr,int crit_comp,int * red_deg,int * next_place_add,int **g_l,
       resolvente totake)
{
  if (resPairs[0].lcm==NULL) return FALSE;
  int i=0;
  poly deg_soc;

  if (TEST_OPT_PROT)
    Print("(%d,%d)",index,resPairs[0].order);
  while ((i<l_pairs) && (resPairs[i].order==*red_deg))
  {
    assume(totake[index-1]!=NULL);
    assume(pGetComp(resPairs[i].p1)<=IDELEMS(totake[index-1]));
    assume(totake[index-1]->m[pGetComp(resPairs[i].p1)-1]!=NULL);
    deg_soc = totake[index-1]->m[pGetComp(resPairs[i].p1)-1];
    redOnePairHIndex(resPairs,i,crit_comp,syzstr,index, add_generators,add_repr,
                     new_generators, new_repr,next_place_add,g_l,deg_soc);
    i++;
  }
  syCompactifyPairSet(resPairs,l_pairs,0);
  if (resPairs[0].lcm==NULL)  //there are no pairs left and no new_gens
    return FALSE;
  else
    *red_deg = resPairs[0].order;
  return TRUE;
}

/*3
* we proceed the generators of the next module;
* they are stored in add_generators and add_repr;
* if the normal form of a new genrators w.r.t. add_generators has
* pGetComp<crit_comp it is skipped from the reduction;
* new_generators and new_repr (which are empty) stores the result of the
* reduction which is normalized afterwards
*/
static void procedeNextGenerators(ideal temp_generators,ideal /*temp_repr*/,
      ideal new_generators, ideal new_repr, ideal add_generators,
      ideal add_repr, syStrategy syzstr,int index, int crit_comp,
      resolvente totake)
{
  int i=0,j,next_new_el;
  int idel_temp=IDELEMS(temp_generators);
  int next_place_add;
  int p_length,red_deg,l_pairs=IDELEMS(add_generators);
  poly next_p;
  int * gen_length=(int*)omAlloc0(IDELEMS(add_generators)*sizeof(int));
  int * secgen_length=(int*)omAlloc0(IDELEMS(syzstr->res[index])*sizeof(int));
  BOOLEAN pairs_left;
  SSet resPairs=(SSet)omAlloc0(l_pairs*sizeof(SObject));

  for (j=IDELEMS(syzstr->res[index])-1;j>=0;j--)
  {
    if (syzstr->res[index]->m[j]!=NULL)
      secgen_length[j] = pLength(syzstr->res[index]->m[j]);
  }
  assume(idIs0(new_generators));
  next_place_add = IDELEMS(add_generators);
  while ((next_place_add>0) && (add_generators->m[next_place_add-1]==NULL))
    next_place_add--;
  int next_deg = p_FDeg(temp_generators->m[i],currRing);
  next_new_el = next_place_add;
/*--- loop about all all elements-----------------------------------*/
  while ((i<idel_temp) && (temp_generators->m[i]!=NULL))
  {
/*--- separates elements of equal degree----------------------------*/
#ifdef USE_REGULARITY
    if (syzstr->regularity>0)
    {
      if (next_deg >= syzstr->regularity+index)
      {
        while ((i<idel_temp) && (temp_generators->m[i]!=NULL))
        {
          pDelete(&temp_generators->m[i]);
          i++;
        }
        break;
      }
    }
#endif
    while ((i<idel_temp) && (p_FDeg(temp_generators->m[i],currRing)==next_deg))
    {
      next_p = temp_generators->m[i];
      temp_generators->m[i] = NULL;
      next_p = normalize(next_p,add_generators,syzstr,gen_length,&p_length,
                crit_comp);
      if (next_p!=NULL)
      {
        if (pGetComp(next_p)<=crit_comp)
        {
          pDelete(&next_p);
          //if (TEST_OPT_PROT) Print("u(%d)",index);
        }
        else
        {
          next_p = syRedTailSyz(next_p,add_generators,syzstr->res[index],crit_comp,syzstr,
            gen_length,secgen_length,&p_length);
          if (!nIsOne(pGetCoeff(next_p)))
            pNorm(next_p);
          if (next_place_add>=IDELEMS(add_generators))
          {
            pEnlargeSet(&add_generators->m,IDELEMS(add_generators),16);
            pEnlargeSet(&add_repr->m,IDELEMS(add_repr),16);
            gen_length = (int*)omRealloc0Size((ADDRESS)gen_length, IDELEMS(add_generators)*sizeof(int),
                                        (IDELEMS(add_generators)+16)*sizeof(int));
            IDELEMS(add_generators) += 16;
            IDELEMS(add_repr) += 16;
          }
          add_generators->m[next_place_add] = next_p;
          if (totake[index]==NULL)
            totake[index] = idInit(16,new_generators->rank);
          if ((*syzstr->Tl)[index]==IDELEMS(totake[index]))
          {
            pEnlargeSet(&totake[index]->m,IDELEMS(totake[index]),
                        (*syzstr->Tl)[index]+16-IDELEMS(totake[index]));
            for (j=IDELEMS(totake[index]);j<(*syzstr->Tl)[index]+16;j++)
              totake[index]->m[j] = NULL;
            IDELEMS(totake[index]) = (*syzstr->Tl)[index]+16;
          }
#ifdef FULL_TOTAKE
          totake[index]->m[(*syzstr->Tl)[index]] = pCopy(next_p);
#else
          totake[index]->m[(*syzstr->Tl)[index]] = pHead(next_p);
#endif
          assume(add_repr->m[next_place_add]==NULL);
#ifdef WITH_SCHREYER_ORD
          add_repr->m[next_place_add] = pHead(add_generators->m[next_place_add]);
#else
          add_repr->m[next_place_add] = pOne();
#endif
          ((*syzstr->Tl)[index])++;
          pSetComp(add_repr->m[next_place_add],(*syzstr->Tl)[index]);
          pSetmComp(add_repr->m[next_place_add]);
          gen_length[next_place_add] = p_length;
          next_place_add++;
        }
      }
      i++;
    }                        //end inner loop
    red_deg = next_deg;
    if (i<idel_temp)
      next_deg = p_FDeg(temp_generators->m[i],currRing);
    else
      next_deg = -1;
    if ((next_place_add>next_new_el) || (next_deg<0))  //there are new generators or pairs
    {
/*-reducing and generating pairs untill the degree of the next generators-*/
      pairs_left = TRUE;
      while (pairs_left && ((next_deg<0) || (red_deg<= next_deg)))
      {
        updatePairsHIndex(&resPairs,&l_pairs,syzstr,index,add_generators,
          add_repr,new_generators,new_repr,crit_comp,&next_new_el);
        pairs_left = reducePairsHIndex(resPairs,l_pairs,syzstr,index,add_generators,
           add_repr,new_generators,new_repr,crit_comp,&red_deg,&next_place_add,&gen_length,
           totake);
      }
    }
  }
  omFreeSize((SSet)resPairs,l_pairs*sizeof(SObject));
  omFreeSize((ADDRESS)gen_length,IDELEMS(add_generators)*sizeof(int));
  omFreeSize((ADDRESS)secgen_length,IDELEMS(syzstr->res[index])*sizeof(int));
}

/*3
* normalizes the part of the next reduction lying within the block
* of former generators (old_generators);
*/
static ideal normalizeOldPart(ideal new_generators,ideal new_repr,
                      syStrategy syzstr,int index,int /*crit_comp*/)
{
  ideal old_generators= syzstr->res[index];
  ideal old_repr= syzstr->orderedRes[index];
  int i,j=0,ii=IDELEMS(old_generators)-1,dummy;
  poly p;
  number n;
  int * g_l=(int*)omAlloc0(IDELEMS(old_generators)*sizeof(int));

  for (i=0;i<IDELEMS(old_generators);i++)
  {
    if (old_generators->m[i]!=NULL)
    {
      g_l[i] = pLength(old_generators->m[i]);
    }
  }
  for (i=IDELEMS(new_generators)-1;i>=0;i--)
  {
    if (new_generators->m[i]!=NULL)
    {
      kBucketInit(syzstr->bucket,new_generators->m[i],
                   pLength(new_generators->m[i]));
      kBucketInit(syzstr->syz_bucket,new_repr->m[i],
                   pLength(new_repr->m[i]));
      p = kBucketGetLm(syzstr->bucket);
      loop
      {
        if ((j>=ii) || (p==NULL)) break;
        if ((old_generators->m[j]!=NULL) &&
            (pDivisibleBy(old_generators->m[j],p)))
        {
          sySPRedSyz_Kosz(syzstr,old_generators->m[j],old_repr->m[j],p);
          n = kBucketPolyRed(syzstr->bucket,old_generators->m[j], g_l[j],
                             NULL, FALSE);
          nDelete(&n);
          p = kBucketGetLm(syzstr->bucket);
          j = 0;
        }
        else
          j++;
      }
      assume (p==NULL);
      kBucketClear(syzstr->bucket,&new_generators->m[i],&dummy);
      kBucketClear(syzstr->syz_bucket,&new_repr->m[i],&dummy);
    }
  }
  ideal result=idInit(IDELEMS(new_repr),new_repr->rank);
  for (j=IDELEMS(new_repr)-1;j>=0;j--)
  {
    result->m[j] = new_repr->m[j];
    if ((result->m[j]!=NULL) && (!nIsOne(pGetCoeff(result->m[j]))))
      pNorm(result->m[j]);
    new_repr->m[j] = NULL;
  }
  omFreeSize((ADDRESS)g_l,IDELEMS(old_generators)*sizeof(int));
  return result;
}

/*3
* constructs the new subresolution for a nonregular extension
*/
static ideal kosz_ext(ideal new_generators,ideal new_repr,syStrategy syzstr,
                      int index,int next_comp,resolvente totake)
{
  ideal temp_generators =idInit(IDELEMS(new_generators),new_generators->rank);
  ideal temp_repr=idInit(IDELEMS(new_repr),new_repr->rank);
  ideal add_generators =idInit(IDELEMS(new_generators),new_generators->rank);
  ideal add_repr=idInit(IDELEMS(new_repr),new_repr->rank);
  int min_deg=-1;
  int j,jj,k,deg_p,idel_temp=IDELEMS(temp_generators);
  poly p;
/*--reorder w.r.t. the degree----------------------------------------*/
  for (j=IDELEMS(new_generators)-1;j>=0;j--)
  {
    if (new_generators->m[j]!=NULL)
    {
      p = new_generators->m[j];
      new_generators->m[j] = NULL;
      deg_p = p_FDeg(p,currRing);
      if (min_deg<0)
      {
        min_deg = deg_p;
      }
      else
      {
        if (deg_p<min_deg) min_deg = deg_p;
      }
      k = 0;
      while ((k<idel_temp) && (temp_generators->m[k]!=NULL) &&
             (p_FDeg(temp_generators->m[k],currRing)<=deg_p)) k++;
      for (jj=idel_temp-1;jj>k;jj--)
      {
        temp_generators->m[jj] = temp_generators->m[jj-1];
      }
      temp_generators->m[k] = p;
    }
  }
/*--- computing the standard basis in the resolution of the extension -*/
  procedeNextGenerators(temp_generators,temp_repr,new_generators,new_repr,
    add_generators,add_repr,syzstr,index,next_comp,totake);
  j = IDELEMS(syzstr->res[index]);
  while ((j>0) && (syzstr->res[index]->m[j-1]==NULL)) j--;
  jj = IDELEMS(add_generators);
  while ((jj>0) && (add_generators->m[jj-1]==NULL)) jj--;
  if (j+jj>=IDELEMS(syzstr->res[index]))
  {
    pEnlargeSet(&syzstr->res[index]->m,IDELEMS(syzstr->res[index]),
                j+jj+1-IDELEMS(syzstr->res[index]));
    IDELEMS(syzstr->res[index]) = j+jj+1;
    pEnlargeSet(&syzstr->orderedRes[index]->m,IDELEMS(syzstr->orderedRes[index]),
                j+jj+1-IDELEMS(syzstr->orderedRes[index]));
    IDELEMS(syzstr->orderedRes[index]) = j+jj+1;
  }
  for (k=0;k<jj;k++)
  {
    syzstr->res[index]->m[j+k] = add_generators->m[k];
    syzstr->orderedRes[index]->m[j+k] = add_repr->m[k];
    add_generators->m[k] = NULL;
    add_repr->m[k] = NULL;
  }
  assume(idIs0(add_generators));
  assume(idIs0(add_repr));
  idDelete(&add_generators);
  idDelete(&add_repr);
  idDelete(&temp_generators);
  idDelete(&temp_repr);
/*--- normalizing the rest to get the syzygies ------------------------*/
  return normalizeOldPart(new_generators,new_repr,syzstr,index,next_comp);
}

/*
* this procedure assumes that the first order is C !!!
* INPUT: old_generators - the generators of the actual module
*                         computed so far (they are mixed vectors)
*        old_repr       - the representations of the old generators
*        new_generators - generators coming from reductions below,
*                         they must have leading terms in new components
*                         (they live only in the module part)
*  (*syzstr->Tl)[index] - the last used component in the syzygy
* OUTPUT: old_generators is updated
*         new_generators is empty
*         the return value is a set of new generators for the syzygies,
*/
static ideal syAppendSyz(ideal new_generators, syStrategy syzstr,int index,int crit_comp,
                         resolvente totake)
{
  int i,j;
  ideal result;
  int rk_new_gens = id_RankFreeModule(new_generators,currRing);
  if (syzstr->res[index]==NULL)
  {
    syzstr->res[index] = idInit(1,si_max(rk_new_gens,1));
    syzstr->orderedRes[index] = idInit(1,si_max(rk_new_gens,1));
  }
  int ng_idel=IDELEMS(new_generators);
  ideal new_repr =idInit(ng_idel, crit_comp+ng_idel);

  if (index==0)
  {
    //int * og_l=(int*)omAlloc0(IDELEMS(syzstr->res[0])*sizeof(int));
    //for (i=IDELEMS(syzstr->res[0])-1;i>=0;i--)
    //{
      //if (syzstr->res[0]->m[i]!=NULL)
        //og_l[i] = pLength(syzstr->res[0]->m[i]);
    //}
    for (i=0;i<ng_idel;i++)
    {
      if (new_generators->m[i]!=NULL)
      {
        //int ng_l=pLength(new_generators->m[i]);
        //new_generators->m[i] = syRedTailSyz(new_generators->m[i],syzstr->res[0],NULL,0,syzstr,
            //og_l,NULL,&ng_l);
        if (totake[index]==NULL)
          totake[index] = idInit(16,new_generators->rank);
        if ((*syzstr->Tl)[index]>=IDELEMS(totake[index]))
        {
          pEnlargeSet(&totake[index]->m,IDELEMS(totake[index]),
                      (*syzstr->Tl)[index]+16-IDELEMS(totake[index]));
          for (j=IDELEMS(totake[index]);j<(*syzstr->Tl)[index]+16;j++)
            totake[index]->m[j] = NULL;
          IDELEMS(totake[index]) = (*syzstr->Tl)[index]+16;
        }
#ifdef FULL_TOTAKE
        totake[index]->m[(*syzstr->Tl)[index]] = pCopy(new_generators->m[i]);
#else
        totake[index]->m[(*syzstr->Tl)[index]] = pHead(new_generators->m[i]);
#endif
#ifdef WITH_SCHREYER_ORD
        new_repr->m[i] = pHead(new_generators->m[i]);
#else
        new_repr->m[i] = pOne();
#endif
        ((*syzstr->Tl)[index])++;
        pSetComp(new_repr->m[i],(*syzstr->Tl)[index]);
        pSetmComp(new_repr->m[i]);
      }
    }
    //omFreeSize((ADDRESS)og_l,IDELEMS(syzstr->res[0])*sizeof(int));
#ifdef SHOW_PROT
PrintS("Add new generators:\n");
idPrint(new_generators);
PrintS("with representaions:\n");
idPrint(new_repr);
#endif
    result = kosz_std(new_generators,new_repr,syzstr,index,crit_comp);
  }
  else
  {
    result = kosz_ext(new_generators,new_repr,syzstr,index,crit_comp,totake);
  }
  idSkipZeroes(result);
  assume(idIs0(new_repr));
  idDelete(&new_repr);
  return result;
}

/*
* main call of the extended Koszul-resolution
*/
syStrategy syKosz(ideal arg,int * length)
{
  int i,j,jj,k=0,index=0,rk_arg/*,next_syz=0*/;
  int crit_comp,t_comp,next_deg,old_tl;
  ideal temp=NULL,old_ideal,old_repr;
  ring origR = currRing;
  poly next_gen;
  BOOLEAN isRegular;

  discard_pairs = 0;
  short_pairs = 0;
  if (idIs0(arg)) return NULL;
  rk_arg = id_RankFreeModule(arg,currRing);
  syStrategy syzstr=(syStrategy)omAlloc0(sizeof(ssyStrategy));
/*--- changes to a Cdp-ring ----------------------------*/
  syzstr->syRing = rAssure_C_dp(origR); rChangeCurrRing(syzstr->syRing);
/*--- initializes the data structures---------------*/
  syzstr->length = *length = (syzstr->syRing->N)+2;
  syzstr->regularity = -1;
  if (origR!=syzstr->syRing)
    temp = idrCopyR(arg, origR, syzstr->syRing);
  else
    temp = idCopy(arg);
  if (rk_arg==0)
  {
    id_Shift(temp,1,currRing);
  }
  idSkipZeroes(temp);
#ifdef WITH_SORT
  if (temp->m[0]!=NULL)
  {
    int md;
    int maxdeg=p_FDeg(temp->m[IDELEMS(temp)-1],currRing);
    ideal temp1=idInit(IDELEMS(temp),temp->rank);
    for (j=IDELEMS(temp)-2;j>=0;j--)
    {
      jj = p_FDeg(temp->m[j],currRing);
      if (jj>maxdeg) maxdeg = jj;
    }
    while (!idIs0(temp))
    {
      md = maxdeg;
      for (j=IDELEMS(temp)-1;j>=0;j--)
      {
        if (temp->m[j]!=NULL)
        {
          jj = p_FDeg(temp->m[j],currRing);
          if (jj<md) md = jj;
        }
      }
      for (j=0;j<IDELEMS(temp);j++)
      {
        if ((temp->m[j]!=NULL) && (p_FDeg(temp->m[j],currRing)==md))
        {
          temp1->m[k] = temp->m[j];
          temp->m[j] = NULL;
          k++;
        }
      }
    }
    idDelete(&temp);
    temp = temp1;
    temp1 = NULL;
  }
#endif
#ifdef USE_REGULARITY
  int last_generator=IDELEMS(temp)-1;
  while ((last_generator>=0) && (temp->m[last_generator]==NULL))
    last_generator--;
#endif
  syzstr->res = (resolvente)omAlloc0((*length+1)*sizeof(ideal));
  syzstr->orderedRes = (resolvente)omAlloc0((*length+1)*sizeof(ideal));
  resolvente totake=(resolvente)omAlloc0((*length+1)*sizeof(ideal));
  syzstr->Tl = new intvec(*length+1);
  syzstr->bucket = kBucketCreate(currRing);
  syzstr->syz_bucket = kBucketCreate(currRing);
  ideal new_generators=idInit(1,si_max(rk_arg,1));
  ideal temp_gens,old_std;
  syzstr->res[0] = idInit(1,1);
  if (rk_arg>1) syzstr->res[0]->rank = rk_arg;
  syzstr->orderedRes[0] = idInit(1,1);
/*--- computes the resolution ----------------------*/
  i = 0;
  while (i<IDELEMS(temp))
  {
    if (temp->m[i]!=NULL)
    {
      new_generators->m[0] = kNF(syzstr->res[0],currRing->qideal,temp->m[i]);
      if (!nIsOne(pGetCoeff(new_generators->m[0])))
        pNorm(new_generators->m[0]);
      next_deg = p_FDeg(new_generators->m[0],currRing);
      next_gen = pCopy(new_generators->m[0]);
    }
    if (!idIs0(new_generators))
    {
      index = 0;
      while (index<=*length)
      {
        if (index==0)
        {
          old_ideal = idCopy(syzstr->res[0]);
          old_repr = idCopy(syzstr->orderedRes[0]);
          old_tl = (*syzstr->Tl)[0];
          old_std = id_Head(syzstr->res[0],currRing);
        }
        t_comp = (*syzstr->Tl)[index];
        if (index==0) crit_comp = t_comp;
        temp_gens = syAppendSyz(new_generators,syzstr, index,crit_comp,totake);
        crit_comp = t_comp;
        if (index==0)
        {
          isRegular = syIsRegular(old_std,syzstr->res[0],next_deg);
#ifndef ONLY_STD
          if (isRegular)
            syCreateRegularExtension(syzstr,old_ideal,old_repr,old_tl,next_gen,
                                     totake);
#ifdef USE_REGULARITY
        if ((index==0) && (!isRegular) && (i==last_generator))
        {
/*----------- we are computing the regularity -----------------------*/
          ideal initial=id_Head(syzstr->res[0],currRing);
          int len=0,reg=0;
          intvec *w=NULL;
          ring dp_C_ring = rAssure_dp_C(currRing); rChangeCurrRing(dp_C_ring);
          initial = idrMoveR_NoSort(initial, syzstr->syRing, dp_C_ring);
          resolvente res = sySchreyerResolvente(initial,-1,&len,TRUE, TRUE);
          intvec * dummy = syBetti(res,len,&reg, w);
          syzstr->regularity = reg+2;
          delete dummy;
          delete w;
          for (j=0;j<len;j++)
          {
            if (res[j]!=NULL) idDelete(&(res[j]));
          }
          omFreeSize((ADDRESS)res,len*sizeof(ideal));
          idDelete(&initial);
          rChangeCurrRing(syzstr->syRing);
          rDelete(dp_C_ring);
        }
#endif
#endif
          idDelete(&old_ideal);
          idDelete(&old_repr);
          idDelete(&old_std);
          if (TEST_OPT_PROT)
          {
            if (isRegular)
              PrintS("\n regular\n");
            else
              PrintS("\n not regular\n");
          }
          if (next_gen!=NULL)
            pDelete(&next_gen);
          if (isRegular)
          {
            idDelete(&temp_gens);
            break;
          }
        }
        idDelete(&new_generators);
        new_generators = temp_gens;
#ifdef ONLY_STD
        break;
#endif
        if (idIs0(new_generators)) break;
        index++;
      }
      if (!idIs0(new_generators))
      {
        for (j=0;j<IDELEMS(new_generators);j++)
        {
          if (new_generators->m[j]!=NULL)
          {
            pDelete(&new_generators->m[j]);
            new_generators->m[j] = NULL;
          }
        }
      }
    }
    i++;
  }
  if (idIs0(new_generators) && new_generators!=NULL) idDelete(&new_generators);
  if (temp!=NULL) idDelete(&temp);
  kBucketDestroy(&(syzstr->bucket));
  kBucketDestroy(&(syzstr->syz_bucket));
  index = 0;
  syzstr->fullres = syzstr->res;
  syzstr->res = NULL;
  index = 0;
  while ((index<=*length) && (syzstr->fullres[index]!=NULL))
  {
#ifdef SHOW_RESULT
    Print("The %d-th syzygy-module is now:\n",index);
    ideal ttt=id_Head(syzstr->fullres[index],currRing);
    idShow(ttt);
    idDelete(&ttt);
    //if (index>0)
    //{
      //Print("The related module is: \n");
      //idPrint(totake[index-1]);
    //}
    //Print("The %d-th module of the minimal resolution is:\n",index);
    if (!idIs0(totake[index]))
      idShow(totake[index]);
    //Print("with standard basis:\n");
    //idPrint(syzstr->fullres[index]);
    //if ((index<*length) && (totake[index+1]!=NULL))
    //{
      //Print("The %d-th syzygy-module is now:\n",index+1);
      //idPrint(totake[index+1]);
      //matrix m1=idModule2Matrix(totake[index]);
      //matrix m2=idModule2Matrix(totake[index+1]);
      //matrix m3=mpMult(m1,m2);
      //idPrint((ideal)m3);
    //}
#endif
    if (!idIs0(totake[index]))
    {
      for(i=0;i<IDELEMS(totake[index]);i++)
      {
        if (totake[index]->m[i]!=NULL)
        {
          j=0;
          while ((j<IDELEMS(syzstr->fullres[index])) &&
            ((syzstr->fullres[index]->m[j]==NULL) ||
            (!pLmEqual(syzstr->fullres[index]->m[j],totake[index]->m[i])))) j++;
          if (j<IDELEMS(syzstr->fullres[index]))
          {
            pDelete(&totake[index]->m[i]);
            totake[index]->m[i] = syzstr->fullres[index]->m[j];
            syzstr->fullres[index]->m[j] = NULL;
          }
          else
          {
            PrintS("Da ist was faul!!!\n");
            Print("Aber: Regularitaet %d, Grad %ld\n",
                   syzstr->regularity,p_FDeg(totake[index]->m[i],currRing));
          }
        }
      }
      idDelete(&syzstr->fullres[index]);
      syzstr->fullres[index] = totake[index];
    }
#ifdef SHOW_RESULT
    idShow(syzstr->fullres[index]);
#endif
    index++;
  }
  syReorder_Kosz(syzstr);
  index = 0;
  while ((index<=*length) && (syzstr->orderedRes[index]!=NULL))
  {
    idDelete(&(syzstr->orderedRes[index]));
    index++;
  }
  if (origR!=syzstr->syRing)
  {
    rChangeCurrRing(origR);
    index = 0;
    while ((index<=*length) && (syzstr->fullres[index]!=NULL))
    {
      syzstr->fullres[index] = idrMoveR(syzstr->fullres[index],syzstr->syRing, origR);
      index++;
    }
  }
  delete syzstr->Tl;
  syzstr->Tl = NULL;
  rDelete(syzstr->syRing);
  syzstr->syRing = NULL;
  omFreeSize((ADDRESS)totake,(*length+1)*sizeof(ideal));
  omFreeSize((ADDRESS)syzstr->orderedRes,(*length+1)*sizeof(ideal));
//Print("Pairs to discard: %d\n",discard_pairs);
//Print("Pairs shorter reduced: %d\n",short_pairs);
//discard_pairs = 0;
//short_pairs = 0;
  return syzstr;
}

