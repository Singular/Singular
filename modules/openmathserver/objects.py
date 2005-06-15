from omexceptions import *
from exceptions import *
#from cd import *
class XMLattribute(object):
    def __init__(self, name, value):
        self.name=name
        self.value=value
    def encode(self, context):
        return "".join([self.name,"=\"",self.value,"\""])
class OMObjectBase(object):
    """ at the moment only a base class"""
    def __init__(self):
        self.attributes={}
    def __getChildren(self):
        try:
            return self.getChildren()
        except AttributeError:
            try:
                return self.__children
            except AttributeError:
                return []
    def __delChildren(self):
        try:
            self.delChildren()
            return
        except AttributeError:
            try:
                del self.__children
            except AttributeError:
                pass
    def __setChildren(self,children):
        try:
            self.setChildren(children)
        except AttributeError:
                raise UnsupportedOperationError
    def __getBody(self):
        try:
            return self.getBody()
        except AttributeError:
            try:
                return self.__body
            except AttributeError:
                return None
    def __delBody(self):
        try:
            self.delBody()
            return
        except AttributeError:
            try:
                del self.__body
            except AttributeError:
                pass
    def __setBody(self,body):
        try:
            self.setBody(body)
        except AttributeError:
                raise UnsupportedOperationError
    def __getXMLattributes(self):
        try:
            return self.getXMLattributes()
        except AttributeError:
            try:
                return self.__XMLattributes
            except AttributeError:
                #do return None, cause if modifiying a new list, changes will not be saved
                return []
    def __delXMLattributes(self):
        try:
            self.delXMLattributes()
            return
        except AttributeError:
            try:
                del self.__XMLattributes
            except AttributeError:
                pass
    def __setXMLattributes(self,XMLattributes):
        try:
            self.setBody(XMLattributes)
        except AttributeError:
            raise UnsupportedOperationError
    children=property(__getChildren, __setChildren,__delChildren,\
                      """ children in an OMtree""")
    body=property(__getBody,__setBody,__delBody,\
        "xml body,FIXME: at the moment only char data")
    XMLattributes=property(__getXMLattributes,__setXMLattributes,__delXMLattributes,\
        "xml attributes")
    def XMLencode(self, context):
        
        attr=self.XMLattributes
        if attr:
            attrstr=" "+" ".join([a.encode(context) for a in attr])
        else:
            attrstr=""
        opening="".join(["<", self.XMLtag, attrstr,">"])
        children=self.children
        if children:
            body="".join([context.XMLEncodeObject(c) for c in children])
        else:
            body=self.body
            if not body:
                body=""
            assert body!=None
            body=context.XMLEncodeBody(body)
            assert body!=None
        closing="".join(["</"+self.XMLtag+">"])
        return "".join([opening,body,closing])
class OMvar(OMObjectBase):
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
    XMLtag="OMV"
    def getXMLattributes(self):
        return [XMLattribute("name", self.name)]
        
class OMapplication(OMObjectBase):
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
                return OMapplication(efunc, eargs)
                #return self
        else:
            return OMapplication(efunc, eargs)
    XMLtag="OMA"
    def getChildren(self):
        return [self.func]+self.args
    def setChildren(self):
        raise UnsupportedOperationError
        
class OMsymbol(OMObjectBase):
    def __init__(self,name,cd=None):
        super(OMsymbol,self).__init__()
        self.cd=cd
        self.name=name
    def __eq__(self, other):
        try:
            return bool(other.name==self.name and self.cd==other.cd)
        except:
            return False
    def __str__(self):
        return "OMS("+self.name+", "+self.cd.name + ")"
    def __hash__(self):#
        return hash((self.name,self.cd.__hash__()))
    def evaluate(self,context):
        return context.evaluateSymbol(self)
    XMLtag="OMS"
    def getXMLattributes(self):
        return [XMLattribute("name", self.name),\
                 XMLattribute("cdbase",self.cd.base),\
                 XMLattribute("cd",self.cd.name)]
    def setXMLattributes(self):
        raise UnsupportedOperationError
class SimpleValue(OMObjectBase):
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
        if not isinstance(value,int):
            value=self.parse(value)
        super(OMint,self).__init__(value)
    def parse(self,value):
        """FIXME: Not fully standard compliant,
        -> hex encodings"""
        return int(value,10)
    def __str__(self):
        return "OMint("+repr(self.value)+")"
    def getBody(self):
        return str(self.value)
    def setBody(self, value):
        raise UnsupportedOperationError
    XMLtag="OMI"
class OMfloat(SimpleValue):
    def __init__(self,value):
        super(OMfloat,self).__init__(value)
    def parse(self, value):
        """FIXME: Not fully standard compliant,
        -> hex encodings"""
        return float(value)
    def __str__(self):
        return "OMfloat("+repr(self.value)+")"
    XMLtag="OMF"
    def getXMLattributes(self):
        return [XMLattribute("dec",str(self.value))]
class OMref(OMObjectBase):
    def __init__(self, ref):
        self.ref=ref
    def evaluate(self, context):
        return context.evaluate(self.ref)
    def XMLencode(self, context):
        "FIXME: maybe it should also be able to encode as reference"
        return context.XMLEncodeObject(self.ref)
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
    i=OMint(22482489)
    print i.body
    print i.XMLencode(context)
    #i.body="dshj"
   