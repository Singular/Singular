/* emacs edit mode for this file is -*- C++ -*- */

//{{{ docu
//
// cf_factor.cc - factorization and square free algorithms.
//
// Used by: fac_multivar.cc, fac_univar.cc, cf_irred.cc
//
// Header file: cf_algorithm.h
//
//}}}


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
#include "cfGcdAlgExt.h"
#include "facAlgExt.h"
#include "facFactorize.h"
#include "singext.h"
#include "cf_util.h"

#include "int_int.h"
#ifdef HAVE_NTL
#include "NTLconvert.h"
#endif

#include <factory/cf_gmp.h>
#ifdef HAVE_FLINT
#include "FLINTconvert.h"
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
        printf("+%ld",f.intval());
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
        printf("%s",str);
        delete[] str;
        mpz_clear(m);
      }
      else if (f.inQ())
      {
        mpz_t m;
        gmp_numerator(f,m);
        char * str = new char[mpz_sizeinbase( m, 10 ) + 2];
        str = mpz_get_str( str, 10, m );
        printf("%s/",str);
        delete[] str;
        mpz_clear(m);
        gmp_denominator(f,m);
        str = new char[mpz_sizeinbase( m, 10 ) + 2];
        str = mpz_get_str( str, 10, m );
        printf("%s",str);
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


///////////////////////////////////////////////////////////////
// get_max_degree_Variable returns Variable with             //
// highest degree. We assume f is *not* a constant!          //
///////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////
// get_Terms: Split the polynomial in the containing terms.  //
// getTerms: the real work is done here.                     //
///////////////////////////////////////////////////////////////
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


///////////////////////////////////////////////////////////////
// homogenize homogenizes f with Variable x                  //
///////////////////////////////////////////////////////////////

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

int singular_homog_flag=1;

int cmpCF( const CFFactor & f, const CFFactor & g )
{
  if (f.exp() > g.exp()) return 1;
  if (f.exp() < g.exp()) return 0;
  if (f.factor() > g.factor()) return 1;
  return 0;
}

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
  if (! f.isUnivariate() )
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
#ifdef HAVE_NTL
#ifdef HAVE_FLINT
      if (degree (f) < 300)
      {
        nmod_poly_t f1;
        convertFacCF2nmod_poly_t (f1, f);
        nmod_poly_factor_t result;
        nmod_poly_factor_init (result);
        mp_limb_t leadingCoeff= nmod_poly_factor (result, f1);
        F= convertFLINTnmod_poly_factor2FacCFFList (result, leadingCoeff, f.mvar());
        nmod_poly_factor_clear (result);
        nmod_poly_clear (f1);
      }
      else
#endif
      {
        // USE NTL
        if (getCharacteristic()!=2)
        {
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
        }
        else /*getCharacteristic()==2*/
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
        }
      }
#else
      // Use Factory without NTL
      factoryError ("univariate factorization depends on NTL(missing)");
      return CFFList (CFFactor (f, 1));
#endif //HAVE_NTL
    }
    else
    {
      #ifdef HAVE_NTL
      if (issqrfree)
      {
        CFList factors;
        Variable alpha;
        if (CFFactory::gettype() == GaloisFieldDomain)
          factors= GFSqrfFactorize (f);
        else
          factors= FpSqrfFactorize (f);
        for (CFListIterator i= factors; i.hasItem(); i++)
          F.append (CFFactor (i.getItem(), 1));
      }
      else
      {
        Variable alpha;
        if (CFFactory::gettype() == GaloisFieldDomain)
          F= GFFactorize (f);
        else
          F= FpFactorize (f);
      }
      #else
      ASSERT( f.isUnivariate(), "multivariate factorization depends on NTL(missing)" );
      factoryError ("multivariate factorization depends on NTL(missing)");
      return CFFList (CFFactor (f, 1));
      #endif
    }
  }
  else
  {
    bool on_rational = isOn(SW_RATIONAL);
    On(SW_RATIONAL);
    CanonicalForm cd = bCommonDen( f );
    CanonicalForm fz = f * cd;
    Off(SW_RATIONAL);
    if ( f.isUnivariate() )
    {
      #ifdef HAVE_NTL
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
      else
      {
        if ( !F.getFirst().factor().inCoeffDomain() )
        {
          CFFactor new_first( 1 );
          F.insert( new_first );
        }
      }
      #else
      factoryError ("univariate factorization over Z depends on NTL(missing)"); 
      return CFFList (CFFactor (f, 1));
      #endif
    }
    else
    {
      #ifdef HAVE_NTL
      On (SW_RATIONAL);
      if (issqrfree)
      {
        CFList factors;
        factors= ratSqrfFactorize (fz);
        for (CFListIterator i= factors; i.hasItem(); i++)
          F.append (CFFactor (i.getItem(), 1));
      }
      else
        F = ratFactorize (fz);
      Off (SW_RATIONAL);
      #else
      factoryError ("multivariate factorization  depends on NTL(missing)");
      return CFFList (CFFactor (f, 1));
      #endif
    }

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

  //out_cff(F);
  if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
  return F;
}

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
  if (f.isUnivariate()&& (ch>0))
  {
#ifdef HAVE_NTL
    //USE NTL
    if (ch>2)
    {
#if (HAVE_FLINT && __FLINT_VERSION_MINOR >= 4)
      nmod_poly_t FLINTmipo, leadingCoeff;
      fq_nmod_ctx_t fq_con;

      nmod_poly_init (FLINTmipo, getCharacteristic());
      nmod_poly_init (leadingCoeff, getCharacteristic());
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
#else
      // First all cases with characteristic !=2
      // set remainder
      if (fac_NTL_char != getCharacteristic())
      {
        fac_NTL_char = getCharacteristic();
        zz_p::init(getCharacteristic());
      }

      // set minimal polynomial in NTL
      zz_pX minPo=convertFacCF2NTLzzpX(getMipo(alpha));
      zz_pE::init (minPo);

      // convert to NTL
      zz_pEX f1=convertFacCF2NTLzz_pEX(f,minPo);
      zz_pE leadcoeff= LeadCoeff(f1);

      //make monic
      f1=f1 / leadcoeff;

      // factorize using NTL
      vec_pair_zz_pEX_long factors;
      CanZass(factors,f1);

      // return converted result
      F=convertNTLvec_pair_zzpEX_long2FacCFFList(factors,leadcoeff,f.mvar(),alpha);
#endif
    }
    else if (/*getCharacteristic()*/ch==2)
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
    }
#else
    factoryError ("univariate factorization  depends on NTL(missing)");
    return CFFList (CFFactor (f, 1));
#endif //HAVE_NTL
  }
  else if (ch>0)
  {
    #ifdef HAVE_NTL
    F= FqFactorize (f, alpha);
    #else
    ASSERT( f.isUnivariate(), "multivariate factorization depends on NTL(missing)" );
    factoryError ("multivariate factorization  depends on NTL(missing)");
    return CFFList (CFFactor (f, 1));
    #endif

  }
  else if (f.isUnivariate() && (ch == 0)) // Q(a)[x]
  {
    F= AlgExtFactorize (f, alpha);
  }
  else //Q(a)[x1,...,xn]
  {
#ifdef HAVE_NTL
    F= ratFactorize (f, alpha);
#else
    ASSERT( f.isUnivariate(), "multivariate factorization  depends on NTL(missing)" );
    factoryError ("multivariate factorization  depends on NTL(missing)");
    return CFFList (CFFactor (f, 1));
#endif
  }
  if(isOn(SW_USE_NTL_SORT)) F.sort(cmpCF);
  return F;
}

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

