#include <poly_wrap.h>
#include <boost/python.hpp>
#include "mod2.h"
#include "Poly.h"
#include "Ideal.h"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using boost::python::self;
using namespace boost::python;
static boost::python::object Poly_as_str(Poly& p)
{
  using boost::python::str;
  //ring r=p.getRing();
 
  char* out=p.c_string();
  return boost::python::str(out,strlen(out));
}
void export_ideal()
{
   boost::python::class_<Ideal>("ideal")
    .def(boost::python::init <>())
     .def(vector_indexing_suite<Ideal >());
}


