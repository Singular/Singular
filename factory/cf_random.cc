// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_random.cc,v 1.1 1997-04-07 16:10:17 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:44  stobbe
Initial revision

*/

#include <config.h>

#include <stdlib.h>
#include <math.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_random.h"
#include "ffops.h"
#include "gfops.h"
#include "imm.h"


CanonicalForm FFRandom::generate () const
{
    return CanonicalForm( int2imm_p( factoryrandom( ff_prime ) ) );
}

CFRandom * FFRandom::clone () const
{
    return new FFRandom();
}

CanonicalForm GFRandom::generate () const
{
    return CanonicalForm( int2imm_gf( factoryrandom( gf_q ) ) );
}

CFRandom * GFRandom::clone () const
{
    return new GFRandom();
}


IntRandom::IntRandom()
{
    max = 100;
}

IntRandom::IntRandom( int m )
{
    max = m;
}

IntRandom::~IntRandom() {}

CanonicalForm IntRandom::generate() const
{
    return factoryrandom( max );
}

CFRandom * IntRandom::clone() const
{
    return new IntRandom( max );
}

AlgExtRandomF::AlgExtRandomF()
{
    ASSERT( 0, "not a valid random generator" );
}

AlgExtRandomF::AlgExtRandomF( const AlgExtRandomF & )
{
    ASSERT( 0, "not a valid random generator" );
}

AlgExtRandomF& AlgExtRandomF::operator= ( const AlgExtRandomF & )
{
    ASSERT( 0, "not a valid random generator" );
    return *this;
}

AlgExtRandomF::AlgExtRandomF( const Variable & v )
{
    ASSERT( v.level() < 0, "not an algebraic extension" );
    algext = v;
    n = degree( getMipo( v ) );
    gen = CFRandomFactory::generate();
}

AlgExtRandomF::AlgExtRandomF( const Variable & v1, const Variable & v2 )
{
    ASSERT( v1.level() < 0 && v2.level() < 0 && v1 != v2, "not an algebraic extension" );
    algext = v2;
    n = degree( getMipo( v2 ) );
    gen = new AlgExtRandomF( v1 );
}

AlgExtRandomF::AlgExtRandomF( const Variable & v, CFRandom * g, int nn )
{
    algext = v;
    n = nn;
    gen = g;
}

AlgExtRandomF::~AlgExtRandomF()
{
    delete gen;
}

CanonicalForm AlgExtRandomF::generate() const
{
    CanonicalForm result;
    for ( int i = 0; i < n; i++ )
	result += power( algext, i ) * gen->generate();
    return result;
};

CFRandom * AlgExtRandomF::clone () const
{
    return new AlgExtRandomF( algext, gen->clone(), n );
}

CFRandom * CFRandomFactory::generate()
{
    if ( getCharacteristic() == 0 )
	return new IntRandom();
    if ( getGFDegree() > 1 )
	return new GFRandom();
    else
	return new FFRandom();
}

int factoryrandom( int n )
{
    return rand() % n;
}

void factoryseed ( int s )
{
    srand( s );
}
