#include "kernel/mod2.h"
#ifdef HAVE_PYTHON
#include <boost/python.hpp>
#include "Singular/ipshell.h"
#include "ring_wrap.h"
#include "poly_wrap.h"
static boost::python::object Ring_as_str(const Ring& r)
{
  using boost::python::str;
  StringSetS("");
  rWrite(r.pimpl.get());
  char* out=StringEndS();
  return boost::python::str(out,strlen(out));
}
void ring_set(Ring & R)
{
    ring r=R.pimpl.get();
    idhdl h=rFindHdl(r,NULL);
    if (h==NULL)
    {
      char name_buffer[100];
      STATIC_VAR int ending=0;
      ending++;
      sprintf(name_buffer, "PYTHON_RING_VAR%d",ending);
      h=enterid(name_buffer,0,RING_CMD,&IDROOT);
      IDRING(h)=r;
      r->ref++;
    }
    rSetHdl(h);
    for(int i=myynest;i>=0;i--) iiLocalRing[i]=r;
}
void export_ring()
{
boost::python::class_<Ring>("Ring", "reference to a Singular ring")
     .def("__str__", Ring_as_str)
     .def("set", ring_set,"equivalent to the singular command setring, which is not mapped as it is a command")
     .def(boost::python::init <>());
}
#endif
