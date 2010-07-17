"""Definition of the OMBinding object"""
from objects import OMObjectBase, OMSymbol
from cd import OMCD
#from omexceptions import UnsupportedOperationError
from itertools import izip
from copy import copy
cdFns1 = OMCD("fns1")
lambdasym = OMSymbol("lambda", cdFns1)

def islambda(sym):
    "return True, iff sym is the lambda binder"
    return lambdasym == sym
    
class OMBinding(OMObjectBase):
    """hopefully fixed possible problems: reevaluation writes new scope, 
       if it isn't
       meant so, references do not work correctly because of scopes
       solve this by first evaluation to bounded OMBinding"""
    def __init__(self, binder, variables, block):
        super(OMBinding, self).__init__()
        self.block = block
        self.binder = binder
        self.variables = variables
        self.bounded = False
    def evaluate(self, context):
        "evaluate the OMbinding in context"
        assert islambda(self.binder)
        if not self.bounded:
            mycopy = copy(self)
            mycopy.scope = context.scopeFromCurrentScope()
            mycopy.bounded = True
            return mycopy
        else:
            return self
    def bind(self, args):
        "bind arguments to values"
        #print args, self.variables
        assert len(args) == len(self.variables)
        varBindings = dict(izip([i.name for i in self.variables], args))
        self.scope.push(varBindings)
    def unbind(self):
        "unbind the arguments"
        self.scope.pop()

    def calcErg(self, context):
        "do the actual computation"
        return context.evaluateInScope(self.block, self.scope)
    def __call__(self, context, *args):
        self.bind(args)
        erg = self.calcErg(context)
        self.unbind()
        #print "__call__ erg is", erg
        return erg   
        
    XMLtag = "OMBIND"
    def getChildren(self):
        "get children for (XML) representation"
        return [self.binder]+self.variables+[self.block]
    
