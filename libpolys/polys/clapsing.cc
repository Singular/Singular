// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: interface between Singular and factory
*/

//#define FACTORIZE2_DEBUG

#include "misc/auxiliary.h"
#include "clapsing.h"

#include "factory/factory.h"
#include "factory/cf_roots.h"

#include "coeffs/numbers.h"
#include "coeffs/coeffs.h"
#include "coeffs/bigintmat.h"

#include "monomials/ring.h"
#include "simpleideals.h"
#include "polys/flintconv.h"
#include "polys/flint_mpoly.h"


//#include "polys.h"
#define TRANSEXT_PRIVATES

#include "ext_fields/transext.h"


#include "clapconv.h"

#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"
#include "polys/simpleideals.h"
#include "misc/intvec.h"
#include "polys/matpol.h"
#include "coeffs/bigintmat.h"


void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

poly singclap_gcd_r ( poly f, poly g, const ring r )
{
  poly res=NULL;

  assume(f!=NULL);
  assume(g!=NULL);

  if(pNext(f)==NULL)
  {
    return p_GcdMon(f,g,r);
  }
  else if(pNext(g)==NULL)
  {
    return p_GcdMon(g,f,r);
  }
  #ifdef HAVE_FLINT
  #if __FLINT_RELEASE >= 20503
  if (rField_is_Zp(r) && (r->cf->ch>10))
  {
    nmod_mpoly_ctx_t ctx;
    if (!convSingRFlintR(ctx,r))
    {
      // leading coef. 1
      return Flint_GCD_MP(f,pLength(f),g,pLength(g),ctx,r);
    }
  }
  else
  if (rField_is_Q(r))
  {
    fmpq_mpoly_ctx_t ctx;
    if (!convSingRFlintR(ctx,r))
    {
      // leading coef. positive, all coeffs in Z
      poly res=Flint_GCD_MP(f,pLength(f),g,pLength(g),ctx,r);
      res=p_Cleardenom(res,r);
      return res;
    }
  }
  #endif
  #endif
  Off(SW_RATIONAL);
  if (rField_is_Q(r) || rField_is_Zp(r) || rField_is_Z(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g, r ) );
    res=convFactoryPSingP( gcd( F, G ) , r);
    if ( rField_is_Zp(r))
      p_Norm(res,r); // leading coef. 1
    else if (rField_is_Q(r) && (!n_GreaterZero(pGetCoeff(res),r->cf)))
      res = p_Neg(res,r); // leading coef. positive, all coeffs in Z
  }
  // and over Q(a) / Fp(a)
  else if ( r->cf->extRing!=NULL )
  {
    if ( rField_is_Q_a(r)) setCharacteristic( 0 );
    else                   setCharacteristic( rChar(r) );
    if (r->cf->extRing->qideal!=NULL)
    {
      bool b1=isOn(SW_USE_QGCD);
      if ( rField_is_Q_a(r) ) On(SW_USE_QGCD);
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                           r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      res= convFactoryAPSingAP( gcd( F, G ),r );
      prune (a);
      if (!b1) Off(SW_USE_QGCD);
      if ( rField_is_Zp_a(r)) p_Norm(res,r); // leading coef. 1
    }
    else
    {
      convSingTrP(f,r);
      convSingTrP(g,r);
      CanonicalForm F( convSingTrPFactoryP( f,r ) ), G( convSingTrPFactoryP( g,r ) );
      res= convFactoryPSingTrP( gcd( F, G ),r );
    }
  }
  else if (r->cf->convSingNFactoryN==ndConvSingNFactoryN)
    WerrorS( feNotImplemented );
  else
  { // handle user type coeffs:
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g, r ) );
    res=convFactoryPSingP( gcd( F, G ) , r);
  }
  Off(SW_RATIONAL);
  return res;
}

poly singclap_gcd_and_divide ( poly& f, poly& g, const ring r)
{
  poly res=NULL;

  if (g == NULL)
  {
    res= f;
    f=p_One (r);
    return res;
  }
  if (f==NULL)
  {
    res= g;
    g=p_One (r);
    return res;
  }
  if (pNext(g)==NULL)
  {
    poly G=p_GcdMon(g,f,r);
    if (!n_IsOne(pGetCoeff(G),r->cf)
    || (!p_IsConstant(G,r)))
    {
      f=p_Div_mm(f,G,r);
      g=p_Div_mm(g,G,r);
    }
    return G;
  }
  else if (pNext(f)==NULL)
  {
    poly G=p_GcdMon(f,g,r);
    if (!n_IsOne(pGetCoeff(G),r->cf)
    || (!p_IsConstant(G,r)))
    {
      f=p_Div_mm(f,G,r);
      g=p_Div_mm(g,G,r);
    }
    return G;
  }

  Off(SW_RATIONAL);
  CanonicalForm F,G,GCD;
  if (rField_is_Q(r) || (rField_is_Zp(r))
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    bool b1=isOn(SW_USE_EZGCD_P);
    setCharacteristic( rChar(r) );
    F=convSingPFactoryP( f,r );
    G=convSingPFactoryP( g,r );
    GCD=gcd(F,G);
    if (!GCD.isOne())
    {
      p_Delete(&f,r);
      p_Delete(&g,r);
      if (getCharacteristic() == 0)
        On (SW_RATIONAL);
      F /= GCD;
      G /= GCD;
      if (getCharacteristic() == 0)
      {
        CanonicalForm denF= bCommonDen (F);
        CanonicalForm denG= bCommonDen (G);
        G *= denG;
        F *= denF;
        Off (SW_RATIONAL);
        CanonicalForm gcddenFdenG= gcd (denG, denF);
        denG /= gcddenFdenG;
        denF /= gcddenFdenG;
        On (SW_RATIONAL);
        G *= denF;
        F *= denG;
      }
      f=convFactoryPSingP( F, r);
      g=convFactoryPSingP( G, r);
    }
    res=convFactoryPSingP( GCD , r);
    if (!b1) Off (SW_USE_EZGCD_P);
  }
  // and over Q(a) / Fp(a)
  else if ( r->cf->extRing )
  {
    if ( rField_is_Q_a(r)) setCharacteristic( 0 );
    else                   setCharacteristic( rChar(r) );
    if (r->cf->extRing->qideal!=NULL)
    {
      bool b1=isOn(SW_USE_QGCD);
      if ( rField_is_Q_a(r) ) On(SW_USE_QGCD);
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                           r->cf->extRing);
      Variable a=rootOf(mipo);
      F=( convSingAPFactoryAP( f,a,r ) );
      G=( convSingAPFactoryAP( g,a,r ) );
      GCD=gcd(F,G);
      if (!GCD.isOne())
      {
        p_Delete(&f,r);
        p_Delete(&g,r);
        if (getCharacteristic() == 0)
          On (SW_RATIONAL);
        F /= GCD;
        G /= GCD;
        if (getCharacteristic() == 0)
        {
          CanonicalForm denF= bCommonDen (F);
          CanonicalForm denG= bCommonDen (G);
          G *= denG;
          F *= denF;
          Off (SW_RATIONAL);
          CanonicalForm gcddenFdenG= gcd (denG, denF);
          denG /= gcddenFdenG;
          denF /= gcddenFdenG;
          On (SW_RATIONAL);
          G *= denF;
          F *= denG;
        }
        f= convFactoryAPSingAP( F,r );
        g= convFactoryAPSingAP( G,r );
      }
      res= convFactoryAPSingAP( GCD,r );
      prune (a);
      if (!b1) Off(SW_USE_QGCD);
    }
    else
    {
      F=( convSingTrPFactoryP( f,r ) );
      G=( convSingTrPFactoryP( g,r ) );
      GCD=gcd(F,G);
      if (!GCD.isOne())
      {
        p_Delete(&f,r);
        p_Delete(&g,r);
        if (getCharacteristic() == 0)
          On (SW_RATIONAL);
        F /= GCD;
        G /= GCD;
        if (getCharacteristic() == 0)
        {
          CanonicalForm denF= bCommonDen (F);
          CanonicalForm denG= bCommonDen (G);
          G *= denG;
          F *= denF;
          Off (SW_RATIONAL);
          CanonicalForm gcddenFdenG= gcd (denG, denF);
          denG /= gcddenFdenG;
          denF /= gcddenFdenG;
          On (SW_RATIONAL);
          G *= denF;
          F *= denG;
        }
        f= convFactoryPSingTrP( F,r );
        g= convFactoryPSingTrP( G,r );
      }
      res= convFactoryPSingTrP( GCD,r );
    }
  }
  else
    WerrorS( feNotImplemented );
  Off(SW_RATIONAL);
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
  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  if (rField_is_Zp(r) || rField_is_Q(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    Variable X(i);
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r ) );
    res=convFactoryPSingP( resultant( F, G, X),r );
    Off(SW_RATIONAL);
    goto resultant_returns_res;
  }
  // and over Q(a) / Fp(a)
  else if (r->cf->extRing!=NULL)
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else               setCharacteristic( rChar(r) );
    Variable X(i+rPar(r));
    if (r->cf->extRing->qideal!=NULL)
    {
      //Variable X(i);
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                           r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      res= convFactoryAPSingAP( resultant( F, G, X ),r );
      prune (a);
    }
    else
    {
      //Variable X(i+rPar(currRing));
      number nf,ng;
      p_Cleardenom_n(f,r,nf);p_Cleardenom_n(g,r,ng);
      int ef,eg;
      ef=pGetExp_Var(f,i,r);
      eg=pGetExp_Var(g,i,r);
      CanonicalForm F( convSingTrPFactoryP( f,r ) ), G( convSingTrPFactoryP( g,r ) );
      res= convFactoryPSingTrP( resultant( F, G, X ),r );
      if ((nf!=NULL)&&(!n_IsOne(nf,r->cf)))
      {
        number n=n_Invers(nf,r->cf);
        while(eg>0)
        {
          res=__p_Mult_nn(res,n,r);
          eg--;
        }
        n_Delete(&n,r->cf);
      }
      n_Delete(&nf,r->cf);
      if ((ng!=NULL)&&(!n_IsOne(ng,r->cf)))
      {
        number n=n_Invers(ng,r->cf);
        while(ef>0)
        {
          res=__p_Mult_nn(res,n,r);
          ef--;
        }
        n_Delete(&n,r->cf);
      }
      n_Delete(&ng,r->cf);
    }
    Off(SW_RATIONAL);
    goto resultant_returns_res;
  }
  else
    WerrorS( feNotImplemented );
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
  if ( rField_is_Q(r) || rField_is_Zp(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r) );
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
    res=convFactoryPSingP( extgcd( F, G, Fa, Gb ),r );
    pa=convFactoryPSingP(Fa,r);
    pb=convFactoryPSingP(Gb,r);
    Off(SW_RATIONAL);
  }
  // and over Q(a) / Fp(a)
  else if ( r->cf->extRing!=NULL )
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else                 setCharacteristic( rChar(r) );
    CanonicalForm Fa,Gb;
    if (r->cf->extRing->qideal!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                           r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      CanonicalForm FpG=F+G;
      if (!(FpG.isUnivariate()|| FpG.inCoeffDomain()))
      //if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
      {
        WerrorS("not univariate");
        return TRUE;
      }
      res= convFactoryAPSingAP( extgcd( F, G, Fa, Gb ),r );
      pa=convFactoryAPSingAP(Fa,r);
      pb=convFactoryAPSingAP(Gb,r);
      prune (a);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f, r ) ), G( convSingTrPFactoryP( g, r ) );
      CanonicalForm FpG=F+G;
      if (!(FpG.isUnivariate()|| FpG.inCoeffDomain()))
      //if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
      {
        Off(SW_RATIONAL);
        WerrorS("not univariate");
        return TRUE;
      }
      res= convFactoryPSingTrP( extgcd( F, G, Fa, Gb ), r );
      pa=convFactoryPSingTrP(Fa, r);
      pb=convFactoryPSingTrP(Gb, r);
    }
    Off(SW_RATIONAL);
  }
  else
  {
    WerrorS( feNotImplemented );
    return TRUE;
  }
#ifndef SING_NDEBUG
  // checking the result of extgcd:
  poly dummy;
  dummy=p_Sub(p_Add_q(pp_Mult_qq(f,pa,r),pp_Mult_qq(g,pb,r),r),p_Copy(res,r),r);
  if (dummy!=NULL)
  {
    PrintS("extgcd( ");p_Write(f,r);p_Write0(g,r);PrintS(" )\n");
    PrintS("extgcd( ");p_Write(f,r);p_Write0(g,r);PrintS(" )\n");
    p_Delete(&dummy,r);
  }
#endif
  return FALSE;
}

poly singclap_pmult ( poly f, poly g, const ring r )
{
  poly res=NULL;
  On(SW_RATIONAL);
  if (rField_is_Zp(r) || rField_is_Q(r) || rField_is_Z(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    if (rField_is_Z(r)) Off(SW_RATIONAL);
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r ) );
    res = convFactoryPSingP( F * G,r );
  }
  else if (r->cf->extRing!=NULL)
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else               setCharacteristic( rChar(r) );
    if (r->cf->extRing->qideal!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                                 r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      res= convFactoryAPSingAP(  F * G, r  );
      prune (a);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f,r ) ), G( convSingTrPFactoryP( g,r ) );
      res= convFactoryPSingTrP(  F * G,r  );
    }
  }
#if 0 // not yet working
  else if (rField_is_GF())
  {
    //Print("GF(%d^%d)\n",nfCharP,nfMinPoly[0]);
    setCharacteristic( nfCharP,nfMinPoly[0], currRing->parameter[0][0] );
    CanonicalForm F( convSingGFFactoryGF( f ) ), G( convSingGFFactoryGF( g ) );
    res = convFactoryGFSingGF( F * G );
  }
#endif
  else
    WerrorS( feNotImplemented );
  Off(SW_RATIONAL);
  return res;
}

poly singclap_pdivide ( poly f, poly g, const ring r )
{
  poly res=NULL;

  #ifdef HAVE_FLINT
  #if __FLINT_RELEASE >= 20503
  /*
    If the division is not exact, control will pass to factory where the
    polynomials can be divided using the ordering that factory chooses.
  */
  if (rField_is_Zp(r))
  {
    nmod_mpoly_ctx_t ctx;
    if (!convSingRFlintR(ctx,r))
    {
      res = Flint_Divide_MP(f,0,g,0,ctx,r);
      if (res != NULL)
        return res;
    }
  }
  else
  if (rField_is_Q(r))
  {
    fmpq_mpoly_ctx_t ctx;
    if (!convSingRFlintR(ctx,r))
    {
      res = Flint_Divide_MP(f,0,g,0,ctx,r);
      if (res != NULL)
        return res;
    }
  }
  #endif
  #endif

  On(SW_RATIONAL);
  if (rField_is_Zp(r) || rField_is_Q(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r ) );
    res = convFactoryPSingP( F / G,r );
  }
  // div is not implemented for ZZ coeffs in factory
  else if (r->cf->extRing!=NULL)
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else               setCharacteristic( rChar(r) );
    if (r->cf->extRing->qideal!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                                 r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      res= convFactoryAPSingAP(  F / G, r  );
      prune (a);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f,r ) ), G( convSingTrPFactoryP( g,r ) );
      res= convFactoryPSingTrP(  F / G,r  );
    }
  }
#if 0 // not yet working
  else if (rField_is_GF())
  {
    //Print("GF(%d^%d)\n",nfCharP,nfMinPoly[0]);
    setCharacteristic( nfCharP,nfMinPoly[0], currRing->parameter[0][0] );
    CanonicalForm F( convSingGFFactoryGF( f ) ), G( convSingGFFactoryGF( g ) );
    res = convFactoryGFSingGF( F / G );
  }
#endif
  else
    WerrorS( feNotImplemented );
  Off(SW_RATIONAL);
  return res;
}

poly singclap_pmod ( poly f, poly g, const ring r )
{
  poly res=NULL;
  On(SW_RATIONAL);
  if (rField_is_Zp(r) || rField_is_Q(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r ) );
    CanonicalForm Q,R;
    divrem(F,G,Q,R);
    res = convFactoryPSingP(R,r);
    //res = convFactoryPSingP( F-(F/G)*G,r );
  }
  // mod is not implemented for ZZ coeffs in factory
  else if (r->cf->extRing!=NULL)
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else               setCharacteristic( rChar(r) );
    if (r->cf->extRing->qideal!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                                 r->cf->extRing);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      CanonicalForm Q,R;
      divrem(F,G,Q,R);
      res = convFactoryAPSingAP(R,r);
      //res= convFactoryAPSingAP( F-(F/G)*G, r  );
      prune (a);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f,r ) ), G( convSingTrPFactoryP( g,r ) );
      CanonicalForm Q,R;
      divrem(F,G,Q,R);
      res = convFactoryPSingTrP(R,r);
      //res= convFactoryPSingTrP(  F-(F/G)*G,r  );
    }
  }
#if 0 // not yet working
  else if (rField_is_GF())
  {
    //Print("GF(%d^%d)\n",nfCharP,nfMinPoly[0]);
    setCharacteristic( nfCharP,nfMinPoly[0], currRing->parameter[0][0] );
    CanonicalForm F( convSingGFFactoryGF( f ) ), G( convSingGFFactoryGF( g ) );
    res = convFactoryGFSingGF( F / G );
  }
#endif
  else
    WerrorS( feNotImplemented );
  Off(SW_RATIONAL);
  return res;
}

#if 0
// unused
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
    g = convSingPFactoryP( NUM(((fraction)g1)), r->cf->extRing );
    g = gcd( g, convSingPFactoryP( NUM(((fraction)g2)) , r->cf->extRing));

    // second run: gcd's

    p = f;
    while ( (p != NULL) && (g != 1)  && ( g != 0))
    {
      h = convSingPFactoryP( NUM(((fraction)pGetCoeff(p))), r->cf->extRing );
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
        fraction c=(fraction)pGetCoeff(p);
        p_Delete(&(NUM(c)),r->cf->extRing); // 2nd arg used to be nacRing
        NUM(c)=convFactoryPSingP( i.getItem() / g, r->cf->extRing );
        //nTest((number)c);
        //#ifdef LDEBUG
        //number cn=(number)c;
        //StringSetS(""); nWrite(nt); StringAppend(" ==> ");
        //nWrite(cn);PrintS(StringEndS("\n")); // NOTE/TODO: use StringAppendS("\n"); omFree(s);
        //#endif
      }
    }
    // pTest(f);
  }
}
#endif

BOOLEAN count_Factors(ideal I, intvec *v,int j, poly &f, poly fac, const ring r)
{
  p_Test(f,r);
  p_Test(fac,r);
  int e=0;
  if (!p_IsConstant(fac,r))
  {
#ifdef FACTORIZE2_DEBUG
    printf("start count_Factors(%d), Fdeg=%d, factor deg=%d\n",j,p_Totaldegree(f,r),p_Totaldegree(fac,r));
    p_wrp(fac,r);PrintLn();
#endif
    On(SW_RATIONAL);
    CanonicalForm F, FAC,Q,R;
    Variable a;
    if (rField_is_Zp(r) || rField_is_Q(r)
    || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
    {
      F=convSingPFactoryP( f,r );
      FAC=convSingPFactoryP( fac,r );
    }
    else if (r->cf->extRing!=NULL)
    {
      if (r->cf->extRing->qideal!=NULL)
      {
        CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                    r->cf->extRing);
        a=rootOf(mipo);
        F=convSingAPFactoryAP( f,a,r );
        FAC=convSingAPFactoryAP( fac,a,r );
      }
      else
      {
        F=convSingTrPFactoryP( f,r );
        FAC=convSingTrPFactoryP( fac,r );
      }
    }
    else
      WerrorS( feNotImplemented );

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
        if (rField_is_Zp(r) || rField_is_Q(r)
        || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
        {
          q = convFactoryPSingP( Q,r );
        }
        else if (r->cf->extRing!=NULL)
        {
          if (r->cf->extRing->qideal!=NULL)
          {
            q= convFactoryAPSingAP( Q,r );
          }
          else
          {
            q= convFactoryPSingTrP( Q,r );
          }
        }
        e++; p_Delete(&f,r); f=q; q=NULL; F=Q;
      }
      else
      {
        break;
      }
    }
    if (r->cf->extRing!=NULL)
      if (r->cf->extRing->qideal!=NULL)
        prune (a);
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

VAR int singclap_factorize_retry;

ideal singclap_factorize ( poly f, intvec ** v , int with_exps, const ring r)
/* destroys f, sets *v */
{
  p_Test(f,r);
#ifdef FACTORIZE2_DEBUG
  printf("singclap_factorize, degree %ld\n",p_Totaldegree(f,r));
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
      if (res->m[0]==NULL) res->m[0]=p_One(r);
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
  //PrintS("S:");p_Write(f,r);PrintLn();
  // use factory/libfac in general ==============================
  Variable dummy(-1); prune(dummy); // remove all (tmp.) extensions
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFFList L;
  number N=NULL;
  number NN=NULL;
  number old_lead_coeff=n_Copy(pGetCoeff(f), r->cf);

  Variable a;
  if (r->cf->convSingNFactoryN!=ndConvSingNFactoryN)
  {
    if (rField_is_Q(r) || rField_is_Q_a(r) || rField_is_Z(r)) /* Q, Q(a), Z */
    {
      //if (f!=NULL) // already tested at start of routine
      {
        number n0=n_Copy(pGetCoeff(f),r->cf);
        if (with_exps==0)
          N=n_Copy(n0,r->cf);
        if (rField_is_Z(r)) p_Content(f, r);
        else                p_Cleardenom(f, r);
        //after here f should not have a denominator!! and no content
        //PrintS("S:");p_Write(f,r);PrintLn();
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
    if (rField_is_Q(r) || rField_is_Zp(r) || rField_is_Z(r) || rField_is_Zn(r))
    {
      setCharacteristic( rChar(r) );
      if (errorreported) goto notImpl; // char too large
      CanonicalForm F( convSingPFactoryP( f,r ) );
      L = factorize( F );
    }
    // and over Q(a) / Fp(a)
    else if (r->cf->extRing!=NULL)
    {
      if (rField_is_Q_a (r)) setCharacteristic (0);
      else                   setCharacteristic( rChar(r) );
      if (errorreported) goto notImpl; // char too large
      if (r->cf->extRing->qideal!=NULL) /*algebraic extension */
      {
        CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                             r->cf->extRing);
        a=rootOf(mipo);
        CanonicalForm F( convSingAPFactoryAP( f, a, r ) );
        L = factorize( F, a );
        prune(a);
      }
      else /* rational functions */
      {
        CanonicalForm F( convSingTrPFactoryP( f,r ) );
        L = factorize( F );
      }
    }
    else
    {
      goto notImpl;
    }
  }
  else
  {
    goto notImpl;
  }
  if (errorreported)
  {
    errorreported=FALSE;
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
      if (rField_is_Zp(r) || rField_is_Q(r)||  rField_is_Z(r)
      || rField_is_Zn(r))           /* Q, Fp, Z */
      {
        //count_Factors(res,*v,f, j, convFactoryPSingP( J.getItem().factor() );
        res->m[j] = convFactoryPSingP( J.getItem().factor(),r );
      }
#if 0
      else if (rField_is_GF())
        res->m[j] = convFactoryGFSingGF( J.getItem().factor() );
#endif
      else if (r->cf->extRing!=NULL)     /* Q(a), Fp(a) */
      {
#ifndef SING_NDEBUG
        intvec *w=NULL;
        if (v!=NULL) w=*v;
#endif
        if (r->cf->extRing->qideal==NULL)
        {
#ifdef SING_NDEBUG
          res->m[j]= convFactoryPSingTrP( J.getItem().factor(),r );
#else
          if(!count_Factors(res,w,j,ff,convFactoryPSingTrP( J.getItem().factor(),r ),r))
          {
            if (w!=NULL)
              (*w)[j]=1;
            res->m[j]=p_One(r);
          }
#endif
        }
        else
        {
#ifdef SING_NDEBUG
          res->m[j]= convFactoryAPSingAP( J.getItem().factor(),r );
#else
          if (!count_Factors(res,w,j,ff,convFactoryAPSingAP( J.getItem().factor(),r ),r))
          {
            if (w!=NULL)
              (*w)[j]=1;
            res->m[j]=p_One(r);
          }
#endif
        }
      }
    }
    if (r->cf->extRing!=NULL)
      if (r->cf->extRing->qideal!=NULL)
        prune (a);
#ifndef SING_NDEBUG
    if ((r->cf->extRing!=NULL) && (!p_IsConstant(ff,r)))
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
#endif
    p_Delete(&ff,r);
    if (N!=NULL)
    {
      __p_Mult_nn(res->m[0],N,r);
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
  if (rField_is_Q_a(r) && (r->cf->extRing->qideal!=NULL))
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
  prune(a);
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
  if ((v!=NULL) && ((*v)!=NULL) &&(withexps==2))
  {
     *v = new intvec( 1 );
     (*v)[0]=1;
  }
  res=idInit(2,1);
  res->m[0]=p_One(r);
  res->m[1]=f;
  return res;
}

ideal singclap_sqrfree ( poly f, intvec ** v , int with_exps, const ring r)
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
    if (with_exps!=1 && with_exps!=3)
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
    if (with_exps==0 || with_exps==3) n++; // with coeff
    res=idInit(si_max(n,1),1);
    if(with_exps!=1)
    {
        (*v)=new intvec(si_max(1,n));
        (**v)[0]=1;
    }
    res->m[0]=p_NSet(n_Copy(pGetCoeff(f),r->cf),r);
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
  CFFList L;
  number N=NULL;
  number NN=NULL;
  number old_lead_coeff=n_Copy(pGetCoeff(f), r->cf);
  Variable a;

  if (!rField_is_Zp(r) && !rField_is_Zp_a(r)) /* Q, Q(a) */
  {
    //if (f!=NULL) // already tested at start of routine
    number n0=n_Copy(old_lead_coeff,r->cf);
    if (with_exps==0 || with_exps==3)
      N=n_Copy(n0,r->cf);
    p_Cleardenom(f, r);
    //after here f should not have a denominator!!
    //PrintS("S:");p_Write(f,r);PrintLn();
    NN=n_Div(n0,pGetCoeff(f),r->cf);
    n_Delete(&n0,r->cf);
    if (with_exps==0 || with_exps==3)
    {
      n_Delete(&N,r->cf);
      N=n_Copy(NN,r->cf);
    }
  }
  else if (rField_is_Zp_a(r))
  {
    //if (f!=NULL) // already tested at start of routine
    if (singclap_factorize_retry==0)
    {
      number n0=n_Copy(old_lead_coeff,r->cf);
      if (with_exps==0 || with_exps==3)
        N=n_Copy(n0,r->cf);
      p_Norm(f,r);
      p_Cleardenom(f, r);
      NN=n_Div(n0,pGetCoeff(f),r->cf);
      n_Delete(&n0,r->cf);
      if (with_exps==0 || with_exps==3)
      {
        n_Delete(&N,r->cf);
        N=n_Copy(NN,r->cf);
      }
    }
  }
  if (rField_is_Q(r) || rField_is_Zp(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
  {
    setCharacteristic( rChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) );
    L = sqrFree( F );
  }
  else if (r->cf->extRing!=NULL)
  {
    if (rField_is_Q_a (r)) setCharacteristic (0);
    else                   setCharacteristic( rChar(r) );
    if (r->cf->extRing->qideal!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(r->cf->extRing->qideal->m[0],
                                           r->cf->extRing);
      a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f, a, r ) );
      L= sqrFree (F);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f,r ) );
      L = sqrFree( F );
    }
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
    if (with_exps!=1)
    {
      if ((with_exps==2)&&(n>1))
      {
        n--;
        J++;
      }
      *v = new intvec( n );
    }
    else if (L.getFirst().factor().inCoeffDomain() && with_exps!=3)
    {
      n--;
      J++;
    }
    res = idInit( n ,1);
    for ( ; J.hasItem(); J++, j++ )
    {
      if (with_exps!=1 && with_exps!=3) (**v)[j] = J.getItem().exp();
      if (rField_is_Zp(r) || rField_is_Q(r)
      || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
        res->m[j] = convFactoryPSingP( J.getItem().factor(),r );
      else if (r->cf->extRing!=NULL)     /* Q(a), Fp(a) */
      {
        if (r->cf->extRing->qideal==NULL)
          res->m[j]=convFactoryPSingTrP( J.getItem().factor(),r );
        else
          res->m[j]=convFactoryAPSingAP( J.getItem().factor(),r );
      }
    }
    if (res->m[0]==NULL)
    {
      res->m[0]=p_One(r);
    }
    if (N!=NULL)
    {
      __p_Mult_nn(res->m[0],N,r);
      n_Delete(&N,r->cf);
      N=NULL;
    }
  }
  if (r->cf->extRing!=NULL)
    if (r->cf->extRing->qideal!=NULL)
      prune (a);
  if (rField_is_Q_a(r) && (r->cf->extRing->qideal!=NULL))
  {
    int i=IDELEMS(res)-1;
    int stop=1;
    if (with_exps!=0 || with_exps==3) stop=0;
    for(;i>=stop;i--)
    {
      p_Norm(res->m[i],r);
    }
    if (with_exps==0 || with_exps==3) p_SetCoeff(res->m[0],old_lead_coeff,r);
    else n_Delete(&old_lead_coeff,r->cf);
  }
  else
    n_Delete(&old_lead_coeff,r->cf);
  p_Delete(&f,r);
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
  return res;
}

matrix singclap_irrCharSeries ( ideal I, const ring r)
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
  if (rField_is_Q(r) || rField_is_Zp(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
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
        p_Delete(&p,r);
      }
    }
  }
  // and over Q(a) / Fp(a)
  else if (nCoeff_is_transExt (r->cf))
  {
    setCharacteristic( rChar(r) );
    for(i=0;i<IDELEMS(I);i++)
    {
      poly p=I->m[i];
      if (p!=NULL)
      {
        p=p_Copy(p,r);
        p_Cleardenom(p, r);
        L.append(convSingTrPFactoryP(p,r));
        p_Delete(&p,r);
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
    LL=irrCharSeries(L);
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
    iiWriteMatrix((matrix)I,"I",2,r,0);
    m=si_max(m,1);
    n=si_max(n,1);
  }
  res=mpNew(m,n);
  CFListIterator Li;
  for ( m=1, LLi = LL; LLi.hasItem(); LLi++, m++ )
  {
    for (n=1, Li = LLi.getItem(); Li.hasItem(); Li++, n++)
    {
      if (rField_is_Q(r) || rField_is_Zp(r)
      || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
        MATELEM(res,m,n)=convFactoryPSingP(Li.getItem(),r);
      else
        MATELEM(res,m,n)=convFactoryPSingTrP(Li.getItem(),r);
    }
  }
  Off(SW_RATIONAL);
  return res;
}

char* singclap_neworder ( ideal I, const ring r)
{
  int i;
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFList L;
  if (rField_is_Q(r) || rField_is_Zp(r)
  || (rField_is_Zn(r)&&(r->cf->convSingNFactoryN!=ndConvSingNFactoryN)))
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
  else if (nCoeff_is_transExt (r->cf))
  {
    setCharacteristic( rChar(r) );
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
  char * s=StringEndS();
  if (s[strlen(s)-1]==',') s[strlen(s)-1]='\0';
  return s;
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

int singclap_det_i( intvec * m, const ring /*r*/)
{
//  assume( r == currRing ); // Anything else is not guaranted to work!

  setCharacteristic( 0 ); // ?
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
  return res;
}

number singclap_det_bi( bigintmat * m, const coeffs cf)
{
  assume(m->basecoeffs()==cf);
  CFMatrix M(m->rows(),m->cols());
  int i,j;
  BOOLEAN setchar=TRUE;
  for(i=m->rows();i>0;i--)
  {
    for(j=m->cols();j>0;j--)
    {
      M(i,j)=n_convSingNFactoryN(BIMATELEM(*m,i,j),setchar,cf);
      setchar=FALSE;
    }
  }
  number res=n_convFactoryNSingN( determinant(M,m->rows()),cf ) ;
  return res;
}

#if defined(HAVE_NTL) || defined(AHVE_FLINT)
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

bigintmat* singntl_HNF(bigintmat*  b)
{
  int r=b->rows();
  if (r!=b->cols())
  {
    Werror("HNF of %d x %d matrix",r,b->cols());
    return NULL;
  }
  setCharacteristic( 0 );
  CFMatrix M(r,r);
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=r;j>0;j--)
    {
      M(i,j)=n_convSingNFactoryN(BIMATELEM(*b,i,j),FALSE,b->basecoeffs());
    }
  }
  CFMatrix *MM=cf_HNF(M);
  bigintmat *mm=bimCopy(b);
  for(i=r;i>0;i--)
  {
    for(j=r;j>0;j--)
    {
      BIMATELEM(*mm,i,j)=n_convFactoryNSingN((*MM)(i,j),b->basecoeffs());
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
    for(j=c;j>0;j--)
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
#else
matrix singntl_HNF(matrix  m, const ring s )
{
  WerrorS("NTL/FLINT missing");
  return NULL;
}

intvec* singntl_HNF(intvec*  m)
{
  WerrorS("NTL/FLINT missing");
  return NULL;
}

matrix singntl_LLL(matrix  m, const ring s )
{
  WerrorS("NTL/FLINT missing");
  return NULL;
}

intvec* singntl_LLL(intvec*  m)
{
  WerrorS("NTL/FLINT missing");
  return NULL;
}
#endif

#ifdef HAVE_NTL
ideal singclap_absFactorize ( poly f, ideal & mipos, intvec ** exps, int & numFactors, const ring r)
{
  p_Test(f, r);

  ideal res=NULL;

  int offs = rPar(r);
  if (f==NULL)
  {
    res= idInit (1, 1);
    mipos= idInit (1, 1);
    mipos->m[0]= convFactoryPSingTrP (Variable (offs), r); //overkill
    (*exps)=new intvec (1);
    (**exps)[0]= 1;
    numFactors= 0;
    return res;
  }
  CanonicalForm F( convSingTrPFactoryP( f, r) );

  bool isRat= isOn (SW_RATIONAL);
  if (!isRat)
    On (SW_RATIONAL);

  CFAFList absFactors= absFactorize (F);

  int n= absFactors.length();
  *exps=new intvec (n);

  res= idInit (n, 1);

  mipos= idInit (n, 1);

  Variable x= Variable (offs);
  Variable alpha;
  int i= 0;
  numFactors= 0;
  int count;
  CFAFListIterator iter= absFactors;
  CanonicalForm lead= iter.getItem().factor();
  if (iter.getItem().factor().inCoeffDomain())
  {
    i++;
    iter++;
  }
  for (; iter.hasItem(); iter++, i++)
  {
    (**exps)[i]= iter.getItem().exp();
    alpha= iter.getItem().minpoly().mvar();
    if (iter.getItem().minpoly().isOne())
      lead /= power (bCommonDen (iter.getItem().factor()), iter.getItem().exp());
    else
      lead /= power (power (bCommonDen (iter.getItem().factor()), degree (iter.getItem().minpoly())), iter.getItem().exp());
    res->m[i]= convFactoryPSingTrP (replacevar (iter.getItem().factor()*bCommonDen (iter.getItem().factor()), alpha, x), r);
    if (iter.getItem().minpoly().isOne())
    {
      count= iter.getItem().exp();
      mipos->m[i]= convFactoryPSingTrP (x,r);
    }
    else
    {
      count= iter.getItem().exp()*degree (iter.getItem().minpoly());
      mipos->m[i]= convFactoryPSingTrP (replacevar (iter.getItem().minpoly(), alpha, x), r);
    }
    if (!iter.getItem().minpoly().isOne())
      prune (alpha);
    numFactors += count;
  }
  if (!isRat)
    Off (SW_RATIONAL);

  (**exps)[0]= 1;
  res->m[0]= convFactoryPSingTrP (lead, r);
  mipos->m[0]= convFactoryPSingTrP (x, r);
  return res;
}

#else
ideal singclap_absFactorize ( poly f, ideal & mipos, intvec ** exps, int & numFactors, const ring r)
{
  WerrorS("NTL missing");
  return NULL;
}

#endif /* HAVE_NTL */

int * Zp_roots(poly p, const ring r)
{
  CanonicalForm pp=convSingPFactoryP(p,r);
  return Zp_roots(pp);
}
