from objects import OMObjectBase, OMSymbol
from cd import *
from omexceptions import *
from itertools import izip
from copy import copy
cdFns1=OMcd("fns1")
lambdasym=OMSymbol("lambda",cdFns1)

def islambda(sym):
    return lambdasym==sym
    
class OMbinding(OMObjectBase):
    """hopefully fixed possible problems: reevaluation writes new scope, if it isn't
       meant so, references do not work correctly because of scopes
       solve this by first evaluation to bounded OMbinding"""
    def __init__(self, binder,variables,block):
        super(OMbinding,self).__init__()
        self.block=block
        self.binder=binder
        self.variables=variables
        self.bounded=False
    def evaluate(self,context):
        assert islambda(self.binder)
        if not self.bounded:
            mycopy=copy(self)
            mycopy.scope=context.scopeFromCurrentScope()
            mycopy.bounded=True
            return mycopy
        else:
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
        
    XMLtag="OMBIND"
    def getChildren(self):
        return [self.binder]+self.variables+[self.block]
    def setChildren(self):
        raise UnsupportedOperationError 
        
