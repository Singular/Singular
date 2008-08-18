/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_char.cc,v 1.10 2008-08-18 11:19:33 Singular Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "imm.h"
#include "int_pp.h"
#include "cf_primes.h"
#include "cf_binom.h"
#ifdef SINGULAR
#include "singext.h"
#endif

static int theCharacteristic = 0;
static int theDegree = 1;

int initializeCharacteristic ()
{
    theCharacteristic = 0;
    theDegree = 1;
    return 1;
}

void setCharacteristic( int c )
{
    if ( c == 0 )
    {
        theDegree = 0;
        CFFactory::settype( IntegerDomain );
        theCharacteristic = 0;
    }
    else
    {
        theDegree = 1;
        CFFactory::settype( FiniteFieldDomain );
        theCharacteristic = c;
        ff_big = c > cf_getSmallPrime( cf_getNumSmallPrimes()-1 );
#ifdef SINGULAR
	if (!errorreported && (c > 536870909)) WerrorS("characteristic too large(max is 2^29)");
#endif
        ff_setprime( c );
        resetFPT();
    }
}

void setCharacteristic( int c, int n )
{
    ASSERT( c > 1 && n > 0, "illegal characteristic" );
    setCharacteristic( c );
    InternalPrimePower::setPrimePower( c, n );
    CFFactory::settype( PrimePowerDomain );
}

void setCharacteristic( int c, int n, char name )
{
    ASSERT( c != 0 && n > 1, "illegal GF(q)" );
    setCharacteristic( c );
    gf_setcharacteristic( c, n, name );
    theDegree = n;
    CFFactory::settype( GaloisFieldDomain );
}

int getCharacteristic()
{
    return theCharacteristic;
}

int getExp()
{
  return InternalPrimePower::getk();
}


int getGFDegree()
{
    ASSERT( theDegree > 0, "not in GF(q)" );
    return theDegree;
}

CanonicalForm getGFGenerator()
{
    ASSERT( theDegree > 1, "not in GF(q)" );
    return int2imm_gf( 1 );
}
