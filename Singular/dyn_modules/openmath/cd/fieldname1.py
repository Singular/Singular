#from binding import *
#from context import *
from objects import *
#from cd import *
#from omexceptions import *

content=OMCD("fieldname1")
implementation=OMCDImplementation(content)

Qsym=OMS("Q",content)
implementation.support("Q")