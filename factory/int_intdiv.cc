/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: int_intdiv.cc,v 1.2 1998-03-17 15:57:00 schmidt Exp $ */

//{{{ docu
//
// int_intdiv.cc - `InternalInteger' division algorithms.
//
//}}}

#include <config.h>

#include "imm.h"
#include "int_cf.h"
#include "int_int.h"
#include "int_rat.h"
#include "cf_gmp.h"
#include "gmpext.h"
#include "ftmpl_functions.h"

#include "stdio.h"

//{{{ InternalCF * InternalInteger::dividesame, dividecoeff ( InternalCF * c )
// docu: see CanonicalForm::operator /()
InternalCF *
InternalInteger::dividesame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain,
	    "type error: InternalInteger expected" );

    if ( c == this ) {
	if ( deleteObject() ) delete this;
	return int2imm( 1 );
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
	MP_INT mpiResult;
	mpz_init( &mpiResult );
	if ( mpz_sgn( &MPI( c ) ) > 0 )
	    mpz_fdiv_q( &mpiResult, &thempi, &MPI( c ) );
	else
	    mpz_cdiv_q( &mpiResult, &thempi, &MPI( c ) );
	return normalizeMPI( mpiResult );
    } else {
	if ( mpz_sgn( &MPI( c ) ) > 0 )
	    mpz_fdiv_q( &thempi, &thempi, &MPI( c ) );
	else
	    mpz_cdiv_q( &thempi, &thempi, &MPI( c ) );
	return normalizeMyself();
    }
}

InternalCF *
InternalInteger::dividecoeff ( InternalCF * c, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK,
	    "type error: immediate integer expected" );
    ASSERT( invert || imm2int( c ) != 0,
	    "math error: divide by zero" );

    int intC = imm2int( c );

    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	MP_INT n, d;
	if ( invert ) {
	    mpz_init_set_si( &n, intC );
	    mpz_init_set( &d, &thempi );
	}
	else {
	    mpz_init_set_si( &d, intC );
	    mpz_init_set( &n, &thempi );
	}
	if ( deleteObject() ) delete this;
	InternalRational * result = new InternalRational( n, d );
	return result->normalize_myself();
    }

    if ( invert ) {
	int mpiSign = mpz_sgn( &thempi );
	if ( deleteObject() ) delete this;
	if ( intC > 0 )
	    return int2imm( 0 );
	else
	    return int2imm( -mpiSign );
    } else if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT mpiResult;
	mpz_init( &mpiResult );
	if ( intC > 0 )
	    mpz_fdiv_q_ui( &mpiResult, &thempi, intC );
	else {
	    mpz_fdiv_q_ui( &mpiResult, &thempi, -intC );
	    mpz_neg( &mpiResult, &mpiResult );
	}
	return normalizeMPI( mpiResult );
    } else {
	if ( intC > 0 )
	    mpz_fdiv_q_ui( &thempi, &thempi, intC );
	else {
	    mpz_fdiv_q_ui( &thempi, &thempi, -intC );
	    mpz_neg( &thempi, &thempi );
	}
	return normalizeMyself();
    }
}
//}}}

//{{{ InternalCF * InternalInteger::divsame, divcoeff ( InternalCF * c )
// docu: see CanonicalForm::div()
InternalCF *
InternalInteger::divsame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain,
	    "type error: InternalInteger expected" );

    if ( getRefCount() > 1 ) {
	deleteObject();
	MP_INT mpiResult;
	mpz_init( &mpiResult );
	mpz_divexact( &mpiResult, &thempi, &MPI( c ) );
	return normalizeMPI( mpiResult );
    } else {
	mpz_divexact( &thempi, &thempi, &MPI( c ) );
	return normalizeMyself();
    }
}

InternalCF *
InternalInteger::divcoeff ( InternalCF * c, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK,
	    "type error: immediate integer expected" );
    ASSERT( ! invert || imm2int( c ) == 0, "math error: c does not divide CO" );

    if ( invert ) {
	if ( deleteObject() ) delete this;
	// this may happen iff `c' == 0
	return int2imm( 0 );
    } else if ( getRefCount() > 1 ) {
	deleteObject();
	MP_INT mpiC;
	MP_INT mpiResult;
	mpz_init_set_si( &mpiC, imm2int( c ) );
	mpz_init( &mpiResult );
	mpz_divexact( &mpiResult, &thempi, &mpiC );
	mpz_clear( &mpiC );
	return normalizeMPI( mpiResult );
    } else {
	MP_INT mpiC;
	mpz_init_set_si( &mpiC, imm2int( c ) );
	mpz_divexact( &thempi, &thempi, &mpiC );
	mpz_clear( &mpiC );
	return normalizeMyself();
    }
}
//}}}

// we have to divide the immediate integer `c' by CO.
// If `c' is positive, this is quite simple: we
// simply return `c'.  If `c' is negative we return
// abs(CO)-abs(`c').
// we have to divide CO by the immediate integer `c'.
// The remainder will be less than the absolute value of
// `c', hence it will fit again into an immediate.

//{{{ InternalCF * InternalInteger::modulosame, modulocoeff ( InternalCF * c )
// docu: see CanonicalForm::operator %()
InternalCF *
InternalInteger::modulosame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain,
	    "type error: InternalInteger expected" );

    if ( cf_glob_switches.isOn( SW_RATIONAL ) || c == this ) {
	if ( deleteObject() ) delete this;
	return int2imm( 0 );
    }

    if ( getRefCount() > 1 ) {
	decRefCount();
	MP_INT mpiResult;
	mpz_init( &mpiResult );
	mpz_mod( &mpiResult, &thempi, &MPI( c ) );
	return uiNormalizeMPI( mpiResult );
    } else {
	mpz_mod( &thempi, &thempi, &MPI( c ) );
	return uiNormalizeMyself();
    }
}

InternalCF *
InternalInteger::modulocoeff ( InternalCF * c, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK,
	    "type error: immediate integer expected" );
    ASSERT( invert || imm2int( c ) != 0,
	    "math error: divide by zero" );

    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	if ( deleteObject() ) delete this;
	return int2imm( 0 );
    }

    InternalCF * result;

    int intC = imm2int( c );

    if ( invert ) {
	// calculate c % CO
	if ( intC > 0 )
	    result = int2imm( intC );
	else {
	    // no checks for refCount == 1 are done.  It is not worth ...
	    MP_INT mpiResult;
	    mpz_init_set( &mpiResult, &thempi );
	    mpz_abs( &mpiResult, &mpiResult );
	    mpz_sub_ui( &mpiResult, &mpiResult, -intC );
	    result = uiNormalizeMPI( mpiResult );
	}
    } else {
	// calculate CO % c
	MP_INT dummy;
	mpz_init( &dummy );
	result = int2imm( mpz_mod_ui( &dummy, &thempi, (intC > 0 ? intC : -intC) ) );
	mpz_clear( &dummy );
    }

    // clean up
    if ( deleteObject() ) delete this;
    return result;
}
//}}}

//{{{ InternalCF * InternalInteger::modsame, modcoeff ( InternalCF * c )
// docu: see CanonicalForm::mod()
InternalCF *
InternalInteger::modsame ( InternalCF * c )
{
    return modulosame( c );
}

InternalCF *
InternalInteger::modcoeff ( InternalCF * c, bool invert )
{
    return modulocoeff( c, invert );
}
//}}}

//{{{ void InternalInteger::divremsame, divremcoeff ( InternalCF * c, InternalCF * & quot, InternalCF * & rem )
// docu: see CanonicalForm::divrem()
void
InternalInteger::divremsame ( InternalCF * c, InternalCF * & quot, InternalCF * & rem )
{
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	quot = copyObject();
	quot = quot->dividesame( c );
	rem = int2imm( 0 );
    }
    else  if ( c == this ) {
	quot = int2imm( 1 );
	rem = int2imm( 0 );
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

void
InternalInteger::divremcoeff ( InternalCF * c, InternalCF * & quot, InternalCF * & rem, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );

    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
	quot = copyObject();
	quot = quot->dividecoeff( c, invert );
	rem = int2imm( 0 );
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
//}}}

//{{{ bool InternalInteger::divremsamet, divremcoefft ( InternalCF * c, InternalCF * & quot, InternalCF * & rem )
// docu: see CanonicalForm::divremt()
bool
InternalInteger::divremsamet ( InternalCF * c, InternalCF * & quot, InternalCF * & rem )
{
    divremsame( c, quot, rem );
    return true;
}

bool
InternalInteger::divremcoefft ( InternalCF * c, InternalCF * & quot, InternalCF * & rem, bool invert )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );
    divremcoeff( c, quot, rem, invert );
    return true;
}
//}}}
