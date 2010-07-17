//$Id$
#ifndef PYTHON_SINGULAR_WRAPPER_HEADER
#define PYTHON_SINGULAR_WRAPPER_HEADER
#include <Python.h>
#include <boost/python.hpp>
#include <sstream>
#include "mod2.h"


#include "structs.h"
#include "numbers.h"

#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
//#include "util.h"
#include "Number.h"
#include "Poly.h"
#include "PowerSeries.h"
#include "Ideal.h"
#include "ideal_wrap.h"
#include <factory.h>
#include "poly_wrap.h"
#include "vector_wrap.h"
#include "CF_wrap.h"
#include "number_wrap.h"
#include "playground.h"
#include "interpreter_support.h"
#include "ring_wrap.h"
#include "intvec_wrap.h"
using boost::python::numeric::array;
using boost::python::extract;

using namespace boost::python;

Vector unitVector0(int i){
  return unitVector(i,currRing);
}







//typedef void * idhdl;
void different_ring_translator(DifferentDomainException const& x) {
    PrintS("hoho");
    PyErr_SetString(PyExc_UserWarning, "Objects didn't have the same ring");
}
BOOST_PYTHON_MODULE(Singular){
  //Print("ref count after add: %d", currRing->ref);
  register_exception_translator<
          DifferentDomainException>(different_ring_translator);
  export_poly();

  

  export_number();
  export_vector();
  export_playground();
  export_ideal();
  export_module();
  //export_interpreter();
  export_ring();
  export_intvec();
  boost::python::class_<PowerSeries>("power_series")       
    .def(boost::python::init <const PowerSeries::numerator_type &,const PowerSeries::denominator_type&>())
    .def("__iter__", boost::python::iterator<PowerSeries>());
  boost::python::class_<VectorPowerSeries>("vector_power_series")
    .def(boost::python::init <const VectorPowerSeries::numerator_type&,const VectorPowerSeries::denominator_type &>())
    .def("__iter__", boost::python::iterator<VectorPowerSeries>());
  def("gen",unitVector0);


  //    .def(self+=self)
   
  //   .def(self+self)
  //.def(self*=Number())
  //.def(Number() * self);

}
BOOST_PYTHON_MODULE(factory){
  boost::python::class_<Variable>("variable")
    .def(boost::python::init <const int, char>())
    .def(boost::python::init <char>())
    
    .def(boost::python::init <const int>());
  export_CF();
}
BOOST_PYTHON_MODULE(_Singular){
export_interpreter();

}
#endif
