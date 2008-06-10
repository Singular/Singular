///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: tmpl_inst.h,v 1.4 2008-06-10 14:49:16 Singular Exp $
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

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/09/12 07:20:01  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:16:31  michael
Version for libfac-0.2.1

*/

