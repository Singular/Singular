#include "kernel/mod2.h"
#ifdef HAVE_PYTON
#include <boost/python.hpp>
#include "Poly.h"
#include "vector_wrap.h"
#include "ring_wrap.h"
using boost::python::self;

static boost::python::object Vector_as_str(Vector& p)
{
  using boost::python::str;
  //ring r=p.getRing();

  char* out=p.c_string();
  return boost::python::str(out,strlen(out));
}
static Ring Vector_get_Ring(const Vector & p){
  return p.getRing();
}
void export_vector(){
  boost::python::class_<Vector>("Vector")
    .def(boost::python::init <>())
    .def("__str__", Vector_as_str)

    //    .def("__str__", Poly_as_str)
    .def("__iter__", boost::python::iterator<Vector>())
    .def(-self)
    .def(self+=self)

    .def(self+self)
    .def(self*=Number())
    .def(Poly() * self)
    .def(Number() * self)
    .def("ring",Vector_get_Ring);
}
#endif
