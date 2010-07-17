from Singular import *
#from _Singular import *
import _Singular
try:
    import psyco
    def optimize(f):
        psyco.bind(f)
except:
    def optimize(f):
        pass
def debug_out(s):
  pass
  #print s
#def build_arg_list(*args)
def is_int_tuple(a):
    if isinstance(a,tuple):
        for e in a:
            if not isinstance(e, int):
                False
        return True
    else:
        return False
def int_tuple2iv(a):
    res=IntVector()
    for e in a:
        res.append(e)
    return res
    
    
def list2arg_list(args):
    l=_Singular.i_arg_list()
    for a in args:
        if isinstance(a,list):
            l.append(list2arg_list(a))
            continue
        if is_int_tuple(a):
            l.append(int_tuple2iv(a))
            continue
#            at=i_arg_list()
#            for a2 in a:
#                at.append(a2)
#            l.append(at)
        
        l.append(a)
    return l
class SingularGlobalsProxy(object):
    """The Most important class
      singular=singular_globals_proxy()
      for some ideal
        singular.groebner(i)
        singular.std(i)
      in general all interpreter and kernel functions
      with supported data type work
      for some global poly variable p in the interpreter and p in python
        singular.p=p
        p=singular.p
      this is always a copy as this object is only some magic proxy"""
    def __mycbm(self,name,*args):
      l=list2arg_list(args)
      prepare_ring(args)
      res= _Singular.cbm(name,l)
      finish_ring()
      return res

    def __getattr__(self,name):
        proc=_Singular.get_idhdl(name)
        if proc.is_zero():
            if _Singular.is_builtin(name):
                def fun_wrapper(*args):
                    return self.__mycbm(name,*args)
                try:
                    fun_wrapper.__name__=name
                except:
                    pass
                return fun_wrapper
            else:
                raise AttributeError("Global variable " + name + " not present in the Singular interpreter")
        if proc.is_proc():
            def fun_wrapper(*args):
                
                
                proc=_Singular.get_idhdl(name)
                if not proc.is_proc():
                    proc.print_type()
                    raise Exception
                prepare_ring(args)
                l=list2arg_list(args)
                erg= _Singular.call_interpreter_method(proc, l)
                finish_ring()
                return erg
            try:
                fun_wrapper.__name__=name
            except:
                pass
            return fun_wrapper
        else:
            res=_Singular.transfer_to_python(proc)
            if res is None:
                raise AttributeError("Global variable "+name+" has unknown type")
            return res
    def __setattr__(self,name,value):
        id=_Singular.get_idhdl(name)
        if id.is_zero():
            raise Exception
        else:
            if isinstance(value,list):
                value=list2arg_list(value)
            id.write(value)
#for compatibility the old name
#global_functions=singular_globals_proxy


def find_rings(arglist):
  """FIXME: doesn't handle everything and depth"""
  for item in arglist:
    if isinstance(item,Polynomial) or isinstance(item,Ideal) or isinstance(item, Number) or isinstance(item, Vector):
      return [item.ring()]
  return []
  

oldrings=[]
def prepare_ring(arglist):
  rl=find_rings(arglist)
  debug_out("rl is" +str(rl))
  if len(rl)==1:
    r=rl[0]
    oldrings.append(Ring())
    r.set()
  else:
    if len(rl)==0:
      oldrings.append(None)
    else:
      debug_out("Warning to many rings in call")
      oldrings.append(None)

def finish_ring():
  r=oldrings.pop()
  if r:
    r.set()
#optimize(prepare_ring)
#optimize(mycbm)
#optimize(finish_ring)
