// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_char.cc,v 1.3 1996-06-24 11:25:45 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.2  1996/06/18 12:09:43  stobbe
"setCharacteristic: changed the setting of ff_big.
"

Revision 1.1  1996/06/18 06:52:45  stobbe
"setCharacteristic: now sets the flag ff_big
"

Revision 1.0  1996/05/17 10:59:43  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "canonicalform.h"
#include "imm.h"
#include "int_pp.h"
#include "cf_primes.h"

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
    if ( c == 0 ) {
	theDegree = 0;
	CFFactory::settype( IntegerDomain );
	theCharacteristic = 0;
    }
    else {
	theDegree = 1;
	CFFactory::settype( FiniteFieldDomain );
	theCharacteristic = c;
	ff_big = c > cf_getSmallPrime( cf_getNumSmallPrimes()-1 );
	ff_setprime( c );
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
