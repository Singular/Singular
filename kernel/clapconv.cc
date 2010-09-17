// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id$
/*
* ABSTRACT: convert data between Singular and factory
*/


#include <kernel/mod2.h>
#ifdef HAVE_FACTORY
#include <omalloc/omalloc.h>
#include <kernel/structs.h>
#define SI_DONT_HAVE_GLOBAL_VARS
#include <kernel/clapconv.h>
#include <kernel/numbers.h>
#include <kernel/modulop.h>
#include <kernel/longalg.h>
#include <kernel/polys.h>
#include <kernel/febase.h>
#include <kernel/ring.h>
#include <kernel/sbuckets.h>
#include <kernel/ffields.h>

typedef __mpz_struct lint;

void out_cf(char *s1,const CanonicalForm &f,char *s2);

static void convRec( const CanonicalForm & f, int * exp, poly & result );

static void convRecAlg( const CanonicalForm & f, int * exp, napoly & result );

static void conv_RecPP ( const CanonicalForm & f, int * exp, sBucket_pt result, ring r );

static void convRecTrP ( const CanonicalForm & f, int * exp, poly & result, int offs, const ring r );

static void convRecGFGF ( const CanonicalForm & f, int * exp, poly & result );

static number convFactoryNSingAN( const CanonicalForm &f, const ring r);

CanonicalForm convSingNFactoryN( number n, const ring r )
{
  CanonicalForm term;
  /* does only work for Zp, Q */
  if ( getCharacteristic() != 0 )
  {
    term = npInt( n,r );
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
        lint dummy;
        mpz_init_set( &dummy,n->z );
        term = make_cf( dummy );
      }
      else
      {
        // assume s==0 or s==1
        lint num, den;
        On(SW_RATIONAL);
        mpz_init_set( &num, n->z );
        mpz_init_set( &den, n->n );
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
    gmp_numerator( n, z->z );
    if ( n.den().isOne() )
      z->s = 3;
    else
    {
      gmp_denominator( n, z->n );
      z->s = 0;
    }
    return z;
  }
}

poly convFactoryPSingP ( const CanonicalForm & f, const ring r )
{
//    cerr << " f = " << f << endl;
  int n = rVar(r)+1;
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
      gmp_numerator( f, z->z );
      if ( f.den().isOne() )
        z->s = 3;
      else
      {
        gmp_denominator( f, z->n );
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


CanonicalForm convSingPFactoryP( poly p, const ring r )
{
  CanonicalForm result = 0;
  int e, n = rVar(r);

  while ( p!=NULL)
  {
    CanonicalForm term;
    /* does only work for finite fields */
    if ( rField_is_Zp(r) )
    {
      term = npInt( pGetCoeff( p ),r );
    }
    else if (rField_is_Q(r))
    {
      if ( SR_HDL(pGetCoeff( p )) & SR_INT )
        term = SR_TO_INT( pGetCoeff( p ) );
      else
      {
        if ( pGetCoeff( p )->s == 3 )
        {
          lint dummy;
          mpz_init_set( &dummy, (pGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else
        {
          // assume s==0 or s==1
          lint num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, (pGetCoeff( p )->z) );
          mpz_init_set( &den, (pGetCoeff( p )->n) );
          term = make_cf( num, den, ( pGetCoeff( p )->s != 1 ));
        }
      }
    }
    else
    {
      WerrorS("conversion error");
      return result;
    }
    for ( int i = n; i >0; i-- )
    {
      if ( (e = p_GetExp( p, i, r)) != 0 )
        term *= power( Variable( i ), e );
    }
    result += term;
    pIter( p );
  }
  return result;
}

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a, const ring r)
{
  CanonicalForm result = 0;
  int e, n = pVariables;
  int off=rPar(currRing);

  On(SW_RATIONAL);
  while ( p!=NULL)
  {
    CanonicalForm term=convSingAFactoryA(((lnumber)pGetCoeff(p))->z,a, r);
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
convRecAP_R ( const CanonicalForm & f, int * exp, poly & result, int par_start, int var_start, const ring r) ;

poly convFactoryAPSingAP_R ( const CanonicalForm & f, int par_start, int var_start, const ring r )
{
  int n = rVar(r)+rPar(r)+1;
  int * exp = (int *)omAlloc0(n*sizeof(int));
  poly result = NULL;
  convRecAP_R( f, exp, result,par_start, var_start, r );
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

poly convFactoryAPSingAP ( const CanonicalForm & f, const ring r )
{
  return convFactoryAPSingAP_R(f,0,rPar(r),r);
}

static void convRecAP_R ( const CanonicalForm & f, int * exp, poly & result, int par_start, int var_start, const ring r )
{
  if (f.isZero())
    return;
  if ( ! f.inCoeffDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l] = i.exp();
      convRecAP_R( i.coeff(), exp, result, par_start, var_start, r);
    }
    exp[l] = 0;
  }
  else
  {
    napoly z=(napoly)convFactoryASingA( f,r );
    if (z!=NULL)
    {
      poly term = p_Init(r);
      pNext( term ) = NULL;
      int i;
      for ( i = rVar(r); i>0 ; i-- )
        p_SetExp( term, i , exp[i+var_start],r);
      //if (rRing_has_Comp(currRing->algring)) p_SetComp(term, 0, currRing->algring); // done by pInit
      if (par_start==0)
      {
        for ( i = 1; i <= var_start; i++ )
        //z->e[i-1]+=exp[i];
          p_AddExp(z,i,exp[i],r->algring);
      }
      else
      {
        for ( i = par_start+1; i <= var_start+rPar(currRing); i++ )
        //z->e[i-1]+=exp[i];
          p_AddExp(z,i,exp[i-par_start],r->algring);
      }
      pGetCoeff(term)=(number)omAlloc0Bin(rnumber_bin);
      ((lnumber)pGetCoeff(term))->z=z;
      p_Setm( term,r );
      result = p_Add_q( result, term, r );
    }
  }
}

CanonicalForm convSingAFactoryA ( napoly p , const Variable & a, const ring r )
{
  CanonicalForm result = 0;
  int e;

  while ( p!=NULL )
  {
    CanonicalForm term;
    /* does only work for finite fields:Z/p */
    if ( rField_is_Zp_a() )
    {
      term = npInt( napGetCoeff( p ),r );
    }
    else
    {
      if ( SR_HDL(napGetCoeff( p )) & SR_INT )
        term = SR_TO_INT(napGetCoeff( p )) ;
      else
      {
        if ( napGetCoeff( p )->s == 3 )
        {
          lint dummy;
          mpz_init_set( &dummy, (napGetCoeff( p )->z) );
          term = make_cf( dummy );
        }
        else
        {
          // assume s==0 or s==1
          lint num, den;
          On(SW_RATIONAL);
          mpz_init_set( &num, (napGetCoeff( p )->z) );
          mpz_init_set( &den, (napGetCoeff( p )->n) );
          term = make_cf( num, den, ( napGetCoeff( p )->s != 1 ));
        }
      }
    }
    if ( (e = p_GetExp( p, 1, r->algring )) != 0 )
      term *= power( a , e );
    result += term;
    p = pNext( p );
  }
  return result;
}

static number convFactoryNSingAN( const CanonicalForm &f, const ring r)
{
  if ( f.isImm() )
    return n_Init( f.intval(), r->algring );
  else
  {
    number z=(number)omAllocBin(rnumber_bin);
#if defined(LDEBUG)
    z->debug=123456;
#endif
    gmp_numerator( f, z->z );
    if ( f.den().isOne() )
    {
      z->s = 3;
    }
    else
    {
      gmp_denominator( f, z->n );
      z->s = 0;
      nlNormalize(z);
    }
    #ifdef LDEBUG
    nlDBTest(z,__FILE__,__LINE__);
    #endif
    return z;
  }
}

napoly convFactoryASingA ( const CanonicalForm & f, const ring r )
{
  poly a=NULL;
  poly t;
  for( CFIterator i=f; i.hasTerms(); i++)
  {
    t=napNew();
    // pNext( t ) = NULL; //already done by napNew
    pGetCoeff(t)=convFactoryNSingAN( i.coeff(), r );
    if (n_IsZero(napGetCoeff(t),r->algring))
    {
      p_Delete(&t,r->algring);
    }
    else
    {
      p_SetExp(t,1,i.exp(),r->algring);
      a=p_Add_q(a,t,r->algring);
    }
  }
  if (a!=NULL)
  {
    if (r->minpoly!=NULL)
    {
      lnumber l=(lnumber)r->minpoly;
      if (p_GetExp(a,1,r->algring) >= p_GetExp(l->z,1,r->algring))
        a = napRemainder( a, l->z);
    }
  }
  return a;
}

CanonicalForm convSingTrPFactoryP ( poly p, const ring r )
{
  CanonicalForm result = 0;
  int e, n = rVar(r);
  int offs = rPar(r);

  while ( p!=NULL )
  {
    n_Normalize(pGetCoeff(p),r);
    CanonicalForm term=convSingPFactoryP(((lnumber)pGetCoeff(p))->z,r->algring);

    if ((((lnumber)pGetCoeff(p))->n!=NULL)
    && (!errorreported))
    {
      WerrorS("conversion error: denominator!= 1");
    }

    for ( int i = n; i > 0; i-- )
    {
      if ( (e = p_GetExp( p, i,r )) != 0 )
        term = term * power( Variable( i + offs ), e );
    }
    result += term;
    p = pNext( p );
  }
  return result;
}

poly convFactoryPSingTrP ( const CanonicalForm & f, const ring r )
{
  int n = rVar(r)+1;
  int * exp = (int*)omAlloc0(n*sizeof(int));
  poly result = NULL;
  convRecTrP( f, exp, result , rPar(r), r );
  omFreeSize((ADDRESS)exp,n*sizeof(int));
  return result;
}

static void
convRecTrP ( const CanonicalForm & f, int * exp, poly & result , int offs, const ring r)
{
  if (f.isZero())
    return;
  if ( f.level() > offs )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      exp[l-offs] = i.exp();
      convRecTrP( i.coeff(), exp, result, offs, r );
    }
    exp[l-offs] = 0;
  }
  else
  {
    poly term = p_Init(r);
    pNext( term ) = NULL;
    for ( int i = rVar(r); i>0; i-- )
      p_SetExp( term, i ,exp[i], r);
    //if (rRing_has_Comp(currRing)) p_SetComp(term, 0, currRing); // done by pInit
    pGetCoeff(term)=(number)omAlloc0Bin(rnumber_bin);
    ((lnumber)pGetCoeff(term))->z=convFactoryPSingP( f, r->algring );
    p_Setm( term,r );
    result = p_Add_q( result, term,r );
  }
}

number   nlChineseRemainder(number *x, number *q,int rl)
// elemenst in the array are x[0..(rl-1)], q[0..(rl-1)]
{
#ifdef HAVE_FACTORY
  setCharacteristic( 0 ); // only in char 0
  CFArray X(rl), Q(rl);
  int i;
  for(i=rl-1;i>=0;i--)
  {
    X[i]=convSingNFactoryN(x[i],currRing); // may be larger MAX_INT
    Q[i]=convSingNFactoryN(q[i],currRing); // may be larger MAX_INT
  }
  CanonicalForm xnew,qnew;
  chineseRemainder(X,Q,xnew,qnew);
  number n=convFactoryNSingN(xnew);
  number p=convFactoryNSingN(qnew);
  number p2=nlIntDiv(p,nlInit(2, currRing));
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
