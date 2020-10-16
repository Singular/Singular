/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "int_rat.h"
#include "int_int.h"
#include "imm.h"
#include "canonicalform.h"
#include "cf_factory.h"
#include "gmpext.h"

static long intgcd( long a, long b )
{
    if ( a < 0 ) a = -a;
    if ( b < 0 ) b = -b;

    long c;

    while ( b != 0 )
    {
        c = a % b;
        a = b;
        b = c;
    }
    return a;
}


InternalRational::InternalRational()
{
    mpz_init( _num );
    mpz_init_set_si( _den, 1 );
}

InternalRational::InternalRational( const int i )
{
    mpz_init_set_si( _num, i );
    mpz_init_set_si( _den, 1 );
}

InternalRational::InternalRational( const int n, const int d )
{
    ASSERT( d != 0, "divide by zero" );
    if ( n == 0 )
    {
        mpz_init_set_si( _num, 0 );
        mpz_init_set_si( _den, 1 );
    }
    else
    {
        long g = intgcd( (long) n, (long) d );
        if ( d < 0 )
        {
          mpz_init_set_si( _num, -((long)n) / g );
          mpz_init_set_si( _den, -((long)d) / g );
        }
        else
        {
          mpz_init_set_si( _num, n / g );
          mpz_init_set_si( _den, d / g );
        }
    }
}

InternalRational::InternalRational( const long i )
{
    mpz_init_set_si( _num, i );
    mpz_init_set_si( _den, 1 );
}

InternalRational::InternalRational( const long n, const long d )
{
    ASSERT( d != 0, "divide by zero" );
    if ( n == 0 )
    {
        mpz_init_set_si( _num, 0 );
        mpz_init_set_si( _den, 1 );
    }
    else
    {
        long g = intgcd( n, d );
        if ( d < 0 )
        {
          mpz_init_set_si( _num, -n / g );
          mpz_init_set_si( _den, -d / g );
        }
        else
        {
          mpz_init_set_si( _num, n / g );
          mpz_init_set_si( _den, d / g );
        }
    }
}

InternalRational::InternalRational( const char * )
{
    // sollte nicht gebraucht werden !!!
    ASSERT( 0, "fatal error" );
    mpz_init( _num );
    mpz_init( _den );
}

//InternalRational::InternalRational( const mpz_ptr n ) : _num(n)
//{
//    mpz_init_set_si( _den, 1 );
//}

InternalRational::InternalRational( const mpz_ptr n )
{
    _num[0]=*n;
    mpz_init_set_si( _den, 1 );
}

InternalRational::InternalRational( const mpz_ptr n, const mpz_ptr d )
{
  _num[0]=*n;
  _den[0]=*d;
}

InternalRational::~InternalRational()
{
    mpz_clear( _num );
    mpz_clear( _den );
}

InternalCF* InternalRational::deepCopyObject() const
{
    mpz_t dummy_num;
    mpz_t dummy_den;
    mpz_init_set( dummy_num, _num );
    mpz_init_set( dummy_den, _den );
    return new InternalRational( dummy_num, dummy_den );
}

#ifndef NOSTREAMIO
void InternalRational::print( OSTREAM & os, char * c )
{
    char * str = new char[mpz_sizeinbase( _num, 10 ) + 2];
    str = mpz_get_str( str, 10, _num );
    os << str << '/';
    delete [] str;
    str = new char[mpz_sizeinbase( _den, 10 ) + 2];
    str = mpz_get_str( str, 10, _den );
    os << str << c;
    delete [] str;
}
#endif /* NOSTREAMIO */

bool InternalRational::is_imm() const
{
    return mpz_cmp_si( _den, 1 ) == 0 && mpz_is_imm( _num );
}

InternalCF* InternalRational::genZero()
{
    if ( isZero() )
        return copyObject();
    else
        return new InternalRational();
}

InternalCF* InternalRational::genOne()
{
    if ( isOne() )
        return copyObject();
    else
        return new InternalRational( 1 );
}

/**
 * @sa CanonicalForm::num(), CanonicalForm::den(), InternalRational::den()
**/
InternalCF * InternalRational::num ()
{
    if ( mpz_is_imm( _num ) )
    {
        InternalCF * res = int2imm( mpz_get_si( _num ) );
        return res;
    }
    else
    {
        mpz_t dummy;
        mpz_init_set( dummy, _num );
        return new InternalInteger( dummy );
    }
}

/**
 * @sa CanonicalForm::num(), CanonicalForm::den(), InternalRational::num()
**/
InternalCF * InternalRational::den ()
{
    if ( mpz_is_imm( _den ) )
    {
        InternalCF * res = int2imm( mpz_get_si( _den ) );
        return res;
    }
    else
    {
        mpz_t dummy;
        mpz_init_set( dummy, _den );
        return new InternalInteger( dummy );
    }
}

/** InternalCF * InternalRational::neg ()
 * @sa CanonicalForm::operator -()
**/
InternalCF *
InternalRational::neg ()
{
    if ( getRefCount() > 1 )
    {
        decRefCount();
        mpz_t dummy_num;
        mpz_t dummy_den;
        mpz_init_set( dummy_num, _num );
        mpz_init_set( dummy_den, _den );
        mpz_neg( dummy_num, dummy_num );
        return new InternalRational( dummy_num, dummy_den );
    }
    else
    {
        mpz_neg( _num, _num );
        return this;
    }
}

InternalCF* InternalRational::addsame( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == RationalDomain, "illegal domain" );
    mpz_t n, d, g;

    mpz_init( g ); mpz_init( n ); mpz_init( d );
    mpz_gcd( g, _den, MPQDEN( c ) );

    if ( mpz_cmp_si( g, 1 ) == 0 )
    {
      mpz_mul( n, _den, MPQNUM( c ) );
      mpz_mul( g, _num, MPQDEN( c ) );
      mpz_add( n, n, g );
      mpz_mul( d, _den, MPQDEN( c ) );
    }
    else
    {
      mpz_t tmp1;
      mpz_t tmp2;
      mpz_init( tmp1 );
      mpz_divexact( tmp1, _den, g );
      mpz_init( tmp2 );
      mpz_divexact( tmp2, MPQDEN( c ), g );
      mpz_mul( d, tmp2, _den );
      mpz_mul( tmp2, tmp2, _num );
      mpz_mul( tmp1, tmp1, MPQNUM( c ) );
      mpz_add( n, tmp1, tmp2 );
      mpz_gcd( g, n, d );
      if ( mpz_cmp_si( g, 1 ) != 0 )
      {
          mpz_divexact( n, n, g );
          mpz_divexact( d, d, g );
      }
      mpz_clear( tmp1 );
      mpz_clear( tmp2 );
    }
    mpz_clear( g );
    if ( deleteObject() ) delete this;
    if ( mpz_cmp_si( d, 1 ) == 0 )
    {
      mpz_clear( d );
      if ( mpz_is_imm( n ) )
      {
          InternalCF * res = int2imm( mpz_get_si( n ) );
          mpz_clear( n );
          return res;
      }
      else
      {
          return new InternalInteger( n );
      }
    }
    else
    {
      return new InternalRational( n, d );
    }
}

InternalCF* InternalRational::subsame( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == RationalDomain, "illegal domain" );
    mpz_t n, d, g;

    mpz_init( g ); mpz_init( n ); mpz_init( d );
    mpz_gcd( g, _den, MPQDEN( c ) );

    if ( mpz_cmp_si( g, 1 ) == 0 )
    {
        mpz_mul( n, _den, MPQNUM( c ) );
        mpz_mul( g, _num, MPQDEN( c ) );
        mpz_sub( n, g, n );
        mpz_mul( d, _den, MPQDEN( c ) );
    }
    else
    {
        mpz_t tmp1;
        mpz_t tmp2;
        mpz_init( tmp1 );
        mpz_divexact( tmp1, _den, g );
        mpz_init( tmp2 );
        mpz_divexact( tmp2, MPQDEN( c ), g );
        mpz_mul( d, tmp2, _den );
        mpz_mul( tmp2, tmp2, _num );
        mpz_mul( tmp1, tmp1, MPQNUM( c ) );
        mpz_sub( n, tmp2, tmp1 );
        mpz_gcd( g, n, d );
        if ( mpz_cmp_si( g, 1 ) != 0 )
        {
            mpz_divexact( n, n, g );
            mpz_divexact( d, d, g );
        }
        mpz_clear( tmp1 );
        mpz_clear( tmp2 );
    }
    mpz_clear( g );
    if ( deleteObject() ) delete this;
    if ( mpz_cmp_si( d, 1 ) == 0 )
    {
        mpz_clear( d );
        if ( mpz_is_imm( n ) )
        {
            InternalCF * res = int2imm( mpz_get_si( n ) );
            mpz_clear( n );
            return res;
        }
        else
        {
            return new InternalInteger( n );
        }
    }
    else
        return new InternalRational( n, d );
}

InternalCF* InternalRational::mulsame( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == RationalDomain, "illegal domain" );
    mpz_t n, d;
    mpz_init( n ); mpz_init( d );

    if ( this == c )
    {
        mpz_mul( n, _num, _num );
        mpz_mul( d, _den, _den );
    }
    else
    {
        mpz_t g1, g2, tmp1, tmp2;
        mpz_init( g1 ); mpz_init( g2 );
        mpz_gcd( g1, _num, MPQDEN( c ) );
        mpz_gcd( g2, _den, MPQNUM( c ) );
        bool g1is1 = mpz_cmp_si( g1, 1 ) == 0;
        bool g2is1 = mpz_cmp_si( g2, 1 ) == 0;
        mpz_init( tmp1 ); mpz_init( tmp2 );
        if ( ! g1is1 )
            mpz_divexact( tmp1, _num, g1 );
        else
            mpz_set( tmp1, _num );
        if ( ! g2is1 )
            mpz_divexact( tmp2, MPQNUM( c ), g2 );
        else
            mpz_set( tmp2, MPQNUM( c ) );
        mpz_mul( n, tmp1, tmp2 );
        if ( ! g1is1 )
            mpz_divexact( tmp1, MPQDEN( c ), g1 );
        else
            mpz_set( tmp1, MPQDEN( c ) );
        if ( ! g2is1 )
            mpz_divexact( tmp2, _den, g2 );
        else
            mpz_set( tmp2, _den );
        mpz_mul( d, tmp1, tmp2 );
        mpz_clear( tmp1 ); mpz_clear( tmp2 );
        mpz_clear( g1 ); mpz_clear( g2 );
    }
    if ( deleteObject() ) delete this;
    if ( mpz_cmp_si( d, 1 ) == 0 )
    {
        mpz_clear( d );
        if ( mpz_is_imm( n ) )
        {
            InternalCF * res = int2imm( mpz_get_si( n ) );
            mpz_clear( n );
            return res;
        }
        else
        {
            return new InternalInteger( n );
        }
    }
    else
        return new InternalRational( n, d );
}

InternalCF* InternalRational::dividesame( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == RationalDomain, "illegal domain" );

    if ( this == c )
    {
        if ( deleteObject() ) delete this;
        return CFFactory::basic( 1 );
    }
    else
    {
        mpz_t n, d;
        mpz_t g1, g2, tmp1, tmp2;
        mpz_init( n ); mpz_init( d );
        mpz_init( g1 ); mpz_init( g2 );
        mpz_gcd( g1, _num, MPQNUM( c ) );
        mpz_gcd( g2, _den, MPQDEN( c ) );
        bool g1is1 = mpz_cmp_si( g1, 1 ) == 0;
        bool g2is1 = mpz_cmp_si( g2, 1 ) == 0;
        mpz_init( tmp1 ); mpz_init( tmp2 );
        if ( ! g1is1 )
            mpz_divexact( tmp1, _num, g1 );
        else
            mpz_set( tmp1, _num );
        if ( ! g2is1 )
            mpz_divexact( tmp2, MPQDEN( c ), g2 );
        else
            mpz_set( tmp2, MPQDEN( c ) );
        mpz_mul( n, tmp1, tmp2 );
        if ( ! g1is1 )
            mpz_divexact( tmp1, MPQNUM( c ), g1 );
        else
            mpz_set( tmp1, MPQNUM( c ) );
        if ( ! g2is1 )
            mpz_divexact( tmp2, _den, g2 );
        else
            mpz_set( tmp2, _den );
        mpz_mul( d, tmp1, tmp2 );
        mpz_clear( tmp1 ); mpz_clear( tmp2 );
        mpz_clear( g1 ); mpz_clear( g2 );
        if ( deleteObject() ) delete this;
        if ( mpz_cmp_si( d, 0 ) < 0 )
        {
            mpz_neg( d, d );
            mpz_neg( n, n );
        }
        if ( mpz_cmp_si( d, 1 ) == 0 )
        {
            mpz_clear( d );
            if ( mpz_is_imm( n ) )
            {
                InternalCF * res = int2imm( mpz_get_si( n ) );
                mpz_clear( n );
                return res;
            }
            else
            {
                return new InternalInteger( n );
            }
        }
        else
            return new InternalRational( n, d );
    }
}

InternalCF* InternalRational::divsame( InternalCF * c )
{
    return dividesame( c );
}

InternalCF* InternalRational::modulosame( InternalCF * c )
{
    return modsame( c );
}

InternalCF* InternalRational::modsame( InternalCF * )
{
    if ( deleteObject() ) delete this;
    return CFFactory::basic( 0 );
}

void InternalRational::divremsame( InternalCF * c, InternalCF*& quot, InternalCF*& rem )
{
    quot = copyObject();
    quot = quot->dividesame( c );
    rem = CFFactory::basic( 0 );
}

bool InternalRational::divremsamet( InternalCF* c, InternalCF*& quot, InternalCF*& rem )
{
    divremsame( c, quot, rem );
    return true;
}

/**
 * comparesame(), comparecoeff() - compare with an
 *   InternalRational.
 *
 * comparesame() compares the CO=a/b and c=p/q using the
 * equivalence a/b < p/q iff a*q < p*b.
 *
 * Note: May be relatively expensive due to the
 * multiplications.
 *
 * See also: CanonicalForm::operator <(), CanonicalForm::operator ==()
 *
**/
int
InternalRational::comparesame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == RationalDomain, "incompatible base coefficients" );
    mpz_t dummy1, dummy2;
    mpz_init( dummy1 ); mpz_init( dummy2 );
    mpz_mul( dummy1, _num, MPQDEN( c ) );
    mpz_mul( dummy2, _den, MPQNUM( c ) );
    int result = mpz_cmp( dummy1, dummy2 );
    mpz_clear( dummy1 ); mpz_clear( dummy2 );
    return result;
}

/**
 * comparecoeff() compares the CO=a/b and the integer c using the
 * equivalence a/b < c iff a < c*b.
 *
 * Note: May be relatively expensive due to the
 * multiplications.
**/
int
InternalRational::comparecoeff ( InternalCF* c )
{
    if ( ::is_imm( c ) )
    {
        ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
        mpz_t dummy;
        mpz_init_set_si( dummy, imm2int( c ) );
        mpz_mul( dummy, dummy, _den );
        int result = mpz_cmp( _num, dummy );
        mpz_clear( dummy );
        return result;
    }
    else
    {
        ASSERT( c->levelcoeff() == IntegerDomain, "incompatible base coefficients" );
        mpz_t dummy;
        mpz_init( dummy );
        mpz_mul( dummy, _den, InternalInteger::MPI( c ) );
        int result = mpz_cmp( _num, dummy );
        mpz_clear( dummy );
        return result;
    }
}

InternalCF* InternalRational::addcoeff( InternalCF* c )
{
    ASSERT( ::is_imm( c ) == INTMARK || ! ::is_imm( c ), "expected integer" );
    mpz_t n, d;
    if ( ::is_imm( c ) )
    {
        long cc = imm2int( c );
        if ( cc == 0 )
            return this;
        else
	{
          mpz_init( n );
	  if ( cc < 0 )
          {
            mpz_mul_ui( n, _den, -cc );
            mpz_sub( n, _num, n );
          }
          else
          {
            mpz_mul_ui( n, _den, cc );
            mpz_add( n, _num, n );
          }
	}
    }
    else
    {
        ASSERT( c->levelcoeff() == IntegerDomain, "expected integer" );
        mpz_init( n );
        mpz_mul( n, _den, InternalInteger::MPI( c ) );
        mpz_add( n, _num, n );
    }
    mpz_init_set( d, _den );
    // at this point there is no way that the result is not a true rational
    if ( deleteObject() ) delete this;
    return new InternalRational( n, d );
}

InternalCF* InternalRational::subcoeff( InternalCF* c, bool negate )
{
    ASSERT( ::is_imm( c ) == INTMARK || ! ::is_imm( c ), "expected integer" );
    mpz_t n, d;
    if ( ::is_imm( c ) )
    {
        long cc = imm2int( c );
        if ( cc == 0 )
        {
            if ( negate )
            {
                if ( getRefCount() == 1 )
                {
                    mpz_neg( _num, _num );
                    return this;
                }
                else
                {
                    decRefCount();
                    mpz_init_set( d, _den );
                    mpz_init_set( n, _num );
                    mpz_neg( n, n );
                    return new InternalRational( n, d );
                }
            }
            else
                return this;
        }
        mpz_init( n );
        if ( cc < 0 )
        {
            mpz_mul_ui( n, _den, -cc );
            mpz_neg( n, n );
        }
        else
            mpz_mul_ui( n, _den, cc );
        if ( negate )
            mpz_sub( n, n, _num );
        else
            mpz_sub( n, _num, n );
    }
    else
    {
        ASSERT( c->levelcoeff() == IntegerDomain, "expected integer" );
        mpz_init( n );
        mpz_mul( n, _den, InternalInteger::MPI( c ) );
        if ( negate )
            mpz_sub( n, n, _num );
        else
            mpz_sub( n, _num, n );
    }
    mpz_init_set( d, _den );
    // at this point there is no way that the result is not a true rational
    if ( deleteObject() ) delete this;
    return new InternalRational( n, d );
}

InternalCF* InternalRational::mulcoeff( InternalCF* c )
{
    ASSERT( ::is_imm( c ) == INTMARK || ! ::is_imm( c ), "expected integer" );
    mpz_t n, d, g;
    if ( ::is_imm( c ) )
    {
        long cc = imm2int( c );
        if ( cc == 0 )
        {
            if ( deleteObject() ) delete this;
            return CFFactory::basic( 0 );
        }
        mpz_init_set_si( n, cc );
    }
    else
    {
        ASSERT( c->levelcoeff() == IntegerDomain, "expected integer" );
        mpz_init_set( n, InternalInteger::MPI( c ) );
    }
    mpz_init( g );
    mpz_gcd( g, n, _den );
    if ( mpz_cmp_si( g, 1 ) == 0 )
    {
        mpz_mul( n, n, _num );
        mpz_init_set( d, _den );
    }
    else
    {
        mpz_divexact( n, n, g );
        mpz_mul( n, n, _num );
        mpz_init( d );
        mpz_divexact( d, _den, g );
    }
    mpz_clear( g );
    if ( deleteObject() ) delete this;
    if ( mpz_cmp_si( d, 1 ) == 0 )
    {
        mpz_clear( d );
        if ( mpz_is_imm( n ) )
        {
            InternalCF * res = int2imm( mpz_get_si( n ) );
            mpz_clear( n );
            return res;
        }
        else
        {
            return new InternalInteger( n );
        }
    }
    else
        return new InternalRational( n, d );
}

InternalCF* InternalRational::dividecoeff( InternalCF* c, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK || ! ::is_imm( c ), "expected integer" );
    mpz_t n, d, g;
    if ( ::is_imm( c ) )
    {
        long cc = imm2int( c );
        ASSERT( c != 0 || invert, "divide by zero" );
        if ( cc == 0 )
        {
            // => invert
            if ( deleteObject() ) delete this;
            return CFFactory::basic( 0 );
        }
        if ( invert )
        {
            mpz_init_set_si( n, cc );
            mpz_mul( n, n, _den );
            mpz_init_set( d, _num );
        }
        else
        {
            mpz_init_set_si( d, cc );
            mpz_mul( d, d, _den );
            mpz_init_set( n, _num );
        }
    }
    else
    {
        ASSERT( c->levelcoeff() == IntegerDomain, "expected integer" );
        if ( invert )
        {
            mpz_init_set( n, InternalInteger::MPI( c ) );
            mpz_mul( n, n, _den );
            mpz_init_set( d, _num );
        }
        else
        {
            mpz_init_set( d, InternalInteger::MPI( c ) );
            mpz_mul( d, d, _den );
            mpz_init_set( n, _num );
        }
    }
    if ( mpz_cmp_si( d, 0 ) < 0 )
    {
        mpz_neg( d, d );
        mpz_neg( n, n );
    }
    mpz_init( g );
    mpz_gcd( g, n, d );
    if ( mpz_cmp_si( g, 1 ) != 0 )
    {
        mpz_divexact( d, d, g );
        mpz_divexact( n, n, g );
    }
    mpz_clear( g );
    if ( deleteObject() ) delete this;
    if ( ! invert )
    {
        // then there was no way for the result to become an integer
        return new InternalRational( n, d );
    }
    if ( mpz_cmp_si( d, 1 ) == 0 )
    {
        mpz_clear( d );
        if ( mpz_is_imm( n ) )
        {
            InternalCF * res = int2imm( mpz_get_si( n ) );
            mpz_clear( n );
            return res;
        }
        else
        {
            return new InternalInteger( n );
        }
    }
    else
        return new InternalRational( n, d );
}

InternalCF* InternalRational::divcoeff( InternalCF* c, bool invert )
{
    return dividecoeff( c, invert );
}

InternalCF* InternalRational::modulocoeff( InternalCF* c, bool invert )
{
    return modcoeff( c, invert );
}

InternalCF* InternalRational::modcoeff( InternalCF* c, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK || ! ::is_imm( c ), "integer expected" );
    ASSERT( invert || ! ::is_imm( c ) || imm2int( c ) != 0, "divide by zero" );
    if ( deleteObject() ) delete this;
    return CFFactory::basic( 0 );
}

void InternalRational::divremcoeff( InternalCF* c, InternalCF*& quot, InternalCF*& rem, bool invert )
{
    quot = copyObject();
    quot = quot->dividecoeff( c, invert );
    rem = CFFactory::basic( 0 );
}

bool InternalRational::divremcoefft( InternalCF* c, InternalCF*& quot, InternalCF*& rem, bool invert )
{
    divremcoeff( c, quot, rem, invert );
    return true;
}

/**
 * @sa CanonicalForm::bgcd(), InternalRational::bgcdcoeff()
**/
InternalCF *
InternalRational::bgcdsame ( const InternalCF * const ) const
{
    return int2imm( 1 );
}

/**
 * @sa CanonicalForm::bgcd(), InternalRational::bgcdsame()
**/
InternalCF *
InternalRational::bgcdcoeff ( const InternalCF * const )
{
    return int2imm( 1 );
}

/**
 * @sa CanonicalForm::bextgcd(), InternalRational::bextgcdcoeff()
**/
InternalCF *
InternalRational::bextgcdsame ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
{
    a = 1/CanonicalForm( copyObject() ); b = 0;
    return int2imm( 1 );
}

/**
 * @sa CanonicalForm::bextgcd(), InternalRational::bextgcdsame()
**/
InternalCF *
InternalRational::bextgcdcoeff ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
{
    a = 1/CanonicalForm( copyObject() ); b = 0;
    return int2imm( 1 );
}

/**
 * reduce InternalRational to lowest terms
**/
InternalCF * InternalRational::normalize_myself()
{
    ASSERT( getRefCount() == 1, "illegal operation" );
    mpz_t g;
    mpz_init( g );
    mpz_gcd( g, _num, _den );
    if ( mpz_cmp_si( g, 1 ) != 0 )
    {
        mpz_divexact( _num, _num, g );
        mpz_divexact( _den, _den, g );
    }
    mpz_clear( g );
    if ( mpz_cmp_si( _den, 0 ) < 0 )
    {
        mpz_neg( _num, _num );
        mpz_neg( _den, _den );
    }
    if ( mpz_cmp_si( _den, 1 ) == 0 )
    {
        if ( mpz_is_imm( _num ) )
        {
            InternalCF * res = int2imm( mpz_get_si( _num ) );
            delete this;
            return res;
        }
        else
        {
            mpz_t res;
            mpz_init_set( res, _num );
            delete this;
            return new InternalInteger( res );
        }
    }
    else
        return this;
}


long InternalRational::intval() const
{

    ASSERT( mpz_cmp_si( _den, 1 ) == 0, "illegal operation" );
    return mpz_get_si( _num );

}

/**
 * @sa CanonicalForm::sign()
**/
int
InternalRational::sign () const
{
    return mpz_sgn( _num );
}
