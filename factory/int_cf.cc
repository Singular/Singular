// emacs edit mode for this file is -*- C++ -*-
// $Id: int_cf.cc,v 1.0 1996-05-17 10:59:46 stobbe Exp $

/*
$Log: not supported by cvs2svn $
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
    cerr << "not implemented for class " << this->classname() << endl;
    ASSERT( 0, "illegal conversion" );
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
    cerr << "not implemented for class " << this->classname() << endl;
    ASSERT( 0, "internal factory error" );
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
    cerr << "not implemented for class " << this->classname() << endl;
    ASSERT( 0, "fatal error" );
    return 0;
}

InternalCF*
InternalCF::sqrt()
{
    cerr << "not implemented for class " << this->classname() << endl;
    ASSERT( 0, "fatal error" );
    return 0;
}
