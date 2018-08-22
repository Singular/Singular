// ----------------------------------------------------------------------------
//  GMPrat.cc
//  begin of file
//  originally written by Gerd Sussner, sussner@mi.uni-erlangen.de
//  copied by Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define  GMPRAT_CC

#include "kernel/mod2.h"

#ifdef HAVE_SPECTRUM

#ifdef   GMPRAT_PRINT
#include <iostream.h>
#ifndef  GMPRAT_IOSTREAM
#include <stdio.h>
#endif
#endif

#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif

#include "kernel/spectrum/GMPrat.h"

// ----------------------------------------------------------------------------
//  disconnect a rational from its reference
// ----------------------------------------------------------------------------

void    Rational::disconnect()
{
    if( p->n>1)
    {
        rep *old_p = p;
        p->n--;
        p = new rep;
        mpq_init(p->rat);
        mpq_set(p->rat, old_p->rat);
    }
}

// ----------------------------------------------------------------------------
//  Constructors
// ----------------------------------------------------------------------------

Rational::Rational( )
{
    p = new rep;
    mpq_init( p->rat );
}

Rational::Rational( int a )
{
    p = new rep;
    mpq_init( p->rat );
    mpq_set_si( p->rat,(long)a,1 );
}

Rational::Rational( const Rational& a )
{
    a.p->n++;
    p=a.p;
}

// ----------------------------------------------------------------------------
//  Constructors with two arguments: numerator and denominator
// ----------------------------------------------------------------------------

Rational::Rational(const Rational& a, const Rational& b)
{
    p=new rep;
    mpq_init(p->rat);
    mpq_div(p->rat, a.p->rat, b.p->rat);
}

Rational::Rational(int a, int b)
{
    if (b<0) a=-a;
    p=new rep;
    mpq_init(p->rat);
    mpq_set_si(p->rat,(long) a,(unsigned long) abs(b));
    mpq_canonicalize(p->rat);
}

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------

Rational::~Rational()
{
  if (--(p->n)==0)
  {
    mpq_clear(p->rat);
    delete p;
  }
}

// ----------------------------------------------------------------------------
//  Assignment operators
// ----------------------------------------------------------------------------

Rational& Rational::operator=(int a)
{
  if( p->n>1)
  {
    p->n--;
    p = new rep;
    mpq_init(p->rat);
  }
  mpq_set_si(p->rat,(long) a,1);
  return *this;
}

Rational& Rational::operator=(const Rational& a)
{
  a.p->n++;
  if (--(p->n)==0)
  {
    mpq_clear(p->rat);
    delete p;
  }
  p=a.p;
  return *this;
}

// ----------------------------------------------------------------------------
//  Numerator and denominator
// ----------------------------------------------------------------------------

Rational Rational::get_num( )
{
    Rational erg;

    mpq_set_num( erg.p->rat,mpq_numref( p->rat ) );

    return  erg;
}

int Rational::get_num_si( )
{
    return  mpz_get_si( mpq_numref( p->rat ) );
}

Rational Rational::get_den( )
{
    Rational erg;

    mpq_set_num( erg.p->rat,mpq_denref( p->rat ) );

    return  erg;
}

int Rational::get_den_si( )
{
    return  mpz_get_si( mpq_denref( p->rat ) );
}

// ----------------------------------------------------------------------------
//  Casting
// ----------------------------------------------------------------------------

Rational::operator int()
{
  mpz_t h;
  long ret_val;

  mpz_init(h);
  mpz_tdiv_q(h,mpq_numref(p->rat),mpq_denref(p->rat));
  ret_val=mpz_get_si(h);
  mpz_clear(h);

  return ret_val;
}

// ----------------------------------------------------------------------------
//  Unary minus
// ----------------------------------------------------------------------------

Rational
Rational::operator-()
{
  Rational erg;

  mpq_neg(erg.p->rat,p->rat);
  return erg;
}

Rational operator - ( const Rational &r )
{
  Rational erg;

  mpq_neg(erg.p->rat,r.p->rat);
  return erg;
}

// ----------------------------------------------------------------------------
//  Inverse
// ----------------------------------------------------------------------------

Rational
Rational::operator~()
{
  Rational erg;

  mpq_inv(erg.p->rat,p->rat);
  return erg;
}

// ----------------------------------------------------------------------------
//  +=, -= ...
// ----------------------------------------------------------------------------

Rational&
Rational::operator+=(const Rational &a)
{
  disconnect();
  mpq_add(p->rat,p->rat,a.p->rat);
  return *this;
}

Rational&
Rational::operator-=(const Rational &a)
{
  disconnect();
  mpq_sub(p->rat,p->rat,a.p->rat);
  return *this;
}

Rational&
Rational::operator*=(const Rational &a)
{
  disconnect();
  mpq_mul(p->rat,p->rat,a.p->rat);
  return *this;
}

Rational&
Rational::operator/=(const Rational &a)
{
  disconnect();
  mpq_div(p->rat,p->rat,a.p->rat);
  return *this;
}

// ----------------------------------------------------------------------------
//  Increment and decrement
// ----------------------------------------------------------------------------

Rational&
Rational::operator++()
{
  disconnect();
  mpz_add(mpq_numref(p->rat), mpq_numref(p->rat), mpq_denref(p->rat));
  return *this;
}

Rational
Rational::operator++(int)
{
  Rational erg(*this);

  disconnect();
  mpz_add(mpq_numref(p->rat), mpq_numref(p->rat), mpq_denref(p->rat));
  return erg;
}

Rational&
Rational::operator--()
{
  disconnect();
  mpz_sub(mpq_numref(p->rat), mpq_numref(p->rat), mpq_denref(p->rat));
  return *this;
}

Rational
Rational::operator--(int)
{
  Rational erg(*this);

  disconnect();
  mpz_sub(mpq_numref(p->rat), mpq_numref(p->rat), mpq_denref(p->rat));
  return erg;
}

// ----------------------------------------------------------------------------
//  Relational operators
// ----------------------------------------------------------------------------

bool operator<(const Rational& a,const Rational& b)
{
  if (mpq_cmp(a.p->rat,b.p->rat)<0) return true;
  return false;
}

bool operator<=(const Rational& a,const Rational& b)
{
  if (mpq_cmp(a.p->rat,b.p->rat)>0) return false;
  return true;
}

bool operator>(const Rational& a,const Rational& b)
{
  if (mpq_cmp(a.p->rat,b.p->rat)>0) return true;
  return false;
}

bool operator>=(const Rational& a,const Rational& b)
{
  if (mpq_cmp(a.p->rat,b.p->rat)<0) return false;
  return true;
}

bool operator==(const Rational& a,const Rational& b)
{
  if (mpq_equal(a.p->rat,b.p->rat)) return true;
  return false;
}

bool operator!=(const Rational& a,const Rational& b)
{
  if (mpq_equal(a.p->rat,b.p->rat)) return false;
  return true;
}

// ----------------------------------------------------------------------------
//  Ostream
// ----------------------------------------------------------------------------

#ifdef GMPRAT_PRINT
ostream& operator<< (ostream& s,const Rational& a)
{
    char *snum,*sdenom;

    snum   = mpz_get_str( NULL,10,mpq_numref(a.p->rat) );
    sdenom = mpz_get_str( NULL,10,mpq_denref(a.p->rat) );

    if( sdenom[0] == '1' && sdenom[1] == '\0' )
    {
        #ifdef GMPRAT_IOSTREAM
            s << snum;
        #else
            fprintf( stdout,snum );
        #endif
    }
    else
    {
        #ifdef GMPRAT_IOSTREAM
            s << snum << "/" << sdenom;
        #else
            fprintf( stdout,snum );
            fprintf( stdout,"/" );
            fprintf( stdout,sdenom );
        #endif
    }

    //free( snum );
    //free( sdenom );

    return s;
}
#endif

unsigned int Rational::length( ) const
{
    char *snum = (char*)omAlloc(mpz_sizeinbase(mpq_numref(p->rat),10)+2);
    char *sden = (char*)omAlloc(mpz_sizeinbase(mpq_denref(p->rat),10)+2);

    snum = mpz_get_str( snum,10,mpq_numref( p->rat ) );
    sden = mpz_get_str( sden,10,mpq_denref( p->rat ) );

    int length = strlen( snum );

    if( sden[0] != '1' || sden[1] != '\0' ) length += strlen( sden ) + 1;

    omFree( snum );
    omFree( sden );

    return  length;
}

// ----------------------------------------------------------------------------
//  Operators
// ----------------------------------------------------------------------------

Rational
operator+(const Rational& a,const Rational &b)
{
  Rational
    erg(a);

  return erg+=b;
}

Rational
operator-(const Rational& a,const Rational &b)
{
  Rational
    erg(a);

  return erg-=b;
}

Rational
operator*(const Rational& a,const Rational &b)
{
  Rational
    erg(a);

  return erg*=b;
}

Rational pow( const Rational& a,int e )
{
    Rational erg(1);

    for( int i=0; i<e; i++ )
    {
        erg *= a;
    }
    return erg;
}

Rational operator/(const Rational& a,const Rational &b)
{
  Rational
    erg(a);

  return erg/=b;
}

int sgn(const Rational& a)
{
  return mpq_sgn(a.p->rat);
}

Rational
abs(const Rational& a)
{
  Rational
    erg;

  if (mpq_sgn(a.p->rat)<0)
    mpq_neg(erg.p->rat,a.p->rat);
  else
    mpq_set(erg.p->rat,a.p->rat);
  return erg;
}

Rational gcd( const Rational &a,const Rational &b )
{
    if( a == 0 )
    {
        if( b == 0 )
        {
            return  (Rational)1;
        }
        else
        {
            return  abs( b );
        }
    }
    else if( b == 0 )
    {
        return  abs( a );
    }

    Rational erg;

    mpz_gcd( mpq_numref( erg.p->rat ),
            mpq_numref( a.p->rat ),mpq_numref( b.p->rat ) );
    mpz_gcd( mpq_denref( erg.p->rat ),
            mpq_denref( a.p->rat ),mpq_denref( b.p->rat ) );

    //mpq_canonicalize( erg.p->rat );

    return  abs( erg );
}

Rational gcd( Rational *a,int n )
{
    if( n == 1 )
    {
        return  a[0];
    }

    Rational g = gcd( a[0],a[1] );

    for( int i=2; i<n; i++ )
    {
        g = gcd( g,a[i] );
    }

    return  g;
}

Rational lcm( const Rational &a,const Rational &b )
{
    if( a == 0 )
    {
        return b;
    }
    else if( b == 0 )
    {
        return a;
    }

    return a*b/gcd(a,b);
}

Rational lcm( Rational *a,int n )
{
    if( n == 1 )
    {
        return  a[0];
    }

    Rational g = lcm( a[0],a[1] );

    for( int i=2; i<n; i++ )
    {
        g = lcm( g,a[i] );
    }

    return  g;
}

double  Rational::complexity( ) const
{
    double num = mpz_get_d( mpq_numref( p->rat ) );
    double den = mpz_get_d( mpq_denref( p->rat ) );

    if( num < 0 ) num = -num;
    if( den < 0 ) den = -den;

    return  ( num > den ? num : den );
}

#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  GMPrat.cc
//  end of file
// ----------------------------------------------------------------------------
