/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: int_cf.cc,v 1.6 1997-10-10 10:23:17 schmidt Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "int_cf.h"
#include "canonicalform.h"
#include "cf_factory.h"

//{{{ CanonicalForm InternalCF::lc (), Lc (), LC ()
// docu: see CanonicalForm::lc(), Lc(), LC()
CanonicalForm
InternalCF::lc ()
{
    return CanonicalForm( copyObject() );
}

CanonicalForm
InternalCF::Lc ()
{
    return CanonicalForm( copyObject() );
}

CanonicalForm
InternalCF::LC ()
{
    return CanonicalForm( copyObject() );
}
//}}}

//{{{ int InternalCF::degree ()
// docu: see CanonicalForm::degree()
int
InternalCF::degree ()
{
    if ( isZero() )
	return -1;
    else
	return 0;
}
//}}}

//{{{ CanonicalForm InternalCF::tailcoeff (), int InternalCF::taildegree ()
// docu: see CanonicalForm::tailcoeff(), taildegree()
CanonicalForm
InternalCF::tailcoeff ()
{
    return CanonicalForm( copyObject() );
}

int
InternalCF::taildegree ()
{
    return 0;
}
//}}}

//{{{ InternalCF* InternalCF::num (), den ()
// docu: see CanonicalForm::num(), den()
InternalCF*
InternalCF::num ()
{
    return copyObject();
}

InternalCF*
InternalCF::den ()
{
    return CFFactory::basic( 1 );
}
//}}}

//{{{ int InternalCF::sign () const
// docu: see CanonicalForm::sign()
int
InternalCF::sign () const
{
    ASSERT1( 0, "fatal error: not implemented for class %s", this->classname() );
    return 0;
}
//}}}

//{{{ InternalCF* InternalCF::sqrt ()
// docu: see CanonicalForm::sqrt()
InternalCF*
InternalCF::sqrt ()
{
    ASSERT1( 0, "fatal error: not implemented for class %s", this->classname() );
    return 0;
}
//}}}

//{{{ int InternalCF::ilog2 ()
// docu: see CanonicalForm::ilog2()
int
InternalCF::ilog2 ()
{
    ASSERT1( 0, "fatal error: not implemented for class %s", this->classname() );
    return 0;
}
//}}}

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

InternalCF*
InternalCF::invert()
{
    ASSERT1( 0, "internal factory error: not implemented for class %s", this->classname() );
    return 0;
}

int
InternalCF::comparecoeff ( InternalCF* )
{
    ASSERT1( 0, "fatal error: not implemented for class %s", this->classname() );
    return 0;
}
