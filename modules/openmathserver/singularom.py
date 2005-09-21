from Singular import *
from interpreter import *
from objects import *
from polyd import DMPsym, SDMPsym, termsym, lpsym,dpsym,poly_ring_dsym, DMPLsym
from fieldname1 import Qsym as Rationals
singular=singular_globals_proxy()
class SingularException(Exception):
  pass

def encodePoly(p):
  terms=[encodeTerm(t) for t in p]
  return OMApply(SDMPsym,terms)

def encodePolyWithRing(p):
  pe=encodePoly(p)
  r=encodeRing(p.ring())
  return OMApply(DMPsym,[r,pe])
  
  
orderingTable={
  "lp": lpsym,
  "dp": dpsym
}
def encodeOrdering(r):
  rl=singular.ringlist(r)
  return orderingTable[rl[2][0][0]]

def encodeField(r):
  char=singular.char(r)
  if char==0 and singular.npars(r)==0:
    return Rationals
  else:
    raise SingularException("unknown field to encode")

def encodeIdeal(i):
  """FIXME: uses only currentRing"""
  r=encodeRing(i.ring())
  return OMApply(DMPLsym,[r]+[encodePoly(p) for p in i])
def encodeRing(r):
    nv=singular.nvars(r)
    f=encodeField(r)
    return OMApply(poly_ring_dsym,[f,OMint(nv)])   
  
def encodeTerm(t):
  """FIXME: ugly because it uses slow interpreter interface and setting of rings for this should be automatically"""
  t.ring().set()
  exponents=singular.leadexp(t)
  c=singular.leadcoef(t)
  exponents=[OMint(i) for i in exponents]
  return OMApply(termsym,[OMint(str(c))]+exponents)