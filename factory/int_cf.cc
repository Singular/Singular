/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#include "cf_assert.h"

#include "cf_defs.h"
#include "int_cf.h"
#include "canonicalform.h"
#include "cf_factory.h"

//{{{ bool InternalCF::isOne, isZero () const
// docu: see CanonicalForm::isOne(), CanonicalForm::isZero()
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
//}}}

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
    if ( isZero() )
        return -1;
    else
        return 0;
}
//}}}

//{{{ InternalCF * InternalCF::num (), den ()
// docu: see CanonicalForm::num(), den()
InternalCF *
InternalCF::num ()
{
    return copyObject();
}

InternalCF *
InternalCF::den ()
{
    return CFFactory::basic( 1L );
}
//}}}

//{{{ InternalCF * InternalCF::sqrt ()
// docu: see CanonicalForm::sqrt()
InternalCF *
InternalCF::sqrt ()
{
    ASSERT1( 0, "sqrt() not implemented for class %s", this->classname() );
    return 0;
}
//}}}

//{{{ int InternalCF::ilog2 ()
// docu: see CanonicalForm::ilog2()
int
InternalCF::ilog2 ()
{
    ASSERT1( 0, "ilog2() not implemented for class %s", this->classname() );
    return 0;
}
//}}}

//{{{ CanonicalForm InternalCF::coeff ( int i )
// docu: see CanonicalForm::operator []()
CanonicalForm
InternalCF::coeff ( int i )
{
    if ( i == 0 )
        return CanonicalForm( copyObject() );
    else
        return CanonicalForm( 0 );
}
//}}}

//{{{ InternalCF * InternalCF::bgcdsame, bgcdcoeff ( const InternalCF * const )
// docu: see CanonicalForm::bgcd()
InternalCF *
InternalCF::bgcdsame ( const InternalCF * const ) const
{
    ASSERT1( 0, "bgcd() not implemented for class %s", this->classname() );
    return CFFactory::basic( 0L );
}

InternalCF *
InternalCF::bgcdcoeff ( const InternalCF * const )
{
    ASSERT1( 0, "bgcd() not implemented for class %s", this->classname() );
    return CFFactory::basic( 0L );
}
//}}}

//{{{ InternalCF * InternalCF::bextgcdsame ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
// docu: see CanonicalForm::bextgcd()
InternalCF *
InternalCF::bextgcdsame ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
{
    ASSERT1( 0, "bextgcd() not implemented for class %s", this->classname() );
    a = 0; b = 0;
    return CFFactory::basic( 0L );
}

InternalCF *
InternalCF::bextgcdcoeff ( InternalCF *, CanonicalForm & a, CanonicalForm & b )
{
    ASSERT1( 0, "bextgcd() not implemented for class %s", this->classname() );
    a = 0; b = 0;
    return CFFactory::basic( 0L );
}
//}}}

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
