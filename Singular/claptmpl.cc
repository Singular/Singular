// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: claptmpl.cc,v 1.7 1997-05-20 08:52:02 Singular Exp $
/*
* ABSTRACT - instantiation of all templates
*/

#include "mod2.h"

#ifdef HAVE_FACTORY
#define SINGULAR 1
#include <factory.h>
#endif

#if defined(HAVE_FACTORY) || defined(HAVE_FGLM)
#include <templates/list.cc>
#endif

#ifdef HAVE_FGLM
#include "fglm.h"
#endif

#ifdef HAVE_FACTORY
#include <templates/array.cc>
#include <templates/factor.cc>
#include <templates/functions.h>
#include <templates/matrix.cc>

template class Factor<CanonicalForm>;
template class List<CFFactor>;
template class ListItem<CFFactor>;
template class ListItem<CanonicalForm>;
template class ListIterator<CFFactor>;
template class List<CanonicalForm>;
template class ListIterator<CanonicalForm>;
template class Array<CanonicalForm>;
template class List<MapPair>;
template class ListItem<MapPair>;
template class ListIterator<MapPair>;
template class Matrix<CanonicalForm>;
template class SubMatrix<CanonicalForm>;

template List<CFFactor> Union ( const List<CFFactor>&, const List<CFFactor>& );

template CanonicalForm tmax ( const CanonicalForm&, const CanonicalForm& );
template CanonicalForm tmin ( const CanonicalForm&, const CanonicalForm& );

template int tmax ( const int&, const int& );
template int tmin ( const int&, const int& );

// place here your own template stuff, not instantiated by factory

#ifdef HAVE_LIBFAC_P
#include <templates/tmpl_inst.h>
#include <templates/class.cc>

template class List<int>;
template class ListIterator<int>;

template class List<IntList>;
template class ListIterator<IntList>;

template class Substitution<CanonicalForm>;
template class Array<Variable>;
template class Array<int>;
template class List<SForm>;
template class ListIterator<SForm>;
template class List<Variable>;
template class ListIterator<Variable> ;

template List<Variable> Union ( const List<Variable>&, const List<Variable>& );
template List<Variable> Difference ( const List<Variable>&, const List<Variable>& );

template List<CanonicalForm> Union ( const List<CanonicalForm>&, const List<CanonicalForm>& );
template List<CanonicalForm> Difference ( const List<CanonicalForm>&, const List<CanonicalForm>& );

// for charsets:
template class List<CFList>;
template class ListIterator<CFList>;

#endif
#endif

#ifdef HAVE_FGLM
// templates for fglm:

template class List<fglmSelem>;
template class ListIterator<fglmSelem>;

template class List<fglmDelem>;
template class ListIterator<fglmDelem>;

#endif
