// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and factory
*/





#include "misc/auxiliary.h"

#include "factory/factory.h"

#include "coeffs/coeffs.h"

#include "coeffs/longrat.h" // snumber is necessary

#include "polys/monomials/p_polys.h"
#include "polys/sbuckets.h"
#include "polys/clapconv.h"

#include "simpleideals.h"

#define TRANSEXT_PRIVATES
#include "polys/ext_fields/transext.h"

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

static void conv_RecPP ( const CanonicalForm & f, int * exp, sBucket_pt result, ring r );

static void convRecTrP ( const CanonicalForm & f, int * exp, poly & result, int offs, const ring r );

//static void convRecGFGF ( const CanonicalForm & f, int * exp, poly & result );

static number convFactoryNSingAN( const CanonicalForm &f, const ring r);

poly convFactoryPSingP ( const CanonicalForm & f, const ring r )
{
  if (f.isZero()) return NULL;
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
  // assume f!=0
  if (LIKELY(! f.inCoeffDomain()))
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
    coeffs cf=r->cf;
    number n=cf->convFactoryNSingN(f, cf);
    if (UNLIKELY( n_IsZero(n, cf) ))
    {
      n_Delete(&n,cf);
    }
    else
    {
      poly term = p_Init(r);
      //pNext( term ) = NULL; // done by p_Init
      pGetCoeff(term)=n;
      p_SetExpV(term,exp,r);
      sBucket_Merge_m(result,term);
    }
  }
}

static inline void convPhalf(poly p,int l,poly &p1,poly &p2)
{
  p1=p;
  l=l/2;
  while(l>1) { p=pNext(p); l--; }
  p2=pNext(p);
  pNext(p)=NULL;
}

static inline poly convPunhalf(poly p1,poly p2)
{
  poly p=p1;
  while(pNext(p1)!=NULL) { p1=pNext(p1);}
  pNext(p1)=p2;
  return p;
}

#define MIN_CONV_LEN 7
static CanonicalForm convSingPFactoryP_intern( poly p, int l, BOOLEAN & setChar,const ring r )
{
  CanonicalForm result = 0;
  int e, n = rVar(r);
  assume(l==(int)pLength(p));

  if (l>MIN_CONV_LEN)
  {
    poly p1,p2;
    convPhalf(p,l,p1,p2);
    CanonicalForm P=convSingPFactoryP_intern(p1,l/2,setChar,r);
    P+=convSingPFactoryP_intern(p2,l-l/2,setChar,r);
    convPunhalf(p1,p2);
    return P;
  }
  BOOLEAN setChar_loc=setChar;
  setChar=FALSE;
  coeffs cf=r->cf;
  while ( p!=NULL )
  {
    CanonicalForm term=cf->convSingNFactoryN(pGetCoeff( p ),setChar_loc, cf);
    if (errorreported) break;
    setChar_loc=FALSE;
    for ( int i = 1; i <=n; i++ )
    {
      if (LIKELY( (e = p_GetExp( p, i, r)) != 0 ))
        term *= CanonicalForm( Variable( i ), e );
    }
    result += term;
    pIter( p );
 }
 return result;
}

CanonicalForm convSingPFactoryP( poly p, const ring r )
{
  if (rVar(r)>1)
  {
    BOOLEAN setChar=TRUE;
    return convSingPFactoryP_intern(p,pLength(p),setChar,r);
  }
  else
  {
    CanonicalForm result = 0;
    BOOLEAN setChar=TRUE;
    coeffs cf=r->cf;
    int e;
    while ( p!=NULL )
    {
      CanonicalForm term=cf->convSingNFactoryN(pGetCoeff( p ),setChar, cf);
      if (errorreported) break;
      setChar=FALSE;
      if (LIKELY( (e = p_GetExp( p, 1, r)) != 0 ))
        term *= CanonicalForm( Variable( 1 ), e );
      result += term;
      pIter( p );
    }
    return result;
  }
}

int convFactoryISingI( const CanonicalForm & f)
{
  if (!f.isImm()) WerrorS("int overflow in det");
  return f.intval();
}

CanonicalForm convSingAPFactoryAP ( poly p , const Variable & a, const ring r)
{
  CanonicalForm result = 0;
  int e, n = r-> N;
  int off=rPar(r);

  if (!rField_is_Zp_a(r))
    On(SW_RATIONAL);
  while ( p!=NULL)
  {
    CanonicalForm term=convSingAFactoryA(((poly)p_GetCoeff(p, r->cf->extRing)),a, r);
    for ( int i = 1; i <= n; i++ )
    {
      if ( (e = p_GetExp( p, i, r )) != 0 )
        term *= CanonicalForm( Variable( i + off), e );
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
  if (f.isZero()) return NULL;
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
  // assume f!=0
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
    poly z=(poly)convFactoryASingA( f,r );
    if (z!=NULL)
    {
      poly term = p_Init(r);
      //pNext( term ) = NULL; // done by p_Init
      int i;
      for ( i = rVar(r); i>0 ; i-- )
        p_SetExp( term, i , exp[i+var_start],r);
      //if (rRing_has_Comp(currRing->extRing)) p_SetComp(term, 0, currRing->extRing); // done by pInit
      if (par_start==0)
      {
        for ( i = 1; i <= var_start; i++ )
        //z->e[i-1]+=exp[i];
          p_AddExp(z,i,exp[i],r->cf->extRing);
      }
      else
      {
        for ( i = par_start+1; i <= var_start+rPar(r); i++ )
        //z->e[i-1]+=exp[i];
          p_AddExp(z,i,exp[i-par_start],r->cf->extRing);
      }
      p_GetCoeff(term, r->cf->extRing)=(number) z;
      p_Setm( term,r );
      result = p_Add_q( result, term, r );
    }
  }
}

CanonicalForm convSingAFactoryA ( poly p , const Variable & a, const ring r )
{
  CanonicalForm result = 0;
  int e;

  while ( p!=NULL )
  {
    CanonicalForm term;
    if ( rField_is_Zp_a(r) )
    {
      term = n_Int( p_GetCoeff( p, r->cf->extRing ), r->cf->extRing->cf );
    }
    else
    {
      if ( SR_HDL(p_GetCoeff( p, r->cf->extRing )) & SR_INT )
        term = SR_TO_INT(p_GetCoeff( p, r->cf->extRing )) ;
      else
      {
        if ( p_GetCoeff( p, r->cf->extRing )->s == 3 )
        {
          mpz_t dummy;
          mpz_init_set( dummy, (p_GetCoeff( p,r->cf->extRing )->z) );
          term = make_cf( dummy );
        }
        else
        {
          // assume s==0 or s==1
          mpz_t num, den;
          On(SW_RATIONAL);
          mpz_init_set( num, (p_GetCoeff( p, r->cf->extRing )->z) );
          mpz_init_set( den, (p_GetCoeff( p, r->cf->extRing )->n) );
          term = make_cf( num, den, ( p_GetCoeff( p, r->cf->extRing )->s != 1 ));
        }
      }
    }
    if ( (e = p_GetExp( p, 1, r->cf->extRing )) != 0 )
      term *= power( a , e );
    result += term;
    p = pNext( p );
  }
  return result;
}

static number convFactoryNSingAN( const CanonicalForm &f, const ring r)
{
  assume (r != NULL);
  assume (r->cf != NULL);
  assume (r->cf->extRing != NULL);
  // r->cf->extRing->cf has to be Q or Z/p (the supported types of factory)
  return n_convFactoryNSingN( f, r->cf->extRing->cf );
}

poly convFactoryASingA ( const CanonicalForm & f, const ring r )
{
  poly a=NULL;
  for( CFIterator i=f; i.hasTerms(); i++)
  {
    number n= convFactoryNSingAN( i.coeff(), r );
    if (n_IsZero(n,r->cf->extRing->cf))
    {
      n_Delete(&n,r->cf->extRing->cf);
    }
    else
    {
      poly t= p_Init (r->cf->extRing);
      pGetCoeff(t)=n;
      p_SetExp(t,1,i.exp(),r->cf->extRing);
      //p_Setm(t,r->cf->extRing);// not needed for rings with 1 variable
      a=p_Add_q(a,t,r->cf->extRing);
    }
  }
  if (a!=NULL)
  {
    if( r->cf->extRing != NULL )
      if (r->cf->extRing->qideal->m[0]!=NULL)
      {
        poly l=r->cf->extRing->qideal->m[0];
        if (p_GetExp(a,1,r->cf->extRing) >= p_GetExp(l,1,r->cf->extRing))
          a = p_PolyDiv (a, l, FALSE, r->cf->extRing); // ???
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
    //n_Normalize(p_GetCoeff(p, r), r->cf);

    // test if denominator is constant
    if (!errorreported && !p_IsConstant(DEN ((fraction)p_GetCoeff (p,r)),r->cf->extRing))
      WerrorS("conversion error: denominator!= 1");

    CanonicalForm term=convSingPFactoryP(NUM ((fraction)p_GetCoeff(p, r)),r->cf->extRing);

    // if denominator is not NULL it should be a constant at this point
    if (DEN ((fraction)p_GetCoeff(p,r)) != NULL)
    {
      CanonicalForm den= convSingPFactoryP(DEN ((fraction)p_GetCoeff(p, r)),r->cf->extRing);
      if (rChar (r) == 0)
        On (SW_RATIONAL);
      term /= den;
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

BOOLEAN convSingTrP(poly p, const ring r )
{
  while ( p!=NULL )
  {
    n_Normalize(p_GetCoeff(p, r), r->cf);

    // test if denominator is constant
    if (!p_IsConstant(DEN ((fraction)p_GetCoeff (p,r)),r->cf->extRing))
      return FALSE;
    pIter(p);
  }
  return TRUE;
}

poly convFactoryPSingTrP ( const CanonicalForm & f, const ring r )
{
  if (f.isZero()) return NULL;
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
  // assume f!= 0
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
    //pNext( term ) = NULL; // done by p_Init
    for ( int i = rVar(r); i>0; i-- )
      p_SetExp( term, i ,exp[i], r);
    //if (rRing_has_Comp(currRing)) p_SetComp(term, 0, currRing); // done by pInit
    pGetCoeff(term)=ntInit(convFactoryPSingP( f, r->cf->extRing ), r->cf);
    p_Setm( term,r );
    result = p_Add_q( result, term,r );
  }
}

#if 0
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

#endif
