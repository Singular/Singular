from Singular import *
from interpreter import *
from objects import *
from util import create_ring
import cd.polyd, cd.primdec, cd.list1
from cd.polyd import DMPsym, SDMPsym, termsym, lpsym,dpsym,poly_ring_dsym, DMPLsym, implementation, groebnerdsym
from cd.fieldname1 import Qsym as Rationals
singular=SingularGlobalsProxy()
import cd.primdec
class SingularException(Exception):
  pass

def poly2OM(p):
  terms=[term2OM(t) for t in p]
  return OMA(SDMPsym,terms)

def poly_in_ring2OM(p):
  pe=poly2OM(p)
  r=ring2OM(p.ring())
  return OMA(DMPsym,[r,pe])
  
def gb2OM(gb):
  i=ideal2OM(gb)
  o=ordering2OM(gb.ring())
  return OMA(groebnerdsym,[o,i])
orderingTable={
  "lp": lpsym,
  "dp": dpsym
}
OrderingTableBack={}
for k in orderingTable:
  OrderingTableBack[orderingTable[k]]=k

def ordering2OM(r):
  rl=singular.ringlist(r)
  return orderingTable[rl[2][0][0]]

def encode_field(r):
  char=singular.char(r)
  if char==0 and singular.npars(r)==0:
    return Rationals
  else:
    raise SingularException("unknown field to encode")

def ideal2OM(i):
  r=ring2OM(i.ring())
  return OMA(DMPLsym,[r]+[poly2OM(p) for p in i])
def ring2OM(r):
    nv=singular.nvars(r)
    f=encode_field(r)
    return OMA(poly_ring_dsym,[f,nv])   

def OM2ring(ring_desc, ordering="dp"):
  assert isinstance(ring_desc, OMA)
  if (ring_desc.args[0]==Rationals):
    i=ring_desc.args[1]
    return create_ring(char=0, nvars=i, ordering=ordering)
 
  raise SingularException("ring not supported")
def OM2poly(poly_desc):
  """assumes the right ring is set"""
  assert isinstance(poly_desc, OMA)
  terms=[OM2term(t) for t in poly_desc.args]
  res=Polynomial(Number(0))
  for t in terms:
    res=res+t
    #res+=t
  return res
def OM2term(term_desc):
  assert isinstance(term_desc, OMA)
  assert len(term_desc.args)==singular.nvars(Ring())+1
  assert isinstance(term_desc.args[0], int)
  
  coef=Number(term_desc.args[0])
  exp=IntVector()
  for e in term_desc.args[1:]:
    assert isinstance(e,int)
    exp.append(e)
  #print coef, polynomial(exp)
  return coef*Polynomial(exp)

def OM2ideal_raw(dmpl):
  """assumes that the right ring is set"""
  i=Ideal()
  assert len(dmpl.args)>=1
  ps=[OM2poly(d) for d in dmpl.args[1:]]
  for p in ps:
    i.append(p)
  return i
def OM2ideal(dmpl,o):
  assert len(dmpl.args)>=1
  return OM2ring(dmpl.args[0], ordering=OrderingTableBack[o])

leadcoef=singular.leadcoef
leadexp=singular.leadexp
def term2OM(t):
  """FIXME: ugly because it uses slow interpreter interface and setting of rings for this should be automatically"""
  #t.ring().set()
  #exponents=leadexp(t)
  #c=leadcoef(t)
  exponents=t.leadExp()
  c=t.leadCoef()
  exponents=[i for i in exponents]
  return OMA(termsym,[int(str(c))]+exponents)
def groebnerfunc(ordering, dmpl):

  r=OM2ring(dmpl.args[0], OrderingTableBack[ordering])
  r.set()
  i=OM2ideal_raw(dmpl)
  res=singular.groebner(i)
  #FIXME: singular.groebner does not work may because of bad ring changes
  #FIXME: increase reference to ring in ideal, polynomial class
  return gb2OM(res)
implementation.groebner=groebnerfunc

def dmplQ(a):
    if isinstance(a,OMA) and a.func==DMPLsym:
        return True
    else:
        return False


def input_convert(f):
    def my2OM(a):
        """FIXME: very dirty"""
        global save_ring


        if (dmplQ(a)):
            dmpl=a
            r=OM2ring(dmpl.args[0])
            r.set()
            i=OM2ideal_raw(dmpl)
            save_ring=r
            return i
        
        return a
            
    def wrapper(*args):
        return f(*[my2OM(a) for a in args])
    
    
    wrapper.__name__=f.__name__
    return wrapper
    
def output_convert(f):
    def my2om(a):
        """FIXME: very dirty"""
        global save_ring

        if isinstance(a,Ideal):
            return ideal2OM(a)
        if (isinstance(a,list)):
            return cd.list1.list2OM([my2om(a2) for a2 in a])
        
        return a
            
    def wrapper(*args):
        return my2om(f(*args))
    
    
    wrapper.__name__=f.__name__
    return wrapper
    
def min_ass_func(dmpl):
    r=OM2ring(dmpl.args[0])
    r.set()
    i=OM2ideal_raw(dmpl)
    l=singular.minAssGTZ(i)
    return cd.list1.OM2list([ideal2OM(i) for i in l])
@input_convert
def min_ass_func2(i):

    l=singular.minAssGTZ(i)
    return cd.list1.list2OM([ideal2OM(i) for i in l])
    

def singular_default_converter(f):
    return output_convert(input_convert(f))
cd.primdec.implementation.converter=singular_default_converter
#cd.primdec.implementation.minAss=min_ass_func2
cd.primdec.implementation.minAss=singular.minAssGTZ#input_convert(output_convert(singular.minAssGTZ))

cd.primdec.implementation.minAssGTZ=singular.minAssGTZ
#input_convert(output_convert(singular.minAssGTZ))

cd.primdec.implementation.primdecGTZ=singular.primdecGTZ#input_convert(output_convert(singular.primdecGTZ))
cd.primdec.implementation.radical=singular.radical

optimize(poly2OM)
optimize(term2OM)
