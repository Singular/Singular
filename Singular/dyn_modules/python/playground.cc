#include "kernel/mod2.h"
#include <boost/python.hpp>
#include "Poly.h"
#include "boost/python/numeric.hpp"
using boost::python::numeric::array;
using boost::python::self;
using boost::python::make_tuple;
using boost::python::tuple;
using boost::python::object;
using boost::python::list;
object foo()
{
  list l;
  for(int j=0;j<2;j++)
  {
    list row;
    for(int i=0;i<10;i++)
    {
      Poly ip(i*(j+1),currRing);
      row.append(ip);
      //a[boost::python::make_tuple(i%2,i%5)]=ip;
      //a[boost::python::make_tuple(i%2,i%5)]=ip;
    }
    l.append(row);
  }
  boost::python::numeric::array::set_module_and_type("Numeric",
                             "ArrayType"
                             );
  boost::python::numeric::array a(l);
  return a;
}
object foo2(array f)
{
  using boost::python::extract;
  object o=f.attr("shape");

  object o1=o[0];

  object o2=o[1];
  int l1=extract<int>(o1);

  Print("%d",l1);
  int l2=extract<int>(o2);
  Print("%d",l2);
  Poly& x = boost::python::extract<Poly&>(f[boost::python::make_tuple(0,0)]);
  x.print();

  return boost::python::str("suc");
}
void export_playground()
{
  def("foo",foo);
  def("foo2",foo2);
}
