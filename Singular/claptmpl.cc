// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
// $Id: claptmpl.cc,v 1.36 2007-02-08 10:40:35 bricken Exp $
/*
* ABSTRACT - instantiation of all templates
*/

#include "mod2.h"
//#include <vector>
//using namespace std;
#ifdef HAVE_FACTORY
  #define SINGULAR 1
  #include <factory.h>
#endif

#if defined(HAVE_FACTORY) || defined(HAVE_FGLM)
  #include <templates/ftmpl_list.cc>
  #ifdef HAVE_FGLM
    #include "fglm.h"
  #endif

  #ifdef HAVE_FACTORY
    #include <templates/ftmpl_array.cc>
    #include <templates/ftmpl_factor.cc>
    #include <templates/ftmpl_functions.h>
    #include <templates/ftmpl_matrix.cc>

    template class Factor<CanonicalForm>;
    template class List<CFFactor>;
    template class ListItem<CFFactor>;
    template class ListItem<CanonicalForm>;
    template class ListIterator<CFFactor>;
    template class List<CanonicalForm>;
    template class List<List<CanonicalForm> >;
    template class ListIterator<CanonicalForm>;
    template class Array<CanonicalForm>;
    template class List<MapPair>;
    template class ListItem<MapPair>;
    template class ListIterator<MapPair>;
    template class Matrix<CanonicalForm>;
    template class SubMatrix<CanonicalForm>;
    template class Array<REvaluation>;
    //template class vector<poly>;
    #ifndef NOSTREAMIO
    template ostream & operator<<(ostream &, const List<Factor<CanonicalForm> > &);
    template ostream & operator<<(ostream &, const List<List<CanonicalForm> > &);
    template ostream & operator<<(ostream &, const List<Variable> &);
    template ostream & operator<<(ostream &, const Matrix<Variable> &);
    #endif

    template List<CFFactor> Union ( const List<CFFactor>&, const List<CFFactor>& );
    template int operator == ( const Factor<CanonicalForm>&, const Factor<CanonicalForm>& );

    template CanonicalForm tmax ( const CanonicalForm&, const CanonicalForm& );
    template CanonicalForm tmin ( const CanonicalForm&, const CanonicalForm& );

    template Variable tmax ( const Variable&, const Variable& );
    template Variable tmin ( const Variable&, const Variable& );

    template int tmax ( const int&, const int& );
    template int tmin ( const int&, const int& );
    template int tabs ( const int& );

  #endif
// place here your own template stuff, not instantiated by factory
  #ifdef HAVE_LIBFAC_P
    #include <factor.h>

// class.h:
template <class T>
class Substitution {
private:
    T _factor;
    T _exp;
public:
    Substitution() : _factor(1), _exp(0) {}
    Substitution( const Substitution<T> & f ) : _factor(f._factor), _exp(f._exp) {}
    Substitution( const T & f, const T & e ) : _factor(f), _exp(e) {}
    Substitution( const T & f ) : _factor(f), _exp(1) {}
    ~Substitution() {}
    Substitution<T>& operator= ( const Substitution<T>& );
    Substitution<T>& operator= ( const T& );
    T factor() const { return _factor; }
    T exp() const { return _exp; }
#ifndef NOSTREAMIO
    //friend ostream & operator <<<>(ostream &, Substitution<T> &);
    void print ( ostream& s ) const
    {
      s << "("  << factor() << ")^" << exp();
    }
#endif
};
template <class T>
int operator== ( const Substitution<T>&, const Substitution<T>& );

// class.cc
template <class T>
Substitution<T>& Substitution<T>::operator= ( const Substitution<T>& f )
{
    if ( this != &f ) {
        _factor = f._factor;
        _exp = f._exp;
    }
    return *this;
}

template <class T>
Substitution<T>& Substitution<T>::operator= ( const T & f )
{
    _factor = f;
    _exp = 1;
    return *this;
}

#ifndef NOSTREAMIO
template <class T>
ostream & operator <<(ostream & os, Substitution<T> &a)
{
  a.print(os);
  return os;
}
template ostream & operator <<(ostream &, Substitution<CanonicalForm> &);
template ostream & operator <<(ostream &, const List<CanonicalForm> &);
template ostream & operator <<(ostream &, const Array<CanonicalForm> &);
template ostream & operator<<(ostream &, const List<Substitution<CanonicalForm> > &);
#endif

template <class T>
int operator== ( const Substitution<T> &f1, const Substitution<T> &f2 )
{
    return (f1.exp() == f2.exp()) && (f1.factor() == f2.factor());
}

    template class List<int>;
    template class ListIterator<int>;

    template class List<IntList>;
    template class ListIterator<IntList>;

    template class Substitution<CanonicalForm>;
    template class Array<Variable>;
    template class Array<int>;
    typedef Substitution<CanonicalForm> SForm ;
    template class List<SForm>;
    template class ListIterator<SForm>;
    template class List<Variable>;
    template class ListIterator<Variable> ;

    template List<Variable> Union ( const List<Variable>&, const List<Variable>& );
    template List<Variable> Difference ( const List<Variable>&, const List<Variable>& );

    template List<CanonicalForm> Union ( const List<CanonicalForm>&, const List<CanonicalForm>& );
    template List<CanonicalForm> Difference ( const List<CanonicalForm>&, const List<CanonicalForm>& );

// for charsets:
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

// ----------------------------------------------------------------------------
//  kmatrix.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#ifdef HAVE_SPECTRUM

#ifdef   KMATRIX_PRINT
#include <iostream.h>
#ifndef   KMATRIX_IOSTREAM
#include <stdio.h>
#endif
#endif

#include "GMPrat.h"
#include "kmatrix.h"

template    class   KMatrix<Rational>;

#ifdef   KMATRIX_PRINT
template    ostream &   operator << ( ostream&,const KMatrix<Rational>& );
template    static  void    print_rational( ostream&,int,const Rational& );
#endif

#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  kmatrix.cc
//  end of file
// ----------------------------------------------------------------------------
#include "tgb_internal.h"
#ifdef HAVE_BOOST
#include <boost/dynamic_bitset.hpp>
#include <vector>
using std::vector;
using boost::dynamic_bitset;
template class dynamic_bitset<>;
template class vector<dynamic_bitset<> >;
#elif defined(USE_STDVECBOOL)
#include <vector>
using std::vector;
template class vector<bool>;
template class vector<vector<bool> >;
#endif

template int pos_helper(kStrategy strat, poly p, int len, intset  setL, polyset set);
template int pos_helper(kStrategy strat, poly p, wlen_type len, wlen_set setL, polyset set);
#ifdef NORO_CACHE
//template class std::map<PolySimple,std::pair<PolySimple,int> >;
template class std::vector<PolySimple>;
#endif
//template int pos_helper(kStrategy strat, poly p, int len, intset setL, polyset set);
