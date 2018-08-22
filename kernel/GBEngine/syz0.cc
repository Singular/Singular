/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: resolutions
*/

#include "kernel/mod2.h"
#include "misc/options.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/combinatorics/stairc.h"
#include "misc/intvec.h"
#include "coeffs/numbers.h"
#include "kernel/ideals.h"
#include "misc/intvec.h"
#include "polys/monomials/ring.h"
#include "kernel/GBEngine/syz.h"
#include "polys/kbuckets.h"
#include "polys/prCopy.h"

static void syInitSort(ideal arg,intvec **modcomp)
{
  int i,j,k,kk,kkk,jj;
  idSkipZeroes(arg);
  polyset F,oldF=arg->m;
  int Fl=IDELEMS(arg);
  int rkF=id_RankFreeModule(arg,currRing);
  int syComponentOrder=currRing->ComponentOrder;

  while ((Fl!=0) && (oldF[Fl-1]==NULL)) Fl--;
  if (*modcomp!=NULL) delete modcomp;
  *modcomp = new intvec(rkF+2);
  F=(polyset)omAlloc0(IDELEMS(arg)*sizeof(poly));
  j=0;
  for(i=0;i<=rkF;i++)
  {
    k=0;
    jj = j;
    (**modcomp)[i] = j;
    while (k<Fl)
    {
      while ((k<Fl) && (pGetComp(oldF[k]) != i)) k++;
      if (k<Fl)
      {
        kk=jj;
        while ((kk<Fl) && (F[kk]) && (pLmCmp(oldF[k],F[kk])!=syComponentOrder))
        {
            kk++;
        }
        for (kkk=j;kkk>kk;kkk--)
        {
          F[kkk] = F[kkk-1];
        }
        F[kk] = oldF[k];
//Print("Element %d: ",kk);pWrite(F[kk]);
        j++;
        k++;
      }
    }
  }
  (**modcomp)[rkF+1] = Fl;
  arg->m = F;
  omFreeSize((ADDRESS)oldF,IDELEMS(arg)*sizeof(poly));
}

static void syCreatePairs(polyset F,int lini,int wend,int k,int j,int i,
           polyset pairs,int regularPairs=0,ideal mW=NULL)
{
  int l,ii=0,jj;
  poly p,q;

  while (((k<wend) && (pGetComp(F[k]) == i)) ||
         ((currRing->qideal!=NULL) && (k<regularPairs+IDELEMS(currRing->qideal))))
  {
    p = pOne();
    if ((k<wend) && (pGetComp(F[k]) == i) && (k!=j))
      pLcm(F[j],F[k],p);
    else if (ii<IDELEMS(currRing->qideal))
    {
      q = pHead(F[j]);
      if (mW!=NULL)
      {
        for(jj=1;jj<=(currRing->N);jj++)
          pSetExp(q,jj,pGetExp(q,jj) -pGetExp(mW->m[pGetComp(q)-1],jj));
        pSetm(q);
      }
      pLcm(q,currRing->qideal->m[ii],p);
      if (mW!=NULL)
      {
        for(jj=1;jj<=(currRing->N);jj++)
          pSetExp(p,jj,pGetExp(p,jj) +pGetExp(mW->m[pGetComp(p)-1],jj));
        pSetm(p);
      }
      pDelete(&q);
      k = regularPairs+ii;
      ii++;
    }
    l=lini;
    while ((l<k) && ((pairs[l]==NULL) || (!pDivisibleBy(pairs[l],p))))
    {
      if ((pairs[l]!=NULL) && (pDivisibleBy(p,pairs[l])))
        pDelete(&(pairs[l]));
      l++;
    }
    if (l==k)
    {
      pSetm(p);
      pairs[l] = p;
    }
    else
      pDelete(&p);
    k++;
  }
}

static poly syRedtail2(poly p, polyset redWith, intvec *modcomp)
{
  poly h, hn;
  int hncomp,nxt;
  int j;

  h = p;
  hn = pNext(h);
  while(hn != NULL)
  {
    hncomp = pGetComp(hn);
    j = (*modcomp)[hncomp];
    nxt = (*modcomp)[hncomp+1];
    while (j < nxt)
    {
      if (pLmDivisibleByNoComp(redWith[j], hn))
      {
        //if (TEST_OPT_PROT) PrintS("r");
        hn = ksOldSpolyRed(redWith[j],hn);
        if (hn == NULL)
        {
          pNext(h) = NULL;
          return p;
        }
        hncomp = pGetComp(hn);
        j = (*modcomp)[hncomp];
        nxt = (*modcomp)[hncomp+1];
      }
      else
      {
        j++;
      }
    }
    h = pNext(h) = hn;
    hn = pNext(h);
  }
  return p;
}

/*2
* computes the Schreyer syzygies in the local case
* input: arg   (only allocated: Shdl, Smax)
* output: Shdl, Smax
*/
static ideal sySchreyersSyzygiesFM(ideal arg,intvec ** modcomp)
{
  int Fl=IDELEMS(arg);
  while ((Fl!=0) && (arg->m[Fl-1]==NULL)) Fl--;
  ideal result=idInit(16,arg->rank+Fl);
  polyset F=arg->m,*Shdl=&(result->m);
  if (Fl==0) return result;

  int i,j,l,k,totalToRed,ecartToRed,kk;
  int bestEcart,totalmax,rkF,Sl=0,smax,tmax,tl;
  int *ecartS, *ecartT, *totalS,
    *totalT=NULL, *temp=NULL;
  polyset pairs,S,T,ST/*,oldF*/;
  poly p,q,toRed;
  BOOLEAN notFound = FALSE;
  intvec * newmodcomp = new intvec(Fl+2);
  intvec * tempcomp;

//Print("Naechster Modul\n");
//idPrint(arg);
/*-------------initializing the sets--------------------*/
  ST=(polyset)omAlloc0(Fl*sizeof(poly));
  S=(polyset)omAlloc0(Fl*sizeof(poly));
  ecartS=(int*)omAlloc(Fl*sizeof(int));
  totalS=(int*)omAlloc(Fl*sizeof(int));
  T=(polyset)omAlloc0(2*Fl*sizeof(poly));
  ecartT=(int*)omAlloc(2*Fl*sizeof(int));
  totalT=(int*)omAlloc(2*Fl*sizeof(int));
  pairs=(polyset)omAlloc0(Fl*sizeof(poly));

  smax = Fl;
  tmax = 2*Fl;
  for (j=1;j<IDELEMS(arg);j++)
  {
    if (arg->m[j] != NULL)
    {
      assume (arg->m[j-1] != NULL);
      assume (pGetComp(arg->m[j-1])-pGetComp(arg->m[j])<=0);
    }
  }
  rkF=id_RankFreeModule(arg,currRing);
/*----------------construction of the new ordering----------*/
  if (rkF>0)
    rSetSyzComp(rkF, currRing);
  else
    rSetSyzComp(1, currRing);
/*----------------creating S--------------------------------*/
  for(j=0;j<Fl;j++)
  {
    S[j] = pCopy(F[j]);
    totalS[j] = p_LDeg(S[j],&k,currRing);
    ecartS[j] = totalS[j]-p_FDeg(S[j],currRing);
//Print("%d", pGetComp(S[j]));PrintS("  ");
    p = S[j];
    if (rkF==0) pSetCompP(p,1);
    while (pNext(p)!=NULL) pIter(p);
    pNext(p) = pHead(F[j]);
    pIter(p);
    if (rkF==0)
      pSetComp(p,j+2);
    else
      pSetComp(p,rkF+j+1);
    pSetmComp(p);
  }
//PrintLn();
  if (rkF==0) rkF = 1;
/*---------------creating the initial for T----------------*/
  j=0;
  l=-1;
  totalmax=-1;
  for (k=0;k<smax;k++)
    if (totalS[k]>totalmax) totalmax=totalS[k];
  for (kk=1;kk<=rkF;kk++)
  {
    for (k=0;k<=totalmax;k++)
    {
      for (l=0;l<smax;l++)
      {
        if ((pGetComp(S[l])==kk) && (totalS[l]==k))
        {
          ST[j] = S[l];
          totalT[j] = totalS[l];
          ecartT[j] = ecartS[l];
//Print("%d", totalS[l]);PrintS("  ");
          j++;
        }
      }
    }
  }
//PrintLn();
  for (j=0;j<smax;j++)
  {
     totalS[j] = totalT[j];
     ecartS[j] = ecartT[j];
  }

/*---------------computing---------------------------------*/
  for(j=0;j<smax;j++)
  {
    (*newmodcomp)[j+1] = Sl;
    i = pGetComp(S[j]);
    int syComponentOrder= currRing->ComponentOrder;
    int lini,wend;
    if (syComponentOrder==1)
    {
      lini=k=j+1;
      wend=Fl;
    }
    else
    {
      lini=k=0;
      while ((k<j) && (pGetComp(S[k]) != i)) k++;
      wend=j;
    }
    if (TEST_OPT_PROT)
    {
      Print("(%d)",Fl-j);
      mflush();
    }
    syCreatePairs(S,lini,wend,k,j,i,pairs);
    for (k=lini;k<wend;k++)
    {
      if (pairs[k]!=NULL)
      {
/*--------------creating T----------------------------------*/
        for (l=0;l<smax;l++)
        {
          ecartT[l] = ecartS[l];
          totalT[l] = totalS[l];
          T[l] = ST[l];
        }
        tl = smax;
        tempcomp = ivCopy(*modcomp);
/*--------------begin to reduce-----------------------------*/
        toRed = ksOldCreateSpoly(S[j],S[k]);
        ecartToRed = 1;
        bestEcart = 1;
        if (TEST_OPT_DEBUG)
        {
          PrintS("pair: ");pWrite0(S[j]);PrintS(" ");pWrite(S[k]);
        }
        if (TEST_OPT_PROT)
        {
           PrintS(".");
           mflush();
        }
//Print("Reduziere Paar %d,%d (ecart %d): \n",j,k,ecartToRed);
//Print("Poly %d: ",j);pWrite(S[j]);
//Print("Poly %d: ",k);pWrite(S[k]);
//Print("Spoly: ");pWrite(toRed);
        while (pGetComp(toRed)<=rkF)
        {
          if (TEST_OPT_DEBUG)
          {
            PrintS("toRed: ");pWrite(toRed);
          }
/*
*         if ((bestEcart) || (ecartToRed!=0))
*         {
*/
            totalToRed = p_LDeg(toRed,&kk,currRing);
            ecartToRed = totalToRed-p_FDeg(toRed,currRing);
/*
*         }
*/
//Print("toRed now (neuer ecart %d): ",ecartToRed);pWrite(toRed);
          notFound = TRUE;
          bestEcart = 32000;  //a very large integer
          p = NULL;
          int l=0;
#define OLD_SEARCH
#ifdef OLD_SEARCH
          while ((l<tl) && (pGetComp(T[l])<pGetComp(toRed))) l++;
          //assume (l==(**modcomp)[pGetComp(toRed)]);
          while ((l<tl) && (notFound))
#else
          l = (**modcomp)[pGetComp(toRed)];
          int kkk = (**modcomp)[pGetComp(toRed)+1];
          while ((l<kkk) && (notFound))
#endif
          {
            if ((ecartT[l]<bestEcart) && (pDivisibleBy(T[l],toRed)))
            {
              if (ecartT[l]<=ecartToRed) notFound = FALSE;
              p = T[l];
              bestEcart = ecartT[l];
            }
            l++;
          }
          if (p==NULL)
          {
            pDelete(&toRed);
            for(k=j;k<Fl;k++) pDelete(&(pairs[k]));
            omFreeSize((ADDRESS)pairs,Fl*sizeof(poly));
            omFreeSize((ADDRESS)ST,Fl*sizeof(poly));
            omFreeSize((ADDRESS)S,Fl*sizeof(poly));
            omFreeSize((ADDRESS)T,tmax*sizeof(poly));
            omFreeSize((ADDRESS)ecartT,tmax*sizeof(int));
            omFreeSize((ADDRESS)totalT,tmax*sizeof(int));
            omFreeSize((ADDRESS)ecartS,Fl*sizeof(int));
            omFreeSize((ADDRESS)totalS,Fl*sizeof(int));
            for(k=0;k<IDELEMS(result);k++) pDelete(&((*Shdl)[k]));
            WerrorS("ideal not a standard basis");//no polynom for reduction
            return result;
          }
          else
          {
//Print("reduced with (ecart %d): ",bestEcart);wrp(p);PrintLn();
            if (notFound)
            {
              if (tl>=tmax)
              {
                pEnlargeSet(&T,tmax,16);
                tmax += 16;
                temp = (int*)omAlloc((tmax+16)*sizeof(int));
                for(l=0;l<tmax;l++) temp[l]=totalT[l];
                totalT = temp;
                temp = (int*)omAlloc((tmax+16)*sizeof(int));
                for(l=0;l<tmax;l++) temp[l]=ecartT[l];
                ecartT = temp;
              }
//PrintS("t");
              int comptR=pGetComp(toRed);
              for (l=tempcomp->length()-1;l>comptR;l--)
              {
                if ((*tempcomp)[l]>0)
                  (*tempcomp)[l]++;
              }
              l=0;
              while ((l<tl) && (comptR>pGetComp(T[l]))) l++;
              while ((l<tl) && (totalT[l]<=totalToRed)) l++;
              for (kk=tl;kk>l;kk--)
              {
                T[kk]=T[kk-1];
                totalT[kk]=totalT[kk-1];
                ecartT[kk]=ecartT[kk-1];
              }
              q = pCopy(toRed);
              pNorm(q);
              T[l] = q;
              totalT[l] = totalToRed;
              ecartT[l] = ecartToRed;
              tl++;
            }
            toRed = ksOldSpolyRed(p,toRed);
          }
        }
//Print("toRed finally (neuer ecart %d): ",ecartToRed);pWrite(toRed);
//PrintS("s");
        if (pGetComp(toRed)>rkF)
        {
          if (Sl>=IDELEMS(result))
          {
            pEnlargeSet(Shdl,IDELEMS(result),16);
            IDELEMS(result) += 16;
          }
          //p_Shift(&toRed,-rkF,currRing);
          pNorm(toRed);
          (*Shdl)[Sl] = toRed;
          Sl++;
        }
/*----------------deleting all polys not from ST--------------*/
        for(l=0;l<tl;l++)
        {
          kk=0;
          while ((kk<smax) && (T[l] != S[kk])) kk++;
          if (kk>=smax)
          {
            pDelete(&T[l]);
//Print ("#");
          }
        }
        delete tempcomp;
      }
    }
    for(k=lini;k<wend;k++) pDelete(&(pairs[k]));
  }
  (*newmodcomp)[Fl+1] = Sl;
  omFreeSize((ADDRESS)pairs,Fl*sizeof(poly));
  omFreeSize((ADDRESS)ST,Fl*sizeof(poly));
  omFreeSize((ADDRESS)S,Fl*sizeof(poly));
  omFreeSize((ADDRESS)T,tmax*sizeof(poly));
  omFreeSize((ADDRESS)ecartT,tmax*sizeof(int));
  omFreeSize((ADDRESS)totalT,tmax*sizeof(int));
  omFreeSize((ADDRESS)ecartS,Fl*sizeof(int));
  omFreeSize((ADDRESS)totalS,Fl*sizeof(int));
  delete *modcomp;
  *modcomp = newmodcomp;
  return result;
}

/*3
*special Normalform for Schreyer in factor rings
*/
poly sySpecNormalize(poly toNorm,ideal mW=NULL)
{
  int j,i=0;
  poly p;

  if (toNorm==NULL) return NULL;
  p = pHead(toNorm);
  if (mW!=NULL)
  {
    for(j=1;j<=(currRing->N);j++)
      pSetExp(p,j,pGetExp(p,j) -pGetExp(mW->m[pGetComp(p)-1],j));
  }
  while ((p!=NULL) && (i<IDELEMS(currRing->qideal)))
  {
    if (pDivisibleBy(currRing->qideal->m[i],p))
    {
      //pNorm(toNorm);
      toNorm = ksOldSpolyRed(currRing->qideal->m[i],toNorm);
      pDelete(&p);
      if (toNorm==NULL) return NULL;
      p = pHead(toNorm);
      if (mW!=NULL)
      {
        for(j=1;j<=(currRing->N);j++)
          pSetExp(p,j,pGetExp(p,j) -pGetExp(mW->m[pGetComp(p)-1],j));
      }
      i = 0;
    }
    else
    {
      i++;
    }
  }
  pDelete(&p);
  return toNorm;
}

/*2
* computes the Schreyer syzygies in the global case
* input: F
* output: Shdl, Smax
* modcomp, length stores the start position of the module comp. in arg
*/
static ideal sySchreyersSyzygiesFB(ideal arg,intvec ** modcomp,ideal mW,BOOLEAN redTail=TRUE)
{
  kBucket_pt sy0buck = kBucketCreate(currRing);

  int Fl=IDELEMS(arg);
  while ((Fl!=0) && (arg->m[Fl-1]==NULL)) Fl--;
  ideal result=idInit(16,Fl);
  int i,j,l,k,kkk,/*rkF,*/Sl=0,syComponentOrder=currRing->ComponentOrder;
  int /*fstart,*/wend,lini,ltR,gencQ=0;
  intvec *newmodcomp;
  int *Flength;
  polyset pairs,F=arg->m,*Shdl=&(result->m);
  poly /*p,*/q,toRed,syz,lastmonom,multWith;
  BOOLEAN isNotReduced=TRUE;

//#define WRITE_BUCKETS
#ifdef WRITE_BUCKETS
  PrintS("Input: \n");
  ideal twr=idHead(arg);
  idPrint(arg);
  idDelete(&twr);
  if (modcomp!=NULL) (*modcomp)->show(0,0);
#endif

  newmodcomp = new intvec(Fl+2);
  //for (j=0;j<Fl;j++) pWrite(F[j]);
  //PrintLn();
  if (currRing->qideal==NULL)
    pairs=(polyset)omAlloc0(Fl*sizeof(poly));
  else
  {
    gencQ = IDELEMS(currRing->qideal);
    pairs=(polyset)omAlloc0((Fl+gencQ)*sizeof(poly));
  }
  // rkF=id_RankFreeModule(arg,currRing);
  Flength = (int*)omAlloc0(Fl*sizeof(int));
  for(j=0;j<Fl;j++)
  {
    Flength[j] = pLength(F[j]);
  }
  for(j=0;j<Fl;j++)
  {
    (*newmodcomp)[j+1] = Sl;
    if (TEST_OPT_PROT)
    {
      Print("(%d)",Fl-j);
      mflush();
    }
    i = pGetComp(F[j]);
    if (syComponentOrder==1)
    {
      lini=k=j+1;
      wend=Fl;
    }
    else
    {
      lini=k=0;
      while ((k<j) && (pGetComp(F[k]) != i)) k++;
      wend=j;
    }
    syCreatePairs(F,lini,wend,k,j,i,pairs,Fl,mW);
    if (currRing->qideal!=NULL) wend = Fl+gencQ;
    for (k=lini;k<wend;k++)
    {
      if (pairs[k]!=NULL)
      {
        if (TEST_OPT_PROT)
        {
           PrintS(".");
           mflush();
        }
        //begins to construct the syzygy
        if (k<Fl)
        {
          number an=nCopy(pGetCoeff(F[k])),bn=nCopy(pGetCoeff(F[j]));
          /*int ct =*/ (void) ksCheckCoeff(&an, &bn, currRing->cf);
          syz = pCopy(pairs[k]);
          //syz->coef = nCopy(F[k]->coef);
          syz->coef = an;
          //syz->coef = nInpNeg(syz->coef);
          pNext(syz) = pairs[k];
          lastmonom = pNext(syz);
          //lastmonom->coef = nCopy(F[j]->coef);
          lastmonom->coef = bn;
          lastmonom->coef = nInpNeg(lastmonom->coef);
          pSetComp(lastmonom,k+1);
        }
        else
        {
          syz = pairs[k];
          syz->coef = nCopy(currRing->qideal->m[k-Fl]->coef);
          syz->coef = nInpNeg(syz->coef);
          lastmonom = syz;
          multWith = pMDivide(syz,F[j]);
          multWith->coef = nCopy(currRing->qideal->m[k-Fl]->coef);
        }
        pSetComp(syz,j+1);
        pairs[k] = NULL;
        //the next term of the syzygy
        //constructs the spoly
        if (TEST_OPT_DEBUG)
        {
          if (k<Fl)
          {
            PrintS("pair: ");pWrite0(F[j]);PrintS("  ");pWrite(F[k]);
          }
          else
          {
            PrintS("pair: ");pWrite0(F[j]);PrintS("  ");pWrite(currRing->qideal->m[k-Fl]);
          }
        }
        if (k<Fl)
          toRed = ksOldCreateSpoly(F[j],F[k]);
        else
        {
          q = pMult_mm(pCopy(F[j]),multWith);
          toRed = sySpecNormalize(q,mW);
          pDelete(&multWith);
        }
        kBucketInit(sy0buck,toRed,-1);
        toRed = kBucketGetLm(sy0buck);
        isNotReduced = TRUE;
        while (toRed!=NULL)
        {
          if (TEST_OPT_DEBUG)
          {
            PrintS("toRed: ");pWrite(toRed);
          }
//          l=0;
//          while ((l<Fl) && (!pDivisibleBy(F[l],toRed))) l++;
//          if (l>=Fl)
          l = (**modcomp)[pGetComp(toRed)+1]-1;
          kkk = (**modcomp)[pGetComp(toRed)];
          while ((l>=kkk) && (!pDivisibleBy(F[l],toRed))) l--;
#ifdef WRITE_BUCKETS
          kBucketClear(sy0buck,&toRed,&ltR);
          printf("toRed in Pair[%d, %d]:", j, k);
          pWrite(toRed);
          kBucketInit(sy0buck,toRed,-1);
#endif

          if (l<kkk)
          {
            if ((currRing->qideal!=NULL) && (isNotReduced))
            {
              kBucketClear(sy0buck,&toRed,&ltR);
              toRed = sySpecNormalize(toRed,mW);
#ifdef WRITE_BUCKETS
              printf("toRed in Pair[%d, %d]:", j, k);
              pWrite(toRed);
#endif
              kBucketInit(sy0buck,toRed,-1);
              toRed = kBucketGetLm(sy0buck);
              isNotReduced = FALSE;
            }
            else
            {
              pDelete(&toRed);

              pDelete(&syz);
              for(k=j;k<Fl;k++) pDelete(&(pairs[k]));
              omFreeSize((ADDRESS)pairs,(Fl + gencQ)*sizeof(poly));


              for(k=0;k<IDELEMS(result);k++) pDelete(&((*Shdl)[k]));

              kBucketDestroy(&(sy0buck));

              //no polynom for reduction
              WerrorS("ideal not a standard basis");

              return result;
            }
          }
          else
          {
            //the next monom of the syzygy
            isNotReduced = TRUE;
            if (TEST_OPT_DEBUG)
            {
              PrintS("reduced with: ");pWrite(F[l]);
            }
            pNext(lastmonom) = pHead(toRed);
            pIter(lastmonom);
            lastmonom->coef = nDiv(lastmonom->coef,F[l]->coef);
            //lastmonom->coef = nInpNeg(lastmonom->coef);
            pSetComp(lastmonom,l+1);
            //computes the new toRed
            number up = kBucketPolyRed(sy0buck,F[l],Flength[l],NULL);
            if (! nIsOne(up))
            {
              // Thomas: Now do whatever you need to do
#ifdef WRITE_BUCKETS
              PrintS("multiplied with: ");nWrite(up);PrintLn();
#endif
              syz=__p_Mult_nn(syz,up,currRing);
            }
            nDelete(&up);

            toRed = kBucketGetLm(sy0buck);
            //the module component of the new monom
//pWrite(toRed);
          }
        }
        kBucketClear(sy0buck,&toRed,&ltR); //Zur Sichereheit
//PrintLn();
        if (syz!=NULL)
        {
          if (Sl>=IDELEMS(result))
          {
            pEnlargeSet(Shdl,IDELEMS(result),16);
            IDELEMS(result) += 16;
          }
          pNorm(syz);
          if (BTEST1(OPT_REDTAIL) && redTail)
          {
            (*newmodcomp)[j+2] = Sl;
            (*Shdl)[Sl] = syRedtail2(syz,*Shdl,newmodcomp);
            (*newmodcomp)[j+2] = 0;
          }
          else
            (*Shdl)[Sl] = syz;
          Sl++;
        }
      }
    }
//    for(k=j;k<Fl;k++) pDelete(&(pairs[k]));
  }
  (*newmodcomp)[Fl+1] = Sl;
  if (currRing->qideal==NULL)
    omFreeSize((ADDRESS)pairs,Fl*sizeof(poly));
  else
    omFreeSize((ADDRESS)pairs,(Fl+IDELEMS(currRing->qideal))*sizeof(poly));
  omFreeSize((ADDRESS)Flength,Fl*sizeof(int));
  delete *modcomp;
  *modcomp = newmodcomp;

  kBucketDestroy(&(sy0buck));
  return result;
}

void syReOrderResolventFB(resolvente res,int length, int initial)
{
  int syzIndex=length-1,i,j;
  poly p;

  while ((syzIndex!=0) && (res[syzIndex]==NULL)) syzIndex--;
  while (syzIndex>=initial)
  {
    for(i=0;i<IDELEMS(res[syzIndex]);i++)
    {
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

#if 0
static void syMergeSortResolventFB(resolvente res,int length, int initial=1)
{
  int syzIndex=length-1,i,j;
  poly qq,pp,result=NULL;
  poly p;

  while ((syzIndex!=0) && (res[syzIndex]==NULL)) syzIndex--;
  while (syzIndex>=initial)
  {
    for(i=0;i<IDELEMS(res[syzIndex]);i++)
    {
      p = res[syzIndex]->m[i];
      if (p != NULL)
      {
        for (;;)
        {
          qq = p;
          for(j=1;j<=(currRing->N);j++)
          {
            pSetExp(p,j,pGetExp(p,j)
                        -pGetExp(res[syzIndex-1]->m[pGetComp(p)-1],j));
          }
          pSetm(p);
          for (;;)
          {
            if (pNext(p) == NULL)
            {
              pAdd(result, qq);
              break;
            }
            pp = pNext(p);
            for(j=1;j<=(currRing->N);j++)
            {
              pSetExp(pp,j,pGetExp(pp,j)
                          -pGetExp(res[syzIndex-1]->m[pGetComp(pp)-1],j));
            }
            pSetm(pp);
            if (pCmp(p,pNext(p)) != 1)
            {
              pp = p;
              pIter(p);
              pNext(pp) = NULL;
              result = pAdd(result, qq);
              break;
            }
            pIter(p);
          }
        }
      }
      res[syzIndex]->m[i] = p;
    }
    syzIndex--;
  }
}
#endif

BOOLEAN syTestOrder(ideal M)
{
  int i=id_RankFreeModule(M,currRing);
  if (i == 0) return FALSE;
  int j=0;

  while ((currRing->order[j]!=ringorder_c) && (currRing->order[j]!=ringorder_C))
    j++;
  if (currRing->order[j+1]!=0)
    return TRUE;
  return FALSE;
}

#if 0 /*debug only */
static void syPrintResolution(resolvente res,int start,int length)
{
  while ((start < length) && (res[start]))
  {
    Print("Syz(%d): \n",start);
    idTest(res[start]);
    //idPrint(res[start]);
    start++;
  }
}
#endif

resolvente sySchreyerResolvente(ideal arg, int maxlength, int * length,
                                BOOLEAN isMonomial, BOOLEAN /*notReplace*/)
{
  ideal mW=NULL;
  int i,syzIndex = 0,j=0;
  intvec * modcomp=NULL,*w=NULL;
  // int ** wv=NULL;
  tHomog hom=(tHomog)idHomModule(arg,NULL,&w);
  ring origR = currRing;
  ring syRing = NULL;

  if ((!isMonomial) && syTestOrder(arg))
  {
    WerrorS("sres only implemented for modules with ordering  ..,c or ..,C");
    return NULL;
  }
  *length = 4;
  resolvente res = (resolvente)omAlloc0(4*sizeof(ideal)),newres;
  res[0] = idCopy(arg);

  while ((!idIs0(res[syzIndex])) && ((maxlength==-1) || (syzIndex<maxlength)))
  {
    i = IDELEMS(res[syzIndex]);
    //while ((i!=0) && (!res[syzIndex]->m[i-1])) i--;
    if (syzIndex+1==*length)
    {
      newres = (resolvente)omAlloc0((*length+4)*sizeof(ideal));
      // for (j=0;j<*length+4;j++) newres[j] = NULL;
      for (j=0;j<*length;j++) newres[j] = res[j];
      omFreeSize((ADDRESS)res,*length*sizeof(ideal));
      *length += 4;
      res=newres;
    }

    if ((hom==isHomog)|| (rHasGlobalOrdering(origR)))
    {
      if (syzIndex==0) syInitSort(res[0],&modcomp);

      if ((syzIndex==0) && !rRing_has_CompLastBlock(currRing))
        res[syzIndex+1] = sySchreyersSyzygiesFB(res[syzIndex],&modcomp,mW,FALSE);
      else
        res[syzIndex+1] = sySchreyersSyzygiesFB(res[syzIndex],&modcomp,mW);

      if (errorreported)
      {
        for (j=0;j<*length;j++) idDelete( &res[j] );
        omFreeSize((ADDRESS)res,*length*sizeof(ideal));
        return NULL;
      }

      mW = res[syzIndex];
    }
//idPrint(res[syzIndex+1]);

    if ( /*(*/ syzIndex==0 /*)*/ )
    {
      if ((hom==isHomog)|| (rHasGlobalOrdering(origR)))
      {
        syRing = rAssure_CompLastBlock(origR, TRUE);
        if (syRing != origR)
        {
          rChangeCurrRing(syRing);
          for (i=0; i<IDELEMS(res[1]); i++)
          {
            res[1]->m[i] = prMoveR( res[1]->m[i], origR, syRing);
          }
        }
        idTest(res[1]);
      }
      else
      {
        syRing = rAssure_SyzComp_CompLastBlock(origR);
        if (syRing != origR)
        {
          rChangeCurrRing(syRing);
          for (i=0; i<IDELEMS(res[0]); i++)
          {
            res[0]->m[i] = prMoveR( res[0]->m[i], origR, syRing);
          }
        }
        idTest(res[0]);
      }
    }
    if ((hom!=isHomog) && (rHasLocalOrMixedOrdering(origR)))
    {
      if (syzIndex==0) syInitSort(res[0],&modcomp);
      res[syzIndex+1] = sySchreyersSyzygiesFM(res[syzIndex],&modcomp);
      if (errorreported)
      {
        for (j=0;j<*length;j++) idDelete( &res[j] );
        omFreeSize((ADDRESS)res,*length*sizeof(ideal));
        return NULL;
      }
    }
    syzIndex++;
    if (TEST_OPT_PROT) Print("[%d]\n",syzIndex);
  }
  //syPrintResolution(res,1,*length);
  if ((hom!=isHomog) && (rHasLocalOrMixedOrdering(origR)))
  {
    syzIndex = 1;
    while ((syzIndex < *length) && (!idIs0(res[syzIndex])))
    {
      id_Shift(res[syzIndex],-rGetMaxSyzComp(syzIndex, currRing),currRing);
      syzIndex++;
    }
  }
  if ((hom==isHomog) || (rHasGlobalOrdering(origR)))
    syzIndex = 1;
  else
    syzIndex = 0;
  syReOrderResolventFB(res,*length,syzIndex+1);
  if (/*ringOrderChanged:*/ origR!=syRing && syRing != NULL)
  {
    rChangeCurrRing(origR);
    // Thomas: Here I assume that all (!) polys of res live in tmpR
    while ((syzIndex < *length) && (res[syzIndex]))
    {
      for (i=0;i<IDELEMS(res[syzIndex]);i++)
      {
        if (res[syzIndex]->m[i])
        {
          res[syzIndex]->m[i] = prMoveR( res[syzIndex]->m[i], syRing, origR);
        }
      }
      syzIndex++;
    }
//    j = 0; while (currRing->order[j]!=0) j++; // What was this for???!
    rDelete(syRing);
  }
  else
  {
    // Thomas -- are you sure that you have to "reorder" here?
    while ((syzIndex < *length) && (res[syzIndex]))
    {
      for (i=0;i<IDELEMS(res[syzIndex]);i++)
      {
        if (res[syzIndex]->m[i])
          res[syzIndex]->m[i] = pSortCompCorrect(res[syzIndex]->m[i]);
      }
      syzIndex++;
    }
  }
  if ((hom==isHomog) || (rHasGlobalOrdering(origR)))
  {
    if (res[1]!=NULL)
    {
      syReOrderResolventFB(res,2,1);
      for (i=0;i<IDELEMS(res[1]);i++)
      {
        if (res[1]->m[i])
          res[1]->m[i] = pSort(res[1]->m[i]);
      }
    }
  }
  //syPrintResolution(res,0,*length);

  //syMergeSortResolventFB(res,*length);
  if (modcomp!=NULL) delete modcomp;
  if (w!=NULL) delete w;
  return res;
}

syStrategy sySchreyer(ideal arg, int maxlength)
{
  int rl;
  resolvente fr = sySchreyerResolvente(arg,maxlength,&(rl));
  if (fr==NULL) return NULL;

  // int typ0;
  syStrategy result=(syStrategy)omAlloc0(sizeof(ssyStrategy));
  result->length=rl;
  result->fullres = (resolvente)omAlloc0((rl /*result->length*/+1)*sizeof(ideal));
  for (int i=rl /*result->length*/-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
    {
      result->fullres[i] = fr[i];
      fr[i] = NULL;
    }
  }
  if (currRing->qideal!=NULL)
  {
    for (int i=0; i<rl; i++)
    {
      if (result->fullres[i]!=NULL)
      {
        ideal t=kNF(currRing->qideal,NULL,result->fullres[i]);
        idDelete(&result->fullres[i]);
        result->fullres[i]=t;
        if (i<rl-1)
        {
          for(int j=IDELEMS(t)-1;j>=0; j--)
          {
            if ((t->m[j]==NULL) && (result->fullres[i+1]!=NULL))
            {
              for(int k=IDELEMS(result->fullres[i+1])-1;k>=0; k--)
              {
                if (result->fullres[i+1]->m[k]!=NULL)
                {
                  pDeleteComp(&(result->fullres[i+1]->m[k]),j+1);
                }
              }
            }
          }
        }
        idSkipZeroes(result->fullres[i]);
      }
    }
    if ((rl>maxlength) && (result->fullres[rl-1]!=NULL))
    {
      idDelete(&result->fullres[rl-1]);
    }
  }
  omFreeSize((ADDRESS)fr,(rl /*result->length*/)*sizeof(ideal));
  return result;
}

