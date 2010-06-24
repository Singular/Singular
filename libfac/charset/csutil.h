////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
////////////////////////////////////////////////////////////

#ifndef INCL_CSUTIL_H
#define INCL_CSUTIL_H

#include <factory.h>
#include "algfactor.h"
#include <tmpl_inst.h>

// inline function's:

inline int 
cls ( const CanonicalForm & f ){ if ( getNumVars(f) == 0 ) {return 0;} else {return f.level();} }

inline int 
rank ( const CanonicalForm & f ){ return cls(f); }

inline CanonicalForm 
ini ( const CanonicalForm & f ){ return LC(f); }

inline CanonicalForm 
ini ( const CanonicalForm & f , const Variable v ){ return LC(f,v); }

inline Variable 
lvar ( const CanonicalForm & f ){ return f.mvar(); }

///////////////////////////////////////////////////////////////
// a class definition needed in charset.cc for Prem
///////////////////////////////////////////////////////////////
/*BEGINPUBLIC*/

class PremForm {
public:
  CFList FS1;
  CFList FS2;
  inline PremForm& operator=( const PremForm&  value ){
    if ( this != &value ){
      FS1 = value.FS1;
      FS2 = value.FS2;
    }
    return *this;
  }
};

/*ENDPUBLIC*/

// functions from csutil.cc:
/*BEGINPUBLIC*/
CanonicalForm  Prem( const CanonicalForm &f, const CanonicalForm &g );
CanonicalForm  Prem( const CanonicalForm &f, const CFList &L );
CFList         Prem( const CFList &AS, const CFList &L );
CanonicalForm alg_gcd(const CanonicalForm &, const CanonicalForm &, const CFList &);
/*ENDPUBLIC*/
CanonicalForm  divide( const CanonicalForm & ff, const CanonicalForm & f, const CFList & as);
CFList         remsetb( const CFList & ps, const CFList & as);
CanonicalForm  lowestRank( const CFList & F );

CFList         removecontent ( const CFList & PS, PremForm & Remembern );
void           removefactor( CanonicalForm & r , PremForm & Remembern);
CFList         factorps( const CFList &ps );
//CFList         initalset(const CFList & CSet);
CFList         initalset1(const CFList & CSet);
CFList         initalset2(const CFList & CSet, const CanonicalForm & reducible);
int            irreducible( const CFList & ASet);
CFList         select( const ListCFList & PSet);
void           select( const ListCFList & ppi, int length, ListCFList & ppi1, 
		       ListCFList & ppi2);
bool           same( const CFList &A, const CFList &B );
bool           member( const CFList & cs, const ListCFList & pi );
bool           subset( const CFList &PSet, const CFList &CSet );
ListCFList     MyUnion( const ListCFList & a, const ListCFList &b );
ListCFList     MyDifference( const ListCFList & a, const CFList &b);
ListCFList     Minus( const ListCFList & a, const ListCFList &b);
#endif /* INCL_CSUTIL_H */

