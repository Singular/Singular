// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: clapconv.cc,v 1.6 1997-04-25 15:03:51 obachman Exp $
/*
* ABSTRACT: convert data between Singular and factory
*/


#include "mod2.h"
#ifdef HAVE_FACTORY
#include "tok.h"
#define SI_DONT_HAVE_GLOBAL_VARS
#include "clapconv.h"
#include "numbers.h"
#include "polys.h"
#include "modulop.h"
#include "mmemory.h"
#include "febase.h"
#include "ipid.h"
#include "ring.h"

static void convRec( const CanonicalForm & f, int * exp, poly & result );

static void convRecAlg( const CanonicalForm & f, int * exp, alg & result );

static void convRecPP ( const CanonicalForm & f, int * exp, poly & result );

static void convRecPTr ( const CanonicalForm & f, int * exp, alg & result );

static void convRecAP ( const CanonicalForm & f, int * exp, poly & result );

static void convRecTrP ( const CanonicalForm & f, int * exp, poly & result, int offs );

static number convClapNSingAN( const CanonicalForm &f);

CanonicalForm
convSingPClapP( poly p )
{
  CanonicalForm result = 0;
  int e, n = pVariables;

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
      if ( (e = pGetExp( p, i )) != 0 )
         term = term * power( Variable( i ), e );
    }
    result = result + term;
    p = pNext( p );
  }
  return result;
}

poly
convClapPSingP ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = pVariables+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  for ( int i = 0; i < n; i++ )
    exp[i] = 0;
  poly result = NULL;
  convRecPP( f, exp, result );
  delete [] exp;
  return result;
}

static void
convRecPP ( const CanonicalForm & f, int * exp, poly & result )
{
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
    poly term = pNew();
    pNext( term ) = 0;
    for ( int i = 0; i <= pVariables; i++ )
      pGetExp( term, i ) = exp[i];
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
        number z=(number)Alloc(sizeof(rnumber));
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
    result = pAdd( result, term );
  }
}


CanonicalForm
convSingTrClapP( alg p )
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
      //  napNormalize( napGetCoeff( p ) );
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
          Print("switch to rational\n");
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, false );
        }
        else
        { // assume s == 0
          MP_INT num, den;
          On(SW_RATIONAL);
          Print("switch to rational ?1\n");
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, true );
        }
      }
    }
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = napGetExp( p, i )) != 0 )
        term = term * power( Variable( i ), e );
    }
    result = result + term;
    p = napNext( p );
  }
  return result;
}

alg
convClapPSingTr ( const CanonicalForm & f )
{
//    cerr << " f = " << f << endl;
  int n = napVariables+1;
  /* ASSERT( level( f ) <= napVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  for ( int i = 0; i < n; i++ )
    exp[i] = 0;
  alg result = NULL;
  convRecPTr( f, exp, result );
  delete [] exp;
  return result;
}

static void
convRecPTr ( const CanonicalForm & f, int * exp, alg & result )
{
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
    alg term = napNew();
    napNext( term ) = 0;
    for ( int i = 1; i <= napVariables; i++ )
      napGetExp( term, i ) = exp[i];
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
        number z=(number)Alloc(sizeof(rnumber));
        z->z = gmp_numerator( f );
        if ( f.den() == 1 )
          z->s = 3;
        else
        {
          z->n = gmp_denominator( f );
          if (mpz_cmp_si(&z->n,(long)1)==0)
          {
            mpz_clear(&z->n);
            z->s=3;
            if (mpz_size1(&z->z)<=MP_SMALL)
            {
              int ui=(int)mpz_get_si(&z->z);
              if ((((ui<<3)>>3)==ui)
              && (mpz_cmp_si(&z->z,(long)ui)==0))
              {
                mpz_clear(&z->z);
                Free((ADDRESS)z,sizeof(rnumber));
                z=INT_TO_SR(ui);
              }
            }
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

  while ( p!=NULL)
  {
    CanonicalForm term=convSingAClapA(((lnumber)pGetCoeff(p))->z,a);
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = pGetExp( p, i )) != 0 )
        term = term * power( Variable( i ), e );
    }
    result = result + term;
    p = pNext( p );
  }
  return result;
}

poly convClapAPSingAP ( const CanonicalForm & f )
{
  int n = pVariables+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  for ( int i = 0; i < n; i++ )
    exp[i] = 0;
  poly result = NULL;
  convRecAP( f, exp, result );
  delete [] exp;
  return result;
}

static void
convRecAP ( const CanonicalForm & f, int * exp, poly & result )
{
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
    alg z=(alg)convClapASingA( f );
    if (z!=NULL)
    {
      poly term = pNew();
      pNext( term ) = 0;
      for ( int i = 0; i <= pVariables; i++ )
        pGetExp( term, i ) = exp[i];
      pGetCoeff(term)=(number)Alloc0(sizeof(rnumber));
      ((lnumber)pGetCoeff(term))->z=z;
      pSetm( term );
      result = pAdd( result, term );
    }
  }
}

CanonicalForm convSingAClapA ( alg p , const Variable & a )
{
  CanonicalForm result = 0;
  int e;

  while ( p!=NULL )
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
      //  napNormalize( napGetCoeff( p ) );
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
          Print("switch to rational\n");
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, false );
        }
        else
        { // assume s == 0
          MP_INT num, den;
          On(SW_RATIONAL);
          Print("switch to rational ?2\n");
          mpz_init_set( &num, &(napGetCoeff( p )->z) );
          mpz_init_set( &den, &(napGetCoeff( p )->n) );
          term = make_cf( num, den, true );
        }
      }
    }
    if ( (e = napGetExp( p, 1 )) != 0 )
      term = term * power( a , e );
    result = result + term;
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
    number z=(number)Alloc(sizeof(rnumber));
    z->z = gmp_numerator( f );
    if ( f.den() == 1 )
      z->s = 3;
    else
    {
      z->n = gmp_denominator( f );
      z->s = 0;
    }
    return z;
  }
}

alg convClapASingA ( const CanonicalForm & f )
{
  alg a=NULL;
  alg t;
  for( CFIterator i=f; i.hasTerms(); i++)
  {
    t=napNew();
    napNext( t ) = NULL;
    napGetCoeff(t)=convClapNSingAN( i.coeff() );
    if (nacIsZero(napGetCoeff(t)))
    {
      napDelete(&t);
    }
    else
    {
      napGetExp(t,1)=i.exp();
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
    CanonicalForm term=convSingTrClapP(((lnumber)pGetCoeff(p))->z);
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = pGetExp( p, i )) != 0 )
        term = term * power( Variable( i + offs ), e );
    }
    result = result + term;
    p = pNext( p );
  }
  return result;
}

poly convClapPSingTrP ( const CanonicalForm & f )
{
  int n = pVariables+1;
  /* ASSERT( level( f ) <= pVariables, "illegal number of variables" ); */
  int * exp = new int[n];
  for ( int i = 0; i < n; i++ )
    exp[i] = 0;
  poly result = NULL;
  convRecTrP( f, exp, result , rPar(currRing) );
  delete [] exp;
  return result;
}

static void
convRecTrP ( const CanonicalForm & f, int * exp, poly & result , int offs)
{
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
    poly term = pNew();
    pNext( term ) = NULL;
    for ( int i = 0; i <= pVariables; i++ )
      pGetExp( term, i ) = exp[i];
    pGetCoeff(term)=(number)Alloc0(sizeof(rnumber));
    ((lnumber)pGetCoeff(term))->z=convClapPSingTr( f );
    pSetm( term );
    result = pAdd( result, term );
  }
}

int convClapISingI( const CanonicalForm & f)
{
  if (!f.isImm()) WerrorS("int overflow in det");
  return f.intval();
}
#endif
