from objects import *
from context import *
context=Context()
context.push({})
i=OMint(22482489)
print i.body
print i.XMLencode(context)
v=OMvar("x")
print v.XMLencode(context)
print v.XMLattributes