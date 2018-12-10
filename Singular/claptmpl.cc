// emacs edit mode for this file is -*- C++ -*-
/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT - instantiation of all templates


*/




#include "kernel/mod2.h"
//using namespace std;
  #define SINGULAR 1
  #include "factory/factory.h"
  #include "factory/templates/ftmpl_list.cc"
  #include "kernel/fglm/fglm.h"

// templates for fglm:
  template class List<fglmSelem>;
  template class ListItem<fglmSelem>;
  template class ListIterator<fglmSelem>;
  template class List<fglmDelem>;
  template class ListItem<fglmDelem>;
  template class ListIterator<fglmDelem>;

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

#include "kernel/spectrum/GMPrat.h"
#include "kernel/spectrum/kmatrix.h"

template class KMatrix<Rational>;

#ifdef   KMATRIX_PRINT
template    OSTREAM &   operator << ( OSTREAM&,const KMatrix<Rational>& );
template    static  void    print_rational( OSTREAM&,int,const Rational& );
#endif

#endif /*!
!
 HAVE_SPECTRUM 

*/
// ----------------------------------------------------------------------------
//  kmatrix.cc
//  end of file
// ----------------------------------------------------------------------------

#ifdef HAVE_PLURAL

#include "polys/nc/ncSAMult.h" // for CMultiplier etc classes

template class CMultiplier<int>;
template class CMultiplier<poly>;
template class CMultiplier<CPower>;

// #include "kernel/ncSACache.h" // for CCacheHash etc classes
// template class CCacheHash<poly>;
// template class CCacheHash<int>;

#endif

#include "kernel/GBEngine/tgb_internal.h"
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
//#include <queue>
template class std::vector<PolySimple>;
//template class std::priority_queue<MonRedRes>;
//template class std::vector<NoroPlaceHolder>;
//template class std::vector<std::vector<NoroPlaceHolder> >;
//template class std::vector<DataNoroCacheNode<tgb_uint16>* >;
//template class std::vector<DataNoroCacheNode<tgb_uint8>* >;
template class std::vector<DataNoroCacheNode<tgb_uint32>* >;
//template SparseRow<tgb_uint16> * noro_red_to_non_poly_t<tgb_uint16>(poly p, int &len, NoroCache<tgb_uint16>* cache,slimgb_alg* c);
template SparseRow<tgb_uint32>* noro_red_to_non_poly_t<tgb_uint32>(poly p, int &len, NoroCache<tgb_uint32>* cache,slimgb_alg* c);
//template SparseRow<tgb_uint8>* noro_red_to_non_poly_t<tgb_uint8>(poly p, int &len, NoroCache<tgb_uint8>* cache,slimgb_alg* c);
//template void simplest_gauss_modp<tgb_uint16> (tgb_uint16* a, int nrows,int ncols);
template void simplest_gauss_modp<tgb_uint32> (tgb_uint32* a, int nrows,int ncols);
//template void simplest_gauss_modp<tgb_uint8> (tgb_uint8* a, int nrows,int ncols);
//template poly row_to_poly<tgb_uint8>(tgb_uint8* row, poly* terms, int tn, ring r);
template poly row_to_poly<tgb_uint32>(tgb_uint32* row, poly* terms, int tn, ring r);
//template poly row_to_poly<tgb_uint16>(tgb_uint16* row, poly* terms, int tn, ring r);
template void noro_step<tgb_uint8>(poly*p,int &pn,slimgb_alg* c);
template void noro_step<tgb_uint16>(poly*p,int &pn,slimgb_alg* c);
template void noro_step<tgb_uint32>(poly*p,int &pn,slimgb_alg* c);
//std::priority_queue<MonRedRes>
//
#endif

/*!
!
 next lines are templates used in new minor code 

*/
#include <list>
#include "kernel/linear_algebra/Minor.h"
#include "kernel/linear_algebra/Cache.h"

template class std::list<int>;
template class std::list<MinorKey>;
template class std::list<IntMinorValue>;
template class std::list<PolyMinorValue>;


/*!
!

template class std::_List_base<IntMinorValue, std::allocator<IntMinorValue> >;
template class std::_List_base<int, std::allocator<int> >;
template class std::_List_base<MinorKey, std::allocator<MinorKey> >;
template class std::_List_base<PolyMinorValue, std::allocator<PolyMinorValue> >;


*/

template class Cache<MinorKey, IntMinorValue>;
template class Cache<MinorKey, PolyMinorValue>;

