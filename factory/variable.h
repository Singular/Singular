/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: variable.h,v 1.4 2005-04-13 15:01:53 Singular Exp $ */

#ifndef INCL_VARIABLE_H
#define INCL_VARIABLE_H

#include <config.h>

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

#include "cf_defs.h"

/*BEGINPUBLIC*/

class CanonicalForm;

class Variable
{
private:
    int _level;
    Variable( int l, bool flag );
public:
    Variable() : _level(LEVELBASE) {}
    Variable( int l );
    Variable( char name );
    Variable( int l, char name );
    Variable( const Variable & v ) : _level(v._level) {}
    ~Variable() {};
    Variable& operator= ( const Variable & v )
    {
	_level = v._level;
	return *this;
    }
    int level() const { return _level; }
    char name() const;
    static Variable highest() { return Variable( LEVELQUOT-1 ); }
    Variable next() const { return Variable( _level+1 ); }
    friend bool operator == ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level == rhs._level;
    }
    friend bool operator != ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level != rhs._level;
    }
    friend bool operator > ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level > rhs._level;
    }
    friend bool operator < ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level < rhs._level;
    }
    friend bool operator >= ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level >= rhs._level;
    }
    friend bool operator <= ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level <= rhs._level;
    }
#ifndef NOSTREAMIO
    friend ostream & operator << ( ostream & os, const Variable & v );
#endif /* NOSTREAMIO */
    friend void swap_levels();
    friend Variable rootOf( const CanonicalForm &, char name = '@' );
};

inline int level( const Variable & v ) { return v.level(); }
inline char name( const Variable & v ) { return v.name(); }

CanonicalForm getMipo( const Variable & alpha, const Variable & x );

char getDefaultVarName();
char getDefaultExtName();

int ExtensionLevel();

/*ENDPUBLIC*/

// the following functions do not need to be public available
CanonicalForm getMipo( const Variable & alpha );
class InternalPoly;
InternalPoly * getInternalMipo ( const Variable & alpha );
bool getReduce( const Variable & alpha );
void setReduce( const Variable & alpha, bool reduce );

#endif /* ! INCL_VARIABLE_H */
