from binding import *
from context import *
from objects import *
from cd import *
from omexceptions import *

content=OMCD("arith1")
implementation=OMCDImplementation(content)

plussym=OMSymbol("plus",content)

def plusfunc(context, *args):
    #print "plusfunc", args
    args=[context.toGeneric(o) for o in args]
    if len(args)==0:
        return content.package(0)
    else:
        erg=args[0]
        for i in xrange(1,len(args)):
            erg=erg+args[i]
    return erg
implementation.implement("plus", plusfunc)
#print implementation[sym]
#print plussym
#print sym==plussym
#print implementation[plussym]
#plusimpl=ImplementedOMSymbol(plussym,plusfunc)
#implementation[plussym]=plusimpl
#print "Laenge", (len(implementation.implementations))
#assert OMSymbol("plus",content)==OMSymbol("plus",content)
