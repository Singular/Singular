from objects import OMsymbol
from omexceptions import *
class OMcd(object):
    def __init__(self,name, base="http://www.openmath.org/cd"):
        self.name=name
        self.base=base
    def  __eq__(self, other):
        return (self.name==other.name) and (self.base==other.base)
    def __hash__(self):
        return hash((self.name,self.base))

class OMcdImplementation(object):
    def __init__(self,cd):
        self.cd=cd
        self.implementations={}
    def __getitem__(self, name):
        return self.implementations[name]
    def __setitem__(self,name,value):
        self.implementations[name]=value
    def implement(self,symbolname, func):
        symbol=OMsymbol(symbolname,self.cd)
        impl=ImplementedOMsymbol(symbol,func)
        self[symbol]=impl
        


class ImplementedOMsymbol(OMsymbol):
    def __init__(self,symbol, func):
        super(ImplementedOMsymbol,self).__init__(symbol.name, symbol.cd)
        self.implementation=func
    def __str__(self):
        return "ImplementedOMS("+self.name+", " + self.cd.name +")"
    def __call__(self, context, *args):
        try:
            erg=self.implementation(context,*args)
        except KeyError:
             raise EvaluationFailedError
        return context.package(erg)