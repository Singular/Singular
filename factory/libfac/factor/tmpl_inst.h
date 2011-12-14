///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
///////////////////////////////////////////////////////////////////////////////
#ifndef TMPL_INST_H
#define TMPL_INST_H
#include "class.h"
//typedef CanonicalForm CF;
//typedef List<CanonicalForm> CFList;
//typedef ListIterator<CanonicalForm> CFListIterator;
/*BEGINPUBLIC*/
typedef List<CFList> ListCFList;
/*ENDPUBLIC*/
typedef ListIterator<CFList> ListCFListIterator ;

//typedef Factor<CanonicalForm> CFFactor;
//typedef List<CFFactor> CFFList;
//typedef ListIterator<CFFactor> CFFListIterator;

/*BEGINPUBLIC*/
typedef List<int> IntList ;
typedef ListIterator<int> IntListIterator ;
/*ENDPUBLIC*/
typedef List<IntList> ListIntList;
typedef ListIterator<IntList> ListIntListIterator ;

typedef Substitution<CanonicalForm> SForm ;
typedef List<SForm> SFormList;
typedef ListIterator<SForm> SFormListIterator;

/*BEGINPUBLIC*/
typedef List<Variable> Varlist;
typedef ListIterator<Variable> VarlistIterator;
/*ENDPUBLIC*/

typedef Array<int> Intarray;

#ifdef HAVE_DATABASE
typedef List<CFFList> ListCFFList;
typedef ListIterator<CFFList> ListCFFListIterator;
#endif

//typedef Array<CFArray> CCFArray ;
//typedef Array<CCFArray> CCCFArray ;

#endif /* TMPL_INST_H */


