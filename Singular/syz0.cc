/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz0.cc,v 1.15 1998-04-29 07:05:31 siebert Exp $ */
/*
* ABSTRACT: resolutions
*/


#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kstd1.h"
#include "kutil.h"
#include "spolys.h"
#include "stairc.h"
#include "ipid.h"
#include "cntrlc.h"
#include "ipid.h"
#include "intvec.h"
#include "ipshell.h"
#include "numbers.h"
#include "ideals.h"
#include "intvec.h"
#include "ring.h"
#include "syz.h"


static polyset syInitSort(polyset oldF,int rkF,int Fmax,
         int syComponentOrder,intvec **modcomp)
{
  int i,j,k,kk,kkk,jj;
  polyset F;
  int Fl=Fmax;

  while ((Fl!=0) && (oldF[Fl-1]==NULL)) Fl--;
  if (*modcomp!=NULL) delete modcomp;
  *modcomp = new intvec(rkF+2);
  F=(polyset)Alloc0(Fmax*sizeof(poly));
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
        while ((kk<Fl) && (F[kk]) && (pComp0(oldF[k],F[kk])!=syComponentOrder))
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
  return F;
}

static void syCreatePairs(polyset F,int lini,int wend,int k,int j,int i,
           polyset pairs,int regularPairs=0,ideal mW=NULL)
{
  int l,ii=0,jj;
  poly p,q;

  while (((k<wend) && (pGetComp(F[k]) == i)) ||
         ((currQuotient!=NULL) && (k<regularPairs+IDELEMS(currQuotient))))
  {
    p = pOne();
    if ((k<wend) && (pGetComp(F[k]) == i) && (k!=j))
      pLcm(F[j],F[k],p);
    else if (ii<IDELEMS(currQuotient))
    {
      q = pHead(F[j]);
      if (mW!=NULL)
      {
        for(jj=1;jj<=pVariables;jj++)
          pSetExp(q,jj,pGetExp(q,jj) -pGetExp(mW->m[pGetComp(q)-1],jj));
        pSetm(q);
      }
      pLcm(q,currQuotient->m[ii],p);
      if (mW!=NULL)
      {
        for(jj=1;jj<=pVariables;jj++)
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

static poly syRedtail2(poly p, polyset redWith, intvec *modcomp, spSpolyLoopProc SpolyLoop)
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
      if (pDivisibleBy2(redWith[j], hn))
      {
        //if (TEST_OPT_PROT) Print("r");
        hn = spSpolyRed(redWith[j],hn,NULL, SpolyLoop);
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
* input: F, Fmax,  noSort: F is already ordered by: Schreyer-order
*              (only allocated: Shdl, Smax)
* output: Shdl, Smax
*/
void sySchreyersSyzygiesFM(polyset F,int Fmax,polyset* Shdl,int* Smax,
  BOOLEAN noSort)
{
  int Fl=Fmax;
  while ((Fl!=0) && (F[Fl-1]==NULL)) Fl--;
  if (Fl==0) return;

  int i,j,l,k,totalToRed,ecartToRed,kk,bestEcart,totalmax,rkF,
    Sl=0,smax,tmax,tl;
  int *ecartS, *ecartT, *totalS,
    *totalT=NULL, *temp=NULL;
  intvec * modcomp=NULL;
  polyset pairs,S,T,ST,oldF;
  poly p,q,toRed;
  BOOLEAN notFound = FALSE;

/*-------------initializing the sets--------------------*/
  ideal idF=(ideal)Alloc(sizeof(ip_sideal));
  ST=(polyset)Alloc0(Fl*sizeof(poly));
  S=(polyset)Alloc0(Fl*sizeof(poly));
  ecartS=(int*)Alloc(Fl*sizeof(int));
  totalS=(int*)Alloc(Fl*sizeof(int));
  T=(polyset)Alloc0(2*Fl*sizeof(poly));
  ecartT=(int*)Alloc(2*Fl*sizeof(int));
  totalT=(int*)Alloc(2*Fl*sizeof(int));
  pairs=(polyset)Alloc0(Fl*sizeof(poly));

  smax = Fl;
  tmax = 2*Fl;
  idF->m=F;IDELEMS(idF)=Fmax;
  rkF=idRankFreeModule(idF);
  Free((ADDRESS)idF,sizeof(ip_sideal));
  spSet(currRing);
/*-------------sorting of F for index handling------------*/
  if (noSort)
  {
    oldF = F;
    F=syInitSort(F,rkF,Fmax,1,&modcomp);
  }
/*----------------construction of the new ordering----------*/
  pSetSchreyerOrdM(F,Fl,rkF);
/*----------------creating S--------------------------------*/
  for(j=0;j<Fl;j++)
  {
    S[j] = pCopy(F[j]);
    totalS[j] = pLDeg(S[j],&k);
    ecartS[j] = totalS[j]-pFDeg(S[j]);
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
    i = pGetComp(S[j]);
    k=j+1;
/*----------------constructing all pairs with S[j]---------*/
    if (TEST_OPT_PROT)
    {
      Print("(%d)",Fl-j);
      mflush();
    }
    syCreatePairs(S,j+1,Fl,k,j,i,pairs);
/*--------------computing the syzygies----------------------*/
    for (k=j+1;k<Fl;k++)
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
/*--------------begin to reduce-----------------------------*/
        toRed = spSpolyCreate(S[j],S[k],NULL);
        ecartToRed = 1;
        bestEcart = 1;
        if (BTEST1(6))
        {
          PrintS("pair: ");pWrite0(S[j]);PrintS(" ");pWrite(S[k]);
        }
        if (TEST_OPT_PROT)
        {
           PrintS(".");
           mflush();
        }
        while (pGetComp(toRed)<=rkF)
        {
          if (BTEST1(6))
          {
            PrintS("toRed: ");pWrite(toRed);
          }
/*
*         if ((bestEcart) || (ecartToRed!=0))
*         {
*/
            totalToRed = pLDeg(toRed,&kk);
            ecartToRed = totalToRed-pFDeg(toRed);
/*
*         }
*/
          notFound = TRUE;
          l=0;
          bestEcart = 32000;  //a very large integer
          p = NULL;
          while ((l<tl) && (pGetComp(T[l])<pGetComp(toRed))) l++;
          while ((l<tl) && (notFound))
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
            WerrorS("ideal not a standardbasis");//no polynom for reduction
            pDelete(&toRed);
            for(k=j;k<Fl;k++) pDelete(&(pairs[k]));
            Free((ADDRESS)pairs,Fl*sizeof(poly));
            Free((ADDRESS)ST,Fl*sizeof(poly));
            Free((ADDRESS)S,Fl*sizeof(poly));
            Free((ADDRESS)T,tmax*sizeof(poly));
            Free((ADDRESS)ecartT,tmax*sizeof(int));
            Free((ADDRESS)totalT,tmax*sizeof(int));
            Free((ADDRESS)ecartS,Fl*sizeof(int));
            Free((ADDRESS)totalS,Fl*sizeof(int));
            if (noSort)
            {
              Free((ADDRESS)F,Fl*sizeof(poly));
              F = oldF;
            }
            for(k=0;k<*Smax;k++) pDelete(&((*Shdl)[k]));
            return;
          }
          else
          {
//PrintS("reduced with: ");pWrite(p);PrintLn();
            if (notFound)
            {
              if (tl>=tmax)
              {
                pEnlargeSet(&T,tmax,16);
                tmax += 16;
                temp = (int*)Alloc((tmax+16)*sizeof(int));
                for(l=0;l<tmax;l++) temp[l]=totalT[l];
                totalT = temp;
                temp = (int*)Alloc((tmax+16)*sizeof(int));
                for(l=0;l<tmax;l++) temp[l]=ecartT[l];
                ecartT = temp;
              }
//PrintS("t");
              l=0;
              while ((l<tl) && (pGetComp(toRed)>pGetComp(T[l]))) l++;
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

            toRed = spSpolyRed(p,toRed,NULL, NULL);
          }
        }
//PrintS("s");
        if (pGetComp(toRed)>rkF)
        {
          if (Sl>=*Smax)
          {
            pEnlargeSet(Shdl,*Smax,16);
            *Smax += 16;
          }
          pShift(&toRed,-rkF);
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
      }
    }
    for(k=j;k<Fl;k++) pDelete(&(pairs[k]));
  }
  Free((ADDRESS)pairs,Fl*sizeof(poly));
  Free((ADDRESS)ST,Fl*sizeof(poly));
  Free((ADDRESS)S,Fl*sizeof(poly));
  Free((ADDRESS)T,tmax*sizeof(poly));
  Free((ADDRESS)ecartT,tmax*sizeof(int));
  Free((ADDRESS)totalT,tmax*sizeof(int));
  Free((ADDRESS)ecartS,Fl*sizeof(int));
  Free((ADDRESS)totalS,Fl*sizeof(int));
  if (noSort)
  {
    if (modcomp!=NULL) delete modcomp;
    Free((ADDRESS)F,Fl*sizeof(poly));
    F = oldF;
  }
}

/*3
*special Normalform for Schreyer in factor rings
*/
poly sySpecNormalize(poly toNorm,ideal mW=NULL, spSpolyLoopProc SpolyLoop=NULL)
{
  int j,i=0;
  poly p;
  
  if (toNorm==NULL) return NULL;
  p = pHead(toNorm);
  if (mW!=NULL)
  {
    for(j=1;j<=pVariables;j++)
      pSetExp(p,j,pGetExp(p,j) -pGetExp(mW->m[pGetComp(p)-1],j));
  }
  while ((p!=NULL) && (i<IDELEMS(currQuotient)))
  {
    if (pDivisibleBy(currQuotient->m[i],p))
    {
      //pNorm(toNorm);
      toNorm = spSpolyRed(currQuotient->m[i],toNorm,NULL, SpolyLoop);
      pDelete(&p); 
      if (toNorm==NULL) return NULL;
      p = pHead(toNorm);
      if (mW!=NULL)
      {
        for(j=1;j<=pVariables;j++)
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
* input: F, Fmax,  noSort: F is already ordered by: Schreyer-order
*              (only allocated: Shdl, Smax)
* output: Shdl, Smax
* modcomp, length stores the start position of the module comp. in FF
*/
void sySchreyersSyzygiesFB(polyset *FF,int Fmax,polyset* Shdl,int* Smax,
   BOOLEAN noSort,intvec ** modcomp, int * length,ideal mW)
{
  int i,j,l,k,kkk,rkF,Sl=0,Fl=Fmax,syComponentOrder=pModuleOrder();
  int fstart,wend,lini;
  intvec *newmodcomp;
  polyset pairs,oldF,F=*FF;
  poly p,q,toRed,syz,lastmonom,multWith;
  ideal idF=(ideal)Alloc(sizeof(*idF)),null;
  BOOLEAN isNotReduced=TRUE;

  while ((Fl!=0) && (F[Fl-1]==NULL)) Fl--;
  newmodcomp = new intvec(Fl+2);
//for (j=0;j<Fl;j++) pWrite(F[j]);
//PrintLn();
  if (currQuotient==NULL)
    pairs=(polyset)Alloc0(Fl*sizeof(poly));
  else
    pairs=(polyset)Alloc0((Fl+IDELEMS(currQuotient))*sizeof(poly));
  idF->m=F;IDELEMS(idF)=Fmax;
  rkF=idRankFreeModule(idF);
  null = idInit(1,rkF);
  Free((ADDRESS)idF,sizeof(*idF));
  if (noSort)
  {
    oldF = *FF;
    F=syInitSort(*FF,rkF,Fmax,syComponentOrder,modcomp);
  }
  else
  {
    F = *FF;
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
    if (currQuotient!=NULL) wend = Fl+IDELEMS(currQuotient);
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
          syz = pCopy(pairs[k]);
          syz->coef = nCopy(F[k]->coef);
          syz->coef = nNeg(syz->coef);
          pNext(syz) = pairs[k];
          lastmonom = pNext(syz);
          lastmonom->coef = nCopy(F[j]->coef);
          pSetComp(lastmonom,k+1);
        }
        else
        {
          syz = pairs[k];
          syz->coef = nCopy(currQuotient->m[k-Fl]->coef);
          lastmonom = syz;
          multWith = pDivide(syz,F[j]);
          multWith->coef = nCopy(currQuotient->m[k-Fl]->coef);
        }
        pSetComp(syz,j+1);
        pairs[k] = NULL;
        //the next term of the syzygy
        //constructs the spoly
        if (BTEST1(6))
        {
          if (k<Fl)
          {
            PrintS("pair: ");pWrite0(F[j]);PrintS("  ");pWrite(F[k]);
          }
          else
          {
            PrintS("pair: ");pWrite0(F[j]);PrintS("  ");pWrite(currQuotient->m[k-Fl]);
          }
        }
        if (k<Fl)
          toRed = spSpolyCreate(F[j],F[k],NULL);
        else
        {
          q = pMultT(pCopy(F[j]),multWith);
          toRed = sySpecNormalize(q,mW, NULL);
          pDelete(&multWith);
        }
        isNotReduced = TRUE;
        while (toRed!=NULL)
        {
          if (BTEST1(6))
          {
            PrintS("toRed: ");pWrite(toRed);
          }
//          l=0;
//          while ((l<Fl) && (!pDivisibleBy(F[l],toRed))) l++;
//          if (l>=Fl)
          l = (**modcomp)[pGetComp(toRed)+1]-1;
          kkk = (**modcomp)[pGetComp(toRed)];
          while ((l>=kkk) && (!pDivisibleBy(F[l],toRed))) l--;
          if (l<kkk)
          {
            if ((currQuotient!=NULL) && (isNotReduced))
            {
              toRed = sySpecNormalize(toRed,mW, NULL);
              isNotReduced = FALSE;
            }
            else
            {
              //no polynom for reduction
              WerrorS("ideal not a standardbasis");
              pDelete(&toRed);
              pDelete(&syz);
              for(k=j;k<Fl;k++) pDelete(&(pairs[k]));
              Free((ADDRESS)pairs,Fl*sizeof(poly));
              if (noSort)
              {
                Free((ADDRESS)F,Fl*sizeof(poly));
                F = oldF;
              }
              for(k=0;k<*Smax;k++) pDelete(&((*Shdl)[k]));
              return;
            }
          }
          else
          {
            //the next monom of the syzygy
            isNotReduced = TRUE;
            if (BTEST1(6))
            {
              PrintS("reduced with: ");pWrite(F[l]);
            }
            multWith = pDivide(toRed,F[l]);
            multWith->coef = nDiv(toRed->coef,F[l]->coef);
            multWith->coef = nNeg(multWith->coef);
            pNext(lastmonom) = toRed;
            pIter(lastmonom);
            pIter(toRed);
            pNext(lastmonom) = NULL;
            lastmonom->coef = nDiv(lastmonom->coef,F[l]->coef);
            lastmonom->coef = nNeg(lastmonom->coef);
            pSetComp(lastmonom,l+1);
            //computes the new toRed
            p = pCopy(pNext(F[l]));
            p = pMultT(p,multWith);
            pDelete(&multWith);
            toRed = pAdd(toRed,p);
            //the module component of the new monom
//pWrite(toRed);
          }
        }
//PrintLn();
        if (syz!=NULL)
        {
          if (Sl>=*Smax)
          {
            pEnlargeSet(Shdl,*Smax,16);
            *Smax += 16;
          }
          pNorm(syz);
          if (BTEST1(OPT_REDTAIL))
          {
            (*newmodcomp)[j+2] = Sl;
            (*Shdl)[Sl] = syRedtail2(syz,*Shdl,newmodcomp, NULL);
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
  if (currQuotient==NULL)
    Free((ADDRESS)pairs,Fl*sizeof(poly));
  else
    Free((ADDRESS)pairs,(Fl+IDELEMS(currQuotient))*sizeof(poly));
  if (noSort)
  {
    Free((ADDRESS)oldF,Fmax*sizeof(poly));
    *FF = F;
  }
  delete *modcomp;
  *length = Fl+2;
  *modcomp = newmodcomp;
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
          for(j=1;j<=pVariables;j++)
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

BOOLEAN syTestOrder(ideal M)
{
  int i=idRankFreeModule(M);
  int j=0;

  while ((currRing->order[j]!=ringorder_c) && (currRing->order[j]!=ringorder_C))
    j++;
  if ((i>0) && (currRing->order[j+1]!=0))
  {
    return TRUE;
  }
  return FALSE;
}

resolvente sySchreyerResolvente(ideal arg, int maxlength, int * length,
  BOOLEAN isMonomial,BOOLEAN notReplace)
{
  ideal mW=NULL;
  int i,syzIndex = 0,j=0,lgth,*ord=NULL,*bl0=NULL,*bl1=NULL;
  intvec * modcomp=NULL,*w=NULL;
  short ** wv=NULL;
  BOOLEAN sort = TRUE;
  tHomog hom=(tHomog)idHomModule(arg,NULL,&w);
  ring origR = currRing;
  sip_sring tmpR;

  if ((!isMonomial) && syTestOrder(arg))
  {
    WerrorS("sres only implemented for modules with ordering  ..,c or ..,C");
    return NULL;
  }
  *length = 4;
  resolvente res = (resolvente)Alloc0(4*sizeof(ideal)),newres;
  res[0] = idCopy(arg);
  while ((!idIs0(res[syzIndex])) && ((maxlength==-1) || (syzIndex<maxlength)))
  {
    i = IDELEMS(res[syzIndex]);
    //while ((i!=0) && (!res[syzIndex]->m[i-1])) i--;
    if (syzIndex+1==*length)
    {
      newres = (resolvente)Alloc((*length+4)*sizeof(ideal));
      for (j=0;j<*length+4;j++) newres[j] = NULL;
      for (j=0;j<*length;j++) newres[j] = res[j];
      Free((ADDRESS)res,*length*sizeof(ideal));
      *length += 4;
      res=newres;
    }
    res[syzIndex+1] = idInit(16,1);
    if ((currRing->OrdSgn == 1) || (hom==isHomog))
    {
      sySchreyersSyzygiesFB(&(res[syzIndex]->m),i,&(res[syzIndex+1]->m),
        &(IDELEMS(res[syzIndex+1])),sort,&modcomp,&lgth,mW);
      mW = res[syzIndex];
    }
    else
      sySchreyersSyzygiesFM(res[syzIndex]->m,i,&(res[syzIndex+1]->m),
        &(IDELEMS(res[syzIndex+1])),sort);
//idPrint(res[syzIndex+1]);
    if ((syzIndex==0) && (currRing->OrdSgn==1))
    {
      j = 0;
      while ((currRing->order[j]!=ringorder_c)
              && (currRing->order[j]!=ringorder_C))
        j++;
      if ((!notReplace) && (currRing->order[j]!=0))
      {
        while (currRing->order[j]!=0) j++;
        ord = (int*)Alloc0((j+2)*sizeof(int));
        wv = (short**)Alloc0((j+2)*sizeof(short*));
        bl0 = (int*)Alloc0((j+2)*sizeof(int));
        bl1 = (int*)Alloc0((j+2)*sizeof(int));
        j = 0;
        while ((currRing->order[j]!=ringorder_c)
                && (currRing->order[j]!=ringorder_C))
        {
          ord[j] = currRing->order[j];
          bl0[j] = currRing->block0[j];
          bl1[j] = currRing->block1[j];
          wv[j] = currRing->wvhdl[j];
          j++;
        }
        int m_order=j;
        while (currRing->order[j+1]!=0)
        {
          ord[j] = currRing->order[j+1];
          bl0[j] = currRing->block0[j+1];
          bl1[j] = currRing->block1[j+1];
          wv[j] = currRing->wvhdl[j+1];
          j++;
        }
        ord[j] = currRing->order[m_order];
        bl0[j] = currRing->block0[m_order];
        bl1[j] = currRing->block1[m_order];
        wv[j] = currRing->wvhdl[m_order];
	tmpR = *currRing;
	tmpR.order = ord;
	tmpR.block0 = bl0;
	tmpR.block1 = bl1;
	tmpR.wvhdl = wv;
	rComplete(&tmpR);
        rChangeCurrRing(&tmpR, TRUE);
      }
    }
    if (sort) sort=FALSE;
    syzIndex++;
    if (TEST_OPT_PROT) Print("[%d]\n",syzIndex);
  }
  if (currRing->OrdSgn == -1)
    pSetSchreyerOrdM(NULL,0,0);
  syReOrderResolventFB(res,*length);
  syzIndex = 1;
  if (/*ringOrderChanged:*/ ord!=NULL)
  {
    j = 0;
    while (currRing->order[j]!=0) j++;
    Free((ADDRESS)ord,(j+2)*sizeof(int));
    Free((ADDRESS)bl0,(j+2)*sizeof(int));
    Free((ADDRESS)bl1,(j+2)*sizeof(int));
    Free((ADDRESS)wv,(j+2)*sizeof(short*));
    rChangeCurrRing(origR, TRUE);
  }
  while ((syzIndex < *length) && (res[syzIndex]))
  {
    for (i=0;i<IDELEMS(res[syzIndex]);i++)
    {
      if (res[syzIndex]->m[i])
        res[syzIndex]->m[i] = pOrdPolyMerge(res[syzIndex]->m[i]);
    }
    syzIndex++;
  }
  if (modcomp!=NULL) delete modcomp;
  if (w!=NULL) delete w;
  return res;
}

syStrategy sySchreyer(ideal arg, int maxlength)
{
  int typ0;
  syStrategy result=(syStrategy)Alloc0(sizeof(ssyStrategy));

  resolvente fr = sySchreyerResolvente(arg,maxlength,&(result->length));
  result->fullres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      result->fullres[i] = fr[i];
      fr[i] = NULL;
  }
  Free((ADDRESS)fr,(result->length)*sizeof(ideal));
  return result;
}

