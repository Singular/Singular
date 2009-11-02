//$Id$

#include <boost/python.hpp>
#include "mod2.h"
#include "ring_wrap.h"
#include "poly_wrap.h"
#include "febase.h"
static boost::python::object Ring_as_str(const Ring& r)
{
  using boost::python::str;
  StringSetS("");
  rWrite(r.pimpl.get());
  char* out=StringAppendS("");
  return boost::python::str(out,strlen(out));
}
void ring_set(Ring & r){
      //FIXME: only a hack, no solution
      char name_buffer[100];
      static int ending=0;
      ending++;
      sprintf(name_buffer, "PYTHON_RING_VAR%d",ending);
      idhdl shadow_hdl=enterid(name_buffer,0,RING_CMD,&IDROOT);
      r.pimpl.get()->ref++;
      shadow_hdl->data.uring=r.pimpl.get();
      rChangeCurrRing(r.pimpl.get());
      currRingHdl=shadow_hdl;
      
    }
void export_ring(){
boost::python::class_<Ring>("Ring", "reference to a Singular ring")
     .def("__str__", Ring_as_str)
     .def("set", ring_set,"equivalent to the singular command setring, which is not mapped as it is a command")
     .def(boost::python::init <>());

}
