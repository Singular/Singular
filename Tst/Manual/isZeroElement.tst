LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
intmat g[2][3]=
1,0,1,
0,1,1;
intmat t[2][1]=
-2,
1;
intmat tt[2][1]=
1,
-1;
setBaseMultigrading(g,t);
poly a = x10yz;
poly b = x8y2z;
poly c = x4z2;
poly d = y5;
poly e = x2y2;
poly f = z2;
intvec v1 = multiDeg(a) - multiDeg(b);
v1;
isZeroElement(v1);
isZeroElement(v1, tt);
intvec v2 = multiDeg(a) - multiDeg(c);
v2;
isZeroElement(v2);
isZeroElement(v2, tt);
intvec v3 = multiDeg(e) - multiDeg(f);
v3;
isZeroElement(v3);
isZeroElement(v3, tt);
intvec v4 = multiDeg(c) - multiDeg(d);
v4;
isZeroElement(v4);
isZeroElement(v4, tt);
tst_status(1);$
