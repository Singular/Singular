/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

//{{{ docu
//
// var_intglobal.cc - internal data layout for global variable
//   data.
//
// In this file, some auxiliary classes and template classes are
// defined which help managing global data associated with
// variables.
//
// Used by: var_global.cc
//
//}}}

#include <config.h>

#include "assert.h"

#include <string.h>

#include "var_intglobal.h"

class Prim;
class InternalPoly;

//{{{ template <class T> T & PrimitiveArray<T>::operator [] ( int i )
//{{{ docu
//
// operator []() - get i'th element.
//
// Resize array if necessary.  Mark the i'th element as
// accessed.
//
//}}}
template <class T>
T &
PrimitiveArray<T>::operator [] ( int i )
{
    ASSERT( i >= 0, "array index less than zero" );
    if ( i < _size ) {
        _initializedFlags[ i ] = true;
        return _array[ i ];
    } else {
        // resize arrays
        T * newArray = new T[ i+1 ];
        bool * newFlags = new bool[ i+1 ];
        int j = 0;
        // copy old contents
        while ( j < _size ) {
            newArray[ j ] = _array[ j ];
            newFlags[ j ] = _initializedFlags[ j ];
            j++;
        }
        // mark rest as uninitialized
        while ( j < i ) {
            newFlags[ j ] = false;
            j++;
        }
        // actual element mark as initialized
        newFlags[ i ] = true;

        // make new arrays actual ones
        delete [] _array;
        delete [] _initializedFlags;
        _array = newArray;
        _initializedFlags = newFlags;
        _size = i + 1;

        return _array[ i ];
    }
}
//}}}

//{{{ template <class T> bool PrimitiveArray<T>::isInitialized ( int i ) const
//{{{ docu
//
// isInitialized() - check whether element i is already
//   initialized ( = has been accessed previously).
//
//}}}
template <class T>
bool
PrimitiveArray<T>::isInitialized ( int i ) const
{
    ASSERT( i >= 0, "array index less than zero" );
    if ( i >= _size )
        return false;
    else
        return _initializedFlags[ i ];
}
//}}}

//{{{ template <class T> int PrimitiveArray<T>::getFirstUninitialized () const
//{{{ docu
//
// getFirstUninitialized() - get index to first uninitialized
//   element.
//
//}}}
template <class T>
int
PrimitiveArray<T>::getFirstUninitialized () const
{
    int i = 0;
    while ( i < _size && _initializedFlags[ i ] )
        i++;
    return i;
}
//}}}

//{{{ void globalVarData::initializeLongName( char * longName )
//{{{ docu
//
// initializeLongName() - correctly initialize _longName from
//   longName.
//
// _longName is first destroyed.  If longName does not equal zero
// we copy it to _longName after allocating memory for it.
//
// Note: be sure that _longName may be destroyed (e.g. when
// calling this function from constructors).
//
//}}}
void
globalVarData::initializeLongName( char * longName )
{
    delete [] _longName;
    if ( ! longName )
        _longName = 0;
    else {
        _longName = new char[ strlen( longName ) + 1 ];
        strcpy( _longName, longName );
    }
}
//}}}

//{{{ globalVarData::globalVarData ( char name, char * longName )
//{{{ docu
//
// globalVarData() - (default) constructor.
//
//}}}
globalVarData::globalVarData ( char name, char * longName ) : _name( name )
{
    _longName = 0;
    initializeLongName( longName );
}
//}}}

//{{{ globalVarData::globalVarData ( const globalVarData & d )
//{{{ docu
//
// globalVarData() - copy constructor.
//
//}}}
globalVarData::globalVarData ( const globalVarData & d ) : _name( d._name )
{
    _longName = 0;
    initializeLongName( d._longName );
}
//}}}

//{{{ globalVarData & globalVarData::operator = ( const globalVarData & d )
//{{{ docu
//
// operator =() - assignment operator.
//
//}}}
globalVarData &
globalVarData::operator = ( const globalVarData & d )
{
    if ( this != &d ) {
        initializeLongName( d._longName );
        _name = d._name;
    }
    return *this;
}
//}}}

//{{{ bool operator ==, operator != ( const globalVarData & lhs, const globalVarData & rhs )
//{{{ docu
//
// operator ==(), operator !=() - compare two global data fields.
//
// Two data fields are equal iff their long names are equal.
//
//}}}
bool
operator == ( const globalVarData & lhs, const globalVarData & rhs )
{
    if ( ! lhs._longName && ! rhs._longName )
        return true;
    if ( lhs._longName && rhs._longName )
        return ! strcmp ( lhs._longName, rhs._longName );
    else
        return false;
}

bool
operator != ( const globalVarData & lhs, const globalVarData & rhs )
{
    return ! (lhs == rhs);
}
//}}}

//{{{ globalAlgData & globalAlgData::operator = ( const globalAlgData & d )
//{{{ docu
//
// operator =() - assignment operator.
//
//}}}
globalAlgData &
globalAlgData::operator = ( const globalAlgData & d )
{
    if ( this != &d ) {
        globalVarData::operator= ( d );
        _mipo = d._mipo;
        _reduce = d._reduce;
    }
    return *this;
}
//}}}

//{{{ globalAlgNumData & globalAlgNumData::operator = ( const globalAlgNumData & d )
//{{{ docu
//
// operator =() - assignment operator.
//
//}}}
globalAlgNumData &
globalAlgNumData::operator = ( const globalAlgNumData & d )
{
    if ( this != &d ) {
        (globalVarData)*this = d;
        _prim = d._prim;
    }
    return *this;
}
//}}}

// for convenience, the templates are instantiated in this place
// instead of in ftmpl_inst.cc
template class PrimitiveArray<globalVarData>;
template class PrimitiveArray<globalAlgNumData>;
template class PrimitiveArray<globalAlgData>;
