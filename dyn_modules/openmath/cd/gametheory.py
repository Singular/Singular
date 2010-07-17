from binding import *
from context import *
from objects import *
from cd import *
from omexceptions import *

content=OMCD("gametheory","http://www.singular.uni-kl.de/openmath/cd")
implementation=OMCDImplementation(content)

eatcake_sym=OMSymbol("eatcake", content)
left_wins_sym=OMSymbol("left_wins",content)

def cake_to_OM(region_list):
	args=[]
	for (l,r) in region_list:
		args.append(OMint(l))
		args.append(OMint(r))
	return OMApply(eatcake_sym,args)

