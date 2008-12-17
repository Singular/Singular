// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapconv.cc,v 1.12 2008-12-17 15:07:46 Singular Exp $
/*
* ABSTRACT: convert data between Singular and factory
*/


#include "mod2.h"
#ifdef HAVE_FACTORY
#include "omalloc.h"
#include "structs.h"
#define SI_DONT_HAVE_GLOBAL_VARS
#include "clapconv.h"
#include "numbers.h"
#include "modulop.h"
#include "longalg.h"
#include "polys.h"
#include "febase.h"
#include "ring.h"
#include "sbuckets.h"
#include "ffields.h"
void out_cf(char *s1,const CanonicalForm &f,char *s2);

static void convRec( const CanonicalForm & f, int * exp, poly & result );

static void convRecAlg( const CanonicalForm & f, int * exp, napoly & result );

static void conv_RecPP ( const CanonicalForm & f, int * exp, sBucket_pt result, ring r );

static void convRecPTr ( const CanonicalForm & f, int * exp, napoly & result );

static void convRecTrP ( const CanonicalForm & f, int * exp, poly & result, int offs );

static void convRecGFGF ( const CanonicalForm & f, int * exp, poly & result );

static number convFactoryNSingAN( const CanonicalForm &f);

CanonicalForm convSingNFactoryN( number n )
{
  CanonicalForm term;
  /* does only work for Zp, Q */
  if ( getCharacteristic() != 0 )
  {
    term = npInt( n );
  }
  else
  {
    if ( SR_HDL(n) & SR_INT )
    {
      term = SR_TO_INT(n);
    }
    else
    {
      if ( n->s == 3 )
      {
        MP_INT dummy;
        mpz_init_set( &dummy, &(n->z) );
        term = make_cf( dummy );
      }
      else
      {
        // assume s==0 or s==1
        MP_INT num, den;
        On(SW_RATIONAL);
        mpz_init_set( &num, &(n->z) );
        mpz_init_set( &den, &(n->n) );
        term = make_cf( num, den, ( n->s != 1 ));
      }
    }
  }
  return term;
}

number convFactoryNSingN( const CanonicalForm & n)
{
  if (n.isImm())
    return nInit(n.intval());
  else
  {
    number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
    z->debug=123456;
#endif
    z->z = gmp_numerator( n );
    if ( n.den().isOne() )
      z->s = 3;
    else
    {
      z->n = gmp_denominator( n );
      z->s = 0;
    }
    return z;
  }
}

CanonicalForm conv_SingPFactoryP( poly p, ring r )
{
  CanonicalForm result = 0;
  int e, n = r->N;
  assume( rPar(r)==0);

  if ( getCharacteristic() != 0 )
  {
    /* does only work for finite fields Z/p*/
    while ( p != NULL )
    {
      CanonicalForm term = npInt( pGetCoeff( p ) );
      for ( int i = n; i >0; i-- )
      {
        if ( (e = p_GetExp( p, i, r )) != 0 )
           term *= power( Variable( i ), e );
      }
      result += term;
      pIter( p );
    }
  }
  else
  {
    while ( p != NULL )
    {
      CanonicalForm term;
      if ( SR_HDL(pGetCoeff( p )) & SR_INT )
      {
        term = SR_TO_INT( pGetCoeff( p ) );
      }
      else
      {
        if ( pGetCoeff( p )->s == 3 )
        {
          MP_INT dummy;
          mpz_init_set( &dummy, &(pGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else
        {
          // assume s==1 or s==0
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(pGetCoeff( p )->z) );
          mpz_init_set( &den, &(pGetCoeff( p )->n) );
          term = make_cf( num, den, ( pGetCoeff( p )->s != 1 ));
        }
      }
      for ( int i = n; i >0; i-- )
      {
        if ( (e = p_GetExp( p, i, r )) != 0 )
           term *= power( Variable( i ), e );
      }
      result += term;
      pIter( p );
    }
  }
  return result;
}

poly conv_FactoryPSingP ( const CanonicalForm & f, ring r )
{
//    cerr << " f = " << f << endl;
  int n = r->N+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = (int*)omAlloc0(n*sizeof(int));
  sBucket_pt result_bucket=sBucketCreate(r);
  conv_RecPP( f, exp, result_bucket, r );
  poly result; int dummy;
  sBucketDestroyMerge(result_bucket,&result,&dummy);
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

static void conv_RecPP ( const CanonicalForm & f, int * exp, sBucket_pt result, ring r )
{
  if (f.isZero())
    return;
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l] = i.exp();
      conv_RecPP( i.coeff(), exp, result, r );
    }
    exp[l] = 0;
  }
  else
  {
    poly term = p_Init(r);
    pNext( term ) = NULL;
    for ( int i = 1; i <= r->N; i++ )
      p_SetExp( term, i, exp[i], r);
    //if (rRing_has_Comp(r)) p_SetComp(term, 0, r); // done by p_Init
    if ( f.isImm() )
      pGetCoeff( term ) = n_Init( f.intval(), r );
    else
    {
      number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
      z->debug=123456;
#endif
      z->z = gmp_numerator( f );
      if ( f.den().isOne() )
        z->s = 3;
      else
      {
        z->n = gmp_denominator( f );
        z->s = 0;
        nlNormalize(z);
      }
      pGetCoeff( term ) = z;
    }
    p_Setm( term, r );
    if ( n_IsZero(pGetCoeff(term), r) )
    {
      p_Delete(&term,r);
    }
    else
    {
      sBucket_Merge_p(result,term,1);
    }
  }
}


CanonicalForm convSingTrFactoryP( napoly p )
{
  CanonicalForm result = 0;
  int e, n = napVariables;

  while ( p!=NULL)
  {
    CanonicalForm term;
    /* does only work for finite fields */
    if ( getCharacteristic() != 0 )
    {
      term = npInt( napGetCoeff( p ) );
    }
    else
    {
      if ( SR_HDL(napGetCoeff( p )) & SR_INT )
        term = SR_TO_INT( napGetCoeff( p ) );
      else
      {
        if ( napGetCoeff( p )->s == 3 )
        {
          MP_INT dummy;
          mpz_init_set( &dummy, &(napGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else
        {
          // assume s==0 or s==1
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, ( napGetCoeff( p )->s != 1 ));
        }
      }
    }
    for ( int i = n; i >0; i-- )
    {
      if ( (e = napGetExp( p, i )) != 0 )
        term *= power( Variable( i ), e );
    }
    result += term;
    p = napNext( p );
  }
  return result;
}

napoly convFactoryPSingTr ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = napVariables+1;
  /* ASSERT( level( f ) <= napVariables, "illegal number of variables" ); */
  int * exp = (int *)omAlloc0(n*sizeof(int));
  napoly result = NULL;
  convRecPTr( f, exp, result );
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

static void convRecPTr ( const CanonicalForm & f, int * exp, napoly & result )
{
  if (f.isZero())
    return;
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
        exp[l] = i.exp();
        convRecPTr( i.coeff(), exp, result );
    }
    exp[l] = 0;
  }
  else
  {
    napoly term = napNew();
    // napNext( term ) = NULL; //already done by napNew
    for ( int i = 1; i <= napVariables; i++ )
      napSetExp( term, i , exp[i]);
    if ( f.isImm() )
      napGetCoeff( term ) = nacInit( f.intval() );
    else
    {
      number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
      z->debug=123456;
#endif
      z->z = gmp_numerator( f );
      if ( f.den().isOne() )
      {
        z->s = 3;
      }
      else
      {
        z->n = gmp_denominator( f );
        if (mpz_cmp_si(&z->n,(long)1)==0)
        {
          mpz_clear(&z->n);
          z->s=3;
        }
        else
        {
          z->s = 0;
          nacNormalize(z);
        }
      }
      napGetCoeff( term ) = z;
    }
    if (nacIsZero(pGetCoeff(term)))
    {
      napDelete(&term);
    }
    else
    {
      result = napAdd( result, term );
    }
  }
}

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a)
{
  CanonicalForm result = 0;
  int e, n = pVariables;
  int off=rPar(currRing);

  On(SW_RATIONAL);
  while ( p!=NULL)
  {
    CanonicalForm term=convSingAFactoryA(((lnumber)pGetCoeff(p))->z,a);
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = pGetExp( p, i )) != 0 )
        term *= power( Variable( i + off), e );
    }
    result += term;
    pIter( p );
  }
  return result;
}

static void
convRecAP_R ( const CanonicalForm & f, int * exp, poly & result, int par_start, int var_start) ;

poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start )
{
  int n = pVariables+1+rPar(currRing);
  int * exp = (int *)omAlloc0(n*sizeof(int));
  poly result = NULL;
  convRecAP_R( f, exp, result,par_start, var_start );
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

poly convFactoryAPSingAP ( const CanonicalForm & f )
{
  return convFactoryAPSingAP_R(f,0,rPar(currRing));
}

static void convRecAP_R ( const CanonicalForm & f, int * exp, poly & result, int par_start, int var_start )
{
  if (f.isZero())
    return;
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l] = i.exp();
      convRecAP_R( i.coeff(), exp, result, par_start, var_start );
    }
    exp[l] = 0;
  }
  else
  {
    napoly z=(napoly)convFactoryASingA( f );
    if (z!=NULL)
    {
      poly term = pInit();
      pNext( term ) = NULL;
      int i;
      for ( i = 1; i <= pVariables; i++ )
        pSetExp( term, i , exp[i+var_start]);
      //if (rRing_has_Comp(currRing->algring)) p_SetComp(term, 0, currRing->algring); // done by pInit
      if (par_start==0)
      {
        for ( i = 1; i <= var_start; i++ )
        //z->e[i-1]+=exp[i];
          napAddExp(z,i,exp[i]);
      }
      else
      {
        for ( i = par_start+1; i <= var_start+rPar(currRing); i++ )
        //z->e[i-1]+=exp[i];
          napAddExp(z,i,exp[i-par_start]);
      }
      pGetCoeff(term)=(number)omAlloc0Bin(rnumber_bin);
      ((lnumber)pGetCoeff(term))->z=z;
      pSetm( term );
      result = pAdd( result, term );
    }
  }
}

CanonicalForm convSingAFactoryA ( napoly p , const Variable & a )
{
  CanonicalForm result = 0;
  int e;

  while ( p!=NULL )
  {
    CanonicalForm term;
    /* does only work for finite fields:Z/p */
    if ( rField_is_Zp_a() )
    {
      term = npInt( napGetCoeff( p ) );
    }
    else
    {
      if ( SR_HDL(napGetCoeff( p )) & SR_INT )
        term = SR_TO_INT(napGetCoeff( p )) ;
      else
      {
        if ( napGetCoeff( p )->s == 3 )
        {
          MP_INT dummy;
          mpz_init_set( &dummy, &(napGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else
        {
          // assume s==0 or s==1
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, ( napGetCoeff( p )->s != 1 ));
        }
      }
    }
    if ( (e = napGetExp( p, 1 )) != 0 )
      term *= power( a , e );
    result += term;
    p = napNext( p );
  }
  return result;
}

static number convFactoryNSingAN( const CanonicalForm &f)
{
  if ( f.isImm() )
    return nacInit( f.intval() );
  else
  {
    number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
    z->debug=123456;
#endif
    z->z = gmp_numerator( f );
    if ( f.den().isOne() )
    {
      z->s = 3;
    }
    else
    {
      z->n = gmp_denominator( f );
      z->s = 0;
      nlNormalize(z);
    }
    #ifdef LDEBUG
    nlDBTest(z,__FILE__,__LINE__);
    #endif
    return z;
  }
}

napoly convFactoryASingA ( const CanonicalForm & f )
{
  napoly a=NULL;
  napoly t;
  for( CFIterator i=f; i.hasTerms(); i++)
  {
    t=napNew();
    // napNext( t ) = NULL; //already done by napNew
    napGetCoeff(t)=convFactoryNSingAN( i.coeff() );
    if (nacIsZero(napGetCoeff(t)))
    {
      napDelete(&t);
    }
    else
    {
      napSetExp(t,1,i.exp());
      a=napAdd(a,t);
    }
  }
  if (a!=NULL)
  {
    if (naMinimalPoly!=NULL)
    {
      if (napGetExp(a,1) >= napGetExp(naMinimalPoly,1))
        a = napRemainder( a, naMinimalPoly);
    }
  }
  return a;
}

CanonicalForm convSingTrPFactoryP ( poly p )
{
  CanonicalForm result = 0;
  int e, n = pVariables;
  int offs = rPar(currRing);

  while ( p!=NULL )
  {
    nNormalize(pGetCoeff(p));
    CanonicalForm term=convSingTrFactoryP(((lnumber)pGetCoeff(p))->z);

    if ((((lnumber)pGetCoeff(p))->n!=NULL)
    && (!errorreported))
    {
      WerrorS("conversion error: denominator!= 1");
    }

    for ( int i = n; i > 0; i-- )
    {
      if ( (e = pGetExp( p, i )) != 0 )
        term = term * power( Variable( i + offs ), e );
    }
    result += term;
    p = pNext( p );
  }
  return result;
}

poly convFactoryPSingTrP ( const CanonicalForm & f )
{
  int n = pVariables+1;
  int * exp = (int*)omAlloc0(n*sizeof(int));
  poly result = NULL;
  convRecTrP( f, exp, result , rPar(currRing) );
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

static void
convRecTrP ( const CanonicalForm & f, int * exp, poly & result , int offs)
{
  if (f.isZero())
    return;
  if ( f.level() > offs )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l-offs] = i.exp();
      convRecTrP( i.coeff(), exp, result, offs );
    }
    exp[l-offs] = 0;
  }
  else
  {
    poly term = pInit();
    pNext( term ) = NULL;
    for ( int i = 1; i <= pVariables; i++ )
      pSetExp( term, i ,exp[i]);
    //if (rRing_has_Comp(currRing)) p_SetComp(term, 0, currRing); // done by pInit
    pGetCoeff(term)=(number)omAlloc0Bin(rnumber_bin);
    ((lnumber)pGetCoeff(term))->z=convFactoryPSingTr( f );
    pSetm( term );
    result = pAdd( result, term );
  }
}

number   nlChineseRemainder(number *x, number *q,int rl)
// elemenst in the array are x[0..(rl-1)], q[0..(rl-1)]
{
#ifdef HAVE_FACTORY
  CFArray X(rl), Q(rl);
  int i;
  for(i=rl-1;i>=0;i--)
  {
    X[i]=CanonicalForm(nlInt(x[i]));
    Q[i]=CanonicalForm(nlInt(q[i]));
  }
  CanonicalForm xnew,qnew;
  chineseRemainder(X,Q,xnew,qnew);
  number n=convFactoryNSingN(xnew);
  number p=convFactoryNSingN(qnew);
  number p2=nlIntDiv(p,nlInit(2));
  if (nlGreater(n,p2))
  {
     number n2=nlSub(n,p);
     nlDelete(&n,currRing);
     n=n2;
  }
  nlDelete(&p,currRing);
  nlDelete(&p2,currRing);
  return n;
#else
  WerrorS("not implemented");
  return nlInit(0);
#endif
}

CanonicalForm
convSingGFFactoryGF( poly p )
{
  CanonicalForm result=CanonicalForm(0);
  int e, n = pVariables;

  while ( p != NULL )
  {
    CanonicalForm term;
    term = make_cf_from_gf( (int)(long)pGetCoeff( p ) );
    //int * A=(int *)&term;
    //Print("term=%x, == 0 ?: %d\n",*A, term.isZero());
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = pGetExp( p, i )) != 0 )
         term *= power( Variable( i ), e );
    }
    result += term;
    p = pNext( p );
  }
  return result;
}

poly
convFactoryGFSingGF ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = pVariables+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = (int*)omAlloc0(n*sizeof(int));
  poly result = NULL;
  convRecGFGF( f, exp, result );
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

static void
convRecGFGF ( const CanonicalForm & f, int * exp, poly & result )
{
  if (f.isZero())
    return;
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l] = i.exp();
      convRecGFGF( i.coeff(), exp, result );
    }
    exp[l] = 0;
  }
  else
  {
    poly term = pInit();
    pNext( term ) = NULL;
    for ( int i = 1; i <= pVariables; i++ )
      pSetExp( term, i, exp[i]);
    //if (rRing_has_Comp(currRing)) p_SetComp(term, 0, currRing); // done by pInit
    pGetCoeff( term ) = (number) gf_value (f);
    pSetm( term );
    result = pAdd( result, term );
  }
}

int convFactoryISingI( const CanonicalForm & f)
{
  if (!f.isImm()) WerrorS("int overflow in det");
  return f.intval();
}
#endif
