/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_factor.cc,v 1.14 2002-08-19 11:10:41 Singular Exp $ */

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
#ifdef HAVE_NTL 
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
    if (e>=0)
    {
      if (i.exp() > exp_f[e]) exp_f[e]=i.exp();
    }
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

#if 0
void out_cf(char *s1,const CanonicalForm &f,char *s2)
{
  printf("%s",s1);
  if (f==0) printf("+0");
  //else if (! f.inCoeffDomain() )
  else if (! f.inBaseDomain() )
  {
    int l = f.level();
    for ( CFIterator i = f; i.hasTerms(); i++ )
    {
      int e=i.exp();
      if (i.coeff().isOne())
      {
        printf("+");
	if (e==0) printf("1");
	else 
	{
	  printf("v(%d)",l);
	  if (e!=1) printf("^%d",e);
        }
      }	
      else
      {
        out_cf("+(",i.coeff(),")");
        if (e!=0)
	{
	  printf("*v(%d)",l);
	  if (e!=1) printf("^%d",e);
	}  
      }	
    }
  }
  else
  {
    if ( f.isImm() )
    {
      printf("+%d",f.intval());
    }
    else printf("+...");
    //if (f.inZ()) printf("(Z)");
    //else if (f.inQ()) printf("(Q)");
    //else if (f.inFF()) printf("(FF)");
    //else if (f.inPP()) printf("(PP)");
    //else if (f.inGF()) printf("(PP)");
    //else
    if (f.inExtension()) printf("E(%d)",f.level());
  }
  printf("%s",s2);
}
void out_cff(CFFList &L)
{
  int n = L.length();
  CFFListIterator J=L;
  int j=0;
  for ( ; J.hasItem(); J++, j++ )
  {
    printf("F%d",j);out_cf(":",J.getItem().factor()," ^ ");
    printf("%d\n", J.getItem().exp());
  }
}
void test_cff(CFFList &L,const CanonicalForm & f)
{
  int n = L.length();
  CFFListIterator J=L;
  CanonicalForm t=1;
  int j=0;
  if (!(L.getFirst().factor().inCoeffDomain()))
    printf("first entry is not const\n");
  for ( ; J.hasItem(); J++, j++ )
  {
    CanonicalForm tt=J.getItem().factor();
    if (tt.inCoeffDomain() && (j!=0))
      printf("other entry is const\n");
    j=J.getItem().exp();
    while(j>0) { t*=tt; j--; }
  }
  if ((f-t)!=0) { printf("problem:\n");out_cf("factor:",f," has problems\n");}
}
#endif

static bool isPurePoly(const CanonicalForm & f)
{
  if (f.level()<=0) return false;
  for (CFIterator i=f;i.hasTerms();i++)
  {
    if (!(i.coeff().inBaseDomain())) return false;
  }
  return true;
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
    if ( getCharacteristic() == 0 ) Off(SW_USE_NTL);
  }
  CFFList F;
  if ( getCharacteristic() > 0 )
  {
    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    #ifdef HAVE_NTL
    if (isOn(SW_USE_NTL) && (isPurePoly(f)))
    {
      // USE NTL
      if (getCharacteristic()!=2)
      {
        // set remainder
        ZZ r;
        r=getCharacteristic();
        ZZ_pContext ccc(r);
        ccc.restore();
        ZZ_p::init(r);
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
        //test_cff(F,f);
      }
      else
      {
        // Specialcase characteristic==2
        ZZ r;r=2;
        ZZ_p::init(r);

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
      #ifdef HAVE_NTL
      if ((isOn(SW_USE_NTL)) && (isPurePoly(f)))
      {
        //USE NTL
        CanonicalForm ic=icontent(fz);
        fz/=ic;
        ZZ c;
        vec_pair_ZZX_long factors;
        //factorize the converted polynomial
        factor(c,factors,convertFacCF2NTLZZX(fz));

        //convert the result back to Factory
        F=convertNTLvec_pair_ZZX_long2FacCFFList(factors,c,fz.mvar());
        if ( ! ic.isOne() )
        {
          if ( F.getFirst().factor().inCoeffDomain() )
          {
            CFFactor new_first( F.getFirst().factor() * ic );
            F.removeFirst();
            F.insert( new_first );
          }
          else
            F.insert( CFFactor( ic ) );
        }
        if ( F.getFirst().factor().isOne() )
        {
          F.removeFirst();
        }
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
CanonicalForm fntl ( const CanonicalForm & f, int j )
{
  ZZX f1=convertFacCF2NTLZZX(f);
  return convertZZ2CF(coeff(f1,j));
}  

CFFList factorize ( const CanonicalForm & f, const Variable & alpha )
{
    CFFList F;
    ASSERT( alpha.level() < 0, "not an algebraic extension" );
    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    ASSERT( getCharacteristic() > 0, "char 0 factorization not implemented" );
    #ifdef HAVE_NTL
    if  (isOn(SW_USE_NTL))
    {
      //USE NTL
      if (1 ) //getCharacteristic()!=2)
      {
        // First all cases with characteristic !=2
        // set remainder
        ZZ r;
        r=getCharacteristic();
        ZZ_pContext ccc(r);
        ccc.restore();

        // set minimal polynomial in NTL
        ZZ_pX minPo=convertFacCF2NTLZZpX(getMipo(alpha));
        ZZ_pEContext c(minPo);

        c.restore();

        // convert to NTL
        ZZ_pEX f1=convertFacCF2NTLZZ_pEX(f,minPo);

        //make monic
        ZZ_pE leadcoeff= LeadCoeff(f1);
        f1=f1 / leadcoeff;

        // factorize using NTL
        vec_pair_ZZ_pEX_long factors;
        CanZass(factors,f1);

        // return converted result
        F=convertNTLvec_pair_ZZpEX_long2FacCFFList(factors,leadcoeff,f.mvar(),alpha);
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
        GF2X f_tmp=convertFacCF2NTLGF2X(f);
        GF2EX f1=to_GF2EX(f_tmp);

        // no make monic necessary in GF2

        // factorize using NTL
        vec_pair_GF2EX_long factors;
        CanZass(factors,f1);

        // return converted result
        F=convertNTLvec_pair_GF2EX_long2FacCFFList(factors,LeadCoeff(f1),f.mvar(),alpha);
      }

    }
    else
    #endif
    {
	printf("factorize without NTL: alg. ext.\n");
      F=FpFactorizeUnivariateCZ( f, false, 1, alpha, Variable() );
    }
    return F;
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

