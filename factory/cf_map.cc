// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_map.cc,v 1.0 1996-05-17 10:59:44 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "cf_defs.h"
#include "cf_map.h"
#include "cf_iter.h"


static int cmpfunc ( const MapPair & p1, const MapPair & p2 );
static void insfunc ( MapPair & orgp, const MapPair & newp );
static CanonicalForm subsrec( const CanonicalForm & f, const ListIterator<MapPair> & i );

MapPair&
MapPair::operator= ( const MapPair & p )
{
    if ( this != &p ) {
	V = p.V;
	S = p.S;
    }
    return *this;
}

ostream&
operator << ( ostream& s, const MapPair & p )
{
    s << p.var() << " -> " << p.subst();
    return s;
}

CFMap::CFMap ( const List<CanonicalForm> & L )
{
    ListIterator<CanonicalForm> i;
    int j;
    for ( i = L, j = 1; i.hasItem(); i++, j++ )
	P.append( MapPair( Variable(j), i.getItem() ) );
}

CFMap&
CFMap::operator= ( const CFMap & m )
{
    if ( this != &m )
	P = m.P;
    return *this;
}

void
CFMap::newpair( const Variable & v, const CanonicalForm & s )
{
    P.insert( MapPair( v, s ), cmpfunc, insfunc );
}

CanonicalForm
CFMap::operator() ( const CanonicalForm & f ) const
{
    ListIterator<MapPair> i = P;
    return subsrec( f, i );
}

ostream&
operator<< ( ostream& s, const CFMap & m )
{
    if ( m.P.isEmpty() )
	s << "( )";
    else {
	ListIterator<MapPair> i = m.P;
	s << "( " << i.getItem();
	i++;
	while ( i.hasItem() ) {
	    s << ", " << i.getItem();
	    i++;
	}
	s << " )";
    }
    return s;
}

CanonicalForm
compress ( const CanonicalForm & f, CFMap & m )
{
    CanonicalForm result = f;
    int i, n;
    int * degs = degrees( f );

    m = CFMap();
    n = i = 1;
    while ( i <= level( f ) ) {
	while( degs[i] == 0 ) i++;
	if ( i != n ) {
	    m.newpair( Variable( n ), Variable( i ) );
	    result = swapvar( result, Variable( i ), Variable( n ) );
	}
	n++; i++;
    }
    delete [] degs;
    return result;
}

void
compress ( const CFArray & a, CFMap & M, CFMap & N )
{
    M = N = CFMap();
    if ( a.size() == 0 )
	return;
    int maxlevel = level( a[a.min()] );
    int i, j;
    for ( i = a.min() + 1; i <= a.max(); i++ )
	if ( level( a[i] ) > maxlevel )
	    maxlevel = level( a[i] );
    if ( maxlevel <= 0 )
	return;
    int * degs = new int[maxlevel+1];
    int * tmp = new int[maxlevel+1];
    for ( i = 1; i <= maxlevel; i++ )
	degs[i] = 0;
    for ( i = a.min(); i <= a.max(); i++ ) {
	tmp = degrees( a[i], tmp );
	for ( j = 1; j <= level( a[i] ); j++ )
	    if ( tmp[j] != 0 )
		degs[j] = 1;
    }
    i = 1;
    j = 1;
    while ( i <= maxlevel ) {
	if ( degs[i] != 0 ) {
	    M.newpair( Variable(i), Variable(j) );
	    N.newpair( Variable(j), Variable(i) );
	    j++;
	}
	i++;
    }
    delete [] tmp;
    delete [] degs;
}

// static functions

int
cmpfunc ( const MapPair & p1, const MapPair & p2 )
{
    if ( p1.var() > p2.var() ) return -1;
    else if ( p1.var() == p2.var() ) return 0;
    else return 1;
}

void
insfunc ( MapPair & orgp, const MapPair & newp )
{
    orgp = newp;
}

CanonicalForm
subsrec( const CanonicalForm & f, const ListIterator<MapPair> & i )
{
    if ( f.inBaseDomain() ) return f;
    ListIterator<MapPair> j = i;
    while ( j.hasItem() && j.getItem().var() > f.mvar() ) j++;
    if ( j.hasItem() )
	if ( j.getItem().var() != f.mvar() ) {
	    CanonicalForm result = 0;
	    CFIterator I;
	    for ( I = f; I.hasTerms(); I++ )
		result += power( f.mvar(), I.exp() ) * subsrec( I.coeff(), j );
	    return result;
	}
	else {
	    CanonicalForm result = 0, s = j.getItem().subst();
	    CFIterator I;
	    j++;
	    for ( I = f; I.hasTerms(); I++ )
		result += subsrec( I.coeff(), j ) * power( s, I.exp() );
	    return result;
	}
    else
	return f;
}


