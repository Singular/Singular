"""Implementation of openmath basic objects"""
from omexceptions import UnsupportedOperationError 
from  omexceptions import OutOfScopeError, EvaluationFailedError
from exceptions import NotImplementedError
from copy import copy
from xml.sax.xmlreader import AttributesImpl as attr
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

class XMLAttribute(object):
    def __init__(self, name, value):
        self.name = name
        self.value = value
    def encode(self, context):
        return "".join([self.name,"=\"",self.value,"\""])
class OMObjectBase(object):
    """ at the moment only a base class"""
    #def __init__(self):
    #    self.attributes={}
    attributes={}
    #dangerous, if you change attributes, always copy attributes
    
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
    def __getXMLAttributes(self):
        if hasattr(self, "getXMLAttributes"):
            return self.getXMLAttributes()
        if hasattr(self,"__XMLAttributes"):
            return self.__XMLAttributes
        return {}
            #except AttributeError:
                #do return None, cause if modifiying a new list, changes will not be saved
            #    return {}
    def __delXMLAttributes(self):
        try:
            self.delXMLAttributes()
            return
        except AttributeError:
            try:
                del self.__XMLAttributes
            except AttributeError:
                pass
    def __setXMLAttributes(self,XMLAttributes):
        try:
            self.setBody(XMLAttributes)
        except AttributeError:
            raise UnsupportedOperationError
    children=property(__getChildren, __setChildren,__delChildren,\
                      """ children in an OMtree""")
    body=property(__getBody,__setBody,__delBody,\
        "xml body,FIXME: at the moment only char data")
    XMLAttributes=property(__getXMLAttributes,\
        __setXMLAttributes,__delXMLAttributes,\
        "xml attributes")
    def XMLEncode(self, context):
        
        attr=self.XMLAttributes
        if attr:
            attrstr = " "+" ".join([a.encode(context) for a in attr])
        else:
            attrstr = ""
        opening = "".join(["<", self.XMLtag, attrstr,">"])
        children = self.children
        if children:
            body = "".join([context.XMLEncodeObject(c) for c in children])
        else:
            body = self.body
            if not body:
                body = ""
            assert body != None
            body = context.XMLEncodeBody(body)
            assert body != None
        closing = "".join(["</"+self.XMLtag+">"])
        return "".join([opening,body,closing])
    def XMLPreEncode(self, context, encodingList=None):
        #works not for attp
        if encodingList==None:
            encodingList=[]
        encodingList.extend(["<", self.XMLtag])
        attr=self.XMLAttributes
        if attr:
            for a in attr:
                encodingList.append(" ")
                encodingList.append(a.encode(context))
        encodingList.append(">")
        children = self.children
        if children:
            for c in children:
                c.XMLPreEncode(context, encodingList)
        else:
            body = self.body
            if body:
                encodingList.append(context.XMLEncodeBody(body))
        encodingList.extend(["</"+self.XMLtag+">"])
        return encodingList
    def XMLSAXEncode(self, context, generator):
        #works not for attp
        #print "attr", self.XMLAttributes
        generator.startElement(self.XMLtag, attr(self.XMLAttributes))
        for c in self.children:
          c.XMLSAXEncode(context, generator)
        if not self.children:
          body=self.body
          if body:
            generator.characters(self.body)
        generator.endElement(self.XMLtag)
class OMObject(OMObjectBase):
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
class OMVar(OMObjectBase):
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
    XMLtag = "OMV"
    def getXMLAttributes(self):
        return [XMLAttribute("name", self.name)]
        
class OMApply(OMObjectBase):
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
					return OMApply(efunc, eargs)
                #return self
        else:
            #print efunc
            return OMApply(efunc, eargs)
    XMLtag="OMA"
    def getChildren(self):
        return [self.func]+self.args
    def setChildren(self):
        raise UnsupportedOperationError
        
class OMSymbol(OMObjectBase):
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
class SimpleValue(OMObjectBase):
    def __init__(self, value):
        #super(SimpleValue, self).__init__()
        if (isinstance(value, str)):
            value = self.parse(value)
        self.value = value
    def evaluate(self, context):
        return self
    def getValue(self):
        return self.value
    def __str__(self):
        return "OM("+repr(self.value)+")"

class OMint(SimpleValue):
    def __init__(self, value):
        if not isinstance(value, int):
            value = self.parse(value)
        #super(OMint, self).__init__(value)
        self.value=value
    def parse(self, value):
        """FIXME: Not fully standard compliant,
        -> hex encodings"""
        return int(value, 10)
    def __str__(self):
        return "OMint("+repr(self.value)+")"
    def getBody(self):
        return str(self.value)
    def setBody(self, value):
        raise UnsupportedOperationError
    XMLtag = "OMI"
class OMfloat(SimpleValue):
    def __init__(self, value):
        #super(OMfloat, self).__init__(value)
        self.value=value
    def parse(self, value):
        """FIXME: Not fully standard compliant,
        -> hex encodings"""
        return float(value)
    def __str__(self):
        return "OMfloat("+repr(self.value)+")"
    XMLtag = "OMF"
    
    def getXMLAttributes(self):
        return {"dec":str(self.value)}
class OMString(SimpleValue):
    def __init__(self,value):
        #super(OMString,self).__init__(value)
        self.value=value
    def __str__(self):
        return "OMSTR("+repr(self.value)+")"
    XMLtag = "OMSTR"
    def getBody(self):
        return self.value
class OMByteArray(SimpleValue):
    def __init__(self, value):
        #super(OMByteArray,self).__init__(value)
        self.value=value
    def __str__(self):
        return "OMByteArray(" + repr(self.value) + ")"
    def parse(self, value):
        return value
    XMLtag = "OMB"
    def getBody(self):
        return base64.encodestring(self.value)
class OMRef(OMObjectBase):
    def __init__(self, ref):
        self.ref=ref
    def evaluate(self, context):
        return context.evaluate(self.ref)
    def XMLEncode(self, context):
        "FIXME: maybe it should also be able to encode as reference"
        return context.XMLEncodeObject(self.ref)
class OMAttributePair(OMObjectBase):
    def __init__(self, key, value):
        super(OMAttributePair,self).__init__()
        self.key = key
        self.value = value
    def getChildren(self):
        return [self.key, self.value]
    XMLtag = "OMATP"
    def evaluate(self, context):
        return OMAttributePair(context.evaluate(self.key),\
            context.evaluate(self.value))
class OMAttribution(OMObjectBase):
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
    from context import Context

    from binding import OMBinding, lambdasym

    context = Context()

    context.push({})

    context["x"] = OMint(1)

    x = OMVar("x")

    y = OMVar("y")

    print context.evaluate(x)
    print context.evaluate(y)
    firstArg=OMBinding(lambdasym,[OMVar("x"), OMVar("y")], OMVar("x"))
    #print context.evaluate(firstArg)
    application = OMApply(firstArg, [x,y])
    print context.evaluate(application)
    application = OMApply(firstArg, [y,x])
    print context.evaluate(application)
    import CD.arith1 as arith1
    context.addCDImplementation(arith1.implementation)
    #print type(context.lookupImplementation(arith1.plussym))
    #application=OMApply(arith1.plussym,[x])
    #application=OMApply(arith1.plussym,[x,x])
    application = OMApply(OMSymbol("plus", arith1.content), [x, x])
    
    print context.evaluate(application)
    application = OMApply(OMSymbol("plus", arith1.content), [x, x, x])
    
    print context.evaluate(application)
    i =  OMint(22482489)
    print i.body
    print i.XMLEncode(context)
    print context.XMLEncodeObject(OMSymbol("plus", arith1.content))
    #i.body="dshj"
   
#optimize(OMObjectBase.__init__)
optimize(OMObjectBase.XMLPreEncode)
optimize(SimpleValue.__init__)
optimize(OMint.__init__)
optimize(OMApply.__init__)