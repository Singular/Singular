/* emacs edit mode for this file is -*- C++ -*- */

/**
 *
 * @file cf_inline.cc
 *
 * definition of configurable inline
 *   `CanonicalForm' methods.
 *
 * Hierarchy: canonicalform
 *
 * Header file: canonicalform.h
 *
 * Developers note:
 * ----------------
 * The central class in Factory is, of course, `CanonicalForm'.
 * Hence it is a quiet reasonable to assume that inlining its
 * most important methods will improve execution speed.  The same
 * holds for some methods of the `CFIterator' class.  Everything
 * on configurable inline `CanonicalForm' methods explained here
 * applies mutatis mutandis to the `CFIterator' methods.
 *
 * However, inlining `CanonicalForm' methods has two major
 * drawbacks:
 *
 * o If `CanonicalForm' methods simply would have been declared
 *   `inline' it would have been necessary to include the
 *   definition of `InternalCF' in `factory.h'.  This would have
 *   been quite a contradiction to the internal nature of the
 *   class.
 *   Hence it seemed desirable to introduce a mechanism to have
 *   both the inlined versions for internal use and compiled
 *   versions for the library.
 *
 * o Second, inlining in most cases leads to larger object code.
 *   E.g., inlining `CanonicalForm::~CanonicalForm()' increases the
 *   object code by approx. 15% without any effect on computation
 *   speed.
 *   Thus another design aim was to keep things configurable.
 *   That is why the methods defined here are called
 *   "configurable inline methods".
 *
 * The low level solution to both problems is the macro
 * `CF_INLINE' which either expands to `inline' or nothing.  The
 * counterpart `CF_NO_INLINE' exists only for convenience, it
 * always expands to nothing.  `CF_INLINE' is set immediately
 * before defining resp. declaring the methods to exclude any
 * esoteric influences from included files.
 *
 * The high level interface is the macro `CF_USE_INLINE'.  If it
 * is defined any header file that uses configurable inline
 * methods defines them to be `inline', otherwise they are
 * defined as ordinary methods.  `CF_USE_INLINE' is defined in
 * `config.h' only.
 *
 * To switch on (off) all configurable inline methods, it is
 * sufficient to define (undefine) `CF_USE_INLINE' in `config.h'.
 * To switch off separate configurable inline methods it is
 * necessary to prefix their declaration in `canonicalform.h' by
 * `CF_NO_INLINE' instead of `CF_INLINE'.  Furthermore, to avoid
 * duplicate symbols at link time, their definition in this file
 * has to be wrapped by an `#ifndef INCL_CF_INLINE_CC'.
 *
 * It turned out that inlining the following methods (and only
 * them) results in the best time to size ratio on Linux and HP
 * machines:
 * o all `CanonicalForm' constructors
 * o the binary `CanonicalForm' operators `+' and `*'
 *
**/

// check whether we are included or translated and
// define `INCL_CF_INLINE_CC' if we are included
#ifdef INCL_CANONICALFORM_H
#define INCL_CF_INLINE_CC
#endif


#include "config.h"


#include "cf_assert.h"

// temporarily switch off `CF_USE_INLINE' and include
// `canonicalform.h' if we are being translated.
// `CF_USE_INLINE_SAVE' is used to save the state of
// `CF_USE_INLINE'.  It is unset after use.
#ifndef INCL_CF_INLINE_CC
#ifdef CF_USE_INLINE
#define CF_USE_INLINE_SAVE
#undef CF_USE_INLINE
#endif
#include "canonicalform.h"
#ifdef CF_USE_INLINE_SAVE
#define CF_USE_INLINE
#undef CF_USE_INLINE_SAVE
#endif
#endif /* ! INCL_CF_INLINE_CC */

// regular include files
#include "int_cf.h"
#include "imm.h"
#include "cf_factory.h"

// set the value of `CF_INLINE' for the following methods and
// functions
#if defined( CF_USE_INLINE ) && defined( INCL_CF_INLINE_CC )
#undef CF_INLINE
#define CF_INLINE inline
#else
#undef CF_INLINE
#define CF_INLINE
#endif /* ! defined( CF_USE_INLINE ) && defined( INCL_CF_INLINE_CC ) */

// constructors, destructors, assignment
/** CF_INLINE CanonicalForm::CanonicalForm ()
 *
 *
 * CanonicalForm() - create the default canonical form.
 *
 * The canonical form is initialized to zero from the current
 * domain.
 *
**/
CF_INLINE
CanonicalForm::CanonicalForm ()
    : value( CFFactory::basic( 0L ) )
{
}

/** CF_INLINE CanonicalForm::CanonicalForm ( const int i )
 *
 *
 * CanonicalForm() - create a canonical form from an integer.
 *
 * The canonical form is initialized to the "canonical image" of
 * `i' in the current domain.  This is `i' itself for
 * characteristic zero, `i' mod p for finite fields of
 * characteristic p, and `i' mod p^n for prime power domains with
 * p^n elements.
 *
**/
#if SIZEOF_LONG == 8
CF_INLINE
CanonicalForm::CanonicalForm ( const int i )
    : value( CFFactory::basic( i ) )
#else
CF_INLINE
CanonicalForm::CanonicalForm ( const int i )
    : value( CFFactory::basic( (long)i ) )
#endif
{
}

CF_INLINE
CanonicalForm::CanonicalForm ( const long i )
    : value( CFFactory::basic( i ) )
{
}

/** CF_INLINE CanonicalForm::CanonicalForm ( const CanonicalForm & cf )
 *
 *
 * CanonicalForm() - create a copy of a canonical form.
 *
 * Type info:
 * ----------
 * cf: Anything
 *
**/
CF_INLINE
CanonicalForm::CanonicalForm ( const CanonicalForm & cf )
    : value( is_imm( cf.value ) ? cf.value : cf.value->copyObject() )
{
}

/** CF_INLINE CanonicalForm::CanonicalForm ( InternalCF * cf )
 *
 *
 * CanonicalForm() - create a canonical form from a pointer to an
 *   internal canonical form.
 *
 * This constructor is reserved for internal usage.
 *
 * Developers note:
 * ----------------
 * The canonical form gets its value immediately from `cf'.
 * `cf's reference counter is not incremented, so be careful with
 * this constructor.
 *
**/
CF_INLINE
CanonicalForm::CanonicalForm ( InternalCF * cf )
    : value( cf )
{
}

/** CF_INLINE CanonicalForm::CanonicalForm ( const Variable & v )
 *
 *
 * CanonicalForm() - create a canonical form from a variable.
 *
 * If `v' is a polynomial variable or an algebraic element the
 * resulting polynomial (or algebraic element) is 1*`v'^1, the
 * one being from the current domain.
 *
 * Variables of level `LEVELBASE' are transformed to one from the
 * current domain.
 *
 * Type info:
 * ----------
 * v: Anything
 *
**/
CF_INLINE
CanonicalForm::CanonicalForm ( const Variable & v )
    : value( CFFactory::poly( v ) )
{
}

/** CF_INLINE CanonicalForm::CanonicalForm ( const Variable & v, int e )
 *
 *
 * CanonicalForm() - create a canonical form from a power of a
 *   variable.
 *
 * If `v' is a polynomial variable or an algebraic element the
 * resulting polynomial (or algebraic element) is 1*`v'^`e', the
 * one being from the current domain.  Algebraic elements are
 * reduced modulo their minimal polynomial.
 *
 * Variables of level `LEVELBASE' are transformed to one from the
 * current domain.
 *
 * Type info:
 * ----------
 * v: Anything
 *
**/
CF_INLINE
CanonicalForm::CanonicalForm ( const Variable & v, int e )
    : value( CFFactory::poly( v, e ) )
{
    //ASSERT( e > 0, "math error: exponent has to be positive" );
}

#ifndef INCL_CF_INLINE_CC
/** CF_INLINE CanonicalForm::~CanonicalForm ()
 *
 *
 * ~CanonicalForm() - delete CO.
 *
 * Type info:
 * ----------
 * CO: Anything
 *
**/
CF_INLINE
CanonicalForm::~CanonicalForm ()
{
    if ( (! is_imm( value )) && value->deleteObject() )
        delete value;
}
#endif

#ifndef INCL_CF_INLINE_CC
/** CF_INLINE CanonicalForm & CanonicalForm::operator = ( const CanonicalForm & cf )
 *
 *
 * operator =() - assign `cf' to CO.
 *
 * Type info:
 * ----------
 * CO, cf: Anything
 *
**/
CF_INLINE CanonicalForm &
CanonicalForm::operator = ( const CanonicalForm & cf )
{
    if ( this != &cf ) {
        if ( (! is_imm( value )) && value->deleteObject() )
            delete value;
        value = (is_imm( cf.value )) ? cf.value : cf.value->copyObject();
    }
    return *this;
}

/**
 *
 * operator =() - assign long `cf' to CO.
 *
 * `cf' converted to a canonical form as described in the
 * canonical form constructor which creates a canonical form from
 * an integer.
 *
 * Type info:
 * ----------
 * CO: Anything
 *
 * Developers note:
 * ----------------
 * Strictly speaking, this operator is superfluous.  The ordinary
 * assignment operator together with automatic conversion from
 * `int' to `CanonicalForm' would do the job, too.  But this way
 * the common operation of assigning an integer is faster.
 *
**/
CF_INLINE CanonicalForm &
CanonicalForm::operator = ( const long cf )
{
    if ( (! is_imm( value )) && value->deleteObject() )
        delete value;
    value = CFFactory::basic( cf );
    return *this;
}
#endif

// predicates
#ifndef INCL_CF_INLINE_CC
/** CF_INLINE bool CanonicalForm::isOne, isZero () const
 *
 *
 * isOne(), isZero() - test whether a `CanonicalForm' equals one
 *   or zero, resp.
 *
 * The predicates `isOne()' and `isZero()' are much faster than
 * the comparison operators.  Furthermore, a test `f.isZero()' is
 * independent from the current domain, whereas an expression
 * `f == 0' is not.
 *
 * Type info:
 * ----------
 * CO: Anything
 *
 * Internal implementation:
 * ------------------------
 * Note that only immediate objects and objects of class
 * `InternalPrimePower' may equal one or zero, resp.
 *
 * imm_isone(), imm_iszero()
 * Trivial.
 *
 * imm_isone_p(), imm_iszero_p()
 * Trivial.
 *
 * imm_isone_gf(), imm_iszero_gf()
 * Use `gf_isone()' and `gf_iszero()', resp., to test whether CO
 * equals zero or one, resp.
 *
 * InternalCF::isOne(), isZero()
 * Always return false.
 *
 * InternalPrimePower::isOne(), isZero()
 * Use `mpz_cpm_ui()' resp. `mpz_sgn()' to check the underlying
 * mpi.
 *
 * @sa CanonicalForm::isZero()
**/
CF_INLINE bool
CanonicalForm::isOne () const
{
    int what = is_imm( value );

    if ( ! what )
        return value->isOne();
    else  if ( what == INTMARK )
        return imm_isone( value );
    else if ( what == FFMARK )
        return imm_isone_p( value );
    else
        return imm_isone_gf( value );
}

/**
 * @sa CanonicalForm::isOne()
**/
CF_INLINE bool
CanonicalForm::isZero () const
{
    int what = is_imm( value );

    if ( what == 0 )
        return value->isZero();
    else  if ( what == INTMARK )
        return imm_iszero( value );
    else if ( what == FFMARK )
        return imm_iszero_p( value );
    else
        return imm_iszero_gf( value );
}
#endif

// arithmetic operators
#ifndef INCL_CF_INLINE_CC
/** CF_INLINE CanonicalForm operator - ( const CanonicalForm & cf )
 *
 *
 * operator -() - return additive inverse of `cf'.
 *
 * Returns the additive inverse of `cf'.  One should keep in mind
 * that to negate a canonical form a complete (deep) copy of it
 * has to be created.
 *
 * Type info:
 * ----------
 * cf: CurrentPP
 *
 * In fact, the type is almost `Anything', but it is, e.g., not
 * possible to invert an element from a finite field when the
 * characteristic of the current domain has changed.
 *
 * Internal implementation:
 * ------------------------
 * All internal methods check whether the reference counter
 * equals one.  If so CO is negated in-place.  Otherwise, a new
 * copy of CO is created and negated.
 *
 * imm_neg()
 * Trivial.
 *
 * imm_neg_p()
 * Use `ff_neg()' to negate CO.
 *
 * imm_neg_gf()
 * Use `gf_neg()' to negate CO.
 *
 * InternalInteger::neg()
 * Use `mpz_neg()' to negate the underlying mpi.
 *
 * InternalRational::neg ()
 * Use `mpz_neg()' to negate the denominator.
 *
 * InternalPrimePower::neg()
 * Subtract CO from `primepow' using `mpz_sub'.
 *
 * InternalPoly::neg()
 * If reference counter is one use `negateTermList()' to negate
 * the terms, otherwise create a negated copy using
 * `copyTermList()'.
 *
 * @sa CanonicalForm::operator -=()
**/
CF_INLINE CanonicalForm
operator - ( const CanonicalForm & cf )
{
    CanonicalForm result( cf );
    int what = is_imm( result.value );

    if ( ! what )
        result.value = result.value->neg();
    else  if ( what == INTMARK )
        result.value = imm_neg( result.value );
    else if ( what == FFMARK )
        result.value = imm_neg_p( result.value );
    else
        result.value = imm_neg_gf( result.value );

    return result;
}
#endif

// binary arithmetic operators and functions
/** CF_INLINE CanonicalForm operator +, -, *, /, % ( const CanonicalForm & lhs, const CanonicalForm & rhs )
 *
 *
 * operators +, -, *, /, %(), div(), mod() - binary arithmetic
 *   operators.
 *
 * The binary operators have their standard (mathematical)
 * semantics.  As explained for the corresponding arithmetic
 * assignment operators, the operators `/' and `%' return the
 * quotient resp. remainder of (polynomial) division with
 * remainder, whereas `div()' and `mod()' may be used for exact
 * division and term-wise remaindering, resp.
 *
 * It is faster to use the arithmetic assignment operators (e.g.,
 * `f += g;') instead of the binary operators (`f = f+g;' ).
 *
 * Type info:
 * ----------
 * lhs, rhs: CurrentPP
 *
 * There are weaker preconditions for some cases (e.g.,
 * arithmetic operations with elements from Q or Z work in any
 * domain), but type `CurrentPP' is the only one guaranteed to
 * work for all cases.
 *
 * Developers note:
 * ----------------
 * All binary operators have their corresponding `CanonicalForm'
 * assignment operators (e.g., `operator +()' corresponds to
 * `CanonicalForm::operator +=()', `div()' corresponds to
 * `CanonicalForm::div()).
 *
 * And that is how they are implemented, too: Each of the binary
 * operators first creates a copy of `lhs', adds `rhs' to this
 * copy using the assignment operator, and returns the result.
 *
 * @sa CanonicalForm::operator +=()
**/
CF_INLINE CanonicalForm
operator + ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result += rhs;
    return result;
}

#ifndef INCL_CF_INLINE_CC
CF_INLINE CanonicalForm
operator - ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result -= rhs;
    return result;
}
#endif

/**
 * @sa CanonicalForm::operator *=()
**/
CF_INLINE CanonicalForm
operator * ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result *= rhs;
    return result;
}

#ifndef INCL_CF_INLINE_CC
/**
 * @sa CanonicalForm::operator /=()
**/
CF_INLINE CanonicalForm
operator / ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result /= rhs;
    return result;
}

/**
 * @sa CanonicalForm::operator %=()
**/
CF_INLINE CanonicalForm
operator % ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result %= rhs;
    return result;
}
#endif

#ifndef INCL_CF_INLINE_CC
/** CF_INLINE CanonicalForm div, mod ( const CanonicalForm & lhs, const CanonicalForm & rhs )
 * @sa mod(), operator/(), CanonicalForm::operator /=()
**/
CF_INLINE CanonicalForm
div ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result.div( rhs );
    return result;
}

/**
 * @sa div(), operator%(), CanonicalForm::operator %=()
**/
CF_INLINE CanonicalForm
mod ( const CanonicalForm & lhs, const CanonicalForm & rhs )
{
    CanonicalForm result( lhs );
    result.mod( rhs );
    return result;
}
#endif
