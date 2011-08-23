/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_VAR_INTGLOBAL_H
#define INCL_VAR_INTGLOBAL_H

//{{{ docu
//
// var_intglob.h - header to var_intglob.cc.
//
//}}}

// #include "config.h"

class Prim;
class InternalPoly;

//{{{ template <class T> class PrimitiveArray
//{{{ docu
//
// class PrimitiveArray - primitive array template class.
//
// This template implements a dynamical growing array which may
// have gaps.  As soon as you access any element outside the
// array the array is resized to contain that element.  Any
// elements that are created "between" the last existing element
// and the newly created element are marked as "uninitialized".
// Using this flag it is possible to check which of the elements
// previously have been accessed and which not.
//
// Consequently, as soon as some element of the array is accessed
// using operator []() that element will be marked as
// "initialized".
//
// _array: the array itself
// _initializedFlags: used to keep track of initialized elements
// _size: the current size of _array
//
//}}}
//{{{ inline method docu
//
// int size () const
// T * array () const
//
// size() - return size of array.
// array() - return pointer to the underlying array.  Use with
//   care!
//
//}}}
template <class T>
class PrimitiveArray
{
private:
    T * _array;
    bool * _initializedFlags;
    int _size;
public:
    PrimitiveArray () : _array( 0 ), _initializedFlags( 0 ), _size( 0 ) {}
    ~PrimitiveArray () { delete [] _array; delete [] _initializedFlags; }

    T & operator [] ( int i );
    bool isInitialized ( int i ) const;
    int getFirstUninitialized () const;

private:
    int size () const { return _size; }
    T * array () const { return _array; }
};
//}}}

//{{{ class globalVarData
//{{{ docu
//
// class globalVarData - global data associated with a variable.
//
// This class holds information which may be associated with any
// variable: a short name for reading and printing the variable
// and a long name for printing it only.  The class keeps copies
// of all long names passed to it (and not only the pointers).
//
// _name: short name
// _longName: long name
//
//}}}
//{{{ inline method docu
//
// virtual char & name ()
// virtual char * longName ()
// virtual void setLongName ( char * longName )
//
// name() - return name
// longName() - return long name.  No new copy of long name is
//   created, so be careful with this pointer.
// setLongName() - set long name to longName.  As mentioned
//   before, a new copy is created.
//
//}}}
class globalVarData
{
private:
    char _name;
    char * _longName;

    void initializeLongName( char * );

public:
    // constructors, destructors, assignment
    globalVarData ( char name = 0, char * longName = 0 );
    globalVarData ( const globalVarData & );

    virtual ~globalVarData () { delete [] _longName; }

    globalVarData & operator = ( const globalVarData & );

    // selectors
    virtual char & name () { return _name; }
    virtual char * longName () const { return _longName; }
    virtual void setLongName ( char * longName )
	{ initializeLongName( longName ); }

    // comparisons
    friend bool operator == ( const globalVarData &, const globalVarData & );
    friend bool operator != ( const globalVarData &, const globalVarData & );
};
//}}}

//{{{ class globalAlgData : public globalVarData
//{{{ docu
//
// class globalAlgData - global data associated with algebraic
//   variables.
//
// This class is derived from class globalVarData.  It adds the
// information on the minimal polynomial associated with the
// algebraic variable and a flag whether arithmetic operations
// should reduce module the minimal polynomial or not.
//
// _mipo: pointer to the minimal polynomial.  This class does
//   merely keeps the pointer.  It neither deletes it nor calls
//   any methods of the InternalPoly which it references.
// _reduce: reduce flag
//
//}}}
//{{{ inline method docu
//
// InternalPoly * & mipo ()
// bool & reduce ()
//
// mipo() - return the minimal polynomial
// reduce() - return the reduce flag
//
//}}}
class globalAlgData : public globalVarData
{
private:
    InternalPoly * _mipo;
    bool _reduce;

public:
    // constructors, destructors, assignment
    globalAlgData ( char name = 0, char * longName = 0, InternalPoly * mipo = 0, bool reduce = false )
	: globalVarData( name, longName ), _mipo( mipo ), _reduce( reduce ) {}
    globalAlgData ( const globalAlgData & d )
	: globalVarData( d ), _mipo( d._mipo ), _reduce( d._reduce ) {}

    globalAlgData & operator = ( const globalAlgData & );

    // selectors
    InternalPoly * & mipo () { return _mipo; }
    bool & reduce () { return _reduce; }
};
//}}}

//{{{ class globalAlgNumData : public globalVarData
//{{{ docu
//
// class globalAlgData - global data associated with algebraic
//   variables.
//
// This class is derived from class globalVarData.  It adds the
// information on the prim element associated with the variable.
//
// _prim: pointer to the prim element.  This class does
//   merely keeps the pointer.  It neither deletes it nor calls
//   any methods of the Prim which it references.
//
//}}}
//{{{ inline method docu
//
// Prim * & prim ()
//
// prim() - return the prim element
//
//}}}
class globalAlgNumData : public globalVarData
{
private:
    Prim * _prim;

public:
    // constructors, destructors, assignment
    globalAlgNumData ( char name = 0, char * longName = 0, Prim * prim = 0 )
	: globalVarData( name, longName ), _prim( prim ) {}
    globalAlgNumData ( const globalAlgNumData & d )
	: globalVarData( d ), _prim( d._prim ) {}

    globalAlgNumData & operator = ( const globalAlgNumData & );

    // selectors
    Prim * & prim () { return _prim; }
};
//}}}

#endif /* ! INCL_VAR_INTGLOBAL_H */
