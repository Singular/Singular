LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring r  = 11,(x,y,z),wp(2,1,2);
ideal i = x*(z3 - xy4 + x2);
list nor= normalP(i); nor;
//the result says that both components of i are normal, but i itself
//has infinite delta
ring s = 2,(x,y),dp;
ideal i = y*((x-y^2)^2 - x^3);
list nor = normalP(i,"withRing"); nor;
def R2  = nor[1][2]; setring R2;
norid; normap;
tst_status(1);$
