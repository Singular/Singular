#include <boost/python.hpp>
#include "mod2.h"
#include "Poly.h"
#include "boost/python/numeric.hpp"
using boost::python::numeric::array;
using boost::python::self;
using boost::python::make_tuple;
using boost::python::tuple;
using boost::python::object;
object foo(){
  array::set_module_and_type("Numeric",
		      "ArrayType"
		      );
  Poly n=Poly();
  array a(boost::python::make_tuple());
  a.resize(boost::python::make_tuple(2,5));
  for(int i=0;i<10;i++){
    Poly ip(i);
    //a[boost::python::make_tuple(i%2,i%5)]=ip;
    //a[boost::python::make_tuple(i%2,i%5)]=ip;
  }
  return a;
}

void export_playground(){
  def("foo",foo);
}
