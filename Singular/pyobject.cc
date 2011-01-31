// -*- c++ -*-
//*****************************************************************************
/** @file pyobject.cc
 *
 * @author Alexander Dreyer
 * @date 2010-12-15
 *
 * This file defines the @c blackbox operations for the pyobject type.
 *
 * @par Copyright:
 *   (c) 2010 by The Singular Team, see LICENSE file
**/
//*****************************************************************************





#include <Singular/mod2.h>

#include <Singular/ipid.h>
#include <Singular/blackbox.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/longrat.h>
#include <Singular/subexpr.h>

#include <Singular/lists.h>

#include <Python.h>
#include <iterator>             // std::distance
#include <stdio.h>

void sync_contexts();

/** @class PythonInterpreter
 * This class initializes and finalized the python interpreter.
 *
 * It also stores the Singular token number, which is assigned to this type on
 * runtime.
 **/
class PythonInterpreter {
public:
  typedef int id_type;

  ~PythonInterpreter()  { if(m_owns_python) Py_Finalize();  }

  /// Initialize unique (singleton) python interpreter instance, 
  /// and set Singular type identifier
  static void init(id_type num ) { instance().m_id = num; }

  /// Get Singular type identitfier 
  static id_type id() { return instance().m_id; }

private:
  /// Singleton: Only init() is allowed to construct an instance
  PythonInterpreter():
    m_id(0), m_owns_python(false)  { start_python(); }

  /// Static initialization - 
  /// safely takes care of destruction on program termination
  static PythonInterpreter& instance() 
  {
    static PythonInterpreter init_interpreter;
    return init_interpreter;
  }

  void start_python()
  {
    if (!Py_IsInitialized()) init_python();
    set_python_defaults();
  }
 
  void init_python()
  {
    Py_Initialize();
    m_owns_python = true;
  }

  void set_python_defaults()
  {
    // Sone python modules needs argc, argv set for some reason
    char* argv = "";
    PySys_SetArgv(1, &argv);
    PyRun_SimpleString("from sys import path, modules");
    PyRun_SimpleString("_SINGULAR_IMPORTED = dict()");

    char cmd[MAXPATHLEN + 20];
    sprintf(cmd, "path.insert(0, '%s')", feGetResource('b'));
    PyRun_SimpleString(cmd);
    PyRun_SimpleString("del path");  // cleanup
  }    

  id_type m_id;
  bool m_owns_python;
};

/** @class PythonObject
 * This class defines an interface for calling PyObject from Singular.
 *
 * @note This class does not take care of the memory mangement, this is done in
 * the blackbox routines.
 **/
class PythonObject
{
  typedef PythonObject self;

public:
  typedef PyObject* ptr_type;
  struct sequence_tag{};

  PythonObject(): m_ptr(Py_None) { }
  PythonObject(ptr_type ptr): m_ptr(ptr) {if (!ptr) handle_exception();}

  ptr_type check_context(ptr_type ptr) const {
    if(ptr) sync_contexts(); 
    return ptr;
  }
  /// Unary operations
  self operator()(int op) const
  {
    switch(op)
    {
    case '(':  return check_context(PyObject_CallObject(*this, NULL));
    case ATTRIB_CMD: return PyObject_Dir(*this);
    case PROC_CMD: return *this;
    }

    if (op == PythonInterpreter::id())
      return *this;

    Werror("unary op %d not implemented for pyobject", op);
    return self();
  }

  /// Binary and n-ary operations
  self operator()(int op, const self& arg) const {

    switch(op)
    {
      case '+':  return PyNumber_Add(*this, arg);
      case '-':  return PyNumber_Subtract(*this, arg);
      case '*':  return PyNumber_Multiply(*this, arg);
      case '/':  return PyNumber_Divide(*this, arg);
      case '^':  return PyNumber_Power(*this, arg, Py_None);
      case '(':  return check_context(PyObject_CallObject(*this, arg));
      case '[':  return operator[](arg);
      case KILLATTR_CMD: return del_attr(arg);
      case LIST_CMD:     return args2list(arg);
      case '.': case COLONCOLON: case ATTRIB_CMD: return attr(arg);
    }
    Werror("binary op %d not implemented for pyobject!", op);

    return self();
  }

  /// Ternary operations
  self operator()(int op, const self& arg1, const self& arg2) const 
  {
    switch(op)
    {
      case ATTRIB_CMD: PyObject_SetAttr(*this, arg1, arg2); return self();
    }

    Werror("ternary op %d not implemented for pyobject!", op);
    return self();
  }

  /// Get item
  self operator[](const self& idx) const { return PyObject_GetItem(*this, idx); }
  self operator[](long idx) const { return operator[](PyInt_FromLong(idx));  }

  /// Get actual PyObject*
  operator const ptr_type() const { return m_ptr; }

  /// Get representative as C-style string
  char* repr() const
  {
    return omStrDup(PyString_AsString(PyObject_Repr(*this)));
  }

  /// Extract C-style string
  char* str() const { return omStrDup(PyString_AsString(*this)); }

  Py_ssize_t size() const { return PyObject_Size(m_ptr); }

  BOOLEAN assign_to(leftv result)
  {
    return (m_ptr == Py_None? none_to(result): python_to(result));
  }

  void import_as(const char* name) const {
    idhdl handle = enterid(omStrDup(name), 0, DEF_CMD, 
                           &IDROOT, FALSE);

    if (handle)
    {
      IDDATA(handle) = (char*)m_ptr;
      Py_XINCREF(m_ptr); 
      IDTYP(handle) =  PythonInterpreter::id();
    }
    else { Werror("Importing pyobject to Singular failed"); }
  }

  int compare(int op, const self& arg) const
  { return PyObject_RichCompareBool(*this, arg, py_opid(op)); }


  self attr(const self& arg) const { return PyObject_GetAttr(*this, arg); }

  self del_attr(const self& arg) const 
  {
    if (!PyObject_HasAttr(*this, arg)) 
      Werror("Cannot delete attribute %s.", arg.repr());
    else
      PyObject_DelAttr(*this, arg); 
  
    return self();
  }

protected:
  self args2list(const self& args) const
  {
    self pylist(PyList_New(0));
    PyList_Append(pylist, *this);
    if(PyTuple_Check(args))  pylist.append_iter(PyObject_GetIter(args));
    else PyList_Append(pylist, args);

    return pylist;
  }

  void handle_exception() {
    
    PyObject *pType, *pMessage, *pTraceback;
    PyErr_Fetch(&pType, &pMessage, &pTraceback);
    
    Werror("pyobject error occured");
    Werror(PyString_AsString(pMessage));
    
    Py_XDECREF(pType);
    Py_XDECREF(pMessage);
    Py_XDECREF(pTraceback);
    
    PyErr_Clear();
  }

  void append_iter(self iterator) {
    ptr_type item;
    while (item = PyIter_Next(iterator)) {
      PyList_Append(*this, item);
      Py_DECREF(item);
    }
  }

  int py_opid(int op) const{
    switch(op)
    { 
      case '<':  return Py_LT;
      case '>':  return Py_GT;
      case EQUAL_EQUAL:  return Py_EQ;
      case NOTEQUAL:  return Py_NE;
      case GE:  return Py_GE; 
      case LE:  return Py_LE; 
    }
    return -1;
  }

private:
  BOOLEAN none_to(leftv result) const
  {
    result->data = NULL;
    result->rtyp = NONE;
    return FALSE;
  }

  BOOLEAN python_to(leftv result) const
  {
    result->data = m_ptr;
    result->rtyp = PythonInterpreter::id();
    return !m_ptr;
  }

  /// The actual pointer
  ptr_type m_ptr;
};



/** @class PythonCastStatic
 * This template class does conversion of Singular objects to python objects on
 * compile-time.
 *
 * @note The Singular objects are assumed to be equivalent to the template argument. 
 **/
template <class CastType = PythonObject::ptr_type>
class PythonCastStatic:
  public PythonObject {
public:

  PythonCastStatic(void* value):
    PythonObject(get(reinterpret_cast<CastType>(value))) {}

  PythonCastStatic(leftv value):
    PythonObject(get(reinterpret_cast<CastType>(value->Data()))) {}

private:
  ptr_type get(ptr_type value)       { return value; }
  ptr_type get(long value)           { return PyInt_FromLong(value); }
  ptr_type get(const char* value)    { return PyString_FromString(value); }
  ptr_type get(char* value) { return get(const_cast<const char*>(value)); }
  ptr_type get(lists value);         // inlined after PythonObjectDynamic
};



/** @class PythonCastDynamic
 * This class does conversion of Singular objects to python objects on runtime.
 *
 **/
class PythonCastDynamic:
  public PythonObject {
  typedef PythonCastDynamic self;

public:
  PythonCastDynamic(leftv value): PythonObject(get(value, value->Typ())) {}

private:
  PythonObject get(leftv value, int typeId)
  {
    if (typeId == PythonInterpreter::id()) return PythonCastStatic<>(value);
    
    switch (typeId)
    {
    case INT_CMD:    return PythonCastStatic<long>(value);
    case STRING_CMD: return PythonCastStatic<const char*>(value);
    case LIST_CMD:   return PythonCastStatic<lists>(value);
      //    case UNKNOWN:    return PythonCastStatic<const char*>((void*)value->Name());
    }
    
    Werror("type # %d incompatible with pyobject", typeId);
    return PythonObject();
  }
};

template <class CastType>
inline PythonObject::ptr_type
PythonCastStatic<CastType>::get(lists value)
{
  ptr_type pylist(PyList_New(0));
  for (std::size_t i = 0; i <= value->nr; ++i)
    PyList_Append(pylist, PythonCastDynamic((value->m) + i));

  return pylist;
}

/// Template specialization for getting handling sequence
template <>
class PythonCastStatic<PythonObject::sequence_tag>:
public PythonObject
{
public:

  PythonCastStatic(leftv value):
    PythonObject(PyTuple_New(size(value)))  { append_to(value); }

  
private:
  std::size_t size(leftv iter, std::size_t distance = 0) const 
  {
    if (iter) { do { ++distance; } while(iter = iter->next); }; 
    return distance;
  }
  
  void append_to(leftv iter) const
  {
    for(std::size_t idx = 0; iter != NULL; iter = iter->next)
      PyTuple_SetItem(*this, idx++, PythonCastDynamic(iter));
  }
};


PythonObject get_attrib_name(leftv arg)
{
  typedef PythonCastStatic<const char*> result_type;
  if (arg->Typ() == STRING_CMD)
    return result_type(arg);

  return result_type((void*)arg->Name());
}

/// Evaluate string in python
PythonObject python_eval(const char* arg) {

  PyObject* globals = PyModule_GetDict(PyImport_Import(PyString_FromString("__main__")));
  return PyRun_String(arg, Py_eval_input, globals, globals);
}

/// Evaluate string in python from Singular
BOOLEAN python_eval(leftv result, leftv arg) {
  if ( !arg || (arg->Typ() != STRING_CMD) ) {
    Werror("expected python_eval('string')");
    return TRUE;
  }

  return python_eval(reinterpret_cast<const char*>(arg->Data())).assign_to(result);
}


/// Execute string in python from Singular
BOOLEAN python_run(leftv result, leftv arg)
{
  if ( !arg || (arg->Typ() != STRING_CMD) ) {
    Werror("expected python_run('string')");
    return TRUE;
  }

  PyRun_SimpleString(reinterpret_cast<const char*>(arg->Data()));
  sync_contexts();
  return PythonCastStatic<>(Py_None).assign_to(result);
}

PythonObject names_from_module(const char* module_name)
{
  char buffer[strlen(module_name) + 30];
  sprintf (buffer, "SINGULAR_MODULE_NAME = '%s'", module_name);
  PyRun_SimpleString(buffer);
  PyRun_SimpleString("from sys import modules");
  PyRun_SimpleString("exec('from ' + SINGULAR_MODULE_NAME + ' import *')");

  return python_eval("[str for str in dir(modules[SINGULAR_MODULE_NAME]) if str[0] != '_']");
}

void from_module_import_all(const char* module_name)
{
  char buffer[strlen(module_name) + 20];
  sprintf (buffer, "from %s import *", module_name);
  PyRun_SimpleString(buffer);
}

/// import python module and export identifiers in Singular namespace
BOOLEAN python_import(leftv result, leftv value) {

  if ((value == NULL) || (value->Typ()!= STRING_CMD)) {
    Werror("expected python_import('string')");
    return TRUE;
  }

  from_module_import_all(reinterpret_cast<const char*>(value->Data()));
  sync_contexts();

  return PythonCastStatic<>(Py_None).assign_to(result);
}

/// blackbox support - initialization
void* pyobject_Init(blackbox*)
{
  return Py_None;
}

/// blackbox support - convert to string representation
char* pyobject_String(blackbox *b, void* ptr)
{
  return PythonCastStatic<>(ptr).repr();
}

/// blackbox support - copy element
void* pyobject_Copy(blackbox*b, void* ptr)
{ 
    Py_XINCREF(ptr);
    return ptr;
}

/// blackbox support - assign element
BOOLEAN pyobject_Assign(leftv l, leftv r)
{
  Py_XDECREF(l->Data());
  PyObject* result = PythonCastDynamic(r);
  Py_XINCREF(result);

  if (l->rtyp == IDHDL)
    IDDATA((idhdl)l->data) = (char *)result;
  else
    l->data = (void *)result;
  
  return !result;
}
                                                                     

/// blackbox support - unary operations
BOOLEAN pyobject_Op1(int op, leftv res, leftv head)
{
  switch(op)
  {
    case INT_CMD:               // built-in return types first
    {
      long value = PyInt_AsLong(PythonCastStatic<>(head));
      if( (value == -1) &&  PyErr_Occurred() ) {
        Werror("pyobject cannot be converted to integer");
        PyErr_Clear();
        return TRUE;
      }
      res->data = (void*) value;
      res->rtyp = INT_CMD;
      return FALSE;
    }
    case TYPEOF_CMD:
      res->data = (void*) omStrDup("pyobject");
      res->rtyp = STRING_CMD;  
      return FALSE; 

  case LIST_CMD:
    return FALSE;

  }
  return PythonCastStatic<>(head)(op).assign_to(res);
}


/// blackbox support - binary operations
BOOLEAN pyobject_Op2(int op, leftv res, leftv arg1, leftv arg2)
{
  PythonCastStatic<> lhs(arg1);

  switch(op)                    // built-in return types and special cases first
  { 
    case '<': case '>': case EQUAL_EQUAL: case NOTEQUAL: case GE: case LE:
    {
      res->data = (void *)lhs.compare(op, PythonCastDynamic(arg2));
      res->rtyp = INT_CMD;
      return FALSE;
    }
    case '.': case COLONCOLON: case ATTRIB_CMD:
      return lhs.attr(get_attrib_name(arg2)).assign_to(res);
  }
  PythonCastDynamic rhs(arg2);
  return lhs(op, rhs).assign_to(res);
}

/// blackbox support - ternary operations
BOOLEAN pyobject_Op3(int op, leftv res, leftv arg1, leftv arg2, leftv arg3)
{
  PythonCastStatic<> lhs(arg1);
  PythonCastDynamic rhs1(arg2);
  PythonCastDynamic rhs2(arg3);

  return lhs(op, rhs1, rhs2).assign_to(res);
}


/// blackbox support - n-ary operations
BOOLEAN pyobject_OpM(int op, leftv res, leftv args)
{
  typedef PythonCastStatic<PythonObject::sequence_tag> seq_type;

  switch(op)                    // built-in return types first
  {
    case STRING_CMD:
    {
      blackbox* a = getBlackboxStuff(args->Typ());
      res->data = (void *)a->blackbox_String(a, args->Data());
      res->rtyp = STRING_CMD;
      return FALSE;
    }
  }

  typedef PythonCastStatic<PythonObject::sequence_tag> seq_type;
  return PythonCastStatic<>(args)(op, seq_type(args->next)).assign_to(res);
}

/// blackbox support - destruction
void pyobject_destroy(blackbox *b, void* ptr)
{
  Py_XDECREF(ptr);
}

PyObject* get_current_definition(const char* name) {
  idhdl handle =  ggetid(name);
  if (!handle || (IDTYP(handle) != PythonInterpreter::id()))  return NULL;
  PythonCastStatic<PyObject*> value(IDDATA(handle));
  return value;
}

/// getting stuff from python to Singular namespace
void sync_contexts()
{
  PyRun_SimpleString("_SINGULAR_NEW = modules['__main__'].__dict__.copy()");

  PythonObject newElts = python_eval("[(_k, _e)   \
    for (_k, _e) in _SINGULAR_NEW.iteritems() \
    if _k not in _SINGULAR_IMPORTED or not _SINGULAR_IMPORTED[_k] is _e]");

  long len = newElts.size();
  for (long idx = 0; idx < len; ++idx)
  {
    char* name = newElts[idx][0].str();
    if (name && (*name != '\0') && (*name != '_'))
    {
      Py_XDECREF(get_current_definition(name));
      newElts[idx][1].import_as(name);
    }

  }

  PythonObject deletedElts = 
    python_eval("list(set(_SINGULAR_IMPORTED.iterkeys()) - \
     set(_SINGULAR_NEW.iterkeys()))");
  len = deletedElts.size();

  for (long idx = 0; idx < len; ++idx)
  {
    char* name = deletedElts[idx].str();
    if (name && (*name != '\0') && (*name != '_'))
      killid(name, &IDROOT);
  }

  PyRun_SimpleString("_SINGULAR_IMPORTED =_SINGULAR_NEW");
  PyRun_SimpleString("del  _SINGULAR_NEW");
}


// forward declaration
int iiAddCproc(char *libname, char *procname, BOOLEAN pstatic,
               BOOLEAN(*func)(leftv res, leftv v));

#define ADD_C_PROC(name) iiAddCproc("", (char*)#name, FALSE, name);


void pyobject_init() 
{
  blackbox *b = (blackbox*)omAlloc0(sizeof(blackbox));
  b->blackbox_destroy = pyobject_destroy;
  b->blackbox_String  = pyobject_String;
  b->blackbox_Init    = pyobject_Init;
  b->blackbox_Copy    = pyobject_Copy;
  b->blackbox_Assign  = pyobject_Assign;
  b->blackbox_Op1     = pyobject_Op1;
  b->blackbox_Op2     = pyobject_Op2;
  b->blackbox_Op3     = pyobject_Op3;
  b->blackbox_OpM     = pyobject_OpM;

  PythonInterpreter::init(setBlackboxStuff(b,"pyobject"));

  ADD_C_PROC(python_import);
  ADD_C_PROC(python_eval);
  ADD_C_PROC(python_run);
}

extern "C" { void mod_init() { pyobject_init(); } }

