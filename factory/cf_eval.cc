// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_eval.cc,v 1.0 1996-05-17 10:59:43 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "cf_defs.h"
#include "cf_eval.h"

static CanonicalForm evalCF ( const CanonicalForm & f, const CFArray & a, int m, int n );


Evaluation& Evaluation::operator= ( const Evaluation & e )
{
    if ( this != &e ) {
	values = e.values;
    }
    return *this;
}

CanonicalForm
Evaluation::operator() ( const CanonicalForm & f ) const
{
    if ( f.inCoeffDomain() || f.level() < values.min() )
	return f;
    else  if ( f.level() < values.max() )
	return evalCF( f, values, 2, f.level() );
    else
	return evalCF( f, values, 2, values.max() );
}

CanonicalForm
Evaluation::operator() ( const CanonicalForm & f, int i, int j ) const
{
    if ( i > j )
	return f;
    return evalCF( f, values, i, j );
}

void
Evaluation::nextpoint()
{
    int n = values.max();
    for ( int i = values.min(); i <= n; i++ )
	values[i] += 1;
}

ostream&
operator<< ( ostream& s, const Evaluation &e )
{
    s << e.values;
    return s;
}

CanonicalForm
evalCF ( const CanonicalForm & f, const CFArray & a, int m, int n )
{
    if ( m > n )
	return f;
    else {
	CanonicalForm result = f;
	while ( n >= m ) {
	    result = result( a[n], Variable( n ) );
	    n--;
	}
	return result;
    }
//    iterated method turned out to be faster than
//    return evalCF( f( a[n], Variable( n ) ), a, m, n-1 );
}
