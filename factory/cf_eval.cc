// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_eval.cc,v 1.3 1997-04-07 15:00:58 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/03/26 16:29:50  schmidt
stream-io wrapped by NOSTREAMIO

Revision 1.1  1996/05/24 09:21:54  stobbe
"operator(): Bug fix, operator( const CanonicalForm & ) now uses
            values.min() as lower index instead of 2 and produces the
            right result.
"

// Revision 1.0  1996/05/17  10:59:43  stobbe
// Initial revision
//
*/

#include <config.h>

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
	return evalCF( f, values, values.min(), f.level() );
    else
	return evalCF( f, values, values.min(), values.max() );
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

#ifndef NOSTREAMIO
ostream&
operator<< ( ostream& s, const Evaluation &e )
{
    s << e.values;
    return s;
}
#endif /* NOSTREAMIO */

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
