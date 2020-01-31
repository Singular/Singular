from probstat import Cartesian
from itertools import repeat
from util import EquivalenceRelation
from interpreter import *
singular=SingularGlobalsProxy()
from util import *
from Singular import *

singular_var=singular.var

class modPNumber:
  """simple class for Turaev/Viro colors,
    each color has an inverse, but it doesn't have to be an algebraic
    structure"""
  def __init__(self,i,n):
    self.val=i%n
    self.mod=n
  def __neg__(self):
    return modPNumber((-self.val)%self.mod,self.mod)
  def __hash__(self):
    return hash((self.val, self.mod))
  def __eq__(self, other):
    return self.val==other.val
  def __repr__(self):
    return repr(self.val)
  def __str__(self):
    return str(self.val)

colors=3
value_range=[modPNumber(i,colors) for i in range(colors)]

wrel=EquivalenceRelation()
for i in value_range:
  wrel[i]=-i
weights_canonical_to_index=dict((weight,i+1) \
    for (i,weight) in enumerate(filter( \
        wrel.isCanonical,value_range)))
wtrans=dict((weight,weights_canonical_to_index[wrel.canonical(weight)]) for weight in \
    value_range)
weights=len(weights_canonical_to_index)


def constr_variations():
  return list(map(tuple,Cartesian(list(repeat(value_range,6)))))
relation=EquivalenceRelation()
for (a,b,c,d,e,f) in constr_variations():
  relation[(a,b,c,d,e,f)]=(b,c,a,f,-d,-e)
  relation[(a,b,c,d,e,f)]=(a,-d,-e,-b,-c,-f)
canonical_to_index=dict((symbol,weights+i+1) for (i,symbol) in \
    enumerate(filter( \
       relation.isCanonical,constr_variations())))
vartrans=dict((symbol,canonical_to_index[relation.canonical(symbol)]) for symbol in \
    constr_variations())
symbols=len(canonical_to_index)

def six_j_symbol(v):
  i=vartrans[v]
  return var_cache[i-1]


def w(i):
  return var_cache[wtrans[i]-1]
r=create_ring(char=0,nvars=weights+symbols)
r.set()
print(r)
myideal=Ideal()
def polysum(l):
  acc=Polynomial(0)
  for p in l:
    acc+=p
  return acc

var_cache=[singular_var(x+1) for x in range(singular.nvars(Ring()))]
for (j1,j2,j3,j4,j5,j6,j7,j8,j9) in Cartesian(list(repeat(value_range,9))):
  p=\
    six_j_symbol((j1,j2,j3,j7,j8,j9))*\
    six_j_symbol((j4,j5,j6,-j7,-j8,-j9))+\
    Number(-1)*\
    polysum([\
      w(j)*six_j_symbol((j,j1,j2,-j4,-j5,j7))*\
      six_j_symbol((j,j2,j3,-j5,-j6,j9))*
      six_j_symbol((j,j3,j1,-j6,-j4,-j8))\
      for j in value_range])
  myideal.append(p)

back_table_v=dict((canonical_to_index[s],s) for s\
    in canonical_to_index)
back_table_w=dict((weights_canonical_to_index[w],w) for w\
    in weights_canonical_to_index)
back_table_joint=dict(back_table_v)
back_table_joint.update(back_table_w)
print(("back", back_table_joint))
print(("original length",len(myideal)))
print("now calculating")
myideal=singular.simplify(myideal,4)
print(("simplified", len(myideal)))
gb=singular.slimgb(myideal)
print(gb)
print((len(gb), "GB elements"))
