from Singular import *
class global_functions(object):
    def __getattr__(self,name):
        def fun_wrapper(*args):
            l=i_arg_list()
            for a in args:
                l.append(a)
            proc=get_idhdl(name)
            return call_interpreter_method(proc, l)
        return fun_wrapper
