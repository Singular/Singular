/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "fac_iterfor.h"

#ifndef HAVE_NTL

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

IteratedFor::IteratedFor( int from, int to, int max ) : MAX( max ), FROM( from ), TO( to ), N( TO-FROM ), last( false )
{
    ASSERT( N >= 0 && max >= 0, "illegal iterated for" );
    index = new int[N+1];
    imax = new int[N+1];
    fill( 0, max );
}

IteratedFor::IteratedFor( const IteratedFor & I ) : MAX( I.MAX ), FROM( I.FROM ), TO( I.TO ), N( I.N ), last( I.last )
{
    index = new int[N+1];
    imax = new int[N+1];
    for ( int i = 0; i <= N; i++ ) {
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
        FROM = I.FROM;
        TO = I.TO;
        MAX = I.MAX;
        last = I.last;
        for ( int i = 0; i<= N; i++ ) {
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
    if ( index[0] == MAX )
        last = true;
    else {
        if ( index[N-1] != imax[N-1] ) {
            index[N-1]++;
            index[N]--;
        }
        else {
            int i = N-1, m = index[N];
            while ( i > 0 && index[i] == imax[i] ) {
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
    ASSERT( i >= FROM && i <= TO, "illegal index" );
    return index[i-FROM];
}

#ifndef NOSTREAMIO
OSTREAM& operator<< ( OSTREAM& os, const IteratedFor & I )
{
    os << "( " << I[I.from()];
    for ( int i = I.from()+1; i <= I.to(); i++ )
        os << ", " << I[i];
    os << " )";
    return os;
}
#endif /* NOSTREAMIO */
#endif
