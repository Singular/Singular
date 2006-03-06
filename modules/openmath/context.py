from exceptions import *
from copy import copy
#from cd import *
from omexceptions import *
from objects import *
from re import compile
import StringIO
from xml.sax.saxutils import XMLGenerator
raiseException=True
class Context(object):
    #TODO: Referenzen durch scope richtig behandeln
    def __init__(self):
        self.scope=Scope()
        self.scope.push({})
        self.implementations={}
        self.XMLEncoder=SimpleXMLEncoder()
        self.errorHandler=SimpleErrorHandler()
    def addCDImplementation(self, implementation):
        self.implementations[implementation.cd]=implementation
    def lookupImplementation(self, oms):
        #print self.implementations
        try:
            
            return getattr(self.implementations[oms.cd],oms.name)
        except ValueError:
            raise NotImplementedError
    def __getitem__(self,itemname):
        return self.scope[itemname]
    def __setitem__(self,itemname, item):
        self.scope[itemname]=item
    def push(self, dict):
        """push a lexical context in form of a dictionary"""
        self.scope.push(dict)
    def pop(self, dict):
        """pop a lexical context"""
        return self.scope.pop(dict)
    def scopeFromCurrentScope(self):
        """returns a new Scope object, sharing the dictionaries,
           which will represent the current scope"""
        return self.scope.derriveScope()
    def toGeneric(self, o):
        return o.getValue()

    def evaluateSymbol(self, oms):
        try:
            impl=self.lookupImplementation(oms)
            #if len(oms.attributes)>0:
            #    impl=copy(impl)
            #    impl.attributes=copy(oms.attributes)
            return impl
        except NotImplementedError:
            #TODO log this, report it
            return oms
    def evaluateVariable(self, omv):
        try:
            return self[omv.name]
        except OutOfScopeError:
            return omv
    def evaluateApplication(self, app):
        efunc = self.evaluate(app.func)
        eargs = [self.evaluate(a) for a in app.args]
        if callable(efunc):
            if raiseException:
                return self.apply(efunc,eargs)
            else:
                try:
                    return self.apply(efunc, eargs)
                except EvaluationFailedError, NotImplementedError:
                    return OMA(efunc, eargs)
                #return self
        else:
            #print efunc
            return OMA(efunc, eargs)
    def evaluate(self,omobject):
        
        if isinstance(omobject,OMV):
           
            return self.evaluateVariable(omobject)
        if isinstance(omobject,OMS):
            return self.evaluateSymbol(omobject)
        if isinstance(omobject,OMA):
            return self.evaluateApplication(omobject)
        if isinstance(omobject,OMBIND):
            return omobject.evaluate(self)
        return omobject
    def evaluateInScope(self,omobject, scope):
        bak=self.scope
        self.scope=scope
        erg=self.evaluate(omobject)
        self.scope=bak
        #print "my erg is", erg
        return erg
    def package(self, val):
        return val
        if isinstance(val, int):
            return OMint(val)
        if isinstance(val, float):
            return OMfloat(val)
            
            raise NotImplementedError
    def apply(self,func,args):
        try:
            return func(self,*args)
        except:
            from traceback import print_exc
            print_exc()
            raise EvaluationFailedError
    def XMLEncodeBody(self,body):
        return self.XMLEncoder.encode(body)
    def XMLEncodeObject(self, obj):
        #TODO: Make Attribution List attributes
        #TODO: Make all objects __hash__ and __eq__
        out=StringIO.StringIO()
        G=XMLGenerator(out)
        obj.XMLSAXEncode(self,G)
        return out.getvalue()
        if (len(obj.attributes)==0):
            return "".join(obj.XMLPreEncode(self))
        else:
            toencode=copy(obj)
            toencode.attributes={}
            #FIXME: look on order
            attribution=OMAttribution(*([OMAttributePair(k,obj.attributes[k])\
                for k in obj.attributes])+[toencode])
            return attribution.XMLEncode(self)
class SimpleErrorHandler(object):
    def __init__(self):
        super(SimpleErrorHandler,self).__init__()
    def handle_unexpected_symbol(self, symbol):
        pass
    def handle_unsupported_cd(self, symbol):
        pass
    def handle_unexpected_symbol(self, symbol):
        pass 
class SimpleXMLEncoder(object):
    def __init__(self):
      self.re_inner=compile("&")
      self.re_outer=compile("<")
    def encode(self, string):
        #return sub("<","&lt;",sub("&","&amp;",string))
      return self.re_outer.sub("&lt", self.re_inner.sub("&amp", string)) 
class Scope(object):
    def __init__(self):
        self.dicts=[]
    def push(self, dict):
        self.dicts.append(dict)
    def pop(self):
        return self.dicts.pop()
    def __getitem__(self,itemname):
        i=len(self.dicts)-1
        while i>=0:
            try:
                return self.dicts[i][itemname]
            except KeyError:
                pass
            i=i-1
        raise OutOfScopeError
    def  __setitem__(self, itemname, item):
        try:
            self.dicts[len(self.dicts)-1][itemname]=item
        except IndexError:
            print "scope has no layers"
    def derriveScope(self):
        erg=Scope()
        erg.dicts=copy(self.dicts)
        return erg
