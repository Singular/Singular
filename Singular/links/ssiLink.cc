/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssiLink.h
 *  Purpose: declaration of sl_link routines for ssi
 ***************************************************************/
#define TRANSEXT_PRIVATES 1 /* allow access to transext internals */

#include "kernel/mod2.h"

#include "misc/intvec.h"
#include "misc/options.h"

#include "reporter/si_signals.h"
#include "reporter/s_buff.h"
#include "reporter/si_signals.h"

#include "coeffs/bigintmat.h"
#include "coeffs/longrat.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/ext_fields/transext.h"
#include "polys/simpleideals.h"
#include "polys/matpol.h"

#include "kernel/oswrapper/timer.h"
#include "kernel/oswrapper/timer.h"
#include "kernel/oswrapper/feread.h"
#include "kernel/oswrapper/rlimit.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/subexpr.h"
#include "Singular/links/silink.h"
#include "Singular/cntrlc.h"
#include "Singular/feOpt.h"
#include "Singular/lists.h"
#include "Singular/htable.h"
#include "Singular/blackbox.h"
#include "Singular/links/ssiLink.h"

#ifdef HAVE_SIMPLEIPC
#include "Singular/links/simpleipc.h"
#endif

#include <errno.h>
#include <sys/types.h>          /* for portability */
#include <ctype.h>   /*for isdigit*/
#ifndef _WIN32
#include <netdb.h>
#include <netinet/in.h> /* for htons etc.*/
#endif


#define SSI_VERSION 16
// 5->6: changed newstruct representation
// 6->7: attributes
// 7->8: qring
// 8->9: module: added rank
// 9->10: tokens in grammar.h/tok.h reorganized
// 10->11: extended ring descr. for named coeffs (not in used until 4.1)
// 11->12: add rank to ideal/module, add smatrix
// 12->13: NC rings
// 13->14: ring references
// 14->15: bigintvec, prune_map, mres_map
// 15->16: htable

EXTERN_VAR BOOLEAN FE_OPT_NO_SHELL_FLAG;
VAR link_list ssiToBeClosed=NULL;
VAR volatile BOOLEAN ssiToBeClosed_inactive=TRUE;

// forward declarations:
static void ssiWritePoly_R(const ssiInfo *d, poly p, const ring r);
static void ssiWritePoly_R_S(poly p, const ring r);
static void ssiWriteIdeal_R(const ssiInfo *d, int typ,const ideal I, const ring r);
static poly ssiReadPoly_R(const ssiInfo *D, const ring r);
static poly ssiReadPoly_R_S(char **s, const ring r);
static ideal ssiReadIdeal_R(const ssiInfo *d,const ring r);

// the helper functions:
static BOOLEAN ssiSetCurrRing(const ring r) /* returned: not accepted */
{
  //  if (currRing!=NULL)
  //  Print("need to change the ring, currRing:%s, switch to: ssiRing%d\n",IDID(currRingHdl),nr);
  //  else
  //  Print("no ring, switch to ssiRing%d\n",nr);
  if (r==currRing)
  {
    rIncRefCnt(r);
    currRingHdl=rFindHdl(r,currRingHdl);
    return TRUE;
  }
  else if ((currRing==NULL) || (!rEqual(r,currRing,1)))
  {
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        h=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        IDRING(h)=rIncRefCnt(r);
        r->ref=2;/*ref==2: d->r and h */
        break;
      }
      else if ((IDTYP(h)==RING_CMD)
      && (rEqual(r,IDRING(h),1)))
      {
        rIncRefCnt(IDRING(h));
        break;
      }
    }
    rSetHdl(h);
    return FALSE;
  }
  else
  {
    rKill(r);
    rIncRefCnt(currRing);
    return TRUE;
  }
}
static void ssiCheckCurrRing(const ring r)
{
  if ((r!=currRing)
  ||(currRingHdl==NULL)
  ||(IDRING(currRingHdl)!=r))
  {
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        h=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        IDRING(h)=rIncRefCnt(r);
        r->ref=2;/*ref==2: d->r and h */
        break;
      }
      else if ((IDTYP(h)==RING_CMD)
      && (rEqual(r,IDRING(h),1)))
      {
        break;
      }
    }
    rSetHdl(h);
  }
  assume((currRing==r) || rEqual(r,currRing));
}
// the implementation of the functions:
void ssiWriteInt(const ssiInfo *d,const int i)
{
  fprintf(d->f_write,"%d ",i);
}
void ssiWriteInt_S(const int i)
{
  StringAppend("%d ",i);
}

static void ssiWriteString(const ssiInfo *d,const char *s)
{
  fprintf(d->f_write,"%d %s ",(int)strlen(s),s);
}
static void ssiWriteString_S(const char *s)
{
  StringAppend("%d %s ",(int)strlen(s),s);
}

static void ssiWriteBigInt(const ssiInfo *d, const number n)
{
  n_WriteFd(n,d,coeffs_BIGINT);
}
static void ssiWriteBigInt_S(const number n)
{
  n_WriteFd_S(n,coeffs_BIGINT);
}

static void ssiWriteNumber_CF(const ssiInfo *d, const number n, const coeffs cf)
{
  // syntax is as follows:
  // case 1 Z/p:   3 <int>
  // case 2 Q:     3 4 <int>
  //        or     3 0 <mpz_t nominator> <mpz_t denominator>
  //        or     3 1  dto.
  //        or     3 3 <mpz_t nominator>
  //        or     3 5 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 6 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 8 <mpz_t raw nom.>
  if (getCoeffType(cf)==n_transExt)
  {
    fraction f=(fraction)n;
    ssiWritePoly_R(d,NUM(f),cf->extRing);
    ssiWritePoly_R(d,DEN(f),cf->extRing);
  }
  else if (getCoeffType(cf)==n_algExt)
  {
    ssiWritePoly_R(d,(poly)n,cf->extRing);
  }
  else if (cf->cfWriteFd!=NULL)
  {
    n_WriteFd(n,d,cf);
  }
  else WerrorS("coeff field not implemented");
}

static void ssiWriteNumber_CF_S(const number n, const coeffs cf)
{
  // syntax is as follows:
  // case 1 Z/p:   3 <int>
  // case 2 Q:     3 4 <int>
  //        or     3 0 <mpz_t nominator> <mpz_t denominator>
  //        or     3 1  dto.
  //        or     3 3 <mpz_t nominator>
  //        or     3 5 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 6 <mpz_t raw nom.> <mpz_t raw denom.>
  //        or     3 8 <mpz_t raw nom.>
  if (getCoeffType(cf)==n_transExt)
  {
    fraction f=(fraction)n;
    ssiWritePoly_R_S(NUM(f),cf->extRing);
    ssiWritePoly_R_S(DEN(f),cf->extRing);
  }
  else if (getCoeffType(cf)==n_algExt)
  {
    ssiWritePoly_R_S((poly)n,cf->extRing);
  }
  else if (cf->cfWriteFd_S!=NULL)
  {
    n_WriteFd_S(n,cf);
  }
  else WerrorS("coeff field not implemented");
}

static void ssiWriteNumber(const ssiInfo *d, const number n)
{
  ssiWriteNumber_CF(d,n,d->r->cf);
}

static void ssiWriteRing_R(ssiInfo *d,const ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <extRing> <Q-ideal> */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  /* ch=-3: cf name follows */
  /* ch=-4: NULL*/
  /* ch=-5: reference <int> */
  /* ch=-6: new reference <int> <ring> */
  if (r!=NULL)
  {
    for(int i=0;i<SI_RING_CACHE;i++)
    {
      if (d->rings[i]==r)
      {
        fprintf(d->f_write,"-5 %d ",i);
        return;
      }
    }
    for(int i=0;i<SI_RING_CACHE;i++)
    {
      if (d->rings[i]==NULL)
      {
        d->rings[i]=rIncRefCnt(r);
        fprintf(d->f_write,"-6 %d ",i);
        break;
      }
    }
    if (rField_is_Q(r) || rField_is_Zp(r))
      fprintf(d->f_write,"%d %d ",n_GetChar(r->cf),r->N);
    else if (rFieldType(r)==n_transExt)
      fprintf(d->f_write,"-1 %d ",r->N);
    else if (rFieldType(r)==n_algExt)
      fprintf(d->f_write,"-2 %d ",r->N);
    else /*dummy*/
    {
      fprintf(d->f_write,"-3 %d ",r->N);
      ssiWriteString(d,nCoeffName(r->cf));
    }

    int i;
    for(i=0;i<r->N;i++)
    {
      fprintf(d->f_write,"%d %s ",(int)strlen(r->names[i]),r->names[i]);
    }
    /* number of orderings:*/
    i=0;
    // remember dummy ring: everything 0:
    if (r->order!=NULL) while (r->order[i]!=0) i++;
    fprintf(d->f_write,"%d ",i);
    /* each ordering block: */
    i=0;
    if (r->order!=NULL) while(r->order[i]!=0)
    {
      fprintf(d->f_write,"%d %d %d ",r->order[i],r->block0[i], r->block1[i]);
      switch(r->order[i])
      {
        case ringorder_a:
        case ringorder_wp:
        case ringorder_Wp:
        case ringorder_ws:
        case ringorder_Ws:
        case ringorder_aa:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s;ii++)
            fprintf(d->f_write,"%d ",r->wvhdl[i][ii]);
        }
        break;
        case ringorder_M:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s*s;ii++)
          {
            fprintf(d->f_write,"%d ",r->wvhdl[i][ii]);
          }
        }
        break;

        case ringorder_a64:
        case ringorder_L:
        case ringorder_IS:
          Werror("ring oder not implemented for ssi:%d",r->order[i]);
          break;

        default: break;
      }
      i++;
    }
    if ((rFieldType(r)==n_transExt)
    || (rFieldType(r)==n_algExt))
    {
      ssiWriteRing_R(d,r->cf->extRing); /* includes alg.ext if rFieldType(r)==n_algExt */
    }
    /* Q-ideal :*/
    if (r->qideal!=NULL)
    {
      ssiWriteIdeal_R(d,IDEAL_CMD,r->qideal,r);
    }
    else
    {
      fputs("0 ",d->f_write/*ideal with 0 entries */);
    }
  }
  else /* dummy ring r==NULL*/
  {
    fputs("0 0 0 0 "/*,r->ch,r->N, blocks, q-ideal*/,d->f_write);
  }
  if (rIsLPRing(r)) // cannot be combined with 23 2
  {
    fprintf(d->f_write,"23 1 %d %d ",SI_LOG2(r->bitmask),r->isLPring);
  }
  else
  {
    unsigned long bm=0;
    int b=0;
    bm=rGetExpSize(bm,b,r->N);
    if (r->bitmask!=bm)
    {
      fprintf(d->f_write,"23 0 %d ",SI_LOG2(r->bitmask));
    }
    if (rIsPluralRing(r))
    {
      fputs("23 2 ",d->f_write);
      ssiWriteIdeal(d,MATRIX_CMD,(ideal)r->GetNC()->C);
      ssiWriteIdeal(d,MATRIX_CMD,(ideal)r->GetNC()->D);
    }
  }
}

static void ssiWriteIdeal_R_S(int typ,const ideal I, const ring R);
static void ssiWriteRing_R_S(ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <extRing> <Q-ideal> */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  /* ch=-3: cf name follows */
  /* ch=-4: NULL*/
  /* ch=-5: reference <int> */
  /* ch=-6: new reference <int> <ring> */
  if (r!=NULL)
  {
    if (rField_is_Q(r) || rField_is_Zp(r))
      StringAppend("%d %d ",n_GetChar(r->cf),r->N);
    else if (rFieldType(r)==n_transExt)
      StringAppend("-1 %d ",r->N);
    else if (rFieldType(r)==n_algExt)
      StringAppend("-2 %d ",r->N);
    else /*dummy*/
    {
      StringAppend("-3 %d ",r->N);
      ssiWriteString_S(nCoeffName(r->cf));
    }

    int i;
    for(i=0;i<r->N;i++)
    {
      StringAppend("%d %s ",(int)strlen(r->names[i]),r->names[i]);
    }
    /* number of orderings:*/
    i=0;
    // remember dummy ring: everything 0:
    if (r->order!=NULL) while (r->order[i]!=0) i++;
    StringAppend("%d ",i);
    /* each ordering block: */
    i=0;
    if (r->order!=NULL) while(r->order[i]!=0)
    {
      StringAppend("%d %d %d ",r->order[i],r->block0[i], r->block1[i]);
      switch(r->order[i])
      {
        case ringorder_a:
        case ringorder_wp:
        case ringorder_Wp:
        case ringorder_ws:
        case ringorder_Ws:
        case ringorder_aa:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s;ii++)
            StringAppend("%d ",r->wvhdl[i][ii]);
        }
        break;
        case ringorder_M:
        {
          int s=r->block1[i]-r->block0[i]+1; // #vars
          for(int ii=0;ii<s*s;ii++)
          {
            StringAppend("%d ",r->wvhdl[i][ii]);
          }
        }
        break;

        case ringorder_a64:
        case ringorder_L:
        case ringorder_IS:
          Werror("ring oder not implemented for ssi:%d",r->order[i]);
          break;

        default: break;
      }
      i++;
    }
    if ((rFieldType(r)==n_transExt)
    || (rFieldType(r)==n_algExt))
    {
      ssiWriteRing_R_S(r->cf->extRing); /* includes alg.ext if rFieldType(r)==n_algExt */
    }
    /* Q-ideal :*/
    if (r->qideal!=NULL)
    {
      ssiWriteIdeal_R_S(IDEAL_CMD,r->qideal,r);
    }
    else
    {
      StringAppendS("0 "/*ideal with 0 entries */);
    }
  }
  else /* dummy ring r==NULL*/
  {
    StringAppendS("0 0 0 0 "/*,r->ch,r->N, blocks, q-ideal*/);
  }
  if (rIsLPRing(r)) // cannot be combined with 23 2
  {
    StringAppend("23 1 %d %d ",SI_LOG2(r->bitmask),r->isLPring);
  }
  else
  {
    unsigned long bm=0;
    int b=0;
    bm=rGetExpSize(bm,b,r->N);
    if (r->bitmask!=bm)
    {
      StringAppend("23 0 %d ",SI_LOG2(r->bitmask));
    }
    if (rIsPluralRing(r))
    {
      StringAppendS("23 2 ");
      ssiWriteIdeal_R_S(MATRIX_CMD,(ideal)r->GetNC()->C,r);
      ssiWriteIdeal_R_S(MATRIX_CMD,(ideal)r->GetNC()->D,r);
    }
  }
}

static void ssiWriteRing(ssiInfo *d,const ring r)
{
  /* 5 <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <extRing> <Q-ideal> */
  /* ch=-1: transext, coeff ring follows */
  /* ch=-2: algext, coeff ring and minpoly follows */
  /* ch=-3: cf name follows */
  /* ch=-4: NULL */
  /* ch=-5: reference <int> */
  /* ch=-6: new reference <int> <ring> */
  if ((r==NULL)||(r->cf==NULL))
  {
    fputs("-4 ",d->f_write);
    return;
  }
  if (r==currRing) // see recursive calls for transExt/algExt
  {
    if (d->r!=NULL) rKill(d->r);
    d->r=r;
  }
  if (r!=NULL)
  {
    /*d->*/rIncRefCnt(r);
  }
  ssiWriteRing_R(d,r);
}

char* ssiWriteRing_S(const ring r)
{
  StringSetS("");
  ssiWriteRing_R_S(r);
  return StringEndS();
}
static void ssiWritePoly_R(const ssiInfo *d, poly p, const ring r)
{
  fprintf(d->f_write,"%d ",pLength(p));//number of terms

  while(p!=NULL)
  {
    ssiWriteNumber_CF(d,pGetCoeff(p),r->cf);
    //nWrite(fich,pGetCoeff(p));
    fprintf(d->f_write,"%ld ",p_GetComp(p,r));//component

    for(int j=1;j<=rVar(r);j++)
    {
      fprintf(d->f_write,"%ld ",p_GetExp(p,j,r ));//x^j
    }
    pIter(p);
  }
}
static void ssiWritePoly_R_S(poly p, const ring r)
{
  StringAppend("%d ",pLength(p));//number of terms

  while(p!=NULL)
  {
    ssiWriteNumber_CF_S(pGetCoeff(p),r->cf);
    //nWrite(fich,pGetCoeff(p));
    StringAppend("%ld ",p_GetComp(p,r));//component

    for(int j=1;j<=rVar(r);j++)
    {
      StringAppend("%ld ",p_GetExp(p,j,r ));//x^j
    }
    pIter(p);
  }
}

static void ssiWritePoly(const ssiInfo *d, poly p)
{
  ssiWritePoly_R(d,p,d->r);
}

char* ssiWritePoly_S(poly p, const ring r)
{
  StringSetS("");
  ssiWritePoly_R_S(p,r);
  return StringEndS();
}
static void ssiWriteIdeal_R(const ssiInfo *d, int typ,const ideal I, const ring R)
{
   // syntax: 7 # of elements <poly 1> <poly2>.....(ideal,module,smatrix)
   // syntax: 8 <rows> <cols> <poly 1> <poly2>.....(matrix)
   // syntax
   matrix M=(matrix)I;
   int mn;
   if (typ==MATRIX_CMD)
   {
     mn=MATROWS(M)*MATCOLS(M);
     fprintf(d->f_write,"%d %d ", MATROWS(M),MATCOLS(M));
   }
   else
   {
     mn=IDELEMS(I);
     fprintf(d->f_write,"%d ",IDELEMS(I));
   }

   for(int i=0;i<mn;i++)
   {
     ssiWritePoly_R(d,I->m[i],R);
   }
}
static void ssiWriteIdeal_R_S(int typ,const ideal I, const ring R)
{
   // syntax: 7 # of elements <poly 1> <poly2>.....(ideal,module,smatrix)
   // syntax: 8 <rows> <cols> <poly 1> <poly2>.....(matrix)
   // syntax
   matrix M=(matrix)I;
   int mn;
   if (typ==MATRIX_CMD)
   {
     mn=MATROWS(M)*MATCOLS(M);
     StringAppend("%d %d ", MATROWS(M),MATCOLS(M));
   }
   else
   {
     mn=IDELEMS(I);
     StringAppend("%d ",IDELEMS(I));
   }

   for(int i=0;i<mn;i++)
   {
     ssiWritePoly_R_S(I->m[i],R);
   }
}
void ssiWriteIdeal(const ssiInfo *d, int typ,const ideal I)
{
  ssiWriteIdeal_R(d,typ,I,d->r);
}
char* ssiWriteIdeal_S(const ideal I, const ring R)
{
  StringSetS("");
  ssiWriteIdeal_R_S(IDEAL_CMD,I,R);
  return StringEndS();
}
char* ssiWriteMatrix_S(const matrix M, const ring R)
{
  StringSetS("");
  ssiWriteIdeal_R_S(MATRIX_CMD,(ideal)M,R);
  return StringEndS();
}
char* ssiWriteModule_S(const ideal M, const ring R) /* also for smatrix*/
{
  StringSetS("");
  ssiWriteIdeal_R_S(MODUL_CMD,M,R);
  return StringEndS();
}

static void ssiWriteCommand(si_link l, command D)
{
  ssiInfo *d=(ssiInfo*)l->data;
  // syntax: <num ops> <operation> <op1> <op2> ....
  fprintf(d->f_write,"%d %d ",D->argc,D->op);
  if (D->argc >0) ssiWrite(l, &(D->arg1));
  if (D->argc < 4)
  {
    if (D->argc >1) ssiWrite(l, &(D->arg2));
    if (D->argc >2) ssiWrite(l, &(D->arg3));
  }
}

static void ssiWriteProc(const ssiInfo *d,procinfov p)
{
  if (p->data.s.body==NULL)
    iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL)
    ssiWriteString(d,p->data.s.body);
  else
    ssiWriteString(d,"");
}
static void ssiWriteProc_S(procinfov p)
{
  if (p->data.s.body==NULL)
    iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL)
    ssiWriteString_S(p->data.s.body);
  else
    ssiWriteString_S("");
}

static void ssiWriteList(si_link l,lists dd)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int Ll=dd->nr;
  fprintf(d->f_write,"%d ",Ll+1);
  int i;
  for(i=0;i<=Ll;i++)
  {
    ssiWrite(l,&(dd->m[i]));
  }
}
static void ssiWriteList_S(lists dd, const ring R)
{
  int Ll=dd->nr;
  StringAppend("%d ",Ll+1);
  int i;
  for(i=0;i<=Ll;i++)
  {
    ssiWrite_S(&(dd->m[i]),R);
  }
}
static void ssiRestoreCurrRing(ring save_ring, idhdl save_hdl)
{
  if (save_ring!=currRing) rChangeCurrRing(save_ring);
  if (save_hdl!=NULL) rSetHdl(save_hdl);
  else currRingHdl=NULL;
}

static BOOLEAN ssiWriteHTable(si_link l, stablerec *t)
{
  ssiInfo *d=(ssiInfo*)l->data;
  fprintf(d->f_write,"%d ",t_countTable(t));
  if (t==NULL) return FALSE;

  ring save_ring=currRing;
  idhdl save_hdl=currRingHdl;
  for (int i=t->max-1; i>=0; i--)
  {
    telem p=t->t[i];
    while (p!=NULL)
    {
      ssiWriteString(d,p->key);
      if (p->val_ring!=NULL)
      {
        if (l->m->SetRing(l,p->val_ring,TRUE))
        {
          ssiRestoreCurrRing(save_ring,save_hdl);
          return TRUE;
        }
      }
      else if ((p->val.Typ()!=HTABLE_CMD) && p->val.RingDependend())
      {
        Werror("object `%s' in htable has no parent ring",p->key);
        ssiRestoreCurrRing(save_ring,save_hdl);
        return TRUE;
      }
      if (ssiWrite(l,&(p->val)))
      {
        ssiRestoreCurrRing(save_ring,save_hdl);
        return TRUE;
      }
      p=p->next;
    }
  }
  ssiRestoreCurrRing(save_ring,save_hdl);
  return FALSE;
}
static void ssiWriteIntvec(const ssiInfo *d,intvec * v)
{
  fprintf(d->f_write,"%d ",v->length());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
}
static void ssiWriteIntvec_S(intvec * v)
{
  StringAppend("%d ",v->length());
  int i;
  for(i=0;i<v->length();i++)
  {
    StringAppend("%d ",(*v)[i]);
  }
}
static void ssiWriteIntmat(const ssiInfo *d,intvec * v)
{
  fprintf(d->f_write,"%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    fprintf(d->f_write,"%d ",(*v)[i]);
  }
}
static void ssiWriteIntmat_S(intvec * v)
{
  StringAppend("%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    StringAppend("%d ",(*v)[i]);
  }
}

static void ssiWriteBigintmat(const ssiInfo *d,bigintmat * v)
{
  fprintf(d->f_write,"%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt(d,(*v)[i]);
  }
}
static void ssiWriteBigintmat_S(bigintmat * v)
{
  StringAppend("%d %d ",v->rows(),v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt_S((*v)[i]);
  }
}

static void ssiWriteBigintvec(const ssiInfo *d,bigintmat * v)
{
  fprintf(d->f_write,"%d ",v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt(d,(*v)[i]);
  }
}
static void ssiWriteBigintvec_S(bigintmat * v)
{
  StringAppend("%d ",v->cols());
  int i;
  for(i=0;i<v->length();i++)
  {
    ssiWriteBigInt_S((*v)[i]);
  }
}

char *ssiReadString(const ssiInfo *d)
{
  char *buf;
  int l;
  l=s_readint(d->f_read);
  buf=(char*)omAlloc0(l+1);
  int throwaway =s_getc(d->f_read); /* skip ' '*/
  throwaway=s_readbytes(buf,l,d->f_read);
  //if (throwaway!=l) printf("want %d, got %d bytes\n",l,throwaway);
  buf[l]='\0';
  return buf;
}
static char *ssiReadString_S(char** s)
{
  char *buf;
  int l;
  l=s_readint_S(s);
  buf=(char*)omAlloc0(l+1);
  (*s)++; /* skip ' '*/
  for(int i=0;i<l;i++) { buf[i]=(**s); (*s)++; }
  buf[l]='\0';
  return buf;
}

int ssiReadInt(const ssiInfo *d)
{
  return s_readint(d->f_read);
}
int ssiReadInt_S(char **s)
{
  return s_readint_S(s);
}

static number ssiReadNumber_CF(const ssiInfo *d, const coeffs cf)
{
  if (cf->cfReadFd!=ndReadFd)
  {
     return n_ReadFd(d,cf);
  }
  else if (getCoeffType(cf) == n_transExt)
  {
    // poly poly
    fraction f=(fraction)n_Init(1,cf);
    p_Delete(&NUM(f),cf->extRing);
    NUM(f)=ssiReadPoly_R(d,cf->extRing);
    DEN(f)=ssiReadPoly_R(d,cf->extRing);
    return (number)f;
  }
  else if (getCoeffType(cf) == n_algExt)
  {
    // poly
    return (number)ssiReadPoly_R(d,cf->extRing);
  }
  else WerrorS("coeffs not implemented in ssiReadNumber");
  return NULL;
}

static number ssiReadNumber_CF_S(char **s, const coeffs cf)
{
  if (cf->cfReadFd_S!=ndReadFd_S)
  {
     return n_ReadFd_S(s,cf);
  }
  else if (getCoeffType(cf) == n_transExt)
  {
    // poly poly
    fraction f=(fraction)n_Init(1,cf);
    p_Delete(&NUM(f),cf->extRing);
    NUM(f)=ssiReadPoly_R_S(s,cf->extRing);
    DEN(f)=ssiReadPoly_R_S(s,cf->extRing);
    return (number)f;
  }
  else if (getCoeffType(cf) == n_algExt)
  {
    // poly
    return (number)ssiReadPoly_R_S(s,cf->extRing);
  }
  else WerrorS("coeffs not implemented in ssiReadNumber");
  return NULL;
}

static number ssiReadBigInt(const ssiInfo *d)
{
  number n=ssiReadNumber_CF(d,coeffs_BIGINT);
  if ((SR_HDL(n) & SR_INT)==0)
  {
    if (n->s!=3) Werror("invalid sub type in bigint:%d",n->s);
  }
  return n;
}
static number ssiReadBigInt_S(char**s)
{
  number n=ssiReadNumber_CF_S(s,coeffs_BIGINT);
  if ((SR_HDL(n) & SR_INT)==0)
  {
    if (n->s!=3) Werror("invalid sub type in bigint:%d",n->s);
  }
  return n;
}

static number ssiReadNumber(ssiInfo *d)
{
  return ssiReadNumber_CF(d,d->r->cf);
}

static ring ssiReadRing(ssiInfo *d)
{
/* syntax is <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <Q-ideal> */
  int ch;
  int new_ref=-1;
  ch=s_readint(d->f_read);
  if (ch==-6)
  {
    new_ref=s_readint(d->f_read);
    ch=s_readint(d->f_read);
  }
  if (ch==-5)
  {
    int index=s_readint(d->f_read);
    ring r=d->rings[index];
    rIncRefCnt(r);
    return r;
  }
  if (ch==-4)
    return NULL;
  int N=s_readint(d->f_read);
  char **names;
  coeffs cf=NULL;
  if (ch==-3)
  {
    char *cf_name=ssiReadString(d);
    cf=nFindCoeffByName(cf_name);
    if (cf==NULL)
    {
      Werror("cannot find cf:%s",cf_name);
      omFreeBinAddr(cf_name);
      return NULL;
    }
  }
  if (N!=0)
  {
    names=(char**)omAlloc(N*sizeof(char*));
    for(int i=0;i<N;i++)
    {
      names[i]=ssiReadString(d);
    }
  }
  // read the orderings:
  int num_ord; // number of orderings
  num_ord=s_readint(d->f_read);
  rRingOrder_t *ord=(rRingOrder_t *)omAlloc0((num_ord+1)*sizeof(rRingOrder_t));
  int *block0=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for(int i=0;i<num_ord;i++)
  {
    ord[i]=(rRingOrder_t)s_readint(d->f_read);
    block0[i]=s_readint(d->f_read);
    block1[i]=s_readint(d->f_read);
    switch(ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        int s=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(s*sizeof(int));
        for(int ii=0;ii<s;ii++)
          wvhdl[i][ii]=s_readint(d->f_read);
      }
      break;
      case ringorder_M:
      {
        int s=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(s*s*sizeof(int));
        for(int ii=0;ii<s*s;ii++)
        {
          wvhdl[i][ii]=s_readint(d->f_read);
        }
      }
      break;
      case ringorder_a64:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring order not implemented for ssi:%d",ord[i]);
        break;

      default: break;
    }
  }
  if (N==0)
  {
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  else
  {
    ring r=NULL;
    if (ch>=0) /* Q, Z/p */
      r=rDefault(ch,N,names,num_ord,ord,block0,block1,wvhdl);
    else if (ch==-1) /* trans ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing(d);
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_transExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-2) /* alg ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing(d); /* includes qideal */
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_algExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-3)
    {
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else
    {
      Werror("ssi: read unknown coeffs type (%d)",ch);
      for(int i=0;i<N;i++)
      {
        omFree(names[i]);
      }
      omFreeSize(names,N*sizeof(char*));
      return NULL;
    }
    ideal q=ssiReadIdeal_R(d,r);
    if (IDELEMS(q)==0) omFreeBin(q,sip_sideal_bin);
    else r->qideal=q;
    for(int i=0;i<N;i++)
    {
      omFree(names[i]);
    }
    omFreeSize(names,N*sizeof(char*));
    rIncRefCnt(r);
    // check if such ring already exist as ssiRing*
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        break;
      }
      else if ((IDTYP(h)==RING_CMD)
      && (r!=IDRING(h))
      && (rEqual(r,IDRING(h),1)))
      {
        rDelete(r);
        r=rIncRefCnt(IDRING(h));
        break;
      }
    }
    if (new_ref!=-1)
    {
      d->rings[new_ref]=r;
      rIncRefCnt(r);
    }
    return r;
  }
}
static ideal ssiReadIdeal_R_S(char** s,const ring r);
static ring ssiReadRing_R_S(char **s)
{
/* syntax is <ch> <N> <l1> <v1> ...<lN> <vN> <number of orderings> <ord1> <block0_1> <block1_1> .... <Q-ideal> */
  int ch;
  int new_ref=-1;
  ch=s_readint_S(s);
  if (ch==-4)
    return NULL;
  int N=s_readint_S(s);
  char **names;
  coeffs cf=NULL;
  if (ch==-3)
  {
    char *cf_name=ssiReadString_S(s);
    cf=nFindCoeffByName(cf_name);
    if (cf==NULL)
    {
      Werror("cannot find cf:%s",cf_name);
      omFreeBinAddr(cf_name);
      return NULL;
    }
  }
  if (N!=0)
  {
    names=(char**)omAlloc(N*sizeof(char*));
    for(int i=0;i<N;i++)
    {
      names[i]=ssiReadString_S(s);
    }
  }
  // read the orderings:
  int num_ord; // number of orderings
  num_ord=s_readint_S(s);
  rRingOrder_t *ord=(rRingOrder_t *)omAlloc0((num_ord+1)*sizeof(rRingOrder_t));
  int *block0=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int *block1=(int *)omAlloc0((num_ord+1)*sizeof(int));
  int **wvhdl=(int**)omAlloc0((num_ord+1)*sizeof(int*));
  for(int i=0;i<num_ord;i++)
  {
    ord[i]=(rRingOrder_t)s_readint_S(s);
    block0[i]=s_readint_S(s);
    block1[i]=s_readint_S(s);
    switch(ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        int ss=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(ss*sizeof(int));
        for(int ii=0;ii<ss;ii++)
          wvhdl[i][ii]=s_readint_S(s);
      }
      break;
      case ringorder_M:
      {
        int ss=block1[i]-block0[i]+1; // #vars
        wvhdl[i]=(int*)omAlloc(ss*ss*sizeof(int));
        for(int ii=0;ii<ss*ss;ii++)
        {
          wvhdl[i][ii]=s_readint_S(s);
        }
      }
      break;
      case ringorder_a64:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring order not implemented for ssi:%d",ord[i]);
        break;

      default: break;
    }
  }
  if (N==0)
  {
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  else
  {
    ring r=NULL;
    if (ch>=0) /* Q, Z/p */
      r=rDefault(ch,N,names,num_ord,ord,block0,block1,wvhdl);
    else if (ch==-1) /* trans ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing_R_S(s);
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_transExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-2) /* alg ext. */
    {
      TransExtInfo T;
      T.r=ssiReadRing_R_S(s); /* includes qideal */
      if (T.r==NULL) return NULL;
      cf=nInitChar(n_algExt,&T);
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else if (ch==-3)
    {
      r=rDefault(cf,N,names,num_ord,ord,block0,block1,wvhdl);
    }
    else
    {
      Werror("ssi: read unknown coeffs type (%d)",ch);
      for(int i=0;i<N;i++)
      {
        omFree(names[i]);
      }
      omFreeSize(names,N*sizeof(char*));
      return NULL;
    }
    ideal q=ssiReadIdeal_R_S(s,r);
    if (IDELEMS(q)==0) omFreeBin(q,sip_sideal_bin);
    else r->qideal=q;
    for(int i=0;i<N;i++)
    {
      omFree(names[i]);
    }
    omFreeSize(names,N*sizeof(char*));
    return r;
  }
}
ring ssiReadRing_S(char *s)
{
  ring r=ssiReadRing_R_S(&s);
  // check if such ring already exist as ssiRing*
  char name[20];
  int nr=0;
  idhdl h=NULL;
  loop // already defined?
  {
    snprintf(name,20,"ssiRing%d",nr); nr++;
    h=IDROOT->get(name, 0);
    if (h==NULL)
    {
      break;
    }
    else if ((IDTYP(h)==RING_CMD)
    && (r!=IDRING(h))
    && (rEqual(r,IDRING(h),1)))
    {
      rDelete(r);
      r=rIncRefCnt(IDRING(h));
      break;
    }
  }
  if ((h==NULL) && ((currRing==NULL) || (!rEqual(r,currRing,1))))
  {
    char name[20];
    int nr=0;
    idhdl hh=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      hh=IDROOT->get(name, 0);
      if (hh==NULL)
      {
        hh=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        break;
      }
      else if ((IDTYP(hh)==RING_CMD)
      && (rEqual(r,IDRING(hh),1)))
      {
        break;
      }
    }
    rSetHdl(hh);
  }
  else
    rSetHdl(h);
  return r;
}

static poly ssiReadPoly_R(const ssiInfo *d, const ring r)
{
// < # of terms> < term1> < .....
  int n,i,l;
  n=ssiReadInt(d); // # of terms
  //Print("poly: terms:%d\n",n);
  poly p;
  poly ret=NULL;
  poly prev=NULL;
  for(l=0;l<n;l++) // read n terms
  {
// coef,comp.exp1,..exp N
    p=p_Init(r,r->PolyBin);
    pSetCoeff0(p,ssiReadNumber_CF(d,r->cf));
    int D;
    D=s_readint(d->f_read);
    p_SetComp(p,D,r);
    for(i=1;i<=rVar(r);i++)
    {
      D=s_readint(d->f_read);
      p_SetExp(p,i,D,r);
    }
    p_Setm(p,r);
    p_Test(p,r);
    if (ret==NULL) ret=p;
    else           pNext(prev)=p;
    prev=p;
 }
 return ret;
}

static poly ssiReadPoly_R_S(char **s, const ring r)
{
// < # of terms> < term1> < .....
  int n,i,l;
  char* c=*s;
  n=s_readint_S(&c); // # of terms
  poly p;
  poly ret=NULL;
  poly prev=NULL;
  for(l=0;l<n;l++) // read n terms
  {
// coef,comp.exp1,..exp N
    p=p_Init(r,r->PolyBin);
    number cf=ssiReadNumber_CF_S(&c,r->cf);
    pSetCoeff0(p,cf);
    int D;
    D=s_readint_S(&c);
    p_SetComp(p,D,r);
    for(i=1;i<=rVar(r);i++)
    {
      D=s_readint_S(&c);
      p_SetExp(p,i,D,r);
    }
    p_Setm(p,r);
    p_Test(p,r);
    if (ret==NULL) ret=p;
    else           pNext(prev)=p;
    prev=p;
 }
 *s=c;
 return ret;
}

static poly ssiReadPoly(ssiInfo *d)
{
  return ssiReadPoly_R(d,d->r);
}

poly ssiReadPoly_S(char *s, const ring r)
{
  return ssiReadPoly_R_S(&s,r);
}
static ideal ssiReadIdeal_R(const ssiInfo *d,const ring r)
{
// < # of terms> < term1> < .....
  int n,i;
  ideal I;
  n=s_readint(d->f_read);
  I=idInit(n,1); // will be fixed later for module/smatrix
  for(i=0;i<IDELEMS(I);i++) // read n terms
  {
    I->m [i]=ssiReadPoly_R(d,r);
  }
  return I;
}
static ideal ssiReadIdeal_R_S(char** s,const ring r)
{
// < # of terms> < term1> < .....
  int n,i;
  ideal I;
  n=s_readint_S(s);
  I=idInit(n,1); // will be fixed later for module/smatrix
  for(i=0;i<IDELEMS(I);i++) // read n terms
  {
    I->m [i]=ssiReadPoly_R_S(s,r);
  }
  return I;
}

ideal ssiReadIdeal(ssiInfo *d)
{
  return ssiReadIdeal_R(d,d->r);
}

ideal ssiReadIdeal_S(char *s, const ring R)
{
  return ssiReadIdeal_R_S(&s,R);
}

static matrix ssiReadMatrix(ssiInfo *d)
{
  int n,m;
  m=s_readint(d->f_read);
  n=s_readint(d->f_read);
  matrix M=mpNew(m,n);
  poly p;
  for(int i=1;i<=MATROWS(M);i++)
    for(int j=1;j<=MATCOLS(M);j++)
    {
      p=ssiReadPoly(d);
      MATELEM(M,i,j)=p;
    }
  return M;
}
static matrix ssiReadMatrix_R_S(char** s, const ring R)
{
  int n,m;
  m=s_readint_S(s);
  n=s_readint_S(s);
  matrix M=mpNew(m,n);
  poly p;
  for(int i=1;i<=MATROWS(M);i++)
    for(int j=1;j<=MATCOLS(M);j++)
    {
      p=ssiReadPoly_R_S(s,R);
      MATELEM(M,i,j)=p;
    }
  return M;
}
matrix ssiReadMatrix_R_S(char* s, const ring R)
{
  return ssiReadMatrix_R_S(&s,R);
}

static command ssiReadCommand(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  // syntax: <num ops> <operation> <op1> <op2> ....
  command D=(command)omAlloc0(sizeof(*D));
  int argc,op;
  argc=s_readint(d->f_read);
  op=s_readint(d->f_read);
  D->argc=argc; D->op=op;
  leftv v;
  if (argc >0)
  {
    v=ssiRead1(l);
    memcpy(&(D->arg1),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  if (argc <4)
  {
    if (D->argc >1)
    {
      v=ssiRead1(l);
      memcpy(&(D->arg2),v,sizeof(*v));
      omFreeBin(v,sleftv_bin);
    }
    if (D->argc >2)
    {
      v=ssiRead1(l);
      memcpy(&(D->arg3),v,sizeof(*v));
      omFreeBin(v,sleftv_bin);
    }
  }
  else
  {
    leftv prev=&(D->arg1);
    argc--;
    while(argc >0)
    {
      v=ssiRead1(l);
      prev->next=v;
      prev=v;
      argc--;
    }
  }
  return D;
}

static procinfov ssiReadProc(const ssiInfo *d)
{
  char *s=ssiReadString(d);
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=s;
  return p;
}
static procinfov ssiReadProc_S(char**s)
{
  char *st=ssiReadString_S(s);
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=st;
  return p;
}
static lists ssiReadList(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int nr;
  nr=s_readint(d->f_read);
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(nr);

  int i;
  leftv v;
  for(i=0;i<=L->nr;i++)
  {
    v=ssiRead1(l);
    memcpy(&(L->m[i]),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  return L;
}
static stablerec* ssiReadHTable(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int nr=ssiReadInt(d);
  if (nr<0)
  {
    Werror("invalid htable size %d in ssi stream",nr);
    return NULL;
  }

  stablerec *t=t_createTable(nr*2+3);
  ring save_ring=currRing;
  idhdl save_hdl=currRingHdl;

  for (int i=0; i<nr; i++)
  {
    char *key=ssiReadString(d);
    leftv val=ssiRead1(l);
    if (val==NULL)
    {
      omFree(key);
      t_destroyTable(t);
      ssiRestoreCurrRing(save_ring,save_hdl);
      return NULL;
    }
    BOOLEAN failed=t_addTable(t,key,val);
    val->CleanUp();
    omFreeBin(val,sleftv_bin);
    if (failed || errorreported)
    {
      t_destroyTable(t);
      ssiRestoreCurrRing(save_ring,save_hdl);
      return NULL;
    }
  }

  ssiRestoreCurrRing(save_ring,save_hdl);
  return t;
}
static lists ssiReadList_S(char**s, const ring R)
{
  int nr;
  nr=s_readint_S(s);
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(nr);

  int i;
  leftv v;
  for(i=0;i<=L->nr;i++)
  {
    v=ssiRead1_S(s,R);
    memcpy(&(L->m[i]),v,sizeof(*v));
    omFreeBin(v,sleftv_bin);
  }
  return L;
}
static intvec* ssiReadIntvec(const ssiInfo *d)
{
  int nr;
  nr=s_readint(d->f_read);
  intvec *v=new intvec(nr);
  for(int i=0;i<nr;i++)
  {
    (*v)[i]=s_readint(d->f_read);
  }
  return v;
}
static intvec* ssiReadIntvec_S(char**s)
{
  int nr;
  nr=s_readint_S(s);
  intvec *v=new intvec(nr);
  for(int i=0;i<nr;i++)
  {
    (*v)[i]=s_readint_S(s);
  }
  return v;
}
static intvec* ssiReadIntmat(const ssiInfo *d)
{
  int r,c;
  r=s_readint(d->f_read);
  c=s_readint(d->f_read);
  intvec *v=new intvec(r,c,0);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=s_readint(d->f_read);
  }
  return v;
}
static intvec* ssiReadIntmat_S(char**s)
{
  int r,c;
  r=s_readint_S(s);
  c=s_readint_S(s);
  intvec *v=new intvec(r,c,0);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=s_readint_S(s);
  }
  return v;
}
static bigintmat* ssiReadBigintmat(const ssiInfo *d)
{
  int r,c;
  r=s_readint(d->f_read);
  c=s_readint(d->f_read);
  bigintmat *v=new bigintmat(r,c,coeffs_BIGINT);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=ssiReadBigInt(d);
  }
  return v;
}
static bigintmat* ssiReadBigintmat_S(char**s)
{
  int r,c;
  r=s_readint_S(s);
  c=s_readint_S(s);
  bigintmat *v=new bigintmat(r,c,coeffs_BIGINT);
  for(int i=0;i<r*c;i++)
  {
    (*v)[i]=ssiReadBigInt_S(s);
  }
  return v;
}
static bigintmat* ssiReadBigintvec(const ssiInfo *d)
{
  int c;
  c=s_readint(d->f_read);
  bigintmat *v=new bigintmat(1,c,coeffs_BIGINT);
  for(int i=0;i<c;i++)
  {
    (*v)[i]=ssiReadBigInt(d);
  }
  return v;
}
static bigintmat* ssiReadBigintvec_S(char**s)
{
  int c;
  c=s_readint_S(s);
  bigintmat *v=new bigintmat(1,c,coeffs_BIGINT);
  for(int i=0;i<c;i++)
  {
    (*v)[i]=ssiReadBigInt_S(s);
  }
  return v;
}

static void ssiReadBlackbox(leftv res, si_link l)
{
  leftv lv=ssiRead1(l);
  char *name=(char*)lv->data;
  omFreeBin(lv,sleftv_bin);
  int tok;
  blackboxIsCmd(name,tok);
  if (tok>MAX_TOK)
  {
    ring save_ring=currRing;
    idhdl save_hdl=currRingHdl;
    blackbox *b=getBlackboxStuff(tok);
    res->rtyp=tok;
    b->blackbox_deserialize(&b,&(res->data),l);
    if (save_ring!=currRing)
    {
      rChangeCurrRing(save_ring);
      if (save_hdl!=NULL) rSetHdl(save_hdl);
      else currRingHdl=NULL;
    }
  }
  else
  {
    Werror("blackbox %s not found",name);
  }
  omFree(name);
}

static void ssiReadAttrib(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  BITSET fl=(BITSET)s_readint(d->f_read);
  int nr_of_attr=s_readint(d->f_read);
  if (nr_of_attr>0)
  {
    for(int i=1;i<nr_of_attr;i++)
    {
    }
  }
  leftv tmp=ssiRead1(l);
  memcpy(res,tmp,sizeof(sleftv));
  memset(tmp,0,sizeof(sleftv));
  omFreeBin(tmp,sleftv_bin);
  if (nr_of_attr>0)
  {
  }
  res->flag=fl;
}
static void ssiReadRingProperties(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int what=s_readint(d->f_read);
  switch(what)
  {
    case 0: // bitmask
    {
      int lb=s_readint(d->f_read);
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      rComplete(d->r);
      break;
    }
    case 1: // LPRing
    {
      int lb=s_readint(d->f_read);
      int isLPring=s_readint(d->f_read);
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      d->r->isLPring=isLPring;
      rComplete(d->r);
      break;
    }
    case 2: // Plural rings
    {
      matrix C=ssiReadMatrix(d);
      matrix D=ssiReadMatrix(d);
      nc_CallPlural(C,D,NULL,NULL,d->r,true,true,false,d->r,false);
      break;
    }
  }
}
//**************************************************************************/

BOOLEAN ssiOpen(si_link l, short flag, leftv u)
{
  if (l!=NULL)
  {
    const char *mode;
    if (flag & SI_LINK_OPEN)
    {
      if (strcmp(l->mode, "r") == 0)
        flag = SI_LINK_READ;
      else if (strcmp(l->mode,"string")==0)
      {
        SI_LINK_SET_RW_OPEN_P(l);
        return FALSE;
      }
      else flag = SI_LINK_WRITE;
    }
    if (((flag == SI_LINK_READ)
      || (flag == SI_LINK_WRITE))
    && (strcmp(l->mode,"string")==0))
    {
      SI_LINK_SET_RW_OPEN_P(l);
      return FALSE;
    }

    if (flag == SI_LINK_READ) mode = "r";
    else if (strcmp(l->mode, "w") == 0) mode = "w";
    else if (strcmp(l->mode, "fork") == 0) mode = "fork";
    else if (strcmp(l->mode, "tcp") == 0) mode = "tcp";
    else if (strcmp(l->mode, "connect") == 0) mode = "connect";
    else mode = "a";

#ifdef _WIN32
    if ((strcmp(mode,"fork")==0)
    || (strcmp(mode,"tcp")==0)
    || (strcmp(mode,"connect")==0))
    {
      WerrorS("ssi process and socket links are not supported on native Windows");
      l->flags=0;
      return TRUE;
    }
#endif

    SI_LINK_SET_OPEN_P(l, flag);
    if(l->data!=NULL) omFreeSize(l->data,sizeof(ssiInfo));
    omFreeBinAddr(l->mode);
    l->mode = omStrDup(mode);

    ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
    l->data=d;
    if (l->name[0] == '\0')
    {
#ifndef _WIN32
      if (strcmp(mode,"fork")==0)
      {
        int cpus = (long) feOptValue(FE_OPT_CPUS);
        if (cpus<1)
        {
          WerrorS("no sub-processes allowed");
          l->flags=0;
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        int pc[2];
        int cp[2];
        int err1=pipe(pc);
        int err2=pipe(cp);
        if (err1 || err2)
        {
          Werror("pipe failed with %d\n",errno);
          l->flags=0;
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        link_list n=(link_list)omAlloc(sizeof(link_struct));
        n->u=u;
        n->l=l;
        n->next=(void *)ssiToBeClosed;
        ssiToBeClosed=n;

        pid_t pid = fork();
        if (pid == -1 && errno == EAGAIN)   // RLIMIT_NPROC too low?
        {
          raise_rlimit_nproc();
          pid = fork();
        }
        if (pid == -1)
        {
          WerrorS("could not fork");
          l->flags=0;
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        if (pid==0) /*fork: child*/
        {
          /* block SIGINT */
          sigset_t sigint;
          sigemptyset(&sigint);
          sigaddset(&sigint, SIGINT);
          sigprocmask(SIG_BLOCK, &sigint, NULL);
          si_set_signal(SIGTERM,sig_term_hdl);
          /* set #cpu to 1 for the child:*/
          feSetOptValue(FE_OPT_CPUS,1);

          link_list hh=(link_list)ssiToBeClosed->next;
          /* we know: l is the first entry in ssiToBeClosed-list */
          while(hh!=NULL)
          {
            SI_LINK_SET_CLOSE_P(hh->l);
            ssiInfo *dd=(ssiInfo*)hh->l->data;
            s_close(dd->f_read);
            fclose(dd->f_write);
            if (dd->r!=NULL) rKill(dd->r);
            omFreeSize((ADDRESS)dd,(sizeof *dd));
            hh->l->data=NULL;
            link_list nn=(link_list)hh->next;
            omFree(hh);
            hh=nn;
          }
          ssiToBeClosed->next=NULL;
#ifdef HAVE_SIMPLEIPC
          memset(sem_acquired, 0, SIPC_MAX_SEMAPHORES*sizeof(sem_acquired[0]));
#endif   // HAVE_SIMPLEIPC
          si_close(pc[1]); si_close(cp[0]);
          d->f_write=fdopen(cp[1],"w");
          d->f_read=s_open(pc[0]);
          d->fd_read=pc[0];
          d->fd_write=cp[1];
          //d->r=currRing;
          //if (d->r!=NULL) d->r->ref++;
          l->data=d;
          omFreeBinAddr(l->mode);
          l->mode = omStrDup(mode);
          singular_in_batchmode=TRUE;
          SI_LINK_SET_RW_OPEN_P(l);
          //myynest=0;
          fe_fgets_stdin=fe_fgets_dummy;
          if ((u!=NULL)&&(u->rtyp==IDHDL))
          {
            idhdl h=(idhdl)u->data;
            h->lev=0;
          }
          loop
          {
            if (!SI_LINK_OPEN_P(l)) m2_end(-1);
            if(d->f_read->is_eof) m2_end(-1);
            leftv h=ssiRead1(l); /*contains an exit.... */
            if (feErrors != NULL && *feErrors != '\0')
            {
              // handle errors:
              PrintS(feErrors); /* currently quite simple */
              *feErrors = '\0';
            }
            ssiWrite(l,h);
            h->CleanUp();
            omFreeBin(h, sleftv_bin);
          }
          /* never reached*/
        }
        else if (pid>0) /*fork: parent*/
        {
          d->pid=pid;
          si_close(pc[0]); si_close(cp[1]);
          d->f_write=fdopen(pc[1],"w");
          d->f_read=s_open(cp[0]);
          d->fd_read=cp[0];
          d->fd_write=pc[1];
          SI_LINK_SET_RW_OPEN_P(l);
          d->send_quit_at_exit=1;
          //d->r=currRing;
          //if (d->r!=NULL) d->r->ref++;
        }
        else
        {
          Werror("fork failed (%d)",errno);
          l->data=NULL;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
      }
      // ---------------------------------------------------------------------
      else if (strcmp(mode,"tcp")==0)
      {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
          WerrorS("ERROR opening socket");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        memset((char *) &serv_addr,0, sizeof(serv_addr));
        portno = 1025;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        do
        {
          portno++;
          serv_addr.sin_port = htons(portno);
          if(portno > 50000)
          {
            WerrorS("ERROR on binding (no free port available?)");
            l->data=NULL;
            l->flags=0;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
        }
        while(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
        Print("waiting on port %d\n", portno);mflush();
        listen(sockfd,1);
        newsockfd = si_accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(newsockfd < 0)
        {
          WerrorS("ERROR on accept");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        PrintS("client accepted\n");
        d->fd_read = newsockfd;
        d->fd_write = newsockfd;
        d->f_read = s_open(newsockfd);
        d->f_write = fdopen(newsockfd, "w");
        SI_LINK_SET_RW_OPEN_P(l);
        si_close(sockfd);
      }
      // no ssi-Link on stdin or stdout
      else
#endif
      if (strcmp(mode,"string")==0)
      {
        SI_LINK_SET_RW_OPEN_P(l);
      }
      else
      {
        Werror("invalid mode >>%s<< for ssi",mode);
        l->data=NULL;
        l->flags=0;
        omFreeSize(d,sizeof(ssiInfo));
        return TRUE;
      }
    }
    // =========================================================================
    else /*now l->name!=NULL*/
    {
      // tcp mode
#ifndef _WIN32
      if(strcmp(mode,"tcp")==0)
      {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0)
        {
          WerrorS("ERROR opening socket");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        memset((char *) &serv_addr,0, sizeof(serv_addr));
        portno = 1025;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        do
        {
          portno++;
          serv_addr.sin_port = htons(portno);
          if(portno > 50000)
          {
            WerrorS("ERROR on binding (no free port available?)");
            l->data=NULL;
            l->flags=0;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
        }
        while(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0);
        //Print("waiting on port %d\n", portno);mflush();
        listen(sockfd,1);
        char* cli_host = (char*)omAlloc(256);
        char* path = (char*)omAlloc(1024);
        int r = si_sscanf(l->name,"%255[^:]:%s",cli_host,path);
        if(r == 0)
        {
          WerrorS("ERROR: no host specified");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          omFree(path);
          omFree(cli_host);
          return TRUE;
        }
        else if(r == 1)
        {
          WarnS("program not specified, using /usr/local/bin/Singular");
          Warn("in line >>%s<<",my_yylinebuf);
          strcpy(path,"/usr/local/bin/Singular");
        }
        char* ssh_command = (char*)omAlloc(256);
        char* ser_host = (char*)omAlloc(64);
        if(strcmp(cli_host,"localhost")==0)
          strcpy(ser_host,"localhost");
        else
          gethostname(ser_host,64);
        if (strcmp(cli_host,"localhost")==0) /*avoid "ssh localhost" as key may change*/
          snprintf(ssh_command,256,"%s -q --batch --link=ssi --MPhost=%s --MPport=%d &",path,ser_host,portno);
        else
          snprintf(ssh_command,256,"ssh %s %s -q --batch --link=ssi --MPhost=%s --MPport=%d &",cli_host,path,ser_host,portno);
        //Print("client on %s started:%s\n",cli_host,path);
        omFree(path);
        omFree(cli_host);
        if (TEST_OPT_PROT) { Print("running >>%s<<\n",ssh_command); }
        int re=system(ssh_command);
        if (re<0)
        {
          Werror("ERROR running `%s` (%d)",ssh_command,re);
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        omFree(ssh_command);
        omFree(ser_host);
        clilen = sizeof(cli_addr);
        newsockfd = si_accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if(newsockfd < 0)
        {
          WerrorS("ERROR on accept");
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
        //PrintS("client accepted\n");
        d->fd_read = newsockfd;
        d->fd_write = newsockfd;
        d->f_read = s_open(newsockfd);
        d->f_write = fdopen(newsockfd, "w");
        si_close(sockfd);
        SI_LINK_SET_RW_OPEN_P(l);
        d->send_quit_at_exit=1;
        link_list newlink=(link_list)omAlloc(sizeof(link_struct));
        newlink->u=u;
        newlink->l=l;
        newlink->next=(void *)ssiToBeClosed;
        ssiToBeClosed=newlink;
        fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,si_opt_1,si_opt_2);
      }
      // ----------------------------------------------------------------------
      else if(strcmp(mode,"connect")==0)
      {
        char* host = (char*)omAlloc(256);
        int sockfd, portno;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        si_sscanf(l->name,"%255[^:]:%d",host,&portno);
        //Print("connect to host %s, port %d\n",host,portno);mflush();
        if (portno!=0)
        {
          sockfd = socket(AF_INET, SOCK_STREAM, 0);
          if (sockfd < 0)
          {
            WerrorS("ERROR opening socket");
            l->flags=0;
            l->data=NULL;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
          server = gethostbyname(host);
          if (server == NULL)
          {
            WerrorS("ERROR, no such host");
            l->flags=0;
            l->data=NULL;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
          memset((char *) &serv_addr, 0, sizeof(serv_addr));
          serv_addr.sin_family = AF_INET;
          memcpy((char *)&serv_addr.sin_addr.s_addr,
                (char *)server->h_addr,
                server->h_length);
          serv_addr.sin_port = htons(portno);
          if (si_connect(sockfd,(sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
          {
            Werror("ERROR connecting(errno=%d)",errno);
            l->flags=0;
            l->data=NULL;
            omFreeSize(d,sizeof(ssiInfo));
            return TRUE;
          }
          //PrintS("connected\n");mflush();
          d->f_read=s_open(sockfd);
          d->fd_read=sockfd;
          d->f_write=fdopen(sockfd,"w");
          d->fd_write=sockfd;
          SI_LINK_SET_RW_OPEN_P(l);
          omFree(host);
        }
        else
        {
          l->data=NULL;
          l->flags=0;
          omFreeSize(d,sizeof(ssiInfo));
          return TRUE;
        }
      }
      // ======================================================================
      else
#endif
      {
        // normal link to a file
        if (FE_OPT_NO_SHELL_FLAG) {WerrorS("no links allowed");return TRUE;}
        FILE *outfile;
        char *filename=l->name;

        if(filename[0]=='>')
        {
          if (filename[1]=='>')
          {
            filename+=2;
            mode = "a";
          }
          else
          {
            filename++;
            mode="w";
          }
        }
        outfile=myfopen(filename,mode);
        if (outfile!=NULL)
        {
          if (strcmp(l->mode,"r")==0)
          {
            fclose(outfile);
            d->f_read=s_open_by_name(filename);
          }
          else
          {
            d->f_write = outfile;
            fprintf(d->f_write,"98 %d %d %u %u\n",SSI_VERSION,MAX_TOK,si_opt_1,si_opt_2);
          }
        }
        else
        {
          omFree(d);
          l->data=NULL;
          l->flags=0;
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

//**************************************************************************/
#if 0
static BOOLEAN ssiPrepClose(si_link l)
{
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      if (d->send_quit_at_exit)
      {
        fputs("99\n",d->f_write);
        fflush(d->f_write);
      }
      d->quit_sent=1;
    }
  }
  return FALSE;
}
#endif

BOOLEAN ssiClose(si_link l)
{
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssiInfo *d = (ssiInfo *)l->data;
    if (d!=NULL)
    {
      // send quit signal
      if ((d->send_quit_at_exit)
      && (d->quit_sent==0))
      {
        fputs("99\n",d->f_write);
        fflush(d->f_write);
        d->quit_sent=1;
      }
      // clean ring
      if (d->r!=NULL) rKill(d->r);
      for(int i=0;i<SI_RING_CACHE;i++)
      {
        if (d->rings[i]!=NULL)  rKill(d->rings[i]);
        d->rings[i]=NULL;
      }
      if (d->f_read!=NULL) { s_close(d->f_read);d->f_read=NULL;}
      if (d->f_write!=NULL) { fclose(d->f_write); d->f_write=NULL; }
#ifndef _WIN32
      if (((strcmp(l->mode,"tcp")==0)
      || (strcmp(l->mode,"fork")==0))
      && (d->pid>1))
      {
        // did the child stop ?
        int pid=si_waitpid(d->pid,NULL,WNOHANG);
        if ((pid==0) /* no status change for child*/
        && (kill(d->pid,0)==0)) // child is still running
        {
          struct timespec t;
          struct timespec rem;
          // wait 60 sec
          for(int i=0;i<50;i++)
          {
            // wait till signal or 100ms:
            t.tv_sec=0;
            t.tv_nsec=100000000; // <=100 ms
            nanosleep(&t, &rem);
            // child finished ?
            if (si_waitpid(d->pid,NULL,WNOHANG) == d->pid) break;
          }
          if (kill(d->pid,0)==0) // child still exists
          {
            kill(d->pid,SIGTERM);
            t.tv_sec=1;
            t.tv_nsec=0; // <=1000 ms
            nanosleep(&t, &rem);
            si_waitpid(d->pid,NULL,WNOHANG);
          }
        }
      }
      if ((strcmp(l->mode,"tcp")==0)
      || (strcmp(l->mode,"fork")==0))
      {
        link_list hh=ssiToBeClosed;
        if (hh!=NULL)
        {
          if (hh->l==l)
          {
             ssiToBeClosed=(link_list)hh->next;
             omFreeSize(hh,sizeof(link_struct));
          }
          else while(hh->next!=NULL)
          {
            link_list hhh=(link_list)hh->next;
            if (hhh->l==l)
            {
              hh->next=hhh->next;
              omFreeSize(hhh,sizeof(link_struct));
              break;
            }
            else
              hh=(link_list)hh->next;
          }
        }
      }
#endif
      omFreeSize((ADDRESS)d,(sizeof *d));
    }
    l->data=NULL;
  }
  return FALSE;
}

//**************************************************************************/
leftv ssiRead1(si_link l)
{
  ssiInfo *d = (ssiInfo *)l->data;
  leftv res=(leftv)omAlloc0Bin(sleftv_bin);
  int t=0;
  t=s_readint(d->f_read);
  //Print("got type %d\n",t);
  switch(t)
  {
    case 1:res->rtyp=INT_CMD;
           res->data=(char *)(long)ssiReadInt(d);
           //Print("int: %d\n",(int)(long)res->data);
           break;
    case 2:res->rtyp=STRING_CMD;
           res->data=(char *)ssiReadString(d);
           //Print("str: %s\n",(char*)res->data);
           break;
    case 3:res->rtyp=NUMBER_CMD;
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char *)ssiReadNumber(d);
           //Print("number\n");
           break;
    case 4:res->rtyp=BIGINT_CMD;
           res->data=(char *)ssiReadBigInt(d);
           //Print("bigint\n");
           break;
    case 15:
    case 5:{
           //Print("ring %d\n",t);
             d->r=ssiReadRing(d);
             if (errorreported) return NULL;
             res->data=(char*)d->r;
             if (d->r!=NULL) rIncRefCnt(d->r);
             res->rtyp=RING_CMD;
             if (t==15) // setring
             {
               if(ssiSetCurrRing(d->r)) { d->r=currRing; }
               omFreeBin(res,sleftv_bin);
               return ssiRead1(l);
             }
           }
           break;
    case 6:res->rtyp=POLY_CMD;
           //Print("poly\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadPoly(d);
           break;
    case 7:res->rtyp=IDEAL_CMD;
           //Print("ideal\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadIdeal(d);
           break;
    case 8:res->rtyp=MATRIX_CMD;
           //Print("matrix\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadMatrix(d);
           break;
    case 9:res->rtyp=VECTOR_CMD;
           //Print("vector\n");
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           res->data=(char*)ssiReadPoly(d);
           break;
    case 10:
    case 22:if (t==22) res->rtyp=SMATRIX_CMD;
           else        res->rtyp=MODUL_CMD;
           //Print("module/smatrix %d\n",t);
           if (d->r==NULL) goto no_ring;
           ssiCheckCurrRing(d->r);
           {
             int rk=s_readint(d->f_read);
             ideal M=ssiReadIdeal(d);
             M->rank=rk;
             res->data=(char*)M;
           }
           break;
    case 11:
           {
           //Print("cmd\n",t);
             res->rtyp=COMMAND;
             res->data=ssiReadCommand(l);
             int nok=res->Eval();
             if (nok) WerrorS("error in eval");
             break;
           }
    case 12: /*DEF_CMD*/
           {
           //Print("def\n",t);
             res->rtyp=0;
             res->name=(char *)ssiReadString(d);
             int nok=res->Eval();
             if (nok) WerrorS("error in name lookup");
             break;
           }
    case 13: res->rtyp=PROC_CMD;
             res->data=ssiReadProc(d);
             break;
    case 14: res->rtyp=LIST_CMD;
             res->data=ssiReadList(l);
             break;
    case 16: res->rtyp=NONE; res->data=NULL;
             break;
    case 17: res->rtyp=INTVEC_CMD;
             res->data=ssiReadIntvec(d);
             break;
    case 18: res->rtyp=INTMAT_CMD;
             res->data=ssiReadIntmat(d);
             break;
    case 19: res->rtyp=BIGINTMAT_CMD;
             res->data=ssiReadBigintmat(d);
             break;
    case 20: ssiReadBlackbox(res,l);
             break;
    case 21: ssiReadAttrib(res,l);
             break;
    case 23: ssiReadRingProperties(l);
             return ssiRead1(l);
             break;
    case 24: res->rtyp=BIGINTVEC_CMD;
             res->data=ssiReadBigintvec(d);
             break;
    case 25: res->rtyp=HTABLE_CMD;
             res->data=ssiReadHTable(l);
             if (res->data==NULL)
             {
               omFreeBin(res,sleftv_bin);
               return NULL;
             }
             break;
    // ------------
    case 98: // version
             {
                int n98_v,n98_m;
                BITSET n98_o1,n98_o2;
                n98_v=s_readint(d->f_read);
                n98_m=s_readint(d->f_read);
                n98_o1=s_readint(d->f_read);
                n98_o2=s_readint(d->f_read);
                if ((n98_v!=SSI_VERSION) ||(n98_m!=MAX_TOK))
                {
                  Print("incompatible versions of ssi: %d/%d vs %d/%d\n",
                                  SSI_VERSION,MAX_TOK,n98_v,n98_m);
                }
                #ifndef SING_NDEBUG
                if (TEST_OPT_DEBUG)
                  Print("// opening ssi-%d, MAX_TOK=%d\n",n98_v,n98_m);
                #endif
                si_opt_1=n98_o1;
                si_opt_2=n98_o2;
                omFreeBin(res,sleftv_bin);
                return ssiRead1(l);
             }
    case 99: omFreeBin(res,sleftv_bin); ssiClose(l); m2_end(-1);
             break; /*to make compiler happy*/
    case 0: if (s_iseof(d->f_read))
            {
              ssiClose(l);
            }
            res->rtyp=DEF_CMD;
            break;
    default: Werror("not implemented (t:%d)",t);
             omFreeBin(res,sleftv_bin);
             res=NULL;
             break;
  }
  // if currRing is required for the result, but lost
  // define "ssiRing%d" as currRing:
  if ((d->r!=NULL)
  && (currRing!=d->r)
  && (res->Typ()!=HTABLE_CMD)
  && (res->RingDependend()))
  {
    if(ssiSetCurrRing(d->r)) { d->r=currRing; }
  }
  return res;
no_ring: WerrorS("no ring");
  omFreeBin(res,sleftv_bin);
  return NULL;
}
leftv ssiRead1_S(char**s, const ring R)
{
  leftv res=(leftv)omAlloc0Bin(sleftv_bin);
  int t=0;
  t=s_readint_S(s);
  switch(t)
  {
    case 1:res->rtyp=INT_CMD;
           res->data=(char *)(long)ssiReadInt_S(s);
           break;
    case 2:res->rtyp=STRING_CMD;
           res->data=(char *)ssiReadString_S(s);
           break;
    case 3:res->rtyp=NUMBER_CMD;
           res->data=(char *)ssiReadNumber_CF_S(s,R->cf);
           break;
    case 4:res->rtyp=BIGINT_CMD;
           res->data=(char *)ssiReadBigInt_S(s);
           //Print("bigint\n");
           break;
    case 15:
    case 5:{
             //Print("ring %d\n",t);
             ring r=ssiReadRing_R_S(s);
             if (errorreported||(r==NULL)) return NULL;
             res->rtyp=RING_CMD;
             res->data=(char*)r;
             if (/*(t==15)&&*/
             ((currRing==NULL)||(!rSamePolyRep(r,currRing))))
             {
               rChangeCurrRing(r);
             }
             break;
           }
    case 6:res->rtyp=POLY_CMD;
           res->data=(char*)ssiReadPoly_R_S(s,R);
           break;
    case 7:res->rtyp=IDEAL_CMD;
           res->data=(char*)ssiReadIdeal_R_S(s,R);
           break;
    case 8:res->rtyp=MATRIX_CMD;
           res->data=(char*)ssiReadMatrix_R_S(s,R);
           break;
    case 9:res->rtyp=VECTOR_CMD;
           res->data=(char*)ssiReadPoly_R_S(s,R);
           break;
    case 10:
    case 22:if (t==22) res->rtyp=SMATRIX_CMD;
           else        res->rtyp=MODUL_CMD;
           {
             int rk=s_readint_S(s);
             ideal M=ssiReadIdeal_R_S(s,R);
             M->rank=rk;
             res->data=(char*)M;
           }
           break;
    #if 0
    case 11:
           {
             res->rtyp=COMMAND;
             res->data=ssiReadCommand(l);
             int nok=res->Eval();
             if (nok) WerrorS("error in eval");
             break;
           }
    #endif
    case 12: /*DEF_CMD*/
           {
             res->rtyp=0;
             res->name=(char *)ssiReadString_S(s);
             int nok=res->Eval();
             if (nok) WerrorS("error in name lookup");
             break;
           }
    case 13: res->rtyp=PROC_CMD;
             res->data=ssiReadProc_S(s);
             break;
    case 14: res->rtyp=LIST_CMD;
             res->data=ssiReadList_S(s,R);
             break;
    case 16: res->rtyp=NONE; res->data=NULL;
             break;
    case 17: res->rtyp=INTVEC_CMD;
             res->data=ssiReadIntvec_S(s);
             break;
    case 18: res->rtyp=INTMAT_CMD;
             res->data=ssiReadIntmat_S(s);
             break;
    case 19: res->rtyp=BIGINTMAT_CMD;
             res->data=ssiReadBigintmat_S(s);
             break;
    #if 0
    case 20: ssiReadBlackbox(res,l);
             break;
    case 21: ssiReadAttrib(res,l);
             break;
    case 23: ssiReadRingProperties(l);
             return ssiRead1(l);
             break;
    #endif
    case 24: res->rtyp=BIGINTVEC_CMD;
             res->data=ssiReadBigintvec_S(s);
             break;
    // ------------
    case 98: // version
             {
                int n98_v,n98_m;
                BITSET n98_o1,n98_o2;
                n98_v=s_readint_S(s);
                n98_m=s_readint_S(s);
                n98_o1=s_readint_S(s);
                n98_o2=s_readint_S(s);
                Print("// version ssi-%d, MAX_TOK=%d\n",n98_v,n98_m);
                si_opt_1=n98_o1;
                si_opt_2=n98_o2;
                omFreeBin(res,sleftv_bin);
                return ssiRead1_S(s,R);
             }
    #if 0
    case 99: omFreeBin(res,sleftv_bin); ssiClose(l); m2_end(-1);
             break; /*to make compiler happy*/
    #endif
    case 0: res->rtyp=DEF_CMD;
            **s='\0'; /* unkown char?*/
            break;
    default: Werror("not implemented (t:%d)",t);
             omFreeBin(res,sleftv_bin);
             res=NULL;
             break;
  }
  while((**s!='\0') &&(**s<=' ')) (*s)++;
  if (**s>' ') res->next=ssiRead1_S(s,R);
  return res;
}
//**************************************************************************/
static BOOLEAN ssiSetRing(si_link l, ring r, BOOLEAN send)
{
  if(SI_LINK_W_OPEN_P(l)==0)
     if (slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL)) return TRUE;
  ssiInfo *d = (ssiInfo *)l->data;
  if (d->r!=r)
  {
    if (send)
    {
      fputs("15 ",d->f_write);
      ssiWriteRing(d,r);
    }
    d->r=r;
  }
  if (currRing!=r) rChangeCurrRing(r);
  return FALSE;
}
//**************************************************************************/

BOOLEAN ssiWrite(si_link l, leftv data)
{
  if(SI_LINK_W_OPEN_P(l)==0)
     if (slOpen(l,SI_LINK_OPEN|SI_LINK_WRITE,NULL)) return TRUE;
  if (strcmp(l->mode,"string")==0) return TRUE;
  ssiInfo *d = (ssiInfo *)l->data;
  d->level++;
  //FILE *fich=d->f;
  while (data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();
    attr *aa=data->Attribute();
    if ((aa!=NULL) && ((*aa)!=NULL)) // n user attributes
    {
      attr a=*aa;
      int n=0;
      while(a!=NULL) { n++; a=a->next;}
      fprintf(d->f_write,"21 %d %d ",data->flag,n);
    }
    else if (data->flag!=0) // only "flag" attributes
    {
      fprintf(d->f_write,"21 %d 0 ",data->flag);
    }
    if ((dd==NULL) && (data->name!=NULL) && (tt==0)) tt=DEF_CMD;
      // return pure undefined names as def

    switch(tt /*data->Typ()*/)
    {
          case 0: /*error*/
          case NONE/* nothing*/:fputs("16 ",d->f_write);
                          break;
          case STRING_CMD: fputs("2 ",d->f_write);
                           ssiWriteString(d,(char *)dd);
                           break;
          case INT_CMD: fputs("1 ",d->f_write);
                        ssiWriteInt(d,(int)(long)dd);
                        break;
          case BIGINT_CMD:fputs("4 ",d->f_write);
                        ssiWriteBigInt(d,(number)dd);
                        break;
          case NUMBER_CMD:
                          if (d->r!=currRing)
                          {
                            fputs("15 ",d->f_write);
                            ssiWriteRing(d,currRing);
                            if (d->level<=1) fputc('\n',d->f_write);
                          }
                          fputs("3 ",d->f_write);
                          ssiWriteNumber(d,(number)dd);
                        break;
          case RING_CMD:fputs("5 ",d->f_write);
                        ssiWriteRing(d,(ring)dd);
                        break;
          case BUCKET_CMD:
                        {
                          sBucket_pt b=(sBucket_pt)dd;
                          if (d->r!=sBucketGetRing(b))
                          {
                            fputs("15 ",d->f_write);
                            ssiWriteRing(d,sBucketGetRing(b));
                            if (d->level<=1) fputc('\n',d->f_write);
                          }
                          fputs("6 ",d->f_write);
                          ssiWritePoly(d,sBucketPeek(b));
                          break;
                        }
          case POLY_CMD:
          case VECTOR_CMD:
                        if (d->r!=currRing)
                        {
                          fputs("15 ",d->f_write);
                          ssiWriteRing(d,currRing);
                          if (d->level<=1) fputc('\n',d->f_write);
                        }
                        if(tt==POLY_CMD) fputs("6 ",d->f_write);
                        else             fputs("9 ",d->f_write);
                        ssiWritePoly(d,(poly)dd);
                        break;
          case IDEAL_CMD:
          case MODUL_CMD:
          case MATRIX_CMD:
          case SMATRIX_CMD:
                        if (d->r!=currRing)
                        {
                          fputs("15 ",d->f_write);
                          ssiWriteRing(d,currRing);
                          if (d->level<=1) fputc('\n',d->f_write);
                        }
                        if(tt==IDEAL_CMD)       fputs("7 ",d->f_write);
                        else if(tt==MATRIX_CMD) fputs("8 ",d->f_write);
                        else /* tt==MODUL_CMD, SMATRIX_CMD*/
                        {
                          ideal M=(ideal)dd;
                          if (tt==MODUL_CMD)
                            fprintf(d->f_write,"10 %d ",(int)M->rank);
                          else /*(tt==SMATRIX_CMD)*/
                            fprintf(d->f_write,"22 %d ",(int)M->rank);
                        }
                        ssiWriteIdeal(d,tt,(ideal)dd);
                        break;
          case COMMAND:
                   fputs("11 ",d->f_write);
                   ssiWriteCommand(l,(command)dd);
                   break;
          case DEF_CMD: /* not evaluated stuff in quotes */
                   fputs("12 ",d->f_write);
                   ssiWriteString(d,data->Name());
                   break;
          case PROC_CMD:
                   fputs("13 ",d->f_write);
                   ssiWriteProc(d,(procinfov)dd);
                   break;
          case LIST_CMD:
                   fputs("14 ",d->f_write);
                   ssiWriteList(l,(lists)dd);
                   break;
          case HTABLE_CMD:
                   fputs("25 ",d->f_write);
                   if (ssiWriteHTable(l,(stablerec*)dd))
                   {
                     d->level=0;
                     return TRUE;
                   }
                   break;
          case INTVEC_CMD:
                   fputs("17 ",d->f_write);
                   ssiWriteIntvec(d,(intvec *)dd);
                   break;
          case INTMAT_CMD:
                   fputs("18 ",d->f_write);
                   ssiWriteIntmat(d,(intvec *)dd);
                   break;
          case BIGINTMAT_CMD:
                   fputs("19 ",d->f_write);
                   ssiWriteBigintmat(d,(bigintmat *)dd);
                   break;
          case BIGINTVEC_CMD:
                   fputs("24 ",d->f_write);
                   ssiWriteBigintvec(d,(bigintmat *)dd);
                   break;
          default:
            if (tt>MAX_TOK)
            {
              blackbox *b=getBlackboxStuff(tt);
              fputs("20 ",d->f_write);
              b->blackbox_serialize(b,dd,l);
            }
            else
            {
              Werror("not implemented (t:%d, rtyp:%d)",tt, data->rtyp);
              d->level=0;
              return TRUE;
            }
            break;
    }
    if (d->level<=1) { fputc('\n',d->f_write); fflush(d->f_write); }
    data=data->next;
  }
  d->level--;
  return FALSE;
}
void ssiWrite_S(leftv data,const ring R)
{
  while(data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();
    switch(tt /*data->Typ()*/)
    {
      case 0: /*error*/
      case NONE/* nothing*/:StringAppendS("16 ");
           break;
      case INT_CMD: StringAppendS("1 ");
                          ssiWriteInt_S((int)(long)dd);
                          break;
      case STRING_CMD: StringAppendS("2 ");
                             ssiWriteString_S((char *)dd);
                             break;
      case BIGINT_CMD:StringAppendS("4 ");
                          ssiWriteBigInt_S((number)dd);
                          break;
      case NUMBER_CMD:
        StringAppendS("3 ");
        ssiWriteNumber_CF_S((number)dd,R->cf);
        break;
      case RING_CMD:StringAppendS("5 ");
                          ssiWriteRing_R_S((ring)dd);
                          break;
      case BUCKET_CMD:
                          {
                            sBucket_pt b=(sBucket_pt)dd;
                            StringAppendS("6 ");
                            ssiWritePoly_R_S(sBucketPeek(b),R);
                            break;
                          }
      case POLY_CMD:
      case VECTOR_CMD:
        if(tt==POLY_CMD) StringAppendS("6 ");
        else             StringAppendS("9 ");
        ssiWritePoly_R_S((poly)dd,R);
        break;
      case IDEAL_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
      case SMATRIX_CMD:
        if(tt==IDEAL_CMD)       StringAppendS("7 ");
        else if(tt==MATRIX_CMD) StringAppendS("8 ");
        else /* tt==MODUL_CMD, SMATRIX_CMD*/
        {
          ideal M=(ideal)dd;
          if (tt==MODUL_CMD)
          {
            StringAppendS("10 ");StringAppend("%d ",(int)M->rank);
          }
          else /*(tt==SMATRIX_CMD)*/
          {
            StringAppendS("22 ");StringAppend("%d ",(int)M->rank);
          }
        }
        ssiWriteIdeal_R_S(tt,(ideal)dd,R);
        break;
      #if 0
      case COMMAND:
                     fputs("11 ",d->f_write);
                     ssiWriteCommand(l,(command)dd);
                     break;
      #endif
      case DEF_CMD: /* not evaluated stuff in quotes */
                     StringAppendS("12 ");
                     ssiWriteString_S(data->Name());
                     break;
      case PROC_CMD:
                     StringAppendS("13 ");
                     ssiWriteProc_S((procinfov)dd);
                     break;
      case LIST_CMD:
                     StringAppendS("14 ");
                     ssiWriteList_S((lists)dd,R);
                     break;
      case INTVEC_CMD:
                     StringAppendS("17 ");
                     ssiWriteIntvec_S((intvec *)dd);
                     break;
      case INTMAT_CMD:
                     StringAppendS("18 ");
                     ssiWriteIntmat_S((intvec *)dd);
                     break;
      case BIGINTMAT_CMD:
                     StringAppendS("19 ");
                     ssiWriteBigintmat_S((bigintmat *)dd);
                     break;
      case BIGINTVEC_CMD:
                     StringAppendS("24 ");
                     ssiWriteBigintvec_S((bigintmat *)dd);
                     break;
      default:
              #if 0
              if (tt>MAX_TOK)
              {
                blackbox *b=getBlackboxStuff(tt);
                fputs("20 ",d->f_write);
                b->blackbox_serialize(b,dd,l);
              }
              else
              #endif
              {
                Werror("not implemented (t:%d, rtyp:%d)",tt, data->rtyp);
                return;
              }
              break;
    }
    data=data->next;
  }
}

BOOLEAN ssiGetDump(si_link l);
BOOLEAN ssiDump(si_link l);

si_link_extension slInitSsiExtension(si_link_extension s)
{
  s->Open=ssiOpen;
  s->Close=ssiClose;
  s->Kill=ssiClose;
  s->Read=ssiRead1;
  s->Read2=ssiRead2;
  s->Write=ssiWrite;
  s->Dump=ssiDump;
  s->GetDump=ssiGetDump;

  s->Status=slStatusSsi;
  s->SetRing=ssiSetRing;
  s->type="ssi";
  return s;
}

const char* slStatusSsi(si_link l, const char* request)
{
  if (strcmp(l->mode,"string")==0)
  {
    if (strcmp(request, "read") == 0)
    {
      if (SI_LINK_R_OPEN_P(l)) return "yes";
      else return "no";
    }
    if (strcmp(request, "write") == 0)
    {
      if (SI_LINK_W_OPEN_P(l)) return "yes";
      else return "no";
    }
    return "inavlid";
  }
  ssiInfo *d=(ssiInfo*)l->data;
  if (d==NULL)
    return "no";
#ifndef _WIN32
  if (((strcmp(l->mode,"fork")==0)
  ||(strcmp(l->mode,"tcp")==0)
  ||(strcmp(l->mode,"connect")==0))
  && (strcmp(request, "read") == 0))
  {
    if (s_isready(d->f_read)) return "ready";
#if defined(HAVE_POLL)
    pollfd pfd;
    loop
    {
      /* Don't block. Return socket status immediately. */
      pfd.fd=d->fd_read;
      pfd.events=POLLIN;
      //Print("test fd %d\n",d->fd_read);
      /* check with select: chars waiting: no -> not ready */
      switch (si_poll(&pfd,1,0))
      {
        case 0: /* not ready */ return "not ready";
        case -1: /*error*/      return "error";
        case 1: /*ready ? */    break;
      }
#else
    fd_set  mask;
    struct timeval wt;
    if (FD_SETSIZE<=d->fd_read)
    {
      Werror("file descriptor number too high (%d)",d->fd_read);
      return "error";
    }

    loop
    {
      /* Don't block. Return socket status immediately. */
      wt.tv_sec  = 0;
      wt.tv_usec = 0;

      FD_ZERO(&mask);
      FD_SET(d->fd_read, &mask);
      //Print("test fd %d\n",d->fd_read);
      /* check with select: chars waiting: no -> not ready */
      switch (si_select(d->fd_read+1, &mask, NULL, NULL, &wt))
      {
        case 0: /* not ready */ return "not ready";
        case -1: /*error*/      return "error";
        case 1: /*ready ? */    break;
      }
#endif
      /* yes: read 1 char*/
      /* if \n, check again with select else ungetc(c), ready*/
      int c=s_getc(d->f_read);
      //Print("try c=%d\n",c);
      if (c== -1) return "eof"; /* eof or error */
      else if (isdigit(c))
      { s_ungetc(c,d->f_read); return "ready"; }
      else if (c>' ')
      {
        Werror("unknown char in ssiLink(%d)",c);
        return "error";
      }
      /* else: next char */
    }
  }
  else
#endif
  if (strcmp(request, "read") == 0)
  {
    if (SI_LINK_R_OPEN_P(l) && (!s_iseof(d->f_read)) && (s_isready(d->f_read))) return "ready";
    else return "not ready";
  }
  else if (strcmp(request, "write") == 0)
  {
    if (SI_LINK_W_OPEN_P(l)) return "ready";
    else return "not ready";
  }
  else return "unknown status request";
}

int slStatusSsiL(lists L, int timeout, BOOLEAN *ignore)
{
#ifdef _WIN32
  (void)L;
  (void)timeout;
  (void)ignore;
  WerrorS("waiting for ssi process and socket links is not supported on native Windows");
  return -2;
#else
// input: L: a list with links of type
//           ssi-connect, ssi-fork, ssi-tcp, MPtcp-fork or MPtcp-launch.
//           Note: Not every entry in L must be set.
//        timeout: timeout for select in milli-seconds
//           or -1 for infinity
//           or 0 for polling
// returns: ERROR (via Werror): L has wrong elements or link not open
//           -2: error in L
//           -1: the read state of all links is eof
//           0:  timeout (or polling): none ready,
//           i>0: (at least) L[i] is ready
#if defined(HAVE_POLL) && !defined(__APPLE__)
// fd is restricted on OsX by ulimit "file descriptors" (256)
  si_link l;
  ssiInfo *d=NULL;
  int d_fd;
  int s;
  int nfd=L->nr+1;
  pollfd *pfd=(pollfd*)omAlloc0(nfd*sizeof(pollfd));
  for(int i=L->nr; i>=0; i--)
  {
    pfd[i].fd=-1;
    if (L->m[i].Typ()!=DEF_CMD)
    {
      if (L->m[i].Typ()!=LINK_CMD)
      { WerrorS("all elements must be of type link"); return -2;}
      l=(si_link)L->m[i].Data();
      if(SI_LINK_OPEN_P(l)==0)
      { WerrorS("all links must be open"); return -2;}
      if (((strcmp(l->m->type,"ssi")!=0) && (strcmp(l->m->type,"MPtcp")!=0))
      || ((strcmp(l->mode,"fork")!=0) && (strcmp(l->mode,"tcp")!=0)
        && (strcmp(l->mode,"launch")!=0) && (strcmp(l->mode,"connect")!=0)))
      {
        WerrorS("all links must be of type ssi:fork, ssi:tcp, ssi:connect");
        return -2;
      }
      if (strcmp(l->m->type,"ssi")==0)
      {
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if (!s_isready(d->f_read))
        {
          pfd[i].fd=d_fd;
          pfd[i].events=POLLIN;
        }
        else
        {
          return i+1;
        }
      }
      else
      {
        Werror("wrong link type >>%s<<",l->m->type);
        return -2;
      }
    }
    else if (ignore!=NULL)
    {
      ignore[i]=TRUE; // not a link
    }
  }
  s=si_poll(pfd,nfd,timeout);
  if (s==-1)
  {
    Werror("error in poll call (errno:%d)",errno);
    return -2; /*error*/
  }
  if(s==0)
  {
    return 0; /*timeout*/
  }
  for(int i=L->nr; i>=0; i--)
  {
    if ((L->m[i].rtyp==LINK_CMD)
    && ((ignore==NULL)||(ignore[i]==FALSE)))
    {
      // the link type is ssi, that's already tested
      l=(si_link)L->m[i].Data();
      d=(ssiInfo*)l->data;
      d_fd=d->fd_read;
      if (pfd[i].fd==d_fd)
      {
        if (pfd[i].revents &POLLIN)
        {
          omFree(pfd);
          return i+1;
        }
      }
    }
  }
  // no ready
  return 0;
#else
  // fd is restricted to <=1024
  si_link l;
  ssiInfo *d=NULL;
  int d_fd;
  fd_set fdmask;
  FD_ZERO(&fdmask);
  int max_fd=0; /* 1 + max fd in fd_set */

  /* timeout */
  struct timeval wt;
  struct timeval *wt_ptr=&wt;
  int startingtime = getRTimer()/TIMER_RESOLUTION;  // in seconds
  if (timeout== -1)
  {
    wt_ptr=NULL;
  }
  else
  {
    wt.tv_sec  = timeout / 1000;
    wt.tv_usec = (timeout % 1000)*1000;
  }

  /* auxiliary variables */
  int i;
  int j;
  int k;
  int s;
  char fdmaskempty;

  /* check the links and fill in fdmask */
  /* check ssi links for ungetc_buf */
  for(i=L->nr; i>=0; i--)
  {
    if (L->m[i].Typ()!=DEF_CMD)
    {
      if (L->m[i].Typ()!=LINK_CMD)
      { WerrorS("all elements must be of type link"); return -2;}
      l=(si_link)L->m[i].Data();
      if(SI_LINK_OPEN_P(l)==0)
      { WerrorS("all links must be open"); return -2;}
      if (((strcmp(l->m->type,"ssi")!=0) && (strcmp(l->m->type,"MPtcp")!=0))
      || ((strcmp(l->mode,"fork")!=0) && (strcmp(l->mode,"tcp")!=0)
        && (strcmp(l->mode,"launch")!=0) && (strcmp(l->mode,"connect")!=0)))
      {
        WerrorS("all links must be of type ssi:fork, ssi:tcp, ssi:connect");
        return -2;
      }
      if (strcmp(l->m->type,"ssi")==0)
      {
        d=(ssiInfo*)l->data;
        d_fd=d->fd_read;
        if (!s_isready(d->f_read))
        {
          if ((ignore==NULL) || (ignore[i]==FALSE))
          {
            FD_SET(d_fd, &fdmask);
            if (d_fd > max_fd) max_fd=d_fd;
          }
        }
        else
          return i+1;
      }
      else
      {
        Werror("wrong link type >>%s<<",l->m->type);
        return -2;
      }
    }
  }
  max_fd++;
  if (FD_SETSIZE<=max_fd)
  {
    Werror("file descriptor number too high (%d)",max_fd);
    return -2;
  }

  /* check with select: chars waiting: no -> not ready */
  s = si_select(max_fd, &fdmask, NULL, NULL, wt_ptr);
  if (s==-1)
  {
    Werror("error in select call (errno:%d)",errno);
    return -2; /*error*/
  }
  if (s==0)
  {
    return 0; /*poll: not ready */
  }
  else /* s>0, at least one ready  (the number of fd which are ready is s)*/
  {
    j=0;
    while (j<=max_fd) { if (FD_ISSET(j,&fdmask)) break; j++; }
    for(i=L->nr; i>=0; i--)
    {
      if (L->m[i].rtyp==LINK_CMD)
      {
        l=(si_link)L->m[i].Data();
        if (strcmp(l->m->type,"ssi")==0)
        {
          d=(ssiInfo*)l->data;
          d_fd=d->fd_read;
          if(j==d_fd) return i+1;
        }
      }
    }
  }
  return 0;
#endif
#endif
}

int ssiBatch(const char *host, const char * port)
/* return 0 on success, >0 else*/
{
#ifdef _WIN32
  (void)host;
  (void)port;
  WerrorS("ssi socket links are not supported on native Windows");
  return 1;
#else
  si_link l=(si_link) omAlloc0Bin(sip_link_bin);
  char *buf=(char*)omAlloc(256);
  snprintf(buf,256,"ssi:connect %s:%s",host,port);
  slInit(l, buf);
  omFreeSize(buf,256);
  if (slOpen(l,SI_LINK_OPEN,NULL)) return 1;
  SI_LINK_SET_RW_OPEN_P(l);

  idhdl id = enterid("link_ll", 0, LINK_CMD, &IDROOT, FALSE);
  IDLINK(id) = l;

  loop
  {
    leftv h=ssiRead1(l); /*contains an exit.... */
    if (feErrors != NULL && *feErrors != '\0')
    {
      // handle errors:
      PrintS(feErrors); /* currently quite simple */
      *feErrors = '\0';
    }
    ssiWrite(l,h);
    h->CleanUp();
    omFreeBin(h, sleftv_bin);
  }
  /* never reached*/
  _exit(0);
#endif
}

#ifdef _WIN32
int ssiReservePort(int clients)
{
  (void)clients;
  WerrorS("ssi socket links are not supported on native Windows");
  return 0;
}

si_link ssiCommandLink()
{
  WerrorS("ssi socket links are not supported on native Windows");
  return NULL;
}
#else
STATIC_VAR int ssiReserved_P=0;
STATIC_VAR int ssiReserved_sockfd;
STATIC_VAR struct sockaddr_in ssiResverd_serv_addr;
STATIC_VAR int  ssiReserved_Clients;
int ssiReservePort(int clients)
{
  if (ssiReserved_P!=0)
  {
    WerrorS("ERROR already a reserved port requested");
    return 0;
  }
  int portno;
  ssiReserved_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(ssiReserved_sockfd < 0)
  {
    WerrorS("ERROR opening socket");
    return 0;
  }
  memset((char *) &ssiResverd_serv_addr,0, sizeof(ssiResverd_serv_addr));
  portno = 1025;
  ssiResverd_serv_addr.sin_family = AF_INET;
  ssiResverd_serv_addr.sin_addr.s_addr = INADDR_ANY;
  do
  {
    portno++;
    ssiResverd_serv_addr.sin_port = htons(portno);
    if(portno > 50000)
    {
      WerrorS("ERROR on binding (no free port available?)");
      return 0;
    }
  }
  while(bind(ssiReserved_sockfd, (struct sockaddr *) &ssiResverd_serv_addr, sizeof(ssiResverd_serv_addr)) < 0);
  ssiReserved_P=portno;
  listen(ssiReserved_sockfd,clients);
  ssiReserved_Clients=clients;
  return portno;
}

EXTERN_VAR si_link_extension si_link_root;
si_link ssiCommandLink()
{
  if (ssiReserved_P==0)
  {
    WerrorS("ERROR no reserved port requested");
    return NULL;
  }
  struct sockaddr_in cli_addr;
  int clilen = sizeof(cli_addr);
  int newsockfd = si_accept(ssiReserved_sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
  if(newsockfd < 0)
  {
    Werror("ERROR on accept (errno=%d)",errno);
    return NULL;
  }
  si_link l=(si_link) omAlloc0Bin(sip_link_bin);
  si_link_extension s = si_link_root;
  si_link_extension prev = s;
  while (strcmp(s->type, "ssi") != 0)
  {
    if (s->next == NULL)
    {
      prev = s;
      s = NULL;
      break;
    }
    else
    {
      s = s->next;
    }
  }
  if (s != NULL)
    l->m = s;
  else
  {
    si_link_extension ns = (si_link_extension)omAlloc0Bin(s_si_link_extension_bin);
    prev->next=slInitSsiExtension(ns);
    l->m = prev->next;
  }
  l->name=omStrDup("");
  l->mode=omStrDup("tcp");
  l->ref=1;
  ssiInfo *d=(ssiInfo*)omAlloc0(sizeof(ssiInfo));
  l->data=d;
  d->fd_read = newsockfd;
  d->fd_write = newsockfd;
  d->f_read = s_open(newsockfd);
  d->f_write = fdopen(newsockfd, "w");
  SI_LINK_SET_RW_OPEN_P(l);
  ssiReserved_Clients--;
  if (ssiReserved_Clients<=0)
  {
    ssiReserved_P=0;
    si_close(ssiReserved_sockfd);
  }
  return l;
}
#endif
/*---------------------------------------------------------------------*/
/**
 * @brief additional default signal handler

  // some newer Linux version cannot have SIG_IGN for SIGCHLD,
  // so use this nice routine here:
  //  SuSe 9.x reports -1 always
  //  Redhat 9.x/FC x reports sometimes -1
  // see also: hpux_system
  // also needed by getrusage (timer etc.)

 @param[in] sig
**/
/*---------------------------------------------------------------------*/
void sig_chld_hdl(int)
{

#if 0
  pid_t kidpid;
  int status;
  loop
  {
    kidpid = si_waitpid(-1, &status, WNOHANG);
    if (kidpid==-1)
    {
      /* continue on interruption (EINTR): */
      if (errno == EINTR) continue;
      /* break on anything else (EINVAL or ECHILD according to manpage): */
      break;
    }
    else if (kidpid==0) break; /* no more children to process, so break */

    //printf("Child %ld terminated\n", kidpid);
    link_list hh=ssiToBeClosed;
    while((hh!=NULL)&&(ssiToBeClosed_inactive))
    {
      if((hh->l!=NULL) && (hh->l->m->Open==ssiOpen))
      {
        ssiInfo *d = (ssiInfo *)hh->l->data;
        if(d->pid==kidpid)
        {
          if(ssiToBeClosed_inactive)
          {
            ssiToBeClosed_inactive=FALSE;
            slClose(hh->l);
            ssiToBeClosed_inactive=TRUE;
            break;
          }
          else break;
        }
        else hh=(link_list)hh->next;
      }
      else hh=(link_list)hh->next;
    }
  }
#endif
}

static BOOLEAN DumpSsiIdhdl(si_link l, idhdl h)
{
  int type_id = IDTYP(h);

  // C-proc not to be dumped, also LIB-proc not
  if (type_id == PROC_CMD)
  {
    if (IDPROC(h)->language == LANG_C) return FALSE;
    if (IDPROC(h)->libname != NULL) return FALSE;
  }
  // do not dump links
  if (type_id == LINK_CMD) return FALSE;

  // do not dump ssi internal rings: ssiRing*
  if ((type_id == RING_CMD) && (strncmp(IDID(h),"ssiRing",7)==0))
    return FALSE;

  // do not dump default cring:
  if (type_id == CRING_CMD)
  {
    if (strcmp(IDID(h),"ZZ")==0) return FALSE;
    if (strcmp(IDID(h),"QQ")==0) return FALSE;
    #ifdef SINGULAR_4_2
    if (strcmp(IDID(h),"AE")==0) return FALSE;
    if (strcmp(IDID(h),"QAE")==0) return FALSE;
    #endif
  }

  command D=(command)omAlloc0(sizeof(*D));
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=COMMAND;
  tmp.data=D;

  if (type_id == PACKAGE_CMD)
  {
    // do not dump Top, Standard
    if ((strcmp(IDID(h), "Top") == 0)
    || (strcmp(IDID(h), "Standard") == 0))
    {
      omFreeSize(D,sizeof(*D));
      return FALSE;
    }
    package p=(package)IDDATA(h);
    // dump Singular-packages as LIB("...");
    if (p->language==LANG_SINGULAR)
    {
      D->op=LOAD_CMD;
      D->argc=2;
      D->arg1.rtyp=STRING_CMD;
      D->arg1.data=p->libname;
      D->arg2.rtyp=STRING_CMD;
      D->arg2.data=(char*)"with";
      ssiWrite(l,&tmp);
      omFreeSize(D,sizeof(*D));
      return FALSE;
    }
    // dump Singular-packages as load("...");
    else if (p->language==LANG_C)
    {
      D->op=LOAD_CMD;
      D->argc=1;
      D->arg1.rtyp=STRING_CMD;
      D->arg1.data=p->libname;
      ssiWrite(l,&tmp);
      omFreeSize(D,sizeof(*D));
      return FALSE;
    }
  }

  // put type and name
  //Print("generic dump:%s,%s\n",IDID(h),Tok2Cmdname(IDTYP(h)));
  D->op='=';
  D->argc=2;
  D->arg1.rtyp=DEF_CMD;
  D->arg1.name=IDID(h);
  D->arg2.rtyp=IDTYP(h);
  D->arg2.data=IDDATA(h);
  ssiWrite(l,&tmp);
  omFreeSize(D,sizeof(*D));
  return FALSE;
}
static BOOLEAN ssiDumpIter(si_link l, idhdl h)
{
  if (h == NULL) return FALSE;

  if (ssiDumpIter(l, IDNEXT(h))) return TRUE;

  // need to set the ring before writing it, otherwise we get in
  // trouble with minpoly
  if (IDTYP(h) == RING_CMD)
    rSetHdl(h);

  if (DumpSsiIdhdl(l, h)) return TRUE;

  // do not dump ssi internal rings: ssiRing*
  // but dump objects of all other rings
  if ((IDTYP(h) == RING_CMD)
  && (strncmp(IDID(h),"ssiRing",7)!=0))
    return ssiDumpIter(l, IDRING(h)->idroot);
  else
    return FALSE;
}
BOOLEAN ssiDump(si_link l)
{
  if (strcmp(l->mode,"string")==0)
  {
    WerrorS("no dump for ssi:string");
    return TRUE;
  }
  idhdl h = IDROOT, rh = currRingHdl;
  BOOLEAN status = ssiDumpIter(l, h);

  //if (! status ) status = DumpAsciiMaps(fd, h, NULL);

  if (currRingHdl != rh) rSetHdl(rh);
  //fprintf(fd, "option(set, intvec(%d, %d));\n", si_opt_1, si_opt_2);

  return status;
}
BOOLEAN ssiGetDump(si_link l)
{
  if (strcmp(l->mode,"string")==0)
  {
    WerrorS("no dump for ssi:string");
    return TRUE;
  }
  ssiInfo *d=(ssiInfo*)l->data;
  loop
  {
    if (!SI_LINK_OPEN_P(l)) break;
    if (s_iseof(d->f_read)) break;
    leftv h=ssiRead1(l); /*contains an exit.... */
    if (feErrors != NULL && *feErrors != '\0')
    {
      // handle errors:
      PrintS(feErrors); /* currently quite simple */
      return TRUE;
      *feErrors = '\0';
    }
    h->CleanUp();
    omFreeBin(h, sleftv_bin);
  }
  return FALSE;
}

void singular_close_links()
{
  link_list hh=ssiToBeClosed;
  while(hh!=NULL)
  {
    if ((hh->l->m!=NULL)
    && (hh->l->m->Open==ssiOpen)
    && SI_LINK_OPEN_P(hh->l)
    && (strcmp(hh->l->mode, "fork")==0))
    {
      SI_LINK_SET_CLOSE_P(hh->l);
      ssiInfo *d = (ssiInfo *)hh->l->data;
      if (d->f_read!=NULL) { s_close(d->f_read);d->f_read=NULL;}
      if (d->f_write!=NULL) { fclose(d->f_write); d->f_write=NULL; }
    }
    hh=(link_list)hh->next;
  }
  ssiToBeClosed=NULL;
}

BOOLEAN ssiWrite2(si_link l, leftv res, leftv u)
{
  if((strcmp(l->mode,"string")==0)
  &&(u->Typ()==STRING_CMD))
  {
    StringSetS("");
    ssiWrite_S(u, currRing);
    res->data=(void*)StringEndS();
    res->rtyp=STRING_CMD;
    return res->data==NULL;
  }
  return TRUE;
}
leftv ssiRead2(si_link l, leftv u)
{
  if((strcmp(l->mode,"string")==0)
  &&(u->Typ()==STRING_CMD))
  {
    char *s=(char*)u->Data();
    return ssiRead1_S(&s,currRing);
  }
 return NULL;
}
// ----------------------------------------------------------------
// format
// 1 int %d
// 2 string <len> %s
// 3 number
// 4 bigint 4 %d or 3 <mpz_t>
// 5 ring
// 6 poly
// 7 ideal
// 8 matrix
// 9 vector
// 10 module
// 11 command
// 12 def <len> %s
// 13 proc <len> %s
// 14 list %d <elem1> ....
// 15 setring .......
// 16 nothing
// 17 intvec <len> ...
// 18 intmat
// 19 bigintmat <r> <c> ...
// 20 blackbox <name> <len> ...
// 21 attrib <bit-attrib> <len> <a-name1> <val1>... <data>
// 22 smatrix
// 23 0 <log(bitmask)> ring properties: max.exp.
// 23 1 <log(bitmask)> <r->IsLPRing> ring properties:LPRing
// 23 2 <matrix C> <matrix D> ring properties: PLuralRing
// 24 bigintvec <c>
// 25 htable <len> <key1> <value1> ...
//
// 98: verify version: <ssi-version> <MAX_TOK> <OPT1> <OPT2>
// 99: quit Singular
