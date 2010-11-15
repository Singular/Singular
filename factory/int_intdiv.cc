/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// int_intdiv.cc - `InternalInteger' division algorithms.
//
//}}}

#include <config.h>

#include "canonicalform.h"
#include "imm.h"
#include "int_cf.h"
#include "int_int.h"
#include "int_rat.h"
#include "cf_gmp.h"
#include "gmpext.h"
#include "ftmpl_functions.h"

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
        } else {
            mpz_init_set( &n, &thempi );
            mpz_init_set_si( &d, intC );
        }
        if ( deleteObject() ) delete this;
        InternalRational * result = new InternalRational( n, d );
        return result->normalize_myself();
    }

    if ( invert ) {
        int mpiSign = mpz_sgn( &thempi );
        if ( deleteObject() ) delete this;
        if ( intC >= 0 )
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

    if ( c == this ) {
        if ( deleteObject() ) delete this;
        return int2imm( 1 );
    }

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
    ASSERT( invert || imm2int( c ) != 0,
            "math error: divide by zero" );
    ASSERT( ! invert || imm2int( c ) == 0,
            "math error: c does not divide CO" );

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

//{{{ InternalCF * InternalInteger::modulosame, modulocoeff ( InternalCF * c )
// docu: see CanonicalForm::operator %()
InternalCF *
InternalInteger::modulosame ( InternalCF * c )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain,
            "type error: InternalInteger expected" );

    if ( (c == this) || cf_glob_switches.isOn( SW_RATIONAL ) ) {
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

    int intC = imm2int( c );

    if ( invert ) {
        if ( intC >= 0 ) {
            if ( deleteObject() ) delete this;
            return c;
        } else {
            // no checks for refCount == 1 are done.  It is not worth ...
            MP_INT mpiResult;
            mpz_init_set( &mpiResult, &thempi );
            mpz_abs( &mpiResult, &mpiResult );
            mpz_sub_ui( &mpiResult, &mpiResult, -intC );
            if ( deleteObject() ) delete this;
            return uiNormalizeMPI( mpiResult );
        }
    } else {
        MP_INT dummy;
        mpz_init( &dummy );
        InternalCF * result = int2imm( mpz_mod_ui( &dummy, &thempi, tabs( intC ) ) );
        mpz_clear( &dummy );
        if ( deleteObject() ) delete this;
        return result;
    }
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
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain,
            "type error: InternalInteger expected" );

    if ( c == this ) {
        quot = int2imm( 1 );
        rem = int2imm( 0 );
        return;
    }

    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
        MP_INT n, d;
        mpz_init_set( &n, &thempi );
        mpz_init_set( &d, &MPI( c ) );
        InternalRational * result = new InternalRational( n, d );
        quot = result->normalize_myself();
        rem = int2imm( 0 );
        return;
    }

    MP_INT q;
    MP_INT r;
    mpz_init( &q ); mpz_init( &r );
    if ( mpz_sgn( &MPI( c ) ) > 0 )
        mpz_fdiv_qr( &q, &r, &thempi, &MPI( c ) );
    else
        mpz_cdiv_qr( &q, &r, &thempi, &MPI( c ) );

    quot = normalizeMPI( q );
    rem = uiNormalizeMPI( r );
}

void
InternalInteger::divremcoeff ( InternalCF * c, InternalCF * & quot, InternalCF * & rem, bool invert )
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
        } else {
            mpz_init_set( &n, &thempi );
            mpz_init_set_si( &d, intC );
        }
        InternalRational * result = new InternalRational( n, d );
        quot = result->normalize_myself();
        rem = int2imm( 0 );
        return;
    }

    if ( invert ) {
        if ( intC >= 0 ) {
            rem = c;
            quot = int2imm( 0 );
        } else {
            MP_INT mpiResult;
            mpz_init_set( &mpiResult, &thempi );
            mpz_abs( &mpiResult, &mpiResult );
            mpz_sub_ui( &mpiResult, &mpiResult, -intC );
            rem = uiNormalizeMPI( mpiResult );
            quot = int2imm( -mpz_sgn( &thempi ) );
        }
    } else {
        MP_INT q;
        MP_INT dummy;
        mpz_init( &q ); mpz_init( &dummy );
        if ( intC > 0 ) {
            rem = int2imm( mpz_fdiv_qr_ui( &q, &dummy, &thempi, intC ) );
            quot = normalizeMPI( q );
        } else {
            rem = int2imm( mpz_fdiv_qr_ui( &q, &dummy, &thempi, -intC ) );
            mpz_neg( &q, &q );
            quot = normalizeMPI( q );
        }
        mpz_clear( &dummy );
    }
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
    divremcoeff( c, quot, rem, invert );
    return true;
}
//}}}
