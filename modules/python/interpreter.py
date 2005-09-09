from Singular import *
class singular_globals_proxy(object):
    def __getattr__(self,name):
        proc=get_idhdl(name)
        if proc.is_zero():
            raise AttributeError("Global variable " + name + " not present in the Singular interpreter")
        if proc.is_proc():
            def fun_wrapper(*args):
                l=i_arg_list()
                for a in args:
                    l.append(a)
                proc=get_idhdl(name)
                if not proc.is_proc():
                    proc.print_type()
                    raise Exception
                return call_interpreter_method(proc, l)
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
