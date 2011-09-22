LIB "tst.lib"; tst_init();
LIB "algebra.lib";
int p = printlevel; printlevel = 1;
ring R = 0,(x,y,z),dp;
ideal A=x2+y2,z2,x4+y4,1,x2z-1y2z,xyz,x3y-1xy3;
poly p1=z;
poly p2=
x10z3-x8y2z3+2x6y4z3-2x4y6z3+x2y8z3-y10z3+x6z4+3x4y2z4+3x2y4z4+y6z4;
algebra_containment(p1,A);
algebra_containment(p2,A);
list L = algebra_containment(p2,A,1);
L[1];
def S = L[2]; setring S;
check;
printlevel = p;
tst_status(1);$
