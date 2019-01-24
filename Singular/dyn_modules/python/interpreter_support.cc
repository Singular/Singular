#include "kernel/mod2.h"
#ifdef HAVE_PYTHON_MOD
#include <sstream>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "Singular/subexpr.h"
#include "Poly.h"
#include "Ideal.h"
#include "kernel/structs.h"
#include "Singular/lists.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "coeffs/numbers.h"

#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "polys/matpol.h"
#include "ring_wrap.h"
#include "intvec_wrap.h"
#include "poly_wrap.h"
EXTERN_VAR int inerror;
using namespace boost::python;
using boost::python::extract;
static void free_leftv(leftv args)
{
    args->CleanUp();
    omFreeBin(args, sleftv_bin);
}

#if BOOST_VERSION < 106500    /* 1.65 */
typedef typename boost::python::numeric::array NumpyArrayType;
#else
#include <boost/python/numpy.hpp>
typedef typename boost::python::numpy::ndarray NumpyArrayType;
#endif

matrix matrixFromArray(const NumpyArrayType& f)
{
  object o=f.attr("shape");
  object o1=o[0];
  object o2=o[1];
  int l1=extract<int>(o1);
  int l2=extract<int>(o2);
  matrix m=mpNew(l1,l2);
  for(int i=0;i<l1;i++)
  {
    for(int j=0;j<l2;j++)
    {
      Poly& x = boost::python::extract<Poly&>(f[boost::python::make_tuple(i,j)]);
      poly p=x.as_poly();
      MATELEM(m,i+1,j+1)=p;
    }
  }
  return m;
}
bool is_builtin(const char* name)
{
  int cmd_n=-1;
  IsCmd(name,cmd_n);
  return (cmd_n!=-1);
}
class arg_list
{
 public:
  leftv args;
  arg_list()
  {
    args=(leftv) NULL;
  }
  ~arg_list()
  {
    if (args!=NULL)
    {
      args->CleanUp();
      omFreeBin(args, sleftv_bin);
    }
  }
  leftv pop_front()
  {
    assume(args!=NULL);
    leftv res=args;
    args=args->next;
    res->next=NULL;
    return res;
  }
  int length()
  {
    leftv v=args;
    int l=0;
    while(v!=NULL)
    {
      l++;
      v=v->next;
    }
    return l;
  }
  void appendPoly(const Poly& p)
  {
    leftv v=initArg();
    v->data=p.as_poly();
    v->rtyp=POLY_CMD;
    internal_append(v);
  }
  void appendIdeal(const Ideal& p)
  {
    leftv v=initArg();
    v->data=p.as_ideal();
    v->rtyp=IDEAL_CMD;
    internal_append(v);
  }
  void appendModule(const Module& p)
  {
    leftv v=initArg();
    v->data=p.as_module();
    v->rtyp=MODUL_CMD;
    internal_append(v);
  }
  void appendint(int p)
  {
    leftv v=initArg();
    v->data=(void*)((long)p);
    v->rtyp=INT_CMD;
    internal_append(v);
  }
  void appendNumber(const Number& p)
  {
    leftv v=initArg();
    v->data=(void*) p.as_number();
    v->rtyp=NUMBER_CMD;
    internal_append(v);
  }
  void appendVector(const Vector& p)
  {
    leftv v=initArg();
    v->data=p.as_poly();
    v->rtyp=VECTOR_CMD;
    internal_append(v);
  }
  void appendArray(const NumpyArrayType& f)
  {
    leftv v=initArg();
    matrix m=matrixFromArray(f);
    v->data=m;
    v->rtyp=MATRIX_CMD;
    internal_append(v);
  }
  void appendString(const char* s)
  {
    leftv v=initArg();
    v->data=omStrDup(s);
    v->rtyp=STRING_CMD;
    internal_append(v);
  }
  void appendRing(const Ring& r)
  {
    leftv v=initArg();
    v->data=r.pimpl.get();
    r.pimpl->ref++;
    v->rtyp=RING_CMD;
    internal_append(v);
  }
  lists dumpToLists()
  {
    int n=length();
    lists res=(lists)omAlloc0Bin(slists_bin);
    res->Init(n);
    for(int i=0;i<n;i++)
    {
      leftv iv=pop_front();
      //swap the content
      memcpy(&res->m[i],iv,sizeof(sleftv));
      //iv->Init();
      omFreeBin(iv, sleftv_bin);
    }
    return res;
  }
  void appendPrelist(arg_list& l)
  {
    leftv v=initArg();
    v->data=l.dumpToLists();
    v->rtyp=LIST_CMD;
    internal_append(v);
  }
  void appendIntvec(Intvec& iv)
  {
    leftv v=initArg();
    v->data=iv.allocate_legacy_intvec_copy();
    v->rtyp=INTVEC_CMD;
    internal_append(v);
  }
protected:
  leftv initArg()
  {
    leftv res=(leftv)omAllocBin(sleftv_bin);
    res->Init();
    return res;
  }
  void internal_append(leftv v)
  {
    if (args!=NULL)
    {
      leftv last=args;
      while(last->next!=NULL)
      {
        last=last->next;
      }
      last->next=v;
    }
    else
      args=v;
  }
};

class idhdl_wrap
{
 public:
  idhdl id;
  idhdl_wrap(idhdl id)
  {
    this->id=id;
  }
  idhdl_wrap()
  {
    id=(idhdl) NULL;
  }
  bool is_zero()
  {
    return id==NULL;
  }
  bool id_is_proc()
  {
    return (id->typ==PROC_CMD);
  }
  bool print_type()
  {
    Print("type:%d\n",id->typ);
  }
  void writePoly(const Poly& p)
  {
    if (id->typ==POLY_CMD)
    {
      p_Delete(&id->data.p, currRing);
      id->data.p=p.as_poly();
    }
  }
  void writeIdeal(const Ideal& p)
  {
    if (id->typ==IDEAL_CMD)
    {
      id_Delete(&id->data.uideal, currRing);
      id->data.uideal=p.as_ideal();
    }
  }
  void writeModule(const Module& p)
  {
    if (id->typ==MODUL_CMD)
    {
      id_Delete(&id->data.uideal, currRing);
      id->data.uideal=p.as_module();
    }
  }
  void writeint(int p)
  {
    if (id->typ==INT_CMD)
    {
      id->data.i=p;
    }
  }
  void writeNumber(const Number& p)
  {
    if (id->typ==NUMBER_CMD)
    {
      n_Delete(&id->data.n, currRing);
      id->data.n=p.as_number();
    }
  }
  void writeVector(const Vector& p)
  {
    if (id->typ==VECTOR_CMD)
    {
      p_Delete(&id->data.p, currRing);
      id->data.p=p.as_poly();
    }
  }
  void writeArray(const NumpyArrayType& f)
  {
    if(id->typ=MATRIX_CMD)
    {
      matrix m=matrixFromArray(f);
      id_Delete((ideal*) &id->data.umatrix,currRing);
      id->data.umatrix=m;
    }
  }
  void writeRing(const Ring& r)
  {
    if(id->typ=RING_CMD)
    {
      r.pimpl->ref++;
      ((ring) id->data.uring)->ref--;//FIXME: destruct it
      ring r2=r.pimpl.get();
            id->data.uring=r2;
    }
  }
  void writeString(const char* s)
  {
    if(id->typ=STRING_CMD)
    {
        omFree((ADDRESS) id->data.ustring);
        id->data.ustring=omStrDup(s);
    }
  }
  void writeIntvec(const Intvec& iv)
  {
    if(id->typ=INTVEC_CMD)
    {
        delete id->data.iv;
        id->data.iv=iv.allocate_legacy_intvec_copy();;
    }
  }
  void writeList(arg_list& f)
  {
    //warning f gets empty
    if(id->typ=LIST_CMD)
    {
        id->data.l->Clean(currRing);
        id->data.l=f.dumpToLists();
    }
  }
};


static NumpyArrayType buildPythonMatrix(matrix m, ring r)
{
  //using boost::python::numeric::array;
  using boost::python::self;
  using boost::python::make_tuple;
  using boost::python::tuple;
  using boost::python::object;
  using boost::python::list;

  list l;
  for(int i=1;i<=MATROWS(m);i++)
  {
    list row;
    for(int j=1;j<=MATCOLS(m);j++)
    {
      Poly ip(MATELEM(m,i,j),r);//copy it
      row.append(ip);
      //a[boost::python::make_tuple(i%2,i%5)]=ip;
      //a[boost::python::make_tuple(i%2,i%5)]=ip;
    }
    l.append(row);
  }
  #if BOOST_VERSION <106500
  //FIXME: should call this only once
  NumpyArrayType::set_module_and_type("Numeric",
                             "ArrayType"
                             );
  return NumpyArrayType(l);
  #else
  return boost::python::numpy::array(l);
  #endif
}
boost::python::object buildPyObjectFromLeftv(leftv v);
boost::python::list buildPythonList(lists l, ring r)
{
    using boost::python::list;
    list res;

    for(int i=0;i<=l->nr;i++)
    {
        leftv lv=&l->m[i];
        object o=buildPyObjectFromLeftv(lv);
        res.append(o);
    }
    return res;
}

boost::python::object buildPyObjectFromLeftv(leftv v)
{
  using boost::python::object;
  switch (v->rtyp)
  {
  case INT_CMD:
    return object((int)((long)v->data));
  case POLY_CMD:
    return object(Poly((poly) v->data, currRing));
  case STRING_CMD:
    return str((const char*) v->data);
  case  VECTOR_CMD:
    return object( Vector((poly) v->data, currRing));
  case IDEAL_CMD:
    return object(Ideal((ideal) v->data, currRing));
  case MODUL_CMD:
    return object(Module((ideal) v->data, currRing));
  case  NUMBER_CMD:
    return object(Number((number) v->data, currRing));
  case MATRIX_CMD:
    return buildPythonMatrix((matrix) v->data,currRing);
  case LIST_CMD:
    return buildPythonList((lists) v->data, currRing);
  case RING_CMD:
    return object(Ring((ring) v->data));
  case INTVEC_CMD:
    return object(Intvec(*(intvec*) v->data));

  default:
    return object();
  }
}
boost::python::object buildPyObjectFromIdhdl(const idhdl_wrap&  id)
{
  using boost::python::object;

  switch (id.id->typ)
  {
  case STRING_CMD:
    return str((const char*) id.id->data.ustring);

  case INT_CMD:
    return object((int)id.id->data.i);
  case POLY_CMD:

    return object(Poly((poly) id.id->data.p, currRing));
  case  VECTOR_CMD:

    return object( Vector((poly) id.id->data.p, currRing));
  case IDEAL_CMD:
    //object res;

    return object(Ideal((ideal) id.id->data.uideal, currRing));
  case MODUL_CMD:
    //object res;

    return object(Module((ideal) id.id->data.uideal, currRing));
  case  NUMBER_CMD:

    return object(Number((number) id.id->data.n, currRing));
  case MATRIX_CMD:
    {
      return buildPythonMatrix((matrix) id.id->data.umatrix,currRing);
    }
  case LIST_CMD:
    return buildPythonList((lists) id.id->data.l, currRing);
  case RING_CMD:
    return object(Ring((ring) id.id->data.uring));
  case INTVEC_CMD:
    return object(Intvec(*(intvec*) id.id->data.iv));
  default:
    return object();
    //Py_INCREF(Py_None);
    //return Py_None;
  }
}

boost::python::object call_interpreter_method(const idhdl_wrap& proc, const arg_list& args)
{
  int err=iiMake_proc(proc.id, NULL, args.args);
  errorreported=inerror=0;

  return buildPyObjectFromLeftv(&iiRETURNEXPR);
}
boost::python::object call_builtin_method_general(const char* name, arg_list& l)
{
  int cmd_n=-1;
  IsCmd(name,cmd_n);
//     Py_INCREF(Py_None);

//   return Py_None;
  if (cmd_n<0)
  {
    return object();
  }
  else
  {
    leftv res=(leftv)omAllocBin(sleftv_bin);
    res->Init();
    switch(l.length())
    {
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
        iiExprArith3(res,cmd_n,arg1,arg2,arg3);
        free_leftv(arg1);
        free_leftv(arg2);
        free_leftv(arg3);
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
static boost::python::str idhdl_as_str(idhdl_wrap iw)
{
  idhdl i=iw.id;
  using boost::python::str;
  //ring r=p.getRing();

  std::basic_stringstream<char>  s;
  s<<i;
  return boost::python::str(s.str());
}
static idhdl_wrap get_idhdl(const char *n)
{
  //idhdl ggetid(const char *n);
  return idhdl_wrap(ggetid(n));
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
    .def("append", &arg_list::appendModule)
    .def("append", &arg_list::appendPrelist)
    .def("append", &arg_list::appendVector)
    .def("append", &arg_list::appendRing)
    .def("append", &arg_list::appendIntvec)
    .def("append", &arg_list::appendString);
  boost::python::class_<idhdl_wrap>("interpreter_id")
    .def("is_zero", &idhdl_wrap::is_zero)
    .def("is_proc", &idhdl_wrap::id_is_proc)
    .def("print_type", &idhdl_wrap::print_type)
    .def("write", &idhdl_wrap::writePoly)
    .def("write", &idhdl_wrap::writeArray)
    .def("write", &idhdl_wrap::writeNumber)
    .def("write", &idhdl_wrap::writeint)
    .def("write", &idhdl_wrap::writeIdeal)
    .def("write", &idhdl_wrap::writeModule)
    .def("write", &idhdl_wrap::writeVector)
    .def("write", &idhdl_wrap::writeList)
    .def("write", &idhdl_wrap::writeString)
    .def("write", &idhdl_wrap::writeIntvec)
    .def("write", &idhdl_wrap::writeRing)
    .def("__str__", idhdl_as_str);
  def("call_interpreter_method",call_interpreter_method);
  def("cbm",call_builtin_method_general);
  def("transfer_to_python",buildPyObjectFromIdhdl);
  def("is_builtin", is_builtin);
}
#endif
