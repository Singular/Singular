from context import Context
from objects import *
context = Context()

#context.push({})
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
def test(expr):
    print "Original: ", expr
    print "Evaluated: ", context.evaluate(expr)
test(OMA(arith1.implementation.plus, [1,2]))
test(OMA(arith1.implementation.unary_minus, [10]))
test(OMA(arith1.implementation.times, [3,4]))
test(OMA(arith1.implementation.abs, [-123]))
print context.evaluate(OMA(arith1.implementation.plus, [1,2]))
application = OMA(OMS("plus", arith1.content), [1,2])

print context.evaluate(application)
firstArg=OMBIND(lambdasym,[OMV("x"), OMV("y")], OMV("x"))
#print context.evaluate(firstArg)
test(OMA(firstArg, [37,777777]))

