// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapsing.cc,v 1.7 1997-06-04 19:45:09 obachman Exp $
/*
* ABSTRACT: interface between Singular and factory
*/


#include "mod2.h"
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include "tok.h"
#include "clapsing.h"
#include "ipid.h"
#include "numbers.h"
#include "subexpr.h"
#include "ipshell.h"
#include <factory.h>
#include "clapconv.h"
#ifdef HAVE_LIBFAC_P
#include <factor.h>
#endif

poly singclap_gcd ( poly f, poly g )
{
  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  if ( nGetChar() == 0 || nGetChar() > 1 )
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPClapP( f ) ), G( convSingPClapP( g ) );
    poly res=convClapPSingP( gcd( F, G ) );
    Off(SW_RATIONAL);
    return res;
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    poly res;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPClapAP( f,a ) ), G( convSingAPClapAP( g,a ) );
      res= convClapAPSingAP( gcd( F, G ) );
    }
    else
    {
      CanonicalForm F( convSingTrPClapP( f ) ), G( convSingTrPClapP( g ) );
      res= convClapPSingTrP( gcd( F, G ) );
    }
    Off(SW_RATIONAL);
    return res;
  }
  else
    WerrorS( "not implemented" );
  return NULL;
}

poly singclap_resultant ( poly f, poly g , poly x)
{
  int i=pIsPurePower(x);
  if (i==0)
  {
    WerrorS("3rd argument must be a ring variable");
    return NULL;
  }
  Variable X(i);
  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  if ( nGetChar() == 0 || nGetChar() > 1 )
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPClapP( f ) ), G( convSingPClapP( g ) );
    poly res=convClapPSingP( resultant( F, G, X ) );
    Off(SW_RATIONAL);
    return res;
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    poly res;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPClapAP( f,a ) ), G( convSingAPClapAP( g,a ) );
      res= convClapAPSingAP( resultant( F, G, X ) );
    }
    else
    {
      CanonicalForm F( convSingTrPClapP( f ) ), G( convSingTrPClapP( g ) );
      res= convClapPSingTrP( resultant( F, G, X ) );
    }
    Off(SW_RATIONAL);
    return res;
  }
  else
    WerrorS( "not implemented" );
  return NULL;
}

lists singclap_extgcd ( poly f, poly g )
{
  // for now there is only the possibility to handle univariate
  // polynomials over
  // Q and Fp ...
  poly res=NULL,pa=NULL,pb=NULL;
  On(SW_SYMMETRIC_FF);
  if ( nGetChar() == 0 || nGetChar() > 1 )
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPClapP( f ) ), G( convSingPClapP( g ) );
    if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
    {
      Off(SW_RATIONAL);
      WerrorS("not univariate");
      return NULL;
    }
    CanonicalForm Fa,Gb;
    res=convClapPSingP( extgcd( F, G, Fa, Gb ) );
    pa=convClapPSingP(Fa);
    pb=convClapPSingP(Gb);
    Off(SW_RATIONAL);
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    CanonicalForm Fa,Gb;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPClapAP( f,a ) ), G( convSingAPClapAP( g,a ) );
      if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
      {
        WerrorS("not univariate");
        return NULL;
      }
      res= convClapAPSingAP( extgcd( F, G, Fa, Gb ) );
      pa=convClapAPSingAP(Fa);
      pb=convClapAPSingAP(Gb);
    }
    else
    {
      CanonicalForm F( convSingTrPClapP( f ) ), G( convSingTrPClapP( g ) );
      if (!F.isUnivariate() || !G.isUnivariate() || F.mvar()!=G.mvar())
      {
        Off(SW_RATIONAL);
        WerrorS("not univariate");
        return NULL;
      }
      res= convClapPSingTrP( extgcd( F, G, Fa, Gb ) );
      pa=convClapPSingTrP(Fa);
      pb=convClapPSingTrP(Gb);
    }
    Off(SW_RATIONAL);
  }
  else
  {
    WerrorS( "not implemented" );
    return NULL;
  }
  lists L=(lists)Alloc(sizeof(slists));
  L->Init(3);
  L->m[0].rtyp=POLY_CMD;
  L->m[0].data=(void *)res;
  L->m[1].rtyp=POLY_CMD;
  L->m[1].data=(void *)pa;
  L->m[2].rtyp=POLY_CMD;
  L->m[2].data=(void *)pb;
  return L;
}

poly singclap_pdivide ( poly f, poly g )
{
  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  if ( nGetChar() == 0 || nGetChar() > 1 )
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPClapP( f ) ), G( convSingPClapP( g ) );
    return convClapPSingP( F / G );
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    poly res;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPClapAP( f,a ) ), G( convSingAPClapAP( g,a ) );
      res= convClapAPSingAP(  F / G  );
    }
    else
    {
      CanonicalForm F( convSingTrPClapP( f ) ), G( convSingTrPClapP( g ) );
      res= convClapPSingTrP(  F / G  );
    }
    Off(SW_RATIONAL);
    return res;
  }
  else
    WerrorS( "not implemented" );
  return NULL;
}

void singclap_divide_content ( poly f )
{
  if ( nGetChar() == 1 )
    setCharacteristic( 0 );
  else  if ( nGetChar() == -1 )
    return; /* not implemented for R */
  else  if ( nGetChar() < 0 )
    setCharacteristic( -nGetChar() );
  else
    setCharacteristic( nGetChar() );
  if ( f==NULL )
  {
    return;
  }
  else  if ( pNext( f ) == NULL )
  {
    pSetCoeff( f, nInit( 1 ) );
      pTest(f);
    return;
  }
  else
  {
    CFList L;
    CanonicalForm g, h;
    poly p = pNext(f);
    g = convSingTrClapP( ((lnumber)pGetCoeff(f))->z );
    L.append( g );
    while ( p && (g != 1) )
    {
      h = convSingTrClapP( ((lnumber)pGetCoeff(p))->z );
      p = pNext( p );
      g = gcd( g, h );
      L.append( h );
    }
    if ( g == 1 )
    {
      pTest(f);
      return;
    }  
    else
    {
      CFListIterator i;
      for ( i = L, p = f; i.hasItem(); i++, p=pNext(p) )
      {
        lnumber c=(lnumber)pGetCoeff(p);
        napDelete(&c->z);
        c->z=convClapPSingTr( i.getItem() / g );
      }
    }
    pTest(f);
  }
}

ideal singclap_factorize ( poly f, intvec ** v , int with_exps)
{
  // with_exps: 1 return only true factors
  //            2 return true factors and exponents
  //            0 return factors and exponents

  ideal res=NULL;
  if (f==NULL)
  {
    res=idInit(1,1);
    if (with_exps!=1)
    {
      (*v)=new intvec(1);
    }
    return res;
  }
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFFList L;
  number N=NULL;

  if ( (nGetChar() == 0) || (nGetChar() > 1) )
  {
    setCharacteristic( nGetChar() );
    if (nGetChar()==0) /* Q */
    {
      if ((with_exps!=0)&&(f!=NULL))
      {
        N=nCopy(pGetCoeff(f));
        pContent(f);
        number nn=nDiv(N,pGetCoeff(f));
        nDelete(&N);
        N=nn;
      }  
    }
    CanonicalForm F( convSingPClapP( f ) );
    if (nGetChar()==0) /* Q */
    {
      L = factorize( F );
    }
    else /* Fp */
    {
#ifdef HAVE_LIBFAC_P
      L = Factorize( F );
#else
      return NULL;
#endif
    }
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    if (currRing->minpoly!=NULL)
    {
      //if (nGetChar()==1)
      //{
      //  WerrorS("not implemented");
      //  return NULL;
      //}
      CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
      Variable a=rootOf(mipo);
      CanonicalForm F( convSingAPClapAP( f,a ) );
      L = factorize( F, a );
    }
    else
    {
      CanonicalForm F( convSingTrPClapP( f ) );
      if (nGetChar()==1) /* Q(a) */
      {
        L = factorize( F );
      }
      else /* Fp(a) */
      {
#ifdef HAVE_LIBFAC_P
        L = Factorize( F );
#else
        return NULL;
#endif
      }
    }
  }
  else
  {
    WerrorS( "not implemented" );
    goto end;
  }
  {
    // the first factor should be a constant
    if ( getNumVars(L.getFirst().factor()) != 0 )
      L.insert(CFFactor(1,1));
    // convert into ideal
    int n = L.length();
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
      if ((nGetChar()==0)||(nGetChar()>1))           /* Q, Fp */
        res->m[j] = convClapPSingP( J.getItem().factor() );
      else if ((nGetChar()==1)||(nGetChar()<-1))     /* Q(a), Fp(a) */
      {
        if (currRing->minpoly==NULL)
          res->m[j] = convClapPSingTrP( J.getItem().factor() );
        else
          res->m[j] = convClapAPSingAP( J.getItem().factor() );
      }
    }
    if (N!=NULL) 
    {
      pMultN(res->m[0],N);
      nDelete(&N);
    }  
    // delete constants
    if ((with_exps!=0) && (res!=NULL))
    {
      int i=IDELEMS(res)-1;
      for(;i>=0;i--)
      {
        if (pIsConstant(res->m[i]))
          pDelete(&(res->m[i]));
      }
      idSkipZeroes(res);
      if (res->m[0]==NULL)
      {
        res->m[0]=pOne();
      }
    }
  }
end:
  return res;
}

matrix singclap_irrCharSeries ( ideal I)
{
#ifdef HAVE_LIBFAC_P
  // for now there is only the possibility to handle polynomials over
  // Q and Fp ...
  matrix res=NULL;
  int i;
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFList L;
  ListCFList LL;
  if ( (nGetChar() == 0) || (nGetChar() > 1) )
  {
    setCharacteristic( nGetChar() );
    for(i=0;i<IDELEMS(I);i++)
    {
      L.append(convSingPClapP(I->m[i]));
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
      L.append(convSingTrPClapP(I->m[i]));
    }
  }
  else
  {
    WerrorS("not implemented");
    return res;
  }

  LL=IrrCharSeries(L);
  int m= LL.length(); // Anzahl Zeilen
  int n=0;
  ListIterator<CFList> LLi;
  CFListIterator Li;
  for ( LLi = LL; LLi.hasItem(); LLi++ )
  {
    n = max(LLi.getItem().length(),n);
  }
  res=mpNew(m,n);
  if ((m==0) || (n==0))
  {
    Warn("char_series returns %d x %d matrix from %d input polys (%d)\n",m,n,IDELEMS(I)+1,LL.length());
    iiWriteMatrix((matrix)I,"I",2,0);
  }
  for ( m=1, LLi = LL; LLi.hasItem(); LLi++, m++ )
  {
    for (n=1, Li = LLi.getItem(); Li.hasItem(); Li++, n++)
    {
      if ( (nGetChar() == 0) || (nGetChar() > 1) )
        MATELEM(res,m,n)=convClapPSingP(Li.getItem());
      else
        MATELEM(res,m,n)=convClapPSingTrP(Li.getItem());
    }
  }
  Off(SW_RATIONAL);
  return res;
#else
  return NULL;
#endif
}

char* singclap_neworder ( ideal I)
{
#ifdef HAVE_LIBFAC_P
  int i;
  Off(SW_RATIONAL);
  On(SW_SYMMETRIC_FF);
  CFList L;
  if ( (nGetChar() == 0) || (nGetChar() > 1) )
  {
    setCharacteristic( nGetChar() );
    for(i=0;i<IDELEMS(I);i++)
    {
      L.append(convSingPClapP(I->m[i]));
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
      L.append(convSingTrPClapP(I->m[i]));
    }
  }
  else
  {
    WerrorS("not implemented");
    return NULL;
  }

  List<int> IL=neworderint(L);
  ListIterator<int> Li;
  StringSet("");
  Li = IL;
  int* mark=(int*)Alloc0(pVariables*sizeof(int));
  int cnt=pVariables;
  loop
  {
    i=Li.getItem()-1;
    mark[i]=1;
    StringAppend(currRing->names[i]);
    Li++;
    cnt--;
    if(cnt==0) break;
    StringAppend(",");
    if(! Li.hasItem()) break;
  }
  for(i=0;i<pVariables;i++)
  {
    if(mark[i]==0)
    {
      StringAppend(currRing->names[i]);
      cnt--;
      if(cnt==0) break;
      StringAppend(",");
    }
  }
  return mstrdup(StringAppend(""));
#else
  return NULL;
#endif
}

BOOLEAN singclap_isSqrFree(poly f)
{
  BOOLEAN b=FALSE;
  Off(SW_RATIONAL);
  //  Q / Fp
  if ( (nGetChar() == 0) || (nGetChar() > 1) )
  {
    setCharacteristic( nGetChar() );
    CanonicalForm F( convSingPClapP( f ) );
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
    //  CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
    //  Variable a=rootOf(mipo);
    //  CanonicalForm F( convSingAPClapAP( f,a ) );
    //  ...
    //}
    //else
    {
      CanonicalForm F( convSingTrPClapP( f ) );
      b=(BOOLEAN)isSqrFree(F);
    }
    Off(SW_RATIONAL);
  }
  else
  {
err:
    WerrorS( "not implemented" );
  }
  return b;
}

poly singclap_det( const matrix m )
{
  poly res=NULL;
  if ( nGetChar() == 0 || nGetChar() > 1 )
  {
    setCharacteristic( nGetChar() );
    CFMatrix M(m->rows(),m->cols());
    int i,j;
    for(i=1;i<=m->rows();i++)
    {
      for(j=1;j<=m->cols();j++)
      {
        M(i,j)=convSingPClapP(MATELEM(m,i,j));
      }
    }
    res= convClapPSingP( determinant(M,m->rows())) ;
  }
  // and over Q(a) / Fp(a)
  else if (( nGetChar()==1 ) /* Q(a) */
  || (nGetChar() <-1))       /* Fp(a) */
  {
    if (nGetChar()==1) setCharacteristic( 0 );
    else               setCharacteristic( -nGetChar() );
    CFMatrix M(m->rows(),m->cols());
    poly res;
    if (currRing->minpoly!=NULL)
    {
      CanonicalForm mipo=convSingTrClapP(((lnumber)currRing->minpoly)->z);
      Variable a=rootOf(mipo);
      int i,j;
      for(i=1;i<=m->rows();i++)
      {
        for(j=1;j<=m->cols();j++)
        {
          M(i,j)=convSingAPClapAP(MATELEM(m,i,j),a);
        }
      }
      res= convClapAPSingAP( determinant(M,m->rows())) ;
    }
    else
    {
      int i,j;
      for(i=1;i<=m->rows();i++)
      {
        for(j=1;j<=m->cols();j++)
        {
          M(i,j)=convSingTrPClapP(MATELEM(m,i,j));
        }
      }
      res= convClapPSingTrP( determinant(M,m->rows()));
    }
  }
  else
    WerrorS( "not implemented" );
  Off(SW_RATIONAL);
  return res;
}

int singclap_det_i( intvec * m )
{
  setCharacteristic( 0 );
  CFMatrix M(m->rows(),m->cols());
  int i,j;
  for(i=1;i<=m->rows();i++)
  {
    for(j=1;j<=m->cols();j++)
    {
      M(i,j)=IMATELEM(*m,i,j);
    }
  }
  int res= convClapISingI( determinant(M,m->rows())) ;
  Off(SW_RATIONAL);
  return res;
}
/*==============================================================*/
/* interpreter interface : */
BOOLEAN jjGCD_P(leftv res, leftv u, leftv v)
{
  res->data=(void *)singclap_gcd((poly)(u->Data()),((poly)v->Data()));
  return FALSE;
}

BOOLEAN jjFAC_P(leftv res, leftv u)
{
  intvec *v=NULL;
  ideal f=singclap_factorize((poly)(u->Data()), &v, 0);
#ifndef HAVE_LIBFAC_P
  if (f==NULL) return TRUE;
#endif
  lists l=(lists)Alloc(sizeof(slists));
  l->Init(2);
  l->m[0].rtyp=IDEAL_CMD;
  l->m[0].data=(void *)f;
  l->m[1].rtyp=INTVEC_CMD;
  l->m[1].data=(void *)v;
  res->data=(void *)l;
  return FALSE;
}

BOOLEAN jjSQR_FREE_DEC(leftv res, leftv u,leftv dummy)
{
  intvec *v=NULL;
  int sw=(int)dummy->Data();
  ideal f=singclap_factorize((poly)(u->Data()), &v, sw);
  switch(sw)
  {
    case 0:
    case 2:
    {
      lists l=(lists)Alloc(sizeof(slists));
      l->Init(2);
      l->m[0].rtyp=IDEAL_CMD;
      l->m[0].data=(void *)f;
      l->m[1].rtyp=INTVEC_CMD;
      l->m[1].data=(void *)v;
      res->data=(void *)l;
      res->rtyp=LIST_CMD;
      return FALSE;
    }
    case 1:
      res->data=(void *)f;
      return f==NULL;
  }
  WerrorS("invalid switch");
  return TRUE;
}

#if 0
BOOLEAN jjIS_SQR_FREE(leftv res, leftv u)
{
  BOOLEAN b=singclap_factorize((poly)(u->Data()), &v, 0);
  res->data=(void *)b;
}
#endif

BOOLEAN jjEXTGCD_P(leftv res, leftv u, leftv v)
{
  res->data=singclap_extgcd((poly)u->Data(),(poly)v->Data());
  return (res->data==NULL);
}
BOOLEAN jjRESULTANT(leftv res, leftv u, leftv v, leftv w)
{
  res->data=singclap_resultant((poly)u->Data(),(poly)v->Data(), (poly)w->Data());
  return (res->data==NULL);
}
BOOLEAN jjCHARSERIES(leftv res, leftv u)
{
  res->data=singclap_irrCharSeries((ideal)u->Data());
  return (res->data==NULL);
}
#endif
