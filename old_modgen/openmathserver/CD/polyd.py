from binding import *
from context import *
from objects import *
from cd import *
from omexceptions import *

content=OMCD("polyd")
implementation=OMCDImplementation(content)

groebnersym=OMSymbol("groebner",content)
DMPsym=OMSymbol("DMP",content)
DMPLsym=OMSymbol("DMPL",content)
SDMPsym=OMSymbol("SDMP",content)
termsym=OMSymbol("term", content)
dpsym=OMSymbol("graded_reverse_lexicographic", content)
lpsym=OMSymbol("lexicographic", content)
groebnerdsym=OMSymbol("groebnerd", content)

poly_ring_dsym=OMSymbol("poly_ring_d",content)
#def groebnerfunc(context, *args):
#    #print "plusfunc", args
#    args=[context.toGeneric(o) for o in args]
#    if len(args)==0:
#        return content.package(0)
#    else:
#        erg=args[0]
#        for i in xrange(1,len(args)):
#            erg=erg+args[i]
#    return erg
