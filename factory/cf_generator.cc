// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_generator.cc,v 1.0 1996-05-17 10:59:43 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "cf_defs.h"
#include "cf_generator.h"
#include "imm.h"
#include "gfops.h"
#include "ffops.h"

bool FFGenerator::hasItems() const
{
    return current < ff_prime;
}

CanonicalForm FFGenerator::item() const
{
    ASSERT( current < ff_prime, "no more items" );
    return CanonicalForm( int2imm_p( current ) );
}

void FFGenerator::next()
{
    ASSERT( current < ff_prime, "no more items" );
    current++;
}


GFGenerator::GFGenerator()
{
    current = gf_zero();
}

bool GFGenerator::hasItems() const
{
    return ( current != gf_q + 1 );
}

void GFGenerator::reset()
{
    current = gf_zero();
}

CanonicalForm GFGenerator::item() const
{
    ASSERT( current != gf_q + 1, "no more items" );
    return CanonicalForm( int2imm_gf( current ) );
}

void GFGenerator::next()
{
    ASSERT( current != gf_q + 1, "no more items" );
    if ( gf_iszero( current ) )
	current = 0;
    else  if ( current == gf_q1 - 1 )
	current = gf_q + 1;
    else
	current++;
}

AlgExtGenerator::AlgExtGenerator()
{
    ASSERT( 0, "not a valid generator" );
}

AlgExtGenerator::AlgExtGenerator( const AlgExtGenerator & )
{
    ASSERT( 0, "not a valid generator" );
}    

AlgExtGenerator& AlgExtGenerator::operator= ( const AlgExtGenerator & )
{
    ASSERT( 0, "not a valid generator" );
    return *this;
}

AlgExtGenerator::AlgExtGenerator( const Variable & a )
{
    ASSERT( a.level() < 0, "not an algebraic extension" );
    ASSERT( getCharacteristic() > 0, "not a finite field" );
    algext = a;
    n = degree( getMipo( a ) );
    gens = new CFGenerator * [n];
    nomoreitems = false;
    for ( int i = 0; i < n; i++ )
	gens[i] = CFGenFactory::generate();
}

AlgExtGenerator::~AlgExtGenerator()
{
    for ( int i = 0; i < n; i++ )
	delete gens[i];
    delete [] gens;
}

void AlgExtGenerator::reset()
{
    for ( int i = 0; i < n; i++ )
	gens[i]->reset();
    nomoreitems = false;
}

CanonicalForm AlgExtGenerator::item() const
{
    ASSERT( ! nomoreitems, "no more items" );
    CanonicalForm result = 0;
    for ( int i = 0; i < n; i++ )
	result += power( algext, i ) * gens[i]->item();
    return result;
}

void AlgExtGenerator::next()
{
    ASSERT( ! nomoreitems, "no more items" );
    int i = 0;
    bool stop = false;
    while ( ! stop && i < n ) {
	gens[i]->next();
	if ( ! gens[i]->hasItems() ) {
	    gens[i]->reset();
	    i++;
	}
	else
	    stop = true;
    }
    if ( ! stop )
	nomoreitems = true;
}

CFGenerator * CFGenFactory::generate()
{
    ASSERT( getCharacteristic() > 0, "not a finite field" );
    if ( getGFDegree() > 1 )
	return new GFGenerator();
    else
	return new FFGenerator();
}
