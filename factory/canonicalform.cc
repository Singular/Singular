/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: canonicalform.cc,v 1.20 1997-10-10 10:40:17 schmidt Exp $ */

#include <config.h>

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
#if defined (USE_MEMUTIL) && ! defined (USE_OLD_MEMMAN)
#include "memman.h"
#endif

#ifndef NOSTREAMIO
CanonicalForm readCF( istream& );
#endif /* NOSTREAMIO */

//{{{ initialization
int initializeGMP();
int initializeCharacteristic();
#ifdef SINGULAR
int mmInit(void);
#endif

int
initCanonicalForm( void )
{
    static bool initialized = false;
    if ( ! initialized ) {
#if (defined (USE_MEMUTIL) && ! defined (USE_OLD_MEMMAN)) || defined (SINGULAR)
	(void)mmInit();
#endif

	(void)initializeCharacteristic();
	(void)initializeGMP();
	initPT();
	initialized = true;
    }
    return 1;
}
//}}}

//{{{ constructors, destructors, selectors
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

CanonicalForm
CanonicalForm::deepCopy() const
{
    if ( is_imm( value ) )
	return *this;
    else
	return CanonicalForm( value->deepCopyObject() );
}
//}}}

//{{{ predicates
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

bool
CanonicalForm::isFFinGF() const
{
    return is_imm( value ) == GFMARK && gf_isff( imm2int( value ) );
}

bool
CanonicalForm::isUnivariate() const
{
    if ( is_imm( value ) )
	return false;
    else
	return value->isUnivariate();
}
//}}}

//{{{ conversion functions
int
CanonicalForm::intval() const
{
    if ( is_imm( value ) )
	return imm_intval( value );
    else
	return value->intval();
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
	    return CanonicalForm( int2imm_gf( gf_int2gf( imm2int( value ) ) ) );
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
//}}}

//{{{ CanonicalForm CanonicalForm::lc (), Lc (), LC (), LC ( v ) const
//{{{ docu
//
// lc(), Lc(), LC() - leading coefficient functions.
//
// All methods return CO if CO is in a base domain.
//
// lc() returns the leading coefficient of CO with respect to
// lexicographic ordering.  Elements in an algebraic extension
// are considered polynomials so lc() always returns a leading
// coefficient in a base domain.  This method is useful to get
// the base domain over which CO is defined.
//
// Lc() returns the leading coefficient of CO with respect to
// lexicographic ordering.  In contrast to lc() elements in an
// algebraic extension are considered coefficients so Lc() always
// returns a leading coefficient in a coefficient domain.
// 
// LC() returns the leading coefficient of CO where CO is
// considered a univariate polynomial in its main variable.  An
// element of an algebraic extension is considered an univariate
// polynomial, too.
//
// LC( v ) returns the leading coefficient of CO where CO is
// considered an univariate polynomial in the polynomial variable
// v.
// Note: If v is less than the main variable of CO we have to
// swap variables which may be quite expensive.
//
// Examples:
// Let x < y be polynomial variables, a an algebraic variable.
//
// (3*a*x*y^2+y+x).lc() = 3
// (3*a*x*y^2+y+x).Lc() = 3*a
// (3*a*x*y^2+y+x).LC() = 3*a*x
// (3*a*x*y^2+y+x).LC( x ) = 3*a*y^2+1
//
// (3*a^2+4*a).lc() = 3
// (3*a^2+4*a).Lc() = 3*a^2+4*a
// (3*a^2+4*a).LC() = 3
// (3*a^2+4*a).LC( x ) = 3*a^2+4*a
//
// See also: InternalCF::lc(), InternalCF::Lc(), InternalCF::LC(),
// InternalPoly::lc(), InternalPoly::Lc(), InternalPoly::LC(),
// ::lc(), ::Lc(), ::LC(), ::LC( v )
//
//}}}
CanonicalForm
CanonicalForm::lc () const
{
    if ( is_imm( value ) )
	return *this;
    else
	return value->lc();
}

CanonicalForm
CanonicalForm::Lc () const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
	return *this;
    else
	return value->Lc();
}

CanonicalForm
CanonicalForm::LC () const
{
    if ( is_imm( value ) )
	return *this;
    else
	return value->LC();
}

CanonicalForm
CanonicalForm::LC ( const Variable & v ) const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
	return *this;

    Variable x = value->variable();
    if ( v > x )
	return *this;
    else if ( v == x )
	return value->LC();
    else {
	CanonicalForm f = swapvar( *this, v, x );
 	if ( f.mvar() == x )
 	    return swapvar( f.value->LC(), v, x );
 	else
	    // v did not occur in f
	    return *this;
    }
}
//}}}

//{{{ int CanonicalForm::degree (), degree ( v ) const
//{{{ docu
//
// degree() - degree methods.
//
// Both methods returns -1 for the zero polynomial and 0 if
// CO is in a base domain.
//
// degree() returns the degree of CO in its main variable.
// Elements in an algebraic extension are considered polynomials.
// degree( v ) returns the degree of CO with respect to v.
// Elements in an algebraic extension are considered polynomials,
// and v may be algebraic.
//
// See also: InternalCf::degree(), InternalPoly::degree(),
// ::degree(), ::degree( v )
//
//}}}
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
    else  if ( is_imm( value ) || value->inBaseDomain() )
	return 0;

    Variable x = value->variable();
    if ( v == x )
	return value->degree();
    else  if ( v > x )
	// relatively to v, f is in a coefficient ring
	return 0;
    else {
	int coeffdeg, result = 0;
	// search for maximum of coefficient degree
	for ( CFIterator i = *this; i.hasTerms(); i++ ) {
	    coeffdeg = i.coeff().degree( v );
	    if ( coeffdeg > result )
		result = coeffdeg;
	}
	return result;
    }
}
//}}}

//{{{ CanonicalForm CanonicalForm::tailcoeff (), int CanonicalForm::taildegree () const
//{{{ docu
//
// tailcoeff(), taildegree() - return last coefficient and
//   degree, resp.
//
// tailcoeff() returns the coefficient of the term with the least
// degree in CO.  Elements in an algebraic extension are
// considered coefficients.
//
// taildegree() returns -1 for the zero polynomial, 0 if CO is in
// a base domain, the least degree of all terms occuring in CO
// otherwise.  In contrast to tailcoeff(), elements in an
// algebraic extension are considered polynomials, not
// coefficients, and such may have a taildegree larger than
// zero.
//
// See also: InternalCF::tailcoeff(), InternalCF::tailcoeff(),
// InternalPoly::tailcoeff(), InternalPoly::taildegree,
// ::tailcoeff(), ::taildegree()
//
//}}}
CanonicalForm
CanonicalForm::tailcoeff () const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
	return *this;
    else
	return value->tailcoeff();
}

int
CanonicalForm::taildegree () const
{
    if ( isZero() )
	return -1;
    else  if ( is_imm( value ) )
	return 0;
    else
	return value->taildegree();
}
//}}}

//{{{ int CanonicalForm::level (), Variable CanonicalForm::mvar () const
//{{{ docu
//
// level(), mvar() - return level and main variable of CO.
//
// level() returns the level of CO.  For a list of the levels,
// see cf_defs.h.
//
// mvar() returns the main variable of CO or Variable() if CO is
// in a base domain.
//
// See also: InternalCF::level(), InternalCF::variable(),
// InternalPoly::level(), InternalPoly::variable(), ::level(),
// ::mvar()
// 
//}}}
int
CanonicalForm::level () const
{
    if ( is_imm( value ) )
	return LEVELBASE;
    else
	return value->level();
}

Variable
CanonicalForm::mvar () const
{
    if ( is_imm( value ) || value->inBaseDomain() )
	return Variable();
    else
	return value->variable();
}
//}}}

//{{{ CanonicalForm CanonicalForm::num (), den () const
//{{{ docu
//
// num(), den() - return numinator and denominator of CO.
//
// num() returns the numinator of CO if CO is a rational number,
// CO itself otherwise.
//
// den() returns the denominator of CO if CO is a rational
// number, 1 (from the current domain!) otherwise.
//
// See also: InternalCF::num(), InternalCF::den(),
// InternalRational::num(), InternalRational::den(), ::num(),
// ::den()
//
//}}}
CanonicalForm
CanonicalForm::num () const
{
    if ( is_imm( value ) )
	return *this;
    else
	return CanonicalForm( value->num() );
}

CanonicalForm
CanonicalForm::den () const
{
    if ( is_imm( value ) )
	return 1;
    else
	return CanonicalForm( value->den() );
}
//}}}

//{{{ assignment operators
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
//}}}

//{{{ evaluation operators
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
//}}}

//{{{ CanonicalForm CanonicalForm::operator[] ( int i ) const
CanonicalForm
CanonicalForm::operator[] ( int i ) const
{
    return value->coeff( i );
}
//}}}

//{{{ CanonicalForm CanonicalForm::deriv (), deriv ( x )
//{{{ docu
//
// deriv() - return the formal derivation of CO.
//
// deriv() derives CO with respect to its main variable.  Returns
// zero if f is in a coefficient domain.
//
// deriv( x ) derives CO with respect to x.  x should be a
// polynomial variable.  Returns zero if f is in a coefficient
// domain.
//
// See also: ::deriv()
//
//}}}
CanonicalForm
CanonicalForm::deriv () const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
	return 0;
    else {
	CanonicalForm result = 0;
	Variable x = value->variable();
	for ( CFIterator i = *this; i.hasTerms(); i++ )
	    if ( i.exp() > 0 )
		result += power( x, i.exp()-1 ) * i.coeff() * i.exp();
	return result;
    }
}

CanonicalForm
CanonicalForm::deriv ( const Variable & x ) const
{
    ASSERT( x.level() > 0, "cannot derive with respect to algebraic variables" );
    if ( is_imm( value ) || value->inCoeffDomain() )
	return 0;

    Variable y = value->variable();
    if ( x > y )
	return 0;
    else if ( x == y )
	return deriv();
    else {
	CanonicalForm result = 0;
	for ( CFIterator i = *this; i.hasTerms(); i++ )
	    result += i.coeff().deriv( x ) * power( y, i.exp() );
	return result;
    }
}
//}}}

//{{{ int CanonicalForm::sign () const
//{{{ docu
//
// sign() - return sign of CO.
//
// If CO is an integer or a rational number, the sign is defined
// as usual.  If CO is an element of a prime power domain or in a
// finite field and SW_SYMMETRIC_FF is on the sign of CO is the
// sign of the symmetric representation of CO.  If CO is in GF(q)
// or in a finite field and SW_SYMMETRIC_FF is off, the sign of
// CO is zero iff CO is zero, otherwise the sign is one.
//
// If CO is a polynomial or in an extension of one of the base
// domains, the sign of CO is the sign of its leading
// coefficient.
//
// See also: InternalCF::sign(), InternalInteger::sign(),
// InternalPrimePower::sign(), InternalRational::sign(),
// InternalPoly::sign(), imm_sign(), gf_sign()
//
//}}}
int
CanonicalForm::sign () const
{
    if ( is_imm( value ) )
	return imm_sign( value );
    else
	return value->sign();
}
//}}}

//{{{ CanonicalForm CanonicalForm::sqrt () const
//{{{ docu
//
// sqrt() - calculate integer square root.
//
// CO has to be an integer greater or equal zero.  Returns the
// largest integer less or equal sqrt(CO).
//
// In the immediate case, we use the newton method to find the
// root.  The algorithm is from H. Cohen - 'A Course in
// Computational Algebraic Number Theory', ch. 1.7.1.
//
// See also: InternalCF::sqrt(), InternalInteger::sqrt(), ::sqrt()
//
//}}}
CanonicalForm
CanonicalForm::sqrt () const
{
    if ( is_imm( value ) ) {
	ASSERT( is_imm( value ) == INTMARK, "sqrt() not implemented" );
	int n = imm2int( value );
	ASSERT( n >= 0, "arg to sqrt() less than zero" );
	if ( n == 0 || n == 1 )
	    return CanonicalForm( CFFactory::basic( n ) );
	else {
	    int x, y = n;
	    do {
		x = y;
		// the intermediate result may not fit into an
		// integer, but the result does
		y = (unsigned int)(x + n/x)/2;
	    } while ( y < x );
	    return CanonicalForm( CFFactory::basic( x ) );
	}
    }
    else
	return CanonicalForm( value->sqrt() );
}
//}}}

//{{{ int CanonicalForm::ilog2 () const
//{{{ docu
//
// ilog2() - integer logarithm to base 2.
//
// Returns the largest integer less or equal logarithm of CO to
// base 2.  CO should be a positive integer.
//
// See also: InternalCF::ilog2(), InternalInteger::ilog2(), ::ilog2()
//
//}}}
int
CanonicalForm::ilog2 () const
{
    if ( is_imm( value ) ) {
	ASSERT( is_imm( value ) == INTMARK, "ilog2() not implemented" );
	int a = imm2int( value );
	ASSERT( a > 0, "arg to ilog2() less or equal zero" );
	int n = -1;
	while ( a != 0 ) {
	    n++;
	    a /= 2;
	}
	return n;
    }
    else
	return value->ilog2();
}
//}}}

//{{{ CanonicalForm CanonicalForm::gcd( const CanonicalForm & ) const
CanonicalForm
CanonicalForm::gcd( const CanonicalForm & ) const
{
//    return ::gcd( *this, f );
    return 1;
}
//}}}

//{{{ comparison operators
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
//}}}

//{{{ arithmetic operators
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
//}}}

//{{{ input/output
#ifndef NOSTREAMIO
void
CanonicalForm::print( ostream & os, char * str ) const
{
    if ( is_imm( value ) )
	imm_print( os, value, str );
    else
	value->print( os, str );
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
#ifndef SINGULAR
    cf = readCF( is );
    return is;
#else /* SINGULAR */
    return 0;
#endif /* SINGULAR */
}
#endif /* NOSTREAMIO */
//}}}

//{{{ genCoeff(), genOne(), genZero()
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
//}}}

//{{{ exponentiation
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
//}}}

//{{{ switches
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
//}}}
