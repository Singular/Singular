/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_char.cc
 *
 * getting and setting the characteristic of a finite field
**/

#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "canonicalform.h"
#include "imm.h"
#include "cf_primes.h"
#include "cf_util.h"

STATIC_VAR int theCharacteristic = 0;
STATIC_VAR int theDegree = 1;

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
        if (c > 536870909) factoryError("characteristic is too large(max is 2^29)");
        ff_setprime( c );
    }
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

int getGFDegree()
{
    //ASSERT( theDegree > 0, "not in GF(q)" );
    return theDegree;
}

CanonicalForm getGFGenerator()
{
    ASSERT( theDegree > 1, "not in GF(q)" );
    return int2imm_gf( 1 );
}
