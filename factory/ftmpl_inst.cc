/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_inst.cc,v 1.2 1997-06-20 10:45:18 schmidt Exp $ */

#include <factoryconf.h>

#include <templates/ftmpl_array.cc>
#include <templates/ftmpl_factor.cc>
#include <templates/ftmpl_list.cc>
#include <templates/ftmpl_functions.h>
#include <templates/ftmpl_matrix.cc>

#include <factory.h>

template class Factor<CanonicalForm>;
template class List<CFFactor>;
template class ListIterator<CFFactor>;
template class List<CanonicalForm>;
template class ListIterator<CanonicalForm>;
template class Array<CanonicalForm>;
template class List<MapPair>;
template class ListIterator<MapPair>;
template class Matrix<CanonicalForm>;
template class SubMatrix<CanonicalForm>;
template class Array<REvaluation>;

#ifndef NOSTREAMIO
template ostream & operator << ( ostream &, const List<CanonicalForm> & );
template ostream & operator << ( ostream &, const List<CFFactor> & );
template ostream & operator << ( ostream &, const List<MapPair> & );
template ostream & operator << ( ostream &, const Array<CanonicalForm> & );
template ostream & operator << ( ostream &, const Factor<CanonicalForm> & );
template ostream & operator << ( ostream &, const Matrix<CanonicalForm> & );
template ostream & operator << ( ostream &, const Array<REvaluation> & );
#endif /* NOSTREAMIO */

template List<CFFactor> Union ( const List<CFFactor>&, const List<CFFactor>& );

/*
template CanonicalForm crossprod ( const Array<CanonicalForm>&, const Array<CanonicalForm>& );
template CanonicalForm prod ( const Array<CanonicalForm>& );
*/

template CanonicalForm tmax ( const CanonicalForm&, const CanonicalForm& );
template CanonicalForm tmin ( const CanonicalForm&, const CanonicalForm& );

template int tmax ( const int&, const int& );
template int tmin ( const int&, const int& );

// place here your own template stuff, not instantiated by factory
