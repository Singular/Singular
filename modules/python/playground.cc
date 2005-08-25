#include <boost/python.hpp>
#include "mod2.h"
#include "Poly.h"
#include "boost/python/numeric.hpp"
using boost::python::numeric::array;
using boost::python::self;
using boost::python::make_tuple;
using boost::python::tuple;
using boost::python::object;
using boost::python::list;
object foo(){
  list l;
  for(int j=0;j<2;j++){
    list row;
    for(int i=0;i<10;i++){
      Poly ip(i*(j+1),currRing);
      row.append(ip);
      //a[boost::python::make_tuple(i%2,i%5)]=ip;
      //a[boost::python::make_tuple(i%2,i%5)]=ip;
    }
    l.append(row);
  }
  array::set_module_and_type("Numeric",
			     "ArrayType"
			     );
  array a(l);
  return a;
  


 
}

void export_playground(){
  def("foo",foo);
}
