from Singular import *
from interpreter import *
from objects import *
from util import create_ring
import CD.polyd
from CD.polyd import DMPsym, SDMPsym, termsym, lpsym,dpsym,poly_ring_dsym, DMPLsym, implementation, groebnerdsym
from CD.fieldname1 import Qsym as Rationals
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

def ringFromOM(ring_desc, ordering="dp"):
  assert isinstance(ring_desc, OMApply)
  if (ring_desc.args[0]==Rationals):
    i=ring_desc.args[1].getValue()
    return create_ring(char=0, nvars=i, ordering=ordering)
  raise SingularException("ring not supported")
def polyFromOM(poly_desc):
  """assumes the right ring is set"""
  assert isinstance(poly_desc, OMApply)
  terms=[termFromOM(t) for t in poly_desc.args]
  res=polynomial(number(0))
  for t in terms:
    res=res+t
    #res+=t
  return res
def termFromOM(term_desc):
  assert isinstance(term_desc, OMApply)
  assert len(term_desc.args)==singular.nvars(ring())+1
  assert isinstance(term_desc.args[0], OMint)
  
  coef=number(term_desc.args[0].getValue())
  exp=intvec()
  for e in term_desc.args[1:]:
    assert isinstance(e,OMint)
    exp.append(e.getValue())
  #print coef, polynomial(exp)
  return coef*polynomial(exp)

def idealFromDMPL(dmpl):
  """assumes that the right ring is set"""
  i=ideal()
  assert len(dmpl.args)>=1
  ps=[polyFromOM(d) for d in dmpl.args[1:]]
  for p in ps:
    i.append(p)
  return i
def ringFromDMPLOrd(dmpl,o):
  assert len(dmpl.args)>=1
  return ringFromOM(dmpl.args[0], ordering=OrderingTableBack[o])
leadcoef=singular.leadcoef
leadexp=singular.leadexp
def encodeTerm(t):
  """FIXME: ugly because it uses slow interpreter interface and setting of rings for this should be automatically"""
  #t.ring().set()
  #exponents=leadexp(t)
  #c=leadcoef(t)
  exponents=t.leadExp()
  c=t.leadCoef()
  exponents=[OMint(i) for i in exponents]
  return OMApply(termsym,[OMint(str(c))]+exponents)
def groebnerfunc(context, ordering, dmpl):
  r=ringFromDMPLOrd(dmpl,ordering)
  r.set()
  i=idealFromDMPL(dmpl)
  res=singular.std(i)
  #FIXME: singular.groebner does not work may because of bad ring changes
  return encodeGB(res)
implementation.implement("groebner", groebnerfunc)
optimize(encodePoly)
optimize(encodeTerm)
