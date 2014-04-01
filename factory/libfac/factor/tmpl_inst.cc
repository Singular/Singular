////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////


#include "config.h"


// #include <factory/factoryconf.h>

// #include "factory.h"
#include "cf_iter.h"
#include "canonicalform.h"
#include "cf_assert.h"
#include "cf_reval.h"

#include "templates/ftmpl_array.cc"
//#include "templates/ftmpl_factor.cc"
#include "templates/ftmpl_list.cc"
//#include "templates/ftmpl_functions.h"
//#include "templates/ftmpl_matrix.cc"

// place here your own template stuff, not instantiated by factory
#include <libfac/factor/tmpl_inst.h>

//template class List<IntList>;
//template class ListIterator<IntList>;

#ifndef NOSTREAMIO
template OSTREAM & operator << ( OSTREAM &, const List<int> & );
template OSTREAM & operator << ( OSTREAM &, const List<IntList> & );
#endif

// for charsets:
//template class List<CFList>;
//template class ListIterator<CFList>;

#ifndef NOSTREAMIO
template OSTREAM & operator << ( OSTREAM &, const List<CFList> & );
template OSTREAM & operator << ( OSTREAM &, const List<Variable> & );
template OSTREAM & operator << ( OSTREAM &, const Array<int> & );
#endif

//template class Array<int>;
//template class Array<Variable>;

// for database
#ifdef HAVE_DATABASE
template class List<CFFList>;
template class ListIterator<CFFList>;
#endif /* HAVE_DATABASE */
