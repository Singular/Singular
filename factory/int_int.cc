/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: int_int.cc,v 1.17 2003-10-15 17:19:40 Singular Exp $ */

#include <config.h>

#include "canonicalform.h"
#include "imm.h"
#include "int_int.h"
#include "int_rat.h"
#include "cf_gmp.h"
#include "gmpext.h"

#ifdef HAVE_OMALLOC
const omBin InternalInteger::InternalInteger_bin = omGetSpecBin(sizeof(InternalInteger));
#endif

InternalInteger::InternalInteger()
{
    mpz_init( &thempi );
}

InternalInteger::InternalInteger( const int i )
{
    mpz_init_set_si( &thempi, i );
}

InternalInteger::InternalInteger( const MP_INT & mpi) : thempi( mpi ) {}

InternalInteger::InternalInteger( const char * str, const int base )
{
    mpz_init_set_str( &thempi, str, base );
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

//{{{ InternalCF * InternalInteger::neg ()
// docu: see CanonicalForm::operator -()
InternalCF *
InternalInteger::neg ()
{
    if ( getRefCount() > 1 ) {
        decRefCount();
        MP_INT dummy;
        mpz_init_set( &dummy, &thempi );
        mpz_neg( &dummy, &dummy );
        return new InternalInteger( dummy );
    } else {
        mpz_neg( &thempi, &thempi );
        return this;
    }
}
//}}}


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

//{{{ InternalCF * InternalInteger::bgcdsame, bgcdcoeff ( const InternalCF * const c )
// docu: see CanonicalForm::bgcd()
InternalCF *
InternalInteger::bgcdsame ( const InternalCF * const c ) const
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain, "incompatible base coefficients" );

    // simply return 1 if we are calculating over the rationals
    if ( cf_glob_switches.isOn( SW_RATIONAL ) )
         return int2imm( 1 );

    // calculate gcd
    MP_INT result;
    mpz_init( &result );
    mpz_gcd( &result, &thempi, &MPI( c ) );
    mpz_abs( &result, &result );

    // check for immediate result
    if ( mpz_is_imm( &result ) ) {
        InternalCF * res = int2imm( mpz_get_si( &result ) );
        mpz_clear( &result );
        return res;
    }
    else
        return new InternalInteger( result );
}

InternalCF *
InternalInteger::bgcdcoeff ( const InternalCF * const c )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );

    // simply return 1 if we are calculating over the rationals
    if ( cf_glob_switches.isOn( SW_RATIONAL ) )
         return int2imm( 1 );

    int cInt = imm2int( c );

    // trivial cases
    if ( cInt == 1 || cInt == -1 )
        return int2imm( 1 );
    else if ( cInt == 0 )
        return copyObject();

    // calculate gcd.  We need a positive operand since
    // `mpz_gcd_ui()' operates an unsigned int's only.
    if ( cInt < 0 ) cInt = -cInt;
    MP_INT dummy;
    mpz_init( &dummy );
    // we do not need dummy since we know that cInt != 0
    cInt = mpz_gcd_ui( &dummy, &thempi, cInt );
    mpz_clear( &dummy );
    if ( cInt < 0 ) cInt = -cInt;
    return int2imm( cInt );
}
//}}}

//{{{ InternalCF * InternalInteger::bextgcdsame( InternalCF * c, CanonicalForm & a, CanonicalForm & b )
InternalCF *
InternalInteger::bextgcdsame( InternalCF * c, CanonicalForm & a, CanonicalForm & b )
{
    ASSERT( ! ::is_imm( c ) && c->levelcoeff() == IntegerDomain, "incompatible base coefficients" );

    // simply return 1 if we are calculating over the rationals
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
        a = 1/CanonicalForm( copyObject() ); b = 0;
        return int2imm( 1 );
    }

    // calculate extended gcd
    MP_INT result;
    MP_INT aMPI;
    MP_INT bMPI;
    mpz_init( &result );
    mpz_init( &aMPI );
    mpz_init( &bMPI );
    mpz_gcdext( &result, &aMPI, &bMPI, &thempi, &MPI( c ) );

    // check and modify signs
    if ( mpz_sgn( &result ) < 0 ) {
        mpz_neg( &result, &result );
        mpz_neg( &aMPI, &aMPI );
        mpz_neg( &bMPI, &bMPI );
    }

    // postconditioning of result
    if ( mpz_is_imm( &aMPI ) ) {
        a = CanonicalForm( int2imm( mpz_get_si( &aMPI ) ) );
        mpz_clear( &aMPI );
    } else
        a = CanonicalForm( new InternalInteger( aMPI ) );
    if ( mpz_is_imm( &bMPI ) ) {
        b = CanonicalForm( int2imm( mpz_get_si( &bMPI ) ) );
        mpz_clear( &bMPI );
    } else
        b = CanonicalForm( new InternalInteger( bMPI ) );
    if ( mpz_is_imm( &result ) ) {
        InternalCF * res = int2imm( mpz_get_si( &result ) );
        mpz_clear( &result );
        return res;
    }
    else
        return new InternalInteger( result );
}

InternalCF *
InternalInteger::bextgcdcoeff( InternalCF * c, CanonicalForm & a, CanonicalForm & b )
{
    ASSERT( ::is_imm( c ) == INTMARK, "incompatible base coefficients" );

    // simply return 1 if we are calculating over the rationals
    if ( cf_glob_switches.isOn( SW_RATIONAL ) ) {
        a = 1/CanonicalForm( copyObject() ); b = 0;
        return int2imm( 1 );
    }

    int cInt = imm2int( c );

    // trivial cases
    if ( cInt == 1 || cInt == -1 ) {
        a = 0; b = cInt;
        return int2imm( 1 );
    } else if ( cInt == 0 ) {
        a = 1; b = 0;
        return copyObject();
    }

    // calculate q and r such that CO = q*cInt + r
    InternalCF * q = 0, * r = 0;
    divremcoeff( c, q, r, false );

    // we do not repeat all the code to calculate the gcd of two
    // immediates.  Note that r is an immediate since c != 0, so
    // we do not have to destroy it.  q is destroyed by the
    // CanonicalForm destructor, hence we do not need to worry
    // about it, either.
    CanonicalForm aPrime, bPrime;
    CanonicalForm result = bextgcd( c, r, aPrime, bPrime );
    a = bPrime;
    b = aPrime - CanonicalForm( q ) * bPrime;

    return result.getval();
}
//}}}

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
