/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include <time.h>

#include "cf_assert.h"

#include "cf_defs.h"
#include "cf_random.h"
#include "ffops.h"
#include "gfops.h"
#include "imm.h"

#ifdef HAVE_FLINT
extern "C"
{
#ifndef __GMP_BITS_PER_MP_LIMB
#define __GMP_BITS_PER_MP_LIMB GMP_LIMB_BITS
#endif
#include <flint/flint.h>

GLOBAL_VAR flint_rand_t FLINTrandom;
}
#endif

class RandomGenerator {
private:
    const int ia, im, iq, ir, deflt;
    int s;

    // s must not equal zero!
    void seedInit( int ss ) { s = ((ss == 0) ? deflt : ss); }
public:
    RandomGenerator();
    RandomGenerator( int ss );
    ~RandomGenerator() {}
    int generate();
    void seed( int ss ) { seedInit( ss ); }
};

RandomGenerator::RandomGenerator() : ia(16807), im(2147483647), iq(127773), ir(2836), deflt(123459876)
{
  seedInit( (int)time( 0 ) );
}

RandomGenerator::RandomGenerator( int ss ) : ia(16807), im(2147483647), iq(127773), ir(2836), deflt(123459876)
{
  seedInit( ss );
}

int
RandomGenerator::generate()
{
    int k;

    k = s/iq;
    s = ia*(s-k*iq)-ir*k;
    if ( s < 0 ) s += im;

    return s;
}

INST_VAR RandomGenerator ranGen;

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
    int i= factoryrandom( gf_q );
    if ( i == gf_q1 ) i++;
    return CanonicalForm( int2imm_gf( i ) );
}

CFRandom * GFRandom::clone () const
{
    return new GFRandom();
}


IntRandom::IntRandom()
{
    max = 50;
}

IntRandom::IntRandom( int m )
{
    max = m;
}

IntRandom::~IntRandom() {}

CanonicalForm IntRandom::generate() const
{
    return factoryrandom( 2*max )-max;
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
}

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
    if ( n == 0 )
        return (int)ranGen.generate();
    else
        return ranGen.generate() % n;
}


void factoryseed ( int s )
{
    ranGen.seed( s );

#ifdef HAVE_FLINT
    flint_randinit(FLINTrandom);
#endif
}
