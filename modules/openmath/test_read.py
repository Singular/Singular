import lxmlencoder
import lxmldecoder
import lxml.etree as et
import sys
from cd.arith1 import implementation as arith1
from context import Context

tree=et.parse("bindingbsp")
inp= lxmldecoder.decode(tree)
h=lxmlencoder.encode(inp)
print h.__class__
h.write(open("hallo", "w"))
c=Context()
c.addCDImplementation(arith1)
lxmlencoder.encode(c.evaluate(inp)).write(open("hallo.eval", "w"))