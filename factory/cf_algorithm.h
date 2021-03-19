/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_algorithm.h
 * declarations of higher level algorithms.
 *
 * Header file corresponds to: cf_algorithm.cc, cf_chinese.cc,
 *   cf_factor.cc, cf_linsys.cc, cf_resultant.cc
 *
 * Hierarchy: mathematical algorithms on canonical forms
 *
 * Developers note:
 * ----------------
 * This header file collects declarations of most of the
 * functions in Factory which implement higher level algorithms
 * on canonical forms (factorization, gcd, etc.) and declarations
 * of some low level mathematical functions, too (absolute value,
 * euclidean norm, etc.).
 *
**/

#ifndef INCL_CF_ALGORITHM_H
#define INCL_CF_ALGORITHM_H

// #include "config.h"

#include "canonicalform.h"
#include "variable.h"

/*BEGINPUBLIC*/

//{{{ function declarations from cf_algorithm.cc
CanonicalForm psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

CanonicalForm psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

void psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable & x );

CanonicalForm FACTORY_PUBLIC bCommonDen ( const CanonicalForm & f );

bool fdivides ( const CanonicalForm & f, const CanonicalForm & g );

bool fdivides ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm& quot );

bool tryFdivides ( const CanonicalForm & f, const CanonicalForm & g, const CanonicalForm& M, bool& fail );

CanonicalForm maxNorm ( const CanonicalForm & f );

CanonicalForm euclideanNorm ( const CanonicalForm & f );
//}}}

//{{{ function declarations from cf_chinese.cc
void FACTORY_PUBLIC chineseRemainder ( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew );

void FACTORY_PUBLIC chineseRemainder ( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew );

void FACTORY_PUBLIC chineseRemainderCached ( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew, CFArray &inv );
void FACTORY_PUBLIC chineseRemainderCached(const CFArray &a, const CFArray &n, CanonicalForm &xnew, CanonicalForm &prod, CFArray &inv);


CanonicalForm Farey ( const CanonicalForm & f, const CanonicalForm & q );
//}}}

//{{{ function declarations from cf_factor.cc
EXTERN_VAR int singular_homog_flag;

bool isPurePoly(const CanonicalForm & f);

bool isPurePoly_m(const CanonicalForm & f);

CFFList FACTORY_PUBLIC factorize ( const CanonicalForm & f, bool issqrfree = false );

CFFList FACTORY_PUBLIC factorize ( const CanonicalForm & f, const Variable & alpha );

CFFList FACTORY_PUBLIC sqrFree ( const CanonicalForm & f, bool sort= false );

CanonicalForm homogenize( const CanonicalForm & f, const Variable & x);
CanonicalForm homogenize( const CanonicalForm & f, const Variable & x,
                                const Variable & v1, const Variable & v2);
Variable get_max_degree_Variable(const CanonicalForm & f);
CFList get_Terms( const CanonicalForm & f );
void getTerms( const CanonicalForm & f, const CanonicalForm & t, CFList & result );


//}}}

//{{{ function declarations from cf_linsys.cc
bool linearSystemSolve ( CFMatrix & M );

CanonicalForm FACTORY_PUBLIC determinant ( const CFMatrix & M, int n );
//}}}

//{{{ function declarations from cf_resultant.cc
CFArray subResChain ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

CanonicalForm FACTORY_PUBLIC resultant ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );
//}}}

/** inline CanonicalForm abs ( const CanonicalForm & f )
 *
 * abs() - return absolute value of `f'.
 *
 * The absolute value is defined in terms of the function
 * `sign()'.  If it reports negative sign for `f' than -`f' is
 * returned, otherwise `f'.
 *
 * This behaviour is most useful for integers and rationals.  But
 * it may be used to sign-normalize the leading coefficient of
 * arbitrary polynomials, too.
 *
 * Type info:
 * ----------
 * f: CurrentPP
 *
**/
inline CanonicalForm
abs ( const CanonicalForm & f )
{
    // it is not only more general to use `sign()' instead of a
    // direct comparison `f < 0', it is faster, too
    if ( sign( f ) < 0 )
        return -f;
    else
        return f;
}
//}}}

/*ENDPUBLIC*/

#endif /* ! INCL_CF_ALGORITHM_H */
