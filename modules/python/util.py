from Singular import *
from interpreter import *
singular=singular_globals_proxy()

def create_ring(char=0, nvars=1, varNames=None, ordering="dp", degvec=None):
  if not varNames:
    varNames=["x("+str(i)+")" for i in xrange(1,nvars+1)]
  else:
    nvars=len(varNames)
  if not degvec:
    degvec=intvec()
    for i in xrange(0,nvars):
      degvec.append(1)
  print degvec
  modulweights=intvec()
  modulweights.append(0)
  print modulweights
  l=[char,varNames,[[ordering,degvec],["C",modulweights]], ideal()]
  #print l
  return singular.ring(l)
  