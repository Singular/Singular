#include <poly_wrap.h>
#include <boost/python.hpp>
#include "mod2.h"
#include "subexpr.h"
#include "Poly.h"
#include "Ideal.h"
#include "lists.h"
#include "structs.h"
#include "ipshell.h"
#include "numbers.h"

#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
#include "matpol.h"
extern BOOLEAN errorreported;
extern int inerror;
using namespace boost::python;
using boost::python::numeric::array;
using boost::python::extract;
static void free_leftv(leftv args){
    args->CleanUp();
    omFreeBin(args, sleftv_bin);
}
matrix matrixFromArray(const array& f){
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
  return m;
}
bool is_builtin(const char* name){
  int cmd_n=-1;
  IsCmd(name,cmd_n);
  return (cmd_n!=-1);
}
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
  bool id_is_proc(){
    return (id->typ==PROC_CMD);
  }
  bool print_type(){
    Print("type:%d\n",id->typ);
  }
  void writePoly(const Poly& p){
    
    if (id->typ==POLY_CMD){
      p_Delete(&id->data.p, currRing);
      id->data.p=p.as_poly();
    }
    
  }
  void writeIdeal(const Ideal& p){
    if (id->typ==IDEAL_CMD){
      id_Delete(&id->data.uideal, currRing);
      
      id->data.uideal=p.as_ideal();
    }
  }
  void writeint(int p){
    if (id->typ==INT_CMD){
      id->data.i=p;
    }
  }
  void writeNumber(const Number& p){
     
 if (id->typ==NUMBER_CMD){
      n_Delete(&id->data.n, currRing);
      id->data.n=p.as_number();
    }
  }
  void writeVector(const Vector& p){
       
    if (id->typ==VECTOR_CMD){
      p_Delete(&id->data.p, currRing);
      id->data.p=p.as_poly();
    }
  }
  void writeArray(const array& f){
    if(id->typ=MATRIX_CMD){
      matrix m=matrixFromArray(f);
      id_Delete((ideal*) &id->data.umatrix,currRing);
      id->data.umatrix;
    }
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
      omFreeBin(args, sleftv_bin);
    }
    
  }
  leftv pop_front(){
    assume(args!=NULL);
    leftv res=args;
    args=args->next;
    res->next=NULL;
    return res;
  }
  int length(){
    leftv v=args;
    int l=0;
    while(v!=NULL){
      l++;
      v=v->next;
    }
    return l;
  }
  void appendPoly(const Poly& p){
    leftv v=initArg();
    v->data=p.as_poly();
    v->rtyp=POLY_CMD;
    internal_append(v);
  }
  void appendIdeal(const Ideal& p){
    leftv v=initArg();
    v->data=p.as_ideal();
    v->rtyp=IDEAL_CMD;
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
    leftv v=initArg();
    matrix m=matrixFromArray(f);
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
static array buildPythonMatrix(matrix m, ring r){
  using boost::python::numeric::array;
  using boost::python::self;
  using boost::python::make_tuple;
  using boost::python::tuple;
  using boost::python::object;
  using boost::python::list;
 
  list l;
  
  
  for(int i=1;i<=MATROWS(m);i++){
    list row;
    for(int j=1;j<=MATCOLS(m);j++){
      Poly ip(MATELEM(m,i,j),r);//copy it
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
  
  return array(l);
}
boost::python::object buildPyObjectFromLeftv(leftv v){
  using boost::python::object;
  switch (v->rtyp){
  case INT_CMD:
    return object((int)v->data);
  case POLY_CMD:
    
    return object(Poly((poly) v->data, currRing));
  case  VECTOR_CMD:
   
    return object( Vector((poly) v->data, currRing));
  case IDEAL_CMD:
    return object(Ideal((ideal) v->data, currRing));
  case  NUMBER_CMD:
  
    return object(Number((number) v->data, currRing));
  case MATRIX_CMD:
    {
      return buildPythonMatrix((matrix) v->data,currRing);
    }
  default:
    
    return object();
  }
}
//boost::python::list buildPythonList(lists l, ring r){
//    using boost::python::list;
//    list res;
//    
//    for(int i=0;i<=l->nr;i++){
//        leftv lv=&l->m[i];
//        PyObject* o=buildPyObjectFromLeftv(lv);
//        res.append(o);
//    }
//    return res;
//}
boost::python::object buildPyObjectFromIdhdl(const idhdl_wrap&  id){
  using boost::python::object;
 
  switch (id.id->typ){
  case INT_CMD:
    return object((int)id.id->data.i);
  case POLY_CMD:
    
    return object(Poly((poly) id.id->data.p, currRing));
  case  VECTOR_CMD:
   
    return object( Vector((poly) id.id->data.p, currRing));
  case IDEAL_CMD:
    //object res;

    return object(Ideal((ideal) id.id->data.uideal, currRing));
  case  NUMBER_CMD:
  
    return object(Number((number) id.id->data.n, currRing));
  case MATRIX_CMD:
    {
      return buildPythonMatrix((matrix) id.id->data.umatrix,currRing);
    }
  //case LIST_CMD:
  //  return to_python_value<boost::python::list>()(buildPythonList((lists) id.id->data.l, currRing));
  default:
    return object();    
    //Py_INCREF(Py_None);
    //return Py_None;
  }
}
object buildBPyObjectFromIdhdl(const idhdl_wrap&  id){
  using boost::python::object;
 
  switch (id.id->typ){
  case INT_CMD:
    return object((int) id.id->data.i);//PyInt_FromLong((int)id.id->data.i);
  //case POLY_CMD:
  //  
  //  return to_python_value<Poly>()(Poly((poly) id.id->data.p, currRing));
  //case  VECTOR_CMD:
  // 
  //  return to_python_value<Vector>()( Vector((poly) id.id->data.p, currRing));
  //case IDEAL_CMD:
  //  return to_python_value<Ideal>()(Ideal((ideal) id.id->data.uideal, currRing));
  //case  NUMBER_CMD:
  //
  //  return to_python_value<Number>()(Number((number) id.id->data.n, currRing));
  //case MATRIX_CMD:
  //  {
  //    return buildPythonMatrix((matrix) id.id->data.umatrix,currRing);
  //  }
  //case LIST_CMD:
  //  return to_python_value<boost::python::list>()(buildPythonList((lists) id.id->data.l, currRing));
  default:
    return object();    
    //Py_INCREF(Py_None);
    //return Py_None;
  }
}

boost::python::object call_interpreter_method(const idhdl_wrap& proc, const arg_list& args){

  int err=iiPStart(proc.id, args.args);
  int voice=myynest+1;
  errorreported=inerror=0;

  return buildPyObjectFromLeftv(&iiRETURNEXPR[voice]);
  
  //return res;
}
//PyObject* call_builtin_method(const char* name, Ideal i){
//  arg_list l;
//  l.appendIdeal(i);
//  leftv a=l.args;
//  int cmd_n=-1;
//  IsCmd(name,cmd_n);
////     Py_INCREF(Py_None);
//
////   return Py_None;
//  if (cmd_n<0){
//  Py_INCREF(Py_None);
//  PrintS("is not a cmd");
//  return Py_None;
// 
//  } else {
//    leftv res=(leftv)omAllocBin(sleftv_bin);
//    res->Init();
//    iiExprArith1(res,a,cmd_n);
//    PyObject* real_res=buildPyObjectFromLeftv(res);
//    res->CleanUp();
//    omFreeBin(res, sleftv_bin);
//    errorreported=inerror=0;
//    return real_res;
//    //cleanup not to forget
//  }
//}
boost::python::object call_builtin_method_general(const char* name, arg_list& l){
  

  int cmd_n=-1;
  IsCmd(name,cmd_n);
//     Py_INCREF(Py_None);

//   return Py_None;
  if (cmd_n<0){
  

  return object();
 
  } else {


    leftv res=(leftv)omAllocBin(sleftv_bin);
    res->Init();
    switch(l.length()){
    case 1:
      iiExprArith1(res,l.args,cmd_n);
      break;
    case 2:
      {
	leftv arg1=l.pop_front();
	leftv arg2=l.pop_front();
	iiExprArith2(res,arg1,cmd_n,arg2,TRUE);
	free_leftv(arg1);
	free_leftv(arg2);
	break;
      }
    case 3:
      {
	leftv arg1=l.pop_front();
	leftv arg2=l.pop_front();
	leftv arg3=l.pop_front();
	free_leftv(arg1);
	free_leftv(arg2);
	free_leftv(arg3);
	
	iiExprArith3(res,cmd_n,arg1,arg2,arg3);
	break;
      }
    default:
      iiExprArithM(res, l.args, cmd_n);
    }

    boost::python::object real_res=buildPyObjectFromLeftv(res);
    res->CleanUp();
    omFreeBin(res, sleftv_bin);
    errorreported=inerror=0;
    return real_res;
    //cleanup not to forget
  }
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
void export_interpreter()
{
  def("get_idhdl", get_idhdl);
  boost::python::class_<arg_list>("i_arg_list")
    .def("append", &arg_list::appendPoly)
    .def("append", &arg_list::appendArray)
    .def("append", &arg_list::appendNumber)
    .def("append", &arg_list::appendint)
    .def("append", &arg_list::appendIdeal)
    .def("append", &arg_list::appendVector);
  boost::python::class_<idhdl_wrap>("interpreter_id")
    .def("is_zero", &idhdl_wrap::is_zero)
    .def("is_proc", &idhdl_wrap::id_is_proc)
    .def("print_type", &idhdl_wrap::print_type)
    .def("write", &idhdl_wrap::writePoly)
    .def("write", &idhdl_wrap::writeArray)
    .def("write", &idhdl_wrap::writeNumber)
    .def("write", &idhdl_wrap::writeint)
    .def("write", &idhdl_wrap::writeIdeal)
    .def("write", &idhdl_wrap::writeVector)
    .def("__str__", idhdl_as_str);
  def("call_interpreter_method",call_interpreter_method);
  def("cbm",call_builtin_method_general);
  def("transfer_to_python",buildPyObjectFromIdhdl);
  def("is_builtin", is_builtin);
}


