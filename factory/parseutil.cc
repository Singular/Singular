/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#include <string.h>
#include <stdlib.h>

#include "cf_assert.h"

#include "cf_defs.h"
#include "parseutil.h"

class PUtilBase
{
public:
    PUtilBase() {};
    virtual ~PUtilBase() {}
    virtual PUtilBase * copy() const = 0;
    virtual CanonicalForm getval() const = 0;
    virtual int getintval() const = 0;
    virtual bool isInt() const = 0;
    virtual bool isCF() const = 0;
    virtual bool isVar() const = 0;
};

class PUtilInt : public PUtilBase
{
private:
    int val;
public:
    PUtilInt() { val = 0; }
    PUtilInt( int i ) { val = i; }
    ~PUtilInt() {}
    PUtilBase * copy() const { return new PUtilInt( val ); }
    CanonicalForm getval() const { return CanonicalForm( val ); }
    int getintval() const { return val; }
    bool isInt() const { return true; }
    bool isCF() const { return false; }
    bool isVar() const { return false; }
};

class PUtilCF : public PUtilBase
{
private:
    CanonicalForm val;
public:
    PUtilCF() { val = 0; }
    PUtilCF( const CanonicalForm & cf ) { val = cf; }
    ~PUtilCF() {}
    PUtilBase * copy() const { return new PUtilCF( val ); }
    CanonicalForm getval() const { return val; }
    int getintval() const { return val.intval(); }
    bool isInt() const { return false; }
    bool isCF() const { return true; }
    bool isVar() const { return false; }
};

class PUtilVar : public PUtilBase
{
private:
    Variable val;
public:
    PUtilVar() { val = Variable(); }
    PUtilVar( const Variable & v ) { val = v; }
    ~PUtilVar() {}
    PUtilBase * copy() const { return new PUtilVar( val ); }
    CanonicalForm getval() const { return CanonicalForm( val ); }
    int getintval() const { return 0; }
    bool isInt() const { return false; }
    bool isCF() const { return false; }
    bool isVar() const { return true; }
};

class PUtilFactory
{
public:
    static PUtilBase * create( ) { return new PUtilInt( 0 ); }
    static PUtilBase * create( int val ) { return new PUtilInt( val ); }
    static PUtilBase * create( const CanonicalForm & cf ) { return new PUtilCF( cf ); }
    static PUtilBase * create( const Variable & v ) { return new PUtilVar( v ); }
    static PUtilBase * create( const char * str )
    {
	if ( strlen( str ) < 9 )
	    return new PUtilInt( atoi( str ) );
	else
	    return new PUtilCF( CanonicalForm( str ) );
    }
};

ParseUtil::ParseUtil()
{
    value = PUtilFactory::create();
}

ParseUtil::ParseUtil( const ParseUtil &pu )
{
    value = pu.value->copy();
}

ParseUtil::ParseUtil( const CanonicalForm &f )
{
    value = PUtilFactory::create( f );
}

ParseUtil::ParseUtil( int i )
{
    value = PUtilFactory::create( i );
}

ParseUtil::ParseUtil( char * str )
{
    value = PUtilFactory::create( str );
}

ParseUtil::~ParseUtil()
{
    delete value;
}

ParseUtil& ParseUtil::operator= ( const ParseUtil &pu )
{
    if ( this != &pu ) {
	delete value;
	value = pu.value->copy();
    }
    return *this;
}

ParseUtil& ParseUtil::operator= ( const CanonicalForm &f )
{
    delete value;
    value = PUtilFactory::create( f );
    return *this;
}

ParseUtil& ParseUtil::operator= ( int i )
{
    delete value;
    value = PUtilFactory::create( i );
    return *this;
}

ParseUtil& ParseUtil::operator= ( const Variable & v )
{
    delete value;
    value = PUtilFactory::create( v );
    return *this;
}

CanonicalForm ParseUtil::getval()
{
    return value->getval();
}

int ParseUtil::getintval()
{
    return value->getintval();
}
