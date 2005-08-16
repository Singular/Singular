//$Id: wrapper.h,v 1.6 2005-08-16 12:20:55 bricken Exp $
#include "mod2.h"
#include "numbers.h"
#include "febase.h"
#include "Poly.h"
#include "Number.h"
#include <Python.h>
#include <boost/python.hpp>

using namespace boost::python;
static boost::python::object Number_as_str(Number n)
{
  using boost::python::str;
  StringSetS("");
  n.write();
  char* out=StringAppendS("");
  return boost::python::str(out,strlen(out));
}

static boost::python::object Poly_as_str(Poly p)
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
    .def("__str__", Poly_as_str)
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
    .def(self*self);
    //.def(self/self)
      //.def(self-self)
      //.def(int()==self)
      //.def(int()+self)
      //.def(int()*self)
      //.def(int()/self)
      //.def(int()-self)
      //.def(self==int())
      //.def(self+int())
    /* .def(self*int()) */
/*     .def(self/int()) */
/*     .def(self-int()) */
/*     .def(self*=int()) */
/*     .def(self+=int()) */
/*     .def(self-=int()) */
/*     .def(self/=int()); */
}
