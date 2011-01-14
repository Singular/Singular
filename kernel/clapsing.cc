// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id$
/*
* ABSTRACT: interface between Singular and factory
*/

//#define FACTORIZE2_DEBUG
#include <kernel/mod2.h>
#include <omalloc/omalloc.h>
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <kernel/structs.h>
#include <kernel/clapsing.h>
#include <kernel/numbers.h>
#include <kernel/ring.h>
#include <kernel/ideals.h>
#include <kernel/ffields.h>
#include <factory/factory.h>
#include <kernel/clapconv.h>
#include <libfac/factor.h>
#include <kernel/ring.h>

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

poly singclap_gcd_r ( poly f, poly g, const ring r )
{
  // assume p_Cleardenom is done
  // assume f!=0, g!=0
  poly res=NULL;

  assume(f!=NULL);
  assume(g!=NULL);

  if((pNext(f)==NULL) && (pNext(g)==NULL))
  {
    poly p=pOne();
    for(int i=rVar(r);i>0;i--)
      p_SetExp(p,i,si_min(p_GetExp(f,i,r),p_GetExp(g,i,r)),r);
    p_Setm(p,r);
    return p;
  }

  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  Off(SW_RATIONAL);
  if (rField_is_Q(r) || (rField_is_Zp(r)))
  {
    setCharacteristic( n_GetChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g, r ) );
    res=convFactoryPSingP( gcd( F, G ) , r);
  }
  // and over Q(a) / Fp(a)
  else if ( rField_is_Extension(r))
  {
    if ( rField_is_Q_a(r)) setCharacteristic( 0 );
    else                   setCharacteristic( -n_GetChar(r) );
    if (r->minpoly!=NULL)
    {
      bool b1=isOn(SW_USE_QGCD);
      bool b2=isOn(SW_USE_fieldGCD);
      if ( rField_is_Q_a() ) On(SW_USE_QGCD);
      else                   On(SW_USE_fieldGCD);
      CanonicalForm mipo=convSingPFactoryP(((lnumber)r->minpoly)->z,
                                           r->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      res= convFactoryAPSingAP( gcd( F, G ),currRing );
      if (!b1) Off(SW_USE_QGCD);
      if (!b2) Off(SW_USE_fieldGCD);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f,r ) ), G( convSingTrPFactoryP( g,r ) );
      res= convFactoryPSingTrP( gcd( F, G ),r );
    }
  }
  #if 0
  else if (( n_GetChar(r)>1 )&&(r->parameter!=NULL)) /* GF(q) */
  {
    int p=rChar(r);
    int n=2;
    int t=p*p;
    while (t!=n_Char(r)) { t*=p;n++; }
    setCharacteristic(p,n,'a');
    CanonicalForm F( convSingGFFactoryGF( f,r ) ), G( convSingGFFactoryGF( g,r ) );
    res= convFactoryGFSingGF( gcd( F, G ),r );
  }
  #endif
  else
    WerrorS( feNotImplemented );

  Off(SW_RATIONAL);
  return res;
}

poly singclap_gcd ( poly f, poly g )
{
  poly res=NULL;

  if (f!=NULL) p_Cleardenom(f, currRing);
  if (g!=NULL) p_Cleardenom(g, currRing);
  else         return f; // g==0 => gcd=f (but do a p_Cleardenom)
  if (f==NULL) return g; // f==0 => gcd=g (but do a p_Cleardenom)

  res=singclap_gcd_r(f,g,currRing);
  pDelete(&f);
  pDelete(&g);
  return res;
}

/*2 find the maximal exponent of var(i) in poly p*/
int pGetExp_Var(poly p, int i)
{
  int m=0;
  int mm;
  while (p!=NULL)
  {
    mm=pGetExp(p,i);
    if (mm>m) m=mm;
    pIter(p);
  }
  return m;
}

// destroys f,g,x
poly singclap_resultant ( poly f, poly g , poly x)
{
  poly res=NULL;
  int i=pIsPurePower(x);
  if (i==0)
  {
    WerrorS("3rd argument must be a ring variable");
    goto resultant_returns_res;
  }
  if ((f==NULL) || (g==NULL))
    goto resultant_returns_res;
  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  if (rField_is_Zp() || rField_is_Q())
  {
    Variable X(i);
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPFactoryP( f ) ), G( convSingPFactoryP( g ) );
    res=convFactoryPSingP( resultant( F, G, X ) );
    Off(SW_RATIONAL);
    goto resultant_returns_res;
  }
  // and over Q(a) / Fp(a)
  else if (rField_is_Extension())
  {
    if (rField_is_Q_a()) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    Variable X(i+rPar(currRing));
    if (currRing->minpoly!=NULL)
    {
      //Variable X(i);
      CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                                 currRing->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,currRing ) ),
                    G( convSingAPFactoryAP( g,a,currRing ) );
      res= convFactoryAPSingAP( resultant( F, G, X ),currRing );
    }
    else
    {
      //Variable X(i+rPar(currRing));
      number nf,ng;
      p_Cleardenom_n(f, currRing,nf);p_Cleardenom_n(g, currRing,ng);
      int ef,eg;
      ef=pGetExp_Var(f,i);
      eg=pGetExp_Var(g,i);
      CanonicalForm F( convSingTrPFactoryP( f ) ), G( convSingTrPFactoryP( g ) );
      res= convFactoryPSingTrP( resultant( F, G, X ) );
      if ((nf!=NULL)&&(!nIsOne(nf))&&(!nIsZero(nf)))
      {
        number n=nInvers(nf);
        while(eg>0)
        {
          res=pMult_nn(res,n);
          eg--;
        }
        nDelete(&n);
      }
      nDelete(&nf);
      if ((ng!=NULL)&&(!nIsOne(ng))&&(!nIsZero(ng)))
      {
        number n=nInvers(ng);
        while(ef>0)
        {
          res=pMult_nn(res,n);
          ef--;
        }
        nDelete(&n);
      }
      nDelete(&ng);
    }
    Off(SW_RATIONAL);
    goto resultant_returns_res;
  }
  else
    WerrorS( feNotImplemented );
resultant_returns_res:
  pDelete(&f);
  pDelete(&g);
  pDelete(&x);
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

BOOLEAN singclap_extgcd ( poly f, poly g, poly &res, poly &pa, poly &pb )
{
  // for now there is only the possibility to handle univariate
  // polynomials over
  // Q and Fp ...
  res=NULL;pa=NULL;pb=NULL;
  On(SW_SYMMETRIC_FF);
  if (rField_is_Zp() || rField_is_Q())
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPFactoryP( f ) ), G( convSingPFactoryP( g ) );
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
    res=convFactoryPSingP( extgcd( F, G, Fa, Gb ) );
    pa=convFactoryPSingP(Fa);
    pb=convFactoryPSingP(Gb);
    Off(SW_RATIONAL);
  }
  // and over Q(a) / Fp(a)
  else if (rField_is_Extension())
  {
    if (rField_is_Q_a()) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    CanonicalForm Fa,Gb;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                                 currRing->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,currRing ) ),
                    G( convSingAPFactoryAP( g,a,currRing ) );
      CanonicalForm FpG=F+G;
      if (!(FpG.isUnivariate()|| FpG.inCoeffDomain()))
      //if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
      {
        WerrorS("not univariate");
        return TRUE;
      }
      res= convFactoryAPSingAP( extgcd( F, G, Fa, Gb ),currRing );
      pa=convFactoryAPSingAP(Fa,currRing);
      pb=convFactoryAPSingAP(Gb,currRing);
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f ) ), G( convSingTrPFactoryP( g ) );
      CanonicalForm FpG=F+G;
      if (!(FpG.isUnivariate()|| FpG.inCoeffDomain()))
      //if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
      {
        Off(SW_RATIONAL);
        WerrorS("not univariate");
        return TRUE;
      }
      res= convFactoryPSingTrP( extgcd( F, G, Fa, Gb ) );
      pa=convFactoryPSingTrP(Fa);
      pb=convFactoryPSingTrP(Gb);
    }
    Off(SW_RATIONAL);
  }
  else
  {
    WerrorS( feNotImplemented );
    return TRUE;
  }
#ifndef NDEBUG
  // checking the result of extgcd:
  poly dummy;
  dummy=pSub(pAdd(pMult(pCopy(f),pCopy(pa)),pMult(pCopy(g),pCopy(pb))),pCopy(res));
  if (dummy!=NULL)
  {
    PrintS("extgcd( ");pWrite(f);pWrite0(g);PrintS(" )\n");
    PrintS("gcd, co-factors:");pWrite(res); pWrite(pa);pWrite(pb);
    pDelete(&dummy);
  }
#endif
  return FALSE;
}

BOOLEAN singclap_extgcd_r ( poly f, poly g, poly &res, poly &pa, poly &pb, const ring r )
{
  // for now there is only the possibility to handle univariate
  // polynomials over
  // Q and Fp ...
  res=NULL;pa=NULL;pb=NULL;
  On(SW_SYMMETRIC_FF);
  if ( rField_is_Q(r) || rField_is_Zp(r) )
  {
    setCharacteristic( n_GetChar(r) );
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
  else if ( rField_is_Extension(r))
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else                 setCharacteristic( -n_GetChar(r) );
    CanonicalForm Fa,Gb;
    if (r->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)r->minpoly)->z,
                                            r->algring);
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
      res= convFactoryAPSingAP( extgcd( F, G, Fa, Gb ),currRing );
      pa=convFactoryAPSingAP(Fa,currRing);
      pb=convFactoryAPSingAP(Gb,currRing);
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
  return FALSE;
}

poly singclap_pdivide ( poly f, poly g )
{
  poly res=NULL;
  On(SW_RATIONAL);
  if (rField_is_Zp() || rField_is_Q())
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPFactoryP( f ) ), G( convSingPFactoryP( g ) );
    res = convFactoryPSingP( F / G );
  }
  else if (rField_is_Extension())
  {
    if (rField_is_Q_a()) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                                 currRing->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,currRing ) ),
                    G( convSingAPFactoryAP( g,a,currRing ) );
      res= convFactoryAPSingAP(  F / G,currRing  );
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f ) ), G( convSingTrPFactoryP( g ) );
      res= convFactoryPSingTrP(  F / G  );
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

poly singclap_pdivide_r ( poly f, poly g, const ring r )
{
  poly res=NULL;
  On(SW_RATIONAL);
  if (rField_is_Zp(r) || rField_is_Q(r))
  {
    setCharacteristic( n_GetChar(r) );
    CanonicalForm F( convSingPFactoryP( f,r ) ), G( convSingPFactoryP( g,r ) );
    res = convFactoryPSingP( F / G,r );
  }
  else if (rField_is_Extension(r))
  {
    if (rField_is_Q_a(r)) setCharacteristic( 0 );
    else               setCharacteristic( -n_GetChar(r) );
    if (r->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)r->minpoly)->z,
                                                 r->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,r ) ),
                    G( convSingAPFactoryAP( g,a,r ) );
      res= convFactoryAPSingAP(  F / G, r  );
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

void singclap_divide_content ( poly f )
{
  if ( f==NULL )
  {
    return;
  }
  else  if ( pNext( f ) == NULL )
  {
    pSetCoeff( f, nInit( 1 ) );
    return;
  }
  else
  {
    if ( rField_is_Q_a() )
      setCharacteristic( 0 );
    else  if ( rField_is_Zp_a() )
      setCharacteristic( -nGetChar() );
    else
      return; /* not implemented*/

    CFList L;
    CanonicalForm g, h;
    poly p = pNext(f);

    // first attemp: find 2 smallest g:

    number g1=pGetCoeff(f);
    number g2=pGetCoeff(p); // p==pNext(f);
    pIter(p);
    int sz1=nSize(g1);
    int sz2=nSize(g2);
    if (sz1>sz2)
    {
      number gg=g1;
      g1=g2; g2=gg;
      int sz=sz1;
      sz1=sz2; sz2=sz;
    }
    while (p!=NULL)
    {
      int n_sz=nSize(pGetCoeff(p));
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
    g = convSingPFactoryP( ((lnumber)g1)->z, currRing->algring );
    g = gcd( g, convSingPFactoryP( ((lnumber)g2)->z , currRing->algring));

    // second run: gcd's

    p = f;
    while ( (p != NULL) && (g != 1)  && ( g != 0))
    {
      h = convSingPFactoryP( ((lnumber)pGetCoeff(p))->z, currRing->algring );
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
        p_Delete(&c->z,nacRing);
        c->z=convFactoryPSingP( i.getItem() / g, currRing->algring );
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

static int primepower(int c)
{
  int p=1;
  int cc=c;
  while(cc!= rInternalChar(currRing)) { cc*=c; p++; }
  return p;
}

BOOLEAN count_Factors(ideal I, intvec *v,int j, poly &f, poly fac)
{
  pTest(f);
  pTest(fac);
  int e=0;
  if (!pIsConstantPoly(fac))
  {
#ifdef FACTORIZE2_DEBUG
    printf("start count_Factors(%d), Fdeg=%d, factor deg=%d\n",j,pTotaldegree(f),pTotaldegree(fac));
    p_wrp(fac,currRing);PrintLn();
#endif
    On(SW_RATIONAL);
    CanonicalForm F, FAC,Q,R;
    Variable a;
    if (rField_is_Zp() || rField_is_Q())
    {
      F=convSingPFactoryP( f );
      FAC=convSingPFactoryP( fac );
    }
    else if (rField_is_Extension())
    {
      if (currRing->minpoly!=NULL)
      {
        CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                    currRing->algring);
        a=rootOf(mipo);
        F=convSingAPFactoryAP( f,a,currRing );
        FAC=convSingAPFactoryAP( fac,a,currRing );
      }
      else
      {
        F=convSingTrPFactoryP( f );
        FAC=convSingTrPFactoryP( fac );
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
        if (rField_is_Zp() || rField_is_Q())
        {
          q = convFactoryPSingP( Q );
        }
        else if (rField_is_Extension())
        {
          if (currRing->minpoly!=NULL)
          {
            q= convFactoryAPSingAP(  Q,currRing  );
          }
          else
          {
            q= convFactoryPSingTrP(  Q  );
          }
        }
        e++; pDelete(&f); f=q; q=NULL; F=Q;
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

int singclap_factorize_retry;
extern int libfac_interruptflag;

ideal singclap_factorize ( poly f, intvec ** v , int with_exps)
/* destroys f, sets *v */
{
  pTest(f);
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
    for(i=pVariables;i>0;i--) if(pGetExp(f,i)!=0) n++;
    if (with_exps==0) n++; // with coeff
    res=idInit(si_max(n,1),1);
    switch(with_exps)
    {
      case 0: // with coef & exp.
        res->m[0]=pNSet(nCopy(pGetCoeff(f)));
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
      res->m[0]=pOne();
      // (**v)[0]=1; is already done
    }
    else
    {
      for(i=pVariables;i>0;i--)
      {
        e=pGetExp(f,i);
        if(e!=0)
        {
          n--;
          poly p=pOne();
          pSetExp(p,i,1);
          pSetm(p);
          res->m[n]=p;
          if (with_exps!=1) (**v)[n]=e;
        }
      }
    }
    pDelete(&f);
    return res;
  }
  //PrintS("S:");pWrite(f);PrintLn();
  // use factory/libfac in general ==============================
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  #ifdef HAVE_NTL
  extern int prime_number;
  if(rField_is_Q()) prime_number=0;
  #endif
  CFFList L;
  number N=NULL;
  number NN=NULL;
  number old_lead_coeff=nCopy(pGetCoeff(f));

  if (!rField_is_Zp() && !rField_is_Zp_a()) /* Q, Q(a) */
  {
    //if (f!=NULL) // already tested at start of routine
    {
      number n0=nCopy(pGetCoeff(f));
      if (with_exps==0)
        N=nCopy(n0);
      p_Cleardenom(f, currRing);
      NN=nDiv(n0,pGetCoeff(f));
      nDelete(&n0);
      if (with_exps==0)
      {
        nDelete(&N);
        N=nCopy(NN);
      }
    }
  }
  else if (rField_is_Zp_a())
  {
    //if (f!=NULL) // already tested at start of routine
    if (singclap_factorize_retry==0)
    {
      number n0=nCopy(pGetCoeff(f));
      if (with_exps==0)
        N=nCopy(n0);
      pNorm(f);
      p_Cleardenom(f, currRing);
      NN=nDiv(n0,pGetCoeff(f));
      nDelete(&n0);
      if (with_exps==0)
      {
        nDelete(&N);
        N=nCopy(NN);
      }
    }
  }
  if (rField_is_Q() || rField_is_Zp())
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPFactoryP( f ) );
    L = factorize( F );
  }
  #if 0
  else if (rField_is_GF())
  {
    int c=rChar(currRing);
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
  // and over Q(a) / Fp(a)
  else if (rField_is_Extension())
  {
    if (rField_is_Q_a()) setCharacteristic( 0 );
    else                 setCharacteristic( -nGetChar() );
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                   currRing->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,currRing ) );
      if (rField_is_Zp_a())
      {
        L = factorize( F, a );
      }
      else
      {
        //  over Q(a)
        if (F.isUnivariate())
        {
          L= factorize (F, a);
        }
        else
        {
          CanonicalForm G( convSingTrPFactoryP( f ) );
          do
          {
            libfac_interruptflag=0;
            L=Factorize2(G, mipo);
          }
          while ((libfac_interruptflag!=0) ||(L.isEmpty()));
          #ifdef FACTORIZE2_DEBUG
          printf("while okay\n");
          #endif
          libfac_interruptflag=0;
        }
      }
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f ) );
      L = factorize( F );
    }
  }
  else
  {
    goto notImpl;
  }
  {
    poly ff=pCopy(f); // a copy for the retry stuff
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
      poly p;
      if (with_exps!=1) (**v)[j] = J.getItem().exp();
      if (rField_is_Zp() || rField_is_Q())           /* Q, Fp */
      {
        //count_Factors(res,*v,f, j, convFactoryPSingP( J.getItem().factor() );
        res->m[j] = convFactoryPSingP( J.getItem().factor() );
      }
      #if 0
      else if (rField_is_GF())
        res->m[j] = convFactoryGFSingGF( J.getItem().factor() );
      #endif
      else if (rField_is_Extension())     /* Q(a), Fp(a) */
      {
        intvec *w=NULL;
        if (v!=NULL) w=*v;
        if (currRing->minpoly==NULL)
        {
          if(!count_Factors(res,w,j,ff,convFactoryPSingTrP( J.getItem().factor() )))
          {
            if (w!=NULL)
              (*w)[j]=1;
            res->m[j]=pOne();
          }
        }
        else
        {
          if (!count_Factors(res,w,j,ff,convFactoryAPSingAP( J.getItem().factor(),currRing )))
          {
            if (w!=NULL)
              (*w)[j]=1;
            res->m[j]=pOne();
          }
        }
      }
    }
    if (rField_is_Extension() && (!pIsConstantPoly(ff)))
    {
      singclap_factorize_retry++;
      if (singclap_factorize_retry<3)
      {
        int jj;
        #ifdef FACTORIZE2_DEBUG
        printf("factorize_retry\n");
        #endif
        intvec *ww=NULL;
        idTest(res);
        ideal h=singclap_factorize ( ff, &ww , with_exps);
        idTest(h);
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
        idDelete(&res);
        idDelete(&h);
        res=hh;
        idTest(res);
        ff=NULL;
      }
      else
      {
        WarnS("problem with factorize");
        #if 0
        pWrite(ff);
        idShow(res);
        #endif
        idDelete(&res);
        res=idInit(2,1);
        res->m[0]=pOne();
        res->m[1]=ff; ff=NULL;
      }
    }
    pDelete(&ff);
    if (N!=NULL)
    {
      pMult_nn(res->m[0],N);
      nDelete(&N);
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
        && (pIsConstant(res->m[i])))
        {
          if (with_exps!=0)
          {
            pDelete(&(res->m[i]));
            if ((v!=NULL) && ((*v)!=NULL))
              (**v)[i]=0;
            j++;
          }
          else if (i!=0)
          {
            while ((v!=NULL) && ((*v)!=NULL) && ((**v)[i]>1))
            {
              res->m[0]=pMult(res->m[0],pCopy(res->m[i]));
              (**v)[i]--;
            }
            res->m[0]=pMult(res->m[0],res->m[i]);
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
        res->m[0]=pOne();
      }
    }
  }
  if (rField_is_Q_a() && (currRing->minpoly!=NULL))
  {
    int i=IDELEMS(res)-1;
    int stop=1;
    if (with_exps!=0) stop=0;
    for(;i>=stop;i--)
    {
      pNorm(res->m[i]);
    }
    if (with_exps==0) pSetCoeff(res->m[0],old_lead_coeff);
    else nDelete(&old_lead_coeff);
  }
  else
    nDelete(&old_lead_coeff);
  errorreported=save_errorreported;
notImpl:
  if (res==NULL)
    WerrorS( feNotImplemented );
  if (NN!=NULL)
  {
    nDelete(&NN);
  }
  if (N!=NULL)
  {
    nDelete(&N);
  }
  if (f!=NULL) pDelete(&f);
  //PrintS("......S\n");
  return res;
}
ideal singclap_sqrfree ( poly f)
{
  pTest(f);
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
    for(i=pVariables;i>0;i--) if(pGetExp(f,i)!=0) n++;
    n++; // with coeff
    res=idInit(si_max(n,1),1);
    res->m[0]=pNSet(nCopy(pGetCoeff(f)));
    if (n==0)
    {
      res->m[0]=pOne();
      // (**v)[0]=1; is already done
      return res;
    }
    for(i=pVariables;i>0;i--)
    {
      e=pGetExp(f,i);
      if(e!=0)
      {
        n--;
        poly p=pOne();
        pSetExp(p,i,1);
        pSetm(p);
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
  if(rField_is_Q()) prime_number=0;
  #endif
  CFFList L;

  if (!rField_is_Zp() && !rField_is_Zp_a()) /* Q, Q(a) */
  {
    //if (f!=NULL) // already tested at start of routine
    {
      p_Cleardenom(f, currRing);
    }
  }
  else if (rField_is_Zp_a())
  {
    //if (f!=NULL) // already tested at start of routine
    if (singclap_factorize_retry==0)
    {
      pNorm(f);
      p_Cleardenom(f, currRing);
    }
  }
  if (rField_is_Q() || rField_is_Zp())
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPFactoryP( f ) );
    L = sqrFree( F );
  }
  #if 0
  else if (rField_is_GF())
  {
    int c=rChar(currRing);
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
  // and over Q(a) / Fp(a)
  else if (rField_is_Extension())
  {
    if (rField_is_Q_a()) setCharacteristic( 0 );
    else                 setCharacteristic( -nGetChar() );
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                    currRing->algring);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPFactoryAP( f,a,currRing ) );
      CFFList SqrFreeMV( const CanonicalForm & f , const CanonicalForm & mipo=0) ;

      L = SqrFreeMV( F,mipo );
      //WarnS("L = sqrFree( F,mipo );");
      //L = sqrFree( F );
    }
    else
    {
      CanonicalForm F( convSingTrPFactoryP( f ) );
      L = sqrFree( F );
    }
  }
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
      poly p;
      if (rField_is_Zp() || rField_is_Q())           /* Q, Fp */
        //count_Factors(res,*v,f, j, convFactoryPSingP( J.getItem().factor() );
        res->m[j] = convFactoryPSingP( J.getItem().factor() );
      #if 0
      else if (rField_is_GF())
        res->m[j] = convFactoryGFSingGF( J.getItem().factor() );
      #endif
      else if (rField_is_Extension())     /* Q(a), Fp(a) */
      {
        if (currRing->minpoly==NULL)
          res->m[j]=convFactoryPSingTrP( J.getItem().factor() );
        else
          res->m[j]=convFactoryAPSingAP( J.getItem().factor(),currRing );
      }
    }
    if (res->m[0]==NULL)
    {
      res->m[0]=pOne();
    }
  }
  pDelete(&f);
  errorreported=save_errorreported;
notImpl:
  if (res==NULL)
    WerrorS( feNotImplemented );
  return res;
}
matrix singclap_irrCharSeries ( ideal I)
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
  if (((nGetChar() == 0) || (nGetChar() > 1) )
  && (currRing->parameter==NULL))
  {
    setCharacteristic( nGetChar() );
    for(i=0;i<IDELEMS(I);i++)
    {
      poly p=I->m[i];
      if (p!=NULL)
      {
        p=pCopy(p);
        p_Cleardenom(p, currRing);
        L.append(convSingPFactoryP(p));
      }
    }
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    for(i=0;i<IDELEMS(I);i++)
    {
      poly p=I->m[i];
      if (p!=NULL)
      {
        p=pCopy(p);
        p_Cleardenom(p, currRing);
        L.append(convSingTrPFactoryP(p));
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
      if ( (nGetChar() == 0) || (nGetChar() > 1) )
        MATELEM(res,m,n)=convFactoryPSingP(Li.getItem());
      else
        MATELEM(res,m,n)=convFactoryPSingTrP(Li.getItem());
    }
  }
  Off(SW_RATIONAL);
  return res;
}

char* singclap_neworder ( ideal I)
{
  int i;
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFList L;
  if (((nGetChar() == 0) || (nGetChar() > 1) )
  && (currRing->parameter==NULL))
  {
    setCharacteristic( nGetChar() );
    for(i=0;i<IDELEMS(I);i++)
    {
      L.append(convSingPFactoryP(I->m[i]));
    }
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    for(i=0;i<IDELEMS(I);i++)
    {
      L.append(convSingTrPFactoryP(I->m[i]));
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
  int offs=rPar(currRing);
  int* mark=(int*)omAlloc0((pVariables+offs)*sizeof(int));
  int cnt=pVariables+offs;
  loop
  {
    if(! Li.hasItem()) break;
    BOOLEAN done=TRUE;
    i=Li.getItem()-1;
    mark[i]=1;
    if (i<offs)
    {
      done=FALSE;
      //StringAppendS(currRing->parameter[i]);
    }
    else
    {
      StringAppendS(currRing->names[i-offs]);
    }
    Li++;
    cnt--;
    if(cnt==0) break;
    if (done) StringAppendS(",");
  }
  for(i=0;i<pVariables+offs;i++)
  {
    BOOLEAN done=TRUE;
    if(mark[i]==0)
    {
      if (i<offs)
      {
        done=FALSE;
        //StringAppendS(currRing->parameter[i]);
      }
      else
      {
        StringAppendS(currRing->names[i-offs]);
      }
      cnt--;
      if(cnt==0) break;
      if (done) StringAppendS(",");
    }
  }
  char * s=omStrDup(StringAppendS(""));
  if (s[strlen(s)-1]==',') s[strlen(s)-1]='\0';
  return s;
}

BOOLEAN singclap_isSqrFree(poly f)
{
  BOOLEAN b=FALSE;
  Off(SW_RATIONAL);
  //  Q / Fp
  if (((nGetChar() == 0) || (nGetChar() > 1) )
  &&(currRing->parameter==NULL))
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPFactoryP( f ) );
    if((nGetChar()>1)&&(!F.isUnivariate()))
      goto err;
    b=(BOOLEAN)isSqrFree(F);
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    //if (currRing->minpoly!=NULL)
    //{
    //  CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
    //                                             currRing->algring);
    //  Variable a=rootOf(mipo);
    //  CanonicalForm F( convSingAPFactoryAP( f,a ) );
    //  ...
    //}
    //else
    {
      CanonicalForm F( convSingTrPFactoryP( f ) );
      b=(BOOLEAN)isSqrFree(F);
    }
    Off(SW_RATIONAL);
  }
  else
  {
err:
    WerrorS( feNotImplemented );
  }
  return b;
}

poly singclap_det( const matrix m )
{
  int r=m->rows();
  if (r!=m->cols())
  {
    Werror("det of %d x %d matrix",r,m->cols());
    return NULL;
  }
  poly res=NULL;
  if (( nGetChar() == 0 || nGetChar() > 1 )
  && (currRing->parameter==NULL))
  {
    setCharacteristic( nGetChar() );
    CFMatrix M(r,r);
    int i,j;
    for(i=r;i>0;i--)
    {
      for(j=r;j>0;j--)
      {
        M(i,j)=convSingPFactoryP(MATELEM(m,i,j));
      }
    }
    res= convFactoryPSingP( determinant(M,r) ) ;
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    CFMatrix M(r,r);
    poly res;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                            currRing->algring);
      Variable a=rootOf(mipo);
      int i,j;
      for(i=r;i>0;i--)
      {
        for(j=r;j>0;j--)
        {
          M(i,j)=convSingAPFactoryAP(MATELEM(m,i,j),a,currRing);
        }
      }
      res= convFactoryAPSingAP( determinant(M,r),currRing ) ;
    }
    else
    {
      int i,j;
      for(i=r;i>0;i--)
      {
        for(j=r;j>0;j--)
        {
          M(i,j)=convSingTrPFactoryP(MATELEM(m,i,j));
        }
      }
      res= convFactoryPSingTrP( determinant(M,r) );
    }
  }
  else
    WerrorS( feNotImplemented );
  Off(SW_RATIONAL);
  return res;
}

int singclap_det_i( intvec * m )
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
  int res= convFactoryISingI( determinant(M,m->rows())) ;
  Off(SW_RATIONAL);
  return res;
}
matrix singntl_HNF(matrix  m )
{
  int r=m->rows();
  if (r!=m->cols())
  {
    Werror("HNF of %d x %d matrix",r,m->cols());
    return NULL;
  }
  matrix res=mpNew(r,r);
  if (rField_is_Q(currRing))
  {

    CFMatrix M(r,r);
    int i,j;
    for(i=r;i>0;i--)
    {
      for(j=r;j>0;j--)
      {
        M(i,j)=convSingPFactoryP(MATELEM(m,i,j));
      }
    }
    CFMatrix *MM=cf_HNF(M);
    for(i=r;i>0;i--)
    {
      for(j=r;j>0;j--)
      {
        MATELEM(res,i,j)=convFactoryPSingP((*MM)(i,j));
      }
    }
    delete MM;
  }
  return res;
}
intvec* singntl_HNF(intvec*  m )
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
matrix singntl_LLL(matrix  m )
{
  int r=m->rows();
  int c=m->cols();
  matrix res=mpNew(r,c);
  if (rField_is_Q(currRing))
  {
    CFMatrix M(r,c);
    int i,j;
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        M(i,j)=convSingPFactoryP(MATELEM(m,i,j));
      }
    }
    CFMatrix *MM=cf_LLL(M);
    for(i=r;i>0;i--)
    {
      for(j=c;j>0;j--)
      {
        MATELEM(res,i,j)=convFactoryPSingP((*MM)(i,j));
      }
    }
    delete MM;
  }
  return res;
}
intvec* singntl_LLL(intvec*  m )
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

napoly singclap_alglcm ( napoly f, napoly g )
{

 // over Q(a) / Fp(a)
 if (nGetChar()==1) setCharacteristic( 0 );
 else               setCharacteristic( -nGetChar() );
 napoly res;

 if (currRing->minpoly!=NULL)
 {
   CanonicalForm mipo=convSingPFactoryP(((lnumber)currRing->minpoly)->z,
                                         currRing->algring);
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
   CanonicalForm F( convSingPFactoryP( f,currRing->algring ) ),
                 G( convSingPFactoryP( g,currRing->algring ) );
   CanonicalForm GCD;
   // calculate gcd
   GCD = gcd( F, G );

   // calculate lcm
   res= convFactoryPSingP( (F/GCD)*G, currRing->algring );
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
                         currRing->algring);
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
   CanonicalForm F( convSingPFactoryP( f,currRing->algring ) ),
                 G( convSingPFactoryP( g,currRing->algring ) );
   CanonicalForm GCD;

   GCD=gcd( F, G );

   if ((GCD!=1) && (GCD!=0))
   {
     ff= convFactoryPSingP( F/ GCD, currRing->algring );
     gg= convFactoryPSingP( G/ GCD, currRing->algring );
   }
 }

 Off(SW_RATIONAL);
}

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
