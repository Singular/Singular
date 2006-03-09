//$Id: ring_wrap.cc,v 1.6 2006-03-09 14:49:17 bricken Exp $

#include <boost/python.hpp>
#include "mod2.h"
#include "ring_wrap.h"
#include "poly_wrap.h"
#include "febase.h"
static boost::python::object Ring_as_str(const Ring& r)
{
  using boost::python::str;
  StringSetS("");
  rWrite(r.pimpl);
  char* out=StringAppendS("");
  return boost::python::str(out,strlen(out));
}
void export_ring(){
boost::python::class_<Ring>("Ring")
     .def("__str__", Ring_as_str)
     .def("set", &Ring::set)
     .def(boost::python::init <>());

}
