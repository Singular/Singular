/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

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
    if ( getGFDegree() > 1 )
    {
      gensg = new GFGenerator * [n];
      for ( int i = 0; i < n; i++ )
        gensg[i] = new GFGenerator();
    }
    else
    {
      gensf = new FFGenerator * [n];
      for ( int i = 0; i < n; i++ )
        gensf[i] = new FFGenerator();
    }
    nomoreitems = false;
}

AlgExtGenerator::~AlgExtGenerator()
{
    if ( getGFDegree() > 1 )
    {
      for ( int i = 0; i < n; i++ )
        delete gensg[i];
      delete [] gensg;
    }
    else
    {
      for ( int i = 0; i < n; i++ )
        delete gensf[i];
      delete [] gensf;
    }
}

void AlgExtGenerator::reset()
{
    if ( getGFDegree() > 1 )
    {
      for ( int i = 0; i < n; i++ )
        gensg[i]->reset();
    }
    else
    {
      for ( int i = 0; i < n; i++ )
        gensf[i]->reset();
    }
    nomoreitems = false;
}

CanonicalForm AlgExtGenerator::item() const
{
    ASSERT( ! nomoreitems, "no more items" );
    CanonicalForm result = 0;
    if ( getGFDegree() > 1 )
    {
      for ( int i = 0; i < n; i++ )
            result += power( algext, i ) * gensg[i]->item();
    }
    else
    {
      for ( int i = 0; i < n; i++ )
            result += power( algext, i ) * gensf[i]->item();
    }
    return result;
}

void AlgExtGenerator::next()
{
    ASSERT( ! nomoreitems, "no more items" );
    int i = 0;
    bool stop = false;
    if ( getGFDegree() > 1 )
    {
      while ( ! stop && i < n )
      {
            gensg[i]->next();
            if ( ! gensg[i]->hasItems() )
        {
              gensg[i]->reset();
              i++;
            }
            else
              stop = true;
      }
    }
    else
    {
      while ( ! stop && i < n )
      {
            gensf[i]->next();
            if ( ! gensf[i]->hasItems() )
        {
              gensf[i]->reset();
              i++;
            }
            else
              stop = true;
      }
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
