LIB "tst.lib"; tst_init();
LIB "normal.lib";
printlevel = printlevel+1;
ring s = 0,(x,y),dp;
ideal i = (x2-y3)*(x2+y2)*x;
list nor = normalC(i);
nor;
// 2 branches have delta = 1, and 1 branch has delta = 0
// the total delta invariant is 13
def R2 = nor[1][2];  setring R2;
norid; normap;
printlevel = printlevel-1;
ring r = 2,(x,y,z),dp;
ideal i = z3-xy4;
nor = normalC(i);  nor;
// the delta invariant is infinite
// xy2z/z2 and xy3/z2 generate the integral closure of r/i as r/i-module
// in its quotient field Quot(r/i)
// the normalization as affine algebra over the ground field:
def R = nor[1][1]; setring R;
norid; normap;
setring r;
nor = normalC(i, "withGens", "prim");    // a different algorithm
nor;
tst_status(1);$
