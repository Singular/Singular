from Singular import *
def debug_out(s):
  print s
#def build_arg_list(*args)
def list2arg_list(args):
    l=i_arg_list()
    for a in args:
        if isinstance(a,list):
            l.append(list2arg_list(a))
#            at=i_arg_list()
#            for a2 in a:
#                at.append(a2)
#            l.append(at)
        else:
            l.append(a)
    return l
class singular_globals_proxy(object):
    def __getattr__(self,name):
        proc=get_idhdl(name)
        if proc.is_zero():
            if is_builtin(name):
                def fun_wrapper(*args):
                    return mycbm(name,*args)
                try:
                    fun_wrapper.__name__=name
                except:
                    pass
                return fun_wrapper
            else:
                raise AttributeError("Global variable " + name + " not present in the Singular interpreter")
        if proc.is_proc():
            def fun_wrapper(*args):
                
                
                proc=get_idhdl(name)
                if not proc.is_proc():
                    proc.print_type()
                    raise Exception
                prepare_ring(args)
                l=list2arg_list(args)
                erg= call_interpreter_method(proc, l)
                finish_ring()
                return erg
            try:
                fun_wrapper.__name__=name
            except:
                pass
            return fun_wrapper
        else:
            res=transfer_to_python(proc)
            if res is None:
                raise AttributeError("Global variable "+name+" has unknown type")
            return res
    def __setattr__(self,name,value):
        id=get_idhdl(name)
        if id.is_zero():
            raise Expception
        else:
            if isinstance(value,list):
                value=list2arg_list(value)
            id.write(value)
#for compatibility the old name
global_functions=singular_globals_proxy

def mycbm(name,*args):
    l=list2arg_list(args)
    prepare_ring(args)
    res= cbm(name,l)
    finish_ring()
    return res

def find_rings(arglist):
  """FIXME: doesn't handle everything and depth"""
  for item in arglist:
    if isinstance(item,polynomial) or isinstance(item,ideal):
      return [item.ring()]
  return []
  

oldrings=[]
def prepare_ring(arglist):
  rl=find_rings(arglist)
  debug_out("rl is" +str(rl))
  if len(rl)==1:
    r=rl[0]
    oldrings.append(ring())
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
    