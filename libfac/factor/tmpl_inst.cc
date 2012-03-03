////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: tmpl_inst.cc 14599 2012-02-17 18:00:31Z motsak $
////////////////////////////////////////////////////////////


#include <templates/ftmpl_array.cc>
#include <templates/ftmpl_factor.cc>
#include <templates/ftmpl_list.cc>
#include <templates/ftmpl_functions.h>
#include <templates/ftmpl_matrix.cc>

#include <factory.h>

// already in factory/ftmpl_inst.cc:
//template class Factor<CanonicalForm>;
//template class List<CFFactor>;
//template class ListIterator<CFFactor>;
//template class List<CanonicalForm>;
//template class ListIterator<CanonicalForm>;
//template class Array<CanonicalForm>;
//template class List<MapPair>;
//template class ListIterator<MapPair>;
//template class Matrix<CanonicalForm>;
//template class SubMatrix<CanonicalForm>;

#ifndef NOSTREAMIO
//template OSTREAM & operator << ( OSTREAM &, const List<CanonicalForm> & );
//template OSTREAM & operator << ( OSTREAM &, const List<CFFactor> & );
//template OSTREAM & operator << ( OSTREAM &, const List<MapPair> & );
//template OSTREAM & operator << ( OSTREAM &, const Array<CanonicalForm> & );
//template OSTREAM & operator << ( OSTREAM &, const Factor<CanonicalForm> & );
//template OSTREAM & operator << ( OSTREAM &, const Matrix<CanonicalForm> & );
#endif

//template List<CFFactor> Union ( const List<CFFactor>&, const List<CFFactor>& );

/*
template CanonicalForm crossprod ( const Array<CanonicalForm>&, const Array<CanonicalForm>& );
template CanonicalForm prod ( const Array<CanonicalForm>& );
*/

//template CanonicalForm tmax ( const CanonicalForm&, const CanonicalForm& );
//template CanonicalForm tmin ( const CanonicalForm&, const CanonicalForm& );

//template int tmax ( const int&, const int& );
//template int tmin ( const int&, const int& );

// place here your own template stuff, not instantiated by factory
#include "tmpl_inst.h"
#include "class.cc"

template class List<int>;
template class ListIterator<int>;

template class List<IntList>;
template class ListIterator<IntList>;

template class Substitution<CanonicalForm>;
template class List<SForm>;
template class ListIterator<SForm>;

//template List<CanonicalForm> Union ( const List<CanonicalForm>&, const List<CanonicalForm>& );
//template List<CanonicalForm> Difference ( const List<CanonicalForm>&, const List<CanonicalForm>& );

#ifndef NOSTREAMIO
template OSTREAM & operator << ( OSTREAM &, const List<int> & );
template OSTREAM & operator << ( OSTREAM &, const List<IntList> & );
#endif

// for charsets:
template class List<CFList>;
template class ListIterator<CFList>;
template class List<Variable>;
template class ListIterator<Variable> ;

template List<Variable> Union ( const List<Variable>&, const List<Variable>& );
template List<Variable> Difference ( const List<Variable>&, const List<Variable>& );

#ifndef NOSTREAMIO
template OSTREAM & operator << ( OSTREAM &, const List<CFList> & );
template OSTREAM & operator << ( OSTREAM &, const List<Variable> & );
template OSTREAM & operator << ( OSTREAM &, const Array<int> & );
#endif

template class Array<int>;
template class Array<Variable>;
//template class Array<REvaluation>;

// for database
#ifdef HAVE_DATABASE
template class List<CFFList>;
template class ListIterator<CFFList>;
#endif /* HAVE_DATABASE */
