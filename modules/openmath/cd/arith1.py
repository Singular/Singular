"Implementation of arith1 content dictionary"
from objects import OMS,OMCD, OMCDImplementation
from operator import add, sub, mul, div, neg, abs
content = OMCD("arith1")
implementation = OMCDImplementation(content)
#implementation.divide=div no integer division allowed
implementation.plus=add
implementation.times=mul

implementation.power=pow
implementation.unary_minus=neg
implementation.abs=abs