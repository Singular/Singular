import unittest
from objects import *
from context import Context

from cd.arith1 import implementation as arith1

class OMTestCase1(unittest.TestCase):
            def assertResult(self,expression, result):
                self.assertEqual(context.evaluate(expression),result)
            def testAddition(self):
                expr=OMA(arith1.plus,[2,3])
                self.assertEqual(len(expr.args),2)
                self.assertResult(expr,5)
            def testMultiplication(self):
                expr=OMA(arith1.times,[4,3])
                self.assertResult(expr,12)
            def testPower(self):
                expr=OMA(arith1.power,[2,3])
                self.assertResult(expr,8)
            def testAbs(self):
                expr=OMA(arith1.abs,[-349])
                self.assertResult(expr,349)
            #def testSub(self):
            #    expr=OMA(arith1.minus,[2,3])
            #    self.assertResult(expr,-1)
                
                
if __name__=='__main__':
    context=Context()
    context.addCDImplementation(arith1)
    unittest.main()
