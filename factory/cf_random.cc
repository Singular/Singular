/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_random.cc,v 1.4 1997-07-01 12:35:11 schmidt Exp $ */

#include <config.h>

#include <time.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_random.h"
#include "ffops.h"
#include "gfops.h"
#include "imm.h"

class RandomGenerator {
private:
#ifdef macintosh
    // mac does not like const long int in a class
    long int ia, im, iq, ir, deflt;
#else
    const long int
	ia = 16807,
	im = 2147483647,
	iq = 127773,
	ir = 2836,
	deflt = 123459876;
#endif
    
    long s;

    // s must not equal zero!
    void seedInit( long ss ) { s = ((ss == 0) ? deflt : ss); }
public:
#ifdef macintosh
    RandomGenerator();
    RandomGenerator( long ss );
#else
    RandomGenerator() { seedInit( (long)time( 0 ) ); }
    RandomGenerator( long ss ) { seedInit( ss ); }
#endif
    ~RandomGenerator() {}
    long generate();
    void seed( long ss ) { seedInit( ss ); }
};

#ifdef macintosh
RandomGenerator::RandomGenerator()
{
  ia = 16807;
  im = 2147483647;
  iq = 127773;
  ir = 2836;
  deflt = 123459876;
  seedInit( (long)time( 0 ) );
}

RandomGenerator::RandomGenerator( long ss )
{
  ia = 16807;
  im = 2147483647;
  iq = 127773;
  ir = 2836;
  deflt = 123459876;
  seedInit( ss );
}
#endif

long
RandomGenerator::generate()
{
    long k;

    k = s/iq;
    s = ia*(s-k*iq)-ir*k;
    if ( s < 0 ) s += im;

    return s;
}

RandomGenerator ranGen;

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
    return ranGen.generate() % n;
}

void factoryseed ( int s )
{
    ranGen.seed( s );
}
