LIB "tst.lib"; tst_init();
LIB "algebra.lib";
int p = printlevel; printlevel = 1;
ring R=0,(x,y,z),dp;
ideal P = x2+y2,z2,x4+y4;           //algebra generators
ideal M = 1,x2z-1y2z,xyz,x3y-1xy3;  //module generators
poly p1=
x10z3-x8y2z3+2x6y4z3-2x4y6z3+x2y8z3-y10z3+x6z4+3x4y2z4+3x2y4z4+y6z4;
module_containment(p1,P,M);
poly p2=z;
list l = module_containment(p2,P,M,1);
l[1];
def S = l[2]; setring S; check;
printlevel=p;
tst_status(1);$
