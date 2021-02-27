#include "kernel/mod2.h"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "IIntvec.h"
#include "intvec_wrap.h"


using namespace boost::python;
static boost::python::object intvec_as_str(const Intvec& p)
{
  using boost::python::str;
  //ring r=p.getRing();
  str helper;
  list tojoin;
  int i;
  int s=p.size();
  tojoin.append("[");
  for(i=0;i<s;i++){
    tojoin.append(str(p[i]));
    if (i<s-1)
      tojoin.append(", ");
  }
  tojoin.append("]");
  str res=helper.join(tojoin);
  return res;

}

void export_intvec(){
boost::python::class_<Intvec>("IntVector")
      .def("__str__", intvec_as_str)
     .def(boost::python::init <>())
     .def(boost::python::vector_indexing_suite<Intvec>());
}
