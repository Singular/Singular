/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"
#include "cf_factory.h"

#include "cf_defs.h"
#include "cf_globals.h"
#include "canonicalform.h"
#include "cf_iter.h"
#include "int_cf.h"
#include "cf_algorithm.h"
#include "imm.h"
#include "int_pp.h"
#include "gfops.h"
#include "facMul.h"
#include "facAlgFuncUtil.h"
#include "FLINTconvert.h"
#include "cf_binom.h"

#ifndef NOSTREAMIO
CanonicalForm readCF( ISTREAM& );
#endif /* NOSTREAMIO */

/** constructors, destructors, selectors **/
CanonicalForm::CanonicalForm( const char * str, const int base ) : value( CFFactory::basic( str, base ) )
{
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

void
CanonicalForm::mpzval(mpz_t val) const
{
    ASSERT (!is_imm (value) && value->levelcoeff() == IntegerDomain, "non-immediate integer expected");
    getmpi (value, val);
}


/** predicates **/
#if 0
bool
CanonicalForm::isImm() const
{
    return is_imm( value );
}
#endif

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

// is_homogeneous returns 1 iff f is homogeneous, 0 otherwise//
bool
CanonicalForm::isHomogeneous() const
{
  if (this->isZero()) return true;
  else if (this->inCoeffDomain()) return true;
  else
  {
#if 0
    CFIterator i;
    int cdeg = -2, dummy;
    for ( i = *this; i.hasTerms(); i++ )
    {
      if (!(i.coeff().isHomogeneous())) return false;
      if ( (dummy = totaldegree( i.coeff() ) + i.exp()) != cdeg )
      {
         if (cdeg == -2) cdeg = dummy;
         else return false;
      }
    }
    return true;
#else
    CFList termlist= get_Terms(*this);
    CFListIterator i;
    int deg= totaldegree(termlist.getFirst());

    for ( i=termlist; i.hasItem(); i++ )
      if ( totaldegree(i.getItem()) != deg ) return false;
    return true;
#endif
  }
}



/** conversion functions **/
long
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
    //ASSERT( is_imm( value ) ||  ! value->inExtension(), "cannot map into different Extension" );
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
            #ifndef HAVE_NTL
            if ( value->levelcoeff() == PrimePowerDomain )
            {
              mpz_t d;
              getmpi( value,d);
              if ( mpz_cmp( InternalPrimePower::primepowhalf, d ) < 0 )
                mpz_sub( d, d, InternalPrimePower::primepow );
              return CFFactory::basic( d );
            }
            else
            #endif
                return *this;
        #ifndef HAVE_NTL
        else  if ( CFFactory::gettype() == PrimePowerDomain )
        {
            ASSERT( value->levelcoeff() == PrimePowerDomain || value->levelcoeff() == IntegerDomain, "no proper map defined" );
            if ( value->levelcoeff() == PrimePowerDomain )
                return *this;
            else
            {
              mpz_t d;
              getmpi(value,d);
              if ( mpz_cmp( InternalPrimePower::primepowhalf, d ) < 0 )
                mpz_sub( d, d, InternalPrimePower::primepow );
              return CFFactory::basic( d );
            }
        }
        #endif
        else
        {
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
    else
    {
        Variable x = value->variable();
        CanonicalForm result;
        for ( CFIterator i = *this; i.hasTerms(); i++ )
            result += (power( x, i.exp() ) * i.coeff().mapinto());
        return result;
    }
}
/** CanonicalForm CanonicalForm::lc (), Lc (), LC (), LC ( v ) const
 *
 * lc(), Lc(), LC() - leading coefficient functions.
 *
 * All methods return CO if CO is in a base domain.
 *
 * lc() returns the leading coefficient of CO with respect to
 * lexicographic ordering.  Elements in an algebraic extension
 * are considered polynomials so lc() always returns a leading
 * coefficient in a base domain.  This method is useful to get
 * the base domain over which CO is defined.
 *
 * Lc() returns the leading coefficient of CO with respect to
 * lexicographic ordering.  In contrast to lc() elements in an
 * algebraic extension are considered coefficients so Lc() always
 * returns a leading coefficient in a coefficient domain.
 *
 * LC() returns the leading coefficient of CO where CO is
 * considered a univariate polynomial in its main variable.  An
 * element of an algebraic extension is considered an univariate
 * polynomial, too.
 *
 * LC( v ) returns the leading coefficient of CO where CO is
 * considered an univariate polynomial in the polynomial variable
 * v.
 * Note: If v is less than the main variable of CO we have to
 * swap variables which may be quite expensive.
 *
 * Examples:
 * > Let x < y be polynomial variables, a an algebraic variable.
 *
 * > (3*a*x*y^2+y+x).lc() = 3
 *
 * > (3*a*x*y^2+y+x).Lc() = 3*a
 *
 * > (3*a*x*y^2+y+x).LC() = 3*a*x
 *
 * > (3*a*x*y^2+y+x).LC( x ) = 3*a*y^2+1
 *
 *
 * > (3*a^2+4*a).lc() = 3
 *
 * > (3*a^2+4*a).Lc() = 3*a^2+4*a
 *
 * > (3*a^2+4*a).LC() = 3
 *
 * > (3*a^2+4*a).LC( x ) = 3*a^2+4*a
 *
 * @sa InternalCF::lc(), InternalCF::Lc(), InternalCF::LC(),
 * InternalPoly::lc(), InternalPoly::Lc(), InternalPoly::LC(),
 * ::lc(), ::Lc(), ::LC(), ::LC( v )
 *
**/
CanonicalForm
CanonicalForm::lc () const
{
    if ( is_imm( value ) )
        return *this;
    else
        return value->lc();
}

/**
 * @sa CanonicalForm::lc(), CanonicalForm::LC(), InternalCF::lc(),
 * InternalCF::Lc(), InternalCF::LC(),
 * InternalPoly::lc(), InternalPoly::Lc(), InternalPoly::LC(),
 * ::lc(), ::Lc(), ::LC(), ::LC( v )
**/
CanonicalForm
CanonicalForm::Lc () const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
        return *this;
    else
        return value->Lc();
}

/**
 * @sa CanonicalForm::lc(), CanonicalForm::Lc(), InternalCF::lc(),
 * InternalCF::Lc(), InternalCF::LC(),
 * InternalPoly::lc(), InternalPoly::Lc(), InternalPoly::LC(),
 * ::lc(), ::Lc(), ::LC(), ::LC( v )
**/
CanonicalForm
CanonicalForm::LC () const
{
    if ( is_imm( value ) )
        return *this;
    else
        return value->LC();
}

/**
 * @sa CanonicalForm::lc(), CanonicalForm::Lc(), InternalCF::lc(),
 * InternalCF::Lc(), InternalCF::LC(),
 * InternalPoly::lc(), InternalPoly::Lc(), InternalPoly::LC(),
 * ::lc(), ::Lc(), ::LC(), ::LC( v )
**/
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

/**
 * Returns -1 for the zero polynomial and 0 if
 * CO is in a base domain.
 *
 * degree() returns the degree of CO in its main variable.
 * Elements in an algebraic extension are considered polynomials.
 *
 * @sa InternalCF::degree(), InternalPoly::degree(),
 * ::degree(), ::degree( v )
 *
**/
int
CanonicalForm::degree() const
{
    int what = is_imm( value );
    if ( what )
        if ( what == FFMARK )
            return imm_iszero_p( value ) ? -1 : 0;
        else if ( what == INTMARK )
            return imm_iszero( value ) ? -1 : 0;
        else
            return imm_iszero_gf( value ) ? -1 : 0;
    else
        return value->degree();
}

/**
 * returns -1 for the zero polynomial and 0 if
 * CO is in a base domain.
 *
 * degree( v ) returns the degree of CO with respect to v.
 * Elements in an algebraic extension are considered polynomials,
 * and v may be algebraic.
 *
 * @sa InternalCF::degree(), InternalPoly::degree(),
 * ::degree(), ::degree( v )
**/
int
CanonicalForm::degree( const Variable & v ) const
{
    int what = is_imm( value );
#if 0
    if ( what )
        if ( what == FFMARK )
            return imm_iszero_p( value ) ? -1 : 0;
        else if ( what == INTMARK )
            return imm_iszero( value ) ? -1 : 0;
        else
            return imm_iszero_gf( value ) ? -1 : 0;
    else if ( value->inBaseDomain() )
        return value->degree();
#else
    switch(what)
    {
      case FFMARK: return imm_iszero_p( value ) ? -1 : 0;
      case INTMARK: return imm_iszero( value ) ? -1 : 0;
      case GFMARK:  return imm_iszero_gf( value ) ? -1 : 0;
      case 0: if ( value->inBaseDomain() )
              return value->degree();
              break;
    }
#endif

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

/**
 *
 * tailcoeff() - return least coefficient
 *
 * tailcoeff() returns the coefficient of the term with the least
 * degree in CO where CO is considered an univariate polynomial
 * in its main variable.  Elements in an algebraic extension are
 * considered coefficients.
 *
 * @sa CanonicalForm::taildegree(), InternalCF::tailcoeff(), InternalCF::tailcoeff(),
 * InternalPoly::tailcoeff(), InternalPoly::taildegree,
 * ::tailcoeff(), ::taildegree()
 *
**/
CanonicalForm
CanonicalForm::tailcoeff () const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
        return *this;
    else
        return value->tailcoeff();
}

/**
 * tailcoeff( v ) returns the tail coefficient of CO where CO is
 * considered an univariate polynomial in the polynomial variable
 * v.
 * Note: If v is less than the main variable of CO we have to
 * swap variables which may be quite expensive.
 *
 * @sa CanonicalForm::taildegree(), InternalCF::tailcoeff(), InternalCF::tailcoeff(),
 * InternalPoly::tailcoeff(), InternalPoly::taildegree,
 * ::tailcoeff(), ::taildegree()
**/
CanonicalForm
CanonicalForm::tailcoeff (const Variable& v) const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
        return *this;

    Variable x = value->variable();
    if ( v > x )
        return *this;
    else if ( v == x )
        return value->tailcoeff();
    else {
        CanonicalForm f = swapvar( *this, v, x );
         if ( f.mvar() == x )
             return swapvar( f.value->tailcoeff(), v, x );
         else
            // v did not occur in f
            return *this;
    }
}


/**
 * taildegree() returns -1 for the zero polynomial, 0 if CO is in
 * a base domain, otherwise the least degree of CO where CO is
 * considered a univariate polynomial in its main variable.  In
 * contrast to tailcoeff(), elements in an algebraic extension
 * are considered polynomials, not coefficients, and such may
 * have a taildegree larger than zero.
 *
 * @sa CanonicalForm::tailcoeff(), InternalCF::tailcoeff(), InternalCF::tailcoeff(),
 * InternalPoly::tailcoeff(), InternalPoly::taildegree,
 * ::tailcoeff(), ::taildegree()
**/
int
CanonicalForm::taildegree () const
{
    int what = is_imm( value );
    if ( what )
        if ( what == FFMARK )
            return imm_iszero_p( value ) ? -1 : 0;
        else if ( what == INTMARK )
            return imm_iszero( value ) ? -1 : 0;
        else
            return imm_iszero_gf( value ) ? -1 : 0;
    else
        return value->taildegree();
}

/**
 * level() returns the level of CO.  For a list of the levels and
 * their meanings, see cf_defs.h.
 *
 * @sa InternalCF::level(), InternalCF::variable(),
 * InternalPoly::level(), InternalPoly::variable(), ::level(),
 * ::mvar()
 *
**/
int
CanonicalForm::level () const
{
    if ( is_imm( value ) )
        return LEVELBASE;
    else
        return value->level();
}

/**
 * mvar() returns the main variable of CO or Variable() if CO is
 * in a base domain.
 *
 * @sa InternalCF::level(), InternalCF::variable(),
 * InternalPoly::level(), InternalPoly::variable(), ::level(),
 * ::mvar()
**/
Variable
CanonicalForm::mvar () const
{
    if ( is_imm( value ) )
        return Variable();
    else
        return value->variable();
}

/**
 * num() returns the numerator of CO if CO is a rational number,
 * CO itself otherwise.
 *
 * @sa InternalCF::num(), InternalCF::den(),
 * InternalRational::num(), InternalRational::den(), ::num(),
 * ::den()
 *
**/
CanonicalForm
CanonicalForm::num () const
{
    if ( is_imm( value ) )
        return *this;
    else
        return CanonicalForm( value->num() );
}

/**
 * den() returns the denominator of CO if CO is a rational
 * number, 1 (from the current domain!) otherwise.
 *
 * @sa InternalCF::num(), InternalCF::den(),
 * InternalRational::num(), InternalRational::den(), ::num(),
 * ::den()
**/
CanonicalForm
CanonicalForm::den () const
{
    if ( is_imm( value ) )
        return CanonicalForm( 1 );
    else
        return CanonicalForm( value->den() );
}

/** assignment operators **/
CanonicalForm &
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

CanonicalForm &
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

CanonicalForm &
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
#if (HAVE_NTL && HAVE_FLINT && __FLINT_RELEASE >= 20400)
#if (__FLINT_RELEASE >= 20503)
        int l_this,l_cf,m=1;
        if ((getCharacteristic()>0)
        && (CFFactory::gettype() != GaloisFieldDomain)
        &&(!hasAlgVar(*this))
        &&(!hasAlgVar(cf))
        &&((l_cf=size_maxexp(cf,m))>10)
        &&((l_this=size_maxexp(*this,m))>10)
        )
        {
          *this=mulFlintMP_Zp(*this,l_this,cf,l_cf,m);
        }
        else
        /*-----------------------------------------------------*/
        if ((getCharacteristic()==0)
        &&(!hasAlgVar(*this))
        &&(!hasAlgVar(cf))
        &&((l_cf=size_maxexp(cf,m))>10)
        &&((l_this=size_maxexp(*this,m))>10)
        )
        {
          *this=mulFlintMP_QQ(*this,l_this,cf,l_cf,m);
        }
        else
#endif

        if (value->levelcoeff() == cf.value->levelcoeff() && cf.isUnivariate() && (*this).isUnivariate())
        {
          if (value->level() < 0 || CFFactory::gettype() == GaloisFieldDomain || (size (cf) <= 10 || size (*this) <= 10) )
            value = value->mulsame( cf.value );
          else
            *this= mulNTL (*this, cf);
        }
        else if (value->levelcoeff() == cf.value->levelcoeff() && (!cf.isUnivariate() || !(*this).isUnivariate()))
            value = value->mulsame( cf.value );
#else
        if ( value->levelcoeff() == cf.value->levelcoeff() )
            value = value->mulsame( cf.value );
#endif
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

CanonicalForm &
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
#if (HAVE_NTL && HAVE_FLINT && __FLINT_RELEASE >= 20400)
        if ( value->levelcoeff() == cf.value->levelcoeff() && (*this).isUnivariate() && cf.isUnivariate())
        {
            if (value->level() < 0 || CFFactory::gettype() == GaloisFieldDomain)
              value = value->dividesame( cf.value );
            else
              *this= divNTL (*this, cf);
        }
        else if (value->levelcoeff() == cf.value->levelcoeff() && (!cf.isUnivariate() || !(*this).isUnivariate()))
            value = value->dividesame( cf.value );
#else
        if (value->levelcoeff() == cf.value->levelcoeff() )
            value = value->dividesame( cf.value );
#endif
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

CanonicalForm &
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

///same as divremt but handles zero divisors in case we are in Z_p[x]/(f) where f is not irreducible
CanonicalForm &
CanonicalForm::tryDiv ( const CanonicalForm & cf, const CanonicalForm& M, bool& fail )
{
    ASSERT (getCharacteristic() > 0, "expected positive characteristic");
    ASSERT (!getReduce (M.mvar()), "do not reduce modulo M");
    fail= false;
    int what = is_imm( value );
    if ( what ) {
        ASSERT ( ! is_imm( cf.value ) || (what==is_imm( cf.value )), "illegal base coefficients" );
        if ( (what = is_imm( cf.value )) == FFMARK )
            value = imm_div_p( value, cf.value );
        else  if ( what == GFMARK )
            value = imm_div_gf( value, cf.value );
        else {
            InternalCF * dummy = cf.value->copyObject();
            value = dummy->divcoeff( value, true );
        }
    }
    else  if ( is_imm( cf.value ) )
        value = value->tryDivcoeff (cf.value, false, M, fail);
    else  if ( value->level() == cf.value->level() ) {
        if ( value->levelcoeff() == cf.value->levelcoeff() )
            value = value->tryDivsame( cf.value, M, fail );
        else  if ( value->levelcoeff() > cf.value->levelcoeff() )
            value = value->tryDivcoeff( cf.value, false, M, fail );
        else {
            InternalCF * dummy = cf.value->copyObject();
            dummy = dummy->tryDivcoeff( value, true, M, fail );
            if ( value->deleteObject() ) delete value;
            value = dummy;
        }
    }
    else  if ( level() > cf.level() )
        value = value->tryDivcoeff( cf.value, false, M, fail );
    else {
        InternalCF * dummy = cf.value->copyObject();
        dummy = dummy->tryDivcoeff( value, true, M, fail );
        if ( value->deleteObject() ) delete value;
        value = dummy;
    }
    return *this;
}

CanonicalForm &
CanonicalForm::operator %= ( const CanonicalForm & cf )
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

CanonicalForm &
CanonicalForm::mod ( const CanonicalForm & cf )
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

///same as divremt but handles zero divisors in case we are in Z_p[x]/(f) where f is not irreducible
bool
tryDivremt ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const CanonicalForm& M, bool& fail )
{
    ASSERT (getCharacteristic() > 0, "expected positive characteristic");
    ASSERT (!getReduce (M.mvar()), "do not reduce modulo M");
    fail= false;
    InternalCF * qq = 0, * rr = 0;
    int what = is_imm( f.value );
    bool result = true;
    if ( what )
        if ( is_imm( g.value ) ) {
            if ( what == FFMARK )
                imm_divrem_p( f.value, g.value, qq, rr );
            else  if ( what == GFMARK )
                imm_divrem_gf( f.value, g.value, qq, rr );
        }
        else
            result = g.value->tryDivremcoefft( f.value, qq, rr, true, M, fail );
    else  if ( (what=is_imm( g.value )) )
        result = f.value->tryDivremcoefft( g.value, qq, rr, false, M, fail );
    else  if ( f.value->level() == g.value->level() )
        if ( f.value->levelcoeff() == g.value->levelcoeff() )
            result = f.value->tryDivremsamet( g.value, qq, rr, M, fail );
        else  if ( f.value->levelcoeff() > g.value->levelcoeff() )
            result = f.value->tryDivremcoefft( g.value, qq, rr, false, M, fail );
        else
            result = g.value->tryDivremcoefft( f.value, qq, rr, true, M, fail );
    else  if ( f.value->level() > g.value->level() )
        result = f.value->tryDivremcoefft( g.value, qq, rr, false, M, fail );
    else
        result = g.value->tryDivremcoefft( f.value, qq, rr, true, M, fail );
    if (fail)
    {
      q= 0;
      r= 0;
      return false;
    }
    if ( result ) {
        ASSERT( qq != 0 && rr != 0, "error in divrem" );
        q = CanonicalForm( qq );
        r = CanonicalForm( rr );
        q= reduce (q, M);
        r= reduce (r, M);
    }
    else {
        q = 0; r = 0;
    }
    return result;
}

/**
 *
 * operator ()() - evaluation operator.
 *
 * Returns CO if CO is in a base domain.
 *
 * operator () ( f ) returns CO with f inserted for the main
 * variable.  Elements in an algebraic extension are considered
 * polynomials.
 *
**/
CanonicalForm
CanonicalForm::operator () ( const CanonicalForm & f ) const
{
    if ( is_imm( value ) || value->inBaseDomain() )
        return *this;
    else {
#if 0
        CFIterator i = *this;
        int lastExp = i.exp();
        CanonicalForm result = i.coeff();
        i++;
        while ( i.hasTerms() ) {
            if ( (lastExp - i.exp()) == 1 )
                result *= f;
            else
                result *= power( f, lastExp - i.exp() );
            result += i.coeff();
            lastExp = i.exp();
            i++;
        }
        if ( lastExp != 0 )
            result *= power( f, lastExp );
#else
        CFIterator i = *this;
        int lastExp = i.exp();
        CanonicalForm result = i.coeff();
        i++;
        while ( i.hasTerms() )
        {
            int i_exp=i.exp();
            if ( (lastExp - i_exp /* i.exp()*/) == 1 )
                result *= f;
            else
                result *= power( f, lastExp - i_exp /*i.exp()*/ );
            result += i.coeff();
            lastExp = i_exp /*i.exp()*/;
            i++;
        }
        if ( lastExp != 0 )
            result *= power( f, lastExp );
#endif
        return result;
    }
}

/**
 * Returns CO if CO is in a base domain.
 *
 * operator () ( f, v ) returns CO with f inserted for v.
 * Elements in an algebraic extension are considered polynomials
 * and v may be an algebraic variable.
**/
CanonicalForm
CanonicalForm::operator () ( const CanonicalForm & f, const Variable & v ) const
{
    if ( is_imm( value ) || value->inBaseDomain() )
        return *this;

    Variable x = value->variable();
    if ( v > x )
        return *this;
    else  if ( v == x )
        return (*this)( f );
    else {
        // v is less than main variable of f
        CanonicalForm result = 0;
        for ( CFIterator i = *this; i.hasTerms(); i++ )
            result += i.coeff()( f, v ) * power( x, i.exp() );
        return result;
    }
}

/**
 *
 * operator []() - return i'th coefficient from CO.
 *
 * Returns CO if CO is in a base domain and i equals zero.
 * Returns zero (from the current domain) if CO is in a base
 * domain and i is larger than zero.  Otherwise, returns the
 * coefficient to x^i in CO (if x denotes the main variable of
 * CO) or zero if CO does not contain x^i.  Elements in an
 * algebraic extension are considered polynomials.  i should be
 * larger or equal zero.
 *
 * Note: Never use a loop like
 *
~~~~~~~~~~~~~~~~~~~~~{.c}
    for ( int i = degree( f ); i >= 0; i-- )
         foo( i, f[ i ] );
~~~~~~~~~~~~~~~~~~~~~
 *
 * which is much slower than
 *
~~~~~~~~~~~~~~~~~~~~~{.c}
 * for ( int i = degree( f ), CFIterator I = f; I.hasTerms(); I++ ) {
 *     // fill gap with zeroes
 *     for ( ; i > I.exp(); i-- )
 *         foo( i, 0 );
 *     // at this point, i == I.exp()
 *     foo( i, i.coeff() );
 *     i--;
 * }
 * // work through trailing zeroes
 * for ( ; i >= 0; i-- )
 *     foo( i, 0 );
~~~~~~~~~~~~~~~~~~~~~
 *
**/
CanonicalForm
CanonicalForm::operator [] ( int i ) const
{
    ASSERT( i >= 0, "index to operator [] less than zero" );
    if ( is_imm( value ) )
        if ( i == 0 )
            return *this;
        else
            return CanonicalForm( 0 );
    else
        return value->coeff( i );
}

/**
 *
 * deriv() - return the formal derivation of CO.
 *
 * deriv() derives CO with respect to its main variable.  Returns
 * zero from the current domain if f is in a coefficient domain.
 *
 * @sa  CanonicalForm::deriv ( const Variable & x )
 *
**/
CanonicalForm
CanonicalForm::deriv () const
{
    if ( is_imm( value ) || value->inCoeffDomain() )
        return CanonicalForm( 0 );
    else {
        CanonicalForm result = 0;
        Variable x = value->variable();
        for ( CFIterator i = *this; i.hasTerms(); i++ )
            if ( i.exp() > 0 )
                result += power( x, i.exp()-1 ) * i.coeff() * i.exp();
        return result;
    }
}

/**
 * deriv( x ) derives CO with respect to x.  x should be a
 * polynomial variable.  Returns zero from the current domain if
 * f is in a coefficient domain.
**/
CanonicalForm
CanonicalForm::deriv ( const Variable & x ) const
{
    ASSERT( x.level() > 0, "cannot derive with respect to algebraic variables" );
    if ( is_imm( value ) || value->inCoeffDomain() )
        return CanonicalForm( 0 );

    Variable y = value->variable();
    if ( x > y )
        return CanonicalForm( 0 );
    else if ( x == y )
        return deriv();
    else {
        CanonicalForm result = 0;
        for ( CFIterator i = *this; i.hasTerms(); i++ )
            result += i.coeff().deriv( x ) * power( y, i.exp() );
        return result;
    }
}

/** int CanonicalForm::sign () const
 *
 * sign() - return sign of CO.
 *
 * If CO is an integer or a rational number, the sign is defined
 * as usual.  If CO is an element of a prime power domain or of
 * FF(p) and SW_SYMMETRIC_FF is on, the sign of CO is the sign of
 * the symmetric representation of CO.  If CO is in GF(q) or in
 * FF(p) and SW_SYMMETRIC_FF is off, the sign of CO is zero iff
 * CO is zero, otherwise the sign is one.
 *
 * If CO is a polynomial or in an extension of one of the base
 * domains, the sign of CO is the sign of its leading
 * coefficient.
 *
 * @sa InternalCF::sign(), InternalInteger::sign(),
 * InternalRational::sign(),
 * InternalPoly::sign(), imm_sign(), gf_sign()
 *
**/
int
CanonicalForm::sign () const
{
    if ( is_imm( value ) )
        return imm_sign( value );
    else
        return value->sign();
}

/** CanonicalForm CanonicalForm::sqrt () const
 *
 * sqrt() - calculate integer square root.
 *
 * CO has to be an integer greater or equal zero.  Returns the
 * largest integer less or equal sqrt(CO).
 *
 * In the immediate case, we use the newton method to find the
 * root.  The algorithm is from H. Cohen - 'A Course in
 * Computational Algebraic Number Theory', ch. 1.7.1.
 *
 * @sa InternalCF::sqrt(), InternalInteger::sqrt(), ::sqrt()
 *
**/
CanonicalForm
CanonicalForm::sqrt () const
{
    if ( is_imm( value ) ) {
        ASSERT( is_imm( value ) == INTMARK, "sqrt() not implemented" );
        long n = imm2int( value );
        ASSERT( n >= 0, "arg to sqrt() less than zero" );
        if ( n == 0 || n == 1 )
            return CanonicalForm( n );
        else {
            long x, y = n;
            do {
                x = y;
                // the intermediate result may not fit into an
                // integer, but the result does
                y = (unsigned long)(x + n/x)/2;
            } while ( y < x );
            return CanonicalForm( x );
        }
    }
    else
        return CanonicalForm( value->sqrt() );
}

/** int CanonicalForm::ilog2 () const
 *
 * ilog2() - integer logarithm to base 2.
 *
 * Returns the largest integer less or equal logarithm of CO to
 * base 2.  CO should be a positive integer.
 *
 * @sa InternalCF::ilog2(), InternalInteger::ilog2(), ::ilog2()
 *
**/
int
CanonicalForm::ilog2 () const
{
    if ( is_imm( value ) )
    {
        ASSERT( is_imm( value ) == INTMARK, "ilog2() not implemented" );
        long a = imm2int( value );
        ASSERT( a > 0, "arg to ilog2() less or equal zero" );
        return SI_LOG2_LONG(a);
    }
    else
        return value->ilog2();
}

/**
 *
 * operator ==() - compare canonical forms on
 *   (in)equality.
 *
 * operator ==() returns true iff lhs equals rhs.
 *
 * This is the point in factory where we essentially use that
 * CanonicalForms in fact are canonical.  There must not be two
 * different representations of the same mathematical object,
 * otherwise, such (in)equality will not be recognized by these
 * operators.  In other word, we rely on the fact that structural
 * different factory objects in any case represent different
 * mathematical objects.
 *
 * So we use the following procedure to test on equality (and
 * analogously on inequality).  First, we check whether lhs.value
 * equals rhs.value.  If so we are ready and return true.
 * Second, if one of the operands is immediate, but the other one
 * not, we return false.  Third, if the operand's levels differ
 * we return false.  Fourth, if the operand's levelcoeffs differ
 * we return false.  At last, we call the corresponding internal
 * method to compare both operands.
 *
 * Both operands should have coefficients from the same base domain.
 *
 * Note: To compare with the zero or the unit of the current domain,
 * you better use the methods `CanonicalForm::isZero()' or
 * `CanonicalForm::isOne()', resp., than something like `f == 0',
 * since the latter is quite a lot slower.
 *
 * @sa CanonicalForm::operator !=(), InternalCF::comparesame(),
 * InternalInteger::comparesame(), InternalRational::comparesame(),
 * InternalPoly::comparesame()
 *
**/
bool
operator == ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    if ( lhs.value == rhs.value )
        return true;
    else if ( is_imm( rhs.value ) || is_imm( lhs.value ) ) {
        ASSERT( ! is_imm( rhs.value ) ||
                ! is_imm( lhs.value ) ||
                is_imm( rhs.value ) == is_imm( lhs.value ),
                "incompatible operands" );
        return false;
    }
    else  if ( lhs.value->level() != rhs.value->level() )
        return false;
    else  if ( lhs.value->levelcoeff() != rhs.value->levelcoeff() )
        return false;
    else
        return rhs.value->comparesame( lhs.value ) == 0;
}

/**
 * operator !=() returns true iff lhs does not equal rhs.
 *
 * @sa CanonicalForm::operator ==()
**/
bool
operator != ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    if ( lhs.value == rhs.value )
        return false;
    else if ( is_imm( rhs.value ) || is_imm( lhs.value ) ) {
        ASSERT( ! is_imm( rhs.value ) ||
                ! is_imm( lhs.value ) ||
                is_imm( rhs.value ) == is_imm( lhs.value ),
                "incompatible operands" );
        return true;
    }
    else  if ( lhs.value->level() != rhs.value->level() )
        return true;
    else  if ( lhs.value->levelcoeff() != rhs.value->levelcoeff() )
        return true;
    else        return rhs.value->comparesame( lhs.value ) != 0;
}

/**
 *
 * operator >() - compare canonical forms. on size or
 *   level.
 *
 * The most common and most useful application of these operators
 * is to compare two integers or rationals, of course.  However,
 * these operators are defined on all other base domains and on
 * polynomials, too.  From a mathematical point of view this may
 * seem meaningless, since there is no ordering on finite fields
 * or on polynomials respecting the algebraic structure.
 * Nevertheless, from a programmer's point of view it may be
 * sensible to order these objects, e.g. to sort them.
 *
 * Therefore, the ordering defined by these operators in any case
 * is a total ordering which fulfills the law of trichotomy.
 *
 * It is clear how this is done in the case of the integers and
 * the rationals.  For finite fields, all you can say is that
 * zero is the minimal element w.r.t. the ordering, the other
 * elements are ordered in an arbitrary (but total!)  way.  For
 * polynomials, you have an ordering derived from the
 * lexicographical ordering of monomials.  E.g. if lm(f) < lm(g)
 * w.r.t. lexicographic ordering, then f < g.  For more details,
 * refer to the documentation of `InternalPoly::operator <()'.
 *
 * Both operands should have coefficients from the same base domain.
 *
 * The scheme how both operators are implemented is allmost the
 * same as for the assignment operators (check for immediates,
 * then check levels, then check levelcoeffs, then call the
 * appropriate internal comparesame()/comparecoeff() method).
 * For more information, confer to the overview for the
 * arithmetic operators.
 *
 * @sa CanonicalForm::operator <(), InternalCF::comparesame(),
 * InternalInteger::comparesame(), InternalRational::comparesame(),
 * InternalPoly::comparesame(),
 * InternalCF::comparecoeff(), InternalInteger::comparecoeff(),
 * InternalRational::comparecoeff(),
 * InternalPoly::comparecoeff(),
 * imm_cmp(), imm_cmp_p(), imm_cmp_gf()
 *
**/
bool
operator > ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    int what = is_imm( rhs.value );
    if ( is_imm( lhs.value ) ) {
        ASSERT( ! what || (what == is_imm( lhs.value )), "incompatible operands" );
        if ( what == 0 )
            return rhs.value->comparecoeff( lhs.value ) < 0;
        else if ( what == INTMARK )
            return imm_cmp( lhs.value, rhs.value ) > 0;
        else if ( what == FFMARK )
            return imm_cmp_p( lhs.value, rhs.value ) > 0;
        else
            return imm_cmp_gf( lhs.value, rhs.value ) > 0;
    }
    else  if ( what )
        return lhs.value->comparecoeff( rhs.value ) > 0;
    else  if ( lhs.value->level() == rhs.value->level() )
        if ( lhs.value->levelcoeff() == rhs.value->levelcoeff() )
            return lhs.value->comparesame( rhs.value ) > 0;
        else  if ( lhs.value->levelcoeff() > rhs.value->levelcoeff() )
            return lhs.value->comparecoeff( rhs.value ) > 0;
        else
            return rhs.value->comparecoeff( lhs.value ) < 0;
    else
        return lhs.value->level() > rhs.value->level();
}

/**
 * @sa CanonicalForm::operator >()
**/
bool
operator < ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    int what = is_imm( rhs.value );
    if ( is_imm( lhs.value ) ) {
        ASSERT( ! what || (what == is_imm( lhs.value )), "incompatible operands" );
        if ( what == 0 )
            return rhs.value->comparecoeff( lhs.value ) > 0;
        else if ( what == INTMARK )
            return imm_cmp( lhs.value, rhs.value ) < 0;
        else if ( what == FFMARK )
            return imm_cmp_p( lhs.value, rhs.value ) < 0;
        else
            return imm_cmp_gf( lhs.value, rhs.value ) < 0;
    }
    else  if ( what )
        return lhs.value->comparecoeff( rhs.value ) < 0;
    else  if ( lhs.value->level() == rhs.value->level() )
        if ( lhs.value->levelcoeff() == rhs.value->levelcoeff() )
            return lhs.value->comparesame( rhs.value ) < 0;
        else  if ( lhs.value->levelcoeff() > rhs.value->levelcoeff() )
            return lhs.value->comparecoeff( rhs.value ) < 0;
        else
            return rhs.value->comparecoeff( lhs.value ) > 0;
    else
        return lhs.value->level() < rhs.value->level();
}

/** CanonicalForm bgcd ( const CanonicalForm & f, const CanonicalForm & g )
 *
 * bgcd() - return base coefficient gcd.
 *
 * If both f and g are integers and `SW_RATIONAL' is off the
 * positive greatest common divisor of f and g is returned.
 * Otherwise, if `SW_RATIONAL' is on or one of f and g is not an
 * integer, the greatest common divisor is trivial: either zero
 * if f and g equal zero or one (both from the current domain).
 *
 * f and g should come from one base domain which should be not
 * the prime power domain.
 *
 * Implementation:
 *
 * CanonicalForm::bgcd() handles the immediate case with a
 *   standard euclidean algorithm.  For the non-immediate cases
 *   `InternalCF::bgcdsame()' or `InternalCF::bgcdcoeff()', resp. are
 *   called following the usual level/levelcoeff approach.
 *
 * InternalCF::bgcdsame() and
 * InternalCF::bgcdcoeff() throw an assertion ("not implemented")
 *
 * InternalInteger::bgcdsame() is a wrapper around `mpz_gcd()'
 *   which takes some care about immediate results and the sign
 *   of the result
 * InternalInteger::bgcdcoeff() is a wrapper around
 *   `mpz_gcd_ui()' which takes some care about the sign
 *   of the result
 *
 * InternalRational::bgcdsame() and
 * InternalRational::bgcdcoeff() always return one
 *
**/
CanonicalForm
bgcd ( const CanonicalForm & f, const CanonicalForm & g )
{
    // check immediate cases
    int what = is_imm( g.value );
    if ( is_imm( f.value ) )
    {
        ASSERT( ! what || (what == is_imm( f.value )), "incompatible operands" );
        if ( what == 0 )
            return g.value->bgcdcoeff( f.value );
        else if ( what == INTMARK && ! cf_glob_switches.isOn( SW_RATIONAL ) )
        {
            // calculate gcd using standard integer
            // arithmetic
            long fInt = imm2int( f.value );
            long gInt = imm2int( g.value );

            if ( fInt < 0 ) fInt = -fInt;
            if ( gInt < 0 ) gInt = -gInt;
            // swap fInt and gInt
            if ( gInt > fInt )
            {
                long swap = gInt;
                gInt = fInt;
                fInt = swap;
            }

            // now, 0 <= gInt <= fInt.  Start the loop.
            while ( gInt )
            {
                // calculate (fInt, gInt) = (gInt, fInt%gInt)
                long r = fInt % gInt;
                fInt = gInt;
                gInt = r;
            }

            return CanonicalForm( fInt );
        }
        else
            // we do not go for maximal speed for these stupid
            // special cases
            return CanonicalForm( f.isZero() && g.isZero() ? 0 : 1 );
    }
    else if ( what )
        return f.value->bgcdcoeff( g.value );

    int fLevel = f.value->level();
    int gLevel = g.value->level();

    // check levels
    if ( fLevel == gLevel )
    {
        fLevel = f.value->levelcoeff();
        gLevel = g.value->levelcoeff();

        // check levelcoeffs
        if ( fLevel == gLevel )
            return f.value->bgcdsame( g.value );
        else if ( fLevel < gLevel )
            return g.value->bgcdcoeff( f.value );
        else
            return f.value->bgcdcoeff( g.value );
    }
    else if ( fLevel < gLevel )
        return g.value->bgcdcoeff( f.value );
    else
        return f.value->bgcdcoeff( g.value );
}

/** CanonicalForm bextgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
 *
 * bextgcd() - return base coefficient extended gcd.
 *
**/
CanonicalForm
bextgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b )
{
    // check immediate cases
    int what = is_imm( g.value );
    if ( is_imm( f.value ) ) {
        ASSERT( ! what || (what == is_imm( f.value )), "incompatible operands" );
        if ( what == 0 )
            return g.value->bextgcdcoeff( f.value, b, a );
        else if ( what == INTMARK && ! cf_glob_switches.isOn( SW_RATIONAL ) ) {
            // calculate extended gcd using standard integer
            // arithmetic
            long fInt = imm2int( f.value );
            long gInt = imm2int( g.value );

            // to avoid any system dpendencies with `%', we work
            // with positive numbers only.  To a pity, we have to
            // redo all the checks when assigning to a and b.
            if ( fInt < 0 ) fInt = -fInt;
            if ( gInt < 0 ) gInt = -gInt;
            // swap fInt and gInt
            if ( gInt > fInt ) {
                long swap = gInt;
                gInt = fInt;
                fInt = swap;
            }

            long u = 1; long v = 0;
            long uNext = 0; long vNext = 1;

            // at any step, we have:
            // fInt_0 * u + gInt_0 * v = fInt
            // fInt_0 * uNext + gInt_0 * vNext = gInt
            // where fInt_0 and gInt_0 denote the values of fint
            // and gInt, resp., at the beginning
            while ( gInt ) {
                long r = fInt % gInt;
                long q = fInt / gInt;
                long uSwap = u - q * uNext;
                long vSwap = v - q * vNext;

                // update variables
                fInt = gInt;
                gInt = r;
                u = uNext; v = vNext;
                uNext = uSwap; vNext = vSwap;
            }

            // now, assign to a and b
            long fTest = imm2int( f.value );
            long gTest = imm2int( g.value );
            if ( gTest > fTest ) {
                a = v; b = u;
            } else {
                a = u; b = v;
            }
            if ( fTest < 0 ) a = -a;
            if ( gTest < 0 ) b = -b;
            return CanonicalForm( fInt );
        } else
            // stupid special cases
            if ( ! f.isZero() ) {
                a = 1/f; b = 0; return CanonicalForm( 1L );
            } else if ( ! g.isZero() ) {
                a = 0; b = 1/g; return CanonicalForm( 1L );
            } else {
                a = 0; b = 0; return CanonicalForm( 0L );
            }
    }
    else if ( what )
        return f.value->bextgcdcoeff( g.value, a, b );

    int fLevel = f.value->level();
    int gLevel = g.value->level();

    // check levels
    if ( fLevel == gLevel ) {
        fLevel = f.value->levelcoeff();
        gLevel = g.value->levelcoeff();

        // check levelcoeffs
        if ( fLevel == gLevel )
            return f.value->bextgcdsame( g.value, a, b );
        else if ( fLevel < gLevel )
            return g.value->bextgcdcoeff( f.value, b, a );
        else
            return f.value->bextgcdcoeff( g.value, a, b );
    }
    else if ( fLevel < gLevel )
        return g.value->bextgcdcoeff( f.value, b, a );
    else
        return f.value->bextgcdcoeff( g.value, a, b );
}

CanonicalForm
blcm ( const CanonicalForm & f, const CanonicalForm & g )
{
    if ( f.isZero() || g.isZero() )
        return CanonicalForm( 0L );
/*
    else if (f.isOne())
        return g;
    else if (g.isOne())
        return f;
*/
    else
        return (f / bgcd( f, g )) * g;
}

/** input/output **/
#ifndef NOSTREAMIO
void
CanonicalForm::print( OSTREAM & os, char * str ) const
{
    if ( is_imm( value ) )
        imm_print( os, value, str );
    else
        value->print( os, str );
}

void
CanonicalForm::print( OSTREAM & os ) const
{
    if ( is_imm( value ) )
        imm_print( os, value, "" );
    else
        value->print( os, "" );
}

OSTREAM&
operator << ( OSTREAM & os, const CanonicalForm & cf )
{
    cf.print( os, "" );
    return os;
}

ISTREAM&
operator >> ( ISTREAM & is, CanonicalForm & cf )
{
    cf = readCF( is );
    return is;
}
#endif /* NOSTREAMIO */

/** genOne(), genZero() **/
CanonicalForm
CanonicalForm::genZero() const
{
    int what = is_imm( value );
    if ( what == FFMARK )
        return CanonicalForm( CFFactory::basic( FiniteFieldDomain, 0L ) );
    else  if ( what == GFMARK )
        return CanonicalForm( CFFactory::basic( GaloisFieldDomain, 0L ) );
    else  if ( what )
        return CanonicalForm( CFFactory::basic( IntegerDomain, 0L ) );
    else
        return CanonicalForm( value->genZero() );
}

CanonicalForm
CanonicalForm::genOne() const
{
    int what = is_imm( value );
    if ( what == FFMARK )
        return CanonicalForm( CFFactory::basic( FiniteFieldDomain, 1L ) );
    else  if ( what == GFMARK )
        return CanonicalForm( CFFactory::basic( GaloisFieldDomain, 1L ) );
    else  if ( what )
        return CanonicalForm( CFFactory::basic( IntegerDomain, 1L ) );
    else
        return CanonicalForm( value->genOne() );
}

/** exponentiation **/
CanonicalForm
power ( const CanonicalForm & f, int n )
{
  ASSERT( n >= 0, "illegal exponent" );
  if ( f.isZero() )
    return CanonicalForm(0L);
  else  if ( f.isOne() )
    return f;
  else  if ( f == -1 )
  {
    if ( n % 2 == 0 )
      return CanonicalForm(1L);
    else
      return CanonicalForm(-1L);
  }
  else  if ( n == 0 )
    return CanonicalForm(1L);

  //else if (f.inGF())
  //{
  //}
  else
  {
    CanonicalForm g,h;
    h=f;
    while(n%2==0)
    {
      h*=h;
      n/=2;
    }
    g=h;
    while(1)
    {
      n/=2;
      if(n==0)
        return g;
      h*=h;
      if(n%2!=0) g*=h;
    }
  }
}

/** exponentiation **/
CanonicalForm
power ( const Variable & v, int n )
{
    //ASSERT( n >= 0, "illegal exponent" );
    if ( n == 0 )
        return 1;
    else  if ( n == 1 )
        return v;
    else  if (( v.level() < 0 ) && (hasMipo(v)))
    {
        CanonicalForm result( v, n-1 );
        return result * v;
    }
    else
        return CanonicalForm( v, n );
}

/** switches **/
void
On( int sw )
{
    cf_glob_switches.On( sw );
}

/** switches **/
void
Off( int sw )
{
    cf_glob_switches.Off( sw );
}

/** switches **/
bool
isOn( int sw )
{
    return cf_glob_switches.isOn( sw );
}

#ifndef HAVE_NTL
static int initialized=0;
int
initCanonicalForm( void )
{
  if ( ! initialized )
  {
    initPT();
    initialized = true;
  }
}
#endif

