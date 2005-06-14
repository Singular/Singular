from omexceptions import *
from cd import *
class OMobject(object):
    def __init__(self):
        self.attributes={}
    pass
class OMvar(OMobject):
    def __init__(self,name):
        super(OMvar,self).__init__()
        self.name=name
    def evaluate(self,context):
        try:
            return context[self.name]
        except OutOfScopeError:
            return self
    def __str__(self):
        return "OMV(" + self.name +")"
class OMapplication(OMobject):
    def __init__(self, func, args):
        super(OMapplication,self).__init__()
        self.func=func
        self.args=args
    def evaluate(self,context):
        efunc=context.evaluate(self.func)
        eargs=[context.evaluate(a) for a in self.args]
        if callable(efunc):
            try:
                return context.apply(efunc, eargs)
            except EvaluationFailedError, NotImplementedError:
                return self
        else:
            return OMapplication(efunc, eargs)
class OMsymbol(OMobject):
    def __init__(self,name,cd=None):
        super(OMsymbol,self).__init__()
        self.cd=cd
        self.name=name
    def __eq__(self, other):
        return bool(other.name==self.name and self.cd==other.cd)
    def __str__(self):
        return "OMS("+self.name+", "+self.cd.name + ")"
    def __hash__(self):#
        return hash((self.name,self.cd.__hash__()))
    def evaluate(self,context):
        return context.evaluateSymbol(self)
class SimpleValue(OMobject):
    def __init__(self,value):
        super(SimpleValue,self).__init__()
        if (isinstance(value, str)):
            value=self.parse(value)
        self.value=value
    def evaluate(self, context):
        return self
    def getValue(self):
        return self.value
    def __str__(self):
        return "OM("+repr(self.value)+")"

class OMint(SimpleValue):
    def __init__(self,value):
        super(OMint,self).__init__(value)
    def parse(self,value):
        """FIXME: Not fully standard compliant,
        -> hex encodings"""
        return int(value,10)
    def __str__(self):
        return "OMint("+repr(self.value)+")"
    

class OMfloat(SimpleValue):
    def __init__(self,value):
        super(OMfloat,self).__init__(value)
    def parse(self, value):
        """FIXME: Not fully standard compliant,
        -> hex encodings"""
        return float(value)
    def __str__(self):
        return "OMfloat("+repr(self.value)+")"
        
if __name__=='__main__':
    from context import *

    from binding import *

    context=Context()

    context.push({})

    context["x"]=OMint(1)

    x=OMvar("x")

    y=OMvar("y")

    print context.evaluate(x)
    print context.evaluate(y)
    firstArg=OMbinding(lambdasym,[OMvar("x"), OMvar("y")], OMvar("x"))
    #print context.evaluate(firstArg)
    application=OMapplication(firstArg, [x,y])
    print context.evaluate(application)
    application=OMapplication(firstArg,[y,x])
    print context.evaluate(application)
    import arith1
    context.addCDImplementation(arith1.implementation)
    #print type(context.lookupImplementation(arith1.plussym))
    #application=OMapplication(arith1.plussym,[x])
    #application=OMapplication(arith1.plussym,[x,x])
    application=OMapplication(OMsymbol("plus",arith1.content),[x,x])
    
    print context.evaluate(application)
    application=OMapplication(OMsymbol("plus",arith1.content),[x,x,x])
    
    print context.evaluate(application)
    
   