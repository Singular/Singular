from Singular import *
from interpreter import *
from objects import *
from util import create_ring
import polyd
from polyd import DMPsym, SDMPsym, termsym, lpsym,dpsym,poly_ring_dsym, DMPLsym, implementation, groebnerdsym
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
  
def encodeGB(gb):
  i=encodeIdeal(gb)
  o=encodeOrdering(gb.ring())
  return OMApply(groebnerdsym,[o,i])
orderingTable={
  "lp": lpsym,
  "dp": dpsym
}
OrderingTableBack={}
for k in orderingTable:
  OrderingTableBack[orderingTable[k]]=k
print OrderingTableBack
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
  r=encodeRing(i.ring())
  return OMApply(DMPLsym,[r]+[encodePoly(p) for p in i])
def encodeRing(r):
    nv=singular.nvars(r)
    f=encodeField(r)
    return OMApply(poly_ring_dsym,[f,OMint(nv)])   

def ringFromOM(ring_desc):
  if not isinstance(ring_desc, OMApply):
    raise SingularException("ringFromOM expects instance of OMApply")
  if (ring_desc.args[0]==Rationals):
    i=ring_desc.args[1].getValue()
    return create_ring(char=0, nvars=i)
  raise SingularException("ring not supported")
def encodeTerm(t):
  """FIXME: ugly because it uses slow interpreter interface and setting of rings for this should be automatically"""
  t.ring().set()
  exponents=singular.leadexp(t)
  c=singular.leadcoef(t)
  exponents=[OMint(i) for i in exponents]
  return OMApply(termsym,[OMint(str(c))]+exponents)