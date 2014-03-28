"""Implementation of openmath basic objects"""
from omexceptions import UnsupportedOperationError 
from  omexceptions import OutOfScopeError, EvaluationFailedError
from exceptions import NotImplementedError
from copy import copy
from xml.sax.xmlreader import AttributesImpl as attr
from itertools import izip
import base64

raiseException=True
#TODO: OMOBJ, OME, OMATTR
#from cd import *
try:
    import psyco
    def optimize(f):
        psyco.bind(f)
except:
    def optimize(f):
        pass

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
    converter=None
    def __init__(self, cd):
        object.__setattr__(self,"cd",cd)
        #object.__setattr__(self,"converter",None)
    def support(self,symbol):
        setattr(self,symbol,OMS(symbol, self.cd))
    def __setattr__(self, name,value):
        if name=="converter":
            object.__setattr__(self,name,value)
            return
        "FIXME: implements this later safer against name conflicts"
        if callable(value) and (not isinstance(value, ImplementedSymbol)):
            
            if self.converter!=None:
                object.__setattr__(
                    self, 
                    name, 
                    ImplementedSymbol(
                        OMS(name,self.cd), 
                        self.converter(value)))
            else:
                object.__setattr__(self, name, ImplementedSymbol(OMS(name,self.cd), value))
        else:
            object.__setattr__(self,name,value)

class OMS(object):
    def __init__(self,name,cd = None):
        #super(OMSymbol,self).__init__()
        self.cd = cd
        self.name = name
    def __eq__(self, other):
        try:
            return bool(other.name == self.name and self.cd == other.cd)
        except:
            return False
    def __str__(self):
        return "OMS(" + self.name + ", " + self.cd.name + ")"
    def __repr__(self):
        return str(self)
    def __hash__(self):#
        return hash((self.name,self.cd.__hash__()))
    def evaluate(self,context):
        return context.evaluateSymbol(self)
    XMLtag="OMS"
    def getXMLAttributes(self):
        #return [XMLAttribute("name", self.name),\
        #         XMLAttribute("cdbase", self.cd.base),\
        #         XMLAttribute("cd", self.cd.name)]
        return {"name":self.name, "cdbase":self.cd.base, "cd": self.cd.name}
    def setXMLAttributes(self):
        raise UnsupportedOperationError


class ImplementedSymbol(OMS):
    def __init__(self,symbol, func):
        super(ImplementedSymbol,self).__init__(symbol.name, symbol.cd)
        self.implementation = func
    def __str__(self):
        return "OMS("+self.name+", " + self.cd.name +")"
    def __call__(self, context, *args):
        try:
            erg=self.implementation(*args)
        except KeyError:
             raise EvaluationFailedError
        return context.package(erg)

cdFns1 = OMCD("fns1")
lambdasym = OMS("lambda", cdFns1)

def islambda(sym):
    "return True, iff sym is the lambda binder"
    return lambdasym == sym
    
class OMBIND(object):
    """hopefully fixed possible problems: reevaluation writes new scope, 
       if it isn't
       meant so, references do not work correctly because of scopes
       solve this by first evaluation to bounded OMBinding"""
    def __init__(self, binder, variables, block):
        #super(OMBinding, self).__init__()
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
        assert self.bounded
        self.bind(args)
        erg = self.calcErg(context)
        self.unbind()
        #print "__call__ erg is", erg
        return erg   
        
    XMLtag = "OMBIND"
    def getChildren(self):
        "get children for (XML) representation"
        return [self.binder]+self.variables+[self.block]
    
    def __str__(self):
        return "OMBIND(self.binder"+", "+str(self.variables) +", " + str(self.block) +")"
        

class OMOBJ(object):
    def __init__(self, children):
        #super(OMObject, self).__init__()
        self.children = children
    def getChildren(self):
        return self.__children
    def setChildren(self ,children):
        self.__children=children
    XMLtag = "OMOBJ"
    def evaluate(self, context):
        return OMObject([context.evaluate(c) for c in self.children])
class OMV(object):
    def __init__(self,name):
        #super(OMVar, self).__init__()
        self.name = name
    def evaluate(self, context):
        try:
            return context[self.name]
        except OutOfScopeError:
            return self
    def __str__(self):
        return "OMV(" + self.name +")"
    def __repr__(self):
        return str(self)
    XMLtag="OMV"
class OMA(object):
    def __init__(self, func, args):
        #super(OMApply, self).__init__()
        self.func = func
        self.args = args
        
    def evaluate(self, context):
        efunc = context.evaluate(self.func)
        eargs = [context.evaluate(a) for a in self.args]
        if callable(efunc):
            if raiseException:
                return context.apply(efunc,eargs)
            else:
                try:
                    return context.apply(efunc, eargs)
                except EvaluationFailedError, NotImplementedError:
                    return OMA(efunc, eargs)
                #return self
        else:
            #print efunc
            return OMA(efunc, eargs)
    def __str__ (self):
        return "OMA"+"("+str(self.func)+str(self.args)
    def __repr__ (self):
        return "OMA"+"("+str(self.func)+str(self.args)
    XMLtag="OMA"
        

# class OMB(SimpleValue):
#     def __init__(self, value):
#         #super(OMByteArray,self).__init__(value)
#         self.value=value
#     def __str__(self):
#         return "OMByteArray(" + repr(self.value) + ")"
#     def parse(self, value):
#         return value
#     XMLtag = "OMB"
#     def getBody(self):
#         return base64.encodestring(self.value)
class OMRef(object):
    def __init__(self, ref):
        self.ref=ref
    def evaluate(self, context):
        return context.evaluate(self.ref)
    def XMLEncode(self, context):
        "FIXME: maybe it should also be able to encode as reference"
        return context.XMLEncodeObject(self.ref)

class OMATTR(object):
    def __init__(self, *args):
        #super(OMAttribution,self).__init__()
        self.attr = list(args[:-1])
        self.value = args[-1]
    def getChildren(self):
        #print type(self.attr)
        #print type(self.value)
        return self.attr+[self.value]
    def evaluate(self, context):
        value = copy(self.value)
        value.attributes = copy(value.attributes)
        for attribute in self.attr:
            attribute_evaluated=context.evaluate(attribute)
            value.attributes[attribute_evaluated.key] = attribute_evaluated.value
        return value
    XMLtag = "OMATTR"
if __name__ == '__main__':
    print OMV("x")
    print OMA(OMV("x"),[OMV("y"),1])
  

    #from binding import OMBinding, lambdasym

    
    context["x"] = 1

    x = OMV("x")

    y = OMV("y")

    print context.evaluate(x)
    print context.evaluateVariable(x)
    print context.evaluate(y)
    import cd.arith1 as arith1
    context.addCDImplementation(arith1.implementation)
    print OMA(arith1.implementation.plus, [1,2])
    print context.evaluate(OMA(arith1.implementation.plus, [1,2]))
    application = OMA(OMS("plus", arith1.content), [1,2])
    
    print context.evaluate(application)
    firstArg=OMBinding(lambdasym,[OMVar("x"), OMVar("y")], OMVar("x"))
    #print context.evaluate(firstArg)
    application = OMApply(firstArg, [x,y])
    print context.evaluate(application)
    application = OMApply(firstArg, [y,x])
    print context.evaluate(application)
    
    #print type(context.lookupImplementation(arith1.plussym))
    #application=OMApply(arith1.plussym,[x])
    #application=OMApply(arith1.plussym,[x,x])
    application = OMApply(OMS("plus", arith1.content), [x, x])
    
    print context.evaluate(application)
    application = OMApply(OMS("plus", arith1.content), [x, x, x])
    
    print context.evaluate(application)
    i =  OMint(22482489)
    print i.body
    print i.XMLEncode(context)
    print context.XMLEncodeObject(OMS("plus", arith1.content))
    #i.body="dshj"
   
#optimize(OMObjectBase.__init__)
#optimize(OMObjectBase.XMLPreEncode)
#optimize(SimpleValue.__init__)
#optimize(OMint.__init__)
#optimize(OMApply.__init__)