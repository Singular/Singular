//$Id: ideal_wrap.cc,v 1.8 2006-03-09 14:49:17 bricken Exp $
#include <boost/python.hpp>
#include "mod2.h"
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
static Ring Ideal_get_Ring(const Ideal & p){
  return p.getRing();
}


void export_ideal()
{
   boost::python::class_<Ideal>("Ideal")
     .def("__str__", Ideal_as_str)
     .def("ring",Ideal_get_Ring)
     .def(boost::python::init <>())
     .def(vector_indexing_suite<Ideal >());
   
}


