from Singular import *
from interpreter import *
from objects import *
from polyd import DMPsym, SDMPsym, termsym
singular=singular_globals_proxy()
def encodePoly(p):
  terms=[encodeTerm(t) for t in p]
  return OMApply(SDMPsym,terms)

def encodeTerm(t):
  """FIXME: ugly because it uses slow interpreter interface"""
  exponents=singular.leadexp(t)
  c=singular.leadcoef(t)
  exponents=[OMint(i) for i in exponents]
  return OMApply(termsym,[OMint(str(c))]+exponents)