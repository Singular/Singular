#include "mod2.h"
#include <omalloc.h>
#include "p_polys.h"
#include "prCopy.h"
#include "ideals.h"
#include "ring.h"
#include "febase.h"
#include "fast_maps.h"

/*******************************************************************************
**
*F  maEggt  . . . . . . . . . . . . . . . . . . . . . . . .  returns extended ggt 
*/
// return NULL if deg(ggt(m1, m2)) < 2
// else return m = ggT(m1, m2) and q1, q2 such that m1 = q1*m m2 = q2*m
static poly maEggT(const poly m1, const poly m2, poly &q1, poly &q2,const ring r)
{

  int i;
  int dg = 0;
  poly ggt = NULL;
  q1 = p_Init(r);
  q2 = p_Init(r);
  ggt=p_Init(r);

  for (i=1;i<=r->N;i++) {
    Exponent_t e1 = p_GetExp(m1, i, r);
    Exponent_t e2 = p_GetExp(m2, i, r);
    if (e1 > 0 && e2 > 0){
      Exponent_t em = (e1 > e2 ? e2 : e1);
      dg += em;
      p_SetExp(ggt, i, em, r);
      p_SetExp(q1, i, e1 - em, r);
      p_SetExp(q2, i, e2 - em, r);
    }
    else {
      p_SetExp(q1, i, e1, r);
      p_SetExp(q2, i, e2, r);
    }
  }
  if (dg>1)
  {
    p_Setm(ggt, r);
    p_Setm(q1, r);
    p_Setm(q2, r);

    
  }
  else {
    p_LmFree(ggt, r);
    p_LmFree(q1, r);
    p_LmFree(q2, r);
    ggt = NULL;
  }
  return ggt;
}

static mapoly maFindBestggT(mapoly mp, mapoly & choice, mapoly & fp, mapoly & fq,const ring r)
{
  int ggt_deg = 0;
  poly p = mp->src;
  mapoly iter = choice;
  poly ggT = NULL;
  fp = NULL;
  fq = NULL;
  poly fp_p=NULL; 
  poly fq_p=NULL;
  choice=NULL;
  while ((iter != NULL) && (pDeg(iter->src, r) > ggt_deg))
  {
    //    maMonomial_Out(iter, r, NULL);
    poly q1, q2, q;
    
    q = maEggT(p, iter->src, q1, q2,r);
    if (q != NULL)
    {
      assume((q1!=NULL)&&(q2!=NULL));
      if (pDeg(q,r) > ggt_deg)
      {
	choice=iter;
        if (ggT != NULL)
        {
          p_LmFree(ggT, r);
          p_LmFree(fp_p, r);
          p_LmFree(fq_p, r);
	  
        }
	ggt_deg = pDeg(q, r);
        ggT = q;
        fp_p = q1;
        fq_p = q2;
      }
      else
      {
        p_LmFree(q, r);
        p_LmFree(q1, r);
        p_LmFree(q2, r);
      }
    }
    iter=iter->next;
  }
  if(ggT!=NULL){
    
    int dq =pTotaldegree(fq_p,r);
    if (dq!=0){
      fq=maPoly_InsertMonomial(mp, fq_p, r, NULL);
      fp=maPoly_InsertMonomial(mp, fp_p, r, NULL);
      return maPoly_InsertMonomial(mp, ggT, r, NULL);
    }
    else {
      fq=NULL;
      fp=maPoly_InsertMonomial(mp, fp_p, r, NULL);
      choice->ref++;
      return choice;
    }
  }
  else {
    return NULL;
  }

  
}
void maPoly_Optimize(mapoly mpoly, ring src_r){
  assume(mpoly!=NULL && mpoly->src!=NULL);
  mapoly iter = mpoly;
  mapoly choice;
  mapoly ggT=NULL;
  mapoly fp=NULL;
  mapoly fq=NULL;
  while (iter->next!=NULL){
    printf("topIter:\n");
    
    choice=iter->next;
    if ((iter->f1==NULL)){
      ggT=maFindBestggT(iter, choice, fp, fq,src_r);      
      if (choice!=NULL){
	iter->f1=fp;
	iter->f2=ggT;
	if (fq!=NULL){
	  ggT->ref++;
	  choice->f1=fq;
	  choice->f2=ggT;
	}
      }

    }
    iter=iter->next;
  }

}
