from objects import *
from context import *
import CD.arith1 as arith1
context=Context()
context.push({})
i=OMint(22482489)
#print i.body
print i.XMLEncode(context)
print context.XMLEncodeObject(i)
v=OMVar("x")
print v.XMLEncode(context)
x=v
y=OMVar("y")
a=OMApply(OMSymbol("plus",arith1.content),[x,y,i])
print a.XMLEncode(context)
print context.XMLEncodeObject(context.evaluate(a))
a1=OMApply(OMSymbol("plus",arith1.content),[OMint(1),OMint(2)])
a2=OMApply(OMSymbol("plus",arith1.content),[a1,OMint(2)])
a3=OMApply(OMSymbol("plus",arith1.content),[a2,x])
context.addCDImplementation(arith1.implementation)
print "a3", context.XMLEncodeObject(context.evaluate(a3))
print "a2", context.XMLEncodeObject(context.evaluate(a2))
print "a1", context.XMLEncodeObject(context.evaluate(a1))

f=OMfloat(1.23434)
print context.XMLEncodeObject(f)
#print v.XMLAttributes

