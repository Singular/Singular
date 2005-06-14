from binding import *
from context import *
from objects import *
from cd import *
from omexceptions import *

content=OMcd("arith1")
implementation=OMcdImplementation(content)

plussym=OMsymbol("plus",content)

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
#plusimpl=ImplementedOMsymbol(plussym,plusfunc)
#implementation[plussym]=plusimpl
#print "Laenge", (len(implementation.implementations))
#assert OMsymbol("plus",content)==OMsymbol("plus",content)