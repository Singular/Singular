// Header file for libfac.a
// Created by -- makeheader --
// $Id$
///////////////////////////////////////
#ifndef FACTOR_H
#define FACTOR_H

#include <factory/factory.h>

// Set this to a nonzero value to interrupt the computation
extern int libfac_interruptflag;

// some values you can ask for:
extern const char * libfac_name;
extern const char * libfac_version;
extern const char * libfac_date;
extern const char * libfac_author;

//// Note: second argument for Factorize is for internal use only.
CFFList Factorize( const CanonicalForm & F, int is_SqrFree=0 ) ;
CFFList Factorize( const CanonicalForm & F, const CanonicalForm & mipo, int is_SqrFree=0 ) ;
CFFList Factorize2(CanonicalForm F, const CanonicalForm & minpoly );
CFFList newfactoras( const CanonicalForm & f, const CFList & as, int &success);

//// Note: InternalSqrFree does only the work needed for factorization.
// CFFList SqrFree( const CanonicalForm & f ) ;

typedef List<CFList> ListCFList;
typedef List<int> IntList ;
typedef ListIterator<int> IntListIterator ;
typedef List<Variable> Varlist;
typedef ListIterator<Variable> VarlistIterator;

////////////////////////////////////////
/// from charsets:

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

CanonicalForm  Prem( const CanonicalForm &f, const CanonicalForm &g );
CanonicalForm  Prem( const CanonicalForm &f, const CFList &L );
CFList         Prem( const CFList &AS, const CFList &L );
CFList     MCharSetN( const CFList &PS, PremForm & Remembern );
ListCFList IrrCharSeries( const CFList &PS, int opt=0 );

// the next three give you a heuristically optimal reorderd list of the
// variables. For internal and external (e.g. Singular/Macaulay2) library use.
// This is really experimental!
// See the comments in reorder.cc.
// 
// this gives you a heuristically optimal ordering for the ring variables
// if you use the irreducible characteristic series.
Varlist  neworder( const CFList & PolyList );
// the same as neworder(...) only returning a list of CanonicalForm 's
// (i.e. the variables as CanonicalForms)
CFList   newordercf(const CFList & PolyList );
// the same as neworder(...) only returning a list of int 's (i.e. the levels)
IntList  neworderint(const CFList & PolyList );

// for library internal use only:
// next function reorders the variables in PS:
// a code segment to use:
// ...
// #include <tmpl_inst.h> // for typedef's
// CFList PS= <setup-your-list-of-CanonicalForms>;
// Varlist betterorder= neworder(PS);
// PS= reorder(betterorder,PS); // reorder variables in PS from oldorder 
//                                 to betterorder
// ListCFList Q= IrrCharSeries( PS );
// Q= reorder(betterorder,Q);   // revert ordering to oldorder
// 
CFList reorder( const Varlist & betterorder, const CFList & PS);
CFFList reorder( const Varlist & betterorder, const CFFList & PS);
ListCFList reorder(const Varlist & betterorder, const ListCFList & Q);


#endif /* FACTOR_H */
