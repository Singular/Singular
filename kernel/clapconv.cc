// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapconv.cc,v 1.2 2005-01-27 16:35:30 Singular Exp $
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

static void convRec( const CanonicalForm & f, int * exp, poly & result );

static void convRecAlg( const CanonicalForm & f, int * exp, napoly & result );

static void convRecPP ( const CanonicalForm & f, int * exp, poly & result );
static void conv_RecPP ( const CanonicalForm & f, int * exp, poly & result, ring r );

static void convRecPTr ( const CanonicalForm & f, int * exp, napoly & result );

static void convRecAP ( const CanonicalForm & f, int * exp, poly & result );

static void convRecTrP ( const CanonicalForm & f, int * exp, poly & result, int offs );

static void convRecGFGF ( const CanonicalForm & f, int * exp, poly & result );

static number convClapNSingAN( const CanonicalForm &f);

CanonicalForm
convSingNClapN( number n )
{
  CanonicalForm term;
  /* does only work for Zp, Q */
  if ( getCharacteristic() != 0 )
  {
    Off(SW_USE_EZGCD);
    term = npInt( n );
  }
  else
  {
    if ( ((int)n) & 1 )
    {
      term = ((int)n) >>2;
    }
    else
    {
      if ( n->s == 3 )
      {
        MP_INT dummy;
        mpz_init_set( &dummy, &(n->z) );
        term = make_cf( dummy );
      }
      else  if ( n->s == 1 )
      {
        MP_INT num, den;
        On(SW_RATIONAL);
        mpz_init_set( &num, &(n->z) );
        mpz_init_set( &den, &(n->n) );
        term = make_cf( num, den, false );
      }
      else
      { // assume s == 0
        MP_INT num, den;
        mpz_init_set( &num, &(n->z) );
        mpz_init_set( &den, &(n->n) );
        term = make_cf( num, den, true );
      }
    }
  }
  return term;
}

number
convClapNSingN( const CanonicalForm & n)
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
    if ( n.den() == 1 )
      z->s = 3;
    else
    {
      z->n = gmp_denominator( n );
      z->s = 0;
    }
    return z;
  }
}

CanonicalForm conv_SingPClapP( poly p, ring r )
{
  CanonicalForm result = 0;
  int e, n = r->N;
  assume( rPar(r)==0);

  while ( p != NULL )
  {
    CanonicalForm term;
    /* does only work for finite fields */
    if ( getCharacteristic() != 0 )
    {
      Off(SW_USE_EZGCD);
      term = npInt( pGetCoeff( p ) );
    }
    else
    {
      if ( (int)(pGetCoeff( p )) & 1 )
      {
        term = ((int)( pGetCoeff( p ) )>>2);
      }
      else
      {
        if ( pGetCoeff( p )->s == 3 )
        {
          MP_INT dummy;
          mpz_init_set( &dummy, &(pGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else  if ( pGetCoeff( p )->s == 1 )
        {
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(pGetCoeff( p )->z) );
          mpz_init_set( &den, &(pGetCoeff( p )->n) );
          term = make_cf( num, den, false );
        }
        else
        { // assume s == 0
          MP_INT num, den;
          mpz_init_set( &num, &(pGetCoeff( p )->z) );
          mpz_init_set( &den, &(pGetCoeff( p )->n) );
          term = make_cf( num, den, true );
        }
      }
    }
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = p_GetExp( p, i, r )) != 0 )
         term *= power( Variable( i ), e );
    }
    result += term;
    p = pNext( p );
  }
  return result;
}

poly convClapPSingP ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = pVariables+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  //for ( int i = 0; i < n; i++ ) exp[i] = 0;
  memset(exp,0,n*sizeof(int));
  poly result = NULL;
  convRecPP( f, exp, result );
  delete [] exp;
  return result;
}

static void convRecPP ( const CanonicalForm & f, int * exp, poly & result )
{
  if (f == 0)
    return;
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l] = i.exp();
      convRecPP( i.coeff(), exp, result );
    }
    exp[l] = 0;
  }
  else
  {
    poly term = pInit();
    pNext( term ) = NULL;
    for ( int i = 1; i <= pVariables; i++ )
      pSetExp( term, i, exp[i]);
    pSetComp(term, 0);
    if ( getCharacteristic() != 0 )
    {
      pGetCoeff( term ) = nInit( f.intval() );
    }
    else
    {
      if ( f.isImm() )
        pGetCoeff( term ) = nInit( f.intval() );
      else
      {
        number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
        z->debug=123456;
#endif
        z->z = gmp_numerator( f );
        if ( f.den() == 1 )
          z->s = 3;
        else
        {
          z->n = gmp_denominator( f );
          z->s = 0;
        }
        pGetCoeff( term ) = z;
      }
    }
    pSetm( term );
    if ( nIsZero(pGetCoeff(term)) )
    {
      pDelete(&term);
    }
    else
    {
      result = pAdd( result, term );
    }
  }
}

poly conv_ClapPSingP ( const CanonicalForm & f, ring r )
{
//    cerr << " f = " << f << endl;
  int n = r->N+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  //for ( int i = 0; i < n; i++ ) exp[i] = 0;
  memset(exp,0,n*sizeof(int));
  poly result = NULL;
  conv_RecPP( f, exp, result, r );
  delete [] exp;
  return result;
}

static void
conv_RecPP ( const CanonicalForm & f, int * exp, poly & result, ring r )
{
  if (f == 0)
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
    if (rRing_has_Comp(r)) p_SetComp(term, 0, r);
    if ( getCharacteristic() != 0 )
    {
      pGetCoeff( term ) = n_Init( f.intval(), r );
    }
    else
    {
      if ( f.isImm() )
        pGetCoeff( term ) = n_Init( f.intval(), r );
      else
      {
        number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
        z->debug=123456;
#endif
        z->z = gmp_numerator( f );
        if ( f.den() == 1 )
          z->s = 3;
        else
        {
          z->n = gmp_denominator( f );
          z->s = 0;
        }
        pGetCoeff( term ) = z;
      }
    }
    p_Setm( term, r );
    if ( n_IsZero(pGetCoeff(term), r) )
    {
      p_Delete(&term,r);
    }
    else
    {
      result = p_Add_q( result, term, r );
    }
  }
}


CanonicalForm
convSingTrClapP( napoly p )
{
  CanonicalForm result = 0;
  int e, n = napVariables;

  while ( p!=NULL)
  {
    CanonicalForm term;
    /* does only work for finite fields */
    if ( getCharacteristic() != 0 )
    {
      Off(SW_USE_EZGCD);
      term = npInt( napGetCoeff( p ) );
    }
    else
    {
      On(SW_USE_EZGCD);
      //if ( (!(int)(napGetCoeff( p )) & 1 )
      //&&  ( napGetCoeff( p )->s == 0))
      //  naNormalize( naGetCoeff( p ) );
      if ( (int)(napGetCoeff( p )) & 1 )
        term = nlInt( napGetCoeff( p ) );
      else
      {
        if ( napGetCoeff( p )->s == 3 )
        {
          MP_INT dummy;
          mpz_init_set( &dummy, &(napGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else  if ( napGetCoeff( p )->s == 1 )
        {
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, false );
        }
        else
        { // assume s == 0
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, true );
        }
      }
    }
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = napGetExp( p, i )) != 0 )
        term *= power( Variable( i ), e );
    }
    result += term;
    p = napNext( p );
  }
  return result;
}

napoly
convClapPSingTr ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = napVariables+1;
  /* ASSERT( level( f ) <= napVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  // for ( int i = 0; i < n; i++ ) exp[i] = 0;
  memset(exp,0,n*sizeof(int));
  napoly result = NULL;
  convRecPTr( f, exp, result );
  delete [] exp;
  return result;
}

static void
convRecPTr ( const CanonicalForm & f, int * exp, napoly & result )
{
  if (f == 0)
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
    if ( getCharacteristic() != 0 )
    {
      napGetCoeff( term ) = npInit( f.intval() );
    }
    else
    {
      if ( f.isImm() )
        napGetCoeff( term ) = nlInit( f.intval() );
      else
      {
        number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
        z->debug=123456;
#endif
        z->z = gmp_numerator( f );
        if ( f.den() == 1 )
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
          }
          nacNormalize(z);
        }
        napGetCoeff( term ) = z;
      }
    }
    result = napAdd( result, term );
  }
}

CanonicalForm convSingAPClapAP ( poly p , const Variable & a)
{
  CanonicalForm result = 0;
  int e, n = pVariables;
  int off=rPar(currRing);

  On(SW_RATIONAL);
  while ( p!=NULL)
  {
    CanonicalForm term=convSingAClapA(((lnumber)pGetCoeff(p))->z,a);
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = pGetExp( p, i )) != 0 )
        term *= power( Variable( i + off), e );
    }
    result += term;
    p = pNext( p );
  }
  return result;
}

poly convClapAPSingAP ( const CanonicalForm & f )
{
  int n = pVariables+1+1/*rPar(currRing)*/;
  int * exp = new int[n];
  // for ( int i = 0; i < n; i++ ) exp[i] = 0;
  memset(exp,0,n*sizeof(int));
  poly result = NULL;
  convRecAP( f, exp, result );
  delete [] exp;
  return result;
}

static void
convRecAP ( const CanonicalForm & f, int * exp, poly & result )
{
  if (f == 0)
    return;
  int off=rPar(currRing);
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l] = i.exp();
      convRecAP( i.coeff(), exp, result );
    }
    exp[l] = 0;
  }
  else
  {
    napoly z=(napoly)convClapASingA( f );
    if (z!=NULL)
    {
      poly term = pInit();
      pNext( term ) = NULL;
      int i;
      for ( i = 1; i <= pVariables; i++ )
        pSetExp( term, i , exp[i+off]);
      pSetComp(term, 0);
      for ( i = 1; i <= off; i++ )
        //z->e[i-1]+=exp[i];
        napAddExp(z,i,exp[i]);
      pGetCoeff(term)=(number)omAlloc0Bin(rnumber_bin);
      ((lnumber)pGetCoeff(term))->z=z;
      pSetm( term );
      result = pAdd( result, term );
    }
  }
}

CanonicalForm convSingAClapA ( napoly p , const Variable & a )
{
  CanonicalForm result = 0;
  int e;

  while ( p!=NULL )
  {
    CanonicalForm term;
    /* does only work for finite fields:Z/p */
    if ( rField_is_Zp_a() )
    {
      Off(SW_USE_EZGCD);
      term = npInt( napGetCoeff( p ) );
    }
    else
    {
      On(SW_USE_EZGCD);
      //if ( (!(int)(napGetCoeff( p )) & 1 )
      //&&  ( napGetCoeff( p )->s == 0))
      //  naNormalize( naGetCoeff( p ) );
      if ( (int)(napGetCoeff( p )) & SR_INT )
        term = nlInt( napGetCoeff( p ) );
        //term = SR_TO_INT(napGetCoeff( p )) ;
      else
      {
        if ( napGetCoeff( p )->s == 3 )
        {
          MP_INT dummy;
          mpz_init_set( &dummy, &(napGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else  if ( napGetCoeff( p )->s == 1 )
        {
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, false );
        }
        else
        { // assume s == 0
          MP_INT num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, true );
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

static number convClapNSingAN( const CanonicalForm &f)
{
  if ((getCharacteristic() != 0) || ( f.isImm() ))
    return nacInit( f.intval() );
  else
  {
    number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
    z->debug=123456;
#endif
    z->z = gmp_numerator( f );
    if ( f.den() == 1 )
    {
      z->s = 3;
    }
    else
    {
      z->n = gmp_denominator( f );
      z->s = 0;
    }
    #ifdef LDEBUG
    nlDBTest(z,__FILE__,__LINE__);
    #endif
    return z;
  }
}

napoly convClapASingA ( const CanonicalForm & f )
{
  napoly a=NULL;
  napoly t;
  for( CFIterator i=f; i.hasTerms(); i++)
  {
    t=napNew();
    // napNext( t ) = NULL; //already done by napNew
    napGetCoeff(t)=convClapNSingAN( i.coeff() );
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
  return a;
}

CanonicalForm convSingTrPClapP ( poly p )
{
  CanonicalForm result = 0;
  int e, n = pVariables;
  int offs = rPar(currRing);

  while ( p!=NULL )
  {
    nNormalize(pGetCoeff(p));
    CanonicalForm term=convSingTrClapP(((lnumber)pGetCoeff(p))->z);

    if ((((lnumber)pGetCoeff(p))->n!=NULL)
    && (!errorreported))
    {
      WerrorS("conversion error: denominator!= 1");
    }

    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = pGetExp( p, i )) != 0 )
        term = term * power( Variable( i + offs ), e );
    }
    result += term;
    p = pNext( p );
  }
  return result;
}

poly convClapPSingTrP ( const CanonicalForm & f )
{
  int n = pVariables+1;
  int * exp = new int[n];
  // for ( int i = 0; i < n; i++ ) exp[i] = 0;
  memset(exp,0,n*sizeof(int));
  poly result = NULL;
  convRecTrP( f, exp, result , rPar(currRing) );
  delete [] exp;
  return result;
}

static void
convRecTrP ( const CanonicalForm & f, int * exp, poly & result , int offs)
{
  if (f == 0)
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
    pSetComp(term, 0);
    pGetCoeff(term)=(number)omAlloc0Bin(rnumber_bin);
    ((lnumber)pGetCoeff(term))->z=convClapPSingTr( f );
    pSetm( term );
    result = pAdd( result, term );
  }
}

#if 0
CanonicalForm
convSingGFClapGF( poly p )
{
  CanonicalForm result = 0;
  int e, n = pVariables;

  while ( p != NULL )
  {
    CanonicalForm term;
    term = make_cf_from_gf( pGetCoeff( p ) );
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
convClapGFSingGF ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = pVariables+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  //for ( int i = 0; i < n; i++ ) exp[i] = 0;
  memset(exp,0,n*sizeof(int));
  poly result = NULL;
  convRecGFGF( f, exp, result );
  delete [] exp;
  return result;
}

static void
convRecGFGF ( const CanonicalForm & f, int * exp, poly & result )
{
  if (f == 0)
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
    pSetComp(term, 0);
    pGetCoeff( term ) = (number) gf_value (f);
    pSetm( term );
    result = pAdd( result, term );
  }
}
#endif

int convClapISingI( const CanonicalForm & f)
{
  if (!f.isImm()) WerrorS("int overflow in det");
  return f.intval();
}
#endif
