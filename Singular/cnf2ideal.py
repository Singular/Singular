import re
import sys
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
    ring_def="".join(["ring r=2,x(1..",str(vars),"),dp;"])
    polys=[gen_poly_Singular(c) for c in clauses]
    polys.extend(["".join(["x(",str(i),")*(x(",str(i)+")-1)"]) for i in xrange(1,vars+1)])
    ideal="".join(["ideal i=",",\n".join(polys),";"])
    command="std(i);"
    return "\n".join([ring_def,ideal,command,"$;\n"])
if __name__=='__main__':        
    clauses=gen_clauses(process_input(sys.stdin))
#    print clauses
    print gen_Singular(clauses)
