//$Id: wrapper.h,v 1.24 2005-08-26 10:39:08 bricken Exp $
#ifndef PYTHON_SINGULAR_WRAPPER_HEADER
#define PYTHON_SINGULAR_WRAPPER_HEADER
#include <Python.h>
#include <boost/python.hpp>
#include <sstream>
#include "mod2.h"
//shouldn't be necessary but it is for structs.h::idrec

#include "structs.h"
#include "numbers.h"

#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
//#include "util.h"
#include "Number.h"
#include "Poly.h"
#include "PowerSeries.h"
#include <factory.h>
#include "poly_wrap.h"
#include "vector_wrap.h"
#include "CF_wrap.h"
#include "number_wrap.h"
#include "playground.h"
#include "matpol.h"
using boost::python::numeric::array;
using boost::python::extract;
extern BOOLEAN errorreported;
extern int inerror;
using namespace boost::python;

Vector unitVector0(int i){
  return unitVector(i,currRing);
}







//typedef void * idhdl;
class idhdl_wrap{
 public:
  idhdl id;
  idhdl_wrap(idhdl id){
    this->id=id;
  }
  idhdl_wrap(){
    id=(idhdl) NULL;
  }
  bool is_zero(){
    return id==NULL;
  }
};
class arg_list{
 public:
  leftv args;
  arg_list(){
    args=(leftv) NULL;
  }
  ~arg_list(){
    if (args!=NULL){
      args->CleanUp();
    }
  }
  void appendPoly(const Poly& p){
    leftv v=initArg();
    v->data=p.as_poly();
    v->rtyp=POLY_CMD;
    internal_append(v);
  }
  void appendint(int p){
      leftv v=initArg();
    v->data=(void*)p;
    v->rtyp=INT_CMD;
    internal_append(v);
  }
  void appendNumber(const Number& p){
      leftv v=initArg();
    v->data=(void*) p.as_number();
    v->rtyp=NUMBER_CMD;
    internal_append(v);
  }
  void appendVector(const Vector& p){
    leftv v=initArg();
    v->data=p.as_poly();
    v->rtyp=VECTOR_CMD;
    internal_append(v);
  }
  void appendArray(const array& f){
    
    
    object o=f.attr("shape");
  
    object o1=o[0];
    
    object o2=o[1];
    int l1=extract<int>(o1);
   

    int l2=extract<int>(o2);
    matrix m=mpNew(l1,l2);
    for(int i=0;i<l1;i++){
      for(int j=0;j<l2;j++){
	Poly& x = boost::python::extract<Poly&>(f[boost::python::make_tuple(i,j)]);
	poly p=x.as_poly();
	MATELEM(m,i+1,j+1)=p;
      }
    }
    leftv v=initArg();
    v->data=m;
    v->rtyp=MATRIX_CMD;
    internal_append(v);
    
  }
 protected:
  leftv initArg(){
    leftv res=(leftv)omAllocBin(sleftv_bin);
    res->Init();
    return res;
  }
  void internal_append(leftv v){
    if (args!=NULL){
      leftv last=args;
      while(last->next!=NULL){
	last=last->next;
      }
      last->next=v;
    } else
      args=v;
  }
  
};
PyObject* buildPyObjectFromLeftv(leftv v){
 
  switch (v->rtyp){
  case INT_CMD:
    return PyInt_FromLong((int)v->data);
  case POLY_CMD:
    
    return to_python_value<Poly>()(Poly((poly) v->data, currRing));
  case  VECTOR_CMD:
   
    return to_python_value<Vector>()( Vector((poly) v->data, currRing));
  case  NUMBER_CMD:
  
    return to_python_value<Number>()(Number((number) v->data, currRing));
  case MATRIX_CMD:
    {
      using boost::python::numeric::array;
      using boost::python::self;
      using boost::python::make_tuple;
      using boost::python::tuple;
      using boost::python::object;
      using boost::python::list;
      matrix m=(matrix) v->data;
      list l;


      for(int i=1;i<=MATROWS(m);i++){
	list row;
	for(int j=1;j<=MATCOLS(m);j++){
	  Poly ip(MATELEM(m,i,j),currRing);//copy it
	  row.append(ip);
	  //a[boost::python::make_tuple(i%2,i%5)]=ip;
	  //a[boost::python::make_tuple(i%2,i%5)]=ip;
	}
	l.append(row);
      }
      //FIXME: should call this only once
      array::set_module_and_type("Numeric",
			     "ArrayType"
			     );

      return to_python_value<array>()(array(l));
    }
  default:
    Py_INCREF(Py_None);
    return Py_None;
  }
}
PyObject* call_interpreter_method(const idhdl_wrap& proc, const arg_list& args){
  int err=iiPStart(proc.id, args.args);
  int voice=myynest+1;
  PyObject* res=buildPyObjectFromLeftv(&iiRETURNEXPR[voice]);
  errorreported=inerror=0;
  return res;
}
static boost::python::str idhdl_as_str(idhdl_wrap iw){
  idhdl i=iw.id;
  using boost::python::str;
  //ring r=p.getRing();
  

  std::basic_stringstream<char>  s;
  s<<i;
  return boost::python::str(s.str());
}
static idhdl_wrap get_idhdl(const char *n){
  //idhdl ggetid(const char *n, BOOLEAN local = FALSE);
  return idhdl_wrap(ggetid(n, FALSE));
}
BOOST_PYTHON_MODULE(Singular){
  export_poly();
  boost::python::class_<arg_list>("i_arg_list")
    .def("append", &arg_list::appendPoly)
    .def("append", &arg_list::appendArray)
    .def("append", &arg_list::appendNumber)
   .def("append", &arg_list::appendint)
   .def("append", &arg_list::appendVector);
  boost::python::class_<idhdl_wrap>("interpreter_id")
    .def("is_zero", &idhdl_wrap::is_zero)
    .def("__str__", idhdl_as_str);
  boost::python::class_<Variable>("variable")
    .def(boost::python::init <const int, char>())
    .def(boost::python::init <char>())
    
    .def(boost::python::init <const int>());
  export_CF();

  export_number();
  export_vector();
  export_playground();
  boost::python::class_<PowerSeries>("power_series")       
    .def(boost::python::init <const PowerSeries::numerator_type &,const PowerSeries::denominator_type&>())
    .def("__iter__", boost::python::iterator<PowerSeries>());
  boost::python::class_<VectorPowerSeries>("vector_power_series")
    .def(boost::python::init <const VectorPowerSeries::numerator_type&,const VectorPowerSeries::denominator_type &>())
    .def("__iter__", boost::python::iterator<VectorPowerSeries>());
  def("gen",unitVector0);
  def("get_idhdl", get_idhdl);
  def("call_interpreter_method",call_interpreter_method);
  //    .def(self+=self)
   
  //   .def(self+self)
  //.def(self*=Number())
  //.def(Number() * self);

}
#endif
