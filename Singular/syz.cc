/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz.cc,v 1.11 1998-10-06 14:37:54 siebert Exp $ */

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

static intvec * syPrepareModComp(ideal arg,intvec ** w)
{
  intvec *w1 = NULL;
  int i;
  BOOLEAN isIdeal=FALSE;

  if ((w==NULL) || (*w==NULL)) return w1;
  int maxxx = (*w)->length();
  if (maxxx==1)
  {
    maxxx = 2;
    isIdeal = TRUE;
  }
  w1 = new intvec(maxxx+IDELEMS(arg));
  if (!isIdeal)
  {
    for (i=0;i<maxxx;i++)
    {
      (*w1)[i] = (**w)[i];
    }
  }
  for (i=maxxx;i<maxxx+IDELEMS(arg);i++)
  {
    if (arg->m[i-maxxx]!=NULL)
    {
      (*w1)[i] = pFDeg(arg->m[i-maxxx]);
      if (pGetComp(arg->m[i-maxxx])!=0)
      {
        (*w1)[i]+=(**w)[pGetComp(arg->m[i-maxxx])-1];
      }
    }
  }
  delete (*w);
  *w = new intvec(IDELEMS(arg)+1);
  for (i=0;i<IDELEMS(arg);i++)
  {
     (**w)[i+1] = (*w1)[i+maxxx];
  }
  return w1;
}

static void syDeleteAbove(ideal up, int k)
{
  if (up!=NULL)
  {
    for (int i=0;i<IDELEMS(up);i++)
    {
      if (up->m[i]!=NULL)
        pDeleteComp(&(up->m[i]),k+1);
    }
  }
}

/*2
*minimizes the module mod and cancel superfluous syzygies
*from syz
*/
static void syMinStep(ideal mod,ideal syz,BOOLEAN final=FALSE,ideal up=NULL,
                      tHomog h=isNotHomog)
{
  ideal deg0=NULL;
  poly Unit1,Unit2,actWith;
  int len,i,j,ModComp,m,k,l;
  BOOLEAN searchUnit,existsUnit;

  if (TEST_OPT_PROT) Print("m");
  if ((final) && (h==isHomog))
  /*minim is TRUE, we are in the module: maxlength, maxlength <>0*/
  {
    deg0=idJet(syz,0);
    idSkipZeroes(deg0);
    syz=deg0;
  }
/*--cancels empty entees and their related components above--*/
  j = IDELEMS(syz);
  while ((j>0) && (!syz->m[j-1])) j--;
  k = 0;
  while (k<j)
  {
    if (syz->m[k]!=NULL)
      k++;
    else
    {
      if (TEST_OPT_PROT) Print(".");
      for (l=k;l<j-1;l++) syz->m[l] = syz->m[l+1];
      syz->m[j-1] = NULL;
      syDeleteAbove(up,k);
      j--;
    }
  }
/*--searches for syzygies coming from superfluous elements
* in the module below--*/
  searchUnit = TRUE;
  while (searchUnit)
  {
    i=0;
    j=IDELEMS(syz);
    while ((j>0) && (syz->m[j-1]==NULL)) j--;
    existsUnit = FALSE;
    if (currRing->OrdSgn == 1)
    {
      while ((i<j) && (!existsUnit))
      {
        existsUnit = pVectorHasUnitB(syz->m[i],&ModComp);
        i++;
      }
    }
    else
    {
      int I=0;
      l = 0;
      len=0;
      for (i=0;i<IDELEMS(syz);i++)
      {
        if (syz->m[i]!=NULL)
        {
          pVectorHasUnit(syz->m[i],&m, &l);
          if ((len==0) ||((l>0) && (l<len)))
          {
            len = l;
            ModComp = m;
            I = i;
          }
        }
      }
//Print("Laenge ist: %d\n",len);
      if (len>0) existsUnit = TRUE;
      i = I+1;
    }
    if (existsUnit)
    {
      i--;
//--takes out the founded syzygy--
      if (TEST_OPT_PROT) Print("f");
      actWith = syz->m[i];
      if (currRing->ch<2) pCleardenom(actWith);
//Print("actWith: ");pWrite(actWith);
      syz->m[i] = NULL;
      for (k=i;k<j-1;k++)  syz->m[k] = syz->m[k+1];
      syz->m[j-1] = NULL;
      syDeleteAbove(up,i);
      j--;
//--makes Gauss alg. for the column ModComp--
      Unit1 = pTakeOutComp(&(actWith), ModComp);
//Print("actWith now: ");pWrite(actWith);
//Print("Unit1: ");pWrite(Unit1);
      k=0;
//Print("j= %d",j);
      while (k<j)
      {
        if (syz->m[k]!=NULL)
        {
          Unit2 = pTakeOutComp(&(syz->m[k]), ModComp);
//Print("element %d: ",k);pWrite(syz->m[k]);
//Print("Unit2: ");pWrite(Unit2);
          syz->m[k] = pMult(syz->m[k],pCopy(Unit1));
          syz->m[k] = pSub(syz->m[k],
            pMult(pCopy(actWith),Unit2));
          if (syz->m[k]==NULL)
          {
            for (l=k;l<j-1;l++)
              syz->m[l] = syz->m[l+1];
            syz->m[j-1] = NULL;
            j--;
            syDeleteAbove(up,k);
            k--;
          }
        }
        k++;
      }
      pDelete(&actWith);
      pDelete(&Unit1);
//--deletes superfluous elements from the module below---
      pDelete(&(mod->m[ModComp-1]));
      for (k=ModComp-1;k<IDELEMS(mod)-1;k++)
        mod->m[k] = mod->m[k+1];
      mod->m[IDELEMS(mod)-1] = NULL;
    }
    else
      searchUnit = FALSE;
  }
  if (TEST_OPT_PROT) Print("\n");
  idSkipZeroes(mod);
  idSkipZeroes(syz);
  if (deg0!=NULL)
    idDelete(&deg0);
}

void syMinimizeResolvente(resolvente res, int length, int first)
{
  int syzIndex=first;
  intvec * dummy;

  if (syzIndex<1) syzIndex=1;
/*                            Noch zu testen!!
*  if ((syzIndex==1) && (idHomModule(res[0],currQuotient,&dummy)))
*  {
*    delete dummy;
*    resolvente res1=syFastMin(res,length);
*    int i;
*    for (i=0;i<length;i++)
*    {
*      idDelete(&res[i]);
*      res[i] = res1[i];
*    }
*    Free((ADDRESS)res1,length*sizeof(ideal));
*    return;
*  }
*/
  while ((syzIndex<length-1) && (res[syzIndex]!=NULL) && (res[syzIndex+1]!=NULL))
  {
    syMinStep(res[syzIndex-1],res[syzIndex],FALSE,res[syzIndex+1]);
    syzIndex++;
  }
  if (res[syzIndex]!=NULL)
    syMinStep(res[syzIndex-1],res[syzIndex]);
}

/*2
* resolution of ideal/module arg, <=maxlength steps, (r[0..maxlength])
*   no limitation in length if maxlength==0
* input:arg
*       minim: TRUE means mres cmd, FALSE res cmd.
*       if *len!=0: modul weights: weights[0]
*          (and weights is defined:weights[0..len-1]
*
* output:resolvente r[0..length-1],
*        modul weights: weights[0..length-1]
*/
resolvente syResolvente(ideal arg, int maxlength, int * length,
                        intvec *** weights, BOOLEAN minim)
{
  resolvente res;
  resolvente newres;
  tHomog hom=isNotHomog;
  ideal temp=NULL;
  intvec *w,*w1=NULL,**tempW;
  int i,k,syzIndex = 0,j;
  int Kstd1_OldDeg=Kstd1_deg;
  BOOLEAN completeMinim;
  BOOLEAN oldDegBound=TEST_OPT_DEGBOUND;
  int wlength=*length;

  if (maxlength!=-1) *length = maxlength+1;
  else              *length = 5;
  if ((wlength!=0) && (*length!=wlength))
  {
    intvec **wtmp = (intvec**)Alloc0((*length)*sizeof(intvec*));
    wtmp[0]=(*weights)[0];
    Free((ADDRESS)*weights,wlength*sizeof(intvec*));
    *weights=wtmp;
  }
  res = (resolvente)Alloc0((*length)*sizeof(ideal));
  res[0] = idCopy(arg);
  if ((weights==NULL) || (*weights==NULL) || ((*weights)[0]==NULL))
  {
    hom=(tHomog)idHomModule(res[0],currQuotient,&w);
    if (hom==isHomog)
    {
      *weights = (intvec**)Alloc0((*length)*sizeof(intvec*));
      if (w!=NULL) (*weights)[0] = ivCopy(w);
//Print("die %d Modulegewichte sind:\n",w->length());
//w->show();
//Print("\n");
    }
  }
  else
  {
    if ((weights!=NULL) && (*weights!=NULL)&& ((*weights)[0]!=NULL))
    {
      w = ivCopy((*weights)[0]);
      hom = isHomog;
    }
  }
  if (hom==isHomog)
  {
    w1 = syPrepareModComp(res[0],&w);
    if (w!=NULL) { delete w;w=NULL; }
  }
  while ((!idIs0(res[syzIndex])) &&
         ((maxlength==-1) || (syzIndex<=maxlength)))
   // (syzIndex<maxlength+(int)minim)))
  /*compute one step more for minimizing*/
  {
    if (Kstd1_deg!=0) Kstd1_deg++;
    if (syzIndex+1==*length)
    {
      newres = (resolvente)Alloc0((*length+5)*sizeof(ideal));
      tempW = (intvec**)Alloc0((*length+5)*sizeof(intvec*));
      for (j=0;j<*length;j++)
      {
        newres[j] = res[j];
        if (*weights!=NULL) tempW[j] = (*weights)[j];
        /*else              tempW[j] = NULL;*/
      }
      Free((ADDRESS)res,*length*sizeof(ideal));
      Free((ADDRESS)*weights,*length*sizeof(intvec*));
      *length += 5;
      res=newres;
      *weights = tempW;
    }
    if (minim || (syzIndex!=0))
    {
      temp = kInterRed(res[syzIndex],currQuotient);
      idDelete(&res[syzIndex]);
      idSkipZeroes(temp);
      res[syzIndex] = temp;
    }
    temp = NULL;
    if ((currQuotient==NULL)&&(syzIndex==0)&& (!TEST_OPT_DEGBOUND))
    {
      res[/*syzIndex+*/1] = idSyzygies(res[0/*syzIndex*/],
        NULL/*currQuotient*/,hom,&w1,TRUE,Kstd1_deg);
      if ((!TEST_OPT_NOTREGULARITY) && (Kstd1_deg>0)) test |= Sy_bit(OPT_DEGBOUND);
    }
    else
      res[syzIndex+1] = idSyzygies(res[syzIndex],currQuotient,hom,&w1);
    completeMinim=(syzIndex!=maxlength) || (maxlength ==-1) || (hom!=isHomog);
    syzIndex++;
    if (TEST_OPT_PROT) Print("[%d]\n",syzIndex);
    if ((minim)||(syzIndex>1))
      syMinStep(res[syzIndex-1],res[syzIndex],!completeMinim,NULL,hom);
    if (!completeMinim)
    /*minim is TRUE, we are in the module: maxlength, maxlength <>0*/
    {
      idDelete(&res[syzIndex]);
    }
    if ((hom == isHomog) && (!idIs0(res[syzIndex])))
    {
//Print("die %d Modulegewichte sind:\n",w1->length());
//w1->show();
//Print("\n");
      k = idRankFreeModule(res[syzIndex]);
      w = new intvec(k+IDELEMS(res[syzIndex]));
      (*weights)[syzIndex] = new intvec(k);
      for (i=0;i<k;i++)
      {
        if (res[syzIndex-1]->m[i]!=NULL) // hs
        {
          (*w)[i] = pFDeg(res[syzIndex-1]->m[i]);
          if (pGetComp(res[syzIndex-1]->m[i])>0)
            (*w)[i] += (*w1)[pGetComp(res[syzIndex-1]->m[i])-1];
          (*((*weights)[syzIndex]))[i] = (*w)[i];
        }
      }
      for (i=k;i<k+IDELEMS(res[syzIndex]);i++)
      {
        if (res[syzIndex]->m[i-k]!=NULL)
          (*w)[i] = pFDeg(res[syzIndex]->m[i-k])
                    +(*w)[pGetComp(res[syzIndex]->m[i-k])-1];
      }
      delete w1;
      w1 = w;
      w = NULL;
    }
  }
  if ((syzIndex!=0) && (res[syzIndex]!=NULL) && (idIs0(res[syzIndex])))
    idDelete(&res[syzIndex]);
  if (w1!=NULL) delete w1;
  //if ((maxlength!=-1) && (!minim))
  //{
  //  while (syzIndex<=maxlength)
  //  {
  //    res[syzIndex]=idInit(1,1);
  //    syzIndex++;
  //    if (syzIndex<=maxlength)
  //    {
  //      res[syzIndex]=idFreeModule(1);
  //      syzIndex++;
  //    }
  //  }
  //}
  Kstd1_deg=Kstd1_OldDeg;
  if (!oldDegBound)
    test &= ~Sy_bit(OPT_DEGBOUND);
  return res;
}

syStrategy syResolution(ideal arg, int maxlength,intvec * w, BOOLEAN minim)
{
  int typ0;
  syStrategy result=(syStrategy)Alloc0(sizeof(ssyStrategy));

  if (w!=NULL)
  {
    result->weights = (intvec**)Alloc0(sizeof(intvec*));
    (result->weights)[0] = ivCopy(w);
    result->length = 1;
  }
  resolvente fr = syResolvente(arg,maxlength,&(result->length),&(result->weights),minim),fr1;
  if (minim)
  {
    result->minres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
    fr1 =  result->minres;
  }
  else
  {
    result->fullres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
    fr1 =  result->fullres;
  }
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      fr1[i] = fr[i];
    fr[i] = NULL;
  }
  Free((ADDRESS)fr,(result->length)*sizeof(ideal));
  return result;
}


resolvente syMinRes(ideal arg, int maxlength, int * length, BOOLEAN minim)
{
  resolvente res;
  resolvente newres;
  tHomog hom;
  ideal temp=NULL;
  intvec *w,*w1=NULL;
  int i,k,syzIndex = 0,j;
  int Kstd1_OldDeg=Kstd1_deg;
  BOOLEAN isIdeal=FALSE,oldDegBound=TEST_OPT_DEGBOUND;

  if (maxlength!=-1) *length = maxlength+1;
  else              *length = 4;
  res = (resolvente)Alloc0((*length)*sizeof(ideal));
  res[0] = idCopy(arg);
  hom=(tHomog)idHomModule(res[0],currQuotient,&w);
//Print("die %d Modulegewichte sind:\n",w->length());
//w->show();
//PrintLn();
  if (hom==isHomog)
  {
    //if (w!=NULL)
    //{
    //  w->show();
    //  PrintLn();
    //  if (res[0]!=NULL) jjPRINT_MA0(idModule2Matrix(idCopy(res[0])),NULL);
    //}
    int maxxx = 0;
    if (w!=NULL) maxxx = w->length();
    if (maxxx==1)
    {
      maxxx = 2;
      isIdeal = TRUE;
    }
    w1 = new intvec(maxxx+IDELEMS(arg));
    if (!isIdeal)
    {
      for (i=0;i<maxxx;i++)
      {
        (*w1)[i] = (*w)[i];
      }
    }
    for (i=maxxx;i<maxxx+IDELEMS(arg);i++)
    {
      if (res[0]->m[i-maxxx]!=0)
        (*w1)[i] = pFDeg(res[0]->m[i-maxxx])+(*w)[pGetComp(res[0]->m[i-maxxx])];
    }
    if (w!=NULL) {delete w;w=NULL;}
  }
  while ((!idIs0(res[syzIndex]))
  && ((maxlength==-1)
    || (syzIndex<maxlength)))
   // (syzIndex<maxlength+(int)minim)))
  /*compute one step more for minimizing*/
  {
    if (Kstd1_deg!=0) Kstd1_deg++;
    if (syzIndex+1==*length)
    {
      newres = (resolvente)Alloc0((*length+4)*sizeof(ideal));
      for (j=0;j<*length;j++) newres[j] = res[j];
      Free((ADDRESS)res,*length*sizeof(ideal));
      *length += 4;
      res=newres;
    }
    if (minim || (syzIndex!=0))
    {
      temp = kInterRed(res[syzIndex],currQuotient);
      idDelete(&res[syzIndex]);
      res[syzIndex] = temp;
    }
    temp = NULL;
    if ((hom==isHomog) && (currRing->OrdSgn==1))
    {
//PrintS("Input: ");
//for (i=0;i<IDELEMS(res[syzIndex]);i++)
//{
//Print("Element %d: ",i);pWrite(res[syzIndex]->m[i]);
//}
      if ((currQuotient==NULL)&&(syzIndex==0)&& (!TEST_OPT_DEGBOUND))
      {
        res[/*syzIndex+*/1] = idSyzMin(res[0/*syzIndex*/],
          NULL/*currQuotient*/,hom,&w1,TRUE,Kstd1_deg);
        if ((!TEST_OPT_NOTREGULARITY) && (Kstd1_deg>0)) test |= Sy_bit(OPT_DEGBOUND);
//Print("Regularity is %d \n",Kstd1_deg);
      }
      else
        res[syzIndex+1] = idSyzMin(res[syzIndex],
          currQuotient,hom,&w1,FALSE,Kstd1_deg);
//PrintS("Input: ");
//for (i=0;i<IDELEMS(res[syzIndex+1]);i++)
//{
//Print("Element %d: ",i);pWrite(res[syzIndex+1]->m[i]);
//}
    }
    else
      res[syzIndex+1] = idSyzygies(res[syzIndex],currQuotient,hom,&w1);
//Print("im %d-ten Syzygienmodul: \n",syzIndex+1);
//for (i=0;i<IDELEMS(res[syzIndex+1]);i++)
//{
//Print("El %d: ",i);pWrite(res[syzIndex+1]->m[i]);
//}
    syzIndex++;
    if ((maxlength!=-1) && (syzIndex==maxlength)) idDelete(&res[syzIndex]);
    if (TEST_OPT_PROT) Print("[%d]\n",syzIndex);
    if  ((hom==isHomog) && (res[syzIndex]!=NULL))
    {
//Print("die %d Modulegewichte sind:\n",w1->length());
//w1->show();
//PrintLn();
      k = idRankFreeModule(res[syzIndex]);
      w = new intvec(k+IDELEMS(res[syzIndex])+1);
      for (i=1;i<=k;i++)
      {
        (*w)[i] = pFDeg(res[syzIndex-1]->m[i-1])
                  +(*w1)[pGetComp(res[syzIndex-1]->m[i-1])];
      }
      for (i=k+1;i<k+1+IDELEMS(res[syzIndex]);i++)
      {
        if (res[syzIndex]->m[i-k-1]!=NULL)
          (*w)[i] = pFDeg(res[syzIndex]->m[i-k-1])
                    +(*w)[pGetComp(res[syzIndex]->m[i-k-1])];
      }
      delete w1;
      w1 = w;
      w = NULL;
    }
  }
  if (w1!=NULL) delete w1;
  if (maxlength!=-1)
  {
    if ((syzIndex<maxlength-1) && (res[syzIndex]!=NULL)) syzIndex++;
    while (syzIndex<maxlength)
    {
      res[syzIndex]=idInit(1,1);
      syzIndex++;
    }
  }
  Kstd1_deg=Kstd1_OldDeg;
  if (!oldDegBound)
    test &= ~Sy_bit(OPT_DEGBOUND);
  return res;
}

void syDetect(ideal id,int index,BOOLEAN homog,int * degrees,int * tocancel)
{
  int i, j, k, ModComp;
  poly p, q, qq, Unit1, Unit2;
  ideal temp;

  if (idIs0(id)) return;
  temp = idInit(IDELEMS(id),id->rank);
  for (i=0;i<IDELEMS(id);i++)
  {
    p = id->m[i];
    while (p!=NULL)
    {
      if (pIsConstantComp(p))
      {
        if (temp->m[i]==NULL)
        {
          temp->m[i] = pHead(p);
          q = temp->m[i];
        }
        else
        {
          pNext(q) = pHead(p);
          pIter(q);
        }
      }
      pIter(p);
    }
  }
  i = IDELEMS(id);
  while ((i>0) && (temp->m[i-1]==NULL)) i--;
  if (i==0)
  {
    idDelete(&temp);
    return;
  }
  j = 0;
  while ((j<i) && (temp->m[j]==NULL)) j++;
  if (j<i)
  {
    p = temp->m[j];
    temp->m[j] = NULL;
  }
  else
  {
    p = NULL;
  }
  while (p!=NULL)
  {
    if (homog)
    {
      k = pFDeg(id->m[j])+degrees[pGetComp(id->m[j])];
      if (k>=index) tocancel[k-index]++;
    }
    else
    {
      tocancel[0]--;
    }
    ModComp = pGetComp(p);
//Print("Element %d: ",j);pWrite(p);
    Unit1 = pTakeOutComp1(&p,ModComp);
    pSetComp(Unit1,0);
    for (k=j+1;k<i;k++)
    {
      if (temp->m[k]!=NULL)
      {
//Print("erst %d: ",k);pWrite(temp->m[k]);
        Unit2 = pTakeOutComp1(&(temp->m[k]),ModComp);
        if (Unit2!=NULL)
        {
          number tmp=nCopy(pGetCoeff(Unit2));
          tmp=nNeg(tmp);
          qq = pCopy(p);
          pMultN(qq,tmp);
          nDelete(&tmp);
          pMultN(temp->m[k],pGetCoeff(Unit1));
          temp->m[k] = pAdd(temp->m[k],qq);
//Print("dann %d: ",k);pWrite(temp->m[k]);
          pDelete(&Unit2);
        }
      }
    }
//PrintLn();
    pDelete(&Unit1);
    pDelete(&p);
    j++;
    while ((j<i) && (temp->m[j]==NULL)) j++;
    if (j>=i)
    {
      idDelete(&temp);
      return;
    }
    p = temp->m[j];
    temp->m[j] = NULL;
  }
  idDelete(&temp);
}

void syDetect(ideal id,int index,int rsmin, BOOLEAN homog,
              intvec * degrees,intvec * tocancel)
{
  int * deg=NULL;
  int * tocan=(int*) Alloc0(tocancel->length()*sizeof(int));
  int i;

  if (homog)
  {
    deg = (int*) Alloc0(degrees->length()*sizeof(int));
    for (i=degrees->length();i>0;i--)
      deg[i-1] = (*degrees)[i-1]-rsmin;
  }
  syDetect(id,index,homog,deg,tocan);
  for (i=tocancel->length();i>0;i--)
    (*tocancel)[i-1] = tocan[i-1];
  if (homog)
    Free((ADDRESS)deg,degrees->length()*sizeof(int));
  Free((ADDRESS)tocan,tocancel->length()*sizeof(int));
}

/*2
* computes the betti numbers from a given resolution
* of length 'length' (0..length-1), not necessairily minimal,
* (if weights are given, they are used)
* returns the int matrix of betti numbers
* and the regularity
*/
intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights)
{
  int i,j=0,k=0,l,rows,cols;
  int *temp1,*temp2,*temp3;/*used to compute degrees*/
  int *tocancel; /*(BOOLEAN)tocancel[i]=element is superfluous*/

  /*------ compute size --------------*/
  *regularity = -1;
  cols = length;
  while ((cols>0)
  && ((res[cols-1]==NULL)
    || (idIs0(res[cols-1]))))
  {
    cols--;
  }
  intvec * result;
  if (idIs0(res[0]))
  {
    if (res[0]==NULL)
      result = new intvec(1,1,1);
    else
      result = new intvec(1,1,res[0]->rank);
    return result;
  }
  l = max(idRankFreeModule(res[0]),res[0]->rank);
  i = 0;
  while ((i<length) && (res[i]!=NULL))
  {
    if (IDELEMS(res[i])>l) l = IDELEMS(res[i]);
    i++;
  }
  temp1 = (int*)Alloc0((l+1)*sizeof(int));
  temp2 = (int*)Alloc((l+1)*sizeof(int));
  rows = 1;
  cols++;
  for (i=0;i<cols-1;i++)
  {
    if ((i==0) && (weights!=NULL)) pSetModDeg(weights);
    memset(temp2,0,(l+1)*sizeof(int));
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
     {
        if ((pGetComp(res[i]->m[j])>l)
        || ((i>1) && (res[i-1]->m[pGetComp(res[i]->m[j])-1]==NULL)))
        {
          WerrorS("input not a resolvent");
          Free((ADDRESS)temp1,(l+1)*sizeof(int));
          Free((ADDRESS)temp2,(l+1)*sizeof(int));
          return NULL;
        }
        temp2[j+1] = pFDeg(res[i]->m[j])+temp1[pGetComp(res[i]->m[j])];
        if (temp2[j+1]-i>rows) rows = temp2[j+1]-i;
      }
    }
    if ((i==0) && (weights!=NULL)) pSetModDeg(NULL);
    temp3 = temp1;
    temp1 = temp2;
    temp2 = temp3;
  }
  /*------ computation betti numbers --------------*/
  result = new intvec(rows,cols,0);
  (*result)[0] = /*idRankFreeModule(res[0])*/ res[0]->rank;
  if ((!idIs0(res[0])) && ((*result)[0]==0)) (*result)[0] = 1;
  tocancel = (int*)Alloc0((rows+1)*sizeof(int));
  memset(temp2,0,l*sizeof(int));
  memset(temp1,0,(l+1)*sizeof(int));
  syDetect(res[0],0,TRUE,temp2,tocancel);
  (*result)[0] -= tocancel[0];
  for (i=0;i<cols-1;i++)
  {
    if ((i==0) && (weights!=NULL)) pSetModDeg(weights);
    memset(temp2,0,l*sizeof(int));
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        temp2[j+1] = pFDeg(res[i]->m[j])+temp1[pGetComp(res[i]->m[j])];
        //(*result)[i+1+(temp2[j+1]-i-1)*cols]++;
        if (temp2[j+1]>i) IMATELEM((*result),temp2[j+1]-i,i+2)++;
      }
    }
  /*------ computation betti numbers, if res not minimal --------------*/
    for (j=0;j<rows-1;j++)
    {
      //(*result)[i+1+j*cols] -= tocancel[j+1];
      IMATELEM((*result),j+1,i+2) -= tocancel[j+1];
    }
    if ((i<length-1) && (res[i+1]!=NULL))
    {
      memset(tocancel,0,(rows+1)*sizeof(int));
      syDetect(res[i+1],i+1,TRUE,temp2,tocancel);
      for (j=0;j<rows;j++)
      {
        //(*result)[i+1+j*cols] -= tocancel[j];
        IMATELEM((*result),j+1,i+2) -= tocancel[j];
      }
    }
    temp3 = temp1;
    temp1 = temp2;
    temp2 = temp3;
    for (j=0;j<rows;j++)
    {
    //  if (((*result)[i+1+j*cols]!=0) && (j>*regularity)) *regularity = j;
      if ((IMATELEM((*result),j+1,i+2)!=0) && (j>*regularity)) *regularity = j;
    }
    if ((i==0) && (weights!=NULL)) pSetModDeg(NULL);
  }
  /*------ clean up --------------*/
  Free((ADDRESS)tocancel,(rows+1)*sizeof(int));
  Free((ADDRESS)temp1,(l+1)*sizeof(int));
  Free((ADDRESS)temp2,(l+1)*sizeof(int));
  j = 0;
  k = 0;
  for (i=0;i<rows*cols;i++)
  {
    if ((*result)[i] != 0)
    {
      j = i/cols;
      if (i>k+j*cols) k = i-j*cols;
    }
  }
  intvec * exactresult=new intvec(j+1,k+1,0);
  for (i=0;i<exactresult->rows();i++)
  {
    for (j=0;j<exactresult->cols();j++)
    {
      IMATELEM(*exactresult,i+1,j+1) = IMATELEM(*result,i+1,j+1);
    }
  }
  delete result;
  return exactresult;
}

/*2
* minbare via syzygies
*/
ideal syMinBase(ideal arg)
{
  intvec ** weights=NULL;
  int leng;
  if (idIs0(arg)) return idInit(1,arg->rank);
  resolvente res=syResolvente(arg,1,&leng,&weights,TRUE);
  ideal result=res[0];
  Free((ADDRESS)res,leng*sizeof(ideal));
  if (weights!=NULL)
  {
    if (*weights!=NULL)
    {
      delete (*weights);
      *weights=NULL;
    }
    if ((leng>=1) && (*(weights+1)!=NULL))
    {
      delete *(weights+1);
      *(weights+1)=NULL;
    }
  }
  idSkipZeroes(result);
  return result;
}

/*2
* computes Betti-numbers from a resolvente of
* (non-)homogeneous objects
* the numbers of entrees !=NULL in res and weights must be equal
* and < length
*/
intvec * syNewBetti(resolvente res, intvec ** weights, int length)
{
  intvec * result,*tocancel;
  int i,j,k,rsmin=0,rsmax=0,rs=0;
  BOOLEAN homog=TRUE;

  if (weights!=NULL)           //---homogeneous Betti numbers
  {
/*--------------computes size of the field----------------------*/
    for (i=1;i<length;i++)
    {
      if (weights[i] != NULL)
      {
        for (j=1;j<(weights[i])->length();j++)
        {
          if ((*(weights[i]))[j]-i<rsmin) rsmin = (*(weights[i]))[j]-i;
          if ((*(weights[i]))[j]-i>rsmax) rsmax = (*(weights[i]))[j]-i;
        }
      }
    }
    i = 0;
    while (weights[i] != NULL) i++;
    i--;
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        k = pFDeg(res[i]->m[j])+(*(weights[i]))[pGetComp(res[i]->m[j])]-i-1;
        if (k>rsmax) rsmax = k;
        if (k<rsmin) rsmin = k;
      }
    }
    for (j=1;j<(weights[0])->length();j++)
    {
      if ((*weights[0])[j]>rsmax) rsmax = (*weights[0])[j];
      if ((*weights[0])[j]<rsmin) rsmin = (*weights[0])[j];
    }
//Print("rsmax = %d\n",rsmax);
//Print("rsmin = %d\n",rsmin);
    rs = rsmax-rsmin+1;
    result = new intvec(rs,i+2,0);
    tocancel = new intvec(rs);
/*-----------enter the Betti numbers-------------------------------*/
    if (/*idRankFreeModule(res[0])*/ res[0]->rank==0)
    {
      IMATELEM(*result,1-rsmin,1)=1;
    }
    else
    {
      for (i=1;i<(weights[0])->length();i++)
        IMATELEM(*result,(*weights[0])[i]+1-rsmin,1)++;
    }
    i = 1;
    while (weights[i]!=NULL)
    {
      for (j=1;j<(weights[i])->length();j++)
      {
        IMATELEM(*result,(*(weights[i]))[j]-i+1-rsmin,i+1)++;
      }
      i++;
    }
    i--;
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        k = pFDeg(res[i]->m[j])+(*(weights[i]))[pGetComp(res[i]->m[j])]-i;
        IMATELEM(*result,k-rsmin,i+2)++;
      }
    }
  }
  else                //-----the non-homgeneous case
  {
    homog = FALSE;
    tocancel = new intvec(1);
    k = length;
    while ((k>0) && (idIs0(res[k-1]))) k--;
    result = new intvec (1,k+1,0);
    (*result)[0] = res[0]->rank;
    for (i=0;i<length;i++)
    {
      if (res[i]!=NULL)
      {
        for (j=0;j<IDELEMS(res[i]);j++)
        {
          if (res[i]->m[j]!=NULL) (*result)[i+1]++;
        }
      }
    }
  }
/*--------computes the Betti numbers for the minimized reolvente----*/

  i = 1;
  while ((res[i]!=NULL) && (weights[i]!=NULL))
  {
    syDetect(res[i],i,rsmin,homog,weights[i],tocancel);
    if (homog)
    {
      for (j=0;j<rs-1;j++)
      {
        IMATELEM((*result),j+1,i+1) -= (*tocancel)[j];
        IMATELEM((*result),j+1,i+2) -= (*tocancel)[j+1];
      }
      IMATELEM((*result),rs,i+1) -= (*tocancel)[rs-1];
    }
    else
    {
      (*result)[i+1] -= (*tocancel)[0];
      (*result)[i+2] -= (*tocancel)[0];
    }
    i++;
  }

/*--------print Betti numbers for control---------------------------*/
  for(i=rsmin;i<=rsmax;i++)
  {
    Print("%2d:",i);
    for(j=1;j<=result->cols();j++)
    {
      Print(" %5d",IMATELEM(*result,i-rsmin+1,j));
    }
    PrintLn();
  }
  return result;
}

/*2
* is looking for the minimal minimized module of a resolvente
* i.e. returns 0 if res comes from a mres-command and 1 in the
* case of res-commands
*/
int syIsMinimizedFrom(resolvente res,int length)
{
  poly p;
  int i,j=length;

  while ((j>0) && (res[j-1]==NULL)) j--;
  while (j>0)
  {
    for (i=0;i<IDELEMS(res[j-1]);i++)
    {
      p = res[j-1]->m[i];
      while (p!=NULL)
      {
        if (pIsConstantComp(p)) return j;
        p = pNext(p);
      }
    }
    j--;
  }
  return j;
}

/*2
* determines the generators of a minimal resolution
* contained in res
*/
static int ** syScanRes(resolvente res, int length)
{
  int i=0,j,k;
  int **result=(int**)Alloc0(length*sizeof(int*));
  poly p;
  ideal tid;

  while ((i<length) && (!idIs0(res[i])))
  {
    result[i] = (int*)Alloc0(IDELEMS(res[i])*sizeof(int));
    for (j=IDELEMS(res[i])-1;j>=0;j--)
    {
      if (res[i]->m[j]==NULL) (result[i])[j] = -1;
    }
    if (i>0)
    {
      tid = idJet(res[i],0);
      Print("Der %d-te 0-jet ist:\n",i);
      for (j=0;j<IDELEMS(tid);j++)
      {
        if (tid->m[j]!=0)
        {
          Print("poly %d :",j);pWrite(tid->m[j]);
        }
      }
      for (j=0;j<IDELEMS(tid);j++)
      {
        p = tid->m[j];
        while (p!=NULL)
        {
          pNorm(p);
          k = (result[i-1])[pGetComp(p)-1];
          if ((k==0) || (k==-2))
          {
            (result[i-1])[pGetComp(p)-1] = j+1;
            (result[i])[j] = -2;
            break;
          }
          else if (k>0)
          {
            p = pSub(p,pCopy(tid->m[k-1]));
          }
          else if (k==-1)
          {
            Print("Something is rotten in the state of Denmark\n");
          }
        }
        tid->m[j] = p;
      }
      Print("Der %d-te 0-jet ist:\n",i);
      for (j=0;j<IDELEMS(tid);j++)
      {
        if (tid->m[j]!=0)
        {
          Print("poly %d :",j);pWrite(tid->m[j]);
        }
      }
      idDelete(&tid);
    }
    i++;
  }
  Print("Die gescannte Struktur ist:\n");
  for (i=0;i<length;i++)
  {
    Print("Fuer den %d-ten Module:\n",i);
    if (!idIs0(res[i]))
      for (j=0;j<IDELEMS(res[i]);j++) Print(" %d",(result[i])[j]);
    Print("\n");
  }
  return result;
}

static void syReduce(ideal toRed,poly redWith,int redComp)
{
  int i;
  poly p=redWith,pp,ppp;
  number n;

  while ((p!=NULL) && (pGetComp(p)!=redComp)) pIter(p);
  if (p==NULL)
  {
    Print("Hier ist was faul!\n");
  }
  else
  {
    n = nCopy(pGetCoeff(p));
  }
  p = redWith;
  for (i=0;i<IDELEMS(toRed);i++)
  {
    pp = toRed->m[i];
    while ((pp!=NULL) && (pGetComp(pp)!=redComp)) pIter(pp);
    if (pp!=NULL)
    {
      ppp = pMultCopyN(p,pGetCoeff(pp));
      pMultN(toRed->m[i],n);
      toRed->m[i] = pSub(toRed->m[i],ppp);
    }
  }
  nDelete(&n);
}

resolvente syFastMin(resolvente res,int length)
{
  int **res_shape=syScanRes(res,length);
  int i,j,k;
  poly p;
  resolvente result=(ideal*)Alloc0(length*sizeof(ideal));
  
  for (i=0;(i<length) && !idIs0(res[i]);i++)
  {
    k = 0;
    result[i] = idInit(IDELEMS(res[i]),res[i]->rank);
    for (j=IDELEMS(res[i])-1; j>=0;j--)
    {
      if ((res_shape[i])[j]==0)
      {
        result[i]->m[k] = pCopy(res[i]->m[j]);
        k++;
      }
    }
    if (i>0)
    {
      for (j=IDELEMS(res[i-1])-1;j>=0;j--)
      {
        if ((res_shape[i-1])[j]>0)
        {
          k = (res_shape[i-1])[j]-1;
          syReduce(result[i],res[i]->m[k],j+1);
        }
      }
    }
  }
  return result;
}
