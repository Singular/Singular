#from binding import *
#from context import *
from objects import *
#from cd import *
from omexceptions import *

content=OMCD("polyd")
implementation=OMCDImplementation(content)

groebner=OMS("groebner",content)
#implementation.support("groebner")
implementation.support("DMP")
implementation.support("DMPL")
implementation.support("SDMP")
implementation.support("term")
implementation.support("graded_reverse_lexicographic")
implementation.support("lexicographic")
implementation.support("poly_ring_d")
implementation.support("groebnerd")
DMPsym=OMS("DMP",content)
DMPLsym=OMS("DMPL",content)
SDMPsym=OMS("SDMP",content)
termsym=OMS("term", content)
dpsym=OMS("graded_reverse_lexicographic", content)
lpsym=OMS("lexicographic", content)
groebnersym=OMS("groebner", content)
groebnerdsym=OMS("groebnerd", content)
poly_ring_dsym=OMS("poly_ring_d",content)
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
