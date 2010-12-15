/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_CF_MAP_H
#define INCL_CF_MAP_H

//{{{ docu
//
// cf_map.h - header to cf_map.cc.
//
//}}}

#include <config.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "variable.h"
#include "canonicalform.h"
#include "templates/ftmpl_list.h"

/*BEGINPUBLIC*/

//{{{ class MapPair
//{{{ docu
//
// class MapPair - stores one mapping pair (Variable -> CanonicalForm).
//
// This class is only used to store such pairs.  It has no
// methods to transform a CanonicalForm as the class CFMap has.
//
// V, S: the pair (V -> S)
//
//}}}
//{{{ inline method docu
//
// Variable var () const
// CanonicalForm subst () const
//
// var(), subst() - selectors, return V and P, resp.
//
//}}}
class MapPair
{
private:
    Variable V;
    CanonicalForm S;
public:
    MapPair ( const Variable & v, const CanonicalForm & s ) : V(v), S(s) {}
    MapPair () : V(), S(1) {}
    MapPair ( const MapPair & p ) : V(p.V), S(p.S) {}
    ~MapPair () {}
    MapPair & operator = ( const MapPair & p );
    Variable var () const { return V; }
    CanonicalForm subst () const { return S; }
#ifndef NOSTREAMIO
    void print( OSTREAM&) const;
    friend OSTREAM & operator << ( OSTREAM & s, const MapPair & p );
#endif /* NOSTREAMIO */
};
//}}}

typedef List<MapPair> MPList;
typedef ListIterator<MapPair> MPListIterator;

//{{{ class CFMap
//{{{ docu
//
// class CFMap - class to map canonical forms.
//
// Use an object of class CFMap to insert 'values' into canonical
// form.  Such a mapping is defined by a list of MapPairs (V -> S)
// describing which canonical form S to insert for variable V.
// Hereby, the substituted canonical forms are not subject to
// further substitutions.
//
// P: list of MapPairs, sorted by level in descending order
//
//}}}
class CFMap
{
private:
  MPList P;
public:
  CFMap () {}
  CFMap ( const CanonicalForm & s ) : P( MapPair( Variable(), s ) ) {}
  CFMap ( const Variable & v ) : P( MapPair( v, 1 ) ) {}
  CFMap ( const Variable & v, const CanonicalForm & s ) : P( MapPair( v, s ) ) {}
  ~CFMap () {}
  CFMap ( const CFList & L );
  CFMap ( const CFMap & m ) : P( m.P ) {}
  CFMap & operator = ( const CFMap & m );
  void newpair ( const Variable & v, const CanonicalForm & s );
  CanonicalForm operator () ( const CanonicalForm & f ) const;
#ifndef NOSTREAMIO
  friend OSTREAM & operator << ( OSTREAM & s, const CFMap & m );
#endif /* NOSTREAMIO */
};
//}}}

CanonicalForm compress ( const CanonicalForm & f, CFMap & m );
void compress ( const CFArray & a, CFMap & M, CFMap & N );
void compress ( const CanonicalForm & f, const CanonicalForm & g, CFMap & M, CFMap & N );

/*ENDPUBLIC*/

#endif /* ! INCL_CF_MAP_H */
