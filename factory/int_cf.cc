// emacs edit mode for this file is -*- C++ -*-
// $Id: int_cf.cc,v 1.1 1997-03-27 10:03:08 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:46  stobbe
Initial revision

*/

#include "assert.h"

#include "cf_defs.h"

#include "int_cf.h"
#include "canonicalform.h"


CanonicalForm
InternalCF::lc()
{
    return CanonicalForm( copyObject() );
}

CanonicalForm
InternalCF::LC()
{
    return CanonicalForm( copyObject() );
}

int
InternalCF::degree()
{
    if ( isZero() )
	return -1;
    else
	return 0;
}

CanonicalForm
InternalCF::tailcoeff()
{
    return CanonicalForm( copyObject() );
}

int
InternalCF::taildegree()
{
    return 0;
}

CanonicalForm
InternalCF::coeff( int i )
{
    if ( i == 0 )
	return CanonicalForm( copyObject() );
    else
	return 0;
}

int
InternalCF::intval() const
{
    ASSERT1( 0, "illegal conversion: not implemented for class %s", this->classname() );
    return 0;
}

int
InternalCF::sign() const
{
    return 1;
}

InternalCF*
InternalCF::invert()
{
    ASSERT1( 0, "internal factory error: not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::num()
{
    return copyObject();
}

InternalCF*
InternalCF::den()
{
    return genOne();
}

int
InternalCF::comparecoeff ( InternalCF* )
{
    ASSERT1( 0, "fatal error: not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::sqrt()
{
    ASSERT1( 0, "fatal error: not implemented for class %s", this->classname() );
    return 0;
}
