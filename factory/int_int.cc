/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: int_int.cc,v 1.10 1997-12-17 11:47:48 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_globals.h"
#include "int_int.h"
#include "int_rat.h"
#include "imm.h"
#include "canonicalform.h"
#include "gmpext.h"

InternalInteger::InternalInteger()
{
    mpz_init( &thempi );
}

InternalInteger::InternalInteger( const int i )
{
    mpz_init_set_si( &thempi, i );
}

InternalInteger::InternalInteger( const MP_INT & mpi) : thempi( mpi ) {}

InternalInteger::InternalInteger( const char * str )
{
    mpz_init_set_str( &thempi, str, 10 );
}

InternalInteger::~InternalInteger()
{
    mpz_clear( &thempi );
}

InternalCF* InternalInteger::deepCopyObject() const
{
    MP_INT dummy;
    mpz_init_set( &dummy, &thempi );
    return new InternalInteger( dummy );
}

#ifndef NOSTREAMIO
void InternalInteger::print( ostream & os, char * c )
{
    if ( *c == '*' && mpz_cmp_si( &thempi, 1 ) == 0 )
	os << c+1;
    else if ( *c == '*' && mpz_cmp_si( &thempi, -1 ) == 0 )
	os << '-' << c+1;
    else {
	char * str = new char[mpz_sizeinbase( &thempi, 10 ) + 2];
	str = mpz_get_str( str, 10, &thempi );
	os << str << c;
	delete [] str;
    }
}
#endif /* NOSTREAMIO */

bool InternalInteger::isZero() const
{
    return mpz_cmp_si( &thempi, 0 ) == 0;
}

bool InternalInteger::isOne() const
{
    return mpz_cmp_si( &thempi, 1 ) == 0;
}

bool InternalInteger::is_imm() const
{
    return mpz_is_imm( &thempi );
}

InternalCF* InternalInteger::genZero()
{
    if ( isZero() )
	return copyObject();
    else
	return new InternalInteger();
}

InternalCF* InternalInteger::genOne()
{
    if ( isOne() )
	return copyObject();
    else
	return new InternalInteger( 1 );
}

InternalCF* InternalInteger::neg()
{
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	mpz_init( &dummy );
	mpz_neg( &dummy, &thempi );
	return new InternalInteger( dummy );
    }
    else {
	mpz_neg( &thempi, &thempi );
	return this;
    }
}


InternalCF* InternalInteger::addsame( InternalCF * c )
{
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	mpz_init( &dummy );
	mpz_add( &dummy, &thempi, &MPI( c ) );
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	mpz_add( &thempi, &thempi, &MPI( c ) );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::subsame( InternalCF * c )
{
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	mpz_init( &dummy );
	mpz_sub( &dummy, &thempi, &MPI( c ) );
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	mpz_sub( &thempi, &thempi, &MPI( c ) );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::mulsame( InternalCF * c )
{
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	mpz_init( &dummy );
	mpz_mul( &dummy, &thempi, &MPI( c ) );
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	mpz_mul( &thempi, &thempi, &MPI( c ) );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::dividesame( InternalCF * c )
{
    return divsame( c );
}

InternalCF* InternalInteger::divsame( InternalCF * c )
{
    if ( c == this ) {
	if ( deleteObject() ) delete this;
	return CFFactory::basic( 1 );
    }
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	MP_INT n, d;
	mpz_init_set( &n, &thempi );
	mpz_init_set( &d, &MPI( c ) );
	if ( deleteObject() ) delete this;
	InternalRational * result = new InternalRational( n, d );
	return result->normalize_myself();
    }
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT quot;
	MP_INT rem;
	int signmpi = mpz_cmp_si( &thempi, 0 );
	int signc = mpz_cmp_si( &MPI( c ), 0 );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	mpz_init( &quot ); mpz_init( &rem );
	mpz_divmod( &quot, &rem, &thempi, &MPI( c ) );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	if ( signmpi < 0 && signc < 0 ) {
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_add_ui( &quot, &quot, 1 );
	}
	else  if ( signc < 0 )
	    mpz_neg( &quot, &quot );
	else  if ( signmpi < 0 ) {
	    mpz_neg( &quot, &quot );
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_sub_ui( &quot, &quot, 1 );
	}
	mpz_clear( &rem );
	if ( mpz_is_imm( &quot ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &quot ) );
	    mpz_clear( &quot );
	    return res;
	}
	else
	    return new InternalInteger( quot );
    }
    else {
	MP_INT rem;
	mpz_init( &rem );
	int signmpi = mpz_cmp_si( &thempi, 0 );
	int signc = mpz_cmp_si( &MPI( c ), 0 );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	mpz_divmod( &thempi, &rem, &thempi, &MPI( c ) );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	if ( signmpi < 0 && signc < 0 ) {
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_add_ui( &thempi, &thempi, 1 );
	}
	else  if ( signc < 0 )
	    mpz_neg( &thempi, &thempi );
	else  if ( signmpi < 0 ) {
	    mpz_neg( &thempi, &thempi );
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_sub_ui( &thempi, &thempi, 1 );
	}
	mpz_clear( &rem );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::modulosame ( InternalCF * c )
{
    return modsame( c );
}

InternalCF* InternalInteger::modsame( InternalCF * c )
{
    if ( cf_glob_switches.isOn( SW_RATIONAL ) || c == this ) {
	if ( deleteObject() ) delete this;
	return CFFactory::basic( 0 );
    }
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	mpz_init( &dummy );
	int signmpi = mpz_cmp_si( &thempi, 0 );
	int signc = mpz_cmp_si( &MPI( c ), 0 );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	mpz_mod( &dummy, &thempi, &MPI( c ) );
	if ( signmpi < 0 && mpz_cmp_si( &dummy, 0 ) != 0 ) {
	    mpz_sub( &dummy, &MPI( c ), &dummy );
	}
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	int signmpi = mpz_cmp_si( &thempi, 0 );
	int signc = mpz_cmp_si( &MPI( c ), 0 );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	mpz_mod( &thempi, &thempi, &MPI( c ) );
	if ( signmpi < 0 && mpz_cmp_si( &thempi, 0 ) != 0 ) {
	    mpz_sub( &thempi, &MPI( c ), &thempi );
	}
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

void InternalInteger::divremsame( InternalCF * c, InternalCF*& quot, InternalCF*& rem )
{
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	quot = copyObject();
	quot = quot->dividesame( c );
	rem = CFFactory::basic( 0 );
    }
    else  if ( c == this ) {
	quot = CFFactory::basic( 1 );
	rem = CFFactory::basic( 0 );
    }
    else {
	MP_INT q;
	MP_INT r;
	mpz_init( &q ); mpz_init( &r );
	int signmpi = mpz_cmp_si( &thempi, 0 );
	int signc = mpz_cmp_si( &MPI( c ), 0 );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	mpz_divmod( &q, &r, &thempi, &MPI( c ) );
	if ( signmpi < 0 && mpz_cmp_si( &r, 0 ) != 0 ) {
	    mpz_sub( &r, &MPI( c ), &r );
	}
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signc < 0 )
	    mpz_neg( &MPI( c ), &MPI( c ) );
	if ( signmpi < 0 && signc < 0 ) {
	    if ( mpz_cmp_si( &r, 0 ) != 0 )
		mpz_add_ui( &q, &q, 1 );
	}
	else  if ( signc < 0 )
	    mpz_neg( &q, &q );
	else  if ( signmpi < 0 ) {
	    mpz_neg( &q, &q );
	    if ( mpz_cmp_si( &r, 0 ) != 0 )
		mpz_sub_ui( &q, &q, 1 );
	}
	if ( mpz_is_imm( &q ) )
	    quot = int2imm( mpz_get_si( &q ) );
	else
	    quot = new InternalInteger( q );
	if ( mpz_is_imm( &r ) )
	    rem = int2imm( mpz_get_si( &r ) );
	else
	    rem = new InternalInteger( r );
    }
}

bool InternalInteger::divremsamet( InternalCF* c, InternalCF*& quot, InternalCF*& rem )
{
    divremsame( c, quot, rem );
    return true;
}

//{{{ int InternalInteger::comparesame, comparecoeff ( InternalCF * c )
// docu: see CanonicalForm::operator <(), CanonicalForm::operator ==()
int
InternalInteger::comparesame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain, "incompatible base coefficients" );
    return mpz_cmp( &thempi, &MPI( c ) );
}

int
InternalInteger::comparecoeff ( InternalCF * c )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    return mpz_cmp_si( &thempi, imm2int( c ) );
}
//}}}

InternalCF* InternalInteger::addcoeff( InternalCF* c )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    int cc = imm2int( c );
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	mpz_init( &dummy );
	if ( cc < 0 )
	    mpz_sub_ui( &dummy, &thempi, -cc );
	else
	    mpz_add_ui( &dummy, &thempi, cc );
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	if ( cc < 0 )
	    mpz_sub_ui( &thempi, &thempi, -cc );
	else
	    mpz_add_ui( &thempi, &thempi, cc );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::subcoeff( InternalCF* c, bool negate )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    int cc = imm2int( c );
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
	if ( negate ) {
	    mpz_init_set_si( &dummy, cc );
	    mpz_sub( &dummy, &dummy, &thempi );
	}
	else {
	    mpz_init( &dummy );
	    if ( cc < 0 )
		mpz_add_ui( &dummy, &thempi, -cc );
	    else
		mpz_sub_ui( &dummy, &thempi, cc );
	}
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	if ( negate ) {
	    MP_INT dummy;
	    mpz_init_set_si( &dummy, cc );
	    mpz_sub( &thempi, &dummy, &thempi );
	    mpz_clear( &dummy );
	}
	else
	    if ( cc < 0 )
		mpz_add_ui( &thempi, &thempi, -cc );
	    else
		mpz_sub_ui( &thempi, &thempi, cc );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::mulcoeff( InternalCF* c )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    int cc = imm2int( c );
    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT dummy;
 	mpz_init( &dummy );
 	if ( cc < 0 ) {
 	    mpz_mul_ui( &dummy, &thempi, -cc );
 	    mpz_neg( &dummy, &dummy );
 	}
 	else
 	    mpz_mul_ui( &dummy, &thempi, cc );
	if ( mpz_is_imm( &dummy ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &dummy ) );
	    mpz_clear( &dummy );
	    return res;
	}
	else
	    return new InternalInteger( dummy );
    }
    else {
	MP_INT dummy;
	mpz_init_set_si( &dummy, cc );
 	if ( cc < 0 ) {
 	    mpz_mul_ui( &thempi, &thempi, -cc );
 	    mpz_neg( &thempi, &thempi );
 	}
 	else
 	    mpz_mul_ui( &thempi, &thempi, cc );
	if ( mpz_is_imm( &thempi ) ) {
	    InternalCF * res = int2imm( mpz_get_si( &thempi ) );
	    delete this;
	    return res;
	}
	else
	    return this;
    }
}

InternalCF* InternalInteger::dividecoeff( InternalCF* c, bool invert )
{
    return divcoeff( c, invert );
}

InternalCF* InternalInteger::divcoeff( InternalCF* c, bool invert )
{
    ASSERT( invert || ! ::is_imm( c ) || imm2int( c ) != 0, "divide by zero" );
    int cc = imm2int( c );
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	MP_INT n, d;
	if ( invert ) {
	    mpz_init_set_si( &n, cc );
	    mpz_init_set( &d, &thempi );
	}
	else {
	    mpz_init_set_si( &d, cc );
	    mpz_init_set( &n, &thempi );
	}
	if ( deleteObject() ) delete this;
	InternalRational * result = new InternalRational( n, d );
	return result->normalize_myself();
    }
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    MP_INT quot;
    MP_INT rem;
    int signmpi = mpz_cmp_si( &thempi, 0 );
    int signc = cc;
    mpz_init( &quot ); mpz_init( &rem );
    if ( signmpi < 0 )
	mpz_neg( &thempi, &thempi );
    if ( signc < 0 )
	cc = -cc;
    if ( invert ) {
	MP_INT ccc;
	mpz_init_set_si( &ccc, cc );
	mpz_divmod( &quot, &rem, &ccc, &thempi );
	mpz_clear( &ccc );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signmpi < 0 && signc < 0 ) {
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_add_ui( &quot, &quot, 1 );
	}
	else  if ( signmpi < 0 )
	    mpz_neg( &quot, &quot );
	else  if ( signc < 0 ) {
	    mpz_neg( &quot, &quot );
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_sub_ui( &quot, &quot, 1 );
	}
    }
    else {
	mpz_divmod_ui( &quot, &rem, &thempi, cc );
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
	if ( signmpi < 0 && signc < 0 ) {
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_add_ui( &quot, &quot, 1 );
	}
	else  if ( signc < 0 )
	    mpz_neg( &quot, &quot );
	else  if ( signmpi < 0 ) {
	    mpz_neg( &quot, &quot );
	    if ( mpz_cmp_si( &rem, 0 ) != 0 )
		mpz_sub_ui( &quot, &quot, 1 );
	}
    }
    mpz_clear( &rem );
    if ( deleteObject() ) delete this;
    if ( mpz_is_imm( &quot ) ) {
	InternalCF * res = int2imm( mpz_get_si( &quot ) );
	mpz_clear( &quot );
	return res;
    }
    else
	return new InternalInteger( quot );
}

InternalCF* InternalInteger::modulocoeff( InternalCF * c, bool invert )
{
    return modcoeff( c, invert );
}

InternalCF* InternalInteger::modcoeff( InternalCF* c, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	// in rational mode remainder is zero
	if ( deleteObject() ) delete this;
	return CFFactory::basic( 0 );
    }
    int cc = imm2int( c );
    MP_INT rem;
    int signmpi = mpz_cmp_si( &thempi, 0 );
    int signc = cc;
    mpz_init( &rem );
    if ( signmpi < 0 )
	mpz_neg( &thempi, &thempi );
    if ( signc < 0 )
	cc = -cc;
    if ( invert ) {
	MP_INT ccc;
	mpz_init_set_si( &ccc, cc );
	mpz_mod( &rem, &ccc, &thempi );
	mpz_clear( &ccc );
	if ( signc < 0 && mpz_cmp_si( &rem, 0 ) != 0 ) {
	    mpz_sub( &rem, &thempi, &rem );
	}
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
    }
    else {
	mpz_mod_ui( &rem, &thempi, cc );
	if ( signmpi < 0 && mpz_cmp_si( &rem, 0 ) != 0 ) {
	    mpz_neg( &rem, &rem );
	    mpz_add_ui( &rem, &rem, cc );
	}
	if ( signmpi < 0 )
	    mpz_neg( &thempi, &thempi );
    }
    if ( deleteObject() ) delete this;
    if ( mpz_is_imm( &rem ) ) {
	InternalCF * res = int2imm( mpz_get_si( &rem ) );
	mpz_clear( &rem );
	return res;
    }
    else
	return new InternalInteger( rem );
}

void InternalInteger::divremcoeff( InternalCF* c, InternalCF*& quot, InternalCF*& rem, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );

    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	quot = copyObject();
	quot = quot->dividecoeff( c, invert );
	rem = CFFactory::basic( 0 );
	return;
    }
    quot = copyObject();
    quot = quot->divcoeff( c, invert );
    rem = copyObject();
    rem = rem->modcoeff( c, invert );
    return;
    int cc = imm2int( c );
    MP_INT q, r;
    int signmpi = mpz_cmp_si( &thempi, 0 );
    int signc = cc;

    mpz_init( &q ); mpz_init( &r );
    if ( signmpi < 0 )
	mpz_neg( &thempi, &thempi );
    if ( signc < 0 )
	cc = -cc;
    if ( invert ) {
	MP_INT ccc;
	mpz_init_set_si( &ccc, cc );
	mpz_divmod( &q, &r, &ccc, &thempi );
	mpz_clear( &ccc );
	if ( signc < 0 && signmpi < 0 ) {
	    if ( mpz_cmp_si( &r, 0 ) != 0 ) {
		mpz_add_ui( &q, &q, 1 );
		mpz_sub( &r, &thempi, &r );
	    }
	}
	else  if ( signc < 0 ) {
	    if ( mpz_cmp_si( &r, 0 ) != 0 ) {
		mpz_add_ui( &q, &q, 1 );
		mpz_neg( &q, &q );
		mpz_sub( &r, &thempi, &r );
	    }
	}
	else  if ( signmpi < 0 )
	    mpz_neg( &q, &q );
    }
    else {
	mpz_divmod_ui( &q, &r, &thempi, cc );
	if ( signmpi < 0 && signc < 0 ) {
	    if ( mpz_cmp_si( &r, 0 ) != 0 ) {
		mpz_add_ui( &q, &q, 1 );
		mpz_neg( &r, &r );
		mpz_add_ui( &r, &r, cc );
	    }
	}
	else  if ( signmpi < 0 ) {
	    if ( mpz_cmp_si( &r, 0 ) != 0 ) {
		mpz_add_ui( &q, &q, 1 );
		mpz_neg( &q, &q );
		mpz_neg( &r, &r );
		mpz_add_ui( &r, &r, cc );
	    }
	}
	else  if ( signc < 0 )
	    mpz_neg( &q, &q );
    }
    if ( signmpi < 0 )
	mpz_neg( &thempi, &thempi );
    if ( mpz_is_imm( &r ) ) {
	rem = int2imm( mpz_get_si( &r ) );
	mpz_clear( &r );
    }
    else
	rem = new InternalInteger( r );
    if ( mpz_is_imm( &q ) ) {
	quot = int2imm( mpz_get_si( &q ) );
	mpz_clear( &q );
    }
    else
	quot = new InternalInteger( q );
}

bool InternalInteger::divremcoefft( InternalCF* c, InternalCF*& quot, InternalCF*& rem, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    divremcoeff( c, quot, rem, invert );
    return true;
}

int InternalInteger::intval() const
{
  return (int)mpz_get_si( &thempi );
}

int InternalInteger::intmod( int p ) const
{
  return (int)mpz_mmod_ui( 0, &thempi, (unsigned long)p );
}

//{{{ int InternalInteger::sign () const
// docu: see CanonicalForm::sign()
int
InternalInteger::sign () const
{
    return mpz_sgn( &thempi );
}
//}}}

//{{{ InternalCF * InternalInteger::sqrt ()
// docu: see CanonicalForm::sqrt()
InternalCF *
InternalInteger::sqrt ()
{
    ASSERT( mpz_cmp_si( &thempi, 0 ) >= 0, "sqrt() argument < 0" );
    MP_INT result;
    mpz_init( &result );
    mpz_sqrt( &result, &thempi );
    if ( mpz_is_imm( &result ) ) {
	InternalCF * res = int2imm( mpz_get_si( &result ) );
	mpz_clear( &result );
	return res;
    }
    else
	return new InternalInteger( result );
}
//}}}

//{{{ int InternalInteger::ilog2 ()
// docu: see CanonicalForm::ilog2()
int
InternalInteger::ilog2 ()
{
    ASSERT( mpz_cmp_si( &thempi, 0 ) > 0, "log() argument <= 0" );
    return mpz_sizeinbase( &thempi, 2 ) - 1;
}
//}}}
