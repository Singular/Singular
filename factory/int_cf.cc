/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "int_cf.h"
#include "canonicalform.h"
#include "cf_factory.h"

/** bool InternalCF::isOne, isZero () const
 * @sa CanonicalForm::isOne(), CanonicalForm::isZero()
**/
bool
InternalCF::isOne () const
{
    return false;
}

bool
InternalCF::isZero () const
{
    return false;
}


/** CanonicalForm InternalCF::lc (), Lc (), LC ()
 *  @sa CanonicalForm::lc(), Lc(), LC()
**/
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

/** int InternalCF::degree ()
 * @sa CanonicalForm::degree()
**/
int
InternalCF::degree ()
{
    if ( isZero() )
        return -1;
    else
        return 0;
}

/** CanonicalForm InternalCF::tailcoeff (), int InternalCF::taildegree ()
 * @sa CanonicalForm::tailcoeff(), taildegree()
**/
CanonicalForm
InternalCF::tailcoeff ()
{
    return CanonicalForm( copyObject() );
}

int
InternalCF::taildegree ()
{
    if ( isZero() )
        return -1;
    else
        return 0;
}

/** InternalCF * InternalCF::num (), den ()
 * @sa CanonicalForm::num(), den()
**/
InternalCF *
InternalCF::num ()
{
    return copyObject();
}

InternalCF *
InternalCF::den ()
{
    return CFFactory::basic( 1 );
}

/** InternalCF * InternalCF::sqrt ()
 * @sa CanonicalForm::sqrt()
**/
InternalCF *
InternalCF::sqrt ()
{
    ASSERT1( 0, "sqrt() not implemented for class %s", this->classname() );
    return 0;
}

/** int InternalCF::ilog2 ()
 * @sa CanonicalForm::ilog2()
**/
int
InternalCF::ilog2 ()
{
    ASSERT1( 0, "ilog2() not implemented for class %s", this->classname() );
    return 0;
}

/** CanonicalForm InternalCF::coeff ( int i )
 * @sa CanonicalForm::operator []()
**/
CanonicalForm
InternalCF::coeff ( int i )
{
    if ( i == 0 )
        return CanonicalForm( copyObject() );
    else
        return CanonicalForm( 0 );
}

/** InternalCF * InternalCF::bgcdsame, bgcdcoeff ( const InternalCF * const )
 * @sa CanonicalForm::bgcd()
**/
InternalCF *
InternalCF::bgcdsame ( const InternalCF * const ) const
{
    ASSERT1( 0, "bgcd() not implemented for class %s", this->classname() );
    return CFFactory::basic( 0 );
}

InternalCF *
InternalCF::bgcdcoeff ( const InternalCF * const )
{
    ASSERT1( 0, "bgcd() not implemented for class %s", this->classname() );
    return CFFactory::basic( 0 );
}

/** InternalCF * InternalCF::bextgcdsame ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
 * @sa CanonicalForm::bextgcd()
**/
InternalCF *
InternalCF::bextgcdsame ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
{
    ASSERT1( 0, "bextgcd() not implemented for class %s", this->classname() );
    a = 0; b = 0;
    return CFFactory::basic( 0 );
}

InternalCF *
InternalCF::bextgcdcoeff ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
{
    ASSERT1( 0, "bextgcd() not implemented for class %s", this->classname() );
    a = 0; b = 0;
    return CFFactory::basic( 0 );
}

long
InternalCF::intval() const
{
    ASSERT1( 0, "intval() not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::invert()
{
    ASSERT1( 0, "invert() not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::tryMulsame( InternalCF*, const CanonicalForm&)
{
    ASSERT1( 0, "tryMulsame() not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::tryInvert ( const CanonicalForm&, bool&)
{
    ASSERT1( 0, "tryInvert() not implemented for class %s", this->classname() );
    return 0;
}

bool
InternalCF::tryDivremsamet ( InternalCF*, InternalCF*&, InternalCF*&, const CanonicalForm&, bool&)
{
    ASSERT1( 0, "tryDivremsamet() not implemented for class %s", this->classname() );
    return 0;
}

bool
InternalCF::tryDivremcoefft ( InternalCF*, InternalCF*&, InternalCF*&, bool, const CanonicalForm&, bool&)
{
    ASSERT1( 0, "tryDivremcoefft() not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::tryDivsame ( InternalCF*, const CanonicalForm&, bool&)
{
    ASSERT1( 0, "tryDivsame() not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::tryDivcoeff ( InternalCF*, bool, const CanonicalForm&, bool&)
{
    ASSERT1( 0, "tryDivcoeff() not implemented for class %s", this->classname() );
    return 0;
}

InternalCF*
InternalCF::tryDividecoeff ( InternalCF*, bool, const CanonicalForm&, bool&)
{
    ASSERT1( 0, "tryDividecoeff() not implemented for class %s", this->classname() );
    return 0;
}
