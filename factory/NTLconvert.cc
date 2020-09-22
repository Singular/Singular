
#include "config.h"

#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "fac_sqrfree.h"
#include "cf_algorithm.h"

#ifdef HAVE_NTL
#ifndef NOSTREAMIO
#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif
#endif
#include <string.h>
#include <NTL/ZZXFactoring.h>
#include <NTL/ZZ_pXFactoring.h>
#include <NTL/lzz_pXFactoring.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/ZZ_pEXFactoring.h>
#include <NTL/lzz_pEXFactoring.h>
#include <NTL/GF2EXFactoring.h>
#include <NTL/tools.h>
#include <NTL/mat_ZZ.h>
#include <NTL/version.h>
#include "int_int.h"
#include <limits.h>
#include "NTLconvert.h"

#ifdef HAVE_OMALLOC
#define Alloc(L) omAlloc(L)
#define Free(A,L) omFreeSize(A,L)
#else
#define Alloc(L) malloc(L)
#define Free(A,L) free(A)
#endif

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);


VAR long fac_NTL_char = -1;         // the current characterstic for NTL calls
                                // -1: undefined
#ifdef NTL_CLIENT               // in <NTL/tools.h>: using of name space NTL
NTL_CLIENT
#endif

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertFacCF2NTLZZpX
///
/// DESCRIPTION:
/// Conversion routine for Factory-type canonicalform into ZZpX of NTL,
/// i.e. polynomials over F_p. As a precondition for correct execution,
/// the characteristic has to a a prime number.
///
/// INPUT:  A canonicalform f
/// OUTPUT: The converted NTL-polynomial over F_p of type ZZpX
////////////////////////////////////////////////////////////////////////////////

ZZ_pX convertFacCF2NTLZZpX(const CanonicalForm & f)
{
  ZZ_pX ntl_poly;

  CFIterator i;
  i=f;

  int NTLcurrentExp=i.exp();
  int largestExp=i.exp();
  int k;

  // we now build up the NTL-polynomial
  ntl_poly.SetMaxLength(largestExp+1);

  for (;i.hasTerms();i++)
  {
    for (k=NTLcurrentExp;k>i.exp();k--)
    {
      SetCoeff(ntl_poly,k,0);
    }
    NTLcurrentExp=i.exp();

    SetCoeff(ntl_poly,NTLcurrentExp,to_ZZ_p (convertFacCF2NTLZZ (i.coeff())));
    NTLcurrentExp--;
  }

  //Set the remaining coefficients of ntl_poly to zero.
  // This is necessary, because NTL internally
  // also stores powers with zero coefficient,
  // whereas factory stores tuples of degree and coefficient
  //leaving out tuples if the coefficient equals zero
  for (k=NTLcurrentExp;k>=0;k--)
  {
    SetCoeff(ntl_poly,k,0);
  }

  //normalize the polynomial and return it
  ntl_poly.normalize();

  return ntl_poly;
}
zz_pX convertFacCF2NTLzzpX(const CanonicalForm & f)
{
  zz_pX ntl_poly;

  CFIterator i;
  i=f;

  int NTLcurrentExp=i.exp();
  int largestExp=i.exp();
  int k;

  // we now build up the NTL-polynomial
  ntl_poly.SetMaxLength(largestExp+1);

  for (;i.hasTerms();i++)
  {
    for (k=NTLcurrentExp;k>i.exp();k--)
    {
      SetCoeff(ntl_poly,k,0);
    }
    NTLcurrentExp=i.exp();

    CanonicalForm c=i.coeff();
    if (!c.isImm()) c=c.mapinto(); //c%= getCharacteristic();
    if (!c.isImm())
    {  //This case will never happen if the characteristic is in fact a prime
       // number, since all coefficients are represented as immediates
       out_cf("f:->",f,"\n");
       out_cf("c:->",c,"\n");
       #ifndef NOSTREAMIO
       cout<<"convertFacCF2NTLzz_pX: coefficient not immediate! : "<<f<<"\n";
       #else
       //NTL_SNS
       printf("convertFacCF2NTLzz_pX: coefficient not immediate!, char=%d\n",
              getCharacteristic());
       #endif
       NTL_SNS exit(1);
    }
    else
    {
      SetCoeff(ntl_poly,NTLcurrentExp,c.intval());
    }
    NTLcurrentExp--;
  }

  //Set the remaining coefficients of ntl_poly to zero.
  // This is necessary, because NTL internally
  // also stores powers with zero coefficient,
  // whereas factory stores tuples of degree and coefficient
  //leaving out tuples if the coefficient equals zero
  for (k=NTLcurrentExp;k>=0;k--)
  {
    SetCoeff(ntl_poly,k,0);
  }

  //normalize the polynomial and return it
  ntl_poly.normalize();

  return ntl_poly;
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertFacCF2NTLGF2X
///
/// DESCRIPTION:
/// Conversion routine for Factory-type canonicalform into GF2X of NTL,
/// i.e. polynomials over F_2. As precondition for correct execution,
/// the characteristic must equal two.
/// This is a special case of the more general conversion routine for
/// canonicalform to ZZpX. It is included because NTL provides additional
/// support and faster algorithms over F_2, moreover the conversion code
/// can be optimized, because certain steps are either completely obsolent
/// (like normalizing the polynomial) or they can be made significantly
/// faster (like building up the NTL-polynomial).
///
/// INPUT:  A canonicalform f
/// OUTPUT: The converted NTL-polynomial over F_2 of type GF2X
////////////////////////////////////////////////////////////////////////////////

GF2X convertFacCF2NTLGF2X(const CanonicalForm & f)
{
  //printf("convertFacCF2NTLGF2X\n");
  GF2X ntl_poly;

  CFIterator i;
  i=f;

  int NTLcurrentExp=i.exp();
  int largestExp=i.exp();
  int k;

  //building the NTL-polynomial
  ntl_poly.SetMaxLength(largestExp+1);

  for (;i.hasTerms();i++)
  {

    for (k=NTLcurrentExp;k>i.exp();k--)
    {
      SetCoeff(ntl_poly,k,0);
    }
    NTLcurrentExp=i.exp();

    if (!i.coeff().isImm()) i.coeff()=i.coeff().mapinto();
    if (!i.coeff().isImm())
    {
      #ifndef NOSTREAMIO
      cout<<"convertFacCF2NTLGF2X: coefficient not immediate! : " << f << "\n";
      #else
      //NTL_SNS
      printf("convertFacCF2NTLGF2X: coefficient not immediate!");
      #endif
      NTL_SNS exit(1);
    }
    else
    {
      SetCoeff(ntl_poly,NTLcurrentExp,i.coeff().intval());
    }
    NTLcurrentExp--;
  }
  for (k=NTLcurrentExp;k>=0;k--)
  {
    SetCoeff(ntl_poly,k,0);
  }
  //normalization is not necessary of F_2

  return ntl_poly;
}


////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLZZpX2CF
///
/// DESCRIPTION:
/// Conversion routine for NTL-Type ZZpX to Factory-Type canonicalform.
/// Additionally a variable x is needed as a parameter indicating the
/// main variable of the computed canonicalform. To guarantee the correct
/// execution of the algorithm, the characteristic has a be an arbitrary
/// prime number.
///
/// INPUT:  A canonicalform f, a variable x
/// OUTPUT: The converted Factory-polynomial of type canonicalform,
///         built by the main variable x
////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLZZpX2CF(const ZZ_pX & poly,const Variable & x)
{
  return convertNTLZZX2CF (to_ZZX (poly), x);
}

CanonicalForm convertNTLzzpX2CF(const zz_pX & poly,const Variable & x)
{
  //printf("convertNTLzzpX2CF\n");
  CanonicalForm bigone;


  if (deg(poly)>0)
  {
    // poly is non-constant
    bigone=0;
    bigone.mapinto();
    // Compute the canonicalform coefficient by coefficient,
    // bigone summarizes the result.
    for (int j=0;j<=deg(poly);j++)
    {
      if (coeff(poly,j)!=0)
      {
        bigone+=(power(x,j)*CanonicalForm(to_long(rep(coeff(poly,j)))));
      }
    }
  }
  else
  {
    // poly is immediate
    bigone=CanonicalForm(to_long(rep(coeff(poly,0))));
    bigone.mapinto();
  }
  return bigone;
}

CanonicalForm convertNTLZZX2CF(const ZZX & polynom,const Variable & x)
{
  //printf("convertNTLZZX2CF\n");
  CanonicalForm bigone;

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result
  bigone=0;
  ZZ coefficient;

  for (int j=0;j<=deg(polynom);j++)
  {
    coefficient=coeff(polynom,j);
    if (!IsZero(coefficient))
    {
      bigone += (power(x,j)*convertZZ2CF(coefficient));
    }
  }
  return bigone;
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLGF2X2CF
///
/// DESCRIPTION:
/// Conversion routine for NTL-Type GF2X to Factory-Type canonicalform,
/// the routine is again an optimized special case of the more general
/// conversion to ZZpX. Additionally a variable x is needed as a
/// parameter indicating the main variable of the computed canonicalform.
/// To guarantee the correct execution of the algorithm the characteristic
/// has a be an arbitrary prime number.
///
/// INPUT:  A canonicalform f, a variable x
/// OUTPUT: The converted Factory-polynomial of type canonicalform,
///         built by the main variable x
////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLGF2X2CF(const GF2X & poly,const Variable & x)
{
  //printf("convertNTLGF2X2CF\n");
  CanonicalForm bigone;

  if (deg(poly)>0)
  {
    // poly is non-constant
    bigone=0;
    bigone.mapinto();
    // Compute the canonicalform coefficient by coefficient,
    // bigone summarizes the result.
    // In constrast to the more general conversion to ZZpX
    // the only possible coefficients are zero
    // and one yielding the following simplified loop
    for (int j=0;j<=deg(poly);j++)
    {
      if (coeff(poly,j)!=0) bigone+=power(x,j);
     // *CanonicalForm(to_long(rep(coeff(poly,j))))) is not necessary any more;
    }
  }
  else
  {
    // poly is immediate
    bigone=CanonicalForm(to_long(rep(coeff(poly,0))));
    bigone.mapinto();
  }

  return bigone;
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLvec_pair_ZZpX_long2FacCFFList
///
/// DESCRIPTION:
/// Routine for converting a vector of polynomials from ZZpX to
/// a CFFList of Factory. This routine will be used after a successful
/// factorization of NTL to convert the result back to Factory.
///
/// Additionally a variable x and the computed content, as a type ZZp
/// of NTL, is needed as parameters indicating the main variable of the
/// computed canonicalform and the conent of the original polynomial.
/// To guarantee the correct execution of the algorithm the characteristic
/// has a be an arbitrary prime number.
///
/// INPUT:  A vector of polynomials over ZZp of type vec_pair_ZZ_pX_long and
///         a variable x and a content of type ZZp
/// OUTPUT: The converted list of polynomials of type CFFList, all polynomials
///         have x as their main variable
////////////////////////////////////////////////////////////////////////////////

CFFList convertNTLvec_pair_ZZpX_long2FacCFFList
                                  (const vec_pair_ZZ_pX_long & e,const ZZ_p & cont,const Variable & x)
{
  //printf("convertNTLvec_pair_ZZpX_long2FacCFFList\n");
  CFFList result;
  ZZ_pX polynom;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x
  // but this is not
  //important for the factorization, but nevertheless would take computing time,
  // so it is omitted


  // Go through the vector e and compute the CFFList
  // again bigone summarizes the result
  for (int i=e.length()-1;i>=0;i--)
  {
    result.append(CFFactor(convertNTLZZpX2CF(e[i].a,x),e[i].b));
  }
  // the content at pos 1
  result.insert(CFFactor(CanonicalForm(to_long(rep(cont))),1));
  return result;
}
CFFList convertNTLvec_pair_zzpX_long2FacCFFList
                                  (const vec_pair_zz_pX_long & e,const zz_p cont,const Variable & x)
{
  //printf("convertNTLvec_pair_zzpX_long2FacCFFList\n");
  CFFList result;
  zz_pX polynom;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x
  // but this is not
  //important for the factorization, but nevertheless would take computing time,
  // so it is omitted


  // Go through the vector e and compute the CFFList
  // again bigone summarizes the result
  for (int i=e.length()-1;i>=0;i--)
  {
    result.append(CFFactor(convertNTLzzpX2CF(e[i].a,x),e[i].b));
  }
  // the content at pos 1
  result.insert(CFFactor(CanonicalForm(to_long(rep(cont))),1));
  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLvec_pair_GF2X_long2FacCFFList
///
/// DESCRIPTION:
/// Routine for converting a vector of polynomials of type GF2X from
/// NTL to a list CFFList of Factory. This routine will be used after a
/// successful factorization of NTL to convert the result back to Factory.
/// As usual this is simply a special case of the more general conversion
/// routine but again speeded up by leaving out unnecessary steps.
/// Additionally a variable x and the computed content, as type
/// GF2 of NTL, are needed as parameters indicating the main variable of the
/// computed canonicalform and the content of the original polynomial.
/// To guarantee the correct execution of the algorithm the characteristic
/// has a be an arbitrary prime number.
///
/// INPUT:  A vector of polynomials over GF2 of type vec_pair_GF2X_long and
///         a variable x and a content of type GF2
/// OUTPUT: The converted list of polynomials of type CFFList, all
///         polynomials have x as their main variable
////////////////////////////////////////////////////////////////////////////////

CFFList convertNTLvec_pair_GF2X_long2FacCFFList
    (const vec_pair_GF2X_long& e, GF2 /*cont*/, const Variable & x)
{
  //printf("convertNTLvec_pair_GF2X_long2FacCFFList\n");
  CFFList result;
  GF2X polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x
  // but this is not
  //important for the factorization, but nevertheless would take computing time
  // so it is omitted.

  // Go through the vector e and compute the CFFList
  // bigone summarizes the result again
  for (int i=e.length()-1;i>=0;i--)
  {
    bigone=0;

    polynom=e[i].a;
    exponent=e[i].b;
    for (int j=0;j<=deg(polynom);j++)
    {
      if (coeff(polynom,j)!=0)
        bigone += (power(x,j)*CanonicalForm(to_long(rep(coeff(polynom,j)))));
    }

    //append the converted polynomial to the CFFList
    result.append(CFFactor(bigone,exponent));
  }
  result.insert(CFFactor(1,1));
  return result;
}

STATIC_VAR unsigned char *cf_stringtemp;
STATIC_VAR unsigned long cf_stringtemp_l=0L;
////////////////////////////////////////////////////////////////////////////////
/// NAME: convertZZ2CF
///
/// DESCRIPTION:
/// Routine for conversion of integers represented in NTL as Type ZZ to
/// integers in Factory represented as canonicalform.
/// To guarantee the correct execution of the algorithm the characteristic
/// has to equal zero.
///
/// INPUT:  The value coefficient of type ZZ that has to be converted
/// OUTPUT: The converted Factory-integer of type canonicalform
////////////////////////////////////////////////////////////////////////////////
CanonicalForm
convertZZ2CF (const ZZ & a)
{
  long coeff_long=to_long(a);

  CanonicalForm result;
  if ( (NumBits(a)<((long)NTL_ZZ_NBITS))
  && (coeff_long>((long)MINIMMEDIATE))
  && (coeff_long<((long)MAXIMMEDIATE)))
  {
    return CanonicalForm(coeff_long);
  }
  else
  {
    const long * rep =
#if NTL_MAJOR_VERSION <= 6
      static_cast<long *>( a.rep );
#elif NTL_MAJOR_VERSION <=9
      static_cast<long *>( a.rep.rep ); // what about NTL7?
#else
      (long*)( a.rep.rep );
#endif
    long sizeofrep= rep[1];
    bool lessZero= false;
    if (sizeofrep < 0)
    {
      lessZero= true;
      sizeofrep= -sizeofrep;
    }
    if (cf_stringtemp_l == 0)
    {
      cf_stringtemp_l= sizeofrep*sizeof(mp_limb_t)*2;
      cf_stringtemp= (unsigned char*) Alloc (cf_stringtemp_l);
    }
    else if (cf_stringtemp_l < sizeofrep*sizeof(mp_limb_t)*2)
    {
      Free (cf_stringtemp, cf_stringtemp_l);
      cf_stringtemp_l= sizeofrep*sizeof(mp_limb_t)*2;
      cf_stringtemp= (unsigned char*) Alloc (cf_stringtemp_l);
    }
    int cc= mpn_get_str (cf_stringtemp, 16, (mp_limb_t *) ((rep) + 2), sizeofrep);

    char* cf_stringtemp2;
    if (lessZero)
    {
      cf_stringtemp2= new char [cc + 2];
      cf_stringtemp2[0]='-';
      for (int j= 1; j <= cc; j++)
        cf_stringtemp2[j]= IntValToChar ((int) cf_stringtemp [j-1]);
      cf_stringtemp2[cc+1]='\0';
    }
    else
    {
      cf_stringtemp2= new char [cc + 1];
      for (int j= 0; j < cc; j++)
        cf_stringtemp2[j]= IntValToChar ((int) cf_stringtemp [j]);
      cf_stringtemp2[cc]='\0';
    }

    result= CanonicalForm (cf_stringtemp2, 16);
    delete [] cf_stringtemp2;
  }
  return result;
}

/*static char *cf_stringtemp;
static char *cf_stringtemp2;
static int cf_stringtemp_l=0;
CanonicalForm convertZZ2CF(const ZZ & coefficient)
{
  long coeff_long;
  //CanonicalForm tmp=0;
  char dummy[2];
  int minusremainder=0;
  char numbers[]="0123456789abcdef";

  coeff_long=to_long(coefficient);

  //Test whether coefficient can be represented as an immediate integer in Factory
  if ( (NumBits(coefficient)<((long)NTL_ZZ_NBITS))
  && (coeff_long>((long)MINIMMEDIATE))
  && (coeff_long<((long)MAXIMMEDIATE)))
  {
    // coefficient is immediate --> return the coefficient as canonicalform
    return CanonicalForm(coeff_long);
  }
  else
  {
    // coefficient is not immediate (gmp-number)
    if (cf_stringtemp_l==0)
    {
      cf_stringtemp=(char *)Alloc(1023);
      cf_stringtemp2=(char *)Alloc(1023);
      cf_stringtemp[0]='\0';
      cf_stringtemp2[0]='\0';
      cf_stringtemp_l=1023;
    }

    // convert coefficient to char* (input for gmp)
    dummy[1]='\0';

    if (coefficient<0)
    {
      // negate coefficient, but store the sign in minusremainder
      minusremainder=1;
      coefficient=-coefficient;
    }

    int l=0;
    while (coefficient>15)
    {
      ZZ quotient,remaind;
      ZZ ten;ten=16;
      DivRem(quotient,remaind,coefficient,ten);
      dummy[0]=numbers[to_long(remaind)];
      //tmp*=10; tmp+=to_long(remaind);

      l++;
      if (l>=cf_stringtemp_l-2)
      {
        Free(cf_stringtemp2,cf_stringtemp_l);
        char *p=(char *)Alloc(cf_stringtemp_l*2);
        //NTL_SNS
        memcpy(p,cf_stringtemp,cf_stringtemp_l);
        Free(cf_stringtemp,cf_stringtemp_l);
        cf_stringtemp_l*=2;
        cf_stringtemp=p;
        cf_stringtemp2=(char *)Alloc(cf_stringtemp_l);
      }
      cf_stringtemp[l-1]=dummy[0];
      cf_stringtemp[l]='\0';
      //strcat(stringtemp,dummy);

      coefficient=quotient;
    }
    //built up the string in dummy[0]
    dummy[0]=numbers[to_long(coefficient)];
    //NTL_SNS
    l++;
    cf_stringtemp[l-1]=dummy[0];
    cf_stringtemp[l]='\0';
    //tmp*=10; tmp+=to_long(coefficient);

    if (minusremainder==1)
    {
      //Check whether coefficient has been negative at the start of the procedure
      cf_stringtemp2[0]='-';
      //tmp*=(-1);
    }

    //reverse the list to obtain the correct string
    //NTL_SNS
    for (int i=l-1;i>=0;i--) // l ist the position of \0
    {
      cf_stringtemp2[l-i-1+minusremainder]=cf_stringtemp[i];
    }
    cf_stringtemp2[l+minusremainder]='\0';
  }

  //convert the string to canonicalform using the char*-Constructor
  return CanonicalForm(cf_stringtemp2,16);
  //return tmp;
}*/

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertFacCF2NTLZZX
///
/// DESCRIPTION:
/// Routine for conversion of canonicalforms in Factory to polynomials
/// of type ZZX of NTL. To guarantee the correct execution of the
/// algorithm the characteristic has to equal zero.
///
/// INPUT:  The canonicalform that has to be converted
/// OUTPUT: The converted NTL-polynom of type ZZX
////////////////////////////////////////////////////////////////////////////////

ZZ convertFacCF2NTLZZ(const CanonicalForm & f)
{
  ZZ temp;
  if (f.isImm()) temp=f.intval();
  else
  {
    //Coefficient is a gmp-number
    mpz_t gmp_val;
    char* stringtemp;

    f.mpzval (gmp_val);
    int l=mpz_sizeinbase(gmp_val,10)+2;
    stringtemp=(char*)Alloc(l);
    stringtemp=mpz_get_str(stringtemp,10,gmp_val);
    mpz_clear(gmp_val);
    conv(temp,stringtemp);
    Free(stringtemp,l);
  }
  return temp;
}

ZZX convertFacCF2NTLZZX(const CanonicalForm & f)
{
    ZZX ntl_poly;

    CFIterator i;
    i=f;

    int NTLcurrentExp=i.exp();
    int largestExp=i.exp();
    int k;

    //set the length of the NTL-polynomial
    ntl_poly.SetMaxLength(largestExp+1);

    //Go through the coefficients of the canonicalform and build up the NTL-polynomial
    for (;i.hasTerms();i++)
    {
      for (k=NTLcurrentExp;k>i.exp();k--)
      {
        SetCoeff(ntl_poly,k,0);
      }
      NTLcurrentExp=i.exp();

      //Coefficient is a gmp-number
      ZZ temp=convertFacCF2NTLZZ(i.coeff());

      //set the computed coefficient
      SetCoeff(ntl_poly,NTLcurrentExp,temp);

      NTLcurrentExp--;
    }
    for (k=NTLcurrentExp;k>=0;k--)
    {
      SetCoeff(ntl_poly,k,0);
    }

    //normalize the polynomial
    ntl_poly.normalize();

    return ntl_poly;
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLvec_pair_ZZX_long2FacCFFList
///
/// DESCRIPTION:
/// Routine for converting a vector of polynomials from ZZ to a list
/// CFFList of Factory. This routine will be used after a successful
/// factorization of NTL to convert the result back to Factory.
/// Additionally a variable x and the computed content, as a type
/// ZZ of NTL, is needed as parameters indicating the main variable of the
/// computed canonicalform and the content of the original polynomial.
/// To guarantee the correct execution of the algorithm the characteristic
/// has to equal zero.
///
/// INPUT:  A vector of polynomials over ZZ of type vec_pair_ZZX_long and
///         a variable x and a content of type ZZ
/// OUTPUT: The converted list of polynomials of type CFFList, all
///         have x as their main variable
////////////////////////////////////////////////////////////////////////////////

CFFList
convertNTLvec_pair_ZZX_long2FacCFFList (const vec_pair_ZZX_long & e,const ZZ & cont,const Variable & x)
{
  CFFList result;
  ZZX polynom;
  long exponent;
  CanonicalForm bigone;

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result
  for (int i=e.length()-1;i>=0;i--)
  {
    ZZ coefficient;
    polynom=e[i].a;
    exponent=e[i].b;
    bigone=convertNTLZZX2CF(polynom,x);
    //append the converted polynomial to the list
    result.append(CFFactor(bigone,exponent));
  }
  // the content at pos 1
  result.insert(CFFactor(convertZZ2CF(cont),1));

  //return the converted list
  return result;
}


////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLZZpX2CF
///
/// DESCRIPTION:
/// Routine for conversion of elements of arbitrary extensions of ZZp,
/// having type ZZpE, of NTL to their corresponding values of type
/// canonicalform in Factory.
/// To guarantee the correct execution of the algorithm the characteristic
/// has to be an arbitrary prime number and Factory has to compute in an
/// extension of F_p.
///
/// INPUT:  The coefficient of type ZZpE and the variable x indicating the main//
///         variable of the computed canonicalform
/// OUTPUT: The converted value of coefficient as type canonicalform
////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLZZpE2CF(const ZZ_pE & coefficient,const Variable & x)
{
  return convertNTLZZpX2CF(rep(coefficient),x);
}
CanonicalForm convertNTLzzpE2CF(const zz_pE & coefficient,const Variable & x)
{
  return convertNTLzzpX2CF(rep(coefficient),x);
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLvec_pair_ZZpEX_long2FacCFFList
///
/// DESCRIPTION:
/// Routine for converting a vector of polynomials from ZZpEX to a CFFList
/// of Factory. This routine will be used after a successful factorization
/// of NTL to convert the result back to Factory.
/// Additionally a variable x and the computed content, as a type
/// ZZpE of NTL, is needed as parameters indicating the main variable of the
/// computed canonicalform and the content of the original polynomial.
/// To guarantee the correct execution of the algorithm the characteristic
/// has a be an arbitrary prime number p and computations have to be done
/// in an extention of F_p.
///
/// INPUT:  A vector of polynomials over ZZpE of type vec_pair_ZZ_pEX_long and
///         a variable x and a content of type ZZpE
/// OUTPUT: The converted list of polynomials of type CFFList, all polynomials
///         have x as their main variable
////////////////////////////////////////////////////////////////////////////////

CFFList
convertNTLvec_pair_ZZpEX_long2FacCFFList(const vec_pair_ZZ_pEX_long & e,const ZZ_pE & cont,const Variable & x,const Variable & alpha)
{
  CFFList result;
  ZZ_pEX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result during every loop
  for (int i=e.length()-1;i>=0;i--)
  {
    bigone=0;

    polynom=e[i].a;
    exponent=e[i].b;

    for (int j=0;j<=deg(polynom);j++)
    {
      if (IsOne(coeff(polynom,j)))
      {
        bigone+=power(x,j);
      }
      else
      {
        CanonicalForm coefficient=convertNTLZZpE2CF(coeff(polynom,j),alpha);
        if (coeff(polynom,j)!=0)
        {
          bigone += (power(x,j)*coefficient);
        }
      }
    }
    //append the computed polynomials together with its exponent to the CFFList
    result.append(CFFactor(bigone,exponent));
  }
  // Start by insert the content
  result.insert(CFFactor(convertNTLZZpE2CF(cont,alpha),1));

  //return the computed CFFList
  return result;
}
CFFList
convertNTLvec_pair_zzpEX_long2FacCFFList(const vec_pair_zz_pEX_long & e,const zz_pE & cont,const Variable & x,const Variable & alpha)
{
  CFFList result;
  zz_pEX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result during every loop
  for (int i=e.length()-1;i>=0;i--)
  {
    bigone=0;

    polynom=e[i].a;
    exponent=e[i].b;

    for (int j=0;j<=deg(polynom);j++)
    {
      if (IsOne(coeff(polynom,j)))
      {
        bigone+=power(x,j);
      }
      else
      {
        CanonicalForm coefficient=convertNTLzzpE2CF(coeff(polynom,j),alpha);
        if (coeff(polynom,j)!=0)
        {
          bigone += (power(x,j)*coefficient);
        }
      }
    }
    //append the computed polynomials together with its exponent to the CFFList
    result.append(CFFactor(bigone,exponent));
  }
  // Start by appending the constant factor
  result.insert(CFFactor(convertNTLzzpE2CF(cont,alpha),1));

  //return the computed CFFList
  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLGF2E2CF
///
/// DESCRIPTION:
/// Routine for conversion of elements of extensions of GF2, having type
/// GF2E, of NTL to their corresponding values of type canonicalform in
/// Factory.
/// To guarantee the correct execution of the algorithm, the characteristic
/// must equal two and Factory has to compute in an extension of F_2.
/// As usual this is an optimized special case of the more general conversion
/// routine from ZZpE to Factory.
///
/// INPUT:  The coefficient of type GF2E and the variable x indicating the
///         main variable of the computed canonicalform
/// OUTPUT: The converted value of coefficient as type canonicalform
////////////////////////////////////////////////////////////////////////////////

CanonicalForm convertNTLGF2E2CF(const GF2E & coefficient,const Variable & x)
{
  return convertNTLGF2X2CF(rep(coefficient),x);
}

////////////////////////////////////////////////////////////////////////////////
/// NAME: convertNTLvec_pair_GF2EX_long2FacCFFList
///
/// DESCRIPTION:
/// Routine for converting a vector of polynomials from GF2EX to a CFFList
/// of Factory. This routine will be used after a successful factorization
/// of NTL to convert the result back to Factory.
/// This is a special, but optimized case of the more general conversion
/// from ZZpE to canonicalform.
/// Additionally a variable x and the computed content, as a type GF2E
/// of NTL, is needed as parameters indicating the main variable of the
/// computed canonicalform and the content of the original polynomial.
/// To guarantee the correct execution of the algorithm the characteristic
/// has to equal two and computations have to be done in an extention of F_2.
///
/// INPUT:  A vector of polynomials over GF2E of type vec_pair_GF2EX_long and
///         a variable x and a content of type GF2E
/// OUTPUT: The converted list of polynomials of type CFFList, all polynomials
///         have x as their main variable
////////////////////////////////////////////////////////////////////////////////

CFFList convertNTLvec_pair_GF2EX_long2FacCFFList
    (const vec_pair_GF2EX_long & e, const GF2E & cont, const Variable & x, const Variable & alpha)
{
  CFFList result;
  GF2EX polynom;
  long exponent;
  CanonicalForm bigone;

  // Maybe, e may additionally be sorted with respect to increasing degree of x, but this is not
  //important for the factorization, but nevertheless would take computing time, so it is omitted

  // Go through the vector e and build up the CFFList
  // As usual bigone summarizes the result during every loop
  for (int i=e.length()-1;i>=0;i--)
  {
    bigone=0;

    polynom=e[i].a;
    exponent=e[i].b;

    for (int j=0;j<=deg(polynom);j++)
    {
      if (IsOne(coeff(polynom,j)))
      {
        bigone+=power(x,j);
      }
      else
      {
        CanonicalForm coefficient=convertNTLGF2E2CF(coeff(polynom,j),alpha);
        if (coeff(polynom,j)!=0)
        {
          bigone += (power(x,j)*coefficient);
        }
      }
    }
    // append the computed polynomial together with its multiplicity
    result.append(CFFactor(bigone,exponent));
  }

  result.insert(CFFactor(convertNTLGF2E2CF(cont,alpha),1));

  // return the computed CFFList
  return result;
}

////////////////////////////////////////////////////
/// CanonicalForm in Z_2(a)[X] to NTL GF2EX
////////////////////////////////////////////////////
GF2EX convertFacCF2NTLGF2EX(const CanonicalForm & f,const GF2X & mipo)
{
  GF2E::init(mipo);
  GF2EX result;
  CFIterator i;
  i=f;

  int NTLcurrentExp=i.exp();
  int largestExp=i.exp();
  int k;

  result.SetMaxLength(largestExp+1);
  for(;i.hasTerms();i++)
  {
    for(k=NTLcurrentExp;k>i.exp();k--) SetCoeff(result,k,0);
    NTLcurrentExp=i.exp();
    CanonicalForm c=i.coeff();
    GF2X cc=convertFacCF2NTLGF2X(c);
    //ZZ_pE ccc;
    //conv(ccc,cc);
    SetCoeff(result,NTLcurrentExp,to_GF2E(cc));
    NTLcurrentExp--;
  }
  for(k=NTLcurrentExp;k>=0;k--) SetCoeff(result,k,0);
  result.normalize();
  return result;
}
////////////////////////////////////////////////////
/// CanonicalForm in Z_p(a)[X] to NTL ZZ_pEX
////////////////////////////////////////////////////
ZZ_pEX convertFacCF2NTLZZ_pEX(const CanonicalForm & f, const ZZ_pX & mipo)
{
  ZZ_pE::init(mipo);
  ZZ_pEX result;
  CFIterator i;
  i=f;

  int NTLcurrentExp=i.exp();
  int largestExp=i.exp();
  int k;

  result.SetMaxLength(largestExp+1);
  for(;i.hasTerms();i++)
  {
    for(k=NTLcurrentExp;k>i.exp();k--) SetCoeff(result,k,0);
    NTLcurrentExp=i.exp();
    CanonicalForm c=i.coeff();
    ZZ_pX cc=convertFacCF2NTLZZpX(c);
    //ZZ_pE ccc;
    //conv(ccc,cc);
    SetCoeff(result,NTLcurrentExp,to_ZZ_pE(cc));
    NTLcurrentExp--;
  }
  for(k=NTLcurrentExp;k>=0;k--) SetCoeff(result,k,0);
  result.normalize();
  return result;
}
zz_pEX convertFacCF2NTLzz_pEX(const CanonicalForm & f, const zz_pX & mipo)
{
  zz_pE::init(mipo);
  zz_pEX result;
  CFIterator i;
  i=f;

  int NTLcurrentExp=i.exp();
  int largestExp=i.exp();
  int k;

  result.SetMaxLength(largestExp+1);
  for(;i.hasTerms();i++)
  {
    for(k=NTLcurrentExp;k>i.exp();k--) SetCoeff(result,k,0);
    NTLcurrentExp=i.exp();
    CanonicalForm c=i.coeff();
    zz_pX cc=convertFacCF2NTLzzpX(c);
    //ZZ_pE ccc;
    //conv(ccc,cc);
    SetCoeff(result,NTLcurrentExp,to_zz_pE(cc));
    NTLcurrentExp--;
  }
  for(k=NTLcurrentExp;k>=0;k--) SetCoeff(result,k,0);
  result.normalize();
  return result;
}

CanonicalForm convertNTLzz_pEX2CF (const zz_pEX& f, const Variable & x, const Variable & alpha)
{
  CanonicalForm bigone;
  if (deg (f) > 0)
  {
    bigone= 0;
    bigone.mapinto();
    for (int j=0;j<deg(f)+1;j++)
    {
      if (coeff(f,j)!=0)
      {
        bigone+=(power(x,j)*convertNTLzzpE2CF(coeff(f,j),alpha));
      }
    }
  }
  else
  {
    bigone= convertNTLzzpE2CF(coeff(f,0),alpha);
    bigone.mapinto();
  }
  return bigone;
}

CanonicalForm convertNTLZZ_pEX2CF (const ZZ_pEX& f, const Variable & x, const Variable & alpha)
{
  CanonicalForm bigone;
  if (deg (f) > 0)
  {
    bigone= 0;
    bigone.mapinto();
    for (int j=0;j<deg(f)+1;j++)
    {
      if (coeff(f,j)!=0)
      {
        bigone+=(power(x,j)*convertNTLZZpE2CF(coeff(f,j),alpha));
      }
    }
  }
  else
  {
    bigone= convertNTLZZpE2CF(coeff(f,0),alpha);
    bigone.mapinto();
  }
  return bigone;
}
//----------------------------------------------------------------------
mat_ZZ* convertFacCFMatrix2NTLmat_ZZ(const CFMatrix &m)
{
  mat_ZZ *res=new mat_ZZ;
  res->SetDims(m.rows(),m.columns());

  int i,j;
  for(i=m.rows();i>0;i--)
  {
    for(j=m.columns();j>0;j--)
    {
      (*res)(i,j)=convertFacCF2NTLZZ(m(i,j));
    }
  }
  return res;
}
CFMatrix* convertNTLmat_ZZ2FacCFMatrix(const mat_ZZ &m)
{
  CFMatrix *res=new CFMatrix(m.NumRows(),m.NumCols());
  int i,j;
  for(i=res->rows();i>0;i--)
  {
    for(j=res->columns();j>0;j--)
    {
      (*res)(i,j)=convertZZ2CF(m(i,j));
    }
  }
  return res;
}

mat_zz_p* convertFacCFMatrix2NTLmat_zz_p(const CFMatrix &m)
{
  mat_zz_p *res=new mat_zz_p;
  res->SetDims(m.rows(),m.columns());

  int i,j;
  for(i=m.rows();i>0;i--)
  {
    for(j=m.columns();j>0;j--)
    {
      if(!(m(i,j)).isImm()) printf("convertFacCFMatrix2NTLmat_zz_p: not imm.\n");
      (*res)(i,j)=(m(i,j)).intval();
    }
  }
  return res;
}
CFMatrix* convertNTLmat_zz_p2FacCFMatrix(const mat_zz_p &m)
{
  CFMatrix *res=new CFMatrix(m.NumRows(),m.NumCols());
  int i,j;
  for(i=res->rows();i>0;i--)
  {
    for(j=res->columns();j>0;j--)
    {
      (*res)(i,j)=CanonicalForm(to_long(rep(m(i,j))));
    }
  }
  return res;
}
mat_zz_pE* convertFacCFMatrix2NTLmat_zz_pE(const CFMatrix &m)
{
  mat_zz_pE *res=new mat_zz_pE;
  res->SetDims(m.rows(),m.columns());

  int i,j;
  for(i=m.rows();i>0;i--)
  {
    for(j=m.columns();j>0;j--)
    {
      zz_pX cc=convertFacCF2NTLzzpX(m(i,j));
      (*res)(i,j)=to_zz_pE(cc);
    }
  }
  return res;
}
CFMatrix* convertNTLmat_zz_pE2FacCFMatrix(const mat_zz_pE &m, const Variable & alpha)
{
  CFMatrix *res=new CFMatrix(m.NumRows(),m.NumCols());
  int i,j;
  for(i=res->rows();i>0;i--)
  {
    for(j=res->columns();j>0;j--)
    {
      (*res)(i,j)=convertNTLzzpE2CF(m(i,j), alpha);
    }
  }
  return res;
}
#endif
