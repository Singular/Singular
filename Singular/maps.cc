/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/maps.cc,v 1.2 1997-03-20 16:59:39 obachman Exp $";
/* $Log: not supported by cvs2svn $
*/
/*
* ABSTRACT - the mapping of polynomials to other rings
*/

#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "polys.h"
#include "numbers.h"
#include "ipid.h"
#include "ring.h"
#include "ideals.h"
#include "matpol.h"
#include "mmemory.h"
#include "kstd1.h"
#include "lists.h"
#include "longalg.h"
#include "maps.h"

#define MAX_MAP_DEG 32

/*2
* copy a map
*/
map maCopy(map theMap)
{
  int i;
  map m=(map)idInit(IDELEMS(theMap),0);
  for (i=IDELEMS(theMap)-1; i>=0; i--)
      m->m[i] = pCopy(theMap->m[i]);
  m->preimage=mstrdup(theMap->preimage);
  return m;
}

poly maEval(map theMap, poly p,int varnum,matrix s)
{
  poly result = NULL,q,pp;
  int i,modulComp;

//  for(i=1; i<=varnum; i++)
//  {
//    pTest(theMap->m[i-1]);
//  }
  while (p!=NULL)
  {
    q=pOne();
    pSetCoeff(q,nMap(pGetCoeff(p)));

    for(i=1; i<=varnum; i++)
    {
      if (pGetExp(p,i)!=0)
      {
        if (theMap->m[i-1]!=NULL)
        {
          poly p1=theMap->m[i-1];
          int pExp=pGetExp(p,i);
          if((s!=NULL)&&(pExp<(MAX_MAP_DEG*2)))
          {
            int e=min(pExp,MAX_MAP_DEG);
            if(MATELEM(s,i,e)!=NULL)
              pp=pCopy(MATELEM(s,i,e));
            else
            {
              int j=2;
              poly p0=p1;
              if(MATELEM(s,i,1)==NULL)
              {
                MATELEM(s,i,1)=pCopy(p1/*theMap->m[i-1]*/);
              }
              else
              {
                while((j<=e)&&(MATELEM(s,i,j)!=NULL))
                {
                  j++;
                }
                p0=MATELEM(s,i,j-1);
              }
              for(;j<=e;j++)
              {
                p0=MATELEM(s,i,j)=pMult(pCopy(p0/*MATELEM(s,i,j-1)*/),
                                        pCopy(p1/*theMap->m[i-1]*/));
              }
              pp=pCopy(p0/*MATELEM(s,i,e)*/);
            }
            while(e!=pExp/*pGetExp(p,i)*/)
            {
              // e is the current exponent,
              // MAX_MAP_DEG the max exponent in the table
              // pExp the end exponent
              pp=pMult(pp,pCopy(MATELEM(s,i,min(MAX_MAP_DEG,pExp-e))));
              e+=min(MAX_MAP_DEG,pExp-e);
            }
          }
          else
          {
            pp = pPower(pCopy(p1/*theMap->m[i-1]*/),pExp/*pGetExp(p,i)*/);
          }
          q = pMult(q,pp);
        }
        else
        {
          pDelete(&q);
          break;
        }
      }
    }
    modulComp = pGetComp(p);
    if (q!=NULL) pSetCompP(q,modulComp);
    result = pAdd(result,q);
    pIter(p);
  }
  if (currRing->minpoly!=NULL) result=pMult(result,pOne());
  pTest(result);
  return result;
}

/*2
*shifts the variables between minvar and maxvar of p to the
*first maxvar-minvar+1 variables in the actual ring
*be carefull: there is no range check for the variables of p
*/
static poly pChangeSizeOfPoly(poly p,int minvar,int maxvar)
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
      pSetExp(resultWorkP,i-minvar+1,pGetExp(p,i));
    pSetComp(resultWorkP,pGetComp(p));
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
*/
ideal maGetPreimage(ring theImageRing, map theMap, ideal id)
{
  int i,j,sizeofpoly;
  int ordersize;
  poly p,pp,q;
  ideal temp1;
  ideal temp2;
  short  ** wv;
#define blockmax 5
  int orders[blockmax],block0[blockmax], block1[blockmax];
  int imagepvariables = theImageRing->N;
  ring sourcering = currRing;
  int N = pVariables+imagepvariables;

//  pChangeRing(theImageRing->N,theImageRing->OrdSgn,theImageRing->order,theImageRing->blocks,theImageRing->wvhdl);
  memset(block0, 0,sizeof(block0));
  memset(block1, 0,sizeof(block1));
  memset(orders, 0,sizeof(orders));
  if (theImageRing->OrdSgn == 1) orders[0] = ringorder_dp;
  else orders[0] = ringorder_ls;
  block1[0] = imagepvariables;
  block0[0] = 1;
  /*
  *if (sourcering->order[blockmax])
  *{
  *  if (sourcering->OrdSgn == 1) orders[1] = ringorder_dp;
  *  else orders[1] = ringorder_ls;
  *  block1[1] = N;
  *}
  *else
  */
  {
    i=0;
    while(sourcering->order[i])
    {
      orders[i+1] = sourcering->order[i];
      block0[i+1] = sourcering->block0[i]+imagepvariables;
      block1[i+1] = sourcering->block1[i]+imagepvariables;
      i++;
    }
  }
  ordersize=i+1;
  wv = (short **) Alloc(ordersize * sizeof(short **));
  memset(wv,0,ordersize * sizeof(short **));
  for (i--;i!=0 ;i--) wv[i+1] = sourcering->wvhdl[i];
  pChangeRing(N,currRing->OrdSgn,orders,block0,block1, wv);
  sizeofpoly = pMonomSize; /*POLYSIZE+sizeof(monomial);*/
  if (id==NULL)
    j = 0;
  else
    j = IDELEMS(id);
  temp1 = idInit(sourcering->N+j,1);
  for (i=0;i<sourcering->N;i++)
  {
    if ((i<IDELEMS(theMap)) && (theMap->m[i]!=NULL))
    {
      p = pChangeSizeOfPoly(theMap->m[i],1,imagepvariables);
      q = p;
      while (pNext(q)) pIter(q);
      pNext(q) = pOne();
      pIter(q);
    }
    else
      q = p = pOne();
    pGetCoeff(q)=nNeg(pGetCoeff(q));
    pSetExp(q,i+1+imagepvariables,1);
    pSetm(q);
    temp1->m[i] = p;
  }
  for (i=sourcering->N;i<sourcering->N+j;i++)
  {
    temp1->m[i] = pChangeSizeOfPoly(id->m[i-sourcering->N],1,imagepvariables);
  }
  // we ignore here homogenity - may be changed later:
  temp2 = std(temp1,NULL,isNotHomog,NULL);
  idDelete(&temp1);
  for (i=0;i<IDELEMS(temp2);i++)
  {
    if (pLowVar(temp2->m[i])<imagepvariables) pDelete(&(temp2->m[i]));
  }
  pChangeRing(sourcering->N,sourcering->OrdSgn,sourcering->order,
              sourcering->block0,sourcering->block1,sourcering->wvhdl);
  temp1 = idInit(5,1);
  j = 0;
  for (i=0;i<IDELEMS(temp2);i++)
  {
    p = temp2->m[i];
    temp2->m[i]=NULL;
    if (p!=NULL)
    {
      q = pChangeSizeOfPoly(p,imagepvariables+1,N);
      if (j>=IDELEMS(temp1))
      {
        pEnlargeSet(&(temp1->m),IDELEMS(temp1),5);
        IDELEMS(temp1)+=5;
      }
      temp1->m[j] = q;
      j++;
      while (p!=NULL)
      {
        pp = pNext(p);
        Free((ADDRESS)p,sizeofpoly);
        p = pp;
      }
    }
  }
  idDelete(&temp2);
  Free((ADDRESS) wv, ordersize*sizeof(short **));
  idSkipZeroes(temp1);
  return temp1;
}

void maFindPerm(char **preim_names, int preim_n, char **preim_par,
                char **names, int n, char **par, int nop,
                int * perm, int *par_perm)
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
    if ((perm[i+1]==0)&&(par!=NULL))
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
    for (i=0; preim_par[i]!=NULL; i++)
    {
      for(j=0; j<n; j++)
      {
        if (strcmp(preim_par[i],names[j])==0)
        {
          if (BVERBOSE(V_IMAP))
            Print("// par %s: par %d -> nr %d\n",preim_par[i],i+1,j+1);
          /* var i+1 from preimage ring is var j+1  (index j+1) from image ring */
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
            /* var i+1 from preimage ring is par j+1 (index j) from image ring */
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
  nSetMap(r->ch,r->parameter,r->P,r->minpoly);
  int *perm=(int *)Alloc0((r->N+1)*sizeof(int));
  //int *par_perm=(int *)Alloc0(rPar(r)*sizeof(int));
  maFindPerm(r->names,r->N, r->parameter,
             currRing->names,currRing->N,currRing->parameter, currRing->P,
             perm,NULL/*par_perm*/);
  poly res=pPermPoly(p,perm,r->N/*,par_perm,rPar(r)*/);
  Free((ADDRESS)perm,(r->N+1)*sizeof(int));
  //Free((ADDRESS)par_perm,rPar(r)*sizeof(int));
  return res;
}

/*3
* find the max. degree in one variable, but not larger than MAX_MAP_DEG
*/
static int maMaxDeg_Ma(ideal a,int N)
{
  int i,j;
  poly p;
  int *m=(int *)Alloc0(N*sizeof(int));

  for (i=MATROWS(a)*MATCOLS(a)-1;i>=0;i--)
  {
    p=a->m[i];
    //pTest(p); // cannot test p because it is from another ring
    while(p!=NULL)
    {
      for(j=N-1;j>=0;j--)
      {
        m[j]=max(m[j],pGetExp(p,j+1));
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
    i=max(i,m[j]);
  }
max_deg_fertig_id:
  Free((ADDRESS)m,N*sizeof(int));
  return i;
}

/*3
* find the max. degree in one variable, but not larger than MAX_MAP_DEG
*/
static int maMaxDeg_P(poly p,int N)
{
  int i,j;
  int *m=(int *)Alloc0(N*sizeof(int));

  pTest(p);
  while(p!=NULL)
  {
    for(j=N-1;j>=0;j--)
    {
      m[j]=max(m[j],pGetExp(p,j+1));
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
    i=max(i,m[j]);
  }
max_deg_fertig_p:
  Free((ADDRESS)m,N*sizeof(int));
  return i;
}

/*2
* maps the expression w to res,
* switch what: MAP_CMD: use theMap for mapping, N for preimage ring
*              FETCH_CMD: use pOrdPoly for mapping
*              IMAP_CMD: use perm for mapping, N for preimage ring
*              default: map only poly-structures,
*                       use perm and par_perm, N and P,
*/
BOOLEAN maApplyFetch(int what,map theMap,leftv res, leftv w,
                     int *perm,int N,
                     int *par_perm, int P)
{
  int i;
  //Print("N=%d ",N);
  //if (perm!=NULL) for(i=1;i<=N;i++) Print("%d -> %d ",i,perm[i]);
  //PrintS("\n");
  //Print("P=%d ",P);
  //if (par_perm!=NULL) for(i=0;i<P;i++) Print("%d -> %d ",i,par_perm[i]);
  //PrintS("\n");
  void *data=w->Data();
  res->rtyp = w->rtyp;
  switch (w->rtyp)
  {
    case NUMBER_CMD:
      if (P!=0)
      {
        res->data=(void *)naPermNumber((number)data,par_perm,P);
        res->rtyp=POLY_CMD;
        if (currRing->minpoly!=NULL)
          res->data=(void *)pMult((poly)res->data,pOne());
      }
      else
      {
        res->data=(void *)nMap((number)data);
        if (currRing->minpoly!=NULL)
        {
          number a=(number)res->data;
          number b=nInit(1);
          number c=nMult(a,b);
          nDelete(&a);
          nDelete(&b);
          res->data=(void *)c;
        }
      }
      break;
    case POLY_CMD:
    case VECTOR_CMD:
      if (what==FETCH_CMD)
        res->data=(void *)pOrdPoly(pCopy((poly)data));
      else if (what==IMAP_CMD)
        res->data=(void *)pPermPoly((poly)data,perm,N,par_perm,P);
      else /*if (what==MAP_CMD)*/
      {
        matrix s=mpNew(N,maMaxDeg_P((poly)data,N));
        res->data=(void *)maEval(theMap,(poly)data,N,s);
        idDelete((ideal *)&s);
      }
      if (currRing->minpoly!=NULL)
        res->data=(void *)pMult((poly)res->data,pOne());
      pTest((poly)res->data);
      break;
    case MODUL_CMD:
    case MATRIX_CMD:
    case IDEAL_CMD:
    case MAP_CMD:
    {
      int C=((matrix)data)->cols();
      int R;
      if (w->rtyp==MAP_CMD) R=1;
      else R=((matrix)data)->rows();
      matrix m=mpNew(R,C);
      char *tmpR=NULL;
      if(w->rtyp==MAP_CMD)
      {
        tmpR=((map)data)->preimage;
        ((matrix)data)->rank=((matrix)data)->rows();
      }
      if (what==FETCH_CMD)
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=pOrdPoly(pCopy(((ideal)data)->m[i]));
          pTest(m->m[i]);
        }
      }
      else if (what==IMAP_CMD)
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=pPermPoly(((ideal)data)->m[i],perm,N,par_perm,P);
          pTest(m->m[i]);
        }
      }
      else /* if(what==MAP_CMD) */
      {
        matrix s=mpNew(N,maMaxDeg_Ma((ideal)data,N));
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=maEval(theMap,((ideal)data)->m[i],N,s);
          pTest(m->m[i]);
        }
        idDelete((ideal *)&s);
      }
      if (currRing->minpoly!=NULL)
      {
        for (i=R*C-1;i>=0;i--)
        {
          m->m[i]=pMult(m->m[i],pOne());
          pTest(m->m[i]);
        }
      }
      if(w->rtyp==MAP_CMD)
      {
        ((map)data)->preimage=tmpR;
        ((map)m)->preimage=mstrdup(tmpR);
      }
      else
      {
        m->rank=((matrix)data)->rank;
      }
      res->data=(char *)m;
      break;
    }
    case LIST_CMD:
    {
      lists l=(lists)data;
      lists ml=(lists)Alloc(sizeof(slists));
      ml->Init(l->nr+1);
      for(i=0;i<=l->nr;i++)
      {
        if (((l->m[i].rtyp>BEGIN_RING)&&(l->m[i].rtyp<END_RING))
        ||(l->m[i].rtyp==LIST_CMD))
        {
          if (maApplyFetch(what,theMap,&ml->m[i],&l->m[i],perm,N,par_perm,P))
          {
            ml->Clean();
            Free((ADDRESS)ml,sizeof(slists));
            res->rtyp=0;
            return TRUE;
          }
        }
        else
        {
          ml->m[i].Copy(&l->m[i]);
        }
      }
      res->data=(char *)ml;
      break;
    }
    default:
    {
      return TRUE;
    }
  }
  return FALSE;
}

