// emacs editmode for this file is -*- C++ -*-
// $Id: canonicalform.cc,v 1.2 1996-07-02 11:18:32 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.1  1996/06/13 07:15:50  stobbe
"CanonicalForm::deriv(x): bug fix, result is now swapped back if x is not
                         the main variable of *this
"

Revision 1.0  1996/05/17 10:59:42  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "cf_globals.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "int_cf.h"
#include "cf_factory.h"
#include "imm.h"
#include "gfops.h"
#include "cf_binom.h"
#if defined USE_MEMUTIL && ! defined USE_OLD_MEMMAN
#include "memman.h"
#endif

CanonicalForm readCF( istream& );


CanonicalForm::CanonicalForm() : value( CFFactory::basic( (int)0 ) )
{
}

CanonicalForm::CanonicalForm( const int i ) : value( CFFactory::basic( i ) )
{
}

CanonicalForm::CanonicalForm( const CanonicalForm & cf ) : value( is_imm( cf.value ) ? cf.value : cf.value->copyObject() )
{
}

CanonicalForm::CanonicalForm( InternalCF * cf ) : value( cf )
{
}

CanonicalForm::CanonicalForm( const Variable & v ) : value( CFFactory::poly( v ) )
{
}

CanonicalForm::CanonicalForm( const Variable & v, int e ) : value( CFFactory::poly( v, e ) )
{
}

CanonicalForm::CanonicalForm( const char * str ) : value( CFFactory::basic( str ) )
{
}

CanonicalForm::~CanonicalForm()
{
    if ( (! is_imm( value )) && value->deleteObject() )
	delete value;
}

InternalCF*
CanonicalForm::getval() const
{
    if ( is_imm( value ) )
	return value;
    else
	return value->copyObject();
}

bool
CanonicalForm::isOne() const
{
    if ( is_imm( value ) == FFMARK )
	return imm_isone_p( value );
    else  if ( is_imm( value ) == GFMARK )
	return imm_isone_gf( value );
    else  if ( is_imm( value ) )
	return imm_isone( value );
    else
	return value->isOne();
}

bool
CanonicalForm::isZero() const
{
    if ( is_imm( value ) == FFMARK )
	return imm_iszero_p( value );
    else  if ( is_imm( value ) == GFMARK )
	return imm_iszero_gf( value );
    else  if ( is_imm( value ) )
	return imm_iszero( value );
    else
	return value->isZero();
}

bool
CanonicalForm::isImm() const
{
    return is_imm( value );
}

bool
CanonicalForm::inZ() const
{
    if ( is_imm( value ) == INTMARK )
	return true;
    else if ( is_imm( value ) )
	return false;
    else
	return value->levelcoeff() == IntegerDomain;
}

bool
CanonicalForm::inQ() const
{
    if ( is_imm( value ) == INTMARK )
	return true;
    else if ( is_imm( value ) )
	return false;
    else
	return value->levelcoeff() == IntegerDomain ||
	    value->levelcoeff() == RationalDomain;
}

bool
CanonicalForm::inFF() const
{
    return is_imm( value ) == FFMARK;
}

bool
CanonicalForm::inGF() const
{
    return is_imm( value ) == GFMARK;
}

bool
CanonicalForm::inPP() const
{
    return ! is_imm( value ) && ( value->levelcoeff() == PrimePowerDomain );
}

bool
CanonicalForm::inBaseDomain() const
{
    if ( is_imm( value ) )
	return true;
    else
	return value->inBaseDomain();
}

bool
CanonicalForm::inExtension() const
{
    if ( is_imm( value ) )
	return false;
    else
	return value->inExtension();
}

bool
CanonicalForm::inCoeffDomain() const
{
    if ( is_imm( value ) )
	return true;
    else
	return value->inCoeffDomain();
}

bool
CanonicalForm::inPolyDomain() const
{
    if ( is_imm( value ) )
	return false;
    else
	return value->inPolyDomain();
}

bool
CanonicalForm::inQuotDomain() const
{
    if ( is_imm( value ) )
	return false;
    else
	return value->inQuotDomain();
}

int
CanonicalForm::intval() const
{
    if ( is_imm( value ) )
	return imm_intval( value );
    else
	return value->intval();
}

CanonicalForm
CanonicalForm::lc() const
{
    if ( is_imm( value ) )
	return *this;
    else
	return value->lc();
}

CanonicalForm
CanonicalForm::LC() const
{
    if ( inBaseDomain() )
	return *this;
    else
	return value->LC();
}

CanonicalForm
CanonicalForm::LC( const Variable & v ) const
{
    if ( inBaseDomain() )
	return *this;
    else  if ( v == mvar() )
	return value->LC();
    else {
	CanonicalForm f = swapvar( *this, v, mvar() );
 	if ( f.mvar() == mvar() )
 	    return swapvar( f.value->LC(), v, mvar() );
 	else
	    return *this;
    }
}

int
CanonicalForm::degree() const
{
    if ( isZero() )
	return -1;
    else  if ( is_imm( value ) )
	return 0;
    else
	return value->degree();
}

int
CanonicalForm::degree( const Variable & v ) const
{
    if ( isZero() )
	return -1;
    else  if ( is_imm( value ) )
	return 0;
    else  if ( inBaseDomain() )
	return 0;
    else  if ( v == mvar() )
	return value->degree();
    else  if ( v > mvar() )
	return 0;
    else {
	CanonicalForm f = swapvar( *this, v, mvar() );
 	if ( f.mvar() == mvar() )
 	    return f.value->degree();
 	else
	    return 0;
    }
}

int
CanonicalForm::taildegree() const
{
    if ( isZero() )
	return -1;
    else  if ( is_imm( value ) )
	return 0;
    else
	return value->taildegree();
}

CanonicalForm
CanonicalForm::tailcoeff() const
{
    if ( inCoeffDomain() )
	return *this;
    else
	return value->tailcoeff();
}

int
CanonicalForm::level() const
{
    if ( is_imm( value ) )
	return LEVELBASE;
    else
	return value->level();
}

Variable
CanonicalForm::mvar() const
{
    if ( is_imm( value ) || value->inBaseDomain() )
	return Variable();
    else
	return value->variable();
}

CanonicalForm
CanonicalForm::num() const
{
    if ( is_imm( value ) )
	return *this;
    else
	return CanonicalForm( value->num() );
}

CanonicalForm
CanonicalForm::den() const
{
    if ( is_imm( value ) )
	return 1;
    else
	return CanonicalForm( value->den() );
}

CanonicalForm
CanonicalForm::deepCopy() const
{
    if ( is_imm( value ) )
	return *this;
    else
	return CanonicalForm( value->deepCopyObject() );
}

CanonicalForm
CanonicalForm::gcd( const CanonicalForm & ) const
{
//    return ::gcd( *this, f );
    return 1;
}

CanonicalForm
CanonicalForm::deriv() const
{
    if ( inCoeffDomain() )
	return 0;
    else {
	CanonicalForm res = 0;
	Variable x = mvar();
	for ( CFIterator i = *this; i.hasTerms(); i++ )
	    if ( i.exp() > 0 )
		res += power( x, i.exp()-1 ) * i.coeff() * i.exp();
	return res;
    }
}

CanonicalForm
CanonicalForm::deriv( const Variable & x ) const
{
    if ( inCoeffDomain() )
	return 0;
    else {
	CanonicalForm res = 0;
	Variable y = mvar();
	for ( CFIterator i = (y==x) ? *this : swapvar( *this, x, y ); i.hasTerms(); i++ )
	    if ( i.exp() > 0 )
		res += power( y, i.exp()-1 ) * i.coeff() * i.exp();
	return (y==x) ? res : swapvar( res, x, y );
    }
}

CanonicalForm
CanonicalForm::genCoeff( int type, int i )
{
    return CanonicalForm( CFFactory::basic( type, i ) );
}

CanonicalForm
CanonicalForm::genZero() const
{
    int what = is_imm( value );
    if ( what == FFMARK )
	return CanonicalForm( CFFactory::basic( FiniteFieldDomain, 0 ) );
    else  if ( what == GFMARK )
	return CanonicalForm( CFFactory::basic( GaloisFieldDomain, 0 ) );
    else  if ( what )
	return CanonicalForm( CFFactory::basic( IntegerDomain, 0 ) );
    else
	return CanonicalForm( value->genZero() );
}

CanonicalForm
CanonicalForm::genOne() const
{
    int what = is_imm( value );
    if ( what == FFMARK )
	return CanonicalForm( CFFactory::basic( FiniteFieldDomain, 1 ) );
    else  if ( what == GFMARK )
	return CanonicalForm( CFFactory::basic( GaloisFieldDomain, 1 ) );
    else  if ( what )
	return CanonicalForm( CFFactory::basic( IntegerDomain, 1 ) );
    else
	return CanonicalForm( value->genOne() );
}

bool
CanonicalForm::isUnivariate() const
{
    if ( is_imm( value ) )
	return false;
    else
	return value->isUnivariate();
}

void
CanonicalForm::print( ostream & os, char * str ) const
{
    if ( is_imm( value ) )
	imm_print( os, value, str );
    else
	value->print( os, str );
}

bool
operator == ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    int what = is_imm( lhs.value );
    if ( what )
	if ( what == is_imm( rhs.value ) )
	    return lhs.value == rhs.value;
	else
	    return false;
    else  if ( is_imm( rhs.value ) )
	return false;
    else  if ( lhs.level() == rhs.level() )
	if ( lhs.value->levelcoeff() >= rhs.value->levelcoeff() )
	    return lhs.value->comparesame( rhs.value ) == 0;
	else
	    return rhs.value->comparesame( lhs.value ) == 0;
    else
	return false;
}

bool
operator != ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    int what = is_imm( lhs.value );
    if ( what )
	if ( what == is_imm( rhs.value ) )
	    return lhs.value != rhs.value;
	else
	    return true;
    else  if ( is_imm( rhs.value ) )
	return true;
    else  if ( lhs.level() == rhs.level() )
	if ( lhs.value->levelcoeff() >= rhs.value->levelcoeff() )
	    return lhs.value->comparesame( rhs.value ) != 0;
	else
	    return rhs.value->comparesame( lhs.value ) != 0;
    else
	return true;
}

bool
operator > ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    int what = is_imm( lhs.value );
    if ( what ) {
	ASSERT ( ! is_imm( rhs.value ) || (what==is_imm( rhs.value )), "illegal base coefficients" );
	if ( what == INTMARK ) {
	    if ( what == is_imm( rhs.value ) )
		return imm_cmp( lhs.value, rhs.value ) > 0;
	    else
		return rhs.value->comparecoeff( lhs.value ) < 0;
	}
	else  if ( what == FFMARK ) {
	    if ( what == is_imm( rhs.value ) )
		return imm_cmp_p( lhs.value, rhs.value ) > 0;
	    else
		return rhs.value->comparecoeff( lhs.value ) < 0;
	}
	else {
	    if ( what == is_imm( rhs.value ) )
		return imm_cmp_gf( lhs.value, rhs.value ) > 0;
	    else
		return rhs.value->comparecoeff( lhs.value ) < 0;
	}
    }
    else  if ( is_imm( rhs.value ) )
	return lhs.value->comparecoeff( rhs.value ) > 0;
    else  if ( lhs.level() == rhs.level() )
	if ( lhs.value->levelcoeff() == rhs.value->levelcoeff() )
	    return lhs.value->comparesame( rhs.value ) > 0;
	else  if ( lhs.value->levelcoeff() > rhs.value->levelcoeff() )
	    return lhs.value->comparecoeff( lhs.value ) > 0;
	else
	    return rhs.value->comparecoeff( lhs.value ) < 0;
    else
	return lhs.value->level() > rhs.value->level();
}

bool
operator < ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    int what = is_imm( lhs.value );
    if ( what ) {
	ASSERT ( ! is_imm( rhs.value ) || (what==is_imm( rhs.value )), "illegal base coefficients" );
	if ( what == INTMARK ) {
	    if ( what == is_imm( rhs.value ) )
		return imm_cmp( lhs.value, rhs.value ) < 0;
	    else
		return rhs.value->comparecoeff( lhs.value ) > 0;
	}
	else  if ( what == FFMARK ) {
	    if ( what == is_imm( rhs.value ) )
		return imm_cmp( lhs.value, rhs.value ) < 0;
	    else
		return rhs.value->comparecoeff( lhs.value ) > 0;
	}
	else {
	    if ( what == is_imm( rhs.value ) )
		return imm_cmp( lhs.value, rhs.value ) < 0;
	    else
		return rhs.value->comparecoeff( lhs.value ) > 0;
	}
    }
    else  if ( is_imm( rhs.value ) )
	return lhs.value->comparecoeff( rhs.value ) < 0;
    else  if ( lhs.level() == rhs.level() )
	if ( lhs.value->levelcoeff() == rhs.value->levelcoeff() )
	    return lhs.value->comparesame( rhs.value ) < 0;
	else  if ( lhs.value->levelcoeff() > rhs.value->levelcoeff() )
	    return lhs.value->comparecoeff( lhs.value ) < 0;
	else
	    return rhs.value->comparecoeff( lhs.value ) > 0;
    else
	return lhs.value->level() < rhs.value->level();
}

CanonicalForm&
CanonicalForm::operator = ( const CanonicalForm & cf )
{
    if ( this != &cf ) {
	if ( (! is_imm( value )) && value->deleteObject() )
	    delete value;
	value = (is_imm( cf.value )) ? cf.value : cf.value->copyObject();
    }
    return *this;
}

CanonicalForm&
CanonicalForm::operator = ( const int cf )
{
    if ( (! is_imm( value )) && value->deleteObject() )
	delete value;
    value = CFFactory::basic( cf );
    return *this;
}

CanonicalForm&
CanonicalForm::operator += ( const CanonicalForm & cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_add_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_add_gf( value, cf.value );
	else  if ( what )
	    value = imm_add( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->addcoeff( value );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->addcoeff( cf.value );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->addsame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->addcoeff( cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->addcoeff( value );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->addcoeff( cf.value );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->addcoeff( value );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm&
CanonicalForm::operator -= ( const CanonicalForm & cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_sub_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_sub_gf( value, cf.value );
	else  if ( what )
	    value = imm_sub( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->subcoeff( value, true );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->subcoeff( cf.value, false );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->subsame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->subcoeff( cf.value, false );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->subcoeff( value, true );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->subcoeff( cf.value, false );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->subcoeff( value, true );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm&
CanonicalForm::operator *= ( const CanonicalForm & cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_mul_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_mul_gf( value, cf.value );
	else  if ( what )
	    value = imm_mul( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->mulcoeff( value );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->mulcoeff( cf.value );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->mulsame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->mulcoeff( cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->mulcoeff( value );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->mulcoeff( cf.value );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->mulcoeff( value );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm&
CanonicalForm::operator /= ( const CanonicalForm & cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_div_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_div_gf( value, cf.value );
	else  if ( what )
	    value = imm_divrat( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->dividecoeff( value, true );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->dividecoeff( cf.value, false );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->dividesame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->dividecoeff( cf.value, false );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->dividecoeff( value, true );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->dividecoeff( cf.value, false );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->dividecoeff( value, true );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm&
CanonicalForm::div ( const CanonicalForm & cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_div_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_div_gf( value, cf.value );
	else  if ( what )
	    value = imm_div( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->divcoeff( value, true );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->divcoeff( cf.value, false );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->divsame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->divcoeff( cf.value, false );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->divcoeff( value, true );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->divcoeff( cf.value, false );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->divcoeff( value, true );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm&
CanonicalForm::operator %= ( const CanonicalForm& cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_mod_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_mod_gf( value, cf.value );
	else  if ( what )
	    value = imm_mod( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->modulocoeff( value, true );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->modulocoeff( cf.value, false );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->modulosame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->modulocoeff( cf.value, false );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->modulocoeff( value, true );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->modulocoeff( cf.value, false );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->modulocoeff( value, true );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm&
CanonicalForm::mod( const CanonicalForm & cf )
{
    int what = is_imm( value );
    if ( what ) {
	ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
	if ( (what = is_imm( cf.value )) == FFMARK )
	    value = imm_mod_p( value, cf.value );
	else  if ( what == GFMARK )
	    value = imm_mod_gf( value, cf.value );
	else  if ( what )
	    value = imm_mod( value, cf.value );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    value = dummy->modcoeff( value, true );
	}
    }
    else  if ( is_imm( cf.value ) )
	value = value->modcoeff( cf.value, false );
    else  if ( value->level() == cf.value->level() ) {
	if ( value->levelcoeff() == cf.value->levelcoeff() )
	    value = value->modsame( cf.value );
	else  if ( value->levelcoeff() > cf.value->levelcoeff() )
	    value = value->modcoeff( cf.value, false );
	else {
	    InternalCF * dummy = cf.value->copyObject();
	    dummy = dummy->modcoeff( value, true );
	    if ( value->deleteObject() ) delete value;
	    value = dummy;
	}
    }
    else  if ( level() > cf.level() )
	value = value->modcoeff( cf.value, false );
    else {
	InternalCF * dummy = cf.value->copyObject();
	dummy = dummy->modcoeff( value, true );
	if ( value->deleteObject() ) delete value;
	value = dummy;
    }
    return *this;
}

CanonicalForm
operator - ( const CanonicalForm & cf )
{
    CanonicalForm result( cf );
    int what = is_imm( result.value );
    if ( what == FFMARK )
	result.value = imm_neg_p( result.value );
    else  if ( what == GFMARK )
	result.value = imm_neg_gf( result.value );
    else  if ( what )
	result.value = imm_neg( result.value );
    else
	result.value = result.value->neg();
    return result;
}

CanonicalForm
operator + ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result += c2;
    return result;
}

CanonicalForm
operator - ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result -= c2;
    return result;
}

CanonicalForm
operator * ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result *= c2;
    return result;
}

CanonicalForm
operator / ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result /= c2;
    return result;
}

CanonicalForm
div ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result.div( c2 );
    return result;
}

CanonicalForm
mod ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result.mod( c2 );
    return result;
}

CanonicalForm
operator % ( const CanonicalForm &c1, const CanonicalForm &c2 )
{
    CanonicalForm result( c1 );
    result %= c2;
    return result;
}

void
divrem ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r )
{
    InternalCF * qq = 0, * rr = 0;
    int what = is_imm( f.value );
    if ( what )
	if ( is_imm( g.value ) ) {
	    if ( what == FFMARK )
		imm_divrem_p( f.value, g.value, qq, rr );
	    else  if ( what == GFMARK )
		imm_divrem_gf( f.value, g.value, qq, rr );
	    else
		imm_divrem( f.value, g.value, qq, rr );
	}
	else
	    g.value->divremcoeff( f.value, qq, rr, true );
    else  if ( (what=is_imm( g.value )) )
	f.value->divremcoeff( g.value, qq, rr, false );
    else  if ( f.value->level() == g.value->level() )
	if ( f.value->levelcoeff() == g.value->levelcoeff() )
	    f.value->divremsame( g.value, qq, rr );
	else  if ( f.value->levelcoeff() > g.value->levelcoeff() )
	    f.value->divremcoeff( g.value, qq, rr, false );
	else
	    g.value->divremcoeff( f.value, qq, rr, true );
    else  if ( f.value->level() > g.value->level() )
	f.value->divremcoeff( g.value, qq, rr, false );
    else
	g.value->divremcoeff( f.value, qq, rr, true );
    ASSERT( qq != 0 && rr != 0, "error in divrem" );
    q = CanonicalForm( qq );
    r = CanonicalForm( rr );
}

bool
divremt ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r )
{
    InternalCF * qq = 0, * rr = 0;
    int what = is_imm( f.value );
    bool result = true;
    if ( what )
	if ( is_imm( g.value ) ) {
	    if ( what == FFMARK )
		imm_divrem_p( f.value, g.value, qq, rr );
	    else  if ( what == GFMARK )
		imm_divrem_gf( f.value, g.value, qq, rr );
	    else
		imm_divrem( f.value, g.value, qq, rr );
	}
	else
	    result = g.value->divremcoefft( f.value, qq, rr, true );
    else  if ( (what=is_imm( g.value )) )
	result = f.value->divremcoefft( g.value, qq, rr, false );
    else  if ( f.value->level() == g.value->level() )
	if ( f.value->levelcoeff() == g.value->levelcoeff() )
	    result = f.value->divremsamet( g.value, qq, rr );
	else  if ( f.value->levelcoeff() > g.value->levelcoeff() )
	    result = f.value->divremcoefft( g.value, qq, rr, false );
	else
	    result = g.value->divremcoefft( f.value, qq, rr, true );
    else  if ( f.value->level() > g.value->level() )
	result = f.value->divremcoefft( g.value, qq, rr, false );
    else
	result = g.value->divremcoefft( f.value, qq, rr, true );
    if ( result ) {
	ASSERT( qq != 0 && rr != 0, "error in divrem" );
	q = CanonicalForm( qq );
	r = CanonicalForm( rr );
    }
    else {
	q = 0; r = 0;
    }
    return result;
}

ostream&
operator << ( ostream & os, const CanonicalForm & cf )
{
    cf.print( os, "" );
    return os;
}

istream&
operator >> ( istream & is, CanonicalForm & cf )
{
#ifdef SINGULAR
    cf = 0;
#else
    cf = readCF( is );
#endif
    return is;
}

CanonicalForm
CanonicalForm::operator () ( const CanonicalForm & f ) const
{
    if ( inBaseDomain() )
	return *this;
    else {
	CanonicalForm result = 0;
	for ( CFIterator i = *this; i.hasTerms(); i++ )
	    if ( i.exp() == 0 )
		result += i.coeff();
	    else
		result += power( f, i.exp() ) * i.coeff();
	return result;
    }
}

CanonicalForm
CanonicalForm::operator () ( const CanonicalForm & f, const Variable & v ) const
{
    if ( inBaseDomain() || v > mvar() )
	return *this;
    else  if ( v == mvar() ) {
	CanonicalForm result = 0;
	for ( CFIterator i = *this; i.hasTerms(); i++ )
	    if ( i.exp() == 0 )
		result += i.coeff();
	    else
		result += power( f, i.exp() ) * i.coeff();
	return result;
    }
    else {
	CanonicalForm G = swapvar( *this, v, Variable::highest() );
	if ( G.mvar() != Variable::highest() )
	    return *this;
	CanonicalForm result = 0;
	for ( CFIterator i = G; i.hasTerms(); ++i )
	    if ( i.exp() == 0 )
		result += i.coeff();
	    else
		result += power( f, i.exp() ) * i.coeff();
	return result;
    }
}

CanonicalForm
CanonicalForm::operator[] ( int i ) const
{
    return value->coeff( i );
}

int
CanonicalForm::sign() const
{
    if ( is_imm( value ) )
	return imm_sign( value );
    else
	return value->sign();
}

CanonicalForm
power ( const CanonicalForm & f, int n )
{
    ASSERT( n >= 0, "illegal exponent" );
    if ( f == 0 )
	return 0;
    else  if ( f == 1 )
	return f;
    else  if ( f == -1 ) {
	if ( n % 2 == 0 )
	    return 1;
	else
	    return -1;
    }
    else  if ( n == 0 )
	return 1;
    else {
	CanonicalForm result = f;
	for ( int i = 1; i < n; i++ )
	    result *= f;
	return result;
    }
}

CanonicalForm
power ( const Variable & v, int n )
{
    ASSERT( n >= 0, "illegal exponent" );
    if ( n == 0 )
	return 1;
    else  if ( n == 1 )
	return v;
    else  if ( v.level() < 0 ) {
	CanonicalForm result( v, n-1 );
	return result * v;
    }
    else
	return CanonicalForm( v, n );
}


int initializeGMP();
int initializeCharacteristic();

int
initCanonicalForm( void )
{
    static bool initialized = false;
    if ( ! initialized ) {
#if defined USE_MEMUTIL && ! defined USE_OLD_MEMMAN
	(void)mmInit();
#endif

	Off( SW_RATIONAL );
	Off( SW_QUOTIENT );
	Off( SW_SYMMETRIC_FF );
	Off( SW_BERLEKAMP );
	Off( SW_FAC_USE_BIG_PRIMES );
	Off( SW_FAC_QUADRATICLIFT );
	Off( SW_USE_EZGCD );

	(void)initializeCharacteristic();
	(void)initializeGMP();
	initPT();
	initialized = true;
    }
    return 1;
}


CanonicalForm
CanonicalForm::mapinto () const
{
    ASSERT( is_imm( value ) ||  ! value->inExtension(), "cannot map into different Extension" );
    if ( is_imm( value ) )
	if ( getCharacteristic() == 0 )
	    if ( is_imm( value ) == FFMARK )
		return CanonicalForm( int2imm( ff_symmetric( imm2int( value ) ) ) );
	    else  if ( is_imm( value ) == GFMARK )
		return CanonicalForm( int2imm( ff_symmetric( gf_gf2ff( imm2int( value ) ) ) ) );
	    else
		return *this;
	else  if ( CFFactory::gettype() == PrimePowerDomain )
	    return CanonicalForm( CFFactory::basic( imm2int( value ) ) );
	else  if ( getGFDegree() == 1 )
	    return CanonicalForm( int2imm_p( ff_norm( imm2int( value ) ) ) );
	else
	    return CanonicalForm( int2imm_gf( ff_norm( imm2int( value ) ) ) );
    else  if ( value->inBaseDomain() )
	if ( getCharacteristic() == 0 )
	    if ( value->levelcoeff() == PrimePowerDomain )
		return CFFactory::basic( getmpi( value, true ) );
	    else
		return *this;
	else  if ( CFFactory::gettype() == PrimePowerDomain ) {
	    ASSERT( value->levelcoeff() == PrimePowerDomain || value->levelcoeff() == IntegerDomain, "no proper map defined" );
	    if ( value->levelcoeff() == PrimePowerDomain )
		return *this;
	    else
		return CFFactory::basic( getmpi( value ) );
	}
	else {
	    int val;
	    if ( value->levelcoeff() == IntegerDomain )
		val = value->intmod( ff_prime );
	    else  if ( value->levelcoeff() == RationalDomain )
		return num().mapinto() / den().mapinto();
	    else {
		ASSERT( 0, "illegal domain" );
		return 0;
	    }
	    if ( getGFDegree() > 1 )
		return CanonicalForm( int2imm_gf( gf_int2gf( val ) ) );
	    else
		return CanonicalForm( int2imm_p( val ) );
	}
    else {
	Variable x = value->variable();
	CanonicalForm result;
	for ( CFIterator i = *this; i.hasTerms(); i++ )
	    result += power( x, i.exp() ) * i.coeff().mapinto();
	return result;
    }
}
	
void
On( int sw )
{
    cf_glob_switches.On( sw );
}

void
Off( int sw )
{
    cf_glob_switches.Off( sw );
}

bool
isOn( int sw )
{
    return cf_glob_switches.isOn( sw );
}

bool
CanonicalForm::isFFinGF() const
{
    return is_imm( value ) == GFMARK && gf_isff( imm2int( value ) );
}

static void
fillVarsRec ( const CanonicalForm & f, int * vars )
{
    int n;
    if ( (n = f.level()) > 0 ) {
	vars[n] = 1;
	CFIterator i;
	for ( i = f; i.hasTerms(); ++i )
	    fillVarsRec( i.coeff(), vars );
    }
}

CanonicalForm
CanonicalForm::sqrt ( ) const
{
    if ( is_imm( value ) ) {
	ASSERT( is_imm( value ) == INTMARK, "not implemented" );
	int a = imm2int( value );
	if ( a == 1 )
	    return CanonicalForm( CFFactory::basic( 1 ) );
	else {
	    int h, x0, x1 = a;
	    do {
		x0 = x1;
		h = x0 * x0 + a - 1;
		if ( h % (2 * x0) == 0 )
		    x1 = h / (2 * x0);
		else
		    x1 = (h - 1)  / (2 * x0);
	    } while ( x1 < x0 );
	    return CanonicalForm( CFFactory::basic( x1 ) );
	}
    }
    else
	return CanonicalForm( value->sqrt() );
}
	

int
getNumVars( const CanonicalForm & f )
{
    int n;
    if ( f.inCoeffDomain() )
	return 0;
    else  if ( (n = f.level()) == 1 )
	return 1;
    else {
	int * vars = new int[ n+1 ];
	int i;
	for ( i = 0; i < n; i++ ) vars[i] = 0;
	for ( CFIterator I = f; I.hasTerms(); ++I )
	    fillVarsRec( I.coeff(), vars );
	int m = 0;
	for ( i = 1; i < n; i++ )
	    if ( vars[i] != 0 ) m++;
	delete [] vars;
	return m+1;
    }
}

CanonicalForm
getVars( const CanonicalForm & f )
{
    int n;
    if ( f.inCoeffDomain() )
	return 1;
    else  if ( (n = f.level()) == 1 )
	return Variable( 1 );
    else {
	int * vars = new int[ n+1 ];
	int i;
	for ( i = 0; i <= n; i++ ) vars[i] = 0;
	for ( CFIterator I = f; I.hasTerms(); ++I )
	    fillVarsRec( I.coeff(), vars );
	CanonicalForm result = 1;
	for ( i = n; i > 0; i-- )
	    if ( vars[i] != 0 ) result *= Variable( i );
	delete [] vars;
	return f.mvar() * result;
    }
}

bool
divides ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( g.level() > 0 && g.level() == f.level() )
	if ( divides( f.tailcoeff(), g.tailcoeff() ) && divides( f.LC(), g.LC() ) ) {
	    CanonicalForm q, r;
	    bool ok = divremt( g, f, q, r );
	    return ok && r == 0;
	}
	else
	    return false;
    else {
	CanonicalForm q, r;
	bool ok = divremt( g, f, q, r );
	return ok && r == 0;
    }
}

