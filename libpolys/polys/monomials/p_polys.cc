/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.cc
 *  Purpose: implementation of ring independent poly procedures?
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *******************************************************************/

#include <ctype.h>

#include "misc/auxiliary.h"

#include "misc/options.h"
#include "misc/intvec.h"


#include "coeffs/longrat.h" // snumber is needed...
#include "coeffs/numbers.h" // ndCopyMap

#include "polys/PolyEnumerator.h"

#define TRANSEXT_PRIVATES

#include "polys/ext_fields/transext.h"
#include "polys/ext_fields/algext.h"

#include "polys/weight.h"
#include "polys/simpleideals.h"

#include "ring.h"
#include "p_polys.h"

#include "polys/templates/p_MemCmp.h"
#include "polys/templates/p_MemAdd.h"
#include "polys/templates/p_MemCopy.h"


#ifdef HAVE_PLURAL
#include "nc/nc.h"
#include "nc/sca.h"
#endif

#ifdef HAVE_SHIFTBBA
#include "polys/shiftop.h"
#endif

#include "clapsing.h"

/*
 * lift ideal with coeffs over Z (mod N) to Q via Farey
 */
poly p_Farey(poly p, number N, const ring r)
{
  poly h=p_Copy(p,r);
  poly hh=h;
  while(h!=NULL)
  {
    number c=pGetCoeff(h);
    pSetCoeff0(h,n_Farey(c,N,r->cf));
    n_Delete(&c,r->cf);
    pIter(h);
  }
  while((hh!=NULL)&&(n_IsZero(pGetCoeff(hh),r->cf)))
  {
    p_LmDelete(&hh,r);
  }
  h=hh;
  while((h!=NULL) && (pNext(h)!=NULL))
  {
    if(n_IsZero(pGetCoeff(pNext(h)),r->cf))
    {
      p_LmDelete(&pNext(h),r);
    }
    else pIter(h);
  }
  return hh;
}
/*2
* xx,q: arrays of length 0..rl-1
* xx[i]: SB mod q[i]
* assume: char=0
* assume: q[i]!=0
* destroys xx
*/
poly p_ChineseRemainder(poly *xx, number *x,number *q, int rl, CFArray &inv_cache,const ring R)
{
  poly r,h,hh;
  int j;
  poly  res_p=NULL;
  loop
  {
    /* search the lead term */
    r=NULL;
    for(j=rl-1;j>=0;j--)
    {
      h=xx[j];
      if ((h!=NULL)
      &&((r==NULL)||(p_LmCmp(r,h,R)==-1)))
        r=h;
    }
    /* nothing found -> return */
    if (r==NULL) break;
    /* create the monomial in h */
    h=p_Head(r,R);
    /* collect the coeffs in x[..]*/
    for(j=rl-1;j>=0;j--)
    {
      hh=xx[j];
      if ((hh!=NULL) && (p_LmCmp(h,hh,R)==0))
      {
        x[j]=pGetCoeff(hh);
        hh=p_LmFreeAndNext(hh,R);
        xx[j]=hh;
      }
      else
        x[j]=n_Init(0, R->cf);
    }
    number n=n_ChineseRemainderSym(x,q,rl,TRUE,inv_cache,R->cf);
    for(j=rl-1;j>=0;j--)
    {
      x[j]=NULL; // n_Init(0...) takes no memory
    }
    if (n_IsZero(n,R->cf)) p_Delete(&h,R);
    else
    {
      //Print("new mon:");pWrite(h);
      p_SetCoeff(h,n,R);
      pNext(h)=res_p;
      res_p=h; // building res_p in reverse order!
    }
  }
  res_p=pReverse(res_p);
  p_Test(res_p, R);
  return res_p;
}
/***************************************************************
 *
 * Completing what needs to be set for the monomial
 *
 ***************************************************************/
// this is special for the syz stuff
STATIC_VAR int* _components = NULL;
STATIC_VAR long* _componentsShifted = NULL;
STATIC_VAR int _componentsExternal = 0;

VAR BOOLEAN pSetm_error=0;

#ifndef SING_NDEBUG
# define MYTEST 0
#else /* ifndef SING_NDEBUG */
# define MYTEST 0
#endif /* ifndef SING_NDEBUG */

void p_Setm_General(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  int pos=0;
  if (r->typ!=NULL)
  {
    loop
    {
      unsigned long ord=0;
      sro_ord* o=&(r->typ[pos]);
      switch(o->ord_typ)
      {
        case ro_dp:
        {
          int a,e;
          a=o->data.dp.start;
          e=o->data.dp.end;
          for(int i=a;i<=e;i++) ord+=p_GetExp(p,i,r);
          p->exp[o->data.dp.place]=ord;
          break;
        }
        case ro_wp_neg:
          ord=POLY_NEGWEIGHT_OFFSET;
          // no break;
        case ro_wp:
        {
          int a,e;
          a=o->data.wp.start;
          e=o->data.wp.end;
          int *w=o->data.wp.weights;
#if 1
          for(int i=a;i<=e;i++) ord+=((unsigned long)p_GetExp(p,i,r))*((unsigned long)w[i-a]);
#else
          long ai;
          int ei,wi;
          for(int i=a;i<=e;i++)
          {
             ei=p_GetExp(p,i,r);
             wi=w[i-a];
             ai=ei*wi;
             if (ai/ei!=wi) pSetm_error=TRUE;
             ord+=ai;
             if (ord<ai) pSetm_error=TRUE;
          }
#endif
          p->exp[o->data.wp.place]=ord;
          break;
        }
        case ro_am:
        {
          ord = POLY_NEGWEIGHT_OFFSET;
          const short a=o->data.am.start;
          const short e=o->data.am.end;
          const int * w=o->data.am.weights;
#if 1
          for(short i=a; i<=e; i++, w++)
            ord += ((*w) * p_GetExp(p,i,r));
#else
          long ai;
          int ei,wi;
          for(short i=a;i<=e;i++)
          {
             ei=p_GetExp(p,i,r);
             wi=w[i-a];
             ai=ei*wi;
             if (ai/ei!=wi) pSetm_error=TRUE;
             ord += ai;
             if (ord<ai) pSetm_error=TRUE;
          }
#endif
          const int c = p_GetComp(p,r);

          const short len_gen= o->data.am.len_gen;

          if ((c > 0) && (c <= len_gen))
          {
            assume( w == o->data.am.weights_m );
            assume( w[0] == len_gen );
            ord += w[c];
          }

          p->exp[o->data.am.place] = ord;
          break;
        }
      case ro_wp64:
        {
          int64 ord=0;
          int a,e;
          a=o->data.wp64.start;
          e=o->data.wp64.end;
          int64 *w=o->data.wp64.weights64;
          int64 ei,wi,ai;
          for(int i=a;i<=e;i++)
          {
            //Print("exp %d w %d \n",p_GetExp(p,i,r),(int)w[i-a]);
            //ord+=((int64)p_GetExp(p,i,r))*w[i-a];
            ei=(int64)p_GetExp(p,i,r);
            wi=w[i-a];
            ai=ei*wi;
            if(ei!=0 && ai/ei!=wi)
            {
              pSetm_error=TRUE;
              #if SIZEOF_LONG == 4
              Print("ai %lld, wi %lld\n",ai,wi);
              #else
              Print("ai %ld, wi %ld\n",ai,wi);
              #endif
            }
            ord+=ai;
            if (ord<ai)
            {
              pSetm_error=TRUE;
              #if SIZEOF_LONG == 4
              Print("ai %lld, ord %lld\n",ai,ord);
              #else
              Print("ai %ld, ord %ld\n",ai,ord);
              #endif
            }
          }
          int64 mask=(int64)0x7fffffff;
          long a_0=(long)(ord&mask); //2^31
          long a_1=(long)(ord >>31 ); /*(ord/(mask+1));*/

          //Print("mask: %x,  ord: %d,  a_0: %d,  a_1: %d\n"
          //,(int)mask,(int)ord,(int)a_0,(int)a_1);
                    //Print("mask: %d",mask);

          p->exp[o->data.wp64.place]=a_1;
          p->exp[o->data.wp64.place+1]=a_0;
//            if(p_Setm_error) PrintS("***************************\n"
//                                    "***************************\n"
//                                    "**WARNING: overflow error**\n"
//                                    "***************************\n"
//                                    "***************************\n");
          break;
        }
        case ro_cp:
        {
          int a,e;
          a=o->data.cp.start;
          e=o->data.cp.end;
          int pl=o->data.cp.place;
          for(int i=a;i<=e;i++) { p->exp[pl]=p_GetExp(p,i,r); pl++; }
          break;
        }
        case ro_syzcomp:
        {
          long c=__p_GetComp(p,r);
          long sc = c;
          int* Components = (_componentsExternal ? _components :
                             o->data.syzcomp.Components);
          long* ShiftedComponents = (_componentsExternal ? _componentsShifted:
                                     o->data.syzcomp.ShiftedComponents);
          if (ShiftedComponents != NULL)
          {
            assume(Components != NULL);
            assume(c == 0 || Components[c] != 0);
            sc = ShiftedComponents[Components[c]];
            assume(c == 0 || sc != 0);
          }
          p->exp[o->data.syzcomp.place]=sc;
          break;
        }
        case ro_syz:
        {
          const unsigned long c = __p_GetComp(p, r);
          const short place = o->data.syz.place;
          const int limit = o->data.syz.limit;

          if (c > (unsigned long)limit)
            p->exp[place] = o->data.syz.curr_index;
          else if (c > 0)
          {
            assume( (1 <= c) && (c <= (unsigned long)limit) );
            p->exp[place]= o->data.syz.syz_index[c];
          }
          else
          {
            assume(c == 0);
            p->exp[place]= 0;
          }
          break;
        }
        // Prefix for Induced Schreyer ordering
        case ro_isTemp: // Do nothing?? (to be removed into suffix later on...?)
        {
          assume(p != NULL);

#ifndef SING_NDEBUG
#if MYTEST
          Print("p_Setm_General: ro_isTemp ord: pos: %d, p: ", pos);  p_wrp(p, r);
#endif
#endif
          int c = p_GetComp(p, r);

          assume( c >= 0 );

          // Let's simulate case ro_syz above....
          // Should accumulate (by Suffix) and be a level indicator
          const int* const pVarOffset = o->data.isTemp.pVarOffset;

          assume( pVarOffset != NULL );

          // TODO: Can this be done in the suffix???
          for( int i = 1; i <= r->N; i++ ) // No v[0] here!!!
          {
            const int vo = pVarOffset[i];
            if( vo != -1) // TODO: optimize: can be done once!
            {
              // Hans! Please don't break it again! p_SetExp(p, ..., r, vo) is correct:
              p_SetExp(p, p_GetExp(p, i, r), r, vo); // copy put them verbatim
              // Hans! Please don't break it again! p_GetExp(p, r, vo) is correct:
              assume( p_GetExp(p, r, vo) == p_GetExp(p, i, r) ); // copy put them verbatim
            }
          }
#ifndef SING_NDEBUG
          for( int i = 1; i <= r->N; i++ ) // No v[0] here!!!
          {
            const int vo = pVarOffset[i];
            if( vo != -1) // TODO: optimize: can be done once!
            {
              // Hans! Please don't break it again! p_GetExp(p, r, vo) is correct:
              assume( p_GetExp(p, r, vo) == p_GetExp(p, i, r) ); // copy put them verbatim
            }
          }
#if MYTEST
//          if( p->exp[o->data.isTemp.start] > 0 )
            PrintS("after Values: "); p_wrp(p, r);
#endif
#endif
          break;
        }

        // Suffix for Induced Schreyer ordering
        case ro_is:
        {
#ifndef SING_NDEBUG
#if MYTEST
          Print("p_Setm_General: ro_is ord: pos: %d, p: ", pos);  p_wrp(p, r);
#endif
#endif

          assume(p != NULL);

          int c = p_GetComp(p, r);

          assume( c >= 0 );
          const ideal F = o->data.is.F;
          const int limit = o->data.is.limit;
          assume( limit >= 0 );
          const int start = o->data.is.start;

          if( F != NULL && c > limit )
          {
#ifndef SING_NDEBUG
#if MYTEST
            Print("p_Setm_General: ro_is : in rSetm: pos: %d, c: %d >  limit: %d\n", c, pos, limit);
            PrintS("preComputed Values: ");
            p_wrp(p, r);
#endif
#endif
//          if( c > limit ) // BUG???
            p->exp[start] = 1;
//          else
//            p->exp[start] = 0;


            c -= limit;
            assume( c > 0 );
            c--;

            if( c >= IDELEMS(F) )
              break;

            assume( c < IDELEMS(F) ); // What about others???

            const poly pp = F->m[c]; // get reference monomial!!!

            if(pp == NULL)
              break;

            assume(pp != NULL);

#ifndef SING_NDEBUG
#if MYTEST
            Print("Respective F[c - %d: %d] pp: ", limit, c);
            p_wrp(pp, r);
#endif
#endif

            const int end = o->data.is.end;
            assume(start <= end);


//          const int st = o->data.isTemp.start;

#ifndef SING_NDEBUG
#if MYTEST
            Print("p_Setm_General: is(-Temp-) :: c: %d, limit: %d, [st:%d] ===>>> %ld\n", c, limit, start, p->exp[start]);
#endif
#endif

            // p_ExpVectorAdd(p, pp, r);

            for( int i = start; i <= end; i++) // v[0] may be here...
              p->exp[i] += pp->exp[i]; // !!!!!!!! ADD corresponding LT(F)

            // p_MemAddAdjust(p, ri);
            if (r->NegWeightL_Offset != NULL)
            {
              for (int i=r->NegWeightL_Size-1; i>=0; i--)
              {
                const int _i = r->NegWeightL_Offset[i];
                if( start <= _i && _i <= end )
                  p->exp[_i] -= POLY_NEGWEIGHT_OFFSET;
              }
            }


#ifndef SING_NDEBUG
            const int* const pVarOffset = o->data.is.pVarOffset;

            assume( pVarOffset != NULL );

            for( int i = 1; i <= r->N; i++ ) // No v[0] here!!!
            {
              const int vo = pVarOffset[i];
              if( vo != -1) // TODO: optimize: can be done once!
                // Hans! Please don't break it again! p_GetExp(p/pp, r, vo) is correct:
                assume( p_GetExp(p, r, vo) == (p_GetExp(p, i, r) + p_GetExp(pp, r, vo)) );
            }
            // TODO: how to check this for computed values???
#if MYTEST
            PrintS("Computed Values: "); p_wrp(p, r);
#endif
#endif
          } else
          {
            p->exp[start] = 0; //!!!!????? where?????

            const int* const pVarOffset = o->data.is.pVarOffset;

            // What about v[0] - component: it will be added later by
            // suffix!!!
            // TODO: Test it!
            const int vo = pVarOffset[0];
            if( vo != -1 )
              p->exp[vo] = c; // initial component v[0]!

#ifndef SING_NDEBUG
#if MYTEST
            Print("ELSE p_Setm_General: ro_is :: c: %d <= limit: %d, vo: %d, exp: %d\n", c, limit, vo, p->exp[vo]);
            p_wrp(p, r);
#endif
#endif
          }

          break;
        }
        default:
          dReportError("wrong ord in rSetm:%d\n",o->ord_typ);
          return;
      }
      pos++;
      if (pos == r->OrdSize) return;
    }
  }
}

void p_Setm_Syz(poly p, ring r, int* Components, long* ShiftedComponents)
{
  _components = Components;
  _componentsShifted = ShiftedComponents;
  _componentsExternal = 1;
  p_Setm_General(p, r);
  _componentsExternal = 0;
}

// dummy for lp, ls, etc
void p_Setm_Dummy(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
}

// for dp, Dp, ds, etc
void p_Setm_TotalDegree(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  p->exp[r->pOrdIndex] = p_Totaldegree(p, r);
}

// for wp, Wp, ws, etc
void p_Setm_WFirstTotalDegree(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  p->exp[r->pOrdIndex] = p_WFirstTotalDegree(p, r);
}

p_SetmProc p_GetSetmProc(const ring r)
{
  // covers lp, rp, ls,
  if (r->typ == NULL) return p_Setm_Dummy;

  if (r->OrdSize == 1)
  {
    if (r->typ[0].ord_typ == ro_dp &&
        r->typ[0].data.dp.start == 1 &&
        r->typ[0].data.dp.end == r->N &&
        r->typ[0].data.dp.place == r->pOrdIndex)
      return p_Setm_TotalDegree;
    if (r->typ[0].ord_typ == ro_wp &&
        r->typ[0].data.wp.start == 1 &&
        r->typ[0].data.wp.end == r->N &&
        r->typ[0].data.wp.place == r->pOrdIndex &&
        r->typ[0].data.wp.weights == r->firstwv)
      return p_Setm_WFirstTotalDegree;
  }
  return p_Setm_General;
}


/* -------------------------------------------------------------------*/
/* several possibilities for pFDeg: the degree of the head term       */

/* comptible with ordering */
long p_Deg(poly a, const ring r)
{
  p_LmCheckPolyRing(a, r);
//  assume(p_GetOrder(a, r) == p_WTotaldegree(a, r)); // WRONG assume!
  return p_GetOrder(a, r);
}

// p_WTotalDegree for weighted orderings
// whose first block covers all variables
long p_WFirstTotalDegree(poly p, const ring r)
{
  int i;
  long sum = 0;

  for (i=1; i<= r->firstBlockEnds; i++)
  {
    sum += p_GetExp(p, i, r)*r->firstwv[i-1];
  }
  return sum;
}

/*2
* compute the degree of the leading monomial of p
* with respect to weigths from the ordering
* the ordering is not compatible with degree so do not use p->Order
*/
long p_WTotaldegree(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  int i, k;
  long j =0;

  // iterate through each block:
  for (i=0;r->order[i]!=0;i++)
  {
    int b0=r->block0[i];
    int b1=r->block1[i];
    switch(r->order[i])
    {
      case ringorder_M:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // in jedem block:
          j+= p_GetExp(p,k,r)*r->wvhdl[i][k - b0 /*r->block0[i]*/]*r->OrdSgn;
        }
        break;
      case ringorder_am:
        b1=si_min(b1,r->N);
        /* no break, continue as ringorder_a*/
      case ringorder_a:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // only one line
          j+= p_GetExp(p,k,r)*r->wvhdl[i][k - b0 /*r->block0[i]*/];
        }
        return j*r->OrdSgn;
      case ringorder_wp:
      case ringorder_ws:
      case ringorder_Wp:
      case ringorder_Ws:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // in jedem block:
          j+= p_GetExp(p,k,r)*r->wvhdl[i][k - b0 /*r->block0[i]*/];
        }
        break;
      case ringorder_lp:
      case ringorder_ls:
      case ringorder_rs:
      case ringorder_dp:
      case ringorder_ds:
      case ringorder_Dp:
      case ringorder_Ds:
      case ringorder_rp:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        {
          j+= p_GetExp(p,k,r);
        }
        break;
      case ringorder_a64:
        {
          int64* w=(int64*)r->wvhdl[i];
          for (k=0;k<=(b1 /*r->block1[i]*/ - b0 /*r->block0[i]*/);k++)
          {
            //there should be added a line which checks if w[k]>2^31
            j+= p_GetExp(p,k+1, r)*(long)w[k];
          }
          //break;
          return j;
        }
      case ringorder_c: /* nothing to do*/
      case ringorder_C: /* nothing to do*/
      case ringorder_S: /* nothing to do*/
      case ringorder_s: /* nothing to do*/
      case ringorder_IS: /* nothing to do */
      case ringorder_unspec: /* to make clang happy, does not occur*/
      case ringorder_no: /* to make clang happy, does not occur*/
      case ringorder_L: /* to make clang happy, does not occur*/
      case ringorder_aa: /* ignored by p_WTotaldegree*/
        break;
    /* no default: all orderings covered */
    }
  }
  return  j;
}

long p_DegW(poly p, const short *w, const ring R)
{
  p_Test(p, R);
  assume( w != NULL );
  long r=-LONG_MAX;

  while (p!=NULL)
  {
    long t=totaldegreeWecart_IV(p,R,w);
    if (t>r) r=t;
    pIter(p);
  }
  return r;
}

int p_Weight(int i, const ring r)
{
  if ((r->firstwv==NULL) || (i>r->firstBlockEnds))
  {
    return 1;
  }
  return r->firstwv[i-1];
}

long p_WDegree(poly p, const ring r)
{
  if (r->firstwv==NULL) return p_Totaldegree(p, r);
  p_LmCheckPolyRing(p, r);
  int i;
  long j =0;

  for(i=1;i<=r->firstBlockEnds;i++)
    j+=p_GetExp(p, i, r)*r->firstwv[i-1];

  for (;i<=rVar(r);i++)
    j+=p_GetExp(p,i, r)*p_Weight(i, r);

  return j;
}


/* ---------------------------------------------------------------------*/
/* several possibilities for pLDeg: the maximal degree of a monomial in p*/
/*  compute in l also the pLength of p                                   */

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
*/
long pLDeg0(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  int ll=1;

  if (k > 0)
  {
    while ((pNext(p)!=NULL) && (__p_GetComp(pNext(p), r)==k))
    {
      pIter(p);
      ll++;
    }
  }
  else
  {
     while (pNext(p)!=NULL)
     {
       pIter(p);
       ll++;
     }
  }
  *l=ll;
  return r->pFDeg(p, r);
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the last one
* but search in all components before syzcomp
*/
long pLDeg0c(poly p,int *l, const ring r)
{
  assume(p!=NULL);
  p_Test(p,r);
  p_CheckPolyRing(p, r);
  long o;
  int ll=1;

  if (! rIsSyzIndexRing(r))
  {
    while (pNext(p) != NULL)
    {
      pIter(p);
      ll++;
    }
    o = r->pFDeg(p, r);
  }
  else
  {
    int curr_limit = rGetCurrSyzLimit(r);
    poly pp = p;
    while ((p=pNext(p))!=NULL)
    {
      if (__p_GetComp(p, r)<=curr_limit/*syzComp*/)
        ll++;
      else break;
      pp = p;
    }
    p_Test(pp,r);
    o = r->pFDeg(pp, r);
  }
  *l=ll;
  return o;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree: the first one
* this works for the polynomial case with degree orderings
* (both c,dp and dp,c)
*/
long pLDegb(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  long o = r->pFDeg(p, r);
  int ll=1;

  if (k != 0)
  {
    while (((p=pNext(p))!=NULL) && (__p_GetComp(p, r)==k))
    {
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p)) !=NULL)
    {
      ll++;
    }
  }
  *l=ll;
  return o;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree:
* this is NOT the last one, we have to look for it
*/
long pLDeg1(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=r->pFDeg(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (__p_GetComp(p, r)==k))
    {
      t=r->pFDeg(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=r->pFDeg(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

/*2
* compute the length of a polynomial (in l)
* and the degree of the monomial with maximal degree:
* this is NOT the last one, we have to look for it
* in all components
*/
long pLDeg1c(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=r->pFDeg(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (__p_GetComp(p, r)<=limit)
      {
        if ((t=r->pFDeg(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=r->pFDeg(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

// like pLDeg1, only pFDeg == pDeg
long pLDeg1_Deg(poly p,int *l, const ring r)
{
  assume(r->pFDeg == p_Deg);
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=p_GetOrder(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (__p_GetComp(p, r)==k))
    {
      t=p_GetOrder(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=p_GetOrder(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

long pLDeg1c_Deg(poly p,int *l, const ring r)
{
  assume(r->pFDeg == p_Deg);
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=p_GetOrder(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (__p_GetComp(p, r)<=limit)
      {
        if ((t=p_GetOrder(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=p_GetOrder(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

// like pLDeg1, only pFDeg == pTotoalDegree
long pLDeg1_Totaldegree(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=p_Totaldegree(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (__p_GetComp(p, r)==k))
    {
      t=p_Totaldegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=p_Totaldegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

long pLDeg1c_Totaldegree(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=p_Totaldegree(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (__p_GetComp(p, r)<=limit)
      {
        if ((t=p_Totaldegree(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=p_Totaldegree(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

// like pLDeg1, only pFDeg == p_WFirstTotalDegree
long pLDeg1_WFirstTotalDegree(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=p_WFirstTotalDegree(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (__p_GetComp(p, r)==k))
    {
      t=p_WFirstTotalDegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      t=p_WFirstTotalDegree(p, r);
      if (t>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

long pLDeg1c_WFirstTotalDegree(poly p,int *l, const ring r)
{
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=p_WFirstTotalDegree(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (__p_GetComp(p, r)<=limit)
      {
        if ((t=p_Totaldegree(p, r))>max) max=t;
        ll++;
      }
      else break;
    }
  }
  else
  {
    while ((p=pNext(p))!=NULL)
    {
      if ((t=p_Totaldegree(p, r))>max) max=t;
      ll++;
    }
  }
  *l=ll;
  return max;
}

/***************************************************************
 *
 * Maximal Exponent business
 *
 ***************************************************************/

static inline unsigned long
p_GetMaxExpL2(unsigned long l1, unsigned long l2, const ring r,
              unsigned long number_of_exp)
{
  const unsigned long bitmask = r->bitmask;
  unsigned long ml1 = l1 & bitmask;
  unsigned long ml2 = l2 & bitmask;
  unsigned long max = (ml1 > ml2 ? ml1 : ml2);
  unsigned long j = number_of_exp - 1;

  if (j > 0)
  {
    unsigned long mask = bitmask << r->BitsPerExp;
    while (1)
    {
      ml1 = l1 & mask;
      ml2 = l2 & mask;
      max |= ((ml1 > ml2 ? ml1 : ml2) & mask);
      j--;
      if (j == 0) break;
      mask = mask << r->BitsPerExp;
    }
  }
  return max;
}

static inline unsigned long
p_GetMaxExpL2(unsigned long l1, unsigned long l2, const ring r)
{
  return p_GetMaxExpL2(l1, l2, r, r->ExpPerLong);
}

poly p_GetMaxExpP(poly p, const ring r)
{
  p_CheckPolyRing(p, r);
  if (p == NULL) return p_Init(r);
  poly max = p_LmInit(p, r);
  pIter(p);
  if (p == NULL) return max;
  int i, offset;
  unsigned long l_p, l_max;
  unsigned long divmask = r->divmask;

  do
  {
    offset = r->VarL_Offset[0];
    l_p = p->exp[offset];
    l_max = max->exp[offset];
    // do the divisibility trick to find out whether l has an exponent
    if (l_p > l_max ||
        (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
      max->exp[offset] = p_GetMaxExpL2(l_max, l_p, r);

    for (i=1; i<r->VarL_Size; i++)
    {
      offset = r->VarL_Offset[i];
      l_p = p->exp[offset];
      l_max = max->exp[offset];
      // do the divisibility trick to find out whether l has an exponent
      if (l_p > l_max ||
          (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
        max->exp[offset] = p_GetMaxExpL2(l_max, l_p, r);
    }
    pIter(p);
  }
  while (p != NULL);
  return max;
}

unsigned long p_GetMaxExpL(poly p, const ring r, unsigned long l_max)
{
  unsigned long l_p, divmask = r->divmask;
  int i;

  while (p != NULL)
  {
    l_p = p->exp[r->VarL_Offset[0]];
    if (l_p > l_max ||
        (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
      l_max = p_GetMaxExpL2(l_max, l_p, r);
    for (i=1; i<r->VarL_Size; i++)
    {
      l_p = p->exp[r->VarL_Offset[i]];
      // do the divisibility trick to find out whether l has an exponent
      if (l_p > l_max ||
          (((l_max & divmask) ^ (l_p & divmask)) != ((l_max-l_p) & divmask)))
        l_max = p_GetMaxExpL2(l_max, l_p, r);
    }
    pIter(p);
  }
  return l_max;
}




/***************************************************************
 *
 * Misc things
 *
 ***************************************************************/
// returns TRUE, if all monoms have the same component
BOOLEAN p_OneComp(poly p, const ring r)
{
  if(p!=NULL)
  {
    long i = p_GetComp(p, r);
    while (pNext(p)!=NULL)
    {
      pIter(p);
      if(i != p_GetComp(p, r)) return FALSE;
    }
  }
  return TRUE;
}

/*2
*test if a monomial /head term is a pure power,
* i.e. depends on only one variable
*/
int p_IsPurePower(const poly p, const ring r)
{
  int i,k=0;

  for (i=r->N;i;i--)
  {
    if (p_GetExp(p,i, r)!=0)
    {
      if(k!=0) return 0;
      k=i;
    }
  }
  return k;
}

/*2
*test if a polynomial is univariate
* return -1 for constant,
* 0 for not univariate,s
* i if dep. on var(i)
*/
int p_IsUnivariate(poly p, const ring r)
{
  int i,k=-1;

  while (p!=NULL)
  {
    for (i=r->N;i;i--)
    {
      if (p_GetExp(p,i, r)!=0)
      {
        if((k!=-1)&&(k!=i)) return 0;
        k=i;
      }
    }
    pIter(p);
  }
  return k;
}

// set entry e[i] to 1 if var(i) occurs in p, ignore var(j) if e[j]>0
int  p_GetVariables(poly p, int * e, const ring r)
{
  int i;
  int n=0;
  while(p!=NULL)
  {
    n=0;
    for(i=r->N; i>0; i--)
    {
      if(e[i]==0)
      {
        if (p_GetExp(p,i,r)>0)
        {
          e[i]=1;
          n++;
        }
      }
      else
        n++;
    }
    if (n==r->N) break;
    pIter(p);
  }
  return n;
}


/*2
* returns a polynomial representing the integer i
*/
poly p_ISet(long i, const ring r)
{
  poly rc = NULL;
  if (i!=0)
  {
    rc = p_Init(r);
    pSetCoeff0(rc,n_Init(i,r->cf));
    if (n_IsZero(pGetCoeff(rc),r->cf))
      p_LmDelete(&rc,r);
  }
  return rc;
}

/*2
* an optimized version of p_ISet for the special case 1
*/
poly p_One(const ring r)
{
  poly rc = p_Init(r);
  pSetCoeff0(rc,n_Init(1,r->cf));
  return rc;
}

void p_Split(poly p, poly *h)
{
  *h=pNext(p);
  pNext(p)=NULL;
}

/*2
* pair has no common factor ? or is no polynomial
*/
BOOLEAN p_HasNotCF(poly p1, poly p2, const ring r)
{

  if (p_GetComp(p1,r) > 0 || p_GetComp(p2,r) > 0)
    return FALSE;
  int i = rVar(r);
  loop
  {
    if ((p_GetExp(p1, i, r) > 0) && (p_GetExp(p2, i, r) > 0))
      return FALSE;
    i--;
    if (i == 0)
      return TRUE;
  }
}

BOOLEAN p_HasNotCFRing(poly p1, poly p2, const ring r)
{

  if (p_GetComp(p1,r) > 0 || p_GetComp(p2,r) > 0)
    return FALSE;
  int i = rVar(r);
  loop
  {
    if ((p_GetExp(p1, i, r) > 0) && (p_GetExp(p2, i, r) > 0))
      return FALSE;
    i--;
    if (i == 0) {
      if (n_DivBy(pGetCoeff(p1), pGetCoeff(p2), r->cf) ||
          n_DivBy(pGetCoeff(p2), pGetCoeff(p1), r->cf)) {
        return FALSE;
      } else {
        return TRUE;
      }
    }
  }
}

/*2
* convert monomial given as string to poly, e.g. 1x3y5z
*/
const char * p_Read(const char *st, poly &rc, const ring r)
{
  if (r==NULL) { rc=NULL;return st;}
  int i,j;
  rc = p_Init(r);
  const char *s = n_Read(st,&(p_GetCoeff(rc, r)),r->cf);
  if (s==st)
  /* i.e. it does not start with a coeff: test if it is a ringvar*/
  {
    j = r_IsRingVar(s,r->names,r->N);
    if (j >= 0)
    {
      p_IncrExp(rc,1+j,r);
      while (*s!='\0') s++;
      goto done;
    }
  }
  while (*s!='\0')
  {
    char ss[2];
    ss[0] = *s++;
    ss[1] = '\0';
    j = r_IsRingVar(ss,r->names,r->N);
    if (j >= 0)
    {
      const char *s_save=s;
      s = eati(s,&i);
      if (((unsigned long)i) >  r->bitmask/2)
      {
        // exponent to large: it is not a monomial
        p_LmDelete(&rc,r);
        return s_save;
      }
      p_AddExp(rc,1+j, (long)i, r);
    }
    else
    {
      // 1st char of is not a varname
      // We return the parsed polynomial nevertheless. This is needed when
      // we are parsing coefficients in a rational function field.
      s--;
      break;
    }
  }
done:
  if (n_IsZero(pGetCoeff(rc),r->cf)) p_LmDelete(&rc,r);
  else
  {
#ifdef HAVE_PLURAL
    // in super-commutative ring
    // squares of anti-commutative variables are zeroes!
    if(rIsSCA(r))
    {
      const unsigned int iFirstAltVar = scaFirstAltVar(r);
      const unsigned int iLastAltVar  = scaLastAltVar(r);

      assume(rc != NULL);

      for(unsigned int k = iFirstAltVar; k <= iLastAltVar; k++)
        if( p_GetExp(rc, k, r) > 1 )
        {
          p_LmDelete(&rc, r);
          goto finish;
        }
    }
#endif

    p_Setm(rc,r);
  }
finish:
  return s;
}
poly p_mInit(const char *st, BOOLEAN &ok, const ring r)
{
  poly p;
  const char *s=p_Read(st,p,r);
  if (*s!='\0')
  {
    if ((s!=st)&&isdigit(st[0]))
    {
      errorreported=TRUE;
    }
    ok=FALSE;
    p_Delete(&p,r);
    return NULL;
  }
  p_Test(p,r);
  ok=!errorreported;
  return p;
}

/*2
* returns a polynomial representing the number n
* destroys n
*/
poly p_NSet(number n, const ring r)
{
  if (n_IsZero(n,r->cf))
  {
    n_Delete(&n, r->cf);
    return NULL;
  }
  else
  {
    poly rc = p_Init(r);
    pSetCoeff0(rc,n);
    return rc;
  }
}
/*2
* assumes that LM(a) = LM(b)*m, for some monomial m,
* returns the multiplicant m,
* leaves a and b unmodified
*/
poly p_MDivide(poly a, poly b, const ring r)
{
  assume((p_GetComp(a,r)==p_GetComp(b,r)) || (p_GetComp(b,r)==0));
  int i;
  poly result = p_Init(r);

  for(i=(int)r->N; i; i--)
    p_SetExp(result,i, p_GetExp(a,i,r)- p_GetExp(b,i,r),r);
  p_SetComp(result, p_GetComp(a,r) - p_GetComp(b,r),r);
  p_Setm(result,r);
  return result;
}

poly p_Div_nn(poly p, const number n, const ring r)
{
  pAssume(!n_IsZero(n,r->cf));
  p_Test(p, r);
  poly result = p;
  poly prev = NULL;
  while (p!=NULL)
  {
    number nc = n_Div(pGetCoeff(p),n,r->cf);
    if (!n_IsZero(nc,r->cf))
    {
      p_SetCoeff(p,nc,r);
      prev=p;
      pIter(p);
    }
    else
    {
      if (prev==NULL)
      {
        p_LmDelete(&result,r);
        p=result;
      }
      else
      {
        p_LmDelete(&pNext(prev),r);
        p=pNext(prev);
      }
    }
  }
  p_Test(result,r);
  return(result);
}

poly p_Div_mm(poly p, const poly m, const ring r)
{
  p_Test(p, r);
  p_Test(m, r);
  poly result = p;
  poly prev = NULL;
  number n=pGetCoeff(m);
  while (p!=NULL)
  {
    number nc = n_Div(pGetCoeff(p),n,r->cf);
    n_Normalize(nc,r->cf);
    if (!n_IsZero(nc,r->cf))
    {
      p_SetCoeff(p,nc,r);
      prev=p;
      p_ExpVectorSub(p,m,r);
      pIter(p);
    }
    else
    {
      if (prev==NULL)
      {
        p_LmDelete(&result,r);
        p=result;
      }
      else
      {
        p_LmDelete(&pNext(prev),r);
        p=pNext(prev);
      }
    }
  }
  p_Test(result,r);
  return(result);
}

/*2
* divides a by the monomial b, ignores monomials which are not divisible
* assumes that b is not NULL, destroyes b
*/
poly p_DivideM(poly a, poly b, const ring r)
{
  if (a==NULL) { p_Delete(&b,r); return NULL; }
  poly result=a;

  if(!p_IsConstant(b,r))
  {
    if (rIsLPRing(r))
    {
      WerrorS("not implemented for letterplace rings");
      return NULL;
    }
    poly prev=NULL;
    while (a!=NULL)
    {
      if (p_DivisibleBy(b,a,r))
      {
        p_ExpVectorSub(a,b,r);
        prev=a;
        pIter(a);
      }
      else
      {
        if (prev==NULL)
        {
          p_LmDelete(&result,r);
          a=result;
        }
        else
        {
          p_LmDelete(&pNext(prev),r);
          a=pNext(prev);
        }
      }
    }
  }
  if (result!=NULL)
  {
    number inv=pGetCoeff(b);
    //if ((!rField_is_Ring(r)) || n_IsUnit(inv,r->cf))
    if (rField_is_Zp(r))
    {
      inv = n_Invers(inv,r->cf);
      __p_Mult_nn(result,inv,r);
      n_Delete(&inv, r->cf);
    }
    else
    {
      result = p_Div_nn(result,inv,r);
    }
  }
  p_Delete(&b, r);
  return result;
}

poly pp_DivideM(poly a, poly b, const ring r)
{
  if (a==NULL) { return NULL; }
  // TODO: better implementation without copying a,b
  return p_DivideM(p_Copy(a,r),p_Head(b,r),r);
}

#ifdef HAVE_RINGS
/* TRUE iff LT(f) | LT(g) */
BOOLEAN p_DivisibleByRingCase(poly f, poly g, const ring r)
{
  int exponent;
  for(int i = (int)rVar(r); i>0; i--)
  {
    exponent = p_GetExp(g, i, r) - p_GetExp(f, i, r);
    if (exponent < 0) return FALSE;
  }
  return n_DivBy(pGetCoeff(g), pGetCoeff(f), r->cf);
}
#endif

// returns the LCM of the head terms of a and b in *m
void p_Lcm(const poly a, const poly b, poly m, const ring r)
{
  for (int i=r->N; i; --i)
    p_SetExp(m,i, si_max( p_GetExp(a,i,r), p_GetExp(b,i,r)),r);

  p_SetComp(m, si_max(p_GetComp(a,r), p_GetComp(b,r)),r);
  /* Don't do a pSetm here, otherwise hres/lres chockes */
}

poly p_Lcm(const poly a, const poly b, const ring r)
{
  poly m=p_Init(r);
  p_Lcm(a, b, m, r);
  p_Setm(m,r);
  return(m);
}

#ifdef HAVE_RATGRING
/*2
* returns the rational LCM of the head terms of a and b
* without coefficient!!!
*/
poly p_LcmRat(const poly a, const poly b, const long lCompM, const ring r)
{
  poly m = // p_One( r);
          p_Init(r);

//  const int (currRing->N) = r->N;

  //  for (int i = (currRing->N); i>=r->real_var_start; i--)
  for (int i = r->real_var_end; i>=r->real_var_start; i--)
  {
    const int lExpA = p_GetExp (a, i, r);
    const int lExpB = p_GetExp (b, i, r);

    p_SetExp (m, i, si_max(lExpA, lExpB), r);
  }

  p_SetComp (m, lCompM, r);
  p_Setm(m,r);
  n_New(&(p_GetCoeff(m, r)), r);

  return(m);
};

void p_LmDeleteAndNextRat(poly *p, int ishift, ring r)
{
  /* modifies p*/
  //  Print("start: "); Print(" "); p_wrp(*p,r);
  p_LmCheckPolyRing2(*p, r);
  poly q = p_Head(*p,r);
  const long cmp = p_GetComp(*p, r);
  while ( ( (*p)!=NULL ) && ( p_Comp_k_n(*p, q, ishift+1, r) ) && (p_GetComp(*p, r) == cmp) )
  {
    p_LmDelete(p,r);
    //    Print("while: ");p_wrp(*p,r);Print(" ");
  }
  //  p_wrp(*p,r);Print(" ");
  //  PrintS("end\n");
  p_LmDelete(&q,r);
}


/* returns x-coeff of p, i.e. a poly in x, s.t. corresponding xd-monomials
have the same D-part and the component 0
does not destroy p
*/
poly p_GetCoeffRat(poly p, int ishift, ring r)
{
  poly q   = pNext(p);
  poly res; // = p_Head(p,r);
  res = p_GetExp_k_n(p, ishift+1, r->N, r); // does pSetm internally
  p_SetCoeff(res,n_Copy(p_GetCoeff(p,r),r),r);
  poly s;
  long cmp = p_GetComp(p, r);
  while ( (q!= NULL) && (p_Comp_k_n(p, q, ishift+1, r)) && (p_GetComp(q, r) == cmp) )
  {
    s   = p_GetExp_k_n(q, ishift+1, r->N, r);
    p_SetCoeff(s,n_Copy(p_GetCoeff(q,r),r),r);
    res = p_Add_q(res,s,r);
    q   = pNext(q);
  }
  cmp = 0;
  p_SetCompP(res,cmp,r);
  return res;
}



void p_ContentRat(poly &ph, const ring r)
// changes ph
// for rat coefficients in K(x1,..xN)
{
  // init array of RatLeadCoeffs
  //  poly p_GetCoeffRat(poly p, int ishift, ring r);

  int len=pLength(ph);
  poly *C = (poly *)omAlloc0((len+1)*sizeof(poly));  //rat coeffs
  poly *LM = (poly *)omAlloc0((len+1)*sizeof(poly));  // rat lead terms
  int *D = (int *)omAlloc0((len+1)*sizeof(int));  //degrees of coeffs
  int *L = (int *)omAlloc0((len+1)*sizeof(int));  //lengths of coeffs
  int k = 0;
  poly p = p_Copy(ph, r); // ph will be needed below
  int mintdeg = p_Totaldegree(p, r);
  int minlen = len;
  int dd = 0; int i;
  int HasConstantCoef = 0;
  int is = r->real_var_start - 1;
  while (p!=NULL)
  {
    LM[k] = p_GetExp_k_n(p,1,is, r); // need LmRat istead of  p_HeadRat(p, is, currRing); !
    C[k] = p_GetCoeffRat(p, is, r);
    D[k] =  p_Totaldegree(C[k], r);
    mintdeg = si_min(mintdeg,D[k]);
    L[k] = pLength(C[k]);
    minlen = si_min(minlen,L[k]);
    if (p_IsConstant(C[k], r))
    {
      // C[k] = const, so the content will be numerical
      HasConstantCoef = 1;
      // smth like goto cleanup and return(pContent(p));
    }
    p_LmDeleteAndNextRat(&p, is, r);
    k++;
  }

  // look for 1 element of minimal degree and of minimal length
  k--;
  poly d;
  int mindeglen = len;
  if (k<=0) // this poly is not a ratgring poly -> pContent
  {
    p_Delete(&C[0], r);
    p_Delete(&LM[0], r);
    p_ContentForGB(ph, r);
    goto cleanup;
  }

  int pmindeglen;
  for(i=0; i<=k; i++)
  {
    if (D[i] == mintdeg)
    {
      if (L[i] < mindeglen)
      {
        mindeglen=L[i];
        pmindeglen = i;
      }
    }
  }
  d = p_Copy(C[pmindeglen], r);
  // there are dd>=1 mindeg elements
  // and pmideglen is the coordinate of one of the smallest among them

  //  poly g = singclap_gcd(p_Copy(p,r),p_Copy(q,r));
  //  return naGcd(d,d2,currRing);

  // adjoin pContentRat here?
  for(i=0; i<=k; i++)
  {
    d=singclap_gcd(d,p_Copy(C[i], r), r);
    if (p_Totaldegree(d, r)==0)
    {
      // cleanup, pContent, return
      p_Delete(&d, r);
      for(;k>=0;k--)
      {
        p_Delete(&C[k], r);
        p_Delete(&LM[k], r);
      }
      p_ContentForGB(ph, r);
      goto cleanup;
    }
  }
  for(i=0; i<=k; i++)
  {
    poly h=singclap_pdivide(C[i],d, r);
    p_Delete(&C[i], r);
    C[i]=h;
  }

  // zusammensetzen,
  p=NULL; // just to be sure
  for(i=0; i<=k; i++)
  {
    p = p_Add_q(p, p_Mult_q(C[i],LM[i], r), r);
    C[i]=NULL; LM[i]=NULL;
  }
  p_Delete(&ph, r); // do not need it anymore
  ph = p;
  // aufraeumen, return
cleanup:
  omFree(C);
  omFree(LM);
  omFree(D);
  omFree(L);
}


#endif


/* assumes that p and divisor are univariate polynomials in r,
   mentioning the same variable;
   assumes divisor != NULL;
   p may be NULL;
   assumes a global monomial ordering in r;
   performs polynomial division of p by divisor:
     - afterwards p contains the remainder of the division, i.e.,
       p_before = result * divisor + p_afterwards;
     - if needResult == TRUE, then the method computes and returns 'result',
       otherwise NULL is returned (This parametrization can be used when
       one is only interested in the remainder of the division. In this
       case, the method will be slightly faster.)
   leaves divisor unmodified */
poly      p_PolyDiv(poly &p, const poly divisor, const BOOLEAN needResult, const ring r)
{
  assume(divisor != NULL);
  if (p == NULL) return NULL;

  poly result = NULL;
  number divisorLC = p_GetCoeff(divisor, r);
  int divisorLE = p_GetExp(divisor, 1, r);
  while ((p != NULL) && (p_Deg(p, r) >= p_Deg(divisor, r)))
  {
    /* determine t = LT(p) / LT(divisor) */
    poly t = p_ISet(1, r);
    number c = n_Div(p_GetCoeff(p, r), divisorLC, r->cf);
    n_Normalize(c,r->cf);
    p_SetCoeff(t, c, r);
    int e = p_GetExp(p, 1, r) - divisorLE;
    p_SetExp(t, 1, e, r);
    p_Setm(t, r);
    if (needResult) result = p_Add_q(result, p_Copy(t, r), r);
    p = p_Add_q(p, p_Neg(p_Mult_q(t, p_Copy(divisor, r), r), r), r);
  }
  return result;
}

/*2
* returns the partial differentiate of a by the k-th variable
* does not destroy the input
*/
poly p_Diff(poly a, int k, const ring r)
{
  poly res, f, last;
  number t;

  last = res = NULL;
  while (a!=NULL)
  {
    if (p_GetExp(a,k,r)!=0)
    {
      f = p_LmInit(a,r);
      t = n_Init(p_GetExp(a,k,r),r->cf);
      pSetCoeff0(f,n_Mult(t,pGetCoeff(a),r->cf));
      n_Delete(&t,r->cf);
      if (n_IsZero(pGetCoeff(f),r->cf))
        p_LmDelete(&f,r);
      else
      {
        p_DecrExp(f,k,r);
        p_Setm(f,r);
        if (res==NULL)
        {
          res=last=f;
        }
        else
        {
          pNext(last)=f;
          last=f;
        }
      }
    }
    pIter(a);
  }
  return res;
}

static poly p_DiffOpM(poly a, poly b,BOOLEAN multiply, const ring r)
{
  int i,j,s;
  number n,h,hh;
  poly p=p_One(r);
  n=n_Mult(pGetCoeff(a),pGetCoeff(b),r->cf);
  for(i=rVar(r);i>0;i--)
  {
    s=p_GetExp(b,i,r);
    if (s<p_GetExp(a,i,r))
    {
      n_Delete(&n,r->cf);
      p_LmDelete(&p,r);
      return NULL;
    }
    if (multiply)
    {
      for(j=p_GetExp(a,i,r); j>0;j--)
      {
        h = n_Init(s,r->cf);
        hh=n_Mult(n,h,r->cf);
        n_Delete(&h,r->cf);
        n_Delete(&n,r->cf);
        n=hh;
        s--;
      }
      p_SetExp(p,i,s,r);
    }
    else
    {
      p_SetExp(p,i,s-p_GetExp(a,i,r),r);
    }
  }
  p_Setm(p,r);
  /*if (multiply)*/ p_SetCoeff(p,n,r);
  if (n_IsZero(n,r->cf))  p=p_LmDeleteAndNext(p,r); // return NULL as p is a monomial
  return p;
}

poly p_DiffOp(poly a, poly b,BOOLEAN multiply, const ring r)
{
  poly result=NULL;
  poly h;
  for(;a!=NULL;pIter(a))
  {
    for(h=b;h!=NULL;pIter(h))
    {
      result=p_Add_q(result,p_DiffOpM(a,h,multiply,r),r);
    }
  }
  return result;
}
/*2
* subtract p2 from p1, p1 and p2 are destroyed
* do not put attention on speed: the procedure is only used in the interpreter
*/
poly p_Sub(poly p1, poly p2, const ring r)
{
  return p_Add_q(p1, p_Neg(p2,r),r);
}

/*3
* compute for a monomial m
* the power m^exp, exp > 1
* destroys p
*/
static poly p_MonPower(poly p, int exp, const ring r)
{
  int i;

  if(!n_IsOne(pGetCoeff(p),r->cf))
  {
    number x, y;
    y = pGetCoeff(p);
    n_Power(y,exp,&x,r->cf);
    n_Delete(&y,r->cf);
    pSetCoeff0(p,x);
  }
  for (i=rVar(r); i!=0; i--)
  {
    p_MultExp(p,i, exp,r);
  }
  p_Setm(p,r);
  return p;
}

/*3
* compute for monomials p*q
* destroys p, keeps q
*/
static void p_MonMult(poly p, poly q, const ring r)
{
  number x, y;

  y = pGetCoeff(p);
  x = n_Mult(y,pGetCoeff(q),r->cf);
  n_Delete(&y,r->cf);
  pSetCoeff0(p,x);
  //for (int i=pVariables; i!=0; i--)
  //{
  //  pAddExp(p,i, pGetExp(q,i));
  //}
  //p->Order += q->Order;
  p_ExpVectorAdd(p,q,r);
}

/*3
* compute for monomials p*q
* keeps p, q
*/
static poly p_MonMultC(poly p, poly q, const ring rr)
{
  number x;
  poly r = p_Init(rr);

  x = n_Mult(pGetCoeff(p),pGetCoeff(q),rr->cf);
  pSetCoeff0(r,x);
  p_ExpVectorSum(r,p, q, rr);
  return r;
}

/*3
*  create binomial coef.
*/
static number* pnBin(int exp, const ring r)
{
  int e, i, h;
  number x, y, *bin=NULL;

  x = n_Init(exp,r->cf);
  if (n_IsZero(x,r->cf))
  {
    n_Delete(&x,r->cf);
    return bin;
  }
  h = (exp >> 1) + 1;
  bin = (number *)omAlloc0(h*sizeof(number));
  bin[1] = x;
  if (exp < 4)
    return bin;
  i = exp - 1;
  for (e=2; e<h; e++)
  {
      x = n_Init(i,r->cf);
      i--;
      y = n_Mult(x,bin[e-1],r->cf);
      n_Delete(&x,r->cf);
      x = n_Init(e,r->cf);
      bin[e] = n_ExactDiv(y,x,r->cf);
      n_Delete(&x,r->cf);
      n_Delete(&y,r->cf);
  }
  return bin;
}

static void pnFreeBin(number *bin, int exp,const coeffs r)
{
  int e, h = (exp >> 1) + 1;

  if (bin[1] != NULL)
  {
    for (e=1; e<h; e++)
      n_Delete(&(bin[e]),r);
  }
  omFreeSize((ADDRESS)bin, h*sizeof(number));
}

/*
*  compute for a poly p = head+tail, tail is monomial
*          (head + tail)^exp, exp > 1
*          with binomial coef.
*/
static poly p_TwoMonPower(poly p, int exp, const ring r)
{
  int eh, e;
  long al;
  poly *a;
  poly tail, b, res, h;
  number x;
  number *bin = pnBin(exp,r);

  tail = pNext(p);
  if (bin == NULL)
  {
    p_MonPower(p,exp,r);
    p_MonPower(tail,exp,r);
    p_Test(p,r);
    return p;
  }
  eh = exp >> 1;
  al = (exp + 1) * sizeof(poly);
  a = (poly *)omAlloc(al);
  a[1] = p;
  for (e=1; e<exp; e++)
  {
    a[e+1] = p_MonMultC(a[e],p,r);
  }
  res = a[exp];
  b = p_Head(tail,r);
  for (e=exp-1; e>eh; e--)
  {
    h = a[e];
    x = n_Mult(bin[exp-e],pGetCoeff(h),r->cf);
    p_SetCoeff(h,x,r);
    p_MonMult(h,b,r);
    res = pNext(res) = h;
    p_MonMult(b,tail,r);
  }
  for (e=eh; e!=0; e--)
  {
    h = a[e];
    x = n_Mult(bin[e],pGetCoeff(h),r->cf);
    p_SetCoeff(h,x,r);
    p_MonMult(h,b,r);
    res = pNext(res) = h;
    p_MonMult(b,tail,r);
  }
  p_LmDelete(&tail,r);
  pNext(res) = b;
  pNext(b) = NULL;
  res = a[exp];
  omFreeSize((ADDRESS)a, al);
  pnFreeBin(bin, exp, r->cf);
//  tail=res;
// while((tail!=NULL)&&(pNext(tail)!=NULL))
// {
//   if(nIsZero(pGetCoeff(pNext(tail))))
//   {
//     pLmDelete(&pNext(tail));
//   }
//   else
//     pIter(tail);
// }
  p_Test(res,r);
  return res;
}

static poly p_Pow(poly p, int i, const ring r)
{
  poly rc = p_Copy(p,r);
  i -= 2;
  do
  {
    rc = p_Mult_q(rc,p_Copy(p,r),r);
    p_Normalize(rc,r);
    i--;
  }
  while (i != 0);
  return p_Mult_q(rc,p,r);
}

static poly p_Pow_charp(poly p, int i, const ring r)
{
  //assume char_p == i
  poly h=p;
  while(h!=NULL) { p_MonPower(h,i,r);pIter(h);}
  return p;
}

/*2
* returns the i-th power of p
* p will be destroyed
*/
poly p_Power(poly p, int i, const ring r)
{
  poly rc=NULL;

  if (i==0)
  {
    p_Delete(&p,r);
    return p_One(r);
  }

  if(p!=NULL)
  {
    if ( (i > 0) && ((unsigned long ) i > (r->bitmask))
    #ifdef HAVE_SHIFTBBA
    && (!rIsLPRing(r))
    #endif
    )
    {
      Werror("exponent %d is too large, max. is %ld",i,r->bitmask);
      return NULL;
    }
    switch (i)
    {
// cannot happen, see above
//      case 0:
//      {
//        rc=pOne();
//        pDelete(&p);
//        break;
//      }
      case 1:
        rc=p;
        break;
      case 2:
        rc=p_Mult_q(p_Copy(p,r),p,r);
        break;
      default:
        if (i < 0)
        {
          p_Delete(&p,r);
          return NULL;
        }
        else
        {
#ifdef HAVE_PLURAL
          if (rIsNCRing(r)) /* in the NC case nothing helps :-( */
          {
            int j=i;
            rc = p_Copy(p,r);
            while (j>1)
            {
              rc = p_Mult_q(p_Copy(p,r),rc,r);
              j--;
            }
            p_Delete(&p,r);
            return rc;
          }
#endif
          rc = pNext(p);
          if (rc == NULL)
            return p_MonPower(p,i,r);
          /* else: binom ?*/
          int char_p=rChar(r);
          if ((char_p>0) && (i>char_p)
          && ((rField_is_Zp(r,char_p)
            || (rField_is_Zp_a(r,char_p)))))
          {
            poly h=p_Pow_charp(p_Copy(p,r),char_p,r);
            int rest=i-char_p;
            while (rest>=char_p)
            {
              rest-=char_p;
              h=p_Mult_q(h,p_Pow_charp(p_Copy(p,r),char_p,r),r);
            }
            poly res=h;
            if (rest>0)
              res=p_Mult_q(p_Power(p_Copy(p,r),rest,r),h,r);
            p_Delete(&p,r);
            return res;
          }
          if ((pNext(rc) != NULL)
             || rField_is_Ring(r)
             )
            return p_Pow(p,i,r);
          if ((char_p==0) || (i<=char_p))
            return p_TwoMonPower(p,i,r);
          return p_Pow(p,i,r);
        }
      /*end default:*/
    }
  }
  return rc;
}

/* --------------------------------------------------------------------------------*/
/* content suff                                                                   */
//number p_InitContent(poly ph, const ring r);

void p_Content(poly ph, const ring r)
{
  if (ph==NULL) return;
  const coeffs cf=r->cf;
  if (pNext(ph)==NULL)
  {
    p_SetCoeff(ph,n_Init(1,cf),r);
  }
  if (cf->cfSubringGcd==ndGcd) /* trivial gcd*/ return;
  number h=p_InitContent(ph,r); /* first guess of a gcd of all coeffs */
  poly p;
  if(n_IsOne(h,cf))
  {
    goto content_finish;
  }
  p=ph;
  // take the SubringGcd of all coeffs
  while (p!=NULL)
  {
    n_Normalize(pGetCoeff(p),cf);
    number d=n_SubringGcd(h,pGetCoeff(p),cf);
    n_Delete(&h,cf);
    h = d;
    if(n_IsOne(h,cf))
    {
      goto content_finish;
    }
    pIter(p);
  }
  // if found<>1, divide by it
  p = ph;
  while (p!=NULL)
  {
    number d = n_ExactDiv(pGetCoeff(p),h,cf);
    p_SetCoeff(p,d,r);
    pIter(p);
  }
content_finish:
  n_Delete(&h,r->cf);
  // and last: check leading sign:
  if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
}

#define CLEARENUMERATORS 1

void p_ContentForGB(poly ph, const ring r)
{
  if(TEST_OPT_CONTENTSB) return;
  assume( ph != NULL );

  assume( r != NULL ); assume( r->cf != NULL );


#if CLEARENUMERATORS
  if( 0 )
  {
    const coeffs C = r->cf;
      // experimentall (recursive enumerator treatment) of alg. Ext!
    CPolyCoeffsEnumerator itr(ph);
    n_ClearContent(itr, r->cf);

    p_Test(ph, r); n_Test(pGetCoeff(ph), C);
    assume(n_GreaterZero(pGetCoeff(ph), C)); // ??

      // if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    return;
  }
#endif


#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    if (rField_has_Units(r))
    {
      number k = n_GetUnit(pGetCoeff(ph),r->cf);
      if (!n_IsOne(k,r->cf))
      {
        number tmpGMP = k;
        k = n_Invers(k,r->cf);
        n_Delete(&tmpGMP,r->cf);
        poly h = pNext(ph);
        p_SetCoeff(ph, n_Mult(pGetCoeff(ph), k,r->cf),r);
        while (h != NULL)
        {
          p_SetCoeff(h, n_Mult(pGetCoeff(h), k,r->cf),r);
          pIter(h);
        }
//        assume( n_GreaterZero(pGetCoeff(ph),r->cf) );
//        if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
      }
      n_Delete(&k,r->cf);
    }
    return;
  }
#endif
  number h,d;
  poly p;

  if(pNext(ph)==NULL)
  {
    p_SetCoeff(ph,n_Init(1,r->cf),r);
  }
  else
  {
    assume( pNext(ph) != NULL );
#if CLEARENUMERATORS
    if( nCoeff_is_Q(r->cf) )
    {
      // experimentall (recursive enumerator treatment) of alg. Ext!
      CPolyCoeffsEnumerator itr(ph);
      n_ClearContent(itr, r->cf);

      p_Test(ph, r); n_Test(pGetCoeff(ph), r->cf);
      assume(n_GreaterZero(pGetCoeff(ph), r->cf)); // ??

      // if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
      return;
    }
#endif

    n_Normalize(pGetCoeff(ph),r->cf);
    if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    if (rField_is_Q(r)||(getCoeffType(r->cf)==n_transExt)) // should not be used anymore if CLEARENUMERATORS is 1
    {
      h=p_InitContent(ph,r);
      p=ph;
    }
    else
    {
      h=n_Copy(pGetCoeff(ph),r->cf);
      p = pNext(ph);
    }
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r->cf);
      d=n_SubringGcd(h,pGetCoeff(p),r->cf);
      n_Delete(&h,r->cf);
      h = d;
      if(n_IsOne(h,r->cf))
      {
        break;
      }
      pIter(p);
    }
    //number tmp;
    if(!n_IsOne(h,r->cf))
    {
      p = ph;
      while (p!=NULL)
      {
        //d = nDiv(pGetCoeff(p),h);
        //tmp = nExactDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringEndS("\n")); // NOTE/TODO: use StringAppendS("\n"); omFree(s);
        //}
        //nDelete(&tmp);
        d = n_ExactDiv(pGetCoeff(p),h,r->cf);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
    }
    n_Delete(&h,r->cf);
    if (rField_is_Q_a(r))
    {
      // special handling for alg. ext.:
      if (getCoeffType(r->cf)==n_algExt)
      {
        h = n_Init(1, r->cf->extRing->cf);
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a
          poly c_n_n=(poly)pGetCoeff(p);
          poly c_n=c_n_n;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=n_NormalizeHelper(h,pGetCoeff(c_n),r->cf->extRing->cf);
            n_Delete(&h,r->cf->extRing->cf);
            h=d;
            pIter(c_n);
          }
          pIter(p);
        }
        /* h contains the 1/lcm of all denominators in c_n_n*/
        //n_Normalize(h,r->cf->extRing->cf);
        if(!n_IsOne(h,r->cf->extRing->cf))
        {
          p=ph;
          while (p!=NULL)
          { // each monom: coeff in Q_a
            poly c_n=(poly)pGetCoeff(p);
            while (c_n!=NULL)
            { // each monom: coeff in Q
              d=n_Mult(h,pGetCoeff(c_n),r->cf->extRing->cf);
              n_Normalize(d,r->cf->extRing->cf);
              n_Delete(&pGetCoeff(c_n),r->cf->extRing->cf);
              pGetCoeff(c_n)=d;
              pIter(c_n);
            }
            pIter(p);
          }
        }
        n_Delete(&h,r->cf->extRing->cf);
      }
      /*else
      {
      // special handling for rat. functions.:
        number hzz =NULL;
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a (Z_a)
          fraction f=(fraction)pGetCoeff(p);
          poly c_n=NUM(f);
          if (hzz==NULL)
          {
            hzz=n_Copy(pGetCoeff(c_n),r->cf->extRing->cf);
            pIter(c_n);
          }
          while ((c_n!=NULL)&&(!n_IsOne(hzz,r->cf->extRing->cf)))
          { // each monom: coeff in Q (Z)
            d=n_Gcd(hzz,pGetCoeff(c_n),r->cf->extRing->cf);
            n_Delete(&hzz,r->cf->extRing->cf);
            hzz=d;
            pIter(c_n);
          }
          pIter(p);
        }
        // hzz contains the gcd of all numerators in f
        h=n_Invers(hzz,r->cf->extRing->cf);
        n_Delete(&hzz,r->cf->extRing->cf);
        n_Normalize(h,r->cf->extRing->cf);
        if(!n_IsOne(h,r->cf->extRing->cf))
        {
          p=ph;
          while (p!=NULL)
          { // each monom: coeff in Q_a (Z_a)
            fraction f=(fraction)pGetCoeff(p);
            NUM(f)=__p_Mult_nn(NUM(f),h,r->cf->extRing);
            p_Normalize(NUM(f),r->cf->extRing);
            pIter(p);
          }
        }
        n_Delete(&h,r->cf->extRing->cf);
      }*/
    }
  }
  if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
}

// Not yet?
#if 1 // currently only used by Singular/janet
void p_SimpleContent(poly ph, int smax, const ring r)
{
  if(TEST_OPT_CONTENTSB) return;
  if (ph==NULL) return;
  if (pNext(ph)==NULL)
  {
    p_SetCoeff(ph,n_Init(1,r->cf),r);
    return;
  }
  if ((pNext(pNext(ph))==NULL)||(!rField_is_Q(r)))
  {
    return;
  }
  number d=p_InitContent(ph,r);
  if (n_Size(d,r->cf)<=smax)
  {
    //if (TEST_OPT_PROT) PrintS("G");
    return;
  }

  poly p=ph;
  number h=d;
  if (smax==1) smax=2;
  while (p!=NULL)
  {
#if 0
    d=n_Gcd(h,pGetCoeff(p),r->cf);
    n_Delete(&h,r->cf);
    h = d;
#else
    STATISTIC(n_Gcd); nlInpGcd(h,pGetCoeff(p),r->cf);
#endif
    if(n_Size(h,r->cf)<smax)
    {
      //if (TEST_OPT_PROT) PrintS("g");
      return;
    }
    pIter(p);
  }
  p = ph;
  if (!n_GreaterZero(pGetCoeff(p),r->cf)) h=n_InpNeg(h,r->cf);
  if(n_IsOne(h,r->cf)) return;
  //if (TEST_OPT_PROT) PrintS("c");
  while (p!=NULL)
  {
#if 1
    d = n_ExactDiv(pGetCoeff(p),h,r->cf);
    p_SetCoeff(p,d,r);
#else
    STATISTIC(n_ExactDiv); nlInpExactDiv(pGetCoeff(p),h,r->cf); // no such function... ?
#endif
    pIter(p);
  }
  n_Delete(&h,r->cf);
}
#endif

number p_InitContent(poly ph, const ring r)
// only for coefficients in Q and rational functions
#if 0
{
  assume(!TEST_OPT_CONTENTSB);
  assume(ph!=NULL);
  assume(pNext(ph)!=NULL);
  assume(rField_is_Q(r));
  if (pNext(pNext(ph))==NULL)
  {
    return n_GetNumerator(pGetCoeff(pNext(ph)),r->cf);
  }
  poly p=ph;
  number n1=n_GetNumerator(pGetCoeff(p),r->cf);
  pIter(p);
  number n2=n_GetNumerator(pGetCoeff(p),r->cf);
  pIter(p);
  number d;
  number t;
  loop
  {
    nlNormalize(pGetCoeff(p),r->cf);
    t=n_GetNumerator(pGetCoeff(p),r->cf);
    if (nlGreaterZero(t,r->cf))
      d=nlAdd(n1,t,r->cf);
    else
      d=nlSub(n1,t,r->cf);
    nlDelete(&t,r->cf);
    nlDelete(&n1,r->cf);
    n1=d;
    pIter(p);
    if (p==NULL) break;
    nlNormalize(pGetCoeff(p),r->cf);
    t=n_GetNumerator(pGetCoeff(p),r->cf);
    if (nlGreaterZero(t,r->cf))
      d=nlAdd(n2,t,r->cf);
    else
      d=nlSub(n2,t,r->cf);
    nlDelete(&t,r->cf);
    nlDelete(&n2,r->cf);
    n2=d;
    pIter(p);
    if (p==NULL) break;
  }
  d=nlGcd(n1,n2,r->cf);
  nlDelete(&n1,r->cf);
  nlDelete(&n2,r->cf);
  return d;
}
#else
{
  /* ph has al least 2 terms */
  number d=pGetCoeff(ph);
  int s=n_Size(d,r->cf);
  pIter(ph);
  number d2=pGetCoeff(ph);
  int s2=n_Size(d2,r->cf);
  pIter(ph);
  if (ph==NULL)
  {
    if (s<s2) return n_Copy(d,r->cf);
    else      return n_Copy(d2,r->cf);
  }
  do
  {
    number nd=pGetCoeff(ph);
    int ns=n_Size(nd,r->cf);
    if (ns<=2)
    {
      s2=s;
      d2=d;
      d=nd;
      s=ns;
      break;
    }
    else if (ns<s)
    {
      s2=s;
      d2=d;
      d=nd;
      s=ns;
    }
    pIter(ph);
  }
  while(ph!=NULL);
  return n_SubringGcd(d,d2,r->cf);
}
#endif

//void pContent(poly ph)
//{
//  number h,d;
//  poly p;
//
//  p = ph;
//  if(pNext(p)==NULL)
//  {
//    pSetCoeff(p,nInit(1));
//  }
//  else
//  {
//#ifdef PDEBUG
//    if (!pTest(p)) return;
//#endif
//    nNormalize(pGetCoeff(p));
//    if(!nGreaterZero(pGetCoeff(ph)))
//    {
//      ph = pNeg(ph);
//      nNormalize(pGetCoeff(p));
//    }
//    h=pGetCoeff(p);
//    pIter(p);
//    while (p!=NULL)
//    {
//      nNormalize(pGetCoeff(p));
//      if (nGreater(h,pGetCoeff(p))) h=pGetCoeff(p);
//      pIter(p);
//    }
//    h=nCopy(h);
//    p=ph;
//    while (p!=NULL)
//    {
//      d=n_Gcd(h,pGetCoeff(p));
//      nDelete(&h);
//      h = d;
//      if(nIsOne(h))
//      {
//        break;
//      }
//      pIter(p);
//    }
//    p = ph;
//    //number tmp;
//    if(!nIsOne(h))
//    {
//      while (p!=NULL)
//      {
//        d = nExactDiv(pGetCoeff(p),h);
//        pSetCoeff(p,d);
//        pIter(p);
//      }
//    }
//    nDelete(&h);
//    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
//    {
//      pTest(ph);
//      singclap_divide_content(ph);
//      pTest(ph);
//    }
//  }
//}
#if 0
void p_Content(poly ph, const ring r)
{
  number h,d;
  poly p;

  if(pNext(ph)==NULL)
  {
    p_SetCoeff(ph,n_Init(1,r->cf),r);
  }
  else
  {
    n_Normalize(pGetCoeff(ph),r->cf);
    if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    h=n_Copy(pGetCoeff(ph),r->cf);
    p = pNext(ph);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r->cf);
      d=n_Gcd(h,pGetCoeff(p),r->cf);
      n_Delete(&h,r->cf);
      h = d;
      if(n_IsOne(h,r->cf))
      {
        break;
      }
      pIter(p);
    }
    p = ph;
    //number tmp;
    if(!n_IsOne(h,r->cf))
    {
      while (p!=NULL)
      {
        //d = nDiv(pGetCoeff(p),h);
        //tmp = nExactDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringEndS("\n")); // NOTE/TODO: use StringAppendS("\n"); omFree(s);
        //}
        //nDelete(&tmp);
        d = n_ExactDiv(pGetCoeff(p),h,r->cf);
        p_SetCoeff(p,d,r->cf);
        pIter(p);
      }
    }
    n_Delete(&h,r->cf);
    //if ( (n_GetChar(r) == 1) || (n_GetChar(r) < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    //{
    //  singclap_divide_content(ph);
    //  if(!n_GreaterZero(pGetCoeff(ph),r)) ph = p_Neg(ph,r);
    //}
  }
}
#endif
/* ---------------------------------------------------------------------------*/
/* cleardenom suff                                                           */
poly p_Cleardenom(poly p, const ring r)
{
  if( p == NULL )
    return NULL;

  assume( r != NULL ); assume( r->cf != NULL ); const coeffs C = r->cf;

#if CLEARENUMERATORS
  if( 0 )
  {
    CPolyCoeffsEnumerator itr(p);
    n_ClearDenominators(itr, C);
    n_ClearContent(itr, C); // divide out the content
    p_Test(p, r); n_Test(pGetCoeff(p), C);
    assume(n_GreaterZero(pGetCoeff(p), C)); // ??
//    if(!n_GreaterZero(pGetCoeff(p),C)) p = p_Neg(p,r);
    return p;
  }
#endif

  number d, h;

  if (rField_is_Ring(r))
  {
    p_ContentForGB(p,r);
    if(!n_GreaterZero(pGetCoeff(p),C)) p = p_Neg(p,r);
    return p;
  }

  if (rField_is_Zp(r) && TEST_OPT_INTSTRATEGY)
  {
    if(!n_GreaterZero(pGetCoeff(p),C)) p = p_Neg(p,r);
    return p;
  }

  assume(p != NULL);

  if(pNext(p)==NULL)
  {
    if (!TEST_OPT_CONTENTSB
    && !rField_is_Ring(r))
      p_SetCoeff(p,n_Init(1,r->cf),r);
    else if(!n_GreaterZero(pGetCoeff(p),C))
      p = p_Neg(p,r);
    return p;
  }

  assume(pNext(p)!=NULL);
  poly start=p;

#if 0 && CLEARENUMERATORS
//CF: does not seem to work that well..

  if( nCoeff_is_Q(C) || nCoeff_is_Q_a(C) )
  {
    CPolyCoeffsEnumerator itr(p);
    n_ClearDenominators(itr, C);
    n_ClearContent(itr, C); // divide out the content
    p_Test(p, r); n_Test(pGetCoeff(p), C);
    assume(n_GreaterZero(pGetCoeff(p), C)); // ??
//    if(!n_GreaterZero(pGetCoeff(p),C)) p = p_Neg(p,r);
    return start;
  }
#endif

  if(1)
  {
    // get lcm of all denominators ----------------------------------
    h = n_Init(1,r->cf);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r->cf);
      d=n_NormalizeHelper(h,pGetCoeff(p),r->cf);
      n_Delete(&h,r->cf);
      h=d;
      pIter(p);
    }
    /* h now contains the 1/lcm of all denominators */
    if(!n_IsOne(h,r->cf))
    {
      // multiply by the lcm of all denominators
      p = start;
      while (p!=NULL)
      {
        d=n_Mult(h,pGetCoeff(p),r->cf);
        n_Normalize(d,r->cf);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
    }
    n_Delete(&h,r->cf);
    p=start;

    p_ContentForGB(p,r);
#ifdef HAVE_RATGRING
    if (rIsRatGRing(r))
    {
      /* quick unit detection in the rational case is done in gr_nc_bba */
      p_ContentRat(p, r);
      start=p;
    }
#endif
  }

  if(!n_GreaterZero(pGetCoeff(p),C)) p = p_Neg(p,r);

  return start;
}

void p_Cleardenom_n(poly ph,const ring r,number &c)
{
  const coeffs C = r->cf;
  number d, h;

  assume( ph != NULL );

  poly p = ph;

#if CLEARENUMERATORS
  if( 0 )
  {
    CPolyCoeffsEnumerator itr(ph);

    n_ClearDenominators(itr, d, C); // multiply with common denom. d
    n_ClearContent(itr, h, C); // divide by the content h

    c = n_Div(d, h, C); // d/h

    n_Delete(&d, C);
    n_Delete(&h, C);

    n_Test(c, C);

    p_Test(ph, r); n_Test(pGetCoeff(ph), C);
    assume(n_GreaterZero(pGetCoeff(ph), C)); // ??
/*
    if(!n_GreaterZero(pGetCoeff(ph),C))
    {
      ph = p_Neg(ph,r);
      c = n_InpNeg(c, C);
    }
*/
    return;
  }
#endif


  if( pNext(p) == NULL )
  {
    if(!TEST_OPT_CONTENTSB)
    {
      c=n_Invers(pGetCoeff(p), C);
      p_SetCoeff(p, n_Init(1, C), r);
    }
    else
    {
      c=n_Init(1,C);
    }

    if(!n_GreaterZero(pGetCoeff(ph),C))
    {
      ph = p_Neg(ph,r);
      c = n_InpNeg(c, C);
    }

    return;
  }
  if (TEST_OPT_CONTENTSB) { c=n_Init(1,C); return; }

  assume( pNext(p) != NULL );

#if CLEARENUMERATORS
  if( nCoeff_is_Q(C) || nCoeff_is_Q_a(C) )
  {
    CPolyCoeffsEnumerator itr(ph);

    n_ClearDenominators(itr, d, C); // multiply with common denom. d
    n_ClearContent(itr, h, C); // divide by the content h

    c = n_Div(d, h, C); // d/h

    n_Delete(&d, C);
    n_Delete(&h, C);

    n_Test(c, C);

    p_Test(ph, r); n_Test(pGetCoeff(ph), C);
    assume(n_GreaterZero(pGetCoeff(ph), C)); // ??
/*
    if(!n_GreaterZero(pGetCoeff(ph),C))
    {
      ph = p_Neg(ph,r);
      c = n_InpNeg(c, C);
    }
*/
    return;
  }
#endif




  if(1)
  {
    h = n_Init(1,r->cf);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r->cf);
      d=n_NormalizeHelper(h,pGetCoeff(p),r->cf);
      n_Delete(&h,r->cf);
      h=d;
      pIter(p);
    }
    c=h;
    /* contains the 1/lcm of all denominators */
    if(!n_IsOne(h,r->cf))
    {
      p = ph;
      while (p!=NULL)
      {
        /* should be: // NOTE: don't use ->coef!!!!
        * number hh;
        * nGetDenom(p->coef,&hh);
        * nMult(&h,&hh,&d);
        * nNormalize(d);
        * nDelete(&hh);
        * nMult(d,p->coef,&hh);
        * nDelete(&d);
        * nDelete(&(p->coef));
        * p->coef =hh;
        */
        d=n_Mult(h,pGetCoeff(p),r->cf);
        n_Normalize(d,r->cf);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
      if (rField_is_Q_a(r))
      {
        loop
        {
          h = n_Init(1,r->cf);
          p=ph;
          while (p!=NULL)
          {
            d=n_NormalizeHelper(h,pGetCoeff(p),r->cf);
            n_Delete(&h,r->cf);
            h=d;
            pIter(p);
          }
          /* contains the 1/lcm of all denominators */
          if(!n_IsOne(h,r->cf))
          {
            p = ph;
            while (p!=NULL)
            {
              /* should be: // NOTE: don't use ->coef!!!!
              * number hh;
              * nGetDenom(p->coef,&hh);
              * nMult(&h,&hh,&d);
              * nNormalize(d);
              * nDelete(&hh);
              * nMult(d,p->coef,&hh);
              * nDelete(&d);
              * nDelete(&(p->coef));
              * p->coef =hh;
              */
              d=n_Mult(h,pGetCoeff(p),r->cf);
              n_Normalize(d,r->cf);
              p_SetCoeff(p,d,r);
              pIter(p);
            }
            number t=n_Mult(c,h,r->cf);
            n_Delete(&c,r->cf);
            c=t;
          }
          else
          {
            break;
          }
          n_Delete(&h,r->cf);
        }
      }
    }
  }

  if(!n_GreaterZero(pGetCoeff(ph),C))
  {
    ph = p_Neg(ph,r);
    c = n_InpNeg(c, C);
  }

}

  // normalization: for poly over Q: make poly primitive, integral
  //                              Qa make poly integral with leading
  //                                  coefficient minimal in N
  //                            Q(t) make poly primitive, integral

void p_ProjectiveUnique(poly ph, const ring r)
{
  if( ph == NULL )
    return;

  assume( r != NULL ); assume( r->cf != NULL );
  const coeffs C = r->cf;

  number h;
  poly p;

  if (nCoeff_is_Ring(C))
  {
    p_ContentForGB(ph,r);
    if(!n_GreaterZero(pGetCoeff(ph),C)) ph = p_Neg(ph,r);
        assume( n_GreaterZero(pGetCoeff(ph),C) );
    return;
  }

  if (nCoeff_is_Zp(C) && TEST_OPT_INTSTRATEGY)
  {
    assume( n_GreaterZero(pGetCoeff(ph),C) );
    if(!n_GreaterZero(pGetCoeff(ph),C)) ph = p_Neg(ph,r);
    return;
  }
  p = ph;

  assume(p != NULL);

  if(pNext(p)==NULL) // a monomial
  {
    p_SetCoeff(p, n_Init(1, C), r);
    return;
  }

  assume(pNext(p)!=NULL);

  if(!nCoeff_is_Q(C) && !nCoeff_is_transExt(C))
  {
    h = p_GetCoeff(p, C);
    number hInv = n_Invers(h, C);
    pIter(p);
    while (p!=NULL)
    {
      p_SetCoeff(p, n_Mult(p_GetCoeff(p, C), hInv, C), r);
      pIter(p);
    }
    n_Delete(&hInv, C);
    p = ph;
    p_SetCoeff(p, n_Init(1, C), r);
  }

  p_Cleardenom(ph, r); //removes also Content


    /* normalize ph over a transcendental extension s.t.
       lead (ph) is > 0 if extRing->cf == Q
       or lead (ph) is monic if extRing->cf == Zp*/
  if (nCoeff_is_transExt(C))
  {
    p= ph;
    h= p_GetCoeff (p, C);
    fraction f = (fraction) h;
    number n=p_GetCoeff (NUM (f),C->extRing->cf);
    if (rField_is_Q (C->extRing))
    {
      if (!n_GreaterZero(n,C->extRing->cf))
      {
        p=p_Neg (p,r);
      }
    }
    else if (rField_is_Zp(C->extRing))
    {
      if (!n_IsOne (n, C->extRing->cf))
      {
        n=n_Invers (n,C->extRing->cf);
        nMapFunc nMap;
        nMap= n_SetMap (C->extRing->cf, C);
        number ninv= nMap (n,C->extRing->cf, C);
        p=__p_Mult_nn (p, ninv, r);
        n_Delete (&ninv, C);
        n_Delete (&n, C->extRing->cf);
      }
    }
    p= ph;
  }

  return;
}

#if 0   /*unused*/
number p_GetAllDenom(poly ph, const ring r)
{
  number d=n_Init(1,r->cf);
  poly p = ph;

  while (p!=NULL)
  {
    number h=n_GetDenom(pGetCoeff(p),r->cf);
    if (!n_IsOne(h,r->cf))
    {
      number dd=n_Mult(d,h,r->cf);
      n_Delete(&d,r->cf);
      d=dd;
    }
    n_Delete(&h,r->cf);
    pIter(p);
  }
  return d;
}
#endif

int p_Size(poly p, const ring r)
{
  int count = 0;
  if (r->cf->has_simple_Alloc)
    return pLength(p);
  while ( p != NULL )
  {
    count+= n_Size( pGetCoeff( p ), r->cf );
    pIter( p );
  }
  return count;
}

/*2
*make p homogeneous by multiplying the monomials by powers of x_varnum
*assume: deg(var(varnum))==1
*/
poly p_Homogen (poly p, int varnum, const ring r)
{
  pFDegProc deg;
  if (r->pLexOrder && (r->order[0]==ringorder_lp))
    deg=p_Totaldegree;
  else
    deg=r->pFDeg;

  poly q=NULL, qn;
  int  o,ii;
  sBucket_pt bp;

  if (p!=NULL)
  {
    if ((varnum < 1) || (varnum > rVar(r)))
    {
      return NULL;
    }
    o=deg(p,r);
    q=pNext(p);
    while (q != NULL)
    {
      ii=deg(q,r);
      if (ii>o) o=ii;
      pIter(q);
    }
    q = p_Copy(p,r);
    bp = sBucketCreate(r);
    while (q != NULL)
    {
      ii = o-deg(q,r);
      if (ii!=0)
      {
        p_AddExp(q,varnum, (long)ii,r);
        p_Setm(q,r);
      }
      qn = pNext(q);
      pNext(q) = NULL;
      sBucket_Add_m(bp, q);
      q = qn;
    }
    sBucketDestroyAdd(bp, &q, &ii);
  }
  return q;
}

/*2
*tests if p is homogeneous with respect to the actual weigths
*/
BOOLEAN p_IsHomogeneous (poly p, const ring r)
{
  poly qp=p;
  int o;

  if ((p == NULL) || (pNext(p) == NULL)) return TRUE;
  pFDegProc d;
  if (r->pLexOrder && (r->order[0]==ringorder_lp))
    d=p_Totaldegree;
  else
    d=r->pFDeg;
  o = d(p,r);
  do
  {
    if (d(qp,r) != o) return FALSE;
    pIter(qp);
  }
  while (qp != NULL);
  return TRUE;
}

/*----------utilities for syzygies--------------*/
BOOLEAN   p_VectorHasUnitB(poly p, int * k, const ring r)
{
  poly q=p,qq;
  int i;

  while (q!=NULL)
  {
    if (p_LmIsConstantComp(q,r))
    {
      i = __p_GetComp(q,r);
      qq = p;
      while ((qq != q) && (__p_GetComp(qq,r) != i)) pIter(qq);
      if (qq == q)
      {
        *k = i;
        return TRUE;
      }
    }
    pIter(q);
  }
  return FALSE;
}

void   p_VectorHasUnit(poly p, int * k, int * len, const ring r)
{
  poly q=p,qq;
  int i,j=0;

  *len = 0;
  while (q!=NULL)
  {
    if (p_LmIsConstantComp(q,r))
    {
      i = __p_GetComp(q,r);
      qq = p;
      while ((qq != q) && (__p_GetComp(qq,r) != i)) pIter(qq);
      if (qq == q)
      {
       j = 0;
       while (qq!=NULL)
       {
         if (__p_GetComp(qq,r)==i) j++;
        pIter(qq);
       }
       if ((*len == 0) || (j<*len))
       {
         *len = j;
         *k = i;
       }
      }
    }
    pIter(q);
  }
}

poly p_TakeOutComp1(poly * p, int k, const ring r)
{
  poly q = *p;

  if (q==NULL) return NULL;

  poly qq=NULL,result = NULL;

  if (__p_GetComp(q,r)==k)
  {
    result = q; /* *p */
    while ((q!=NULL) && (__p_GetComp(q,r)==k))
    {
      p_SetComp(q,0,r);
      p_SetmComp(q,r);
      qq = q;
      pIter(q);
    }
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
//  if (pGetComp(q) > k) pGetComp(q)--;
  while (pNext(q)!=NULL)
  {
    if (__p_GetComp(pNext(q),r)==k)
    {
      if (result==NULL)
      {
        result = pNext(q);
        qq = result;
      }
      else
      {
        pNext(qq) = pNext(q);
        pIter(qq);
      }
      pNext(q) = pNext(pNext(q));
      pNext(qq) =NULL;
      p_SetComp(qq,0,r);
      p_SetmComp(qq,r);
    }
    else
    {
      pIter(q);
//      if (pGetComp(q) > k) pGetComp(q)--;
    }
  }
  return result;
}

poly p_TakeOutComp(poly * p, int k, const ring r)
{
  poly q = *p,qq=NULL,result = NULL;

  if (q==NULL) return NULL;
  BOOLEAN use_setmcomp=rOrd_SetCompRequiresSetm(r);
  if (__p_GetComp(q,r)==k)
  {
    result = q;
    do
    {
      p_SetComp(q,0,r);
      if (use_setmcomp) p_SetmComp(q,r);
      qq = q;
      pIter(q);
    }
    while ((q!=NULL) && (__p_GetComp(q,r)==k));
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
  if (__p_GetComp(q,r) > k)
  {
    p_SubComp(q,1,r);
    if (use_setmcomp) p_SetmComp(q,r);
  }
  poly pNext_q;
  while ((pNext_q=pNext(q))!=NULL)
  {
    if (__p_GetComp(pNext_q,r)==k)
    {
      if (result==NULL)
      {
        result = pNext_q;
        qq = result;
      }
      else
      {
        pNext(qq) = pNext_q;
        pIter(qq);
      }
      pNext(q) = pNext(pNext_q);
      pNext(qq) =NULL;
      p_SetComp(qq,0,r);
      if (use_setmcomp) p_SetmComp(qq,r);
    }
    else
    {
      /*pIter(q);*/ q=pNext_q;
      if (__p_GetComp(q,r) > k)
      {
        p_SubComp(q,1,r);
        if (use_setmcomp) p_SetmComp(q,r);
      }
    }
  }
  return result;
}

// Splits *p into two polys: *q which consists of all monoms with
// component == comp and *p of all other monoms *lq == pLength(*q)
void p_TakeOutComp(poly *r_p, long comp, poly *r_q, int *lq, const ring r)
{
  spolyrec pp, qq;
  poly p, q, p_prev;
  int l = 0;

#ifndef SING_NDEBUG
  int lp = pLength(*r_p);
#endif

  pNext(&pp) = *r_p;
  p = *r_p;
  p_prev = &pp;
  q = &qq;

  while(p != NULL)
  {
    while (__p_GetComp(p,r) == comp)
    {
      pNext(q) = p;
      pIter(q);
      p_SetComp(p, 0,r);
      p_SetmComp(p,r);
      pIter(p);
      l++;
      if (p == NULL)
      {
        pNext(p_prev) = NULL;
        goto Finish;
      }
    }
    pNext(p_prev) = p;
    p_prev = p;
    pIter(p);
  }

  Finish:
  pNext(q) = NULL;
  *r_p = pNext(&pp);
  *r_q = pNext(&qq);
  *lq = l;
#ifndef SING_NDEBUG
  assume(pLength(*r_p) + pLength(*r_q) == lp);
#endif
  p_Test(*r_p,r);
  p_Test(*r_q,r);
}

void p_DeleteComp(poly * p,int k, const ring r)
{
  poly q;

  while ((*p!=NULL) && (__p_GetComp(*p,r)==k)) p_LmDelete(p,r);
  if (*p==NULL) return;
  q = *p;
  if (__p_GetComp(q,r)>k)
  {
    p_SubComp(q,1,r);
    p_SetmComp(q,r);
  }
  while (pNext(q)!=NULL)
  {
    if (__p_GetComp(pNext(q),r)==k)
      p_LmDelete(&(pNext(q)),r);
    else
    {
      pIter(q);
      if (__p_GetComp(q,r)>k)
      {
        p_SubComp(q,1,r);
        p_SetmComp(q,r);
      }
    }
  }
}

poly p_Vec2Poly(poly v, int k, const ring r)
{
  poly h;
  poly res=NULL;

  while (v!=NULL)
  {
    if (__p_GetComp(v,r)==k)
    {
      h=p_Head(v,r);
      p_SetComp(h,0,r);
      pNext(h)=res;res=h;
    }
    pIter(v);
  }
  if (res!=NULL) res=pReverse(res);
  return res;
}

/// vector to already allocated array (len>=p_MaxComp(v,r))
// also used for p_Vec2Polys
void  p_Vec2Array(poly v, poly *p, int len, const ring r)
{
  poly h;
  int k;

  for(int i=len-1;i>=0;i--) p[i]=NULL;
  while (v!=NULL)
  {
    h=p_Head(v,r);
    k=__p_GetComp(h,r);
    if (k>len) { Werror("wrong rank:%d, should be %d",len,k); }
    else
    {
      p_SetComp(h,0,r);
      p_Setm(h,r);
      pNext(h)=p[k-1];p[k-1]=h;
    }
    pIter(v);
  }
  for(int i=len-1;i>=0;i--)
  {
    if (p[i]!=NULL) p[i]=pReverse(p[i]);
  }
}

/*2
* convert a vector to a set of polys,
* allocates the polyset, (entries 0..(*len)-1)
* the vector will not be changed
*/
void  p_Vec2Polys(poly v, poly* *p, int *len, const ring r)
{
  poly h;
  int k;

  *len=p_MaxComp(v,r);
  if (*len==0) *len=1;
  *p=(poly*)omAlloc((*len)*sizeof(poly));
  p_Vec2Array(v,*p,*len,r);
}

//
// resets the pFDeg and pLDeg: if pLDeg is not given, it is
// set to currRing->pLDegOrig, i.e. to the respective LDegProc which
// only uses pFDeg (and not p_Deg, or pTotalDegree, etc)
void pSetDegProcs(ring r, pFDegProc new_FDeg, pLDegProc new_lDeg)
{
  assume(new_FDeg != NULL);
  r->pFDeg = new_FDeg;

  if (new_lDeg == NULL)
    new_lDeg = r->pLDegOrig;

  r->pLDeg = new_lDeg;
}

// restores pFDeg and pLDeg:
void pRestoreDegProcs(ring r, pFDegProc old_FDeg, pLDegProc old_lDeg)
{
  assume(old_FDeg != NULL && old_lDeg != NULL);
  r->pFDeg = old_FDeg;
  r->pLDeg = old_lDeg;
}

/*-------- several access procedures to monomials -------------------- */
/*
* the module weights for std
*/
STATIC_VAR pFDegProc pOldFDeg;
STATIC_VAR pLDegProc pOldLDeg;
STATIC_VAR BOOLEAN pOldLexOrder;

static long pModDeg(poly p, ring r)
{
  long d=pOldFDeg(p, r);
  int c=__p_GetComp(p, r);
  if ((c>0) && ((r->pModW)->range(c-1))) d+= (*(r->pModW))[c-1];
  return d;
  //return pOldFDeg(p, r)+(*pModW)[p_GetComp(p, r)-1];
}

void p_SetModDeg(intvec *w, ring r)
{
  if (w!=NULL)
  {
    r->pModW = w;
    pOldFDeg = r->pFDeg;
    pOldLDeg = r->pLDeg;
    pOldLexOrder = r->pLexOrder;
    pSetDegProcs(r,pModDeg);
    r->pLexOrder = TRUE;
  }
  else
  {
    r->pModW = NULL;
    pRestoreDegProcs(r,pOldFDeg, pOldLDeg);
    r->pLexOrder = pOldLexOrder;
  }
}

/*2
* handle memory request for sets of polynomials (ideals)
* l is the length of *p, increment is the difference (may be negative)
*/
void pEnlargeSet(poly* *p, int l, int increment)
{
  poly* h;

  if (*p==NULL)
  {
    if (increment==0) return;
    h=(poly*)omAlloc0(increment*sizeof(poly));
  }
  else
  {
    h=(poly*)omReallocSize((poly*)*p,l*sizeof(poly),(l+increment)*sizeof(poly));
    if (increment>0)
    {
      memset(&(h[l]),0,increment*sizeof(poly));
    }
  }
  *p=h;
}

/*2
*divides p1 by its leading coefficient
*/
void p_Norm(poly p1, const ring r)
{
  if (rField_is_Ring(r))
  {
    if (!n_IsUnit(pGetCoeff(p1), r->cf)) return;
    // Werror("p_Norm not possible in the case of coefficient rings.");
  }
  else if (p1!=NULL)
  {
    if (pNext(p1)==NULL)
    {
      p_SetCoeff(p1,n_Init(1,r->cf),r);
      return;
    }
    poly h;
    if (!n_IsOne(pGetCoeff(p1),r->cf))
    {
      number k, c;
      n_Normalize(pGetCoeff(p1),r->cf);
      k = pGetCoeff(p1);
      c = n_Init(1,r->cf);
      pSetCoeff0(p1,c);
      h = pNext(p1);
      while (h!=NULL)
      {
        c=n_Div(pGetCoeff(h),k,r->cf);
        // no need to normalize: Z/p, R
        // normalize already in nDiv: Q_a, Z/p_a
        // remains: Q
        if (rField_is_Q(r) && (!n_IsOne(c,r->cf))) n_Normalize(c,r->cf);
        p_SetCoeff(h,c,r);
        pIter(h);
      }
      n_Delete(&k,r->cf);
    }
    else
    {
      //if (r->cf->cfNormalize != nDummy2) //TODO: OPTIMIZE
      {
        h = pNext(p1);
        while (h!=NULL)
        {
          n_Normalize(pGetCoeff(h),r->cf);
          pIter(h);
        }
      }
    }
  }
}

/*2
*normalize all coefficients
*/
void p_Normalize(poly p,const ring r)
{
  if (rField_has_simple_inverse(r)) return; /* Z/p, GF(p,n), R, long R/C */
  while (p!=NULL)
  {
    // no test befor n_Normalize: n_Normalize should fix problems
    n_Normalize(pGetCoeff(p),r->cf);
    pIter(p);
  }
}

// splits p into polys with Exp(n) == 0 and Exp(n) != 0
// Poly with Exp(n) != 0 is reversed
static void p_SplitAndReversePoly(poly p, int n, poly *non_zero, poly *zero, const ring r)
{
  if (p == NULL)
  {
    *non_zero = NULL;
    *zero = NULL;
    return;
  }
  spolyrec sz;
  poly z, n_z, next;
  z = &sz;
  n_z = NULL;

  while(p != NULL)
  {
    next = pNext(p);
    if (p_GetExp(p, n,r) == 0)
    {
      pNext(z) = p;
      pIter(z);
    }
    else
    {
      pNext(p) = n_z;
      n_z = p;
    }
    p = next;
  }
  pNext(z) = NULL;
  *zero = pNext(&sz);
  *non_zero = n_z;
}
/*3
* substitute the n-th variable by 1 in p
* destroy p
*/
static poly p_Subst1 (poly p,int n, const ring r)
{
  poly qq=NULL, result = NULL;
  poly zero=NULL, non_zero=NULL;

  // reverse, so that add is likely to be linear
  p_SplitAndReversePoly(p, n, &non_zero, &zero,r);

  while (non_zero != NULL)
  {
    assume(p_GetExp(non_zero, n,r) != 0);
    qq = non_zero;
    pIter(non_zero);
    qq->next = NULL;
    p_SetExp(qq,n,0,r);
    p_Setm(qq,r);
    result = p_Add_q(result,qq,r);
  }
  p = p_Add_q(result, zero,r);
  p_Test(p,r);
  return p;
}

/*3
* substitute the n-th variable by number e in p
* destroy p
*/
static poly p_Subst2 (poly p,int n, number e, const ring r)
{
  assume( ! n_IsZero(e,r->cf) );
  poly qq,result = NULL;
  number nn, nm;
  poly zero, non_zero;

  // reverse, so that add is likely to be linear
  p_SplitAndReversePoly(p, n, &non_zero, &zero,r);

  while (non_zero != NULL)
  {
    assume(p_GetExp(non_zero, n, r) != 0);
    qq = non_zero;
    pIter(non_zero);
    qq->next = NULL;
    n_Power(e, p_GetExp(qq, n, r), &nn,r->cf);
    nm = n_Mult(nn, pGetCoeff(qq),r->cf);
#ifdef HAVE_RINGS
    if (n_IsZero(nm,r->cf))
    {
      p_LmFree(&qq,r);
      n_Delete(&nm,r->cf);
    }
    else
#endif
    {
      p_SetCoeff(qq, nm,r);
      p_SetExp(qq, n, 0,r);
      p_Setm(qq,r);
      result = p_Add_q(result,qq,r);
    }
    n_Delete(&nn,r->cf);
  }
  p = p_Add_q(result, zero,r);
  p_Test(p,r);
  return p;
}


/* delete monoms whose n-th exponent is different from zero */
static poly p_Subst0(poly p, int n, const ring r)
{
  spolyrec res;
  poly h = &res;
  pNext(h) = p;

  while (pNext(h)!=NULL)
  {
    if (p_GetExp(pNext(h),n,r)!=0)
    {
      p_LmDelete(&pNext(h),r);
    }
    else
    {
      pIter(h);
    }
  }
  p_Test(pNext(&res),r);
  return pNext(&res);
}

/*2
* substitute the n-th variable by e in p
* destroy p
*/
poly p_Subst(poly p, int n, poly e, const ring r)
{
#ifdef HAVE_SHIFTBBA
  // also don't even use p_Subst0 for Letterplace
  if (rIsLPRing(r))
  {
    poly subst = p_LPSubst(p, n, e, r);
    p_Delete(&p, r);
    return subst;
  }
#endif

  if (e == NULL) return p_Subst0(p, n,r);

  if (p_IsConstant(e,r))
  {
    if (n_IsOne(pGetCoeff(e),r->cf)) return p_Subst1(p,n,r);
    else return p_Subst2(p, n, pGetCoeff(e),r);
  }

#ifdef HAVE_PLURAL
  if (rIsPluralRing(r))
  {
    return nc_pSubst(p,n,e,r);
  }
#endif

  int exponent,i;
  poly h, res, m;
  int *me,*ee;
  number nu,nu1;

  me=(int *)omAlloc((rVar(r)+1)*sizeof(int));
  ee=(int *)omAlloc((rVar(r)+1)*sizeof(int));
  if (e!=NULL) p_GetExpV(e,ee,r);
  res=NULL;
  h=p;
  while (h!=NULL)
  {
    if ((e!=NULL) || (p_GetExp(h,n,r)==0))
    {
      m=p_Head(h,r);
      p_GetExpV(m,me,r);
      exponent=me[n];
      me[n]=0;
      for(i=rVar(r);i>0;i--)
        me[i]+=exponent*ee[i];
      p_SetExpV(m,me,r);
      if (e!=NULL)
      {
        n_Power(pGetCoeff(e),exponent,&nu,r->cf);
        nu1=n_Mult(pGetCoeff(m),nu,r->cf);
        n_Delete(&nu,r->cf);
        p_SetCoeff(m,nu1,r);
      }
      res=p_Add_q(res,m,r);
    }
    p_LmDelete(&h,r);
  }
  omFreeSize((ADDRESS)me,(rVar(r)+1)*sizeof(int));
  omFreeSize((ADDRESS)ee,(rVar(r)+1)*sizeof(int));
  return res;
}

/*2
 * returns a re-ordered convertion of a number as a polynomial,
 * with permutation of parameters
 * NOTE: this only works for Frank's alg. & trans. fields
 */
poly n_PermNumber(const number z, const int *par_perm, const int , const ring src, const ring dst)
{
#if 0
  PrintS("\nSource Ring: \n");
  rWrite(src);

  if(0)
  {
    number zz = n_Copy(z, src->cf);
    PrintS("z: "); n_Write(zz, src);
    n_Delete(&zz, src->cf);
  }

  PrintS("\nDestination Ring: \n");
  rWrite(dst);

  /*Print("\nOldPar: %d\n", OldPar);
  for( int i = 1; i <= OldPar; i++ )
  {
    Print("par(%d) -> par/var (%d)\n", i, par_perm[i-1]);
  }*/
#endif
  if( z == NULL )
     return NULL;

  const coeffs srcCf = src->cf;
  assume( srcCf != NULL );

  assume( !nCoeff_is_GF(srcCf) );
  assume( src->cf->extRing!=NULL );

  poly zz = NULL;

  const ring srcExtRing = srcCf->extRing;
  assume( srcExtRing != NULL );

  const coeffs dstCf = dst->cf;
  assume( dstCf != NULL );

  if( nCoeff_is_algExt(srcCf) ) // nCoeff_is_GF(srcCf)?
  {
    zz = (poly) z;
    if( zz == NULL ) return NULL;
  }
  else if (nCoeff_is_transExt(srcCf))
  {
    assume( !IS0(z) );

    zz = NUM((fraction)z);
    p_Test (zz, srcExtRing);

    if( zz == NULL ) return NULL;
    if( !DENIS1((fraction)z) )
    {
      if (!p_IsConstant(DEN((fraction)z),srcExtRing))
        WarnS("Not defined: Cannot map a rational fraction and make a polynomial out of it! Ignoring the denominator.");
    }
  }
  else
  {
    assume (FALSE);
    WerrorS("Number permutation is not implemented for this data yet!");
    return NULL;
  }

  assume( zz != NULL );
  p_Test (zz, srcExtRing);

  nMapFunc nMap = n_SetMap(srcExtRing->cf, dstCf);

  assume( nMap != NULL );

  poly qq;
  if ((par_perm == NULL) && (rPar(dst) != 0 && rVar (srcExtRing) > 0))
  {
    int* perm;
    perm=(int *)omAlloc0((rVar(srcExtRing)+1)*sizeof(int));
    for(int i=si_min(rVar(srcExtRing),rPar(dst));i>0;i--)
      perm[i]=-i;
    qq = p_PermPoly(zz, perm, srcExtRing, dst, nMap, NULL, rVar(srcExtRing)-1);
    omFreeSize ((ADDRESS)perm, (rVar(srcExtRing)+1)*sizeof(int));
  }
  else
    qq = p_PermPoly(zz, par_perm-1, srcExtRing, dst, nMap, NULL, rVar (srcExtRing)-1);

  if(nCoeff_is_transExt(srcCf)
  && (!DENIS1((fraction)z))
  && p_IsConstant(DEN((fraction)z),srcExtRing))
  {
    number n=nMap(pGetCoeff(DEN((fraction)z)),srcExtRing->cf, dstCf);
    qq=p_Div_nn(qq,n,dst);
    n_Delete(&n,dstCf);
    p_Normalize(qq,dst);
  }
  p_Test (qq, dst);

  return qq;
}


/*2
*returns a re-ordered copy of a polynomial, with permutation of the variables
*/
poly p_PermPoly (poly p, const int * perm, const ring oldRing, const ring dst,
       nMapFunc nMap, const int *par_perm, int OldPar, BOOLEAN use_mult)
{
#if 0
    p_Test(p, oldRing);
    PrintS("p_PermPoly::p: "); p_Write(p, oldRing, oldRing);
#endif
  const int OldpVariables = rVar(oldRing);
  poly result = NULL;
  poly result_last = NULL;
  poly aq = NULL; /* the map coefficient */
  poly qq; /* the mapped monomial */
  assume(dst != NULL);
  assume(dst->cf != NULL);
  #ifdef HAVE_PLURAL
  poly tmp_mm=p_One(dst);
  #endif
  while (p != NULL)
  {
    // map the coefficient
    if ( ((OldPar == 0) || (par_perm == NULL) || rField_is_GF(oldRing) || (nMap==ndCopyMap))
    && (nMap != NULL) )
    {
      qq = p_Init(dst);
      assume( nMap != NULL );
      number n = nMap(p_GetCoeff(p, oldRing), oldRing->cf, dst->cf);
      n_Test (n,dst->cf);
      if ( nCoeff_is_algExt(dst->cf) )
        n_Normalize(n, dst->cf);
      p_GetCoeff(qq, dst) = n;// Note: n can be a ZERO!!!
    }
    else
    {
      qq = p_One(dst);
//      aq = naPermNumber(p_GetCoeff(p, oldRing), par_perm, OldPar, oldRing); // no dst???
//      poly    n_PermNumber(const number z, const int *par_perm, const int P, const ring src, const ring dst)
      aq = n_PermNumber(p_GetCoeff(p, oldRing), par_perm, OldPar, oldRing, dst);
      p_Test(aq, dst);
      if ( nCoeff_is_algExt(dst->cf) )
        p_Normalize(aq,dst);
      if (aq == NULL)
        p_SetCoeff(qq, n_Init(0, dst->cf),dst); // Very dirty trick!!!
      p_Test(aq, dst);
    }
    if (rRing_has_Comp(dst))
       p_SetComp(qq, p_GetComp(p, oldRing), dst);
    if ( n_IsZero(pGetCoeff(qq), dst->cf) )
    {
      p_LmDelete(&qq,dst);
      qq = NULL;
    }
    else
    {
      // map pars:
      int mapped_to_par = 0;
      for(int i = 1; i <= OldpVariables; i++)
      {
        int e = p_GetExp(p, i, oldRing);
        if (e != 0)
        {
          if (perm==NULL)
            p_SetExp(qq, i, e, dst);
          else if (perm[i]>0)
          {
            #ifdef HAVE_PLURAL
            if(use_mult)
            {
              p_SetExp(tmp_mm,perm[i],e,dst);
              p_Setm(tmp_mm,dst);
              qq=p_Mult_mm(qq,tmp_mm,dst);
              p_SetExp(tmp_mm,perm[i],0,dst);

            }
            else
            #endif
            p_AddExp(qq,perm[i], e/*p_GetExp( p,i,oldRing)*/, dst);
          }
          else if (perm[i]<0)
          {
            number c = p_GetCoeff(qq, dst);
            if (rField_is_GF(dst))
            {
              assume( dst->cf->extRing == NULL );
              number ee = n_Param(1, dst);
              number eee;
              n_Power(ee, e, &eee, dst->cf); //nfDelete(ee,dst);
              ee = n_Mult(c, eee, dst->cf);
              //nfDelete(c,dst);nfDelete(eee,dst);
              pSetCoeff0(qq,ee);
            }
            else if (nCoeff_is_Extension(dst->cf))
            {
              const int par = -perm[i];
              assume( par > 0 );
//              WarnS("longalg missing 3");
#if 1
              const coeffs C = dst->cf;
              assume( C != NULL );
              const ring R = C->extRing;
              assume( R != NULL );
              assume( par <= rVar(R) );
              poly pcn; // = (number)c
              assume( !n_IsZero(c, C) );
              if( nCoeff_is_algExt(C) )
                 pcn = (poly) c;
               else //            nCoeff_is_transExt(C)
                 pcn = NUM((fraction)c);
              if (pNext(pcn) == NULL) // c->z
                p_AddExp(pcn, -perm[i], e, R);
              else /* more difficult: we have really to multiply: */
              {
                poly mmc = p_ISet(1, R);
                p_SetExp(mmc, -perm[i], e, R);
                p_Setm(mmc, R);
                number nnc;
                // convert back to a number: number nnc = mmc;
                if( nCoeff_is_algExt(C) )
                   nnc = (number) mmc;
                else //            nCoeff_is_transExt(C)
                  nnc = ntInit(mmc, C);
                p_GetCoeff(qq, dst) = n_Mult((number)c, nnc, C);
                n_Delete((number *)&c, C);
                n_Delete((number *)&nnc, C);
              }
              mapped_to_par=1;
#endif
            }
          }
          else
          {
            /* this variable maps to 0 !*/
            p_LmDelete(&qq, dst);
            break;
          }
        }
      }
      if ( mapped_to_par && (qq!= NULL) && nCoeff_is_algExt(dst->cf) )
      {
        number n = p_GetCoeff(qq, dst);
        n_Normalize(n, dst->cf);
        p_GetCoeff(qq, dst) = n;
      }
    }
    pIter(p);

#if 0
    p_Test(aq,dst);
    PrintS("aq: "); p_Write(aq, dst, dst);
#endif


#if 1
    if (qq!=NULL)
    {
      p_Setm(qq,dst);

      p_Test(aq,dst);
      p_Test(qq,dst);

#if 0
    PrintS("qq: "); p_Write(qq, dst, dst);
#endif

      if (aq!=NULL)
         qq=p_Mult_q(aq,qq,dst);
      aq = qq;
      while (pNext(aq) != NULL) pIter(aq);
      if (result_last==NULL)
      {
        result=qq;
      }
      else
      {
        pNext(result_last)=qq;
      }
      result_last=aq;
      aq = NULL;
    }
    else if (aq!=NULL)
    {
      p_Delete(&aq,dst);
    }
  }
  result=p_SortAdd(result,dst);
#else
  //  if (qq!=NULL)
  //  {
  //    pSetm(qq);
  //    pTest(qq);
  //    pTest(aq);
  //    if (aq!=NULL) qq=pMult(aq,qq);
  //    aq = qq;
  //    while (pNext(aq) != NULL) pIter(aq);
  //    pNext(aq) = result;
  //    aq = NULL;
  //    result = qq;
  //  }
  //  else if (aq!=NULL)
  //  {
  //    pDelete(&aq);
  //  }
  //}
  //p = result;
  //result = NULL;
  //while (p != NULL)
  //{
  //  qq = p;
  //  pIter(p);
  //  qq->next = NULL;
  //  result = pAdd(result, qq);
  //}
#endif
  p_Test(result,dst);
#if 0
  p_Test(result,dst);
  PrintS("result: "); p_Write(result,dst,dst);
#endif
  #ifdef HAVE_PLURAL
  p_LmDelete(&tmp_mm,dst);
  #endif
  return result;
}
/**************************************************************
 *
 * Jet
 *
 **************************************************************/

poly pp_Jet(poly p, int m, const ring R)
{
  poly r=NULL;
  poly t=NULL;

  while (p!=NULL)
  {
    if (p_Totaldegree(p,R)<=m)
    {
      if (r==NULL)
        r=p_Head(p,R);
      else
      if (t==NULL)
      {
        pNext(r)=p_Head(p,R);
        t=pNext(r);
      }
      else
      {
        pNext(t)=p_Head(p,R);
        pIter(t);
      }
    }
    pIter(p);
  }
  return r;
}

poly p_Jet(poly p, int m,const ring R)
{
  while((p!=NULL) && (p_Totaldegree(p,R)>m)) p_LmDelete(&p,R);
  if (p==NULL) return NULL;
  poly r=p;
  while (pNext(p)!=NULL)
  {
    if (p_Totaldegree(pNext(p),R)>m)
    {
      p_LmDelete(&pNext(p),R);
    }
    else
      pIter(p);
  }
  return r;
}

poly pp_JetW(poly p, int m, short *w, const ring R)
{
  poly r=NULL;
  poly t=NULL;
  while (p!=NULL)
  {
    if (totaldegreeWecart_IV(p,R,w)<=m)
    {
      if (r==NULL)
        r=p_Head(p,R);
      else
      if (t==NULL)
      {
        pNext(r)=p_Head(p,R);
        t=pNext(r);
      }
      else
      {
        pNext(t)=p_Head(p,R);
        pIter(t);
      }
    }
    pIter(p);
  }
  return r;
}

poly p_JetW(poly p, int m, short *w, const ring R)
{
  while((p!=NULL) && (totaldegreeWecart_IV(p,R,w)>m)) p_LmDelete(&p,R);
  if (p==NULL) return NULL;
  poly r=p;
  while (pNext(p)!=NULL)
  {
    if (totaldegreeWecart_IV(pNext(p),R,w)>m)
    {
      p_LmDelete(&pNext(p),R);
    }
    else
      pIter(p);
  }
  return r;
}

/*************************************************************/
int p_MinDeg(poly p,intvec *w, const ring R)
{
  if(p==NULL)
    return -1;
  int d=-1;
  while(p!=NULL)
  {
    int d0=0;
    for(int j=0;j<rVar(R);j++)
      if(w==NULL||j>=w->length())
        d0+=p_GetExp(p,j+1,R);
      else
        d0+=(*w)[j]*p_GetExp(p,j+1,R);
    if(d0<d||d==-1)
      d=d0;
    pIter(p);
  }
  return d;
}

/***************************************************************/
static poly p_Invers(int n,poly u,intvec *w, const ring R)
{
  if(n<0)
    return NULL;
  number u0=n_Invers(pGetCoeff(u),R->cf);
  poly v=p_NSet(u0,R);
  if(n==0)
    return v;
  short *ww=iv2array(w,R);
  poly u1=p_JetW(p_Sub(p_One(R),__p_Mult_nn(u,u0,R),R),n,ww,R);
  if(u1==NULL)
  {
    omFreeSize((ADDRESS)ww,(rVar(R)+1)*sizeof(short));
    return v;
  }
  poly v1=__p_Mult_nn(p_Copy(u1,R),u0,R);
  v=p_Add_q(v,p_Copy(v1,R),R);
  for(int i=n/p_MinDeg(u1,w,R);i>1;i--)
  {
    v1=p_JetW(p_Mult_q(v1,p_Copy(u1,R),R),n,ww,R);
    v=p_Add_q(v,p_Copy(v1,R),R);
  }
  p_Delete(&u1,R);
  p_Delete(&v1,R);
  omFreeSize((ADDRESS)ww,(rVar(R)+1)*sizeof(short));
  return v;
}


poly p_Series(int n,poly p,poly u, intvec *w, const ring R)
{
  short *ww=iv2array(w,R);
  if(p!=NULL)
  {
    if(u==NULL)
      p=p_JetW(p,n,ww,R);
    else
      p=p_JetW(p_Mult_q(p,p_Invers(n-p_MinDeg(p,w,R),u,w,R),R),n,ww,R);
  }
  omFreeSize((ADDRESS)ww,(rVar(R)+1)*sizeof(short));
  return p;
}

BOOLEAN p_EqualPolys(poly p1,poly p2, const ring r)
{
  while ((p1 != NULL) && (p2 != NULL))
  {
    if (! p_LmEqual(p1, p2,r))
      return FALSE;
    if (! n_Equal(p_GetCoeff(p1,r), p_GetCoeff(p2,r),r->cf ))
      return FALSE;
    pIter(p1);
    pIter(p2);
  }
  return (p1==p2);
}

static inline BOOLEAN p_ExpVectorEqual(poly p1, poly p2, const ring r1, const ring r2)
{
  assume( r1 == r2 || rSamePolyRep(r1, r2) );

  p_LmCheckPolyRing1(p1, r1);
  p_LmCheckPolyRing1(p2, r2);

  int i = r1->ExpL_Size;

  assume( r1->ExpL_Size == r2->ExpL_Size );

  unsigned long *ep = p1->exp;
  unsigned long *eq = p2->exp;

  do
  {
    i--;
    if (ep[i] != eq[i]) return FALSE;
  }
  while (i);

  return TRUE;
}

BOOLEAN p_EqualPolys(poly p1,poly p2, const ring r1, const ring r2)
{
  assume( r1 == r2 || rSamePolyRep(r1, r2) ); // will be used in rEqual!
  assume( r1->cf == r2->cf );

  while ((p1 != NULL) && (p2 != NULL))
  {
    // returns 1 if ExpVector(p)==ExpVector(q): does not compare numbers !!
    // #define p_LmEqual(p1, p2, r) p_ExpVectorEqual(p1, p2, r)

    if (! p_ExpVectorEqual(p1, p2, r1, r2))
      return FALSE;

    if (! n_Equal(p_GetCoeff(p1,r1), p_GetCoeff(p2,r2), r1->cf ))
      return FALSE;

    pIter(p1);
    pIter(p2);
  }
  return (p1==p2);
}

/*2
*returns TRUE if p1 is a skalar multiple of p2
*assume p1 != NULL and p2 != NULL
*/
BOOLEAN p_ComparePolys(poly p1,poly p2, const ring r)
{
  number n,nn;
  pAssume(p1 != NULL && p2 != NULL);

  if (!p_LmEqual(p1,p2,r)) //compare leading mons
      return FALSE;
  if ((pNext(p1)==NULL) && (pNext(p2)!=NULL))
     return FALSE;
  if ((pNext(p2)==NULL) && (pNext(p1)!=NULL))
     return FALSE;
  if (pLength(p1) != pLength(p2))
    return FALSE;
  #ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    if (!n_DivBy(pGetCoeff(p1), pGetCoeff(p2), r->cf)) return FALSE;
  }
  #endif
  n=n_Div(pGetCoeff(p1),pGetCoeff(p2),r->cf);
  while ((p1 != NULL) /*&& (p2 != NULL)*/)
  {
    if ( ! p_LmEqual(p1, p2,r))
    {
        n_Delete(&n, r->cf);
        return FALSE;
    }
    if (!n_Equal(pGetCoeff(p1), nn = n_Mult(pGetCoeff(p2),n, r->cf), r->cf))
    {
      n_Delete(&n, r->cf);
      n_Delete(&nn, r->cf);
      return FALSE;
    }
    n_Delete(&nn, r->cf);
    pIter(p1);
    pIter(p2);
  }
  n_Delete(&n, r->cf);
  return TRUE;
}

/*2
* returns the length of a (numbers of monomials)
* respect syzComp
*/
poly p_Last(const poly p, int &l, const ring r)
{
  if (p == NULL)
  {
    l = 0;
    return NULL;
  }
  l = 1;
  poly a = p;
  if (! rIsSyzIndexRing(r))
  {
    poly next = pNext(a);
    while (next!=NULL)
    {
      a = next;
      next = pNext(a);
      l++;
    }
  }
  else
  {
    int curr_limit = rGetCurrSyzLimit(r);
    poly pp = a;
    while ((a=pNext(a))!=NULL)
    {
      if (__p_GetComp(a,r)<=curr_limit/*syzComp*/)
        l++;
      else break;
      pp = a;
    }
    a=pp;
  }
  return a;
}

int p_Var(poly m,const ring r)
{
  if (m==NULL) return 0;
  if (pNext(m)!=NULL) return 0;
  int i,e=0;
  for (i=rVar(r); i>0; i--)
  {
    int exp=p_GetExp(m,i,r);
    if (exp==1)
    {
      if (e==0) e=i;
      else return 0;
    }
    else if (exp!=0)
    {
      return 0;
    }
  }
  return e;
}

/*2
*the minimal index of used variables - 1
*/
int p_LowVar (poly p, const ring r)
{
  int k,l,lex;

  if (p == NULL) return -1;

  k = 32000;/*a very large dummy value*/
  while (p != NULL)
  {
    l = 1;
    lex = p_GetExp(p,l,r);
    while ((l < (rVar(r))) && (lex == 0))
    {
      l++;
      lex = p_GetExp(p,l,r);
    }
    l--;
    if (l < k) k = l;
    pIter(p);
  }
  return k;
}

/*2
* verschiebt die Indizees der Modulerzeugenden um i
*/
void p_Shift (poly * p,int i, const ring r)
{
  poly qp1 = *p,qp2 = *p;/*working pointers*/
  int     j = p_MaxComp(*p,r),k = p_MinComp(*p,r);

  if (j+i < 0) return ;
  BOOLEAN toPoly= ((j == -i) && (j == k));
  while (qp1 != NULL)
  {
    if (toPoly || (__p_GetComp(qp1,r)+i > 0))
    {
      p_AddComp(qp1,i,r);
      p_SetmComp(qp1,r);
      qp2 = qp1;
      pIter(qp1);
    }
    else
    {
      if (qp2 == *p)
      {
        pIter(*p);
        p_LmDelete(&qp2,r);
        qp2 = *p;
        qp1 = *p;
      }
      else
      {
        qp2->next = qp1->next;
        if (qp1!=NULL) p_LmDelete(&qp1,r);
        qp1 = qp2->next;
      }
    }
  }
}

/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/


static inline unsigned long GetBitFields(const long e,
                                         const unsigned int s, const unsigned int n)
{
#define Sy_bit_L(x)     (((unsigned long)1L)<<(x))
  unsigned int i = 0;
  unsigned long  ev = 0L;
  assume(n > 0 && s < BIT_SIZEOF_LONG);
  do
  {
    assume(s+i < BIT_SIZEOF_LONG);
    if (e > (long) i) ev |= Sy_bit_L(s+i);
    else break;
    i++;
  }
  while (i < n);
  return ev;
}

// Short Exponent Vectors are used for fast divisibility tests
// ShortExpVectors "squeeze" an exponent vector into one word as follows:
// Let n = BIT_SIZEOF_LONG / pVariables.
// If n == 0 (i.e. pVariables > BIT_SIZE_OF_LONG), let m == the number
// of non-zero exponents. If (m>BIT_SIZEOF_LONG), then sev = ~0, else
// first m bits of sev are set to 1.
// Otherwise (i.e. pVariables <= BIT_SIZE_OF_LONG)
// represented by a bit-field of length n (resp. n+1 for some
// exponents). If the value of an exponent is greater or equal to n, then
// all of its respective n bits are set to 1. If the value of an exponent
// is smaller than n, say m, then only the first m bits of the respective
// n bits are set to 1, the others are set to 0.
// This way, we have:
// exp1 / exp2 ==> (ev1 & ~ev2) == 0, i.e.,
// if (ev1 & ~ev2) then exp1 does not divide exp2
unsigned long p_GetShortExpVector(const poly p, const ring r)
{
  assume(p != NULL);
  unsigned long ev = 0; // short exponent vector
  unsigned int n = BIT_SIZEOF_LONG / r->N; // number of bits per exp
  unsigned int m1; // highest bit which is filled with (n+1)
  int i=0,j=1;

  if (n == 0)
  {
    if (r->N <2*BIT_SIZEOF_LONG)
    {
      n=1;
      m1=0;
    }
    else
    {
      for (; j<=r->N; j++)
      {
        if (p_GetExp(p,j,r) > 0) i++;
        if (i == BIT_SIZEOF_LONG) break;
      }
      if (i>0)
        ev = ~(0UL) >> (BIT_SIZEOF_LONG - i);
      return ev;
    }
  }
  else
  {
    m1 = (n+1)*(BIT_SIZEOF_LONG - n*r->N);
  }

  n++;
  while (i<m1)
  {
    ev |= GetBitFields(p_GetExp(p, j,r), i, n);
    i += n;
    j++;
  }

  n--;
  while (i<BIT_SIZEOF_LONG)
  {
    ev |= GetBitFields(p_GetExp(p, j,r), i, n);
    i += n;
    j++;
  }
  return ev;
}


///  p_GetShortExpVector of p * pp
unsigned long p_GetShortExpVector(const poly p, const poly pp, const ring r)
{
  assume(p != NULL);
  assume(pp != NULL);

  unsigned long ev = 0; // short exponent vector
  unsigned int n = BIT_SIZEOF_LONG / r->N; // number of bits per exp
  unsigned int m1; // highest bit which is filled with (n+1)
  int j=1;
  unsigned long i = 0L;

  if (n == 0)
  {
    if (r->N <2*BIT_SIZEOF_LONG)
    {
      n=1;
      m1=0;
    }
    else
    {
      for (; j<=r->N; j++)
      {
        if (p_GetExp(p,j,r) > 0 || p_GetExp(pp,j,r) > 0) i++;
        if (i == BIT_SIZEOF_LONG) break;
      }
      if (i>0)
        ev = ~(0UL) >> (BIT_SIZEOF_LONG - i);
      return ev;
    }
  }
  else
  {
    m1 = (n+1)*(BIT_SIZEOF_LONG - n*r->N);
  }

  n++;
  while (i<m1)
  {
    ev |= GetBitFields(p_GetExp(p, j,r) + p_GetExp(pp, j,r), i, n);
    i += n;
    j++;
  }

  n--;
  while (i<BIT_SIZEOF_LONG)
  {
    ev |= GetBitFields(p_GetExp(p, j,r) + p_GetExp(pp, j,r), i, n);
    i += n;
    j++;
  }
  return ev;
}



/***************************************************************
 *
 * p_ShallowDelete
 *
 ***************************************************************/
#undef LINKAGE
#define LINKAGE
#undef p_Delete__T
#define p_Delete__T p_ShallowDelete
#undef n_Delete__T
#define n_Delete__T(n, r) do {} while (0)

#include "polys/templates/p_Delete__T.cc"

/***************************************************************/
/*
* compare a and b
*/
int p_Compare(const poly a, const poly b, const ring R)
{
  int r=p_Cmp(a,b,R);
  if ((r==0)&&(a!=NULL))
  {
    number h=n_Sub(pGetCoeff(a),pGetCoeff(b),R->cf);
    /* compare lead coeffs */
    r = -1+n_IsZero(h,R->cf)+2*n_GreaterZero(h,R->cf); /* -1: <, 0:==, 1: > */
    n_Delete(&h,R->cf);
  }
  else if (a==NULL)
  {
    if (b==NULL)
    {
      /* compare 0, 0 */
      r=0;
    }
    else if(p_IsConstant(b,R))
    {
      /* compare 0, const */
      r = 1-2*n_GreaterZero(pGetCoeff(b),R->cf); /* -1: <, 1: > */
    }
  }
  else if (b==NULL)
  {
    if (p_IsConstant(a,R))
    {
      /* compare const, 0 */
      r = -1+2*n_GreaterZero(pGetCoeff(a),R->cf); /* -1: <, 1: > */
    }
  }
  return(r);
}

poly p_GcdMon(poly f, poly g, const ring r)
{
  assume(f!=NULL);
  assume(g!=NULL);
  assume(pNext(f)==NULL);
  poly G=p_Head(f,r);
  poly h=g;
  int *mf=(int*)omAlloc((r->N+1)*sizeof(int));
  p_GetExpV(f,mf,r);
  int *mh=(int*)omAlloc((r->N+1)*sizeof(int));
  BOOLEAN const_mon;
  BOOLEAN one_coeff=n_IsOne(pGetCoeff(G),r->cf);
  loop
  {
    if (h==NULL) break;
    if(!one_coeff)
    {
      number n=n_SubringGcd(pGetCoeff(G),pGetCoeff(h),r->cf);
      one_coeff=n_IsOne(n,r->cf);
      p_SetCoeff(G,n,r);
    }
    p_GetExpV(h,mh,r);
    const_mon=TRUE;
    for(unsigned j=r->N;j!=0;j--)
    {
      if (mh[j]<mf[j]) mf[j]=mh[j];
      if (mf[j]>0) const_mon=FALSE;
    }
    if (one_coeff && const_mon) break;
    pIter(h);
  }
  mf[0]=0;
  p_SetExpV(G,mf,r); // included is p_SetComp, p_Setm
  omFreeSize(mf,(r->N+1)*sizeof(int));
  omFreeSize(mh,(r->N+1)*sizeof(int));
  return G;
}

poly p_CopyPowerProduct(poly p, const ring r)
{
  if (p == NULL) return NULL;
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  memcpy(np->exp, p->exp, r->ExpL_Size*sizeof(long));
  pNext(np) = NULL;
  pSetCoeff0(np, n_Init(1, r->cf));
  return np;
}

int p_MaxExpPerVar(poly p, int i, const ring r)
{
  int m=0;
  while(p!=NULL)
  {
    int mm=p_GetExp(p,i,r);
    if (mm>m) m=mm;
    pIter(p);
  }
  return m;
}

