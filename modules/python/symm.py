from probstat import Cartesian
from itertools import repeat,imap
class chainmap(object):
  """this class represents an equivalent relation,
    which is implemented by repeated evaluatation of a map
    c[n]=m means that n is equivalent to m"""
  def __init__(self):
    self.mydict={}
  def canonical(self,var):
    while var in self.mydict:
      var=self.mydict[var]
    return var
  def all_rewritings(self,var):
    pass
  def rewrite(self, a,b):
    a=self.canonical(a)
    b=self.canonical(b)    
    if not a==b:
      self.mydict[a]=b
  def __getitem__(self,key):
    return self.canonical(key)
  def __setitem__(self, val, key):
    self.rewrite(val,key)
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
def constr_value_range(n):
  return [modPNumber(i,n) for i in xrange(n)]
value_range=constr_value_range(colors)

chain=chainmap()
l12=[1,2]

def constr_variations():
  return imap(tuple,Cartesian(list(repeat(value_range,6))))
for (a,b,c,d,e,f) in constr_variations():
  chain[(a,b,c,d,e,f)]=(b,c,a,f,-d,-e)
  chain[(a,b,c,d,e,f)]=(a,-d,-e,-b,-c,-f)

class chaintrans(object):
  def __init__(self, chain,full):
    self.table={}
    self.i=1
    full=list(full)
    for c in full:
      if not c in chain.mydict:
        self.insert(c)
    for c in full:
      self.table[c]=self.table[chain[c]]
  def __getitem__(self, key):
    return self.table[key]
  def insert(self,key):
    self.table[key]=self.i
    self.i=self.i+1
  def __str__(self):
    return str(self.table)



vartrans=chaintrans(chain, constr_variations())
#we only needed this class for construction, can replace it by a function
vartrans=vartrans.table
del chain

#weights=2

from interpreter import *
singular=SingularGlobalsProxy()
from util import *
from Singular import *
singular_var=singular.var

def singvar(v):
  i=vartrans[v]+weights
  return vartable[i-1]
  
wmap=chainmap()
for i in value_range:
  wmap[i]=-i
wtrans=chaintrans(wmap,value_range)
wtrans=wtrans.table
symbols=max([vartrans[i] for i in vartrans])
weights=max([wtrans[i] for i in wtrans])

del wmap

def w(i):
  return vartable[wtrans[i]-1]
r=create_ring(char=0,nvars=weights+symbols)
r.set()
print r
myideal=Ideal()
def polysum(l):
  acc=Polynomial(0)
  for p in l:
    acc=acc+p

  return acc


vartable=[singular_var(x+1) for x in range(singular.nvars(Ring()))]
for (j1,j2,j3,j4,j5,j6,j7,j8,j9) in Cartesian(list(repeat(value_range,9))):
  p=\
    singvar((j1,j2,j3,j7,j8,j9))*\
    singvar((j4,j5,j6,-j7,-j8,-j9))+\
    Number(-1)*\
    polysum([\
      w(j)*singvar((j,j1,j2,-j4,-j5,j7))*\
      singvar((j,j2,j3,-j5,-j6,j9))*
      singvar((j,j3,j1,-j6,-j4,-j8))\
      for j in value_range])
  myideal.append(p)

print "now calculating"

print "original length",len(myideal)
myideal=singular.simplify(myideal,4)
print "simplified", len(myideal)
gb=singular.slimgb(myideal)
print gb
print len(gb), "GB Elemente"