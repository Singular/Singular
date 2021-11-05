/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file cf_factor.cc
 *
 * Interface to factorization and square free factorization algorithms.
 *
 * Used by: cf_irred.cc
 *
 * Header file: cf_algorithm.h
 *
**/


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_sqrfree.h"
#include "cf_algorithm.h"
#include "facFqFactorize.h"
#include "facFqSquarefree.h"
#include "cf_map.h"
#include "facAlgExt.h"
#include "facFactorize.h"
#include "singext.h"
#include "cf_util.h"
#include "fac_berlekamp.h"
#include "fac_cantzass.h"
#include "fac_univar.h"
#include "fac_multivar.h"

#include "int_int.h"
#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#include "factory/cf_gmp.h"
#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#if (__FLINT_RELEASE >= 20700)
#include <flint/nmod_mpoly_factor.h>
#include <flint/fmpq_mpoly_factor.h>
#include <flint/fq_nmod_mpoly_factor.h>
#endif
#endif

//static bool isUnivariateBaseDomain( const CanonicalForm & f )
//{
//    CFIterator i = f;
//    bool ok = i.coeff().inBaseDomain();
//    i++;
//    while ( i.hasTerms() && ( ok = ok && i.coeff().inBaseDomain() ) ) i++;
//    return ok;
//}

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
  int *exp_f=NEW_ARRAY(int,mv+1);
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
  DELETE_ARRAY(exp_f);
  return mv;
}

#if 1
//#ifndef NOSTREAMIO
void out_cf(const char *s1,const CanonicalForm &f,const char *s2)
{
  printf("%s",s1);
  if (f.isZero()) printf("+0");
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
          printf("%c",'a'+l-1);
          if (e!=1) printf("^%d",e);
        }
      }
      else
      {
        out_cf("+(",i.coeff(),")");
        if (e!=0)
        {
          printf("*%c",'a'+l-1);
          if (e!=1) printf("^%d",e);
        }
      }
    }
  }
  else
  {
    if ( f.isImm() )
    {
      if (CFFactory::gettype()==GaloisFieldDomain)
      {
         long a= imm2int (f.getval());
         if ( a == gf_q )
           printf ("+%ld", a);
         else  if ( a == 0L )
           printf ("+1");
         else  if ( a == 1L )
           printf ("+%c",gf_name);
         else
         {
           printf ("+%c",gf_name);
           printf ("^%ld",a);
         }
      }
      else
      {
        long l=f.intval();
        if (l<0) printf("%ld",l);
        else     printf("+%ld",l);
      }
    }
    else
    {
    #ifdef NOSTREAMIO
      if (f.inZ())
      {
        mpz_t m;
        gmp_numerator(f,m);
        char * str = new char[mpz_sizeinbase( m, 10 ) + 2];
        str = mpz_get_str( str, 10, m );
        puts(str);
        delete[] str;
        mpz_clear(m);
      }
      else if (f.inQ())
      {
        mpz_t m;
        gmp_numerator(f,m);
        char * str = new char[mpz_sizeinbase( m, 10 ) + 2];
        str = mpz_get_str( str, 10, m );
        while(str[strlen(str)]<' ') { str[strlen(str)]='\0'; }
        puts(str);putchar('/');
        delete[] str;
        mpz_clear(m);
        gmp_denominator(f,m);
        str = new char[mpz_sizeinbase( m, 10 ) + 2];
        str = mpz_get_str( str, 10, m );
        while(str[strlen(str)]<' ') { str[strlen(str)]='\0'; }
        puts(str);
        delete[] str;
        mpz_clear(m);
      }
    #else
       std::cout << f;
    #endif
    }
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
  //int n = L.length();
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
  //int n = L.length();
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
  if (!(f-t).isZero()) { printf("problem:\n");out_cf("factor:",f," has problems\n");}
}
//#endif
#endif

bool isPurePoly_m(const CanonicalForm & f)
{
  if (f.inBaseDomain()) return true;
  if (f.level()<0) return false;
  for (CFIterator i=f;i.hasTerms();i++)
  {
    if (!isPurePoly_m(i.coeff())) return false;
  }
  return true;
}
bool isPurePoly(const CanonicalForm & f)
{
  if (f.level()<=0) return false;
  for (CFIterator i=f;i.hasTerms();i++)
  {
    if (!(i.coeff().inBaseDomain())) return false;
  }
  return true;
}


/**
 * get_max_degree_Variable returns Variable with
 * highest degree. We assume f is *not* a constant!
**/
Variable
get_max_degree_Variable(const CanonicalForm & f)
{
  ASSERT( ( ! f.inCoeffDomain() ), "no constants" );
  int max=0, maxlevel=0, n=level(f);
  for ( int i=1; i<=n; i++ )
  {
    if (degree(f,Variable(i)) >= max)
    {
      max= degree(f,Variable(i)); maxlevel= i;
    }
  }
  return Variable(maxlevel);
}

/**
 * get_Terms: Split the polynomial in the containing terms.
 * getTerms: the real work is done here.
**/
void
getTerms( const CanonicalForm & f, const CanonicalForm & t, CFList & result )
{
  if ( getNumVars(f) == 0 ) result.append(f*t);
  else{
    Variable x(level(f));
    for ( CFIterator i=f; i.hasTerms(); i++ )
      getTerms( i.coeff(), t*power(x,i.exp()), result);
  }
}
CFList
get_Terms( const CanonicalForm & f ){
  CFList result,dummy,dummy2;
  CFIterator i;
  CFListIterator j;

  if ( getNumVars(f) == 0 ) result.append(f);
  else{
    Variable _x(level(f));
    for ( i=f; i.hasTerms(); i++ ){
      getTerms(i.coeff(), 1, dummy);
      for ( j=dummy; j.hasItem(); j++ )
        result.append(j.getItem() * power(_x, i.exp()));

      dummy= dummy2; // have to initalize new
    }
  }
  return result;
}


/**
 * homogenize homogenizes f with Variable x
**/
CanonicalForm
homogenize( const CanonicalForm & f, const Variable & x)
{
#if 0
  int maxdeg=totaldegree(f), deg;
  CFIterator i;
  CanonicalForm elem, result(0);

  for (i=f; i.hasTerms(); i++)
  {
    elem= i.coeff()*power(f.mvar(),i.exp());
    deg = totaldegree(elem);
    if ( deg < maxdeg )
      result += elem * power(x,maxdeg-deg);
    else
      result+=elem;
  }
  return result;
#else
  CFList Newlist, Termlist= get_Terms(f);
  int maxdeg=totaldegree(f), deg;
  CFListIterator i;
  CanonicalForm elem, result(0);

  for (i=Termlist; i.hasItem(); i++)
  {
    elem= i.getItem();
    deg = totaldegree(elem);
    if ( deg < maxdeg )
      Newlist.append(elem * power(x,maxdeg-deg));
    else
      Newlist.append(elem);
  }
  for (i=Newlist; i.hasItem(); i++) // rebuild
    result += i.getItem();

  return result;
#endif
}

CanonicalForm
homogenize( const CanonicalForm & f, const Variable & x, const Variable & v1, const Variable & v2)
{
#if 0
  int maxdeg=totaldegree(f), deg;
  CFIterator i;
  CanonicalForm elem, result(0);

  for (i=f; i.hasTerms(); i++)
  {
    elem= i.coeff()*power(f.mvar(),i.exp());
    deg = totaldegree(elem);
    if ( deg < maxdeg )
      result += elem * power(x,maxdeg-deg);
    else
      result+=elem;
  }
  return result;
#else
  CFList Newlist, Termlist= get_Terms(f);
  int maxdeg=totaldegree(f), deg;
  CFListIterator i;
  CanonicalForm elem, result(0);

  for (i=Termlist; i.hasItem(); i++)
  {
    elem= i.getItem();
    deg = totaldegree(elem,v1,v2);
    if ( deg < maxdeg )
      Newlist.append(elem * power(x,maxdeg-deg));
    else
      Newlist.append(elem);
  }
  for (i=Newlist; i.hasItem(); i++) // rebuild
    result += i.getItem();

  return result;
#endif
}

VAR int singular_homog_flag=1;

int cmpCF( const CFFactor & f, const CFFactor & g )
{
  if (f.exp() > g.exp()) return 1;
  if (f.exp() < g.exp()) return 0;
  if (f.factor() > g.factor()) return 1;
  return 0;
}

/**
 * factorization over \f$ F_p \f$ or \f$ Q \f$
**/
CFFList factorize ( const CanonicalForm & f, bool issqrfree )
{
  if ( f.inCoeffDomain() )
        return CFFList( f );
#ifndef NOASSERT
  Variable a;
  ASSERT (!hasFirstAlgVar (f, a), "f has an algebraic variable use factorize \
          ( const CanonicalForm & f, const Variable & alpha ) instead");
#endif
  //out_cf("factorize:",f,"==================================\n");
  if (! f.isUnivariate() ) // preprocess homog. polys
  {
    if ( singular_homog_flag && f.isHomogeneous())
    {
      Variable xn = get_max_degree_Variable(f);
      int d_xn = degree(f,xn);
      CFMap n;
      CanonicalForm F = compress(f(1,xn),n);
      CFFList Intermediatelist;
      Intermediatelist = factorize(F);
      CFFList Homoglist;
      CFFListIterator j;
      for ( j=Intermediatelist; j.hasItem(); j++ )
      {
        Homoglist.append(
            CFFactor( n(j.getItem().factor()), j.getItem().exp()) );
      }
      CFFList Unhomoglist;
      CanonicalForm unhomogelem;
      for ( j=Homoglist; j.hasItem(); j++ )
      {
        unhomogelem= homogenize(j.getItem().factor(),xn);
        Unhomoglist.append(CFFactor(unhomogelem,j.getItem().exp()));
        d_xn -= (degree(unhomogelem,xn)*j.getItem().exp());
      }
      if ( d_xn != 0 ) // have to append xn^(d_xn)
        Unhomoglist.append(CFFactor(CanonicalForm(xn),d_xn));
      if(isOn(SW_USE_NTL_SORT)) Unhomoglist.sort(cmpCF);
      return Unhomoglist;
    }
  }
  CFFList F;
  if ( getCharacteristic() > 0 )
  {
    if (f.isUnivariate())
    {
#ifdef HAVE_FLINT
#ifdef HAVE_NTL
      if (degree (f) < 300)
#endif
      {
        // use FLINT: char p, univariate
        nmod_poly_t f1;
        convertFacCF2nmod_poly_t (f1, f);
        nmod_poly_factor_t result;
        nmod_poly_factor_init (result);
        mp_limb_t leadingCoeff= nmod_poly_factor (result, f1);
        F= convertFLINTnmod_poly_factor2FacCFFList (result, leadingCoeff, f.mvar());
        nmod_poly_factor_clear (result);
        nmod_poly_clear (f1);
        if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
        return F;
      }
#endif
#ifdef HAVE_NTL
      { // NTL char 2, univariate
        if (getCharacteristic()==2)
        {
          // Specialcase characteristic==2
          if (fac_NTL_char != 2)
          {
            fac_NTL_char = 2;
            zz_p::init(2);
          }
          // convert to NTL using the faster conversion routine for characteristic 2
          GF2X f1=convertFacCF2NTLGF2X(f);
          // no make monic necessary in GF2
          //factorize
          vec_pair_GF2X_long factors;
          CanZass(factors,f1);

          // convert back to factory again using the faster conversion routine for vectors over GF2X
          F=convertNTLvec_pair_GF2X_long2FacCFFList(factors,LeadCoeff(f1),f.mvar());
          if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
          return F;
        }
      }
#endif
#ifdef HAVE_NTL
      {
        // use NTL char p, univariate
        if (fac_NTL_char != getCharacteristic())
        {
          fac_NTL_char = getCharacteristic();
          zz_p::init(getCharacteristic());
        }

        // convert to NTL
        zz_pX f1=convertFacCF2NTLzzpX(f);
        zz_p leadcoeff = LeadCoeff(f1);

        //make monic
        f1=f1 / LeadCoeff(f1);
        // factorize
        vec_pair_zz_pX_long factors;
        CanZass(factors,f1);

        F=convertNTLvec_pair_zzpX_long2FacCFFList(factors,leadcoeff,f.mvar());
        //test_cff(F,f);
        if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
        return F;
      }
#endif
#if !defined(HAVE_NTL) && !defined(HAVE_FLINT)
      // Use Factory without NTL and without FLINT: char p, univariate
      {
        if ( isOn( SW_BERLEKAMP ) )
          F=FpFactorizeUnivariateB( f, issqrfree );
        else
          F=FpFactorizeUnivariateCZ( f, issqrfree, 0, Variable(), Variable() );
        return F;
      }
#endif
    }
    else // char p, multivariate
    {
      if (CFFactory::gettype() == GaloisFieldDomain)
      {
        #if defined(HAVE_NTL)
        if (issqrfree)
        {
          CFList factors;
          Variable alpha;
          factors= GFSqrfFactorize (f);
          for (CFListIterator i= factors; i.hasItem(); i++)
            F.append (CFFactor (i.getItem(), 1));
        }
        else
        {
          Variable alpha;
          F= GFFactorize (f);
        }
        #else
        factoryError ("multivariate factorization over GF depends on NTL(missing)");
        return CFFList (CFFactor (f, 1));
        #endif
      }
      else
      {
        #if defined(HAVE_FLINT) && (__FLINT_RELEASE >= 20700) && defined(HAVE_NTL)
        if (!isOn(SW_USE_FL_FAC_P))
        {
        #endif
        #if defined(HAVE_NTL)
          if (issqrfree)
          {
            CFList factors;
            Variable alpha;
            factors= FpSqrfFactorize (f);
            for (CFListIterator i= factors; i.hasItem(); i++)
              F.append (CFFactor (i.getItem(), 1));
            goto end_charp;
          }
          else
          {
            Variable alpha;
            F= FpFactorize (f);
            goto end_charp;
          }
        #endif
        #if defined(HAVE_FLINT) && (__FLINT_RELEASE >= 20700) && defined(HAVE_NTL)
        }
        #endif
        #if defined(HAVE_FLINT) && (__FLINT_RELEASE >= 20700)
        nmod_mpoly_ctx_t ctx;
        nmod_mpoly_ctx_init(ctx,f.level(),ORD_LEX,getCharacteristic());
        nmod_mpoly_t Flint_f;
        nmod_mpoly_init(Flint_f,ctx);
        convFactoryPFlintMP(f,Flint_f,ctx,f.level());
        nmod_mpoly_factor_t factors;
        nmod_mpoly_factor_init(factors,ctx);
	int okay;
        if (issqrfree) okay=nmod_mpoly_factor_squarefree(factors,Flint_f,ctx);
        else           okay=nmod_mpoly_factor(factors,Flint_f,ctx);
        nmod_mpoly_t fac;
        nmod_mpoly_init(fac,ctx);
        CanonicalForm cf_fac;
        int cf_exp;
        cf_fac=nmod_mpoly_factor_get_constant_ui(factors,ctx);
        F.append(CFFactor(cf_fac,1));
        for(int i=nmod_mpoly_factor_length(factors,ctx)-1; i>=0; i--)
        {
          nmod_mpoly_factor_get_base(fac,factors,i,ctx);
          cf_fac=convFlintMPFactoryP(fac,ctx,f.level());
          cf_exp=nmod_mpoly_factor_get_exp_si(factors,i,ctx);
          F.append(CFFactor(cf_fac,cf_exp));
        }
        nmod_mpoly_factor_clear(factors,ctx);
        nmod_mpoly_clear(Flint_f,ctx);
        nmod_mpoly_ctx_clear(ctx);
	if (okay==0)
	{
	  Off(SW_USE_FL_GCD_P);
	  Off(SW_USE_FL_FAC_P);
	  F=factorize(f,issqrfree);
	  On(SW_USE_FL_GCD_P);
	  On(SW_USE_FL_FAC_P);
	}
        #endif
        #if !defined(HAVE_FLINT) || (__FLINT_RELEASE < 20700)
        #ifndef HAVE_NTL
        factoryError ("multivariate factorization depends on NTL/FLINT(missing)");
        return CFFList (CFFactor (f, 1));
        #endif
        #endif
      }
    }
  }
  else // char 0
  {
    bool on_rational = isOn(SW_RATIONAL);
    On(SW_RATIONAL);
    CanonicalForm cd = bCommonDen( f );
    CanonicalForm fz = f * cd;
    Off(SW_RATIONAL);
    if ( f.isUnivariate() )
    {
      CanonicalForm ic=icontent(fz);
      fz/=ic;
      if (fz.degree()==1)
      {
        F=CFFList(CFFactor(fz,1));
        F.insert(CFFactor(ic,1));
      }
      else
      #if defined(HAVE_FLINT) && (__FLINT_RELEASE>=20503)  && (__FLINT_RELEASE!= 20600)
      {
        // FLINT 2.6.0 has a bug:
        // factorize x^12-13*x^10-13*x^8+13*x^4+13*x^2-1 runs forever
        // use FLINT: char 0, univariate
        fmpz_poly_t f1;
        convertFacCF2Fmpz_poly_t (f1, fz);
        fmpz_poly_factor_t result;
        fmpz_poly_factor_init (result);
        fmpz_poly_factor(result, f1);
        F= convertFLINTfmpz_poly_factor2FacCFFList (result, fz.mvar());
        fmpz_poly_factor_clear (result);
        fmpz_poly_clear (f1);
        if ( ! ic.isOne() )
        {
           // according to convertFLINTfmpz_polyfactor2FcaCFFlist,
           //  first entry is in CoeffDomain
          CFFactor new_first( F.getFirst().factor() * ic );
          F.removeFirst();
          F.insert( new_first );
        }
      }
      goto end_char0;
      #elif defined(HAVE_NTL)
      {
        //use NTL
        ZZ c;
        vec_pair_ZZX_long factors;
        //factorize the converted polynomial
        factor(c,factors,convertFacCF2NTLZZX(fz));

        //convert the result back to Factory
        F=convertNTLvec_pair_ZZX_long2FacCFFList(factors,c,fz.mvar());
        if ( ! ic.isOne() )
        {
           // according to convertNTLvec_pair_ZZX_long2FacCFFList
           //  first entry is in CoeffDomain
          CFFactor new_first( F.getFirst().factor() * ic );
          F.removeFirst();
          F.insert( new_first );
        }
      }
      goto end_char0;
      #else
      {
        //Use Factory without NTL: char 0, univariate
        F = ZFactorizeUnivariate( fz, issqrfree );
        goto end_char0;
      }
      #endif
    }
    else // multivariate,  char 0
    {
      #if defined(HAVE_FLINT) && (__FLINT_RELEASE >= 20700)
      if (isOn(SW_USE_FL_FAC_0))
      {
        On (SW_RATIONAL);
        fmpz_mpoly_ctx_t ctx;
        fmpz_mpoly_ctx_init(ctx,f.level(),ORD_LEX);
        fmpz_mpoly_t Flint_f;
        fmpz_mpoly_init(Flint_f,ctx);
        convFactoryPFlintMP(fz,Flint_f,ctx,fz.level());
        fmpz_mpoly_factor_t factors;
        fmpz_mpoly_factor_init(factors,ctx);
        int rr;
        if (issqrfree) rr=fmpz_mpoly_factor_squarefree(factors,Flint_f,ctx);
        else           rr=fmpz_mpoly_factor(factors,Flint_f,ctx);
        if (rr==0) printf("fail\n");
        fmpz_mpoly_t fac;
        fmpz_mpoly_init(fac,ctx);
        CanonicalForm cf_fac;
        int cf_exp;
        fmpz_t c;
        fmpz_init(c);
        fmpz_mpoly_factor_get_constant_fmpz(c,factors,ctx);
        cf_fac=convertFmpz2CF(c);
        fmpz_clear(c);
        F.append(CFFactor(cf_fac,1));
        for(int i=fmpz_mpoly_factor_length(factors,ctx)-1; i>=0; i--)
        {
           fmpz_mpoly_factor_get_base(fac,factors,i,ctx);
           cf_fac=convFlintMPFactoryP(fac,ctx,f.level());
           cf_exp=fmpz_mpoly_factor_get_exp_si(factors,i,ctx);
           F.append(CFFactor(cf_fac,cf_exp));
        }
        fmpz_mpoly_factor_clear(factors,ctx);
        fmpz_mpoly_clear(Flint_f,ctx);
        fmpz_mpoly_ctx_clear(ctx);
        goto end_char0;
      }
      #endif
      #if defined(HAVE_NTL)
      On (SW_RATIONAL);
      if (issqrfree)
      {
        CFList factors= ratSqrfFactorize (fz);
        for (CFListIterator i= factors; i.hasItem(); i++)
          F.append (CFFactor (i.getItem(), 1));
      }
      else
      {
        F = ratFactorize (fz);
      }
      #endif
      #if !defined(HAVE_FLINT) || (__FLINT_RELEASE < 20700)
      #ifndef HAVE_NTL
      F=ZFactorizeMultivariate(fz, issqrfree);
      #endif
      #endif
    }

end_char0:
    if ( on_rational )
      On(SW_RATIONAL);
    else
      Off(SW_RATIONAL);
    if ( ! cd.isOne() )
    {
      CFFactor new_first( F.getFirst().factor() / cd );
      F.removeFirst();
      F.insert( new_first );
    }
  }

#if defined(HAVE_NTL)
end_charp:
#endif
  if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
  return F;
}

/**
 * factorization over \f$ F_p(\alpha) \f$ or \f$ Q(\alpha) \f$
**/
CFFList factorize ( const CanonicalForm & f, const Variable & alpha )
{
  if ( f.inCoeffDomain() )
    return CFFList( f );
  //out_cf("factorize:",f,"==================================\n");
  //out_cf("mipo:",getMipo(alpha),"\n");

  CFFList F;
  ASSERT( alpha.level() < 0 && getReduce (alpha), "not an algebraic extension" );
#ifndef NOASSERT
  Variable beta;
  if (hasFirstAlgVar(f, beta))
    ASSERT (beta == alpha, "f has an algebraic variable that \
                            does not coincide with alpha");
#endif
  int ch=getCharacteristic();
  if (ch>0)
  {
    if (f.isUnivariate())
    {
#ifdef HAVE_NTL
      if (/*getCharacteristic()*/ch==2)
      {
        // special case : GF2

        // remainder is two ==> nothing to do

        // set minimal polynomial in NTL using the optimized conversion routines for characteristic 2
        GF2X minPo=convertFacCF2NTLGF2X(getMipo(alpha,f.mvar()));
        GF2E::init (minPo);

        // convert to NTL again using the faster conversion routines
        GF2EX f1;
        if (isPurePoly(f))
        {
          GF2X f_tmp=convertFacCF2NTLGF2X(f);
          f1=to_GF2EX(f_tmp);
        }
        else
          f1=convertFacCF2NTLGF2EX(f,minPo);

        // make monic (in Z/2(a))
        GF2E f1_coef=LeadCoeff(f1);
        MakeMonic(f1);

        // factorize using NTL
        vec_pair_GF2EX_long factors;
        CanZass(factors,f1);

        // return converted result
        F=convertNTLvec_pair_GF2EX_long2FacCFFList(factors,f1_coef,f.mvar(),alpha);
        if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
        return F;
      }
#endif
#if (HAVE_FLINT && __FLINT_RELEASE >= 20400)
      {
        // use FLINT
        nmod_poly_t FLINTmipo, leadingCoeff;
        fq_nmod_ctx_t fq_con;

        nmod_poly_init (FLINTmipo, ch);
        nmod_poly_init (leadingCoeff, ch);
        convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

        fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        fq_nmod_poly_t FLINTF;
        convertFacCF2Fq_nmod_poly_t (FLINTF, f, fq_con);
        fq_nmod_poly_factor_t res;
        fq_nmod_poly_factor_init (res, fq_con);
        fq_nmod_poly_factor (res, leadingCoeff, FLINTF, fq_con);
        F= convertFLINTFq_nmod_poly_factor2FacCFFList (res, f.mvar(), alpha, fq_con);
        F.insert (CFFactor (Lc (f), 1));

        fq_nmod_poly_factor_clear (res, fq_con);
        fq_nmod_poly_clear (FLINTF, fq_con);
        nmod_poly_clear (FLINTmipo);
        nmod_poly_clear (leadingCoeff);
        fq_nmod_ctx_clear (fq_con);
        if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
        return F;
      }
#endif
#ifdef HAVE_NTL
      {
        // use NTL
        if (fac_NTL_char != ch)
        {
          fac_NTL_char = ch;
          zz_p::init(ch);
        }

        // set minimal polynomial in NTL
        zz_pX minPo=convertFacCF2NTLzzpX(getMipo(alpha));
        zz_pE::init (minPo);

        // convert to NTL
        zz_pEX f1=convertFacCF2NTLzz_pEX(f,minPo);
        zz_pE leadcoeff= LeadCoeff(f1);

        //make monic
        f1=f1 / leadcoeff; //leadcoeff==LeadCoeff(f1);

        // factorize
        vec_pair_zz_pEX_long factors;
        CanZass(factors,f1);

        // return converted result
        F=convertNTLvec_pair_zzpEX_long2FacCFFList(factors,leadcoeff,f.mvar(),alpha);
        //test_cff(F,f);
        if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
        return F;
      }
#endif
#if !defined(HAVE_NTL) && !defined(HAVE_FLINT)
      // char p, extension, univariate
      CanonicalForm c=Lc(f);
      CanonicalForm fc=f/c;
      F=FpFactorizeUnivariateCZ( fc, false, 1, alpha, Variable() );
      F.insert (CFFactor (c, 1));
#endif
    }
    else // char p, multivariate
    {
      #if (HAVE_FLINT && __FLINT_RELEASE >= 20700)
        // use FLINT
        nmod_poly_t FLINTmipo;
        fq_nmod_ctx_t fq_con;
        fq_nmod_mpoly_ctx_t ctx;

        nmod_poly_init (FLINTmipo, ch);
        convertFacCF2nmod_poly_t (FLINTmipo, getMipo (alpha));

        fq_nmod_ctx_init_modulus (fq_con, FLINTmipo, "Z");
        fq_nmod_mpoly_ctx_init(ctx,f.level(),ORD_LEX,fq_con);

        fq_nmod_mpoly_t FLINTF;
        fq_nmod_mpoly_init(FLINTF,ctx);
        convertFacCF2Fq_nmod_mpoly_t(FLINTF,f,ctx,f.level(),fq_con);
        fq_nmod_mpoly_factor_t res;
        fq_nmod_mpoly_factor_init (res, ctx);
        fq_nmod_mpoly_factor (res, FLINTF, ctx);
        F= convertFLINTFq_nmod_mpoly_factor2FacCFFList (res, ctx,f.level(),fq_con,alpha);
        //F.insert (CFFactor (Lc (f), 1));

        fq_nmod_mpoly_factor_clear (res, ctx);
        fq_nmod_mpoly_clear (FLINTF, ctx);
        nmod_poly_clear (FLINTmipo);
        fq_nmod_mpoly_ctx_clear (ctx);
        fq_nmod_ctx_clear (fq_con);
        if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
        return F;
      #elif defined(HAVE_NTL)
      F= FqFactorize (f, alpha);
      #else
      factoryError ("multivariate factorization over Z/pZ(alpha) depends on NTL/Flint(missing)");
      return CFFList (CFFactor (f, 1));
      #endif
    }
  }
  else // Q(a)[x]
  {
    if (f.isUnivariate())
    {
      F= AlgExtFactorize (f, alpha);
    }
    else //Q(a)[x1,...,xn]
    {
      #if defined(HAVE_NTL) || defined(HAVE_FLINT)
      F= ratFactorize (f, alpha);
      #else
      factoryError ("multivariate factorization over Q(alpha) depends on NTL or FLINT (missing)");
      return CFFList (CFFactor (f, 1));
      #endif
    }
  }
  if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
  return F;
}

/**
 * squarefree factorization
**/
CFFList sqrFree ( const CanonicalForm & f, bool sort )
{
//    ASSERT( f.isUnivariate(), "multivariate factorization not implemented" );
    CFFList result;

    if ( getCharacteristic() == 0 )
        result = sqrFreeZ( f );
    else
    {
        Variable alpha;
        if (hasFirstAlgVar (f, alpha))
          result = FqSqrf( f, alpha );
        else
          result= FpSqrf (f);
    }
    if (sort)
    {
      CFFactor buf= result.getFirst();
      result.removeFirst();
      result= sortCFFList (result);
      result.insert (buf);
    }
    return result;
}

