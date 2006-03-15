//$Id: ring_wrap.cc,v 1.7 2006-03-15 10:53:58 bricken Exp $

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
boost::python::class_<Ring>("Ring", "reference to a Singular ring")
     .def("__str__", Ring_as_str)
     .def("set", &Ring::set,"equivalent to the singular command setring, which is not mapped as it is a command")
     .def(boost::python::init <>());

}
