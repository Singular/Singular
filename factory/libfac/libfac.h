// Header file for libfac.a
// Created by -- makeheader --
///////////////////////////////////////
#ifndef FACTOR_H
#define FACTOR_H

#include <factory/factory.h>

// Set this to a nonzero value to interrupt the computation
extern "C" int libfac_interruptflag;

// some values you can ask for:
extern const char * libfac_name;
extern const char * libfac_version;
extern const char * libfac_date;
extern const char * libfac_author;

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
CanonicalForm alg_gcd(const CanonicalForm &, const CanonicalForm &, const CFList &);
CFList     MCharSetN( const CFList &PS, PremForm & Remembern );
ListCFList IrrCharSeries( const CFList &PS, int opt=0 );

int hasVar(const CanonicalForm &f, const Variable &v);

#endif /* FACTOR_H */
