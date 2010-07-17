import unittest
from objects import *
from context import Context
import lxmlencoder
import lxmldecoder

from cd.arith1 import implementation as arith1

class OMTestCase1(unittest.TestCase):
    def assertEncodeDecodeOK(self, expr):
        orig=repr(expr)
        new=repr(lxmldecoder.decode(lxmlencoder.encode(expr)))
        self.assertEqual(new,orig)
    def testOMI(self):
        self.assertEncodeDecodeOK(1)
        self.assertEncodeDecodeOK(1233593883529)
    def testOMA(self):
        self.assertEncodeDecodeOK(OMA(arith1.plus,[OMA(arith1.plus,[1,2]),3]))
        
                
                
if __name__=='__main__':
    context=Context()
    context.addCDImplementation(arith1)
    unittest.main()
