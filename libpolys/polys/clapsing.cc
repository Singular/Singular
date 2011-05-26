// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id$
/*
* ABSTRACT: interface between Singular and factory
*/

//#define FACTORIZE2_DEBUG
#include "config.h"
#include <misc/auxiliary.h>

#define SI_DONT_HAVE_GLOBAL_VARS
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <coeffs/coeffs.h>

// #include <kernel/ffields.h>

#include "monomials/ring.h"
#include "simpleideals.h"
//#include "polys.h"


TODO(Martin, Please adapt the following code for the use in SW)
#ifdef HAVE_FACTORY

#include <factory/factory.h>

#include "clapsing.h"
#include "clapconv.h"
// #include <kernel/clapconv.h>
// #include <libfac/factor.h>

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

static poly singclap_gcd_r ( poly f, poly g, const ring r )
{
  poly res=NULL;

  assume(f!=NULL);
  assume(g!=NULL);

  if((pNext(f)==NULL) && (pNext(g)==NULL))
  {
    poly p=p_One(r);
    for(int i=rVar(r);i>0;i--)
      p_SetExp(p,i,si_min(p_GetExp(f,i,r),p_GetExp(g,i,r)),r);
    p_Setm(p,r);
    return p;
  }

  Off(SW_RATIONAL);
  CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g, r ) );
  bool b1=isOn(SW_USE_QGCD);
  bool b2=isOn(SW_USE_fieldGCD);
  if ( rField_is_Q_a(r) ) On(SW_USE_QGCD);
  else                   On(SW_USE_fieldGCD);
  res=convFactoryPSingP( gcd( F, G ) , r);
  if (!b1) Off(SW_USE_QGCD);
  if (!b2) Off(SW_USE_fieldGCD);
  Off(SW_RATIONAL);
  return res;
}

poly singclap_gcd ( poly f, poly g, const ring r)
{
  poly res=NULL;

  if (f!=NULL) p_Cleardenom(f, r);
  if (g!=NULL) p_Cleardenom(g, r);
  else         return f; // g==0 => gcd=f (but do a p_Cleardenom)
  if (f==NULL) return g; // f==0 => gcd=g (but do a p_Cleardenom)

  res=singclap_gcd_r(f,g,r);
  p_Delete(&f, r);
  p_Delete(&g, r);
  return res;
}

/*2 find the maximal exponent of var(i) in poly p*/
int pGetExp_Var(poly p, int i, const ring r)
{
  int m=0;
  int mm;
  while (p!=NULL)
  {
    mm=p_GetExp(p,i,r);
    if (mm>m) m=mm;
    pIter(p);
  }
  return m;
}

// destroys f,g,x
poly singclap_resultant ( poly f, poly g , poly x, const ring r)
{
  poly res=NULL;
  int i=p_IsPurePower(x, r);
  if (i==0)
  {
    WerrorS("3rd argument must be a ring variable");
    goto resultant_returns_res;
  }
  if ((f==NULL) || (g==NULL))
    goto resultant_returns_res;
  {  
    Variable X(i); // TODO: consider extensions
    CanonicalForm F( convSingPFactoryP( f, r ) ), G( convSingPFactoryP( g, r ) );
    res=convFactoryPSingP( resultant( F, G, X ), r );
    Off(SW_RATIONAL);
  }
resultant_returns_res:
  p_Delete(&f,r);
  p_Delete(&g,r);
  p_Delete(&x,r);
  return res;
}
//poly singclap_resultant ( poly f, poly g , poly x)
//{
//  int i=pVar(x);
//  if (i==0)
//  {
//    WerrorS("ringvar expected");
//    return NULL;
//  }
//  ideal I=idInit(1,1);
//
//  // get the coeffs von f wrt. x:
//  I->m[0]=pCopy(f);
//  matrix ffi=mpCoeffs(I,i);
//  ffi->rank=1;
//  ffi->ncols=ffi->nrows;
//  ffi->nrows=1;
//  ideal fi=(ideal)ffi;
//
//  // get the coeffs von g wrt. x:
//  I->m[0]=pCopy(g);
//  matrix ggi=mpCoeffs(I,i);
//  ggi->rank=1;
//  ggi->ncols=ggi->nrows;
//  ggi->nrows=1;
//  ideal gi=(ideal)ggi;
//
//  // contruct the matrix:
//  int fn=IDELEMS(fi); //= deg(f,x)+1
//  int gn=IDELEMS(gi); //= deg(g,x)+1
//  matrix m=mpNew(fn+gn-2,fn+gn-2);
//  if(m==NULL)
//  {
//    return NULL;
//  }
//
//  // enter the coeffs into m:
//  int j;
//  for(i=0;i<gn-1;i++)
//  {
//    for(j=0;j<fn;j++)
//    {
//      MATELEM(m,i+1,fn-j+i)=pCopy(fi->m[j]);
//    }
//  }
//  for(i=0;i<fn-1;i++)
//  {
//    for(j=0;j<gn;j++)
//    {
//      MATELEM(m,gn+i,gn-j+i)=pCopy(gi->m[j]);
//    }
//  }
//
//  poly r=mpDet(m);
//
//  idDelete(&fi);
//  idDelete(&gi);
//  idDelete((ideal *)&m);
//  return r;
//}

BOOLEAN singclap_extgcd ( poly f, poly g, poly &res, poly &pa, poly &pb , const ring r)
{
  // for now there is only the possibility to handle univariate
  // polynomials over
  // Q and Fp ...
  res=NULL;pa=NULL;pb=NULL;
  On(SW_SYMMETRIC_FF);
  CanonicalForm F( convSingPFactoryP( f, r ) ), G( convSingPFactoryP( g, r ) );
  CanonicalForm FpG=F+G;
  if (!(FpG.isUnivariate()|| FpG.inCoeffDomain()))
  //if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
  {
    Off(SW_RATIONAL);
    WerrorS("not univariate");
    return TRUE;
  }
  CanonicalForm Fa,Gb;
  On(SW_RATIONAL);
  res=convFactoryPSingP( extgcd( F, G, Fa, Gb ), r );
  pa=convFactoryPSingP(Fa, r);
  pb=convFactoryPSingP(Gb, r);
  Off(SW_RATIONAL);
#ifndef NDEBUG
  // checking the result of extgcd:
  poly dummy;
  dummy=p_Sub(p_Add_q(pp_Mult_qq(f,pa,r),pp_Mult_qq(g,pb,r),r),p_Copy(res,r),r);
  if (dummy!=NULL)
  {
    PrintS("extgcd( ");p_Write(f,r);p_Write0(g,r);PrintS(" )\n");
    PrintS("gcd, co-factors:");p_Write(res,r); p_Write(pa,r);p_Write(pb,r);
    p_Delete(&dummy,r);
  }
#endif
  return FALSE;
}

poly singclap_pdivide ( poly f, poly g, const ring r )
{
  poly res=NULL;
  On(SW_RATIONAL);
  CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r ) );
  res = convFactoryPSingP( F / G,r );
  Off(SW_RATIONAL);
  return res;
}

void singclap_divide_content ( poly f, const ring r )
{
  if ( f==NULL )
  {
    return;
  }
  else  if ( pNext( f ) == NULL )
  {
    p_SetCoeff( f, n_Init( 1, r->cf ), r );
    return;
  }
  else
  {
    if ( rField_is_Q_a(r) )
      setCharacteristic( 0 );
    else  if ( rField_is_Zp_a(r) )
      setCharacteristic( -rChar(r) );
    else
      return; /* not implemented*/

    CFList L;
    CanonicalForm g, h;
    poly p = pNext(f);

    // first attemp: find 2 smallest g:

    number g1=pGetCoeff(f);
    number g2=pGetCoeff(p); // p==pNext(f);
    pIter(p);
    int sz1=n_Size(g1, r->cf);
    int sz2=n_Size(g2, r->cf);
    if (sz1>sz2)
    {
      number gg=g1;
      g1=g2; g2=gg;
      int sz=sz1;
      sz1=sz2; sz2=sz;
    }
    while (p!=NULL)
    {
      int n_sz=n_Size(pGetCoeff(p),r->cf);
      if (n_sz<sz1)
      {
        sz2=sz1;
        g2=g1;
        g1=pGetCoeff(p);
        sz1=n_sz;
        if (sz1<=3) break;
      }
      else if(n_sz<sz2)
      {
        sz2=n_sz;
        g2=pGetCoeff(p);
        sz2=n_sz;
      }
      pIter(p);
    }
    g = convSingPFactoryP( ((lnumber)g1)->z, r->cf->extRing );
    g = gcd( g, convSingPFactoryP( ((lnumber)g2)->z , r->cf->extRing));

    // second run: gcd's

    p = f;
    while ( (p != NULL) && (g != 1)  && ( g != 0))
    {
      h = convSingPFactoryP( ((lnumber)pGetCoeff(p))->z, r->cf->extRing );
      pIter( p );

      g = gcd( g, h );

      L.append( h );
    }
    if (( g == 1 ) || (g == 0))
    {
      // pTest(f);
      return;
    }
    else
    {
      CFListIterator i;
      for ( i = L, p = f; i.hasItem(); i++, p=pNext(p) )
      {
        lnumber c=(lnumber)pGetCoeff(p);
        p_Delete(&c->z,r->cf->extRing); // 2nd arg used to be nacRing
        c->z=convFactoryPSingP( i.getItem() / g, r->cf->extRing );
        //nTest((number)c);
        //#ifdef LDEBUG
        //number cn=(number)c;
        //StringSetS(""); nWrite(nt); StringAppend(" ==> ");
        //nWrite(cn);PrintS(StringAppend("\n"));
        //#endif
      }
    }
    // pTest(f);
  }
}

static int primepower(int c, const ring r)
{
  int p=1;
  int cc=c;
  while(cc!= rChar(r)) { cc*=c; p++; }
  return p;
}

BOOLEAN count_Factors(ideal I, intvec *v,int j, poly &f, poly fac, const ring r)
{
  p_Test(f,r);
  p_Test(fac,r);
  int e=0;
  if (!p_IsConstantPoly(fac,r))
  {
#ifdef FACTORIZE2_DEBUG
    printf("start count_Factors(%d), Fdeg=%d, factor deg=%d\n",j,pTotaldegree(f),pTotaldegree(fac));
    p_wrp(fac,currRing);PrintLn();
#endif
    On(SW_RATIONAL);
    CanonicalForm F, FAC,Q,R;
    Variable a;
    F=convSingPFactoryP( f,r );
    FAC=convSingPFactoryP( fac,r );

    poly q;
    loop
    {
      Q=F;
      Q/=FAC;
      R=Q;
      R*=FAC;
      R-=F;
      if (R.isZero())
      {
        q = convFactoryPSingP( Q,r );
        e++; p_Delete(&f,r); f=q; q=NULL; F=Q;
      }
      else
      {
        break;
      }
    }
    if (e==0)
    {
      Off(SW_RATIONAL);
      return FALSE;
    }
  }
  else e=1;
  I->m[j]=fac;
  if (v!=NULL) (*v)[j]=e;
  Off(SW_RATIONAL);
  return TRUE;
}

#ifdef HAVE_FACTORY
int singclap_factorize_retry;
#if 0
extern int libfac_interruptflag;
#endif
#endif

ideal singclap_factorize ( poly f, intvec ** v , int with_exps, const ring r)
/* destroys f, sets *v */
{
  p_Test(f,r);
#ifdef FACTORIZE2_DEBUG
  printf("singclap_factorize, degree %ld\n",pTotaldegree(f));
#endif
  // with_exps: 3,1 return only true factors, no exponents
  //            2 return true factors and exponents
  //            0 return coeff, factors and exponents
  BOOLEAN save_errorreported=errorreported;

  ideal res=NULL;

  // handle factorize(0) =========================================
  if (f==NULL)
  {
    res=idInit(1,1);
    if (with_exps!=1)
    {
      (*v)=new intvec(1);
      (**v)[0]=1;
    }
    return res;
  }
  // handle factorize(mon) =========================================
  if (pNext(f)==NULL)
  {
    int i=0;
    int n=0;
    int e;
    for(i=rVar(r);i>0;i--) if(p_GetExp(f,i,r)!=0) n++;
    if (with_exps==0) n++; // with coeff
    res=idInit(si_max(n,1),1);
    switch(with_exps)
    {
      case 0: // with coef & exp.
        res->m[0]=p_NSet(n_Copy(pGetCoeff(f),r->cf),r);
        // no break
      case 2: // with exp.
        (*v)=new intvec(si_max(1,n));
        (**v)[0]=1;
        // no break
      case 1: ;
      #ifdef TEST
      default: ;
      #endif
    }
    if (n==0)
    {
      res->m[0]=p_One(r);
      // (**v)[0]=1; is already done
    }
    else
    {
      for(i=rVar(r);i>0;i--)
      {
        e=p_GetExp(f,i,r);
        if(e!=0)
        {
          n--;
          poly p=p_One(r);
          p_SetExp(p,i,1,r);
          p_Setm(p,r);
          res->m[n]=p;
          if (with_exps!=1) (**v)[n]=e;
        }
      }
    }
    p_Delete(&f,r);
    return res;
  }
  //PrintS("S:");pWrite(f);PrintLn();
  // use factory/libfac in general ==============================
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  #ifdef HAVE_NTL
  extern int prime_number;
  if(rField_is_Q(r)) prime_number=0;
  #endif
  CFFList L;
  number N=NULL;
  number NN=NULL;
  number old_lead_coeff=n_Copy(pGetCoeff(f), r->cf);

  if (!rField_is_Zp(r) && !rField_is_Zp_a(r)) /* Q, Q(a) */
  {
    //if (f!=NULL) // already tested at start of routine
    {
      number n0=n_Copy(pGetCoeff(f),r->cf);
      if (with_exps==0)
        N=n_Copy(n0,r->cf);
      p_Cleardenom(f, r);
      NN=n_Div(n0,pGetCoeff(f),r->cf);
      n_Delete(&n0,r->cf);
      if (with_exps==0)
      {
        n_Delete(&N,r->cf);
        N=n_Copy(NN,r->cf);
      }
    }
  }
  else if (rField_is_Zp_a(r))
  {
    //if (f!=NULL) // already tested at start of routine
    if (singclap_factorize_retry==0)
    {
      number n0=n_Copy(pGetCoeff(f),r->cf);
      if (with_exps==0)
        N=n_Copy(n0,r->cf);
      p_Norm(f,r);
      p_Cleardenom(f, r);
      NN=n_Div(n0,pGetCoeff(f),r->cf);
      n_Delete(&n0,r->cf);
      if (with_exps==0)
      {
        n_Delete(&N,r->cf);
        N=n_Copy(NN,r->cf);
      }
    }
  }
  if (rField_is_Q(r) || rField_is_Zp(r))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) );
    L = factorize( F );
  }
  // and over Q(a) / Fp(a)
  else if (rField_is_Extension(r))
  {
    if (r->cf->extRing->minideal!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->minideal->m[0],
                                           r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingPFactoryP( f,r ) );
      if (rField_is_Zp_a(r))
      {
        L = factorize( F, a );
      }
      else
      {
        //  over Q(a)
        L= factorize (F, a);
      }
    }
    else
    {
      CanonicalForm F( convSingPFactoryP( f,r ) );
      L = factorize( F );
    }
  }
  else
  {
    goto notImpl;
  }
  {
    poly ff=p_Copy(f,r); // a copy for the retry stuff
    // the first factor should be a constant
    if ( ! L.getFirst().factor().inCoeffDomain() )
      L.insert(CFFactor(1,1));
    // convert into ideal
    int n = L.length();
    if (n==0) n=1;
    CFFListIterator J=L;
    int j=0;
    if (with_exps!=1)
    {
      if ((with_exps==2)&&(n>1))
      {
        n--;
        J++;
      }
      *v = new intvec( n );
    }
    res = idInit( n ,1);
    for ( ; J.hasItem(); J++, j++ )
    {
      if (with_exps!=1) (**v)[j] = J.getItem().exp();
      if (rField_is_Zp(r) || rField_is_Q(r))           /* Q, Fp */
      {
        //count_Factors(res,*v,f, j, convFactoryPSingP( J.getItem().factor() );
        res->m[j] = convFactoryPSingP( J.getItem().factor(),r );
      }
      #if 0
      else if (rField_is_GF())
        res->m[j] = convFactoryGFSingGF( J.getItem().factor() );
      #endif
      else if (rField_is_Extension(r))     /* Q(a), Fp(a) */
      {
        intvec *w=NULL;
        if (v!=NULL) w=*v;
        if (r->cf->extRing->minideal==NULL)
        {
          if(!count_Factors(res,w,j,ff,convFactoryPSingP( J.getItem().factor(),r ),r))
          {
            if (w!=NULL)
              (*w)[j]=1;
            res->m[j]=p_One(r);
          }
        }
        else
        {
          if (!count_Factors(res,w,j,ff,convFactoryPSingP( J.getItem().factor(),r ),r))
          {
            if (w!=NULL)
              (*w)[j]=1;
            res->m[j]=p_One(r);
          }
        }
      }
    }
    if (rField_is_Extension(r) && (!p_IsConstantPoly(ff,r)))
    {
      singclap_factorize_retry++;
      if (singclap_factorize_retry<3)
      {
        int jj;
        #ifdef FACTORIZE2_DEBUG
        printf("factorize_retry\n");
        #endif
        intvec *ww=NULL;
        id_Test(res,r);
        ideal h=singclap_factorize ( ff, &ww , with_exps, r );
        id_Test(h,r);
        int l=(*v)->length();
        (*v)->resize(l+ww->length());
        for(jj=0;jj<ww->length();jj++)
          (**v)[jj+l]=(*ww)[jj];
        delete ww;
        ideal hh=idInit(IDELEMS(res)+IDELEMS(h),1);
        for(jj=IDELEMS(res)-1;jj>=0;jj--)
        {
          hh->m[jj]=res->m[jj];
          res->m[jj]=NULL;
        }
        for(jj=IDELEMS(h)-1;jj>=0;jj--)
        {
          hh->m[jj+IDELEMS(res)]=h->m[jj];
          h->m[jj]=NULL;
        }
        id_Delete(&res,r);
        id_Delete(&h,r);
        res=hh;
        id_Test(res,r);
        ff=NULL;
      }
      else
      {
        WarnS("problem with factorize");
        #if 0
        pWrite(ff);
        idShow(res);
        #endif
        id_Delete(&res,r);
        res=idInit(2,1);
        res->m[0]=p_One(r);
        res->m[1]=ff; ff=NULL;
      }
    }
    p_Delete(&ff,r);
    if (N!=NULL)
    {
      p_Mult_nn(res->m[0],N,r);
      n_Delete(&N,r->cf);
      N=NULL;
    }
    // delete constants
    if (res!=NULL)
    {
      int i=IDELEMS(res)-1;
      int j=0;
      for(;i>=0;i--)
      {
        if ((res->m[i]!=NULL)
        && (pNext(res->m[i])==NULL)
        && (p_IsConstant(res->m[i],r)))
        {
          if (with_exps!=0)
          {
            p_Delete(&(res->m[i]),r);
            if ((v!=NULL) && ((*v)!=NULL))
              (**v)[i]=0;
            j++;
          }
          else if (i!=0)
          {
            while ((v!=NULL) && ((*v)!=NULL) && ((**v)[i]>1))
            {
              res->m[0]=p_Mult_q(res->m[0],p_Copy(res->m[i],r),r);
              (**v)[i]--;
            }
            res->m[0]=p_Mult_q(res->m[0],res->m[i],r);
            res->m[i]=NULL;
            if ((v!=NULL) && ((*v)!=NULL))
              (**v)[i]=1;
            j++;
          }
        }
      }
      if (j>0)
      {
        idSkipZeroes(res);
        if ((v!=NULL) && ((*v)!=NULL))
        {
          intvec *w=*v;
          int len=IDELEMS(res);
          *v = new intvec( len );
          for (i=0,j=0;i<si_min(w->length(),len);i++)
          {
            if((*w)[i]!=0)
            {
              (**v)[j]=(*w)[i]; j++;
            }
          }
          delete w;
        }
      }
      if (res->m[0]==NULL)
      {
        res->m[0]=p_One(r);
      }
    }
  }
  if (rField_is_Q_a(r) && (r->cf->extRing->minideal!=NULL))
  {
    int i=IDELEMS(res)-1;
    int stop=1;
    if (with_exps!=0) stop=0;
    for(;i>=stop;i--)
    {
      p_Norm(res->m[i],r);
    }
    if (with_exps==0) p_SetCoeff(res->m[0],old_lead_coeff,r);
    else n_Delete(&old_lead_coeff,r->cf);
  }
  else
    n_Delete(&old_lead_coeff,r->cf);
  errorreported=save_errorreported;
notImpl:
  if (res==NULL)
    WerrorS( feNotImplemented );
  if (NN!=NULL)
  {
    n_Delete(&NN,r->cf);
  }
  if (N!=NULL)
  {
    n_Delete(&N,r->cf);
  }
  if (f!=NULL) p_Delete(&f,r);
  //PrintS("......S\n");
  return res;
}
ideal singclap_sqrfree ( poly f, const ring r)
{
  p_Test(f,r);
#ifdef FACTORIZE2_DEBUG
  printf("singclap_sqrfree, degree %d\n",pTotaldegree(f));
#endif
  // with_exps: 3,1 return only true factors, no exponents
  //            2 return true factors and exponents
  //            0 return coeff, factors and exponents
  BOOLEAN save_errorreported=errorreported;

  ideal res=NULL;

  // handle factorize(0) =========================================
  if (f==NULL)
  {
    res=idInit(1,1);
    return res;
  }
  // handle factorize(mon) =========================================
  if (pNext(f)==NULL)
  {
    int i=0;
    int n=0;
    int e;
    for(i=rVar(r);i>0;i--) if(p_GetExp(f,i,r)!=0) n++;
    n++; // with coeff
    res=idInit(si_max(n,1),1);
    res->m[0]=p_NSet(n_Copy(pGetCoeff(f),r->cf),r);
    if (n==0)
    {
      res->m[0]=p_One(r);
      // (**v)[0]=1; is already done
      return res;
    }
    for(i=rVar(r);i>0;i--)
    {
      e=p_GetExp(f,i,r);
      if(e!=0)
      {
        n--;
        poly p=p_One(r);
        p_SetExp(p,i,1,r);
        p_Setm(p,r);
        res->m[n]=p;
      }
    }
    return res;
  }
  //PrintS("S:");pWrite(f);PrintLn();
  // use factory/libfac in general ==============================
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  #ifdef HAVE_NTL
  extern int prime_number;
  if(rField_is_Q(r)) prime_number=0;
  #endif
  CFFList L;

  if (!rField_is_Zp(r) && !rField_is_Zp_a(r)) /* Q, Q(a) */
  {
    //if (f!=NULL) // already tested at start of routine
    {
      p_Cleardenom(f, r);
    }
  }
  else if (rField_is_Zp_a(r))
  {
    //if (f!=NULL) // already tested at start of routine
    if (singclap_factorize_retry==0)
    {
      p_Norm(f,r);
      p_Cleardenom(f, r);
    }
  }
  if (rField_is_Q(r) || rField_is_Zp(r))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) );
    L = sqrFree( F );
  }
  #if 0
  else if (rField_is_GF())
  {
    int c=rChar(r);
    setCharacteristic( c, primepower(c) );
    CanonicalForm F( convSingGFFactoryGF( f ) );
    if (F.isUnivariate())
    {
      L = factorize( F );
    }
    else
    {
      goto notImpl;
    }
  }
  #endif
  else
  {
    goto notImpl;
  }
  {
    // convert into ideal
    int n = L.length();
    if (n==0) n=1;
    CFFListIterator J=L;
    int j=0;
    res = idInit( n ,1);
    for ( ; J.hasItem(); J++, j++ )
    {
      res->m[j] = convFactoryPSingP( J.getItem().factor(),r );
    }
    if (res->m[0]==NULL)
    {
      res->m[0]=p_One(r);
    }
  }
  p_Delete(&f,r);
  errorreported=save_errorreported;
notImpl:
  if (res==NULL)
    WerrorS( feNotImplemented );
  return res;
}


TODO(somebody, add libfac)
/*matrix singclap_irrCharSeries ( ideal I, const ring r)
{
  if (idIs0(I)) return mpNew(1,1);

  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  matrix res=NULL;
  int i;
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFList L;
  ListCFList LL;
  if (((rChar(r) == 0) || (rChar(r) > 1) )
  && (rPar(r)==0))
  {
    setCharacteristic( rChar(r) );
    for(i=0;i<IDELEMS(I);i++)
    {
      poly p=I->m[i];
      if (p!=NULL)
      {
        p=p_Copy(p,r);
        p_Cleardenom(p, r);
        L.append(convSingPFactoryP(p,r));
      }
    }
  }
  // and over Q(a) / Fp(a)
  else if (( rChar(r)==1 ) // Q(a)
  || (rChar(r) <-1))       // Fp(a)
  {
    if (rChar(r)==1) setCharacteristic( 0 );
    else               setCharacteristic( -rChar(r) );
    for(i=0;i<IDELEMS(I);i++)
    {
      poly p=I->m[i];
      if (p!=NULL)
      {
        p=p_Copy(p,r);
        p_Cleardenom(p, r);
        L.append(convSingTrPFactoryP(p,r));
      }
    }
  }
  else
  {
    WerrorS( feNotImplemented );
    return res;
  }

  // a very bad work-around --- FIX IT in libfac
  // should be fixed as of 2001/6/27
  int tries=0;
  int m,n;
  ListIterator<CFList> LLi;
  loop
  {
    LL=IrrCharSeries(L);
    m= LL.length(); // Anzahl Zeilen
    n=0;
    for ( LLi = LL; LLi.hasItem(); LLi++ )
    {
      n = si_max(LLi.getItem().length(),n);
    }
    if ((m!=0) && (n!=0)) break;
    tries++;
    if (tries>=5) break;
  }
  if ((m==0) || (n==0))
  {
    Warn("char_series returns %d x %d matrix from %d input polys (%d)",
      m,n,IDELEMS(I)+1,LL.length());
    iiWriteMatrix((matrix)I,"I",2,0);
    m=si_max(m,1);
    n=si_max(n,1);
  }
  res=mpNew(m,n);
  CFListIterator Li;
  for ( m=1, LLi = LL; LLi.hasItem(); LLi++, m++ )
  {
    for (n=1, Li = LLi.getItem(); Li.hasItem(); Li++, n++)
    {
      if ( (rChar(r) == 0) || (rChar(r) > 1) )
        MATELEM(res,m,n)=convFactoryPSingP(Li.getItem(),r);
      else
        MATELEM(res,m,n)=convFactoryPSingTrP(Li.getItem(),r);
    }
  }
  Off(SW_RATIONAL);
  return res;
}*/

/*char* singclap_neworder ( ideal I, const ring r)
{
  int i;
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFList L;
  if (((rChar(r) == 0) || (rChar(r) > 1) )
  && (rPar(r)==0))
  {
    setCharacteristic( rChar(r) );
    for(i=0;i<IDELEMS(I);i++)
    {
      L.append(convSingPFactoryP(I->m[i],r));
    }
  }
  // and over Q(a) / Fp(a)
  else if (( rChar(r)==1 ) // Q(a)
  || (rChar(r) <-1))       // Fp(a)
  {
    if (rChar(r)==1) setCharacteristic( 0 );
    else               setCharacteristic( -rChar(r) );
    for(i=0;i<IDELEMS(I);i++)
    {
      L.append(convSingTrPFactoryP(I->m[i],r));
    }
  }
  else
  {
    WerrorS( feNotImplemented );
    return NULL;
  }

  List<int> IL=neworderint(L);
  ListIterator<int> Li;
  StringSetS("");
  Li = IL;
  int offs=rPar(r);
  int* mark=(int*)omAlloc0((rVar(r)+offs)*sizeof(int));
  int cnt=rVar(r)+offs;
  loop
  {
    if(! Li.hasItem()) break;
    BOOLEAN done=TRUE;
    i=Li.getItem()-1;
    mark[i]=1;
    if (i<offs)
    {
      done=FALSE;
      //StringAppendS(r->parameter[i]);
    }
    else
    {
      StringAppendS(r->names[i-offs]);
    }
    Li++;
    cnt--;
    if(cnt==0) break;
    if (done) StringAppendS(",");
  }
  for(i=0;i<rVar(r)+offs;i++)
  {
    BOOLEAN done=TRUE;
    if(mark[i]==0)
    {
      if (i<offs)
      {
        done=FALSE;
        //StringAppendS(r->parameter[i]);
      }
      else
      {
        StringAppendS(r->names[i-offs]);
      }
      cnt--;
      if(cnt==0) break;
      if (done) StringAppendS(",");
    }
  }
  char * s=omStrDup(StringAppendS(""));
  if (s[strlen(s)-1]==',') s[strlen(s)-1]='\0';
  return s;
}*/

BOOLEAN singclap_isSqrFree(poly f, const ring r)
{
  BOOLEAN b=FALSE;
  CanonicalForm F( convSingPFactoryP( f,r ) );
  if((r->cf->type==n_Zp)&&(!F.isUnivariate()))
      goto err;
  b=(BOOLEAN)isSqrFree(F);
  Off(SW_RATIONAL);
  return b;
err:
  WerrorS( feNotImplemented );
  return 0;
}

poly singclap_det( const matrix m, const ring s )
{
  int r=m->rows();
  if (r!=m->cols())
  {
    Werror("det of %d x %d matrix",r,m->cols());
    return NULL;
  }
  poly res=NULL;
  CFMatrix M(r,r);
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=r;j>0;j--)
    {
      M(i,j)=convSingPFactoryP(MATELEM(m,i,j),s);
    }
  }
  res= convFactoryPSingP( determinant(M,r),s ) ;
  Off(SW_RATIONAL);
  return res;
}

int singclap_det_i( intvec * m)
{
  setCharacteristic( 0 );
  CFMatrix M(m->rows(),m->cols());
  int i,j;
  for(i=m->rows();i>0;i--)
  {
    for(j=m->cols();j>0;j--)
    {
      M(i,j)=IMATELEM(*m,i,j);
    }
  }
  int res= convFactoryISingI( determinant(M,m->rows()) ) ;
  Off(SW_RATIONAL);
  return res;
}

#ifdef HAVE_NTL
#if 0
matrix singntl_HNF(matrix  m, const ring s )
{
  int r=m->rows();
  if (r!=m->cols())
  {
    Werror("HNF of %d x %d matrix",r,m->cols());
    return NULL;
  }
  matrix res=mpNew(r,r);
  if (rField_is_Q(s))
  {

    CFMatrix M(r,r);
    int i,j;
    for(i=r;i>0;i--)
    {
      for(j=r;j>0;j--)
      {
        M(i,j)=convSingPFactoryP(MATELEM(m,i,j),s );
      }
    }
    CFMatrix *MM=cf_HNF(M);
    for(i=r;i>0;i--)
    {
      for(j=r;j>0;j--)
      {
        MATELEM(res,i,j)=convFactoryPSingP((*MM)(i,j),s);
      }
    }
    delete MM;
  }
  return res;
}
intvec* singntl_HNF(intvec*  m)
{
  int r=m->rows();
  if (r!=m->cols())
  {
    Werror("HNF of %d x %d matrix",r,m->cols());
    return NULL;
  }
  setCharacteristic( 0 );
  CFMatrix M(r,r);
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=r;j>0;j--)
    {
      M(i,j)=IMATELEM(*m,i,j);
    }
  }
  CFMatrix *MM=cf_HNF(M);
  intvec *mm=ivCopy(m);
  for(i=r;i>0;i--)
  {
    for(j=r;j>0;j--)
    {
      IMATELEM(*mm,i,j)=convFactoryISingI((*MM)(i,j));
    }
  }
  delete MM;
  return mm;
}
matrix singntl_LLL(matrix  m, const ring s )
{
  int r=m->rows();
  int c=m->cols();
  matrix res=mpNew(r,c);
  if (rField_is_Q(s))
  {
    CFMatrix M(r,c);
    int i,j;
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        M(i,j)=convSingPFactoryP(MATELEM(m,i,j),s);
      }
    }
    CFMatrix *MM=cf_LLL(M);
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        MATELEM(res,i,j)=convFactoryPSingP((*MM)(i,j),s);
      }
    }
    delete MM;
  }
  return res;
}
intvec* singntl_LLL(intvec*  m)
{
  int r=m->rows();
  int c=m->cols();
  setCharacteristic( 0 );
  CFMatrix M(r,c);
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=r;j>0;j--)
    {
      M(i,j)=IMATELEM(*m,i,j);
    }
  }
  CFMatrix *MM=cf_LLL(M);
  intvec *mm=ivCopy(m);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      IMATELEM(*mm,i,j)=convFactoryISingI((*MM)(i,j));
    }
  }
  delete MM;
  return mm;
}
#endif
#endif

/*
napoly singclap_alglcm ( napoly f, napoly g )
{

 // over Q(a) / Fp(a)
 if (nGetChar()==1) setCharacteristic( 0 );
 else               setCharacteristic( -nGetChar() );
 napoly res;

 if (currRing->minpoly!=NULL)
 {
   CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                         currRing->extRing);
   Variable a=rootOf(mipo);
   CanonicalForm F( convSingAFactoryA( f,a, currRing ) ),
                 G( convSingAFactoryA( g,a, currRing ) );
   CanonicalForm GCD;

   // calculate gcd
   GCD = gcd( F, G );

   // calculate lcm
   res= convFactoryASingA( (F/GCD)*G,currRing );
 }
 else
 {
   CanonicalForm F( convSingPFactoryP( f,currRing->extRing ) ),
                 G( convSingPFactoryP( g,currRing->extRing ) );
   CanonicalForm GCD;
   // calculate gcd
   GCD = gcd( F, G );

   // calculate lcm
   res= convFactoryPSingP( (F/GCD)*G, currRing->extRing );
 }

 Off(SW_RATIONAL);
 return res;
}

void singclap_algdividecontent ( napoly f, napoly g, napoly &ff, napoly &gg )
{
 // over Q(a) / Fp(a)
 if (nGetChar()==1) setCharacteristic( 0 );
 else               setCharacteristic( -nGetChar() );
 ff=gg=NULL;
 On(SW_RATIONAL);

 if (currRing->minpoly!=NULL)
 {
   CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                         currRing->extRing);
   Variable a=rootOf(mipo);
   CanonicalForm F( convSingAFactoryA( f,a, currRing ) ),
                 G( convSingAFactoryA( g,a, currRing ) );
   CanonicalForm GCD;

   GCD=gcd( F, G );

   if ((GCD!=1) && (GCD!=0))
   {
     ff= convFactoryASingA( F/ GCD, currRing );
     gg= convFactoryASingA( G/ GCD, currRing );
   }
 }
 else
 {
   CanonicalForm F( convSingPFactoryP( f,currRing->extRing ) ),
                 G( convSingPFactoryP( g,currRing->extRing ) );
   CanonicalForm GCD;

   GCD=gcd( F, G );

   if ((GCD!=1) && (GCD!=0))
   {
     ff= convFactoryPSingP( F/ GCD, currRing->extRing );
     gg= convFactoryPSingP( G/ GCD, currRing->extRing );
   }
 }

 Off(SW_RATIONAL);
}
*/

#if 0
lists singclap_chineseRemainder(lists x, lists q)
{
  //assume(x->nr == q->nr);
  //assume(x->nr >= 0);
  int n=x->nr+1;
  if ((x->nr<0) || (x->nr!=q->nr))
  {
    WerrorS("list are empty or not of equal length");
    return NULL;
  }
  lists res=(lists)omAlloc0Bin(slists_bin);
  CFArray X(1,n), Q(1,n);
  int i;
  for(i=0; i<n; i++)
  {
    if (x->m[i-1].Typ()==INT_CMD)
    {
      X[i]=(int)x->m[i-1].Data();
    }
    else if (x->m[i-1].Typ()==NUMBER_CMD)
    {
      number N=(number)x->m[i-1].Data();
      X[i]=convSingNFactoryN(N);
    }
    else
    {
      WerrorS("illegal type in chineseRemainder");
      omFreeBin(res,slists_bin);
      return NULL;
    }
    if (q->m[i-1].Typ()==INT_CMD)
    {
      Q[i]=(int)q->m[i-1].Data();
    }
    else if (q->m[i-1].Typ()==NUMBER_CMD)
    {
      number N=(number)x->m[i-1].Data();
      Q[i]=convSingNFactoryN(N);
    }
    else
    {
      WerrorS("illegal type in chineseRemainder");
      omFreeBin(res,slists_bin);
      return NULL;
    }
  }
  CanonicalForm r, prod;
  chineseRemainder( X, Q, r, prod );
  res->Init(2);
  res->m[0].rtyp=NUMBER_CMD;
  res->m[1].rtyp=NUMBER_CMD;
  res->m[0].data=(char *)convFactoryNSingN( r );
  res->m[1].data=(char *)convFactoryNSingN( prod );
  return res;
}
#endif
#endif

number   nChineseRemainder(number *x, number *q,int rl, const coeffs r)
// elemenst in the array are x[0..(rl-1)], q[0..(rl-1)]
{
#ifdef HAVE_FACTORY
  if (r->type!=n_Q)
  { Werror("nChineseRemainder only for integers"); return NULL; }
  setCharacteristic( 0 ); // only in char 0
  CFArray X(rl), Q(rl);
  int i;
  for(i=rl-1;i>=0;i--)
  {
    X[i]=r->convSingNFactoryN(x[i],FALSE,r); // may be larger MAX_INT
    Q[i]=r->convSingNFactoryN(q[i],FALSE,r); // may be larger MAX_INT
  }
  CanonicalForm xnew,qnew;
  chineseRemainder(X,Q,xnew,qnew);
  number n=r->convFactoryNSingN(xnew,r);
  number p=r->convFactoryNSingN(qnew,r);
  number p2=n_IntDiv(p,n_Init(2, r),r);
  if (n_Greater(n,p2,r))
  {
     number n2=n_Sub(n,p,r);
     n_Delete(&n,r);
     n=n2;
  }
  n_Delete(&p,r);
  n_Delete(&p2,r);
  return n;
#else
  WerrorS("not implemented");
  return n_Init(0,r);
#endif
}
