/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/

#include <kernel/mod2.h>
#include <kernel/options.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/numbers.h>
#include <kernel/ring.h>
#include <kernel/ideals.h>
#include <kernel/matpol.h>
#include <omalloc.h>
#include <kernel/kstd1.h>
#include <kernel/longalg.h>
#include <kernel/maps.h>
#include <kernel/prCopy.h>

#ifdef HAVE_PLURAL
#include <kernel/gring.h>
#endif

// This is a very dirty way to "normalize" numbers w.r.t. a
// MinPoly

/* debug output: Tok2Cmdname in maApplyFetch*/
//#include "ipshell.h"

#define MAX_MAP_DEG 128

/*2
* copy a map
*/
map maCopy(map theMap)
{
  int i;
  map m=(map)idInit(IDELEMS(theMap),0);
  for (i=IDELEMS(theMap)-1; i>=0; i--)
      m->m[i] = pCopy(theMap->m[i]);
  m->preimage=omStrDup(theMap->preimage);
  return m;
}


/*2
* return the image of var(v)^pExp, where var(v) maps to p
*/
poly maEvalVariable(poly p, int v,int pExp,matrix s)
{
  if (pExp==1)
    return pCopy(p);

  poly res;

  if((s!=NULL)&&(pExp<MAX_MAP_DEG))
  {
    int j=2;
    poly p0=p;
    // find starting point
    if(MATELEM(s,v,1)==NULL)
    {
      MATELEM(s,v,1)=pCopy(p/*theMap->m[v-1]*/);
    }
    else
    {
      while((j<=pExp)&&(MATELEM(s,v,j)!=NULL))
      {
        j++;
      }
      p0=MATELEM(s,v,j-1);
    }
    // multiply
    for(;j<=pExp;j++)
    {
      p0=MATELEM(s,v,j)=ppMult_qq(p0, p);
      pNormalize(p0);
    }
    res=pCopy(p0/*MATELEM(s,v,pExp)*/);
  }
  else //if ((p->next!=NULL)&&(p->next->next==NULL))
  {
    res=pPower(pCopy(p),pExp);
  }
  return res;
}

static poly maEvalMonom(map theMap, poly p,ring preimage_r,matrix s, nMapFunc nMap)
{
    poly q=pNSet(nMap(pGetCoeff(p)));

    int i;
    for(i=1;i<=preimage_r->N; i++)
    {
      int pExp=p_GetExp( p,i,preimage_r);
      if (pExp != 0)
      {
        if (theMap->m[i-1]!=NULL)
        {
          poly p1=theMap->m[i-1];
          poly pp=maEvalVariable(p1,i,pExp,s);
          q = pMult(q,pp);
        }
        else
        {
          pDelete(&q);
          break;
        }
      }
    }
    int modulComp = p_GetComp( p,preimage_r);
    if (q!=NULL) pSetCompP(q,modulComp);
  return q;
}

poly maEval(map theMap, poly p,ring preimage_r,nMapFunc nMap,matrix s)
{
  poly result = NULL;
  int i;

//  for(i=1; i<=preimage_r->N; i++)
//  {
//    pTest(theMap->m[i-1]);
//  }
//  while (p!=NULL)
//  {
//    poly q=maEvalMonom(theMap,p,preimage_r,s);
//    result = pAdd(result,q);
//    pIter(p);
//  }
  if (p!=NULL)
  {
    int l = pLength(p)-1;
    poly* monoms;
    if (l>0)
    {
      monoms = (poly*) omAlloc(l*sizeof(poly));

      for (i=0; i<l; i++)
      {
        monoms[i]=maEvalMonom(theMap,p,preimage_r,s, nMap);
        pIter(p);
      }
    }
    result=maEvalMonom(theMap,p,preimage_r,s, nMap);
    if (l>0)
    {
      for(i = l-1; i>=0; i--)
      {
        result=pAdd(result, monoms[i]);
      }
      omFreeSize((ADDRESS)monoms,l*sizeof(poly));
    }
    if (currRing->minpoly!=NULL) result=pMinPolyNormalize(result);
  }
  pTest(result);
  return result;
}

/*2
*shifts the variables between minvar and maxvar of p  \in p_ring to the
*first maxvar-minvar+1 variables in the actual ring
*be carefull: there is no range check for the variables of p
*/
static poly pChangeSizeOfPoly(ring p_ring, poly p,int minvar,int maxvar)
{
  int i;
  poly result = NULL,resultWorkP;
  number n;

  if (p==NULL) return result;
  else result = pInit();
  resultWorkP = result;
  while (p!=NULL)
  {
    for (i=minvar;i<=maxvar;i++)
      pSetExp(resultWorkP,i-minvar+1,p_GetExp(p,i,p_ring));
    pSetComp(resultWorkP,p_GetComp(p,p_ring));
    n=nCopy(pGetCoeff(p));
    pSetCoeff(resultWorkP,n);
    pSetm(resultWorkP);
    pIter(p);
    if (p!=NULL)
    {
      pNext(resultWorkP) = pInit();
      pIter(resultWorkP);
    }
  }
  return result;
}


/*2
*returns the preimage of id under theMap,
*if id is empty or zero the kernel is computed
* (assumes) that both ring have the same coeff.field
*/
ideal maGetPreimage(ring theImageRing, map theMap, ideal id)
{
  ring sourcering = currRing;

#ifdef HAVE_PLURAL
  if (rIsPluralRing(theImageRing))
  {
    if ((rIsPluralRing(sourcering)) && (ncRingType(sourcering)!=nc_comm)) 
    {
      Werror("Sorry, not yet implemented for noncomm. rings");
      return NULL;
    }
  }
#endif
  
  int i,j;
  poly p,pp,q;
  ideal temp1;
  ideal temp2;

  int imagepvariables = theImageRing->N;
  int N = pVariables+imagepvariables;

  ring tmpR;
  if (rSumInternal(theImageRing,sourcering,tmpR,FALSE,TRUE)!=1)
  {
     WerrorS("error in rSumInternal");
     return NULL;
  }

  if (nSetMap(theImageRing) != nCopy)
  {
    Werror("Coefficient fields must be equal");
    return NULL;
  }

  // change to new ring
  rChangeCurrRing(tmpR);
  if (id==NULL)
    j = 0;
  else
    j = IDELEMS(id);
  int j0=j;
  if (theImageRing->qideal!=NULL) j+=IDELEMS(theImageRing->qideal);
  temp1 = idInit(sourcering->N+j,1);
  for (i=0;i<sourcering->N;i++)
  {
    q = pISet(-1);
    pSetExp(q,i+1+imagepvariables,1);
    pSetm(q);
    if ((i<IDELEMS(theMap)) && (theMap->m[i]!=NULL))
    {
      p = pSort(pChangeSizeOfPoly(theImageRing,theMap->m[i],1,imagepvariables));
      p=pAdd(p,q);
    }
    else
    {
      p = q;
    }
    temp1->m[i] = p;
  }
  idTest(temp1);
  for (i=sourcering->N;i<sourcering->N+j0;i++)
  {
    temp1->m[i] = pSort(pChangeSizeOfPoly(theImageRing,
                                    id->m[i-sourcering->N],1,imagepvariables));
  }
  for (i=sourcering->N+j0;i<sourcering->N+j;i++)
  {
    temp1->m[i] = pSort(pChangeSizeOfPoly(theImageRing,
                                    theImageRing->qideal->m[i-sourcering->N-j0],
                                    1,imagepvariables));
  }
  // we ignore here homogenity - may be changed later:
  temp2 = kStd(temp1,NULL,isNotHomog,NULL);
  idDelete(&temp1);
  for (i=0;i<IDELEMS(temp2);i++)
  {
    if (pLowVar(temp2->m[i])<imagepvariables) pDelete(&(temp2->m[i]));
  }

  // let's get back to the original ring
  rChangeCurrRing(sourcering);
  temp1 = idInit(5,1);
  j = 0;
  for (i=0;i<IDELEMS(temp2);i++)
  {
    p = temp2->m[i];
    if (p!=NULL)
    {
      q = pSort(pChangeSizeOfPoly(tmpR, p,imagepvariables+1,N));
      if (j>=IDELEMS(temp1))
      {
        pEnlargeSet(&(temp1->m),IDELEMS(temp1),5);
        IDELEMS(temp1)+=5;
      }
      temp1->m[j] = q;
      j++;
    }
  }
  id_Delete(&temp2, tmpR);
  idSkipZeroes(temp1);
  rKill(tmpR);
  return temp1;
}

void maFindPerm(char **preim_names, int preim_n, char **preim_par, int preim_p,
                char **names,       int n,       char **par,       int nop,
                int * perm, int *par_perm, int ch)
{
  int i,j;
  /* find correspondig vars */
  for (i=0; i<preim_n; i++)
  {
    for(j=0; j<n; j++)
    {
      if (strcmp(preim_names[i],names[j])==0)
      {
        if (BVERBOSE(V_IMAP))
          Print("// var %s: nr %d -> nr %d\n",preim_names[i],i+1,j+1);
        /* var i+1 from preimage ring is var j+1  (index j+1) from image ring */
        perm[i+1]=j+1;
        break;
      }
    }
    if ((perm[i+1]==0)&&(par!=NULL)
        // do not consider par of Fq
         && (ch < 2))
    {
      for(j=0; j<nop; j++)
      {
        if (strcmp(preim_names[i],par[j])==0)
        {
          if (BVERBOSE(V_IMAP))
            Print("// var %s: nr %d -> par %d\n",preim_names[i],i+1,j+1);
          /* var i+1 from preimage ring is par j+1 (index j) from image ring */
          perm[i+1]=-(j+1);
        }
      }
    }
  }
  if (par_perm!=NULL)
  {
    for (i=0; i<preim_p; i++)
    {
      for(j=0; j<n; j++)
      {
        if (strcmp(preim_par[i],names[j])==0)
        {
          if (BVERBOSE(V_IMAP))
            Print("// par %s: par %d -> nr %d\n",preim_par[i],i+1,j+1);
          /*par i+1 from preimage ring is var j+1  (index j+1) from image ring*/
          par_perm[i]=j+1;
          break;
        }
      }
      if ((par!=NULL) && (par_perm[i]==0))
      {
        for(j=0; j<nop; j++)
        {
          if (strcmp(preim_par[i],par[j])==0)
          {
            if (BVERBOSE(V_IMAP))
              Print("// par %s: nr %d -> par %d\n",preim_par[i],i+1,j+1);
            /*par i+1 from preimage ring is par j+1 (index j) from image ring */
            par_perm[i]=-(j+1);
          }
        }
      }
    }
  }
}

/*2
* embeds poly p from the subring r into the current ring
*/
poly maIMap(ring r, poly p)
{
  /* the simplest case:*/
  if(r==currRing) return pCopy(p);
  nMapFunc nMap=nSetMap(r);
  int *perm=(int *)omAlloc0((r->N+1)*sizeof(int));
  //int *par_perm=(int *)omAlloc0(rPar(r)*sizeof(int));
  maFindPerm(r->names,r->N, r->parameter, r->P,
             currRing->names,currRing->N,currRing->parameter, currRing->P,
             perm,NULL, currRing->ch);
  poly res=pPermPoly(p,perm,r, nMap /*,par_perm,rPar(r)*/);
  omFreeSize((ADDRESS)perm,(r->N+1)*sizeof(int));
  //omFreeSize((ADDRESS)par_perm,rPar(r)*sizeof(int));
  return res;
}

/*3
* find the max. degree in one variable, but not larger than MAX_MAP_DEG
*/
int maMaxDeg_Ma(ideal a,ring preimage_r)
{
  int i,j;
  int N = preimage_r->N;
  poly p;
  int *m=(int *)omAlloc0(N*sizeof(int));

  for (i=MATROWS(a)*MATCOLS(a)-1;i>=0;i--)
  {
    p=a->m[i];
    //pTest(p); // cannot test p because it is from another ring
    while(p!=NULL)
    {
      for(j=N-1;j>=0;j--)
      {
        m[j]=si_max(m[j],(int)p_GetExp( p,j+1,preimage_r));
        if (m[j]>=MAX_MAP_DEG)
        {
          i=MAX_MAP_DEG;
          goto max_deg_fertig_id;
        }
      }
      pIter(p);
    }
  }
  i=m[0];
  for(j=N-1;j>0;j--)
  {
    i=si_max(i,m[j]);
  }
max_deg_fertig_id:
  omFreeSize((ADDRESS)m,N*sizeof(int));
  return i;
}

/*3
* find the max. degree in one variable, but not larger than MAX_MAP_DEG
*/
int maMaxDeg_P(poly p,ring preimage_r)
{
  int i,j;
  int N = preimage_r->N;
  int *m=(int *)omAlloc0(N*sizeof(int));

//  pTest(p);
  while(p!=NULL)
  {
    for(j=N-1;j>=0;j--)
    {
      m[j]=si_max(m[j],(int)p_GetExp(p,j+1,preimage_r));
      if (m[j]>=MAX_MAP_DEG)
      {
        i=MAX_MAP_DEG;
        goto max_deg_fertig_p;
      }
    }
    pIter(p);
  }
  i=m[0];
  for(j=N-1;j>0;j--)
  {
    i=si_max(i,m[j]);
  }
max_deg_fertig_p:
  omFreeSize((ADDRESS)m,N*sizeof(int));
  return i;
}

// This is a very dirty way to cancel monoms whose number equals the
// MinPoly
poly pMinPolyNormalize(poly p)
{
  number one = nInit(1);
  spolyrec rp;

  poly q = &rp;

  while (p != NULL)
  {
    // this returns 0, if p == MinPoly
    number product = nMult(pGetCoeff(p), one);
    if (product == NULL)
    {
      pLmDelete(&p);
    }
    else
    {
      pSetCoeff(p, product);
      pNext(q) = p;
      q = p;
      p = pNext(p);
    }
  }
  pNext(q) = NULL;
  return rp.next;
}
