from objects import OMobject, OMsymbol
from cd import *
from omexceptions import *
from itertools import izip
cdFns1=OMcd("fns1")
lambdasym=OMsymbol("lambda",cdFns1)

def islambda(sym):
    return lambdasym==sym
    
class OMbinding(OMobject):
    def __init__(self, binder,variables,block):
        super(OMbinding,self).__init__()
        self.block=block
        self.binder=binder
        self.variables=variables
    def evaluate(self,context):
        assert islambda(self.binder)
        self.scope=context.scopeFromCurrentScope()
        return self
    def bind(self, args):
        #print args, self.variables
        assert len(args)==len(self.variables)
        varBindings=dict(izip([i.name for i in self.variables],args))
        self.scope.push(varBindings)
    def unbind(self):
        self.scope.pop()

    def calcErg(self,context):
        return context.evaluateInScope(self.block,self.scope)
    def __call__(self, context,*args):
        self.bind(args)
        erg=self.calcErg(context)
        self.unbind()
        #print "__call__ erg is", erg
        return erg    
        