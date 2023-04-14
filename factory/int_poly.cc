/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#ifndef NOSTREAMIO
#include <string.h>
#if defined(WINNT) && ! defined(__GNUC__)
#include <strstrea.h>
#else
#if __GNUC__ < 3
#include <strstream.h>
#else
#include <strstream>
using namespace std;
#endif
#endif
#endif /* NOSTREAMIO */

#include "cf_assert.h"

#include "cf_defs.h"
#include "cf_factory.h"
#include "cfUnivarGcd.h"
#include "int_cf.h"
#include "int_int.h"
#include "int_poly.h"
#include "canonicalform.h"
#include "variable.h"
#include "imm.h"

#ifdef __GNUC__
#define LIKELY(X)   (__builtin_expect(!!(X), 1))
#define UNLIKELY(X) (__builtin_expect(!!(X), 0))
#else
#define LIKELY(X)   (X)
#define UNLIKELY(X) (X)
#endif

#ifdef HAVE_OMALLOC
const omBin term::term_bin = omGetSpecBin(sizeof(term));
const omBin InternalPoly::InternalPoly_bin = omGetSpecBin(sizeof(InternalPoly));
#endif

InternalPoly::InternalPoly( termList first, termList last, const Variable & v )
{
    firstTerm = first;
    lastTerm = last;
    var = v;
}

InternalPoly::InternalPoly()
{
    ASSERT( 0, "ups, why do you initialize an empty poly" );
}

InternalPoly::InternalPoly( const Variable & v, const int e, const CanonicalForm& c )
{
    var = v;
    firstTerm = new term( 0, c, e );
    lastTerm = firstTerm;
}

InternalPoly::InternalPoly( const InternalPoly& ):InternalCF()
{
    ASSERT( 0, "ups there is something wrong in your code" );
}

InternalPoly::~InternalPoly()
{
    freeTermList( firstTerm );
}

InternalCF*
InternalPoly::deepCopyObject() const
{
    termList first, last;
    first = deepCopyTermList( firstTerm, last );
    return new InternalPoly( first, last, var );
}

bool
InternalPoly::isUnivariate() const
{
    termList cursor = firstTerm;
    while ( cursor )
    {
        if ( ! cursor->coeff.inCoeffDomain() )
            return false;
        cursor = cursor->next;
    }
    return true;
}

/** int InternalPoly::degree ()
 * @sa CanonicalForm::sign ()
**/
int
InternalPoly::degree ()
{
    return firstTerm->exp;
}


/** int InternalPoly::sign () const
 * @sa CanonicalForm::sign()
**/
int
InternalPoly::sign () const
{
    return firstTerm->coeff.sign();
}


/**
  * @sa CanonicalForm::lc(), CanonicalForm::Lc(), CanonicalForm::LC(), InternalPoly::Lc (), InternalPoly::LC ()
**/
CanonicalForm
InternalPoly::lc ()
{
    return firstTerm->coeff.lc();
}

/**
  * @sa CanonicalForm::lc(), CanonicalForm::Lc(), CanonicalForm::LC(), InternalPoly::lc (), InternalPoly::LC ()
**/
CanonicalForm
InternalPoly::Lc ()
{
    return firstTerm->coeff.Lc();
}

/**
  * @sa CanonicalForm::lc(), CanonicalForm::Lc(), CanonicalForm::LC(), InternalPoly::lc (), InternalPoly::Lc ()
**/
CanonicalForm
InternalPoly::LC ()
{
    return firstTerm->coeff;
}

/** CanonicalForm InternalPoly::tailcoeff (), int InternalPoly::taildegree ()
 * @sa CanonicalForm::tailcoeff(), taildegree()
**/
CanonicalForm
InternalPoly::tailcoeff ()
{
    return lastTerm->coeff;
}

int
InternalPoly::taildegree ()
{
    return lastTerm->exp;
}

/** CanonicalForm InternalPoly::coeff ( int i )
 * @sa CanonicalForm::operator []()
**/
CanonicalForm
InternalPoly::coeff ( int i )
{
    termList theCursor = firstTerm;
    while ( theCursor )
    {
        if ( theCursor->exp == i )
            return theCursor->coeff;
        else if ( theCursor->exp < i )
            return CanonicalForm( 0 );
        else
            theCursor = theCursor->next;
    }
    return CanonicalForm( 0 );
}

#ifndef NOSTREAMIO
void
InternalPoly::print(OSTREAM &aStream, char * aString )
{
    if ( ! firstTerm )
        aStream << 0 << aString;
    else
    {
        char * theString;
        termList theCursor = firstTerm;
        while ( theCursor )
        {
            ostrstream theStream;
            if ( theCursor->exp == 0 )
                theCursor->coeff.print( aStream, aString );
            else  if ( theCursor->coeff.isOne() )
            {
                aStream << var;
                if ( theCursor->exp != 1 )
                    aStream << '^' << theCursor->exp << aString;
                else
                    aStream << aString;
            }
            else  if ( theCursor->coeff.sign() < 0 && (-theCursor->coeff).isOne() )
            {
                aStream << '-' << var;
                if ( theCursor->exp != 1 )
                    aStream << '^' << theCursor->exp << aString;
                else
                    aStream << aString;
            }
            else
            {
                theStream << '*' << var;
                if ( theCursor->exp != 1 )
                    theStream << '^' << theCursor->exp << aString << ends;
                else
                    theStream << aString << ends; // results from error in GNU strstream
                theString = theStream.str();
                theCursor->coeff.print( aStream, theString );
                theStream.freeze(0);//delete [] theString;
            }
            theCursor = theCursor->next;
            if ( theCursor && ( theCursor->coeff.sign() >= 0 ) )
                aStream << '+';
        }
    }
}
#endif /* NOSTREAMIO */

/** InternalCF * InternalPoly::neg ()
 * @sa CanonicalForm::operator -()
**/
InternalCF *
InternalPoly::neg ()
{
    if ( getRefCount() <= 1 )
    {
        negateTermList( firstTerm );
        return this;
    }
    else
    {
        decRefCount();
        termList last, first = copyTermList( firstTerm, last, true );
        return new InternalPoly( first, last, var );
    }
}

InternalCF*
InternalPoly::invert()
{
    if ( inExtension() && getReduce( var ) )
    {
        setReduce( var, false );
        CanonicalForm a( this->copyObject() );
        CanonicalForm b = getMipo( var );
        CanonicalForm u, v;
        CanonicalForm g = extgcd( a, b, u, v );
        setReduce( var, true );
        return u.getval();
    }
    else
        return CFFactory::basic( 0 );
}

InternalCF*
InternalPoly::tryInvert ( const CanonicalForm& M, bool& fail)
{
  if ( inExtension() && !getReduce ( var ) )
  {
    CanonicalForm b, inverse;
    CanonicalForm F ( this ->copyObject() );
    Variable a = M.mvar();
    Variable x = Variable(1);
    F= mod (F, M); //reduce mod M
    CanonicalForm g= extgcd (replacevar( F, a, x ), replacevar( M, a, x ), inverse, b );
    if(!g.isOne())
      fail = true;
    else
      inverse = replacevar( inverse, x, a ); // change back to alg var
    CanonicalForm test= mod (inverse*F, M);
    return inverse.getval();
  }
  else
    return CFFactory::basic( 0 );
}

InternalCF*
InternalPoly::addsame( InternalCF* aCoeff )
{
    InternalPoly * aPoly = (InternalPoly*)aCoeff;
    if ( getRefCount() <= 1 )
    {
        firstTerm = addTermList( firstTerm, aPoly->firstTerm, lastTerm, false );
        if ( firstTerm && firstTerm->exp != 0 )
            return this;
        else  if ( firstTerm )
        {
            InternalCF * res = firstTerm->coeff.getval();
            delete this;
            return res;
        }
        else
        {
            delete this;
            return CFFactory::basic( 0 );
        }
    }
    else
    {
        decRefCount();
        termList last, first = copyTermList( firstTerm, last );
        first = addTermList( first, aPoly->firstTerm, last, false );
        if ( first && first->exp != 0 )
            return new InternalPoly( first, last, var );
        else  if ( first )
        {
            InternalCF * res = first->coeff.getval();
            delete first;
            return res;
        }
        else
            return CFFactory::basic( 0 );

    }
}

InternalCF*
InternalPoly::subsame( InternalCF* aCoeff )
{
    InternalPoly * aPoly = (InternalPoly*)aCoeff;
    if ( getRefCount() <= 1 )
    {
        firstTerm = addTermList( firstTerm, aPoly->firstTerm, lastTerm, true );
        if ( firstTerm && firstTerm->exp != 0 )
            return this;
        else  if ( firstTerm )
        {
            InternalCF * res = firstTerm->coeff.getval();
            delete this;
            return res;
        }
        else
        {
            delete this;
            return CFFactory::basic( 0 );
        }
    }
    else
    {
        decRefCount();
        termList last, first = copyTermList( firstTerm, last );
        first = addTermList( first, aPoly->firstTerm, last, true );
        if ( first && first->exp != 0 )
            return new InternalPoly( first, last, var );
        else  if ( first )
        {
            InternalCF * res = first->coeff.getval();
            delete first;
            return res;
        }
        else
            return CFFactory::basic( 0 );

    }
}

InternalCF*
InternalPoly::mulsame( InternalCF* aCoeff )
{
    if (is_imm(aCoeff)) return mulcoeff(aCoeff);
    InternalPoly *aPoly = (InternalPoly*)aCoeff;
    termList resultFirst = 0, resultLast = 0;
    termList theCursor = firstTerm;

    while ( theCursor )
    {
        resultFirst = mulAddTermList( resultFirst, aPoly->firstTerm,
                          theCursor->coeff, theCursor->exp, resultLast, false );
        theCursor = theCursor->next;
    }
    if ( inExtension() && getReduce( var ) )
    {
        resultFirst = reduceTermList( resultFirst, (getInternalMipo( var ))->firstTerm, resultLast );
        if ( resultFirst == 0 )
        {
            if ( getRefCount() <= 1 )
            {
                delete this;
                return CFFactory::basic(0);
            }
            else
            {
                decRefCount();
                return CFFactory::basic(0);
            }
        }
        else  if ( resultFirst->exp == 0 )
        {
            if ( getRefCount() <= 1 )
            {
                InternalCF * res = resultFirst->coeff.getval();
                delete resultFirst;
                delete this;
                return res;
            }
            else
            {
                decRefCount();
                InternalCF * res = resultFirst->coeff.getval();
                delete resultFirst;
                return res;
            }
        }
    }
    if ( getRefCount() <= 1 )
    {
        freeTermList( firstTerm );
        firstTerm = resultFirst;
        lastTerm = resultLast;
        return this;
    }
    else
    {
        decRefCount();
        return new InternalPoly( resultFirst, resultLast, var );
    }
}

InternalCF*
InternalPoly::tryMulsame( InternalCF* aCoeff, const CanonicalForm& M)
{
    if (is_imm(aCoeff))
       return mulcoeff(aCoeff);
    InternalPoly *aPoly = (InternalPoly*)aCoeff;
    termList resultFirst = 0, resultLast = 0;
    termList theCursor = firstTerm;

    while ( theCursor )
    {
        resultFirst = mulAddTermList( resultFirst, aPoly->firstTerm,
                          theCursor->coeff, theCursor->exp, resultLast, false );
        theCursor = theCursor->next;
    }
    if ( inExtension() && !getReduce( var ) )
    {
        resultFirst= reduceTermList (resultFirst, ((InternalPoly*) M.getval())->firstTerm, resultLast);
        if ( resultFirst == 0 )
        {
            if ( getRefCount() <= 1 )
            {
                delete this;
                return CFFactory::basic(0);
            }
            else
            {
                decRefCount();
                return CFFactory::basic(0);
            }
        }
        else  if ( resultFirst->exp == 0 )
        {
            if ( getRefCount() <= 1 )
            {
                InternalCF * res = resultFirst->coeff.getval();
                delete resultFirst;
                delete this;
                return res;
            }
            else
            {
                decRefCount();
                InternalCF * res = resultFirst->coeff.getval();
                delete resultFirst;
                return res;
            }
        }
    }
    if ( getRefCount() <= 1 )
    {
        freeTermList( firstTerm );
        firstTerm = resultFirst;
        lastTerm = resultLast;
        return this;
    }
    else
    {
        decRefCount();
        return new InternalPoly( resultFirst, resultLast, var );
    }
}

InternalCF*
InternalPoly::dividesame( InternalCF* aCoeff )
{
    return divsame( aCoeff );
}


InternalCF*
InternalPoly::divsame( InternalCF* aCoeff )
{
    if ( inExtension() && getReduce( var ) )
    {
        InternalCF * dummy = aCoeff->invert();
        if (is_imm(dummy)) dummy=this->mulsame(dummy);
        else dummy = dummy->mulsame( this );
        if ( getRefCount() <= 1 )
        {
             delete this;
             return dummy;
        }
        else
        {
            decRefCount();
            return dummy;
        }
    }
    InternalPoly *aPoly = (InternalPoly*)aCoeff;
    termList dummy, first, last, resultfirst = 0, resultlast = 0;
    CanonicalForm coeff, newcoeff;
    int exp, newexp;
    bool singleObject;

    if ( getRefCount() <= 1 )
    {
        first = firstTerm; last = lastTerm; singleObject = true;
    }
    else
    {
        first = copyTermList( firstTerm, last ); singleObject = false;
        decRefCount();
    }
    coeff = aPoly->firstTerm->coeff;
    exp = aPoly->firstTerm->exp;
    while (first && ( first->exp >= exp ) )
    {
        newcoeff = first->coeff / coeff;
        newexp = first->exp - exp;
        dummy = first;
        first = mulAddTermList( first->next, aPoly->firstTerm->next, newcoeff, newexp, last, true );
        delete dummy;
        appendTermList( resultfirst, resultlast, newcoeff, newexp );
    }
    freeTermList( first );
    if ( singleObject )
    {
        if ( resultfirst && resultfirst->exp != 0 )
        {
            firstTerm = resultfirst;
            lastTerm = resultlast;
            return this;
        }
        else  if ( resultfirst )
        {
            InternalCF * res = resultfirst->coeff.getval();
            delete resultfirst;
            firstTerm = 0;
            delete this;
            return res;
        }
        else
        {
            // this should not happen (evtl use assertion)
            ASSERT( 0, "FATAL ERROR, PLEASE INFORM THE AUTHOR" );
            firstTerm = 0;
            delete this;
            return CFFactory::basic( 0 );
        }
    }
    else
    {
        if ( resultfirst && resultfirst->exp != 0 )
            return new InternalPoly( resultfirst, resultlast, var );
        else  if ( resultfirst )
        {
            InternalCF * res = resultfirst->coeff.getval();
            delete resultfirst;
            return res;
        }
        else
            return CFFactory::basic( 0 );
    }
}

InternalCF*
InternalPoly::tryDivsame( InternalCF* aCoeff, const CanonicalForm& M, bool& fail )
{
    if ( inExtension() && !getReduce( var ) )
    {
        InternalCF * dummy = aCoeff->tryInvert(M, fail);
        if (fail)
          return CFFactory::basic( 0 );
        if (is_imm(dummy)) dummy=this->tryMulsame(dummy, M);
        else dummy = dummy->tryMulsame( this, M);
        if (fail)
        {
          if (getRefCount() <= 1)
            delete this;
          else
            decRefCount();
          return dummy;
        }
        if ( getRefCount() <= 1 )
        {
             delete this;
             return dummy;
        }
        else
        {
            decRefCount();
            return dummy;
        }
    }
    InternalPoly *aPoly = (InternalPoly*)aCoeff;
    termList dummy, first, last, resultfirst = 0, resultlast = 0;
    CanonicalForm coeff, newcoeff;
    int exp, newexp;
    bool singleObject;

    if ( getRefCount() <= 1 )
    {
        first = firstTerm; last = lastTerm; singleObject = true;
    }
    else
    {
        first = copyTermList( firstTerm, last ); singleObject = false;
        decRefCount();
    }
    coeff = aPoly->firstTerm->coeff;
    exp = aPoly->firstTerm->exp;
    while (first && ( first->exp >= exp ) )
    {
        newcoeff= first->coeff.tryDiv (coeff, M, fail);
        if (fail)
        {
          freeTermList (first);
          return CFFactory::basic (0);
        }
        newcoeff= reduce (newcoeff, M);
        newexp = first->exp - exp;
        dummy = first;
        first = mulAddTermList( first->next, aPoly->firstTerm->next, newcoeff, newexp, last, true );
        delete dummy;
        if (!newcoeff.isZero())
          appendTermList( resultfirst, resultlast, newcoeff, newexp );
    }
    freeTermList( first );
    if ( singleObject )
    {
        if ( resultfirst && resultfirst->exp != 0 )
        {
            firstTerm = resultfirst;
            lastTerm = resultlast;
            return this;
        }
        else  if ( resultfirst )
        {
            InternalCF * res = resultfirst->coeff.getval();
            delete resultfirst;
            firstTerm = 0;
            delete this;
            return res;
        }
        else
        {
            // this should not happen (evtl use assertion)
            ASSERT( 0, "FATAL ERROR, PLEASE INFORM THE AUTHOR" );
            firstTerm = 0;
            delete this;
            return CFFactory::basic( 0 );
        }
    }
    else
    {
        if ( resultfirst && resultfirst->exp != 0 )
            return new InternalPoly( resultfirst, resultlast, var );
        else  if ( resultfirst )
        {
            InternalCF * res = resultfirst->coeff.getval();
            delete resultfirst;
            return res;
        }
        else
            return CFFactory::basic( 0 );
    }
}

InternalCF*
InternalPoly::modulosame( InternalCF* aCoeff )
{
    return modsame( aCoeff );
}

InternalCF*
InternalPoly::modsame( InternalCF* aCoeff )
{
    if ( inExtension() && getReduce( var ) )
    {
        if ( deleteObject() ) delete this;
        return CFFactory::basic( 0 );
    }
    InternalPoly *aPoly = (InternalPoly*)aCoeff;
    termList dummy, first, last;
    CanonicalForm coeff, newcoeff;
    int exp, newexp;
    bool singleObject;

    if ( getRefCount() <= 1 )
    {
        first = firstTerm; last = lastTerm; singleObject = true;
    }
    else
    {
        first = copyTermList( firstTerm, last ); singleObject = false;
        decRefCount();
    }
    coeff = aPoly->firstTerm->coeff;
    exp = aPoly->firstTerm->exp;
    while (first && ( first->exp >= exp ) )
    {
        newcoeff = first->coeff / coeff;
        newexp = first->exp - exp;
        dummy = first;
        first = mulAddTermList( first->next, aPoly->firstTerm->next, newcoeff, newexp, last, true );
        delete dummy;
    }
    if ( singleObject )
    {
        if ( first && first->exp != 0 )
        {
            firstTerm = first;
            lastTerm = last;
            return this;
        }
        else  if ( first )
        {
            InternalCF * res = first->coeff.getval();
            delete first;
            firstTerm = 0;
            delete this;
            return res;
        }
        else
        {
            firstTerm = 0;
            delete this;
            return CFFactory::basic( 0 );
        }
    }
    else
    {
        if ( first && first->exp != 0 )
            return new InternalPoly( first, last, var );
        else  if ( first )
        {
            InternalCF * res = first->coeff.getval();
            delete first;
            return res;
        }
        else
            return CFFactory::basic( 0 );
    }
}


void
InternalPoly::divremsame( InternalCF* acoeff, InternalCF*& quot, InternalCF*& rem )
{
    if ( inExtension() && getReduce( var ) )
    {
        InternalCF * dummy = acoeff->invert();
        quot = dummy->mulsame( this );
        rem = CFFactory::basic( 0 );
    }
    else
    {
        InternalPoly *aPoly = (InternalPoly*)acoeff;
        termList dummy, first, last, resultfirst = 0, resultlast = 0;
        CanonicalForm coeff, newcoeff;
        int exp, newexp;

        first = copyTermList( firstTerm, last );

        coeff = aPoly->firstTerm->coeff;
        exp = aPoly->firstTerm->exp;
        while (first && ( first->exp >= exp ) )
        {
            newcoeff = first->coeff / coeff;
            newexp = first->exp - exp;
            dummy = first;
            first = mulAddTermList( first->next, aPoly->firstTerm->next, newcoeff, newexp, last, true );
            delete dummy;
            appendTermList( resultfirst, resultlast, newcoeff, newexp );
        }
        if ( resultfirst )
            if ( resultfirst->exp == 0 )
            {
                quot = resultfirst->coeff.getval();
                delete resultfirst;
            }
            else
                quot = new InternalPoly( resultfirst, resultlast, var );
        else
            quot = CFFactory::basic( 0 );
        if ( first )
            if ( first->exp == 0 )
            {
                rem = first->coeff.getval();
                delete first;
            }
            else
                rem = new InternalPoly( first, last, var );
        else
            rem = CFFactory::basic( 0 );
    }
}

bool
InternalPoly::divremsamet( InternalCF* acoeff, InternalCF*& quot, InternalCF*& rem )
{
    if ( inExtension() && getReduce( var ) )
    {
        divremsame( acoeff, quot, rem );
        return true;
    }
    InternalPoly *aPoly = (InternalPoly*)acoeff;
    termList dummy, first, last, resultfirst = 0, resultlast = 0;
    CanonicalForm coeff, newcoeff, dummycoeff;
    int exp, newexp;
    bool divideok = true;

//    if ( ! ::divremt( lastTerm->coeff, aPoly->lastTerm->coeff, newcoeff, dummycoeff ) )
//        return false;

    first = copyTermList( firstTerm, last );

    coeff = aPoly->firstTerm->coeff;
    exp = aPoly->firstTerm->exp;
    while (first && ( first->exp >= exp ) && divideok )
    {
        divideok = divremt( first->coeff, coeff, newcoeff, dummycoeff );
        if ( divideok && dummycoeff.isZero() )
        {
            newexp = first->exp - exp;
            dummy = first;
            first = mulAddTermList( first->next, aPoly->firstTerm->next, newcoeff, newexp, last, true );
            delete dummy;
            appendTermList( resultfirst, resultlast, newcoeff, newexp );
        }
        else
            divideok = false;
    }
    if ( divideok )
    {
        if ( resultfirst )
            if ( resultfirst->exp == 0 )
            {
                quot = resultfirst->coeff.getval();
                delete resultfirst;
            }
            else
                quot = new InternalPoly( resultfirst, resultlast, var );
        else
            quot = CFFactory::basic( 0 );
        if ( first )
            if ( first->exp == 0 )
            {
                rem = first->coeff.getval();
                delete first;
            }
            else
                rem = new InternalPoly( first, last, var );
        else
            rem = CFFactory::basic( 0 );
    }
    else
    {
        freeTermList( resultfirst );
        freeTermList( first );
    }
    return divideok;
}

bool
InternalPoly::tryDivremsamet( InternalCF* acoeff, InternalCF*& quot, InternalCF*& rem, const CanonicalForm& M, bool& fail)
{
    if (inExtension() && !getReduce (var))
    {
       InternalCF * dummy = acoeff->tryInvert(M, fail);
       if (fail)
         return false;
       quot = dummy->tryMulsame( this, M);
       rem = CFFactory::basic( 0 );
       if (fail)
         return false;
       return true;
    }
    InternalPoly *aPoly = (InternalPoly*)acoeff;
    termList dummy, first, last, resultfirst = 0, resultlast = 0;
    CanonicalForm coeff, newcoeff, dummycoeff;
    int exp, newexp;
    bool divideok = true;

    first = copyTermList( firstTerm, last );

    coeff = aPoly->firstTerm->coeff;
    exp = aPoly->firstTerm->exp;
    while (first && ( first->exp >= exp ) && divideok )
    {
        divideok = tryDivremt( first->coeff, coeff, newcoeff, dummycoeff, M, fail );
        if (fail)
        {
          freeTermList (first);
          return false;
        }
        if ( divideok && dummycoeff.isZero() )
        {
            newexp = first->exp - exp;
            dummy = first;
            first = mulAddTermList( first->next, aPoly->firstTerm->next, newcoeff, newexp, last, true );
            delete dummy;
            if (!newcoeff.isZero())
              appendTermList( resultfirst, resultlast, newcoeff, newexp );
        }
        else
            divideok = false;
    }
    if ( divideok )
    {
        if ( resultfirst )
            if ( resultfirst->exp == 0 )
            {
                quot = resultfirst->coeff.getval();
                delete resultfirst;
            }
            else
                quot = new InternalPoly( resultfirst, resultlast, var );
        else
            quot = CFFactory::basic( 0 );
        if ( first )
            if ( first->exp == 0 )
            {
                rem = first->coeff.getval();
                delete first;
            }
            else
            {
                if (first->coeff.isZero())
                {
                  rem= CFFactory::basic (0);
                  delete first;
                }
                else
                  rem = new InternalPoly( first, last, var );
            }
        else
            rem = CFFactory::basic( 0 );
    }
    else
    {
        freeTermList( resultfirst );
        freeTermList( first );
    }
    return divideok;
}

/**
 * comparesame(), comparecoeff() - compare with an
 *   InternalPoly.
 *
 * comparesame() compares the coefficient vectors of f=CO and
 * g=acoeff w.r.t to a lexicographic order in the following way:
 * f < g iff there exists an 0 <= i <= max(deg(f),deg(g)) s.t.
 * i) f[j] = g[j] for all i < j <= max(deg(f),deg(g)) and
 * ii) g[i] occurs in g (i.e. is not equal to zero) and
 *     f[i] does not occur in f or f[i] < g[i] if f[i] occurs
 * where f[i] denotes the coefficient to the power x^i of f.
 *
 * As usual, comparesame() returns 1 if CO is larger than c, 0 if
 * CO equals c, and -1 if CO is less than c.  However, this
 * function is optimized to test on equality since this is its
 * most important and frequent usage.
 *
 * See the respective `CanonicalForm'-methods for an explanation
 * why we define such a strange (but total) ordering on
 * polynomials.
 *
 * @sa CanonicalForm::operator <(), CanonicalForm::operator ==()
 *
**/
int
InternalPoly::comparesame ( InternalCF * acoeff )
{
    ASSERT( ! ::is_imm( acoeff ) && acoeff->level() > LEVELBASE, "incompatible base coefficients" );
    InternalPoly* apoly = (InternalPoly*)acoeff;
    // check on triviality
    if ( this == apoly )
        return 0;
    else
    {
        termList cursor1 = firstTerm;
        termList cursor2 = apoly->firstTerm;
        for ( ; cursor1 && cursor2; cursor1 = cursor1->next, cursor2 = cursor2->next )
        {
            // we test on inequality of coefficients at this
            // point instead of testing on "less than" at the
            // last `else' in the enclosed `if' statement since a
            // test on inequality in general is cheaper
            if ( cursor1->exp > cursor2->exp )
                return 1;
            else  if ( cursor1->exp < cursor2->exp )
                return -1;
            if ( (cursor1->coeff != cursor2->coeff) )
            {
                if ( cursor1->coeff > cursor2->coeff )
                    return 1;
                else
                    return -1;
            }
        }
        // check trailing terms
        if ( cursor1 == cursor2 )
            return 0;
        else if ( cursor1 != 0 )
            return 1;
        else
            return -1;
    }
}

/**
 * comparecoeff() always returns 1 since CO is defined to be
 * larger than anything which is a coefficient w.r.t. CO.
**/
int
InternalPoly::comparecoeff ( InternalCF * )
{
    return 1;
}

InternalCF*
InternalPoly::addcoeff( InternalCF* cc )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( c.isZero() )
        return this;
    else
    {
        if ( getRefCount() <= 1 )
        {
            if ( lastTerm->exp == 0 )
            {
                lastTerm->coeff += c;
                if ( lastTerm->coeff.isZero() )
                {
                    termList cursor = firstTerm;
                    while ( cursor->next != lastTerm )
                        cursor = cursor->next;
                    delete lastTerm;
                    cursor->next = 0;
                    lastTerm = cursor;
                }
            }
            else
            {
                lastTerm->next = new term( 0, c, 0 );
                lastTerm = lastTerm->next;
            }
            return this;
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last, false );
            if ( last->exp == 0 )
            {
                last->coeff += c;
                if ( last->coeff.isZero() )
                {
                    termList cursor = first;
                    while ( cursor->next != last )
                        cursor = cursor->next;
                    delete last;
                    cursor->next = 0;
                    last = cursor;
                }
            }
            else
            {
                last->next = new term( 0, c, 0L );
                last = last->next;
            }
            return new InternalPoly( first, last, var );
        }
    }
}

InternalCF*
InternalPoly::subcoeff( InternalCF* cc, bool negate )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( c.isZero() )
        if ( getRefCount() > 1 )
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last, negate );
            return new InternalPoly( first, last, var );
        }
        else
        {
            if ( negate )
                negateTermList( firstTerm );
            return this;
        }
    else
    {
        if ( getRefCount() <= 1 )
        {
            if ( lastTerm->exp == 0 )
            {
                if ( negate )
                {
                    negateTermList( firstTerm );
                    lastTerm->coeff += c;
                }
                else
                    lastTerm->coeff -= c;
                if ( lastTerm->coeff.isZero() )
                {
                    termList cursor = firstTerm;
                    while ( cursor->next != lastTerm )
                        cursor = cursor->next;
                    delete lastTerm;
                    cursor->next = 0;
                    lastTerm = cursor;
                }
            }
            else
            {
                if ( negate )
                {
                    negateTermList( firstTerm );
                    lastTerm->next = new term( 0, c, 0 );
                }
                else
                    lastTerm->next = new term( 0, -c, 0 );
                lastTerm = lastTerm->next;
            }
            return this;
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last, negate );
            if ( last->exp == 0 )
            {
                if ( negate )
                    last->coeff += c;
                else
                    last->coeff -= c;
                if ( last->coeff.isZero() )
                {
                    termList cursor = first;
                    while ( cursor->next != last )
                        cursor = cursor->next;
                    delete last;
                    cursor->next = 0;
                    last = cursor;
                }
            }
            else
            {
                if ( negate )
                    last->next = new term( 0, c, 0 );
                else
                    last->next = new term( 0, -c, 0 );
                last = last->next;
            }
            return new InternalPoly( first, last, var );
        }
    }
}

InternalCF*
InternalPoly::mulcoeff( InternalCF* cc )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( c.isZero() )
    {
        if ( getRefCount() <= 1 )
        {
            delete this;
            return CFFactory::basic( 0 );
        }
        else
        {
            decRefCount();
            return CFFactory::basic( 0 );
        }
    }
    else  if ( c.isOne() )
        return this;
    else
    {
        if ( getRefCount() <= 1 )
        {
            mulTermList( firstTerm, c, 0 );
            return this;
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last );
            mulTermList( first, c, 0 );
            return new InternalPoly( first, last, var );
        }
    }
}

InternalCF*
InternalPoly::dividecoeff( InternalCF* cc, bool invert )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( inExtension() && getReduce( var ) && invert )
    {
        InternalCF * dummy;
        dummy = this->invert();
        if (is_imm(dummy))
        {
          if (is_imm(cc))
          {
            InternalInteger *d=new InternalInteger(imm2int(dummy)*imm2int(cc));
            dummy=d;
          }
          else
            dummy=cc->mulcoeff(dummy);
        }
        else dummy = dummy->mulcoeff( cc );
        if ( getRefCount() <= 1 )
        {
            delete this;
            return dummy;
        }
        else
        {
            decRefCount();
            return dummy;
        }
    }
    if ( invert )
    {
        if ( getRefCount() <= 1 )
        {
            delete this;
            return CFFactory::basic( 0 );
        }
        else
        {
            decRefCount();
            return CFFactory::basic( 0 );
        }
    }
    if ( c.isOne() )
        return this;
    else
    {
        if ( getRefCount() <= 1 )
        {
            firstTerm = divideTermList( firstTerm, c, lastTerm );
            if ( firstTerm && firstTerm->exp != 0 )
                return this;
            else  if ( firstTerm )
            {
                InternalCF * res = firstTerm->coeff.getval();
                delete this;
                return res;
            }
            else
            {
                delete this;
                return CFFactory::basic( 0 );
            }
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last );
            first = divideTermList( first, c, last );
            if ( first && first->exp != 0 )
                return new InternalPoly( first, last, var );
            else  if ( first )
            {
                InternalCF * res = first->coeff.getval();
                delete first;
                return res;
            }
            else
            {
                delete first;
                return CFFactory::basic( 0 );
            }
        }
    }
}

InternalCF*
InternalPoly::tryDividecoeff( InternalCF* cc, bool invert, const CanonicalForm& M, bool& fail )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( inExtension() && !getReduce( var ) && invert )
    {
        InternalCF * dummy;
        dummy = this->tryInvert(M, fail);
        if (fail)
        {
          if (getRefCount() <= 1)
            delete this;
          else
            decRefCount();
          return dummy; //is equal to CFFactory::basic ( 0 ) in this case
        }
        if (is_imm(dummy))
        {
          if (is_imm(cc))
          {
            InternalInteger *d=new InternalInteger(imm2int(dummy)*imm2int(cc));
            dummy=d;
          }
          else
            dummy=cc->mulcoeff(dummy);
        }
        else dummy = dummy->mulcoeff( cc );
        if ( getRefCount() <= 1 )
        {
            delete this;
            return dummy;
        }
        else
        {
            decRefCount();
            return dummy;
        }
    }
    if ( invert )
    {
        if ( getRefCount() <= 1 )
        {
            delete this;
            return CFFactory::basic( 0 );
        }
        else
        {
            decRefCount();
            return CFFactory::basic( 0 );
        }
    }
    if ( c.isOne() )
        return this;
    //one should never get here
    else
    {
        if ( getRefCount() <= 1 )
        {
            firstTerm = divideTermList( firstTerm, c, lastTerm );
            if ( firstTerm && firstTerm->exp != 0 )
                return this;
            else  if ( firstTerm )
            {
                InternalCF * res = firstTerm->coeff.getval();
                delete this;
                return res;
            }
            else
            {
                delete this;
                return CFFactory::basic( 0 );
            }
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last );
            first = divideTermList( first, c, last );
            if ( first && first->exp != 0 )
                return new InternalPoly( first, last, var );
            else  if ( first )
            {
                InternalCF * res = first->coeff.getval();
                delete first;
                return res;
            }
            else
            {
                delete first;
                return CFFactory::basic( 0 );
            }
        }
    }
}


InternalCF*
InternalPoly::divcoeff( InternalCF* cc, bool invert )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( inExtension() && getReduce( var ) && invert )
    {
        InternalCF * dummy;
        dummy = this->invert();
        dummy = dummy->mulcoeff( cc );
        if ( getRefCount() <= 1 )
        {
            delete this;
            return dummy;
        }
        else
        {
            decRefCount();
            return dummy;
        }
    }
    if ( invert )
    {
        if ( getRefCount() <= 1 )
        {
            delete this;
            return CFFactory::basic( 0 );
        }
        else
        {
            decRefCount();
            return CFFactory::basic( 0 );
        }
    }
    if ( c.isOne() )
        return this;
    else
    {
        if ( getRefCount() <= 1 )
        {
            firstTerm = divTermList( firstTerm, c, lastTerm );
            if ( firstTerm && firstTerm->exp != 0 )
                return this;
            else  if ( firstTerm )
            {
                InternalCF * res = firstTerm->coeff.getval();
                delete this;
                return res;
            }
            else
            {
                delete this;
                return CFFactory::basic( 0 );
            }
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last );
            first = divTermList( first, c, last );
            if ( first && first->exp != 0 )
                return new InternalPoly( first, last, var );
            else  if ( first )
            {
                InternalCF * res = first->coeff.getval();
                delete first;
                return res;
            }
            else
            {
                delete first;
                return CFFactory::basic( 0 );
            }
        }
    }
}

InternalCF*
InternalPoly::tryDivcoeff( InternalCF* cc, bool invert, const CanonicalForm& M, bool& fail )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( inExtension() && !getReduce( var ) && invert )
    {
        InternalCF * dummy;
        dummy = this->tryInvert(M, fail);
        if (fail)
        {
          if (getRefCount() <= 1)
            delete this;
          else
            decRefCount();
          return dummy;
        }
        dummy = dummy->mulcoeff( cc );
        if ( getRefCount() <= 1 )
        {
            delete this;
            return dummy;
        }
        else
        {
            decRefCount();
            return dummy;
        }
    }
    if ( invert )
    {
        if ( getRefCount() <= 1 )
        {
            delete this;
            return CFFactory::basic( 0 );
        }
        else
        {
            decRefCount();
            return CFFactory::basic( 0 );
        }
    }
    if ( c.isOne() )
        return this;
    else
    {
        if ( getRefCount() <= 1 )
        {
            firstTerm = tryDivTermList( firstTerm, c, lastTerm, M, fail );
            if (fail)
            {
              delete this;
              return CFFactory::basic (0);
            }
            if ( firstTerm && firstTerm->exp != 0 )
                return this;
            else  if ( firstTerm )
            {
                InternalCF * res = firstTerm->coeff.getval();
                delete this;
                return res;
            }
            else
            {
                delete this;
                return CFFactory::basic( 0 );
            }
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last );
            first = tryDivTermList( first, c, last, M, fail );
            if (fail)
            {
              delete this;
              return CFFactory::basic (0);
            }
            if (fail)
            {
              delete first;
              return CFFactory::basic (0);
            }
            if ( first && first->exp != 0 )
                return new InternalPoly( first, last, var );
            else  if ( first )
            {
                InternalCF * res = first->coeff.getval();
                delete first;
                return res;
            }
            else
            {
                delete first;
                return CFFactory::basic( 0 );
            }
        }
    }
}

InternalCF*
InternalPoly::modulocoeff( InternalCF* cc, bool invert )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( invert )
    {
        if ( deleteObject() ) delete this;
        return c.getval();
    }
    ASSERT( ! c.isZero(), "divide by zero!" );
    if ( deleteObject() ) delete this;
    return CFFactory::basic( 0 );
}

InternalCF*
InternalPoly::modcoeff( InternalCF* cc, bool invert )
{
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    if ( invert )
    {
        if ( deleteObject() ) delete this;
        return c.getval();
    }
    ASSERT( ! c.isZero(), "divide by zero!" );
    if ( c.isOne() )
    {
        if ( getRefCount() <= 1 )
        {
            delete this;
            return CFFactory::basic( 0 );
        }
        else
        {
            decRefCount();
            return CFFactory::basic( 0 );
        }
    }
    else
    {
        if ( getRefCount() <= 1 )
        {
            firstTerm = modTermList( firstTerm, c, lastTerm );
            if ( firstTerm && firstTerm->exp != 0 )
                return this;
            else  if ( firstTerm )
            {
                InternalCF * res = firstTerm->coeff.getval();
                delete this;
                return res;
            }
            else
            {
                delete this;
                return CFFactory::basic( 0 );
            }
        }
        else
        {
            decRefCount();
            termList last, first = copyTermList( firstTerm, last );
            first = modTermList( first, c, last );
            if ( first && first->exp != 0 )
                return new InternalPoly( first, last, var );
            else  if ( first )
            {
                InternalCF * res = first->coeff.getval();
                delete first;
                return res;
            }
            else
            {
                delete first;
                return CFFactory::basic( 0 );
            }
        }
    }
}

void
InternalPoly::divremcoeff( InternalCF* cc, InternalCF*& quot, InternalCF*& rem, bool invert )
{
    if ( inExtension() && getReduce( var ) )
    {
        quot = copyObject();
        quot = quot->dividecoeff( cc, invert );
        rem = CFFactory::basic( 0 );
    }
    else  if ( invert )
    {
        if ( is_imm( cc ) )
            rem = cc;
        else
            rem = cc->copyObject();
        quot = CFFactory::basic( 0 );
    }
    else
    {
        CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
        ASSERT( ! c.isZero(), "divide by zero!" );
        termList quotlast, quotfirst = copyTermList( firstTerm, quotlast );
        quotfirst = divideTermList( quotfirst, c, quotlast );
        if ( quotfirst )
            if ( quotfirst->exp == 0 )
            {
                quot = quotfirst->coeff.getval();
                delete quotfirst;
            }
            else
                quot = new InternalPoly( quotfirst, quotlast, var );
        else
            quot = CFFactory::basic( 0 );
        rem = CFFactory::basic( 0 );
    }
}

bool
InternalPoly::divremcoefft( InternalCF* cc, InternalCF*& quot, InternalCF*& rem, bool invert )
{
    if ( inExtension() && getReduce( var ) )
    {
        quot = copyObject();
        quot = quot->dividecoeff( cc, invert );
        rem = CFFactory::basic( 0 );
        return true;
    }
    else  if ( invert )
    {
        if ( is_imm( cc ) )
            rem = cc;
        else
            rem = cc->copyObject();
        quot = CFFactory::basic( 0 );
        return true;
    }
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    ASSERT( ! c.isZero(), "divide by zero!" );
    termList quotfirst, quotcursor;
    termList cursor;
    CanonicalForm cquot, crem;
    bool divideok = true;

    cursor = firstTerm;
    quotcursor = quotfirst = new term;

    while ( cursor && divideok )
    {
        divideok = divremt( cursor->coeff, c, cquot, crem );
        divideok = divideok && crem.isZero();
        if ( divideok )
        {
            if ( ! cquot.isZero() )
            {
                quotcursor->next = new term( 0, cquot, cursor->exp );
                quotcursor = quotcursor->next;
            }
            cursor = cursor->next;
        }
    }
    quotcursor->next = 0;
    if ( divideok )
    {
        cursor = quotfirst; quotfirst = quotfirst->next; delete cursor;
        if ( quotfirst )
            if ( quotfirst->exp == 0 )
            {
                quot = quotfirst->coeff.getval();
                delete quotfirst;
            }
            else
                quot = new InternalPoly( quotfirst, quotcursor, var );
        else
            quot = CFFactory::basic( 0 );
        rem = CFFactory::basic( 0 );
    }
    else
    {
        freeTermList( quotfirst );
    }
    return divideok;
}

bool
InternalPoly::tryDivremcoefft( InternalCF* cc, InternalCF*& quot, InternalCF*& rem, bool invert, const CanonicalForm& M, bool& fail )
{
    if ( inExtension() && !getReduce( var ) )
    {
        quot = copyObject();
        quot = quot->tryDividecoeff( cc, invert, M, fail );
        if (fail)
          return false;
        rem = CFFactory::basic( 0 );
        return true;
    }
    else  if ( invert )
    {
        if ( is_imm( cc ) )
            rem = cc;
        else
            rem = cc->copyObject();
        quot = CFFactory::basic( 0 );
        return true;
    }
    CanonicalForm c( is_imm(cc) ? cc : cc->copyObject() );
    ASSERT( ! c.isZero(), "divide by zero!" );
    termList quotfirst, quotcursor;
    termList cursor;
    CanonicalForm cquot, crem;
    bool divideok = true;

    cursor = firstTerm;
    quotcursor = quotfirst = new term;

    while ( cursor && divideok )
    {
        divideok = tryDivremt( cursor->coeff, c, cquot, crem, M, fail );
        if (fail)
        {
          freeTermList (quotfirst);
          return false;
        }
        divideok = divideok && crem.isZero();
        if ( divideok )
        {
            if ( ! cquot.isZero() )
            {
                quotcursor->next = new term( 0, cquot, cursor->exp );
                quotcursor = quotcursor->next;
            }
            cursor = cursor->next;
        }
    }
    quotcursor->next = 0;
    if ( divideok )
    {
        cursor = quotfirst; quotfirst = quotfirst->next; delete cursor;
        if ( quotfirst )
            if ( quotfirst->exp == 0 )
            {
                quot = quotfirst->coeff.getval();
                delete quotfirst;
            }
            else
                quot = new InternalPoly( quotfirst, quotcursor, var );
        else
            quot = CFFactory::basic( 0 );
        rem = CFFactory::basic( 0 );
    }
    else
    {
        freeTermList( quotfirst );
    }
    return divideok;
}

// static functions

termList
InternalPoly::copyTermList ( termList aTermList, termList& theLastTerm, bool negate )
{
    if ( UNLIKELY(aTermList == 0) )
        return 0;
    else  if ( negate )
    {
        termList sourceCursor = aTermList;
        termList dummy = new term;
        termList targetCursor = dummy;

        while ( LIKELY(sourceCursor) )
        {
            targetCursor->next = new term( 0, -sourceCursor->coeff, sourceCursor->exp );
            targetCursor = targetCursor->next;
            sourceCursor = sourceCursor->next;
        }
        targetCursor->next = 0;
        theLastTerm = targetCursor;
        targetCursor = dummy->next;
        delete dummy;
        return targetCursor;
    }
    else
    {
        termList sourceCursor = aTermList;
        termList dummy = new term;
        termList targetCursor = dummy;

        while ( LIKELY(sourceCursor) )
        {
            targetCursor->next = new term( 0, sourceCursor->coeff, sourceCursor->exp );
            targetCursor = targetCursor->next;
            sourceCursor = sourceCursor->next;
        }
        targetCursor->next = 0;
        theLastTerm = targetCursor;
        targetCursor = dummy->next;
        delete dummy;
        return targetCursor;
    }
}

termList
InternalPoly::deepCopyTermList ( termList aTermList, termList& theLastTerm )
{
    if ( aTermList == 0 )
        return 0;
    else
    {
        termList sourceCursor = aTermList;
        termList dummy = new term;
        termList targetCursor = dummy;

        while ( sourceCursor )
        {
            targetCursor->next = new term( 0, sourceCursor->coeff.deepCopy(), sourceCursor->exp );
            targetCursor = targetCursor->next;
            sourceCursor = sourceCursor->next;
        }
        targetCursor->next = 0;
        theLastTerm = targetCursor;
        targetCursor = dummy->next;
        delete dummy;
        return targetCursor;
    }
}

void
InternalPoly::freeTermList ( termList aTermList )
{
    termList cursor = aTermList;

    while ( LIKELY(cursor) )
    {
        cursor = cursor->next;
        delete aTermList;
        aTermList = cursor;
    }
}

void
InternalPoly::negateTermList ( termList terms )
{
    termList cursor = terms;
    while ( LIKELY(cursor) )
    {
        cursor->coeff = -cursor->coeff;
        cursor = cursor->next;
    }
}

termList
InternalPoly::addTermList ( termList theList, termList aList, termList& lastTerm, bool negate )
{
    termList theCursor = theList;
    termList aCursor = aList;
    termList predCursor = 0;

    if (negate)
      while ( theCursor && aCursor )
      {
        if ( theCursor->exp == aCursor->exp )
        {
            theCursor->coeff -= aCursor->coeff;
            if ( theCursor->coeff.isZero() )
            {
                if ( predCursor )
                {
                    predCursor->next = theCursor->next;
                    delete theCursor;
                    theCursor = predCursor->next;
                }
                else
                {
                    theList = theList->next;
                    delete theCursor;
                    theCursor = theList;
                }
            }
            else
            {
                predCursor = theCursor;
                theCursor = theCursor->next;
            }
            aCursor = aCursor->next;
        }
        else if ( theCursor->exp < aCursor->exp )
        {
            if ( predCursor )
            {
                predCursor->next = new term( theCursor, -aCursor->coeff, aCursor->exp );
                predCursor = predCursor->next;
            }
            else
            {
                theList = new term( theCursor, -aCursor->coeff, aCursor->exp );
                predCursor = theList;
            }
            aCursor = aCursor->next;
        }
        else
        {
            predCursor = theCursor;
            theCursor = theCursor->next;
        }
      }
    else
    while ( theCursor && aCursor )
    {
        if ( theCursor->exp == aCursor->exp )
        {
            theCursor->coeff += aCursor->coeff;
            if ( theCursor->coeff.isZero() )
            {
                if ( predCursor )
                {
                    predCursor->next = theCursor->next;
                    delete theCursor;
                    theCursor = predCursor->next;
                }
                else
                {
                    theList = theList->next;
                    delete theCursor;
                    theCursor = theList;
                }
            }
            else
            {
                predCursor = theCursor;
                theCursor = theCursor->next;
            }
            aCursor = aCursor->next;
        }
        else if ( theCursor->exp < aCursor->exp )
        {
            if ( predCursor )
            {
                predCursor->next = new term( theCursor, aCursor->coeff, aCursor->exp );
                predCursor = predCursor->next;
            }
            else
            {
                theList = new term( theCursor, aCursor->coeff, aCursor->exp );
                predCursor = theList;
            }
            aCursor = aCursor->next;
        }
        else
        {
            predCursor = theCursor;
            theCursor = theCursor->next;
        }
    }
    if ( aCursor )
    {
        if ( predCursor )
            predCursor->next = copyTermList( aCursor, lastTerm, negate );
        else
            theList = copyTermList( aCursor, lastTerm, negate );
    }
    else if ( ! theCursor )
        lastTerm = predCursor;

    return theList;
}

void
InternalPoly::mulTermList ( termList theCursor, const CanonicalForm& coeff, const int exp )
{
    while ( LIKELY(theCursor) )
    {
        theCursor->coeff *= coeff;
        theCursor->exp += exp;
        theCursor = theCursor->next;
    }
}

termList
InternalPoly::divideTermList ( termList firstTerm, const CanonicalForm& coeff, termList& lastTerm )
{
    termList theCursor = firstTerm;
    lastTerm = 0;
    termList dummy;

    while ( LIKELY(theCursor) )
    {
        theCursor->coeff /= coeff;
        if ( theCursor->coeff.isZero() )
        {
            if ( theCursor == firstTerm )
                firstTerm = theCursor->next;
            else
                lastTerm->next = theCursor->next;
            dummy = theCursor;
            theCursor = theCursor->next;
            delete dummy;
        }
        else
        {
            lastTerm = theCursor;
            theCursor = theCursor->next;
        }
    }
    return firstTerm;
}

termList
InternalPoly::divTermList ( termList firstTerm, const CanonicalForm& coeff, termList& lastTerm )
{
    termList theCursor = firstTerm;
    lastTerm = 0;
    termList dummy;

    while ( LIKELY(theCursor) )
    {
        theCursor->coeff.div( coeff );
        if ( theCursor->coeff.isZero() )
        {
            if ( theCursor == firstTerm )
                firstTerm = theCursor->next;
            else
                lastTerm->next = theCursor->next;
            dummy = theCursor;
            theCursor = theCursor->next;
            delete dummy;
        }
        else
        {
            lastTerm = theCursor;
            theCursor = theCursor->next;
        }
    }
    return firstTerm;
}

termList
InternalPoly::tryDivTermList ( termList firstTerm, const CanonicalForm& coeff, termList& lastTerm, const CanonicalForm& M, bool& fail )
{
    termList theCursor = firstTerm;
    lastTerm = 0;
    termList dummy;

    while ( theCursor )
    {
        theCursor->coeff.tryDiv( coeff, M, fail );
        if (fail)
          return 0;
        if ( theCursor->coeff.isZero() )
        {
            if ( theCursor == firstTerm )
                firstTerm = theCursor->next;
            else
                lastTerm->next = theCursor->next;
            dummy = theCursor;
            theCursor = theCursor->next;
            delete dummy;
        }
        else
        {
            lastTerm = theCursor;
            theCursor = theCursor->next;
        }
    }
    return firstTerm;
}

termList
InternalPoly::modTermList ( termList firstTerm, const CanonicalForm& coeff, termList& lastTerm )
{
    termList theCursor = firstTerm;
    lastTerm = 0;
    termList dummy;

    while ( theCursor )
    {
        theCursor->coeff.mod( coeff );
        if ( theCursor->coeff.isZero() )
        {
            if ( theCursor == firstTerm )
                firstTerm = theCursor->next;
            else
                lastTerm->next = theCursor->next;
            dummy = theCursor;
            theCursor = theCursor-> next;
            delete dummy;
        }
        else
        {
            lastTerm = theCursor;
            theCursor = theCursor->next;
        }
    }
    return firstTerm;
}

void
InternalPoly::appendTermList ( termList& first, termList& last, const CanonicalForm& coeff, const int exp )
{
    if ( last )
    {
        last->next = new term( 0, coeff, exp );
        last = last->next;
    }
    else
    {
        first = new term( 0, coeff, exp );
        last = first;
    }
}

termList
InternalPoly::mulAddTermList ( termList theList, termList aList, const CanonicalForm & c, const int exp, termList & lastTerm, bool negate )
{
    termList theCursor = theList;
    termList aCursor = aList;
    termList predCursor = 0;
    CanonicalForm coeff;

    if ( negate )
        coeff = -c;
    else
        coeff = c;

    while ( theCursor && aCursor )
    {
        if ( theCursor->exp == aCursor->exp + exp )
        {
            theCursor->coeff += aCursor->coeff * coeff;
            if(UNLIKELY(( theCursor->coeff.isZero() )))
            {
                if ( predCursor )
                {
                    predCursor->next = theCursor->next;
                    delete theCursor;
                    theCursor = predCursor->next;
                }
                else
                {
                    theList = theList->next;
                    delete theCursor;
                    theCursor = theList;
                }
            }
            else
            {
                predCursor = theCursor;
                theCursor = theCursor->next;
            }
            aCursor = aCursor->next;
        }
        else if ( theCursor->exp < aCursor->exp + exp )
        {
            if ( predCursor )
            {
                predCursor->next = new term( theCursor, aCursor->coeff * coeff, aCursor->exp + exp );
                predCursor = predCursor->next;
            }
            else
            {
                theList = new term( theCursor, aCursor->coeff * coeff, aCursor->exp + exp );
                predCursor = theList;
            }
            aCursor = aCursor->next;
        }
        else
        {
            predCursor = theCursor;
            theCursor = theCursor->next;
        }
    }
    if ( aCursor )
    {
        if ( predCursor )
        {
            predCursor->next = copyTermList( aCursor, lastTerm );
            predCursor = predCursor->next;
        }
        else
        {
            theList = copyTermList( aCursor, lastTerm );
            predCursor = theList;
        }
        while ( predCursor )
        {
            predCursor->exp += exp;
            predCursor->coeff *= coeff;
            predCursor = predCursor->next;
        }
    }
    else if ( ! theCursor )
        lastTerm = predCursor;
    return theList;
}

termList
InternalPoly::reduceTermList ( termList first, termList redterms, termList & last )
{
    CanonicalForm coeff = CanonicalForm (1)/redterms->coeff;
    CanonicalForm newcoeff;
    int newexp;
    int exp = redterms->exp;
    termList dummy;
    while ( first && ( first->exp >= exp ) )
    {
        newcoeff = first->coeff * coeff;
        newexp = first->exp - exp;
        dummy = first;
        first = mulAddTermList( first->next, redterms->next, newcoeff, newexp, last, true );
        delete dummy;
    }
    return first;
}

