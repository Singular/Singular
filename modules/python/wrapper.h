//$Id: wrapper.h,v 1.8 2005-08-17 13:35:29 bricken Exp $
#ifndef PYTHON_SINGULAR_WRAPPER_HEADER
#define PYTHON_SINGULAR_WRAPPER_HEADER
#include <Python.h>
#include <boost/python.hpp>
#include "mod2.h"
#include "numbers.h"
#include "febase.h"

#include "Number.h"
#include "Poly.h"
#include "PowerSeries.h"
using namespace boost::python;

static boost::python::object Number_as_str(Number n)
{
  using boost::python::str;
  StringSetS("");
  n.write();
  char* out=StringAppendS("");
  return boost::python::str(out,strlen(out));
}

static boost::python::object Poly_as_str(Poly& p)
{
  using boost::python::str;
  ring r=p.getRing();
 
  char* out=p.c_string();
  return boost::python::str(out,strlen(out));
}


BOOST_PYTHON_MODULE(Singular){
  boost::python::class_<Number>("number")
    .def(boost::python::init <int>())
    .def("__str__", Number_as_str)
    .def(-self)
    .def(self*=self)
    .def(self+=self)
    .def(self-=self)
    .def(self/=self)
    .def(self==self)
    .def(self+self)
    .def(self*self)
    .def(self/self)
    .def(self-self)
    .def(int()==self)
    .def(int()+self)
    .def(int()*self)
    .def(int()/self)
    .def(int()-self)
    .def(self==int())
    .def(self+int())
    .def(self*int())
    .def(self/int())
    .def(self-int())
    .def(self*=int())
    .def(self+=int())
    .def(self-=int())
    .def(self/=int());
  boost::python::class_<Poly>("polynomial")
    .def(boost::python::init <int>())
    .def(boost::python::init <Poly>())
    .def(boost::python::init <std::vector<int> >())
    .def(boost::python::init <Number>())
    .def("__str__", Poly_as_str)
    .def("__iter__", boost::python::iterator<Poly>())
    //read monomials (only) from string
    .def(boost::python::init <const char* >())
    
    //.def("__str__", as_str)
    //.def(-self)
    .def(self*=self)
    .def(self+=self)
    //    .def(self-=self)
    //.def(self/=self)
    //.def(self==self)
    .def(self+self)
    .def(self*=Number())
    .def(self*Number())
    .def(self+Number())
    .def(self+=Number())
    .def(self*=Number())
    .def(self*self);
  boost::python::class_<Vector>("vector")
    
    .def(boost::python::init <>())
    
    
    //    .def("__str__", Poly_as_str)
    .def("__iter__", boost::python::iterator<Vector>())
   
    .def(self+=self)
   
    .def(self+self)
    .def(self*=Number())
    .def(Number() * self);
  boost::python::class_<PowerSeries>("power_series")
       
    .def(boost::python::init <const Poly&,const Poly&>())
       
    
 
    .def("__iter__", boost::python::iterator<PowerSeries>());
     
  //    .def(self+=self)
   
  //   .def(self+self)
  //.def(self*=Number())
  //.def(Number() * self);

}
#endif
