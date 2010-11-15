/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#include <string.h>

#include "assert.h"

#include "cf_defs.h"
#include "variable.h"
#include "canonicalform.h"
#include "cf_factory.h"
#include "int_poly.h"
#include "cf_iter.h"

class ext_entry
{
private:
    InternalPoly * _mipo;
    bool _reduce;
public:
    ext_entry () : _mipo(0), _reduce(false) {}
    ext_entry ( InternalPoly * mipoly, bool reduce ) : _mipo(mipoly), _reduce(reduce) {};
    ext_entry ( const ext_entry & e ) : _mipo(e._mipo), _reduce(e._reduce) {}
    ~ext_entry () {};
    ext_entry & operator= ( const ext_entry & e )
    {
        if ( this != &e ) {
            _mipo = e._mipo;
            _reduce = e._reduce;
        }
        return *this;
    }
    InternalPoly * mipo () { return _mipo; }
    void setmipo( InternalPoly * p ) { _mipo = p; }
    bool & reduce () { return _reduce; }
};

static ext_entry * algextensions = 0;
static char * var_names = 0;
static char * var_names_ext = 0;
static char default_name = 'v';
static char default_name_ext = 'a';

Variable::Variable( int l, bool flag ) : _level(l)
{
    ASSERT( flag, "illegal level" );
}

Variable::Variable( int l ) : _level(l)
{
    ASSERT( l > 0 && l != LEVELQUOT, "illegal level" );
}

Variable::Variable( char name )
{
    bool isext = false;
    int n, i;
    if ( var_names_ext != 0 ) {
        n = strlen( var_names_ext );
        i = 1;
        while ( i < n && var_names_ext[i] != name ) i++;
        if ( i < n ) {
            _level = -i;
            isext = true;
        }
    }
    if ( ! isext ) {
        if ( var_names == 0 ) {
            var_names = new char [3];
            var_names[0] = '@';
            var_names[1] = name;
            var_names[2] = '\0';
            _level = 1;
        }
        else {
            n = strlen( var_names );
            i = 1;
            while ( i < n && var_names[i] != name ) i++;
            if ( i < n )
                _level = i;
            else {
                ASSERT( name != '@', "illegal variable name" );
                char * newvarnames = new char [n+2];
                for ( i = 0; i < n; i++ )
                    newvarnames[i] = var_names[i];
                newvarnames[n] = name;
                newvarnames[n+1] = 0;
                delete [] var_names;
                var_names = newvarnames;
                _level = n;
            }
        }
    }
}

Variable::Variable( int l, char name ) : _level(l)
{
    ASSERT( l > 0 && l != LEVELQUOT, "illegal level" );
    int n;
    if ( (n = (var_names == 0 ? 0 : strlen( var_names ))) <= l ) {
        char * newvarnames = new char [l+2];
        int i;
        for ( i = 0; i < n; i++ )
            newvarnames[i] = var_names[i];
        for ( i = n; i < l; i++ )
            newvarnames[i] = '@';
        newvarnames[l] = name;
        newvarnames[l+1] = 0;
        delete [] var_names;
        var_names = newvarnames;
    }
    else {
        ASSERT( var_names[l] == '@', "illegal name" );
        var_names[l] = name;
    }
}

char
Variable::name() const
{
    if ( _level > 0 && _level < (int)strlen( var_names ) )
        return( var_names[_level] );
    else if ( _level < 0 && -_level < (int)strlen( var_names_ext ) )
        return( var_names_ext[-_level] );
    else
        return '@';
}

#ifndef NOSTREAMIO
OSTREAM & operator << ( OSTREAM & os, const Variable & v )
{
    if ( v._level == LEVELBASE )
        os << "1";
    else {
        char * vn = ( v._level > 0 ) ? var_names : var_names_ext;
        char dn = ( v._level > 0 ) ? default_name : default_name_ext;
        int l = v._level;

        if ( l < 0 ) l = -l;
        if ( (vn == 0) || ((int)strlen( vn ) <= l) )
            os << dn << "_" << l;
        else  if ( vn[l] == '@' )
            os << dn << "_" << l;
        else
            os << vn[l];
    }
    return os;
}
#endif /* NOSTREAMIO */

//static bool legal_mipo( const CanonicalForm & mipo )
//{
//    ASSERT( mipo.inPolyDomain(), "not a legal extension" );
//    bool ok = true;
//    for ( CFIterator i = mipo; ok && i.hasTerms(); i++ )
//        ok = i.coeff().inBaseDomain();
//    return ok;
//}

static CanonicalForm conv2mipo ( const CanonicalForm & mipo, const Variable alpha )
{
    CanonicalForm result;
    for ( CFIterator i = mipo; i.hasTerms(); i++ )
        result += i.coeff() * power( alpha, i.exp() );
    return result;
}

Variable rootOf( const CanonicalForm & mipo, char name )
{
    ASSERT( legal_mipo( mipo ), "not a legal extension" );

    int l;
    if ( var_names_ext == 0 ) {
        var_names_ext = new char [3];
        var_names_ext[0] = '@';
        var_names_ext[1] = name;
        var_names_ext[2] = '\0';
        l = 1;
        Variable result( -l, true );
        algextensions = new ext_entry [2];
        algextensions[1] = ext_entry( 0, false );
        algextensions[1] = ext_entry( (InternalPoly*)(conv2mipo( mipo, result ).getval()), true );
        return result;
    }
    else {
        int i, n = strlen( var_names_ext );
        char * newvarnames = new char [n+2];
        for ( i = 0; i < n; i++ )
            newvarnames[i] = var_names_ext[i];
        newvarnames[n] = name;
        newvarnames[n+1] = 0;
        delete [] var_names_ext;
        var_names_ext = newvarnames;
        l = n;
        Variable result( -l, true );
        ext_entry * newalgext = new ext_entry [n+1];
        for ( i = 0; i < n; i++ )
            newalgext[i] = algextensions[i];
        newalgext[n] = ext_entry( 0, false );
        delete [] algextensions;
        algextensions = newalgext;
        algextensions[n] = ext_entry( (InternalPoly*)(conv2mipo( mipo, result ).getval()), true );
        return result;
    }
}

InternalPoly * getInternalMipo ( const Variable & alpha )
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    return algextensions[-alpha.level()].mipo();
}

CanonicalForm getMipo( const Variable & alpha, const Variable & x )
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    return CanonicalForm( algextensions[-alpha.level()].mipo()->copyObject() )(x,alpha);
}

CanonicalForm getMipo( const Variable & alpha )
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    return CanonicalForm( algextensions[-alpha.level()].mipo()->copyObject() );
}

/*void setMipo ( const Variable & alpha, const CanonicalForm & mipo)
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    algextensions[-alpha.level()]= ext_entry((InternalPoly*)(conv2mipo( mipo, alpha ).getval()), true );
}*/

bool hasMipo( const Variable & alpha )
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    return ((algextensions!=NULL) && getReduce(alpha) );
}

bool getReduce( const Variable & alpha )
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    return algextensions[-alpha.level()].reduce();
}

void setReduce( const Variable & alpha, bool reduce )
{
    ASSERT( alpha.level() < 0 && alpha.level() != LEVELBASE, "illegal extension" );
    algextensions[-alpha.level()].reduce() = reduce;
}

char getDefaultVarName()
{
    return default_name;
}

char getDefaultExtName()
{
    return default_name_ext;
}

int ExtensionLevel()
{
  if( var_names_ext == 0)
    return 0;
  return strlen( var_names_ext )-1;
}

void Reduce( bool on)
{
  int i;
  for (i=ExtensionLevel(); i>0;i--)
  {
    Variable l(-i);
    setReduce(l,on);
  }
}
