import re
import sys
from probstat import Cartesian
from optparse import OptionParser
from itertools import repeat
from string import Template
from re import sub
parser = OptionParser()
PB="PB"
SING="SING"
ALL="all"
parser.add_option("-f", "--format",
                  action="store", dest="format", type="string", default=ALL,
                  help="select format from SING/PB")
#parser.add_option("-c", "--cnf",
#                  action="store", dest="cnf", type="string", default=None,
#                  help="select input cnf")
parser.add_option("-i", "--invert",
                  action="store_true", dest="invert", default=False,
                  help="invert mapping to true/false")

def iscomment(line):
    return re.match("^c",line)

def isheader(line):
    return re.match("^p",line)


def process_input(source):
    global vars
    global clauses
    input_numbers=[]
    for line in source:
        if (not(iscomment(line) or isheader(line))):
            if (re.match("^%",line)):
                break
            line=re.sub("\n","", line)
            line=re.sub("^\s+","",line)

            ind=[int(s) for s in line.split()]

            input_numbers+=ind
            
        else:
            if(isheader(line)):
                m=re.match("^p cnf\s+(?P<vars>\d+)\s+(?P<clauses>\d+)\s*\n",line)
                assert(m)
                #print "vars=", m.group("vars")
                #print "clauses=",m.group("clauses")
                vars=int(m.group("vars"))
                clauses=int(m.group("clauses"))
    return input_numbers


def gen_clauses(input_numbers):
    i=0
    erg=[]
    act=[]
    for i in input_numbers:
        if (i==0):
            if act:
                erg.append(act)
                act=[]
        else:
            act.append(i)
    return erg
    
def xor(a,b):
    return (a and not b) or (b and not a)
def gen_poly_Singular(clause):
    def num2factor(f):
        assert(f!=0)
        if (f>0):
            return "".join(["x(",str(f),")"])
        else:
            return "".join(["(1-x(",str(-f),"))"])
    if clause:
        return("*".join( [num2factor(f) for f in clause]))

def gen_Singular(clauses):
    ring_def="".join(["ring r=2,x(1..",str(vars),"),lp;"])
    polys=[gen_poly_Singular(c) for c in clauses]
    polys.extend(["".join(["x(",str(i),")*(x(",str(i)+")-1)"]) for i in xrange(1,vars+1)])
    ideal="".join(["ideal i=",",\n".join(polys),";"])
    #command='LIB "digimult.lib";\n option(prot);\n satisfiable(i);'
    command='option(redTail);\noption(prot);\nstd(i);\n'
    return "\n".join([ring_def,ideal,command,"$;\n"])


def gen_poly_Magma(clause):
    def num2factor(f):
        assert(f!=0)
        if (f>0):
            return "".join(["x",str(f),""])
        else:
            return "".join(["(1-x",str(-f),")"])
    if clause:
        return("*".join( [num2factor(f) for f in clause]))

def gen_Magma(clauses):
    start_str=Template("""F:=FiniteField(2);
R< $var_list >:=PolynomialRing(F,$nvars,"lex");
i:=ideal< R | $ideal
>;
GroebnerBasis(i);
exit;
""")
    var_list=", ".join(["x"+str(i+1) for i in xrange(vars)])
    ideal=",\n".join([gen_poly_Magma(p) for p in clauses])
    ideal=ideal+",\n"+",\n".join(["x"+str(i)+"^2+x"+str(i) for i in xrange(1,vars+1)])
    return start_str.substitute({"nvars":str(vars), "var_list":var_list, "ideal":ideal})
    
def gen_poly_PB(clause):
    def term2string(t):
        if len(t)==0:
            return "1"
        return "*".join(["x("+str(v) +")" for v in t])
    
    vars=tuple([v for v in clause if v>0])
    negated_vars=tuple([-v for v in clause if v<0])
    if len(negated_vars)>0:
        terms=[tuple([negated_vars[i] for (i,j) in enumerate(combination) if j==1])\
            + vars for combination\
            in Cartesian(list(repeat([0,1],len(negated_vars))))]
    else:
        terms=[vars]
    res="+".join([term2string(t) for t in terms])
    return res
    #add_vars=[negated_var[i] for (i,j) in enumerate(combination) if j==1]
    
def gen_PB(clauses):
    start_str=Template("""from PyPolyBoRi import *
r=Ring($vars)
def x(i):
    return Monomial(Variable(i))
ideal=\
[
""")
    start_str=start_str.substitute(vars=vars)
    
    

    poly_str=",\\\n   ".join([gen_poly_PB(c) for c in clauses])
    end_str="""]
ideal=[Polynomial(p) for p in ideal]
"""
    return start_str+poly_str+end_str

from re import sub
def  convert_file_PB(cnf,invert):
    clauses=gen_clauses(process_input(open(cnf)))
    #clauses=gen_clauses(process_input(sys.stdin))
    if invert:
        clauses=[[-i for i in c] for c in clauses]
    #
#    print clauses
    #print gen_Singular(clauses)
    out_file_name=cnf[:-3]+"py"
    if invert:
        out_file_name=out_file_name[:-3]+"Inverted.py"
    out_file_name=sub("-","_",out_file_name)
    out=open(out_file_name,"w")
    out.write(gen_PB(clauses))
    out.close()
def  convert_file_Singular(cnf,invert):
    clauses=gen_clauses(process_input(open(cnf)))
    
    #clauses=gen_clauses(process_input(sys.stdin))
    if invert:
        clauses=[[-i for i in c] for c in clauses]
    #
#    print clauses
    #print gen_Singular(clauses)
    out_file_name=cnf[:-3]+"sing"
    if invert:
        out_file_name=out_file_name[:-5]+"Inverted.sing"
    out=open(out_file_name,"w")
    out.write(gen_Singular(clauses))
    out.close()


def  convert_file_Magma(cnf,invert):
    clauses=gen_clauses(process_input(open(cnf)))
    
    #clauses=gen_clauses(process_input(sys.stdin))
    if invert:
        clauses=[[-i for i in c] for c in clauses]
    #
#    print clauses
    #print gen_Singular(clauses)
    out_file_name=cnf[:-3]+"magma"
    if invert:
        out_file_name=out_file_name[:-6]+"Inverted.magma"
    out=open(out_file_name,"w")
    #print out
    out.write(gen_Magma(clauses))
    out.close()    
    
if __name__=='__main__':
    (options, args) = parser.parse_args()
    #clauses=gen_clauses(process_input(open(options.cnf)))
    #clauses=gen_clauses(process_input(sys.stdin))
    #if options.invert:
    #    clauses=[[-i for i in c] for c in clauses]
    #
#    print clauses
    #print gen_Singular(clauses)
    #out_file_name=options.cnf[:-3]+"py"
    #if options.invert:
    #    out_file_name=out_file_name[:-3]+"Inverted.py"
    #out=open(out_file_name,"w")
    #out.write(gen_PB(clauses))
    #out.close()
    for a in args:
        if options.format==ALL:
            convert_file_PB(a, options.invert)
            convert_file_Singular(a, options.invert)
            convert_file_Magma(a, options.invert)