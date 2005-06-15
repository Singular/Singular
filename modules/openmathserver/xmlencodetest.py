from objects import *
from context import *
import arith1
context=Context()
context.push({})
i=OMint(22482489)
#print i.body
print i.XMLencode(context)
print context.XMLEncodeObject(i)
v=OMVar("x")
print v.XMLencode(context)
x=v
y=OMVar("y")
a=OMapplication(OMSymbol("plus",arith1.content),[x,y,i])
print a.XMLencode(context)
print context.XMLEncodeObject(context.evaluate(a))
a1=OMapplication(OMSymbol("plus",arith1.content),[OMint(1),OMint(2)])
a2=OMapplication(OMSymbol("plus",arith1.content),[a1,OMint(2)])
a3=OMapplication(OMSymbol("plus",arith1.content),[a2,x])
context.addCDImplementation(arith1.implementation)
print "a3", context.XMLEncodeObject(context.evaluate(a3))
print "a2", context.XMLEncodeObject(context.evaluate(a2))
print "a1", context.XMLEncodeObject(context.evaluate(a1))

f=OMfloat(1.23434)
print context.XMLEncodeObject(f)
#print v.XMLattributes

