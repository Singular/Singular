#include "kernel/mod2.h"
#ifdef HAVE_PYTHON
#include <boost/python.hpp>
#include "Number.h"
#include "number_wrap.h"
#include "ring_wrap.h"
using boost::python::self;
static boost::python::object Number_as_str(const Number& n)
{
  using boost::python::str;
  StringSetS("");
  n.write();
  char* out=StringEndS();
  return boost::python::str(out,strlen(out));
}
static Ring Number_get_Ring(const Number & n){
  return n.getRing();
}
void export_number()
{
   boost::python::class_<Number>("Number")
    .def(boost::python::init <int>("creates a number in currRing by default, more complex constructs can be fetched from the interpreter"))
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
    .def(self/=int())
    .def("ring",Number_get_Ring);
}
#endif
