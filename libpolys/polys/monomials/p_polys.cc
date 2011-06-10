/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_polys.cc
 *  Purpose: implementation of ring independent poly procedures?
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/

#include <ctype.h>


#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#include <misc/options.h>
#include <misc/intvec.h>

#include <coeffs/longrat.h> // ???

#include "weight.h"
#include "simpleideals.h"

#include "monomials/ring.h"
#include "monomials/p_polys.h"
#include <polys/templates/p_MemCmp.h>
#include <polys/templates/p_MemAdd.h>
#include <polys/templates/p_MemCopy.h>


// #include <???/ideals.h>
// #include <???/int64vec.h>

#ifndef NDEBUG
// #include <???/febase.h>
#endif

#ifdef HAVE_PLURAL
#include "nc/nc.h"
#include "nc/sca.h"
#endif

#include "coeffrings.h"
#ifdef HAVE_FACTORY
#include "clapsing.h"
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

long p_DegW(poly p, const short *w, const ring R)
{
  long r=~0L;

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

  for (;i<=r->N;i++)
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
  assume(r->pFDeg == p_Deg);
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

/*2
* convert monomial given as string to poly, e.g. 1x3y5z
*/
const char * p_Read(const char *st, poly &rc, const ring r)
{
  if (r==NULL) { rc=NULL;return st;}
  int i,j;
  rc = p_Init(r);
  const char *s = r->cf->cfRead(st,&(rc->coef),r->cf);
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

  poly q = p;
  while (p != NULL)
  {
    number nc = pGetCoeff(p);
    pSetCoeff0(p, n_Div(nc, n, r->cf));
    n_Delete(&nc, r->cf);
    pIter(p);
  }
  p_Test(q, r);
  return q;
}

/*2
* divides a by the monomial b, ignores monomials which are not divisible
* assumes that b is not NULL, destroyes b
*/
poly p_DivideM(poly a, poly b, const ring r)
{
  if (a==NULL) { p_Delete(&b,r); return NULL; }
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
poly p_PolyDiv(poly &p, poly divisor, BOOLEAN needResult, ring r)
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
    p_SetCoeff(t, c, r);
    int e = p_GetExp(p, 1, r) - divisorLE;
    p_SetExp(t, 1, e, r);
    p_Setm(t, r);
    if (needResult) result = p_Add_q(result, p_Copy(t, r), r);
    p = p_Add_q(p, p_Neg(p_Mult_q(t, p_Copy(divisor, r), r), r), r);
  }
  return result;
}

/* returns NULL if p == NULL, otherwise makes p monic by dividing
   by its leading coefficient (only done if this is not already 1);
   this assumes that we are over a ground field so that division
   is well-defined;
   modifies p */
void p_Monic(poly &p, ring r)
{
  if (p == NULL) return;
  poly pp = p;
  number lc = p_GetCoeff(p, r);
  if (n_IsOne(lc, r->cf)) return;
  number lcInverse = n_Invers(lc, r->cf);
  number n = n_Init(1, r->cf);
  p_SetCoeff(p, n, r);   // destroys old leading coefficient!
  p = pIter(p);
  while (p != NULL)
  {
    number n = n_Mult(p_GetCoeff(p, r), lcInverse, r->cf);
    p_SetCoeff(p, n, r);   // destroys old leading coefficient!
    p = pIter(p);
  }
  n_Delete(&lcInverse, r->cf);
  p = pp;
}

/* see p_Gcd;
   additional assumption: deg(p) >= deg(q);
   must destroy p and q (unless one of them is returned) */
poly p_GcdHelper(poly &p, poly &q, ring r)
{
  if (q == NULL)
  {
    /* We have to make p monic before we return it, so that if the
       gcd is a unit in the ground field, we will actually return 1. */
    p_Monic(p, r);
    return p;
  }
  else
  {
    p_PolyDiv(p, q, FALSE, r);
    return p_GcdHelper(q, p, r);
  }
}

/* assumes that p and q are univariate polynomials in r,
   mentioning the same variable;
   assumes a global monomial ordering in r;
   assumes that not both p and q are NULL;
   returns the gcd of p and q;
   leaves p and q unmodified */
poly p_Gcd(poly p, poly q, ring r)
{
  assume((p != NULL) || (q != NULL));
  
  poly a = p; poly b = q;
  if (p_Deg(a, r) < p_Deg(b, r)) { a = q; b = p; }
  a = p_Copy(a, r); b = p_Copy(b, r);
  return p_GcdHelper(a, b, r);
}

/* see p_ExtGcd;
   additional assumption: deg(p) >= deg(q);
   must destroy p and q (unless one of them is returned) */
poly p_ExtGcdHelper(poly &p, poly &pFactor, poly &q, poly &qFactor,
                    ring r)
{
  if (q == NULL)
  {
    qFactor = NULL;
    pFactor = p_ISet(1, r);
    p_SetCoeff(pFactor, n_Invers(p_GetCoeff(p, r), r->cf), r);
    p_Monic(p, r);
    return p;
  }
  else
  {
    poly pDivQ = p_PolyDiv(p, q, TRUE, r);
    poly ppFactor = NULL; poly qqFactor = NULL;
    poly theGcd = p_ExtGcdHelper(q, qqFactor, p, ppFactor, r);
    pFactor = ppFactor;
    qFactor = p_Add_q(qqFactor,
                      p_Neg(p_Mult_q(pDivQ, p_Copy(ppFactor, r), r), r),
                      r);
    return theGcd;
  }
}

/* assumes that p and q are univariate polynomials in r,
   mentioning the same variable;
   assumes a global monomial ordering in r;
   assumes that not both p and q are NULL;
   returns the gcd of p and q;
   moreover, afterwards pFactor and qFactor contain appropriate
   factors such that gcd(p, q) = p * pFactor + q * qFactor;
   leaves p and q unmodified */
poly p_ExtGcd(poly p, poly &pFactor, poly q, poly &qFactor, ring r)
{
  assume((p != NULL) || (q != NULL));  
  poly a = p; poly b = q; BOOLEAN aCorrespondsToP = TRUE;
  if (p_Deg(a, r) < p_Deg(b, r))
  { a = q; b = p; aCorrespondsToP = FALSE; }
  a = p_Copy(a, r); b = p_Copy(b, r);
  poly aFactor = NULL; poly bFactor = NULL;
  poly theGcd = p_ExtGcdHelper(a, aFactor, b, bFactor, r);
  if (aCorrespondsToP) { pFactor = aFactor; qFactor = bFactor; }
  else                 { pFactor = bFactor; qFactor = aFactor; }
  return theGcd;
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
      bin[e] = n_IntDiv(y,x,r->cf);
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
    p_SetCoeff(ph,n_Init(1,r->cf),r);
  }
  else
  {
    n_Normalize(pGetCoeff(ph),r->cf);
    if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    if (rField_is_Q(r))
    {
      h=p_InitContent(ph,r);
      p=ph;
    }
    else if (rField_is_Extension(r)
             &&
             (
              (rPar(r)>1) || rMinpolyIsNULL(r)
             )
            )
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
    if ( (n_GetChar(r) == 1) || (n_GetChar(r) < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    {
      singclap_divide_content(ph, r);
      if(!n_GreaterZero(pGetCoeff(ph),r->cf)) ph = p_Neg(ph,r);
    }
#endif
    if (rField_is_Q_a(r))
    {
      //number hzz = nlInit(1, r->cf);
      h = nlInit(1, r->cf);
      p=ph;
      Werror("longalg missing 1");
      #if 0
      while (p!=NULL)
      { // each monom: coeff in Q_a
        lnumber c_n_n=(lnumber)pGetCoeff(p);
        poly c_n=c_n_n->z;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(hzz,pGetCoeff(c_n),r->extRing->cf);
          n_Delete(&hzz,r->extRing->cf);
          hzz=d;
          pIter(c_n);
        }
        c_n=c_n_n->n;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(h,pGetCoeff(c_n),r->extRing->cf);
          n_Delete(&h,r->extRing->cf);
          h=d;
          pIter(c_n);
        }
        pIter(p);
      }
      /* hzz contains the 1/lcm of all denominators in c_n_n->z*/
      /* h contains the 1/lcm of all denominators in c_n_n->n*/
      number htmp=nlInvers(h,r->extRing->cf);
      number hzztmp=nlInvers(hzz,r->extRing->cf);
      number hh=nlMult(hzz,h,r->extRing->cf);
      nlDelete(&hzz,r->extRing->cf);
      nlDelete(&h,r->extRing->cf);
      number hg=nlGcd(hzztmp,htmp,r->extRing->cf);
      nlDelete(&hzztmp,r->extRing->cf);
      nlDelete(&htmp,r->extRing->cf);
      h=nlMult(hh,hg,r->extRing->cf);
      nlDelete(&hg,r->extRing->cf);
      nlDelete(&hh,r->extRing->cf);
      nlNormalize(h,r->extRing->cf);
      if(!nlIsOne(h,r->extRing->cf))
      {
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a
          lnumber c_n_n=(lnumber)pGetCoeff(p);
          poly c_n=c_n_n->z;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n),r->extRing->cf);
            nlNormalize(d,r->extRing->cf);
            nlDelete(&pGetCoeff(c_n),r->extRing->cf);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          c_n=c_n_n->n;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n),r->extRing->cf);
            nlNormalize(d,r->extRing->cf);
            nlDelete(&pGetCoeff(c_n),r->extRing->cf);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          pIter(p);
        }
      }
      nlDelete(&h,r->extRing->cf);
      #endif
    }
  }
}
#if 0 // currently not used
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
#endif

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
  int s=n_ParDeg(d,r->cf);
  if (s /* n_ParDeg(d)*/ <=1) return n_Copy(d,r->cf);
  int s2=-1;
  number d2;
  int ss;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return n_Copy(d,r->cf);
      break;
    }
    if ((ss=n_ParDeg(pGetCoeff(ph),r->cf))<s)
    {
      s2=s;
      d2=d;
      s=ss;
      d=pGetCoeff(ph);
      if (s2<=1) break;
    }
  }
  return n_Gcd(d,d2,r->cf);
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
      n_Normalize(pGetCoeff(p),r->cf);
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
      if (n_GetChar(r->cf)==1)
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
      sBucket_Add_p(bp, q, 1);
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
      i = p_GetComp(q,r);
      qq = p;
      while ((qq != q) && (p_GetComp(qq,r) != i)) pIter(qq);
      if (qq == q)
      {
        *k = i;
        return TRUE;
      }
      else
        pIter(q);
    }
    else pIter(q);
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
      i = p_GetComp(q,r);
      qq = p;
      while ((qq != q) && (p_GetComp(qq,r) != i)) pIter(qq);
      if (qq == q)
      {
       j = 0;
       while (qq!=NULL)
       {
         if (p_GetComp(qq,r)==i) j++;
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

  if (p_GetComp(q,r)==k)
  {
    result = q; /* *p */
    while ((q!=NULL) && (p_GetComp(q,r)==k))
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
    if (p_GetComp(pNext(q),r)==k)
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
  if (p_GetComp(q,r)==k)
  {
    result = q;
    do
    {
      p_SetComp(q,0,r);
      if (use_setmcomp) p_SetmComp(q,r);
      qq = q;
      pIter(q);
    }
    while ((q!=NULL) && (p_GetComp(q,r)==k));
    *p = q;
    pNext(qq) = NULL;
  }
  if (q==NULL) return result;
  if (p_GetComp(q,r) > k)
  {
    p_SubComp(q,1,r);
    if (use_setmcomp) p_SetmComp(q,r);
  }
  poly pNext_q;
  while ((pNext_q=pNext(q))!=NULL)
  {
    if (p_GetComp(pNext_q,r)==k)
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
      if (p_GetComp(q,r) > k)
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

#ifdef HAVE_ASSUME
  int lp = pLength(*r_p);
#endif

  pNext(&pp) = *r_p;
  p = *r_p;
  p_prev = &pp;
  q = &qq;

  while(p != NULL)
  {
    while (p_GetComp(p,r) == comp)
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
#ifdef HAVE_ASSUME
  assume(pLength(*r_p) + pLength(*r_q) == lp);
#endif
  p_Test(*r_p,r);
  p_Test(*r_q,r);
}

void p_DeleteComp(poly * p,int k, const ring r)
{
  poly q;

  while ((*p!=NULL) && (p_GetComp(*p,r)==k)) p_LmDelete(p,r);
  if (*p==NULL) return;
  q = *p;
  if (p_GetComp(q,r)>k)
  {
    p_SubComp(q,1,r);
    p_SetmComp(q,r);
  }
  while (pNext(q)!=NULL)
  {
    if (p_GetComp(pNext(q),r)==k)
      p_LmDelete(&(pNext(q)),r);
    else
    {
      pIter(q);
      if (p_GetComp(q,r)>k)
      {
        p_SubComp(q,1,r);
        p_SetmComp(q,r);
      }
    }
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
  *p=(poly*)omAlloc0((*len)*sizeof(poly));
  while (v!=NULL)
  {
    h=p_Head(v,r);
    k=p_GetComp(h,r);
    p_SetComp(h,0,r);
    (*p)[k-1]=p_Add_q((*p)[k-1],h,r);
    pIter(v);
  }
}

/* -------------------------------------------------------- */
/*2
* change all global variables to fit the description of the new ring
*/

void p_SetGlobals(const ring r, BOOLEAN complete)
{
  if (r->ppNoether!=NULL) p_Delete(&r->ppNoether,r);

  if (complete)
  {
    test &= ~ TEST_RINGDEP_OPTS;
    test |= r->options;
  }
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
static pFDegProc pOldFDeg;
static pLDegProc pOldLDeg;
static intvec * pModW;
static BOOLEAN pOldLexOrder;

static long pModDeg(poly p, ring r)
{
  long d=pOldFDeg(p, r);
  int c=p_GetComp(p, r);
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

  h=(poly*)omReallocSize((poly*)*p,l*sizeof(poly),(l+increment)*sizeof(poly));
  if (increment>0)
  {
    //for (i=l; i<l+increment; i++)
    //  h[i]=NULL;
    memset(&(h[l]),0,increment*sizeof(poly));
  }
  *p=h;
}

/*2
*divides p1 by its leading coefficient
*/
void p_Norm(poly p1, const ring r)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    if (!n_IsUnit(pGetCoeff(p1), r->cf)) return;
    // Werror("p_Norm not possible in the case of coefficient rings.");
  }
  else
#endif
  if (p1!=NULL)
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
#ifdef LDEBUG
    n_Test(pGetCoeff(p), r->cf);
#endif
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
*returns a re-ordered copy of a polynomial, with permutation of the variables
*/
poly p_PermPoly (poly p, int * perm, const ring oldRing, const ring dst,
       nMapFunc nMap, int *par_perm, int OldPar)
{
  int OldpVariables = oldRing->N;
  poly result = NULL;
  poly result_last = NULL;
  poly aq=NULL; /* the map coefficient */
  poly qq; /* the mapped monomial */

  while (p != NULL)
  {
    if ((OldPar==0)||(rField_is_GF(oldRing)))
    {
      qq = p_Init(dst);
      number n=nMap(pGetCoeff(p),oldRing->cf,dst->cf);
      if ((!rMinpolyIsNULL(dst))
      && ((rField_is_Zp_a(dst)) || (rField_is_Q_a(dst))))
      {
        n_Normalize(n,dst->cf);
      }
      pGetCoeff(qq)=n;
    // coef may be zero:  pTest(qq);
    }
    else
    {
      qq=p_One(dst);
      WerrorS("longalg missing 2");
      #if 0
      aq=naPermNumber(pGetCoeff(p),par_perm,OldPar, oldRing);
      if ((!rMinpolyIsNULL(dst))
      && ((rField_is_Zp_a(dst)) || (rField_is_Q_a(dst))))
      {
        poly tmp=aq;
        while (tmp!=NULL)
        {
          number n=pGetCoeff(tmp);
          n_Normalize(n,dst->cf);
          pGetCoeff(tmp)=n;
          pIter(tmp);
        }
      }
      p_Test(aq,dst);
      #endif
    }
    if (rRing_has_Comp(dst)) p_SetComp(qq, p_GetComp(p,oldRing),dst);
    if (n_IsZero(pGetCoeff(qq),dst->cf))
    {
      p_LmDelete(&qq,dst);
    }
    else
    {
      int i;
      int mapped_to_par=0;
      for(i=1; i<=OldpVariables; i++)
      {
        int e=p_GetExp(p,i,oldRing);
        if (e!=0)
        {
          if (perm==NULL)
          {
            p_SetExp(qq,i, e, dst);
          }
          else if (perm[i]>0)
            p_AddExp(qq,perm[i], e/*p_GetExp( p,i,oldRing)*/, dst);
          else if (perm[i]<0)
          {
            if (rField_is_GF(dst))
            {
              number c=pGetCoeff(qq);
              number ee=n_Par(1,dst->cf);
              number eee;n_Power(ee,e,&eee,dst->cf); //nfDelete(ee,dst);
              ee=n_Mult(c,eee,dst->cf);
              //nfDelete(c,dst);nfDelete(eee,dst);
              pSetCoeff0(qq,ee);
            }
            else
            {
              WerrorS("longalg missing 3");
              #if 0
              lnumber c=(lnumber)pGetCoeff(qq);
              if (c->z->next==NULL)
                p_AddExp(c->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/,dst->extRing);
              else /* more difficult: we have really to multiply: */
              {
                lnumber mmc=(lnumber)naInit(1,dst);
                p_SetExp(mmc->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/,dst->extRing);
                p_Setm(mmc->z,dst->extRing->cf);
                pGetCoeff(qq)=n_Mult((number)c,(number)mmc,dst->cf);
                n_Delete((number *)&c,dst->cf);
                n_Delete((number *)&mmc,dst->cf);
              }
              mapped_to_par=1;
              #endif
            }
          }
          else
          {
            /* this variable maps to 0 !*/
            p_LmDelete(&qq,dst);
            break;
          }
        }
      }
      if (mapped_to_par
      && (!rMinpolyIsNULL(dst)))
      {
        number n=pGetCoeff(qq);
        n_Normalize(n,dst->cf);
        pGetCoeff(qq)=n;
      }
    }
    pIter(p);
#if 1
    if (qq!=NULL)
    {
      p_Setm(qq,dst);
      p_Test(aq,dst);
      p_Test(qq,dst);
      if (aq!=NULL) qq=p_Mult_q(aq,qq,dst);
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
  poly t=NULL;

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

poly p_Invers(int n,poly u,intvec *w, const ring R)
{
  if(n<0)
    return NULL;
  number u0=n_Invers(pGetCoeff(u),R->cf);
  poly v=p_NSet(u0,R);
  if(n==0)
    return v;
  short *ww=iv2array(w,R);
  poly u1=p_JetW(p_Sub(p_One(R),p_Mult_nn(u,u0,R),R),n,ww,R);
  if(u1==NULL)
  {
    omFreeSize((ADDRESS)ww,(rVar(R)+1)*sizeof(short));
    return v;
  }
  poly v1=p_Mult_nn(p_Copy(u1,R),u0,R);
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

BOOLEAN p_EqualPolys(poly p1,poly p2, const ring r)
{
  while ((p1 != NULL) && (p2 != NULL))
  {
    if (! p_LmEqual(p1, p2,r))
      return FALSE;
    if (! n_Equal(p_GetCoeff(p1,r), p_GetCoeff(p2,r),r ))
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
    if (!n_DivBy(p_GetCoeff(p1, r), p_GetCoeff(p2, r), r)) return FALSE;
  }
#endif
  n=n_Div(p_GetCoeff(p1,r),p_GetCoeff(p2,r),r);
  while ((p1 != NULL) /*&& (p2 != NULL)*/)
  {
    if ( ! p_LmEqual(p1, p2,r))
    {
        n_Delete(&n, r);
        return FALSE;
    }
    if (!n_Equal(p_GetCoeff(p1, r), nn = n_Mult(p_GetCoeff(p2, r),n, r), r))
    {
      n_Delete(&n, r);
      n_Delete(&nn, r);
      return FALSE;
    }
    n_Delete(&nn, r);
    pIter(p1);
    pIter(p2);
  }
  n_Delete(&n, r);
  return TRUE;
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
#define n_Delete__T(n, r) ((void)0)

#include <polys/templates/p_Delete__T.cc>

#ifdef HAVE_RINGS
/* TRUE iff LT(f) | LT(g) */
BOOLEAN p_DivisibleByRingCase(poly f, poly g, const ring r)
{
  int exponent;
  for(int i = (int)r->N; i; i--)
  {
    exponent = p_GetExp(g, i, r) - p_GetExp(f, i, r);
    if (exponent < 0) return FALSE;
  }
  return n_DivBy(p_GetCoeff(g,r), p_GetCoeff(f,r),r->cf);
}
#endif
