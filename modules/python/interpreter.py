from Singular import *
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
                l=list2arg_list(args)
                return call_interpreter_method(proc, l)
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
            id.write(value)
#for compatibility the old name
global_functions=singular_globals_proxy

def mycbm(name,*args):
    l=i_arg_list()
    for a in args:
        if isinstance(a,list):
            at=i_arg_list()
            for a2 in a:
                at.append(a2)
            l.append(at)
        else:
            l.append(a)
    return cbm(name,l)
