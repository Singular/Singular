// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id$
/*
* ABSTRACT: convert data between Singular and factory
*/


#include "config.h"
#include <misc/auxiliary.h>
#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#include <factory/factory.h>

#include <omalloc/omalloc.h>
#include <coeffs/coeffs.h>
#include <polys/monomials/p_polys.h>
#include <polys/sbuckets.h>
#include <polys/clapconv.h>

//typedef __mpz_struct lint;

void out_cf(char *s1,const CanonicalForm &f,char *s2);

static void conv_RecPP ( const CanonicalForm & f, int * exp, sBucket_pt result, ring r );

static number convFactoryNSingAN( const CanonicalForm &f, const ring r);

poly convFactoryPSingP ( const CanonicalForm & f, const ring r )
{
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
    int varoffset=r->cf->factoryVarOffset;
    for ( int i = 1; i <= r->N; i++ )
      p_SetExp( term, i-varoffset, exp[i], r);
    pGetCoeff( term )=r->cf->convFactoryNSingN(f, r->cf);
    p_Setm( term, r );
    if ( n_IsZero(pGetCoeff(term), r->cf) )
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
  BOOLEAN setChar=TRUE;

  while ( p!=NULL )
  {
    CanonicalForm term;
    term=r->cf->convSingNFactoryN(pGetCoeff( p ),setChar, r->cf);
    if (errorreported) break;
    setChar=FALSE;
    int varoffset=r->cf->factoryVarOffset;
    for ( int i = n; i >0; i-- )
    {
      if ( (e = p_GetExp( p, i, r)) != 0 )
        term *= power( Variable( i+varoffset ), e );
    }
    result += term;
    pIter( p );
  }
  return result;
}

CanonicalForm convSingAFactoryA( number pp, Variable a, const coeffs cf )
{
  CanonicalForm result = 0;
  int e;
  poly p=(poly)pp;
  BOOLEAN setChar=TRUE;

  while ( p!=NULL )
  {
    CanonicalForm term;
    term=cf->extRing->cf->convSingNFactoryN(pGetCoeff( p ),setChar, cf->extRing->cf);
    if (errorreported) break;
    setChar=FALSE;
    if ( (e = p_GetExp( p, 1, cf->extRing)) != 0 )
        term *= power( a, e );
    result += term;
    pIter( p );
  }
  return result;
}

number convFactoryASingA ( const CanonicalForm & f, const coeffs cf )
{
  poly a=NULL;
  poly t;
  for( CFIterator i=f; i.hasTerms(); i++)
  {
    t=p_New(cf->extRing);
    // pNext( t ) = NULL; //already done by napNew
    pGetCoeff(t)=cf->extRing->cf->convFactoryNSingN( i.coeff(), cf->extRing->cf );
    if (n_IsZero(p_GetCoeff(t,cf->extRing),cf->extRing->cf))
    {
      p_Delete(&t,cf->extRing);
    }
    else
    {
      p_SetExp(t,1,i.exp(),cf->extRing);
      a=p_Add_q(a,t,cf->extRing);
    }
  }
  number aa=(number)a;
  n_Normalize(aa,cf);
  return aa;
}


int convFactoryISingI( const CanonicalForm & f)
{
  if (!f.isImm()) WerrorS("int overflow in det");
  return f.intval();
}
#endif
