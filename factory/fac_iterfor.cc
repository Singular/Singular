// emacs edit mode for this file is -*- C++ -*-
// $Id: fac_iterfor.cc,v 1.0 1996-05-17 10:59:45 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "cf_defs.h"
#include "fac_iterfor.h"

void
IteratedFor::fill ( int from, int max )
{
    while ( from < N ) {
	imax[from] = max;
	index[from] = 0;
	from++;
    }
    index[N] = max;
}

IteratedFor::IteratedFor( int n, int max ) : MAX( max ), N( n ), last( false )
{
    ASSERT( n > 1 && max >= 0, "illegal iterated for" );
    index = new int[n+1];
    imax = new int[n+1];
    fill( 2, max );
}

IteratedFor::IteratedFor( const IteratedFor & I ) : MAX( I.MAX ), N( I.N ), last( I.last )
{
    index = new int[N+1];
    imax = new int[N+1];
    for ( int i = 2; i<= N; i++ ) {
	index[i] = I.index[i];
	imax[i] = I.imax[i];
    }
}

IteratedFor::~IteratedFor()
{
    delete [] index;
    delete [] imax;
}

IteratedFor&
IteratedFor::operator= ( const IteratedFor & I )
{
    if ( this != &I ) {
	if ( N != I.N ) {
	    N = I.N;
	    delete [] index;
	    delete [] imax;
	    index = new int[N+1];
	    imax = new int[N+1];
	}
	MAX = I.MAX;
	last = I.last;
	for ( int i = 2; i<= N; i++ ) {
	    index[i] = I.index[i];
	    imax[i] = I.imax[i];
	}
    }
    return *this;
}

void
IteratedFor::nextiteration()
{
    ASSERT( ! last, "no more iterations" );
    if ( index[2] == MAX )
	last = true;
    else {
	if ( index[N-1] != imax[N-1] ) {
	    index[N-1]++;
	    index[N]--;
	}
	else {
	    int i = N-1, m = index[N];
	    while ( i > 1 && index[i] == imax[i] ) {
		m += imax[i];
		i--;
	    }
	    index[i]++; m--;
	    fill( i+1, m );
	}
    }
}

int
IteratedFor::operator[] ( int i ) const
{
    ASSERT( i >= 2 && i <= N, "illegal index" );
    return index[i];
}

ostream& operator<< ( ostream& os, const IteratedFor & I )
{
    os << "( " << I[2];
    for ( int i = 3; i <= I.n(); i++ )
	os << ", " << I[i];
    os << " )";
    return os;
}
