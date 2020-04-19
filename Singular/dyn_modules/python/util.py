from Singular import *
from interpreter import *
singular=SingularGlobalsProxy()

def create_ring(char=0, nvars=1, varNames=None, ordering="dp", degvec=None, register=None):
  if not varNames:
    varNames=["x("+str(i)+")" for i in range(1,nvars+1)]
  else:
    nvars=len(varNames)
  if not degvec:
    degvec=IntVector()
    for i in range(0,nvars):
      degvec.append(1)
  modulweights=IntVector()
  modulweights.append(0)
  l=[char,varNames,[[ordering,degvec],["C",modulweights]], Ideal()]
  res=singular.ring(l)
  if register is None:
    return res
  else:
    old_ring=Ring()
    res.set()
    for i in range(nvars):
        v=singular.var(i+1)
        register[str(v)]=v

    old_ring.set()
    return res


class EquivalenceRelation(object):
  """this class represents an equivalent relation,
    which is implemented by repeated evaluatation of a map
    c[n]=m means that n is equivalent to m"""
  def __init__(self):
    self.mydict={}
  def canonical(self,var):
    while var in self.mydict:
      var=self.mydict[var]
    return var
  def isCanonical(self,var):
    if self.canonical(var)==var:
        return True
    else:
        return False
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