#include "kernel/mod2.h"
#ifdef HAVE_PYTHON
#include <boost/python.hpp>
#include "Poly.h"
#include "Ideal.h"
#include "ring_wrap.h"

#include "poly_wrap.h"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using boost::python::self;
using namespace boost::python;

static boost::python::object Ideal_as_str(const Ideal& p)
{
  using boost::python::str;
  //ring r=p.getRing();
  str helper;
  list tojoin;
  int i;
  int s=p.size();
  //tojoin.append("[");
  for(i=0;i<s;i++){
    tojoin.append(Poly_as_str(p[i]));
    if (i<s-1)
      tojoin.append(", ");
  }
  //tojoin.append("]");
  str res=helper.join(tojoin);
  return res;
}
static boost::python::object Module_as_str(const Module& p)
{
  using boost::python::str;
  //ring r=p.getRing();
  str helper;
  list tojoin;
  int i;
  int s=p.size();
  //tojoin.append("[");
  for(i=0;i<s;i++){
    tojoin.append(Vector_as_str(p[i]));
    if (i<s-1)
      tojoin.append(", ");
  }
  //tojoin.append("]");
  str res=helper.join(tojoin);
  return res;
}

static Ring Ideal_get_Ring(const Ideal & p){
  return p.getRing();
}


void export_ideal()
{
   boost::python::class_<Ideal>("Ideal", "supports most operation a\
    python list supports with the expception, that elements must\
    be Polynomials")
     .def(init<>())
     .def(init<const Ideal&>())
     .def("__str__", Ideal_as_str)
     .def("ring",Ideal_get_Ring)
     .def(boost::python::init <>())
     .def(vector_indexing_suite<Ideal >());

}
void export_module()
{
   boost::python::class_<Module>("Module", "supports most operation a\
    python list supports with the expception, that elements must\
    be Polynomials")
     .def(init<>())
     .def(init<const Module&>())
     .def("__str__", Module_as_str)
     .def("ring",Ideal_get_Ring)
     .def(boost::python::init <>())
     .def(vector_indexing_suite<Module>());

}
#endif
