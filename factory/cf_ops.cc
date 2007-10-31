/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_ops.cc,v 1.10 2007-10-31 08:40:53 Singular Exp $ */

//{{{ docu
//
// cf_ops.cc - simple structural algorithms.
//
// A 'structural' algorithm is an algorithm which gives
// structural information on polynomials in contrast to a
// 'mathematical' algorithm which calculates some mathematical
// function.
//
// Compare these functions with the functions in cf_algorithm.cc,
// which are mathematical algorithms.
//
// Used by: allmost everywhere
//
// Header file: canonicalform.h
//
//}}}

#include <config.h>

#include "assert.h"

#include "canonicalform.h"
#include "variable.h"
#include "cf_iter.h"

//{{{ static Variable sv_x1, sv_x2;
//{{{ docu
//
// sv_x1, sv_x2 - variables to swap by swapvar() and replacevar.
//
// These variables are initialized by swapvar() such that sv_x1 <
// sv_x2.  They are used by swapvar_between() and swapvar_rec()
// to swap variables efficiently.
// Furthermore, sv_x1 and sv_x2 are used by replacevar() and
// replacevar_between().
//
//}}}
static Variable sv_x1, sv_x2;
//}}}

//{{{ static void swapvar_between ( const CanonicalForm & f, CanonicalForm & result, const CanonicalForm & term, int expx2 )
//{{{ docu
//
// swapvar_between() - replace occurences of sv_x1 in f with sv_x2.
//
// If Psi denotes the map which maps sv_x1 to sv_x2, this
// function returns
//
//   result + Psi(f) * term * sv_x1^expx2
//
// Used by: swapvar()
//
//}}}
static void
swapvar_between ( const CanonicalForm & f, CanonicalForm & result, const CanonicalForm & term, int expx2 )
{
    if ( f.inCoeffDomain() || f.mvar() < sv_x1 )
	// in this case, we do not have to replace anything
	result += term * power( sv_x1, expx2 ) * f;
    else  if ( f.mvar() == sv_x1 )
	// this is where the real work is done: this iterator
	// replaces sv_x1 with sv_x2
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += power( sv_x2, i.exp() ) * term * power( sv_x1, expx2 ) * i.coeff();
    else
	// f's level is larger than sv_x1: descend down
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    swapvar_between( i.coeff(), result, term * power( f.mvar(), i.exp() ), expx2 );
}
static CanonicalForm
swapvar_between1 ( const CanonicalForm & f )
{
    if ( f.inCoeffDomain() || f.mvar() < sv_x1 )
	// in this case, we do not have to replace anything
	return f;
    else  if ( f.mvar() == sv_x1 ) {
	// this is where the real work is done: this iterator
	// replaces sv_x1 with sv_x2
	CanonicalForm result;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += power( sv_x2, i.exp() ) * i.coeff();
	return result;
    }
    else {
	// f's level is larger than sv_x1: descend down
	CanonicalForm result;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += swapvar_between1( i.coeff() ) * power( f.mvar(), i.exp() );
	return result;
    }
}
//}}}

//{{{ static void swapvar_rec ( const CanonicalForm & f, CanonicalForm & result, const CanonicalForm & term )
//{{{ docu
//
// swapvar_between() - swap occurences of sv_x1 and sv_x2 in f.
//
// If Psi denotes the map which swaps sv_x1 and sv_x2, this
// function returns
//
//   result + Psi(f) * term
//
// Used by: swapvar()
//
//}}}
static void
swapvar_rec ( const CanonicalForm & f, CanonicalForm & result, const CanonicalForm & term )
{
    if ( f.inCoeffDomain() || f.mvar() < sv_x1 )
	// in this case, we do not have to swap anything
	result += term * f;
    else  if ( f.mvar() == sv_x2 )
	// this is where the real work is done: this iterator
	// replaces sv_x1 with sv_x2 in the coefficients of f and
	// remembers the exponents of sv_x2 in the last argument
	// of the call to swapvar_between()
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    swapvar_between( i.coeff(), result, term, i.exp() );
    else  if ( f.mvar() < sv_x2 )
	// sv_x2 does not occur in f, but sv_x1 does.  Replace it.
	swapvar_between( f, result, term, 0 );
    else
	// f's level is larger than sv_x2: descend down
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    swapvar_rec( i.coeff(), result, term * power( f.mvar(), i.exp() ) );
}
static CanonicalForm
swapvar_rec1 ( const CanonicalForm & f )
{
    if ( f.inCoeffDomain() || f.mvar() < sv_x1 )
	return f;
    else  if ( f.mvar() == sv_x2 ) {
	CanonicalForm result;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += swapvar_between1( i.coeff() ) * power( sv_x1, i.exp() );
	return result;
    }
    else  if ( f.mvar() < sv_x2 )
	return swapvar_between1( f );
    else {
	CanonicalForm result;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += swapvar_rec1( i.coeff() ) * power( f.mvar(), i.exp() );
	return result;
    }
}
//}}}

//{{{ CanonicalForm swapvar ( const CanonicalForm & f, const Variable & x1, const Variable & x2 )
//{{{ docu
//
// swapvar() - swap variables x1 and x2 in f.
//
// Returns the image of f under the map which maps x1 to x2 and
// x2 to x1.  This is done quite efficiently because it is used
// really often.  x1 and x2 should be polynomial variables.
//
//}}}
CanonicalForm
swapvar ( const CanonicalForm & f, const Variable & x1, const Variable & x2 )
{
    ASSERT( x1.level() > 0 && x2.level() > 0, "cannot swap algebraic Variables" );
    if ( f.inCoeffDomain() || x1 == x2 || ( x1 > f.mvar() && x2 > f.mvar() ) )
	return f;
    else {
	CanonicalForm result = 0;
	if ( x1 > x2 ) {
	    sv_x1 = x2; sv_x2 = x1;
	}
	else {
	    sv_x1 = x1; sv_x2 = x2;
	}
	if ( f.mvar() < sv_x2 )
	    // we only have to replace sv_x1 by sv_x2
	    swapvar_between( f, result, 1, 0 );
	else
	    // we really have to swap variables
	    swapvar_rec( f, result, 1 );
	return result;
    }
}
CanonicalForm
swapvar1 ( const CanonicalForm & f, const Variable & x1, const Variable & x2 )
{
    ASSERT( x1.level() > 0 && x2.level() > 0, "cannot swap algebraic variables" );
    if ( f.inCoeffDomain() || x1 == x2 || ( x1 > f.mvar() && x2 > f.mvar() ) )
	return f;
    else {
	CanonicalForm result = 0;
	if ( x1 > x2 ) {
	    sv_x1 = x2; sv_x2 = x1;
	}
	else {
	    sv_x1 = x1; sv_x2 = x2;
	}
	if ( f.mvar() < sv_x2 )
	    // we only have to replace sv_x1 by sv_x2
	    return swapvar_between1( f );
	else
	    // we really have to swap variables
	    return swapvar_rec1( f );
    }
}
//}}}

//{{{ static CanonicalForm replacevar_between ( const CanonicalForm & f )
//{{{ docu
//
// replacevar_between() - replace occurences of sv_x1 in f with sv_x2.
//
// This is allmost the same as swapvar_between() except that
// sv_x1 may be an algebraic variable, so we have to test on
// 'f.inBaseDomain()' instead of 'f.inCoeffDomain()' in the
// beginning.
//
// Used by: replacevar()
//
//}}}
static CanonicalForm
replacevar_between ( const CanonicalForm & f )
{
    if ( f.inBaseDomain() )
	return f;

    Variable x = f.mvar();

    if ( x < sv_x1 )
	// in this case, we do not have to replace anything
	return f;
    else  if ( x == sv_x1 ) {
	// this is where the real work is done: this iterator
	// replaces sv_x1 with sv_x2
	CanonicalForm result;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += power( sv_x2, i.exp() ) * i.coeff();
	return result;
    }
    else {
	// f's level is larger than sv_x1: descend down
	CanonicalForm result;
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    result += replacevar_between( i.coeff() ) * power( x, i.exp() );
	return result;
    }
}
//}}}

//{{{ CanonicalForm replacevar ( const CanonicalForm & f, const Variable & x1, const Variable & x2 )
//{{{ docu
//
// replacevar() - replace all occurences of x1 in f by x2.
//
// In contrast to swapvar(), x1 may be an algebraic variable, but
// x2 must be a polynomial variable.
//
//}}}
CanonicalForm
replacevar ( const CanonicalForm & f, const Variable & x1, const Variable & x2 )
{
    //ASSERT( x2.level() > 0, "cannot replace with algebraic variable" );
    if ( f.inBaseDomain() || x1 == x2 || ( x1 > f.mvar() ) )
	return f;
    else {
	sv_x1 = x1;
	sv_x2 = x2;
	return replacevar_between( f );
    }
}
//}}}

//{{{ static void fillVarsRec ( const CanonicalForm & f, int * vars )
//{{{ docu
//
// fillVarsRec - fill array describing occurences of variables in f.
//
// Only polynomial variables are looked up.  The information is
// stored in the arrary vars.  vars should be large enough to
// hold all information, i.e. larger than the level of f.
//
// Used by getVars() and getNumVars().
//
//}}}
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
//}}}

//{{{ int getNumVars ( const CanonicalForm & f )
//{{{ docu
//
// getNumVars() - get number of polynomial variables in f.
//
//}}}
int
getNumVars ( const CanonicalForm & f )
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

	// look for variables
	for ( CFIterator I = f; I.hasTerms(); ++I )
	    fillVarsRec( I.coeff(), vars );

	// count them
	int m = 0;
	for ( i = 1; i < n; i++ )
	    if ( vars[i] != 0 ) m++;

	delete [] vars;
	// do not forget to count our own variable
	return m+1;
    }
}
//}}}

//{{{ CanonicalForm getVars ( const CanonicalForm & f )
//{{{ docu
//
// getVars() - get polynomial variables of f.
//
// Return the product of all of them, 1 if there are not any.
//
//}}}
CanonicalForm
getVars ( const CanonicalForm & f )
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

	// look for variables
	for ( CFIterator I = f; I.hasTerms(); ++I )
	    fillVarsRec( I.coeff(), vars );

	// multiply them all
	CanonicalForm result = 1;
	for ( i = n; i > 0; i-- )
	    if ( vars[i] != 0 ) result *= Variable( i );

	delete [] vars;
	// do not forget our own variable
	return f.mvar() * result;
    }
}
//}}}

//{{{ CanonicalForm apply ( const CanonicalForm & f, void (*mf)( CanonicalForm &, int & ) )
//{{{ docu
//
// apply() - apply mf to terms of f.
//
// Calls mf( f[i], i ) for each term f[i]*x^i of f and builds a
// new term from the result.  If f is in a coefficient domain,
// mf( f, i ) should result in an i == 0, since otherwise it is
// not clear which variable to use for the resulting term.
//
// An example:
//
// void
// diff( CanonicalForm & f, int & i )
// {
//     f = f * i;
//     if ( i > 0 ) i--;
// }
//
// Then apply( f, diff ) is differentation of f with respect to the
// main variable of f.
//
//}}}
CanonicalForm
apply ( const CanonicalForm & f, void (*mf)( CanonicalForm &, int & ) )
{
    if ( f.inCoeffDomain() ) {
	int exp = 0;
	CanonicalForm result = f;
	mf( result, exp );
	ASSERT( exp == 0, "illegal result, do not know what variable to use" );
	return result;
    }
    else {
	CanonicalForm result, coeff;
	CFIterator i;
	int exp;
	Variable x = f.mvar();
	for ( i = f; i.hasTerms(); i++ ) {
	    coeff = i.coeff();
	    exp = i.exp();
	    mf( coeff, exp );
	    if ( ! coeff.isZero() )
		result += power( x, exp ) * coeff;
	}
	return result;
    }
}
//}}}

//{{{ CanonicalForm mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) )
//{{{ docu
//
// mapdomain() - map all coefficients of f through mf.
//
// Recursively descends down through f to the coefficients which
// are in a coefficient domain mapping each such coefficient
// through mf and returns the result.
//
//}}}
CanonicalForm
mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) )
{
    if ( f.inCoeffDomain() )
	return mf( f );
    else {
	CanonicalForm result = 0;
	CFIterator i;
	Variable x = f.mvar();
	for ( i = f; i.hasTerms(); i++ )
	    result += power( x, i.exp() ) * mapdomain( i.coeff(), mf );
	return result;
    }
}
//}}}

//{{{ static void degreesRec ( const CanonicalForm & f, int * degs )
//{{{ docu
//
// degreesRec() - recursively get degrees of f.
//
// Used by degrees().
//
//}}}
static void
degreesRec ( const CanonicalForm & f, int * degs )
{
    if ( ! f.inCoeffDomain() ) {
	int level = f.level();
	int deg = f.degree();
	// calculate the maximum degree of all coefficients which
	// are in the same level
	if ( degs[level] < deg )
	    degs[level] = f.degree();
	for ( CFIterator i = f; i.hasTerms(); i++ )
	    degreesRec( i.coeff(), degs );
    }
}
//}}}

//{{{ int * degrees ( const CanonicalForm & f, int * degs )
//{{{ docu
//
// degress() - return the degrees of all polynomial variables in f.
//
// Returns 0 if f is in a coefficient domain, the degrees of f in
// all its polynomial variables in an array of int otherwise:
//
//   degrees( f, 0 )[i] = degree( f, Variable(i) )
//
// If degs is not the zero pointer the degrees are stored in this
// array.  In this case degs should be larger than the level of
// f.  If degs is the zero pointer, an array of sufficient size
// is allocated automatically.
//
//}}}
int *
degrees ( const CanonicalForm & f, int * degs )
{
    if ( f.inCoeffDomain() )
	return 0;
    else {
	int level = f.level();
	if ( degs == 0 )
	    degs = new int[level+1];
	for ( int i = 0; i <= level; i++ )
	    degs[i] = 0;
	degreesRec( f, degs );
	return degs;
    }
}
//}}}

//{{{ int totaldegree ( const CanonicalForm & f )
//{{{ docu
//
// totaldegree() - return the total degree of f.
//
// If f is zero, return -1.  If f is in a coefficient domain,
// return 0.  Otherwise return the total degree of f in all
// polynomial variables.
//
//}}}
int
totaldegree ( const CanonicalForm & f )
{
    if ( f.isZero() )
	return -1;
    else if ( f.inCoeffDomain() )
	return 0;
    else
    {
	CFIterator i;
	int cdeg = 0, dummy;
	// calculate maximum over all coefficients of f, taking
	// in account our own exponent
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff() ) + i.exp()) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
}
//}}}

//{{{ int totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 )
//{{{ docu
//
// totaldegree() - return the total degree of f as a polynomial
//   in the polynomial variables between v1 and v2 (inclusively).
//
// If f is zero, return -1.  If f is in a coefficient domain,
// return 0.  Also, return 0 if v1 > v2.  Otherwise, take f to be
// a polynomial in the polynomial variables between v1 and v2 and
// return its total degree.
//
//}}}
int
totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 )
{
    if ( f.isZero() )
	return -1;
    else if ( v1 > v2 )
	return 0;
    else if ( f.inCoeffDomain() )
	return 0;
    else if ( f.mvar() < v1 )
	return 0;
    else if ( f.mvar() == v1 )
	return f.degree();
    else if ( f.mvar() > v2 ) {
	// v2's level is larger than f's level, descend down
	CFIterator i;
	int cdeg = 0, dummy;
	// calculate maximum over all coefficients of f
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff(), v1, v2 )) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
    else {
	// v1 < f.mvar() <= v2
	CFIterator i;
	int cdeg = 0, dummy;
	// calculate maximum over all coefficients of f, taking
	// in account our own exponent
	for ( i = f; i.hasTerms(); i++ )
	    if ( (dummy = totaldegree( i.coeff(), v1, v2 ) + i.exp()) > cdeg )
		cdeg = dummy;
	return cdeg;
    }
}
//}}}

//{{{ int size ( const CanonicalForm & f, const Variable & v )
//{{{ docu
//
// size() - count number of monomials of f with level higher
//   or equal than level of v.
//
// Returns one if f is in an base domain.
//
//}}}
int
size ( const CanonicalForm & f, const Variable & v )
{
    if ( f.inBaseDomain() )
	return 1;

    int result = 0;
    CFIterator i;
    if ( f.mvar() < v )
	// polynomials with level < v1 are counted as coefficients
	return 1;
    else {
	// polynomials with level > v2 are not counted al all
	for ( i = f; i.hasTerms(); i++ )
	    result += size( i.coeff(), v );
	return result;
    }
}
//}}}

//{{{ int size ( const CanonicalForm & f )
//{{{ docu
//
// size() - return number of monomials in f which are in an
//   coefficient domain.
//
// Returns one if f is in an coefficient domain.
//
//}}}
int
size ( const CanonicalForm & f )
{
    if ( f.inCoeffDomain() )
	return 1;
    else {
	int result = 0;
	CFIterator i;
	for ( i = f; i.hasTerms(); i++ )
	    result += size( i.coeff() );
	return result;
    }
}
//}}}
