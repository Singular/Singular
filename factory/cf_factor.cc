/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_factor.cc,v 1.12 2002-03-08 10:31:46 Singular Exp $ */

//{{{ docu
//
// cf_factor.cc - factorization and square free algorithms.
//
// Used by: fac_multivar.cc, fac_univar.cc, cf_irred.cc
//
// Header file: cf_algorithm.h
//
//}}}

#include <config.h>

#include "cf_gmp.h"

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_berlekamp.h"
#include "fac_cantzass.h"
#include "fac_univar.h"
#include "fac_multivar.h"
#include "fac_sqrfree.h"
#include "cf_algorithm.h"

#include "int_int.h"
#if 0
#include "NTLconvert.h"
#endif

int getExp(); /* cf_char.cc */

static bool isUnivariateBaseDomain( const CanonicalForm & f )
{
    CFIterator i = f;
    bool ok = i.coeff().inBaseDomain();
    i++;
    while ( i.hasTerms() && ( ok = ok && i.coeff().inBaseDomain() ) ) i++;
    return ok;
}

void find_exp(const CanonicalForm & f, int * exp_f)
{
  if ( ! f.inCoeffDomain() )
  {
    int e=f.level();
    CFIterator i = f;
    if (i.exp() > exp_f[e]) exp_f[e]=i.exp();
    for (; i.hasTerms(); i++ )
    {
      find_exp(i.coeff(), exp_f);
    }
  }
}

int find_mvar(const CanonicalForm & f)
{
  int mv=f.level();
  int *exp_f=new int[mv+1];
  int i;
  for(i=mv;i>0;i--) exp_f[i]=0;
  find_exp(f,exp_f);
  for(i=mv;i>0;i--)
  {
    if ((exp_f[i]>0) && (exp_f[i]<exp_f[mv]))
    {
      mv=i;
    }
  }
  delete[] exp_f;
  return mv;
}

CFFList factorize ( const CanonicalForm & f, bool issqrfree )
{
  if ( f.inCoeffDomain() )
	return CFFList( f );
  int mv=f.level();
  if (! f.isUnivariate() )
  {
    mv=find_mvar(f);
    if (mv!=f.level())
    {
      swapvar(f,Variable(mv),f.mvar());
    }
  }
  CFFList F;
  if ( getCharacteristic() > 0 ) 
  {
    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    #if 0
    if (isOn(SW_USE_NTL) && (getExp()==1))
    { 
      printf("start ntl...\n");
      // USE NTL
      if (getCharacteristic()!=2)
      {
        // set remainder 
        ZZ r;
        r=getCharacteristic();    
        ZZ_pContext ccc(r);
        ccc.restore();
        // convert to NTL
        ZZ_pX f1=convertFacCF2NTLZZpX(f); 
        ZZ_p leadcoeff = LeadCoeff(f1);
        //make monic
        f1=f1 / LeadCoeff(f1);

        // factorize
        vec_pair_ZZ_pX_long factors;
        CanZass(factors,f1);
        
        // convert back to factory
        F=convertNTLvec_pair_ZZpX_long2FacCFFList(factors,leadcoeff,f.mvar());
      }
      else
      {
        // Specialcase characteristic==2
          
        // remainder is 2 --> nothing to set
          
        // convert to NTL using the faster conversion routine for characteristic 2
        GF2X f1=convertFacCF2NTLGF2X(f);
     
        // no make monic necessary in GF2

        //factorize
        vec_pair_GF2X_long factors;
        CanZass(factors,f1);
          
        // convert back to factory again using the faster conversion routine for vectors over GF2X
       F=convertNTLvec_pair_GF2X_long2FacCFFList(factors,LeadCoeff(f1),f.mvar());
      }
      printf("... end ntl\n");
    }
    else
    #endif
    {  // Use Factory without NTL
      if ( isOn( SW_BERLEKAMP ) )
         F=FpFactorizeUnivariateB( f, issqrfree );
      else    
        F=FpFactorizeUnivariateCZ( f, issqrfree, 0, Variable(), Variable() );
    }      
  }
  else
  {
    CanonicalForm cd = bCommonDen( f );
    CanonicalForm fz = f * cd;
    bool on_rational = isOn(SW_RATIONAL);
    Off(SW_RATIONAL);
    if ( f.isUnivariate() )
    {
      #if 0
      if (isOn(SW_USE_NTL))
      {  
        printf("start ntl...\n");
        //USE NTL
        ZZ c;
        vec_pair_ZZX_long factors;
        //factorize the converted polynomial
        factor(c,factors,convertFacCF2NTLZZX(fz));                
            
        //convert the result back to Factory
        F=convertNTLvec_pair_ZZX_long2FacCFFList(factors,c,fz.mvar());
        printf("... end ntl\n");
      }
      else
      #endif
      {            
        //Use Factory without NTL
        F = ZFactorizeUnivariate( fz, issqrfree );
      }
    }
    else
      F = ZFactorizeMultivariate( fz, issqrfree );

    if ( on_rational )
      On(SW_RATIONAL);
    if ( ! cd.isOne() )
    {
      if ( F.getFirst().factor().inCoeffDomain() )
      {
        CFFactor new_first( F.getFirst().factor() / cd );
        F.removeFirst();
        F.insert( new_first );
      }
      else
      {
        F.insert( CFFactor( 1/cd ) );
      }
    }
  }  

  if ((mv!=f.level()) && (! f.isUnivariate() ))
  {
    CFFListIterator J=F;
    for ( ; J.hasItem(); J++)
    {
      swapvar(J.getItem().factor(),Variable(mv),f.mvar());
    }
    swapvar(f,Variable(mv),f.mvar());
  }
  return F;
}

CFFList factorize ( const CanonicalForm & f, const Variable & alpha )
{
    ASSERT( alpha.level() < 0, "not an algebraic extension" );
    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    ASSERT( getCharacteristic() > 0, "char 0 factorization not implemented" );
    #if 0
    if  (isOn(SW_USE_NTL))
    {
      //USE NTL
      if (getCharacteristic()!=2)
      {        
        // First all cases with characteristic !=2
        // set remainder 
        ZZ r;
        r=getCharacteristic();    
        ZZ_pContext ccc(r);
        ccc.restore();
      
        // set minimal polynomial in NTL
        ZZ_pX minPo=convertFacCF2NTLZZpX(getMipo(alpha,f.mvar()));
        ZZ_pEContext c(minPo);
        c.restore();

        // convert to NTL
        ZZ_pEX f1=to_ZZ_pEX(convertFacCF2NTLZZpX(f)); 
        
        //make monic
        ZZ_pE leadcoeff= LeadCoeff(f1);
        f1=f1 / leadcoeff;

        // factorize using NTL
        vec_pair_ZZ_pEX_long factors;
        CanZass(factors,f1);

        // return converted result
        return convertNTLvec_pair_ZZpEX_long2FacCFFList(factors,leadcoeff,f.mvar(),alpha);
      }
      else
      {
        // special case : GF2
        
        // remainder is two ==> nothing to do

        // set minimal polynomial in NTL using the optimized conversion routines for characteristic 2
        GF2X minPo=convertFacCF2NTLGF2X(getMipo(alpha,f.mvar()));
        GF2EContext c(minPo);
        c.restore();
        
        // convert to NTL again using the faster conversion routines
        GF2EX f1=to_GF2EX(convertFacCF2NTLGF2X(f)); 
        
        // no make monic necessary in GF2
        
        // factorize using NTL
        vec_pair_GF2EX_long factors;
        CanZass(factors,f1);

        // return converted result
        return convertNTLvec_pair_GF2EX_long2FacCFFList(factors,LeadCoeff(f1),f.mvar(),alpha);
      }
      
    }
    else   
    #endif
    {
      return FpFactorizeUnivariateCZ( f, false, 1, alpha, Variable() );
    }
    
}

CFFList sqrFree ( const CanonicalForm & f, bool sort )
{
//    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    CFFList result;

    if ( getCharacteristic() == 0 )
	result = sqrFreeZ( f );
    else
	result = sqrFreeFp( f );

    return ( sort ? sortCFFList( result ) : result );
}

bool isSqrFree ( const CanonicalForm & f )
{
//    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    if ( getCharacteristic() == 0 )
	return isSqrFreeZ( f );
    else
	return isSqrFreeFp( f );
}

