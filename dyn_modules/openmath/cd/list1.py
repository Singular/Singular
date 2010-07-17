from objects import *
from copy import copy
content=OMCD("list1")
implementation=OMCDImplementation(content)
implementation.support("list")
def list2OM(l):
    return OMA(implementation.list,copy(l))