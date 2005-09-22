"Implementation of arith1 content dictionary"
from objects import OMSymbol
from cd import OMCD, OMCDImplementation
#from omexceptions import *

content = OMCD("arith1")
implementation = OMCDImplementation(content)

plussym = OMSymbol("plus", content)

def plusfunc(context, *args):
    "tries, convert to generic and adding"
    args = [context.toGeneric(omobj) for omobj in args]
    if len(args) == 0:
        return content.package(0)
    else:
        erg = args[0]
        for i in xrange(1, len(args)):
            erg = erg+args[i]
    return erg
implementation.implement("plus", plusfunc)
