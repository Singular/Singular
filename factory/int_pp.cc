/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"
#include "globaldefs.h"

#include "cf_assert.h"

#include "cf_defs.h"
#include "int_pp.h"
#include "canonicalform.h"
#include "cf_factory.h"
#include "imm.h"

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
#endif

#if !defined(HAVE_NTL)
#if !defined(HAVE_FLINT)||(__FLINT_RELEASE<=20600)
GLOBAL_VAR mpz_t InternalPrimePower::primepow;
GLOBAL_VAR mpz_t InternalPrimePower::primepowhalf;
GLOBAL_VAR int InternalPrimePower::prime;
GLOBAL_VAR int InternalPrimePower::exp;
GLOBAL_VAR bool InternalPrimePower::initialized = false;


InternalPrimePower::InternalPrimePower()
{
    mpz_init( thempi );
}

InternalPrimePower::InternalPrimePower( const int i )
{
    initialize();
    mpz_init_set_si( thempi, i );
    if ( mpz_cmp_si( thempi, 0 ) < 0 ) {
        mpz_neg( thempi, thempi );
        mpz_mod( thempi, thempi, primepow );
        mpz_sub( thempi, primepow, thempi );
    }
    else
        mpz_mod( thempi, thempi, primepow );
}

InternalPrimePower::InternalPrimePower( const mpz_ptr mpi) { thempi[0]=*mpi;}

InternalPrimePower::InternalPrimePower( const char * str, const int base )
{
    initialize();
    mpz_init_set_str( thempi, str, base );
    if ( mpz_cmp_si( thempi, 0 ) < 0 ) {
        mpz_neg( thempi, thempi );
        mpz_mod( thempi, thempi, primepow );
        mpz_sub( thempi, primepow, thempi );
    }
    else
        mpz_mod( thempi, thempi, primepow );
}

InternalPrimePower::~InternalPrimePower()
{
    mpz_clear( thempi );
}

InternalCF* InternalPrimePower::deepCopyObject() const
{
    mpz_t dummy;
    mpz_init_set( dummy, thempi );
    return new InternalPrimePower( dummy );
}

InternalCF * InternalPrimePower::normalize_myself()
{
    ASSERT( getRefCount() == 1, "illegal operation" );
    if ( mpz_cmp_si( thempi, 0 ) < 0 ) {
        mpz_neg( thempi, thempi );
        mpz_mod( thempi, thempi, primepow );
        mpz_sub( thempi, primepow, thempi );
    }
    else
        mpz_mod( thempi, thempi, primepow );
    return this;
}

void InternalPrimePower::initialize()
{
    if (initialized) return;
    mpz_init_set_si( primepow, 3 );
    mpz_init_set_si( primepowhalf, 1 );
    prime = 3;
    exp = 1;
    initialized = true;
}

void
InternalPrimePower::setPrimePower( int p, int k )
{
    ASSERT( p > 1 && k > 0, "illegal prime power" );
    initialize();
    if ( p != prime || k != exp ) {
        mpz_set_si( primepow, p );
        mpz_pow_ui( primepow, primepow, (unsigned int)k );
        mpz_fdiv_q_ui( primepowhalf, primepow, 2 );
        prime = p;
        exp = k;
    }
}

#ifndef NOSTREAMIO
void InternalPrimePower::print( OSTREAM & os, char * c )
{
    if ( *c == '*' && mpz_cmp_si( thempi, 1 ) == 0 )
        os << c+1;
    else if ( *c == '*' && mpz_cmp_si( thempi, -1 ) == 0 )
        os << '-' << c+1;
    else {
        char * str = new char[mpz_sizeinbase( thempi, 10 ) + 2];
        str = mpz_get_str( str, 10, thempi );
        os << str << c;
        delete [] str;
    }
}
#endif /* NOSTREAMIO */

//{{{ bool InternalPrimePower::isOne, isZero () const
// docu: see CanonicalForm::isOne(), CanonicalForm::isZero()
bool
InternalPrimePower::isOne () const
{
    return mpz_cmp_ui( thempi, 1 ) == 0;
}

bool
InternalPrimePower::isZero () const
{
    return mpz_sgn( thempi ) == 0;
}
//}}}

bool InternalPrimePower::is_imm() const
{
    return false;
}

InternalCF* InternalPrimePower::genZero()
{
    if ( isZero() )
        return copyObject();
    else
        return new InternalPrimePower();
}

InternalCF* InternalPrimePower::genOne()
{
    if ( isOne() )
        return copyObject();
    else
        return new InternalPrimePower();
}

//{{{ InternalCF * InternalPrimePower::neg ()
// docu: see CanonicalForm::operator -()
InternalCF *
InternalPrimePower::neg ()
{
    if ( getRefCount() > 1 ) {
        decRefCount();
        mpz_t dummy;
        mpz_init( dummy );
        mpz_sub( dummy, primepow, thempi );
        return new InternalPrimePower( dummy );
    } else {
        mpz_sub( thempi, primepow, thempi );
        return this;
    }
}
//}}}


InternalCF* InternalPrimePower::addsame( InternalCF * c )
{
    if ( getRefCount() > 1 ) {
        decRefCount();
        mpz_t dummy;
        mpz_init( dummy );
        mpz_add( dummy, thempi, MPI( c ) );
        if ( mpz_cmp( dummy, primepow ) >= 0 )
            mpz_sub( dummy, dummy, primepow );
        return new InternalPrimePower( dummy );
    }
    else {
        mpz_add( thempi, thempi, MPI( c ) );
        if ( mpz_cmp( thempi, primepow ) >= 0 )
            mpz_sub( thempi, thempi, primepow );
        return this;
    }
}

InternalCF* InternalPrimePower::subsame( InternalCF * c )
{
    if ( getRefCount() > 1 ) {
        decRefCount();
        mpz_t dummy;
        mpz_init( dummy );
        mpz_sub( dummy, thempi, MPI( c ) );
        if ( mpz_cmp_si( dummy, 0 ) < 0 )
            mpz_add( dummy, dummy, primepow );
        return new InternalPrimePower( dummy );
    }
    else {
        mpz_sub( thempi, thempi, MPI( c ) );
        if ( mpz_cmp_si( thempi, 0 ) < 0 )
            mpz_add( thempi, thempi, primepow );
        return this;
    }
}

InternalCF* InternalPrimePower::mulsame( InternalCF * c )
{
    if ( getRefCount() > 1 ) {
        decRefCount();
        mpz_t dummy;
        mpz_init( dummy );
        mpz_mul( dummy, thempi, MPI( c ) );
        mpz_mod( dummy, dummy, primepow );
        return new InternalPrimePower( dummy );
    }
    else {
        mpz_mul( thempi, thempi, MPI( c ) );
        mpz_mod( thempi, thempi, primepow );
        return this;
    }
}

InternalCF* InternalPrimePower::dividesame( InternalCF * c )
{
    return divsame( c );
}

InternalCF* InternalPrimePower::divsame( InternalCF * c )
{
    if ( c == this ) {
        if ( deleteObject() ) delete this;
        return CFFactory::basic( 1L );
    }
    if ( getRefCount() > 1 ) {
        decRefCount();
        mpz_t dummy, a, b;
        mpz_init( dummy ); mpz_init( a ); mpz_init( b );
        mpz_gcdext( dummy, a, b, primepow, MPI( c ) );
        ASSERT( mpz_cmp_si( dummy, 1 ) == 0, "illegal inversion" );
        mpz_clear( dummy ); mpz_clear( a );
        if ( mpz_cmp_si( b, 0 ) < 0 )
            mpz_add( b, b, primepow );
        mpz_mul( b, b, thempi );
        mpz_mod( b, b, primepow );
        return new InternalPrimePower( b );
    }
    else {
        mpz_t dummy, a, b;
        mpz_init( dummy ); mpz_init( a ); mpz_init( b );
        mpz_gcdext( dummy, a, b, primepow, MPI( c ) );
        ASSERT( mpz_cmp_si( dummy, 1 ) == 0, "illegal inversion" );
        if ( mpz_cmp_si( b, 0 ) < 0 )
            mpz_add( b, b, primepow );
        mpz_mul( thempi, b, thempi );
        mpz_mod( thempi, thempi, primepow );
        mpz_clear( dummy ); mpz_clear( a ); mpz_clear( b );
        return this;
    }
}

InternalCF *
InternalPrimePower::modulosame ( InternalCF * )
{
    if ( deleteObject() ) delete this;
    return CFFactory::basic( 0L );
}

InternalCF *
InternalPrimePower::modsame ( InternalCF * )
{
    if ( deleteObject() ) delete this;
    return CFFactory::basic( 0L );
}

void
InternalPrimePower::divremsame ( InternalCF * c, InternalCF * & quot, InternalCF * & rem )
{
    if ( c == this ) {
        quot = CFFactory::basic( 1L );
        rem = CFFactory::basic( 0L );
    }
    else {
        mpz_t dummy, a, b;
        mpz_init( dummy ); mpz_init( a ); mpz_init( b );
        mpz_gcdext( dummy, a, b, primepow, MPI( c ) );
        ASSERT( mpz_cmp_si( dummy, 1 ) == 0, "illegal inversion" );
        mpz_clear( dummy ); mpz_clear( a );
        if ( mpz_cmp_si( b, 0 ) < 0 )
            mpz_add( b, b, primepow );
        mpz_mul( b, b, thempi );
        mpz_mod( b, b, primepow );
        quot = new InternalPrimePower( b );
        rem = CFFactory::basic( 0L );
    }
}

bool
InternalPrimePower::divremsamet ( InternalCF * c, InternalCF * & quot, InternalCF * & rem )
{
    divremsame( c, quot, rem );
    return true;
}

//{{{ int InternalPrimePower::comparesame, comparecoeff ( InternalCF * c )
// docu: see CanonicalForm::operator <(), CanonicalForm::operator ==()
int
InternalPrimePower::comparesame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == PrimePowerDomain, "incompatible base coefficients" );
    return mpz_cmp( thempi, MPI( c ) );
}

int
InternalPrimePower::comparecoeff ( InternalCF * )
{
    ASSERT1( 0, "comparecoeff() not implemented for class %s", this->classname() );
    return 0;
}
//}}}

InternalCF *
InternalPrimePower::addcoeff ( InternalCF * )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

InternalCF *
InternalPrimePower::subcoeff ( InternalCF *, bool )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

InternalCF *
InternalPrimePower::mulcoeff ( InternalCF * )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

InternalCF *
InternalPrimePower::dividecoeff ( InternalCF *, bool )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

InternalCF *
InternalPrimePower::divcoeff ( InternalCF *, bool )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

InternalCF *
InternalPrimePower::modcoeff ( InternalCF *, bool )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

InternalCF *
InternalPrimePower::modulocoeff ( InternalCF *, bool )
{
    ASSERT( 0, "this function should never be called" );
    return this;
}

void
InternalPrimePower::divremcoeff ( InternalCF *, InternalCF * &, InternalCF * &, bool )
{
    ASSERT( 0, "this function should never be called" );
}

bool
InternalPrimePower::divremcoefft ( InternalCF *, InternalCF * &, InternalCF * &, bool )
{
    ASSERT( 0, "this function should never be called" );
    return true;
}

long
InternalPrimePower::intval () const
{
  return mpz_get_si( thempi );
}

int
InternalPrimePower::intmod( int p ) const
{
  return (int)mpz_fdiv_ui( thempi, (unsigned long)p );
}

//{{{ int InternalPrimePower::sign () const
// docu: see CanonicalForm::sign()
int
InternalPrimePower::sign () const
{
    return mpz_sgn( thempi );
}
//}}}
#endif
#endif
