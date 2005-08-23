//$Id: wrapper.h,v 1.15 2005-08-23 15:55:35 bricken Exp $
#ifndef PYTHON_SINGULAR_WRAPPER_HEADER
#define PYTHON_SINGULAR_WRAPPER_HEADER
#include <Python.h>
#include <boost/python.hpp>
#include <sstream>
#include "mod2.h"
//shouldn't be necessary but it is for structs.h::idrec
#define __cplusplus 1
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
extern BOOLEAN errorreported;
extern int inerror;
using namespace boost::python;
typedef std::basic_stringstream<char>  mysstream;
Vector unitVector0(int i){
  return unitVector(i,currRing);
}

static boost::python::object Number_as_str(Number n)
{
  using boost::python::str;
  StringSetS("");
  n.write();
  char* out=StringAppendS("");
  return boost::python::str(out,strlen(out));
}
static boost::python::object CF_as_str(const CanonicalForm& f)
{
  using boost::python::str;
  mysstream s;
  s<<f;
  return boost::python::str(s.str());
}
static boost::python::object Poly_as_str(Poly& p)
{
  using boost::python::str;
  //ring r=p.getRing();
 
  char* out=p.c_string();
  return boost::python::str(out,strlen(out));
}
static boost::python::object Vector_as_str(Vector& p)
{
  using boost::python::str;
  //ring r=p.getRing();
 
  char* out=p.c_string();
  return boost::python::str(out,strlen(out));
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
  if (v->rtyp==INT_CMD)
    return PyInt_FromLong((int)v->data);
  else {
    if (v->rtyp==POLY_CMD){
      Poly p=Poly((poly) v->data, (ring) currRing);
      //boost::python::wrapper<Poly>* wp=new wrapper<Poly>(Poly((poly) v->data, currRing));
      //return boost::python::get_managed_object(*wp, boost::python::tag);
      return to_python_value<Poly>()(p);
    }
    //Werror("not supported return value");

  }
      Py_INCREF(Py_None);
    return Py_None;
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
  

  mysstream s;
  s<<i;
  return boost::python::str(s.str());
}
static idhdl_wrap get_idhdl(const char *n){
  //idhdl ggetid(const char *n, BOOLEAN local = FALSE);
  return idhdl_wrap(ggetid(n, FALSE));
}
BOOST_PYTHON_MODULE(Singular){
  boost::python::class_<arg_list>("i_arg_list")
    .def("append", &arg_list::appendPoly)
    .def("append", &arg_list::appendNumber)
   .def("append", &arg_list::appendint)
   .def("append", &arg_list::appendVector);
  boost::python::class_<idhdl_wrap>("interpreter_id")
    .def("__str__", idhdl_as_str);
  boost::python::class_<Variable>("variable")
    .def(boost::python::init <const int, char>())
    .def(boost::python::init <char>())
    
    .def(boost::python::init <const int>());
  boost::python::class_<CanonicalForm>("canonical_form")
    .def(boost::python::init <const int>())
    .def(boost::python::init <const Variable>())
    .def("__str__", CF_as_str)
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
    .def(self/=int());
  boost::python::class_<Number>("number")
    .def(boost::python::init <int>())
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
    .def(self/=int());
  boost::python::class_<Poly>("polynomial")
    .def(boost::python::init <int>())
    .def(boost::python::init <Poly>())
    .def(boost::python::init <std::vector<int> >())
    .def(boost::python::init <Number>())
    .def("__str__", Poly_as_str)
    .def("__iter__", boost::python::iterator<Poly>())
    //read monomials (only) from string
    .def(boost::python::init <const char* >())
    
    .def(-self)
    .def(self*=self)
    .def(self+=self)
    //    .def(self-=self)
    //.def(self/=self)
    //.def(self==self)
    .def(self+self)
    .def(self*=Number())
    .def(self*Number())
    .def(self+Number())
    .def(self+=Number())
    .def(self*=Number())
    .def(self*self);
  boost::python::class_<Vector>("vector")
    .def(boost::python::init <>())
    .def("__str__", Vector_as_str)
    
    //    .def("__str__", Poly_as_str)
    .def("__iter__", boost::python::iterator<Vector>())
    .def(-self)
    .def(self+=self)
   
    .def(self+self)
    .def(self*=Number())
    .def(Poly() * self)
    .def(Number() * self);
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
