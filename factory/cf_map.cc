/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// cf_map.cc - definition of class CFMap.
//
// Used by: cf_gcd.cc, fac_multivar.cc, sm_sparsemod.cc
//
//}}}

#include <config.h>

#include "canonicalform.h"
#include "cf_map.h"
#include "cf_iter.h"
#include "ftmpl_functions.h"

//{{{ MapPair & MapPair::operator = ( const MapPair & p )
//{{{ docu
//
// MapPair::operator = - assignment operator.
//
//}}}
MapPair &
MapPair::operator = ( const MapPair & p )
{
    if ( this != &p ) {
        V = p.V;
        S = p.S;
    }
    return *this;
}
//}}}

#ifndef NOSTREAMIO
//{{{ OSTREAM & operator << ( OSTREAM & s, const MapPair & p )
//{{{ docu
//
// operator << - print a map pair ("V -> S").
//
//}}}
OSTREAM &
operator << ( OSTREAM & s, const MapPair & p )
{
    s << p.var() << " -> " << p.subst();
    return s;
}
//}}}

void MapPair::print( OSTREAM&) const
{
}
#endif /* NOSTREAMIO */

//{{{ CFMap::CFMap ( const CFList & L )
//{{{ docu
//
// CFMap::CFMap() - construct a CFMap from a CFList.
//
// Variable[i] will be mapped to CFList[i] under the resulting
// map.
//
//}}}
CFMap::CFMap ( const CFList & L )
{
    CFListIterator i;
    int j;
    for ( i = L, j = 1; i.hasItem(); i++, j++ )
        P.insert( MapPair( Variable(j), i.getItem() ) );
}
//}}}

//{{{ CFMap & CFMap::operator = ( const CFMap & m )
//{{{ docu
//
// CFMap::operator = - assignment operator.
//
//}}}
CFMap &
CFMap::operator = ( const CFMap & m )
{
    if ( this != &m )
        P = m.P;
    return *this;
}
//}}}

//{{{ static int cmpfunc ( const MapPair & p1, const MapPair & p2 )
//{{{ docu
//
// cmpfunc() - compare two map pairs.
//
// Return -1 if p2's variable is less than p1's, 0 if they are
// equal, 1 if p2's level is greater than p1's.
//
//}}}
static int
cmpfunc ( const MapPair & p1, const MapPair & p2 )
{
    if ( p1.var() > p2.var() ) return -1;
    else if ( p1.var() == p2.var() ) return 0;
    else return 1;
}
//}}}

//{{{ static void insfunc ( MapPair & orgp, const MapPair & newp )
//{{{ docu
//
// insfunc() - assign newp to orgp.
//
// cmpfunc() and insfunc() are used as functions for inserting a
// map pair into a map by CFMap::newpair().
//
//}}}
static void
insfunc ( MapPair & orgp, const MapPair & newp )
{
    orgp = newp;
}
//}}}

//{{{ void CFMap::newpair ( const Variable & v, const CanonicalForm & s )
//{{{ docu
//
// CFMap::newpair() - insert a MapPair into a CFMap.
//
//}}}
void
CFMap::newpair ( const Variable & v, const CanonicalForm & s )
{
    P.insert( MapPair( v, s ), cmpfunc, insfunc );
}
//}}}

//{{{ static CanonicalForm subsrec ( const CanonicalForm & f, const MPListIterator & i )
//{{{ docu
//
// subsrec() - recursively apply the substitutions in i to f.
//
// Substitutes algebraic variables, too.  The substituted
// expression are not subject to further substitutions.
//
// Used by: CFMap::operator ()().
//
//}}}
static CanonicalForm
subsrec ( const CanonicalForm & f, const MPListIterator & i )
{
    if ( f.inBaseDomain() ) return f;
    MPListIterator j = i;

    // skip MapPairs larger than the main variable of f
    while ( j.hasItem() && j.getItem().var() > f.mvar() ) j++;

    if ( j.hasItem() )
        if ( j.getItem().var() != f.mvar() ) {
            // simply descend if the current MapPair variable is
            // not the main variable of f
            CanonicalForm result = 0;
            CFIterator I;
            for ( I = f; I.hasTerms(); I++ )
                result += power( f.mvar(), I.exp() ) * subsrec( I.coeff(), j );
            return result;
        }
        else {
            // replace the main variable of f with the image of
            // the current variable under MapPair
            CanonicalForm result = 0;
            CanonicalForm s = j.getItem().subst();
            CFIterator I;
            // move on to the next MapPair
            j++;
            for ( I = f; I.hasTerms(); I++ )
                result += subsrec( I.coeff(), j ) * power( s, I.exp() );
            return result;
        }
    else
        return f;
}
//}}}

//{{{ CanonicalForm CFMap::operator () ( const CanonicalForm & f ) const
//{{{ docu
//
// CFMap::operator () - apply CO to f.
//
// See subsrec() for more detailed information.
//
//}}}
CanonicalForm
CFMap::operator () ( const CanonicalForm & f ) const
{
    MPListIterator i = P;
    return subsrec( f, i );
}
//}}}

#ifndef NOSTREAMIO
//{{{ OSTREAM & operator << ( OSTREAM & s, const CFMap & m )
//{{{ docu
//
// operator << - print a CFMap ("( V[1] -> S[1], ..., V[n] -> // S[n] )".
//
//}}}
OSTREAM &
operator << ( OSTREAM & s, const CFMap & m )
{
    m.P.print(s);
    return s;
}
//}}}
#endif /* NOSTREAMIO */

//{{{ CanonicalForm compress ( const CanonicalForm & f, CFMap & m )
//{{{ docu
//
// compress() - compress the canonical form f.
//
// Compress the polynomial f such that the levels of its
// polynomial variables are ordered without any gaps starting
// from level 1.  Return the compressed polynomial and a map m to
// undo the compression.  That is, if f' = compress(f, m), than f
// = m(f').
//
//}}}
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
            // swap variables and remember the swap in the map
            m.newpair( Variable( n ), Variable( i ) );
            result = swapvar( result, Variable( i ), Variable( n ) );
        }
        n++; i++;
    }
    delete [] degs;
    return result;
}
//}}}

//{{{ void compress ( const CFArray & a, CFMap & M, CFMap & N )
//{{{ docu
//
// compress() - compress the variables occuring in an a.
//
// Compress the polynomial variables occuring in a so that their
// levels are ordered without any gaps starting from level 1.
// Return the CFMap M to realize the compression and its inverse,
// the CFMap N.  Note that if you compress a member of a using M
// the result of the compression is not necessarily compressed,
// since the map is constructed using all variables occuring in
// a.
//
//}}}
void
compress ( const CFArray & a, CFMap & M, CFMap & N )
{
    M = N = CFMap();
    if ( a.size() == 0 )
        return;
    int maxlevel = level( a[a.min()] );
    int i, j;

    // get the maximum of levels in a
    for ( i = a.min() + 1; i <= a.max(); i++ )
        if ( level( a[i] ) > maxlevel )
            maxlevel = level( a[i] );
    if ( maxlevel <= 0 )
        return;

    int * degs = new int[maxlevel+1];
    int * tmp = new int[maxlevel+1];
    for ( i = 1; i <= maxlevel; i++ )
        degs[i] = 0;

    // calculate the union of all levels occuring in a
    for ( i = a.min(); i <= a.max(); i++ ) {
        tmp = degrees( a[i], tmp );
        for ( j = 1; j <= level( a[i] ); j++ )
            if ( tmp[j] != 0 )
                degs[j] = 1;
    }

    // create the maps
    i = 1; j = 1;
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
//}}}

/*
*  compute positions p1 and pe of optimal variables:
*    pe is used in "ezgcd" and
*    p1 in "gcd_poly1"
*/
static
void optvalues ( const int * df, const int * dg, const int n, int & p1, int &pe )
{
    int i, o1, oe;
    i = p1 = pe = 0;
    do
    {
        i++;
        if ( i > n ) return;
    } while ( ( df[i] == 0 ) || ( dg[i] == 0 ) );
    p1 = pe = i;
    if ( df[i] > dg[i] )
    {
        o1 = df[i]; oe = dg[i];
    }
    else
    {
        o1 = dg[i]; oe = df[i];
    }
    while ( i < n )
    {
        i++;
        if ( ( df[i] != 0 ) && ( dg[i] != 0 ) )
        {
            if ( df[i] > dg[i] )
            {
                if ( o1 >= df[i]) { o1 = df[i]; p1 = i; }
                if ( oe < dg[i]) { oe = dg[i]; pe = i; }
            }
            else
            {
                if ( o1 >= dg[i]) { o1 = dg[i]; p1 = i; }
                if ( oe < df[i]) { oe = df[i]; pe = i; }
            }
        }
    }
}


//{{{ void compress ( const CanonicalForm & f, const CanonicalForm & g, CFMap & M, CFMap & N )
//{{{ docu
//
// compress() - compress the variables occurring in f and g with respect
// to optimal variables
//
// Compress the polynomial variables occurring in f and g so that
// the levels of variables common to f and g are ordered without
// any gaps starting from level 1, whereas the variables occuring
// in only one of f or g are moved to levels higher than the
// levels of the common variables.  Return the CFMap M to realize
// the compression and its inverse, the CFMap N.
// N needs only variables common to f and g.
//
//}}}
void
compress ( const CanonicalForm & f, const CanonicalForm & g, CFMap & M, CFMap & N )
{
    int n = tmax( f.level(), g.level() );
    int i, k, p1, pe;
    int * degsf = new int[n+1];
    int * degsg = new int[n+1];

    for ( i = 0; i <= n; i++ )
    {
        degsf[i] = degsg[i] = 0;
    }

    degsf = degrees( f, degsf );
    degsg = degrees( g, degsg );
    optvalues( degsf, degsg, n, p1, pe );

    i = 1; k = 1;
    if ( pe > 1 )
    {
        M.newpair( Variable(pe), Variable(k) );
        N.newpair( Variable(k), Variable(pe) );
        k++;
    }
    while ( i <= n )
    {
        if ( degsf[i] > 0 && degsg[i] > 0 )
        {
            if ( ( i != k ) && ( i != pe ) && ( i != p1 ) )
            {
                M.newpair( Variable(i), Variable(k) );
                N.newpair( Variable(k), Variable(i) );
            }
            k++;
        }
        i++;
    }
    if ( p1 != pe )
    {
        M.newpair( Variable(p1), Variable(k) );
        N.newpair( Variable(k), Variable(p1) );
        k++;
    }
    i = 1;
    while ( i <= n )
    {
        if ( degsf[i] > 0 && degsg[i] == 0 ) {
            if ( i != k )
            {
                M.newpair( Variable(i), Variable(k) );
                k++;
            }
        }
        else if ( degsf[i] == 0 && degsg[i] > 0 )
        {
            if ( i != k )
            {
                M.newpair( Variable(i), Variable(k) );
                k++;
            }
        }
        i++;
    }

    delete [] degsf;
    delete [] degsg;
}
//}}}
