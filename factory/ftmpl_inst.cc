/* emacs edit mode for this file is -*- C++ -*- */

//{{{ docu
//
// ftmpl_inst.cc - Factory's template instantiations.
//
// For a detailed description how to instantiate Factory's
// template classes and functions and how to add new
// instantiations see the `README' file.
//
//}}}

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

// #include <factory/factoryconf.h>
// #include "factory.h"
#include "cf_iter.h"
#include "canonicalform.h"
#include "cf_assert.h"
#include "cf_reval.h"
#include "cf_map.h"


#include "templates/ftmpl_array.cc"
#include "templates/ftmpl_afactor.cc"
#include "templates/ftmpl_factor.cc"
#include "templates/ftmpl_list.cc"
#include "templates/ftmpl_functions.h"
#include "templates/ftmpl_matrix.cc"


//{{{ explicit template class instantiations
template class Factor<CanonicalForm>;
template class List<CFFactor>;
template class ListItem<CFFactor>;
template class ListIterator<CFFactor>;
template class AFactor<CanonicalForm>;
template class List<CFAFactor>;
template class ListItem<CFAFactor>;
template class ListIterator<CFAFactor>;
template class List<CanonicalForm>;
template class ListItem<CanonicalForm>;
template class ListIterator<CanonicalForm>;
template class Array<CanonicalForm>;
template class List<MapPair>;
template class ListItem<MapPair>;
template class ListIterator<MapPair>;
template class Matrix<CanonicalForm>;
template class SubMatrix<CanonicalForm>;
template class Array<REvaluation>;
// libfac:
template class Array<int>;
template class Array<Variable>;
template class List<Variable>;
template class ListItem<Variable>;
template class ListIterator<Variable> ;
template class List<int>;
template class ListItem<int>;
template class ListIterator<int>;
template class List< List<int> >;
template class ListItem< List<int> >;
template class ListIterator< List<int> >;
template class List< List<CanonicalForm> >;
template class ListItem< List<CanonicalForm> >;
template class ListIterator< List<CanonicalForm> >;

template List<Variable> Union ( const List<Variable>&, const List<Variable>& );
template List<Variable> Difference ( const List<Variable>&, const List<Variable>& );

//}}}

//{{{ explicit template function instantiations
#ifndef NOSTREAMIO
template OSTREAM & operator << ( OSTREAM &, const List<CanonicalForm> & );
template OSTREAM & operator << ( OSTREAM &, const List<CFFactor> & );
template OSTREAM & operator << ( OSTREAM &, const List<MapPair> & );
template OSTREAM & operator << ( OSTREAM &, const Array<CanonicalForm> & );
template OSTREAM & operator << ( OSTREAM &, const Factor<CanonicalForm> & );
template OSTREAM & operator << ( OSTREAM &, const Matrix<CanonicalForm> & );
template OSTREAM & operator << ( OSTREAM &, const Array<REvaluation> & );
#endif /* NOSTREAMIO */

template int operator == ( const Factor<CanonicalForm> &, const Factor<CanonicalForm> & );
template int operator == ( const AFactor<CanonicalForm> &, const AFactor<CanonicalForm> & );

template List<CFFactor> Union ( const List<CFFactor> &, const List<CFFactor> & );
template List<CFAFactor> Union ( const List<CFAFactor> &, const List<CFAFactor> & );

#if ! defined(WINNT) || defined(__GNUC__)
template CanonicalForm tmax ( const CanonicalForm &, const CanonicalForm & );
template CanonicalForm tmin ( const CanonicalForm &, const CanonicalForm & );

template Variable tmax ( const Variable &, const Variable & );
template Variable tmin ( const Variable &, const Variable & );

template int tmax ( const int &, const int & );
template int tmin ( const int &, const int & );
template int tabs ( const int & );
#endif
//}}}

//
template List<CanonicalForm> Union ( const List<CanonicalForm> &, const List<CanonicalForm> & );
template List<CanonicalForm> Difference ( const List<CanonicalForm> &, const List<CanonicalForm> & );
template CanonicalForm prod ( const List<CanonicalForm> & );
template bool find ( const List<CanonicalForm> &, const CanonicalForm&);
// place here your own template stuff, not yet instantiated by factory
//
