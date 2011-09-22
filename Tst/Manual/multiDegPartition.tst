LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
intmat g[2][3]=
1,0,1,
0,1,1;
intmat t[2][1]=
-2,
1;
setBaseMultigrading(g,t);
poly f = x10yz+x8y2z-x4z2+y5+x2y2-z2+x17z3-y6;
multiDegPartition(f);
vector v = xy*gen(1)-x3y2*gen(2)+x4y*gen(3);
intmat B[2][3]=1,-1,-2,0,0,1;
v = setModuleGrading(v,B);
getModuleGrading(v);
multiDegPartition(v, B);
tst_status(1);$
