/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file variable.h
 *
 * operations on variables
**/
#ifndef INCL_VARIABLE_H
#define INCL_VARIABLE_H

// #include "config.h"

#ifndef NOSTREAMIO
# ifdef HAVE_IOSTREAM
#  include <iostream>
#  define OSTREAM std::ostream
# elif defined(HAVE_IOSTREAM_H)
#  include <iostream.h>
#  define OSTREAM ostream
# endif
#endif /* NOSTREAMIO */

#include "cf_defs.h"

/*BEGINPUBLIC*/

class CanonicalForm;

/**
 * factory's class for variables
**/
class FACTORY_PUBLIC Variable
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
   friend OSTREAM & operator << ( OSTREAM & os, const Variable & v );
#endif /* NOSTREAMIO */
    friend void swap_levels();
    /** returns a symbolic root of polynomial with name @a name.
     *  Use it to define algebraic variables
     *  @note: algebraic variables have a level < 0
    **/
    friend Variable rootOf( const CanonicalForm &, char name );
};

/** returns a symbolic root of polynomial with name @a name
 *  Use it to define algebraic variables
 *  @note: algebraic variables have a level < 0
**/
Variable FACTORY_PUBLIC rootOf( const CanonicalForm &, char name = '@' );

inline int level( const Variable & v ) { return v.level(); }
inline char name( const Variable & v ) { return v.name(); }

void setReduce( const Variable & alpha, bool reduce );
void setMipo ( const Variable & alpha, const CanonicalForm & mipo);
CanonicalForm getMipo( const Variable & alpha, const Variable & x );
bool hasMipo( const Variable & alpha );

char getDefaultVarName();
char getDefaultExtName();

void FACTORY_PUBLIC prune (Variable& alpha);
void prune1 (const Variable& alpha);
int ExtensionLevel();

/*ENDPUBLIC*/

// the following functions do not need to be public available
CanonicalForm getMipo( const Variable & alpha );
class InternalPoly;
InternalPoly * getInternalMipo ( const Variable & alpha );
bool getReduce( const Variable & alpha );

#endif /* ! INCL_VARIABLE_H */
