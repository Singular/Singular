/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.cc
 *  Purpose: implementation of currRing independent poly procedures
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/


#include <auxialiary.h>

#include "ring.h"
#include "p_polys.h"
#include "ring.h"
#include "ideals.h"
#include "int64vec.h"
#ifndef NDEBUG
#include <kernel/febase.h>
#endif

/***************************************************************
 *
 * Completing what needs to be set for the monomial
 *
 ***************************************************************/
// this is special for the syz stuff
static int* _components = NULL;
static long* _componentsShifted = NULL;
static int _componentsExternal = 0;

BOOLEAN pSetm_error=0;

#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */

void p_Setm_General(poly p, const ring r)
{
  p_LmCheckPolyRing(p, r);
  int pos=0;
  if (r->typ!=NULL)
  {
    loop
    {
      long ord=0;
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
          for(int i=a;i<=e;i++) ord+=p_GetExp(p,i,r)*w[i-a];
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
//            if(p_Setm_error) Print("***************************\n
//                                    ***************************\n
//                                    **WARNING: overflow error**\n
//                                    ***************************\n
//                                    ***************************\n");
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
          int c=p_GetComp(p,r);
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
          const unsigned long c = p_GetComp(p, r);
          const short place = o->data.syz.place;
          const int limit = o->data.syz.limit;
          
          if (c > limit)
            p->exp[place] = o->data.syz.curr_index;
          else if (c > 0)
          {
            assume( (1 <= c) && (c <= limit) );
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

#ifndef NDEBUG
#if MYTEST
          Print("p_Setm_General: isTemp ord: pos: %d, p: ", pos);  p_DebugPrint(p, r, r, 1);
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
	   
#ifndef NDEBUG
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
//          {
//            PrintS("Initial Value: "); p_DebugPrint(p, r, r, 1);
//          }
#endif
#endif
          break;
        }

        // Suffix for Induced Schreyer ordering
        case ro_is:
        {
#ifndef NDEBUG
#if MYTEST
          Print("p_Setm_General: ro_is ord: pos: %d, p: ", pos);  p_DebugPrint(p, r, r, 1);
#endif
#endif

          assume(p != NULL);

          int c = p_GetComp(p, r);

          assume( c >= 0 );
          const ideal F = o->data.is.F;
          const int limit = o->data.is.limit;

          if( F != NULL && c > limit )
          {
#ifndef NDEBUG
#if MYTEST
            Print("p_Setm_General: ro_is : in rSetm: pos: %d, c: %d >  limit: %d\n", c, pos, limit); // p_DebugPrint(p, r, r, 1);
#endif
#endif

            c -= limit;
            assume( c > 0 );
            c--;

            assume( c < IDELEMS(F) ); // What about others???

            const poly pp = F->m[c]; // get reference monomial!!!

#ifndef NDEBUG
#if MYTEST
            Print("Respective F[c - %d: %d] pp: ", limit, c); 
            p_DebugPrint(pp, r, r, 1);
#endif
#endif


            assume(pp != NULL);
            if(pp == NULL) break;

            const int start = o->data.is.start;
            const int end = o->data.is.end;

            assume(start <= end);
	     
//          const int limit = o->data.is.limit;
          assume( limit >= 0 );

//	  const int st = o->data.isTemp.start;	     

          if( c > limit )
            p->exp[start] = 1;
//          else
//            p->exp[start] = 0;

	     
#ifndef NDEBUG
	    Print("p_Setm_General: is(-Temp-) :: c: %d, limit: %d, [st:%d] ===>>> %ld\n", c, limit, start, p->exp[start]);
#endif	     
   

            for( int i = start; i <= end; i++) // v[0] may be here...
              p->exp[i] += pp->exp[i]; // !!!!!!!! ADD corresponding LT(F)

       

	     
#ifndef NDEBUG
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
#endif
          } else
          {
            const int* const pVarOffset = o->data.is.pVarOffset;

            // What about v[0] - component: it will be added later by
            // suffix!!!
            // TODO: Test it!
            const int vo = pVarOffset[0];
            if( vo != -1 )
              p->exp[vo] = c; // initial component v[0]!

#ifndef NDEBUG
#if MYTEST
	    Print("p_Setm_General: ro_is :: c: %d <= limit: %d, vo: %d, exp: %d\n", c, limit, vo, p->exp[vo]);
            p_DebugPrint(p, r, r, 1);
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

p_SetmProc p_GetSetmProc(ring r)
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
  assume(p_GetOrder(a, r) == p_WTotaldegree(a, r));
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
      case ringorder_c:
      case ringorder_C:
      case ringorder_S:
      case ringorder_s:
      case ringorder_IS:
      case ringorder_aa:
        break;
      case ringorder_a:
        for (k=b0 /*r->block0[i]*/;k<=b1 /*r->block1[i]*/;k++)
        { // only one line
          j+= p_GetExp(p,k, r)*r->wvhdl[i][ k- b0 /*r->block0[i]*/];
        }
        //break;
        return j;

#ifndef NDEBUG
      default:
        Print("missing order %d in p_WTotaldegree\n",r->order[i]);
        break;
#endif
    }
  }
  return  j;
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

  for (;i<=r->N;i++)
    j+=p_GetExp(p,i, r)*pWeight(i, r);

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
    while ((pNext(p)!=NULL) && (p_GetComp(pNext(p), r)==k))
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
#ifdef PDEBUG
  _p_Test(p,r,PDEBUG);
#endif
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
      if (p_GetComp(p, r)<=curr_limit/*syzComp*/)
        ll++;
      else break;
      pp = p;
    }
#ifdef PDEBUG
    _p_Test(pp,r,PDEBUG);
#endif
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
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
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
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
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
      if (p_GetComp(p, r)<=limit)
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
  assume(r->pFDeg == pDeg);
  p_CheckPolyRing(p, r);
  long k= p_GetComp(p, r);
  int ll=1;
  long  t,max;

  max=p_GetOrder(p, r);
  if (k > 0)
  {
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
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
  assume(r->pFDeg == pDeg);
  p_CheckPolyRing(p, r);
  int ll=1;
  long  t,max;

  max=p_GetOrder(p, r);
  if (rIsSyzIndexRing(r))
  {
    long limit = rGetCurrSyzLimit(r);
    while ((p=pNext(p))!=NULL)
    {
      if (p_GetComp(p, r)<=limit)
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
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
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
      if (p_GetComp(p, r)<=limit)
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
    while (((p=pNext(p))!=NULL) && (p_GetComp(p, r)==k))
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
      if (p_GetComp(p, r)<=limit)
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
*test if a monomial /head term is a pure power
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
poly p_ISet(int i, const ring r)
{
  poly rc = NULL;
  if (i!=0)
  {
    rc = p_Init(r);
    pSetCoeff0(rc,n_Init(i,r));
    if (r->cf->nIsZero(p_GetCoeff(rc,r)))
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
  pSetCoeff0(rc,n_Init(1,r));
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

/*2
* convert monomial given as string to poly, e.g. 1x3y5z
*/
const char * p_Read(const char *st, poly &rc, const ring r)
{
  if (r==NULL) { rc=NULL;return st;}
  int i,j;
  rc = p_Init(r);
  const char *s = r->cf->nRead(st,&(rc->coef));
  if (s==st)
  /* i.e. it does not start with a coeff: test if it is a ringvar*/
  {
    j = r_IsRingVar(s,r);
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
    j = r_IsRingVar(ss,r);
    if (j >= 0)
    {
      const char *s_save=s;
      s = eati(s,&i);
      if (((unsigned long)i) >  r->bitmask)
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
      p_LmDelete(&rc,r);
      s--;
      return s;
    }
  }
done:
  if (r->cf->nIsZero(pGetCoeff(rc))) p_LmDelete(&rc,r);
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
  #ifdef PDEBUG
  _p_Test(p,r,PDEBUG);
  #endif
  ok=!errorreported;
  return p;
}

/*2
* returns a polynomial representing the number n
* destroys n
*/
poly p_NSet(number n, const ring r)
{
  if (r->cf->nIsZero(n))
  {
    r->cf->cfDelete(&n, r);
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
* assumes that the head term of b is a multiple of the head term of a
* and return the multiplicant *m
* Frank's observation: If LM(b) = LM(a)*m, then we may actually set
* negative(!) exponents in the below loop. I suspect that the correct
* comment should be "assumes that LM(a) = LM(b)*m, for some monomial m..."
*/
poly p_Divide(poly a, poly b, const ring r)
{
  assume((p_GetComp(a,r)==p_GetComp(b,r)) || (p_GetComp(b,r)==0));
  int i;
  poly result = pInit();

  for(i=(int)r->N; i; i--)
    p_SetExp(result,i, p_GetExp(a,i,r)- p_GetExp(b,i,r),r);
  p_SetComp(result, p_GetComp(a,r) - p_GetComp(b,r),r);
  p_Setm(result,r);
  return result;
}

/*2
* divides a by the monomial b, ignores monomials which are not divisible
* assumes that b is not NULL
*/
poly p_DivideM(poly a, poly b, const ring r)
{
  if (a==NULL) return NULL;
  poly result=a;
  poly prev=NULL;
  int i;
#ifdef HAVE_RINGS
  number inv=pGetCoeff(b);
#else
  number inv=n_Invers(pGetCoeff(b),r->cf);
#endif

  while (a!=NULL)
  {
    if (p_DivisibleBy(b,a,r))
    {
      for(i=(int)r->N; i; i--)
         p_SubExp(a,i, p_GetExp(b,i,r),r);
      p_SubComp(a, p_GetComp(b,r),r);
      p_Setm(a,r);
      prev=a;
      pIter(a);
    }
    else
    {
      if (prev==NULL)
      {
        p_DeleteLm(&result,r);
        a=result;
      }
      else
      {
        p_DeleteLm(&pNext(prev),r);
        a=pNext(prev);
      }
    }
  }
#ifdef HAVE_RINGS
  if (n_IsUnit(inv,r->cf))
  {
    inv = n_Invers(inv,r->cf);
    p_Mult_nn(result,inv,r);
    n_Delete(&inv, r->cf);
  }
  else
  {
    p_Div_nn(result,inv,r);
  }
#else
  p_Mult_nn(result,inv,r);
  n_Delete(&inv, r->cf);
#endif
  p_Delete(&b, r);
  return result;
}

/*2
* returns the LCM of the head terms of a and b in *m
*/
void p_Lcm(poly a, poly b, poly m, const ring r)
{
  int i;
  for (i=rVar(r); i; i--)
  {
    p_SetExp(m,i, si_max( p_GetExp(a,i,r), p_GetExp(b,i,r)),r);
  }
  p_SetComp(m, si_max(p_GetComp(a,r), p_GetComp(b,r)),r);
  /* Don't do a pSetm here, otherwise hres/lres chockes */
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

static poly pDiffOpM(poly a, poly b,BOOLEAN multiply, const ring r)
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

  if(!n_IsOne(pGetCoeff(p),r))
  {
    number x, y;
    y = pGetCoeff(p);
    n_Power(y,exp,&x,r);
    n_Delete(&y,r);
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
  int i;

  y = pGetCoeff(p);
  x = n_Mult(y,pGetCoeff(q),r);
  n_Delete(&y,r);
  pSetCoeff0(p,x);
  //for (i=pVariables; i!=0; i--)
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
  int i;
  poly r = p_Init(rr);

  x = n_Mult(pGetCoeff(p),pGetCoeff(q),rr);
  pSetCoeff0(r,x);
  p_ExpVectorSum(r,p, q, rr);
  return r;
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
  number *bin = pnBin(exp);

  tail = pNext(p);
  if (bin == NULL)
  {
    p_MonPower(p,exp,r);
    p_MonPower(tail,exp,r);
#ifdef PDEBUG
    p_Test(p,r);
#endif
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
    x = n_Mult(bin[exp-e],pGetCoeff(h),r);
    p_SetCoeff(h,x,r);
    p_MonMult(h,b,r);
    res = pNext(res) = h;
    p_MonMult(b,tail,r);
  }
  for (e=eh; e!=0; e--)
  {
    h = a[e];
    x = n_Mult(bin[e],pGetCoeff(h),r);
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
  pnFreeBin(bin, exp);
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
#ifdef PDEBUG
  p_Test(res,r);
#endif
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
    if ( (i > 0) && ((unsigned long ) i > (r->bitmask)))
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
          if (rIsPluralRing(r)) /* in the NC case nothing helps :-( */
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
          if ((pNext(rc) != NULL)
#ifdef HAVE_RINGS
             || rField_is_Ring(r)
#endif
             )
            return p_Pow(p,i,r);
          if ((char_p==0) || (i<=char_p))
            return p_TwoMonPower(p,i,r);
          poly p_p=p_TwoMonPower(p_Copy(p,r),char_p,r);
          return p_Mult_q(p_Power(p,i-char_p,r),p_p,r);
        }
      /*end default:*/
    }
  }
  return rc;
}

/* --------------------------------------------------------------------------------*/
/* content suff                                                                   */

static number p_InitContent(poly ph, const ring r);
static number p_InitContent_a(poly ph, const ring r);

void p_Content(poly ph, const ring r)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    if ((ph!=NULL) && rField_has_Units(r))
    {
      number k = nGetUnit(pGetCoeff(ph));
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
      }
      n_Delete(&k,r->cf);
    }
    return;
  }
#endif
  number h,d;
  poly p;

  if(TEST_OPT_CONTENTSB) return;
  if(pNext(ph)==NULL)
  {
    p_SetCoeff(ph,n_Init(1,r),r->cf);
  }
  else
  {
    n_Normalize(pGetCoeff(ph),r->cf);
    if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    if (rField_is_Q())
    {
      h=p_InitContent(ph,r);
      p=ph;
    }
    else if ((rField_is_Extension(r))
    && ((rPar(r)>1)||(r->minpoly==NULL)))
    {
      h=p_InitContent_a(ph,r);
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
        //tmp = nIntDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringAppendS("\n"));
        //}
        //nDelete(&tmp);
        d = n_IntDiv(pGetCoeff(p),h,r->cf);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
    }
    n_Delete(&h,r->cf);
#ifdef HAVE_FACTORY
    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    {
      singclap_divide_content(ph);
      if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    }
#endif
    if (rField_is_Q_a(r))
    {
      number hzz = nlInit(1, r->cf);
      h = nlInit(1, r->cf);
      p=ph;
      while (p!=NULL)
      { // each monom: coeff in Q_a
        lnumber c_n_n=(lnumber)pGetCoeff(p);
        napoly c_n=c_n_n->z;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(hzz,pGetCoeff(c_n),r->algring);
          n_Delete(&hzz,r->algring);
          hzz=d;
          pIter(c_n);
        }
        c_n=c_n_n->n;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(h,pGetCoeff(c_n),r->algring);
          n_Delete(&h,r->algring);
          h=d;
          pIter(c_n);
        }
        pIter(p);
      }
      /* hzz contains the 1/lcm of all denominators in c_n_n->z*/
      /* h contains the 1/lcm of all denominators in c_n_n->n*/
      number htmp=nlInvers(h);
      number hzztmp=nlInvers(hzz);
      number hh=nlMult(hzz,h);
      nlDelete(&hzz,r->algring);
      nlDelete(&h,r->algring);
      number hg=nlGcd(hzztmp,htmp,r->algring);
      nlDelete(&hzztmp,r->algring);
      nlDelete(&htmp,r->algring);
      h=nlMult(hh,hg);
      nlDelete(&hg,r->algring);
      nlDelete(&hh,r->algring);
      nlNormalize(h);
      if(!nlIsOne(h))
      {
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a
          lnumber c_n_n=(lnumber)pGetCoeff(p);
          napoly c_n=c_n_n->z;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n));
            nlNormalize(d);
            nlDelete(&pGetCoeff(c_n),r->algring);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          c_n=c_n_n->n;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n));
            nlNormalize(d);
            nlDelete(&pGetCoeff(c_n),r->algring);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          pIter(p);
        }
      }
      nlDelete(&h,r->algring);
    }
  }
}
void p_SimpleContent(poly ph,int smax, const ring r)
{
  if(TEST_OPT_CONTENTSB) return;
  if (ph==NULL) return;
  if (pNext(ph)==NULL)
  {
    p_SetCoeff(ph,n_Init(1,r_cf),r);
    return;
  }
  if ((pNext(pNext(ph))==NULL)||(!rField_is_Q(r)))
  {
    return;
  }
  number d=p_InitContent(ph,r);
  if (nlSize(d,r->cf)<=smax)
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
    d=nlGcd(h,pGetCoeff(p),r->cf);
    nlDelete(&h,r->cf);
    h = d;
#else
    nlInpGcd(h,pGetCoeff(p),r->cf);
#endif
    if(nlSize(h,r->cf)<smax)
    {
      //if (TEST_OPT_PROT) PrintS("g");
      return;
    }
    pIter(p);
  }
  p = ph;
  if (!nlGreaterZero(pGetCoeff(p),r->cf)) h=nlNeg(h,r->cf);
  if(nlIsOne(h,r->cf)) return;
  //if (TEST_OPT_PROT) PrintS("c");
  while (p!=NULL)
  {
#if 1
    d = nlIntDiv(pGetCoeff(p),h,r->cf);
    p_SetCoeff(p,d,r);
#else
    nlInpIntDiv(pGetCoeff(p),h,r->cf);
#endif
    pIter(p);
  }
  nlDelete(&h,r->cf);
}

static number p_InitContent(poly ph, const ring r)
// only for coefficients in Q
#if 0
{
  assume(!TEST_OPT_CONTENTSB);
  assume(ph!=NULL);
  assume(pNext(ph)!=NULL);
  assume(rField_is_Q(r));
  if (pNext(pNext(ph))==NULL)
  {
    return nlGetNom(pGetCoeff(pNext(ph)),r->cf);
  }
  poly p=ph;
  number n1=nlGetNom(pGetCoeff(p),r->cf);
  pIter(p);
  number n2=nlGetNom(pGetCoeff(p),r->cf);
  pIter(p);
  number d;
  number t;
  loop
  {
    nlNormalize(pGetCoeff(p),r->cf);
    t=nlGetNom(pGetCoeff(p),r->cf);
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
    t=nlGetNom(pGetCoeff(p),r->cf);
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
  number d=pGetCoeff(ph);
  if(SR_HDL(d)&SR_INT) return d;
  int s=mpz_size1(d->z);
  int s2=-1;
  number d2;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return nlCopy(d,r->cf);
      break;
    }
    if (SR_HDL(pGetCoeff(ph))&SR_INT)
    {
      s2=s;
      d2=d;
      s=0;
      d=pGetCoeff(ph);
      if (s2==0) break;
    }
    else
    if (mpz_size1((pGetCoeff(ph)->z))<=s)
    {
      s2=s;
      d2=d;
      d=pGetCoeff(ph);
      s=mpz_size1(d->z);
    }
  }
  return nlGcd(d,d2,r->cf);
}
#endif

number p_InitContent_a(poly ph, const ring r)
// only for coefficients in K(a) anf K(a,...)
{
  number d=pGetCoeff(ph);
  int s=naParDeg(d);
  if (s /* naParDeg(d)*/ <=1) return naCopy(d);
  int s2=-1;
  number d2;
  int ss;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return naCopy(d);
      break;
    }
    if ((ss=naParDeg(pGetCoeff(ph)))<s)
    {
      s2=s;
      d2=d;
      s=ss;
      d=pGetCoeff(ph);
      if (s2<=1) break;
    }
  }
  return naGcd(d,d2,r->cf);
}


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
//      d=nGcd(h,pGetCoeff(p));
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
//        d = nIntDiv(pGetCoeff(p),h);
//        pSetCoeff(p,d);
//        pIter(p);
//      }
//    }
//    nDelete(&h);
//#ifdef HAVE_FACTORY
//    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
//    {
//      pTest(ph);
//      singclap_divide_content(ph);
//      pTest(ph);
//    }
//#endif
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
        //tmp = nIntDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringAppendS("\n"));
        //}
        //nDelete(&tmp);
        d = n_IntDiv(pGetCoeff(p),h,r->cf);
        p_SetCoeff(p,d,r->cf);
        pIter(p);
      }
    }
    n_Delete(&h,r->cf);
#ifdef HAVE_FACTORY
    //if ( (n_GetChar(r) == 1) || (n_GetChar(r) < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    //{
    //  singclap_divide_content(ph);
    //  if(!n_GreaterZero(pGetCoeff(ph),r)) ph = p_Neg(ph,r);
    //}
#endif
  }
}
#endif
/* ---------------------------------------------------------------------------*/
/* cleardenom suff                                                           */
poly p_Cleardenom(poly ph, const ring r)
{
  poly start=ph;
  number d, h;
  poly p;

#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    p_Content(ph,r);
    return start;
  }
#endif
  if (rField_is_Zp(r) && TEST_OPT_INTSTRATEGY) return start;
  p = ph;
  if(pNext(p)==NULL)
  {
    if (TEST_OPT_CONTENTSB)
    {
      number n=n_GetDenom(pGetCoeff(p),r->cf);
      if (!n_IsOne(n,r->cf))
      {
        number nn=n_Mult(pGetCoeff(p),n,r->cf);
        n_Normalize(nn,r->cf);
        p_SetCoeff(p,nn,r);
      }
      n_Delete(&n,r->cf);
    }
    else
      p_SetCoeff(p,n_Init(1,r->cf),r);
  }
  else
  {
    h = n_Init(1,r->cf);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p,r->cf));
      d=n_Lcm(h,pGetCoeff(p),r->cf);
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
        /* should be:
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
      n_Delete(&h,r->cf);
      if (nGetChar()==1)
      {
        loop
        {
          h = n_Init(1,r->cf);
          p=ph;
          while (p!=NULL)
          {
            d=n_Lcm(h,pGetCoeff(p),r->cf);
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
              /* should be:
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
            n_Delete(&h,r->cf);
          }
          else
          {
            n_Delete(&h,r->cf);
            break;
          }
        }
      }
    }
    if (h!=NULL) n_Delete(&h,r->cf);
  
    p_Content(ph,r);
#ifdef HAVE_RATGRING
    if (rIsRatGRing(r))
    {
      /* quick unit detection in the rational case is done in gr_nc_bba */
      pContentRat(ph);
      start=ph;
    }
#endif
  }
  return start;
}

void p_Cleardenom_n(poly ph,const ring r,number &c)
{
  number d, h;
  poly p;

  p = ph;
  if(pNext(p)==NULL)
  {
    c=n_Invers(pGetCoeff(p),r->cf);
    p_SetCoeff(p,n_Init(1,r->cf),r);
  }
  else
  {
    h = n_Init(1,r->cf);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r->cf);
      d=n_Lcm(h,pGetCoeff(p),r->cf);
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
        /* should be:
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
            d=n_Lcm(h,pGetCoeff(p),r->cf);
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
              /* should be:
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
}

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

int p_Size(poly p, const ring r)
{
  int count = 0;
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
  if (pLexOrder && (r->order[0]==ringorder_lp))
    deg=p_Totaldegree;
  else
    deg=pFDeg;

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
      sBucket_Add_p(bp, q, 1);
      q = qn;
    }
    sBucketDestroyAdd(bp, &q, &ii);
  }
  return q;
}

/*4
*Returns the exponent of the maximal power of the leading monomial of
*p2 in that of p1
*/
static int p_GetMaxPower (poly p1,poly p2, const ring r)
{
  int     i,k,res = MAX_INT; /*a very large integer*/

  if (p1 == NULL) return 0;
  for (i=rVar(r); i>0; i--)
  {
    if ( pGetExp(p2,i) != 0)
    {
      k =  p_GetExp(p1,i,r) /  p_GetExp(p2,i,r);
      if (k < res) res = k;
    }
  }
  return res;
}

/*2
*returns the leading monomial of p1 divided by the maximal power of that
*of p2
*/
poly p_DivByMonom (poly p1,poly p2, const ring r)
{
  int     k, i;

  if (p1 == NULL) return NULL;
  k = p_GetMaxPower(p1,p2,r);
  if (k == 0)
    return p_Head(p1,r);
  else
  {
    number n;
    poly p = pInit(r);

    p->next = NULL;
    for (i=rVar(r);i>0; i--)
    {
       p_SetExp(p,i, p_GetExp(p1,i,r)-k* p_GetExp(p2,i,r),r);
    }
    n_Power(p2->coef,k,&n,r->cf);
    pSetCoeff0(p,n_Div(p1->coef,n,r->cf));
    n_Delete(&n,r->cf);
    p_Setm(p,r);
    return p;
  }
}

/*2
*Returns as i-th entry of P the coefficient of the (i-1) power of
*the leading monomial of p2 in p1
*/
void p_CancelPolyByMonom (poly p1,poly p2,polyset * P,int * SizeOfSet, const ring r)
{
  int   maxPow;
  poly  p,qp,Coeff;

  if (*P == NULL)
  {
    *P = (polyset) omAlloc0(5*sizeof(poly));
    *SizeOfSet = 5;
  }
  p = p_Copy(p1,r);
  while (p != NULL)
  {
    qp = pNext(p);
    pNext(p) = NULL;
    maxPow = p_GetMaxPower(p,p2,r);
    Coeff = p_DivByMonom(p,p2,r);
    if (maxPow > *SizeOfSet)
    {
      pEnlargeSet(P,*SizeOfSet,maxPow+1-*SizeOfSet);
      *SizeOfSet = maxPow+1;
    }
    (*P)[maxPow] = p_Add_q((*P)[maxPow],Coeff,r);
    p_Delete(&p,r);
    p = qp;
  }
}
/*2
*replaces the maximal powers of the leading monomial of p2 in p1 by
*the same powers of n, utility for dehomogenization
*/
poly p_Dehomogen (poly p1,poly p2,number n, const ring r)
{
  polyset P;
  int     SizeOfSet=5;
  int     i;
  poly    p;
  number  nn;

  P = (polyset)omAlloc0(5*sizeof(poly));
  p_CancelPolyByMonom(p1,p2,&P,&SizeOfSet,r);
  p = P[0];
  for (i=1; i<SizeOfSet; i++)
  {
    if (P[i] != NULL)
    {
      nPower(n,i,&nn);
      pMult_nn(P[i],nn);
      p = pAdd(p,P[i]);
      //P[i] =NULL; // for safety, may be removed later
      nDelete(&nn);
    }
  }
  omFreeSize((ADDRESS)P,SizeOfSet*sizeof(poly));
  return p;
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
  if (pLexOrder && (r->order[0]==ringorder_lp))
    d=p_Totaldegree;
  else 
    d=pFDeg;
  o = d(p,currRing);
  do
  {
    if (d(qp,r) != o) return FALSE;
    pIter(qp);
  }
  while (qp != NULL);
  return TRUE;
}

/***************************************************************
 *
 * p_ShallowDelete
 *
 ***************************************************************/
#undef LINKAGE
#define LINKAGE
#undef p_Delete
#define p_Delete p_ShallowDelete
#undef n_Delete
#define n_Delete(n, r) ((void)0)

#include <kernel/p_Delete__T.cc>

