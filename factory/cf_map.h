// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_map.h,v 1.0 1996-05-17 10:59:38 stobbe Exp $

#ifndef INCL_CF_MAP_H
#define INCL_CF_MAP_H

/*
$Log: not supported by cvs2svn $
*/

#include <iostream.h>
#include "variable.h"
#include "templates/list.h"
#include "canonicalform.h"

/*BEGINPUBLIC*/

class MapPair
{
private:
    Variable V;
    CanonicalForm S;
public:
    MapPair( const Variable & v, const CanonicalForm & s ) : V(v), S(s) {}
    MapPair() : V(), S(1) {}
    MapPair( const MapPair & p ) : V(p.V), S(p.S) {}
    ~MapPair() {}
    MapPair& operator=( const MapPair & p );
    Variable var() const { return V; }
    CanonicalForm subst() const { return S; }
    friend ostream& operator << ( ostream& s, const MapPair & p );
};

typedef List<MapPair> MPList;
typedef ListIterator<MapPair> MPListIterator;


class CFMap
{
private:
  MPList P;
public:
  CFMap() {}
  CFMap( const CanonicalForm & s ) : P( MapPair( Variable(), s ) ) {}
  CFMap( const Variable & v ) : P( MapPair( v, 1 ) ) {}
  CFMap( const Variable & v, const CanonicalForm & s ) : P( MapPair(v,s) ) {}
  ~CFMap() {}
  CFMap( const List<CanonicalForm> & L );
  CFMap( const CFMap & m ) : P( m.P ) {}
  CFMap& operator=( const CFMap & m );
  void newpair( const Variable & v, const CanonicalForm & s );
  CanonicalForm operator() ( const CanonicalForm & f ) const;
  friend ostream& operator<< ( ostream& s, const CFMap & m );
};

CanonicalForm compress ( const CanonicalForm & f, CFMap & m );
void compress ( const CFArray & a, CFMap & M, CFMap & N );

/*ENDPUBLIC*/

#endif /* INCL_CF_MAP_H */

