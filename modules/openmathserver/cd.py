from objects import OMSymbol
from omexceptions import *
class OMCD(object):
    def __init__(self, name, base="http://www.openmath.org/cd"):
        self.name = name
        self.base = base
    def  __eq__(self, other):
        try:
            return (self.name == other.name) and (self.base == other.base)
        except:
            return False
    def __hash__(self):
        return hash((self.name,self.base))

class OMCDImplementation(object):
    def __init__(self,cd):
        self.cd = cd
        self.implementations = {}
    def __getitem__(self, name):
        return self.implementations[name]
    def __setitem__(self,name,value):
        self.implementations[name] = value
    def implement(self,symbolname, func):
        symbol=OMSymbol(symbolname,self.cd)
        impl = ImplementedOMSymbol(symbol,func)
        self[symbol]=impl
        


class ImplementedOMSymbol(OMSymbol):
    def __init__(self,symbol, func):
        super(ImplementedOMSymbol,self).__init__(symbol.name, symbol.cd)
        self.implementation = func
    def __str__(self):
        return "ImplementedOMS("+self.name+", " + self.cd.name +")"
    def __call__(self, context, *args):
        try:
            erg=self.implementation(context,*args)
        except KeyError:
             raise EvaluationFailedError
        return context.package(erg)
