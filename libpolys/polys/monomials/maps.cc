/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - the mapping of polynomials to other rings
*/

#include <misc/options.h>
#include <monomials/p_polys.h>
#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>
#include <monomials/ring.h>
#include <simpleideals.h>
#include <omalloc/omalloc.h>
#include <polys/prCopy.h>
#include <polys/ext_fields/longtrans.h>
#include <polys/monomials/maps.h>

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
map maCopy(map theMap, const ring r)
{
  int i;
  map m=(map)idInit(IDELEMS(theMap),0);
  for (i=IDELEMS(theMap)-1; i>=0; i--)
      m->m[i] = p_Copy(theMap->m[i],r);
  m->preimage=omStrDup(theMap->preimage);
  return m;
}


/*2
* return the image of var(v)^pExp, where var(v) maps to p
*/
poly maEvalVariable(poly p, int v,int pExp, ideal s, const ring dst_r)
{
  if (pExp==1)
    return p_Copy(p,dst_r);

  poly res;

  if((s!=NULL)&&(pExp<MAX_MAP_DEG))
  {
    int j=2;
    poly p0=p;
    // find starting point
    if(MATELEM(s,v,1)==NULL)
    {
      MATELEM(s,v,1)=p_Copy(p/*theMap->m[v-1]*/,dst_r);
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
      p0=MATELEM(s,v,j)=pp_Mult_qq(p0, p,dst_r);
      p_Normalize(p0, dst_r);
    }
    res=p_Copy(p0/*MATELEM(s,v,pExp)*/,dst_r);
  }
  else //if ((p->next!=NULL)&&(p->next->next==NULL))
  {
    res=p_Power(p_Copy(p,dst_r),pExp,dst_r);
  }
  return res;
}

static poly maEvalMonom(map theMap, poly p,ring preimage_r, ideal s, 
           nMapFunc nMap, const ring dst_r)
{
    poly q=p_NSet(nMap(pGetCoeff(p),preimage_r->cf,dst_r->cf),dst_r);

    int i;
    for(i=1;i<=preimage_r->N; i++)
    {
      int pExp=p_GetExp( p,i,preimage_r);
      if (pExp != 0)
      {
        if (theMap->m[i-1]!=NULL)
        {
          poly p1=theMap->m[i-1];
          poly pp=maEvalVariable(p1,i,pExp,s,dst_r);
          q = p_Mult_q(q,pp,dst_r);
        }
        else
        {
          p_Delete(&q,dst_r);
          break;
        }
      }
    }
    int modulComp = p_GetComp( p,preimage_r);
    if (q!=NULL) p_SetCompP(q,modulComp,dst_r);
  return q;
}

poly maEval(map theMap, poly p,ring preimage_r,nMapFunc nMap, ideal s, const ring dst_r)
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
        monoms[i]=maEvalMonom(theMap,p,preimage_r,s, nMap, dst_r);
        pIter(p);
      }
    }
    result=maEvalMonom(theMap,p,preimage_r,s, nMap, dst_r);
    if (l>0)
    {
      for(i = l-1; i>=0; i--)
      {
        result=p_Add_q(result, monoms[i], dst_r);
      }
      omFreeSize((ADDRESS)monoms,l*sizeof(poly));
    }
    if (dst_r->minpoly!=NULL) result=pMinPolyNormalize(result);
  }
  return result;
}

/*2
*shifts the variables between minvar and maxvar of p  \in p_ring to the
*first maxvar-minvar+1 variables in the actual ring
*be carefull: there is no range check for the variables of p
*/
static poly pChangeSizeOfPoly(ring p_ring, poly p,int minvar,int maxvar, const ring dst_r)
{
  int i;
  poly result = NULL,resultWorkP;
  number n;

  if (p==NULL) return result;
  else result = p_Init(dst_r);
  resultWorkP = result;
  while (p!=NULL)
  {
    for (i=minvar;i<=maxvar;i++)
      p_SetExp(resultWorkP,i-minvar+1,p_GetExp(p,i,p_ring),dst_r);
    p_SetComp(resultWorkP,p_GetComp(p,p_ring),dst_r);
    n=n_Copy(pGetCoeff(p),dst_r->cf);
    p_SetCoeff(resultWorkP,n,dst_r);
    p_Setm(resultWorkP,dst_r);
    pIter(p);
    if (p!=NULL)
    {
      pNext(resultWorkP) = p_Init(dst_r);
      pIter(resultWorkP);
    }
  }
  return result;
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
poly maIMap(ring r, poly p, const ring dst_r)
{
  /* the simplest case:*/
  if(r==dst_r) return p_Copy(p,dst_r);
  nMapFunc nMap=n_SetMap(r->cf,dst_r->cf);
  int *perm=(int *)omAlloc0((r->N+1)*sizeof(int));
  //int *par_perm=(int *)omAlloc0(rPar(r)*sizeof(int));
  maFindPerm(r->names,r->N, r->parameter, r->P,
             dst_r->names,dst_r->N,dst_r->parameter, dst_r->P,
             perm,NULL, dst_r->ch);
  poly res=p_PermPoly(p,perm,r,dst_r, nMap /*,par_perm,rPar(r)*/);
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
poly pMinPolyNormalize(poly p, const ring r)
{
  number one = n_Init(1,r->cf);
  spolyrec rp;

  poly q = &rp;

  while (p != NULL)
  {
    // this returns 0, if p == MinPoly
    number product = n_Mult(pGetCoeff(p), one,r->cf);
    if ((product == NULL)||(n_IsZero(product,r->cf)))
    {
      p_LmDelete(&p,r);
    }
    else
    {
      p_SetCoeff(p, product,r);
      pNext(q) = p;
      q = p;
      p = pNext(p);
    }
  }
  pNext(q) = NULL;
  return rp.next;
}
