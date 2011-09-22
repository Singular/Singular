LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
printlevel=3;
ring r = 0,(x,y,z),dp;
intmat g[2][3]=
1,0,1,
0,1,1;
intmat t[2][1]=
-2,
1;
setBaseMultigrading(g,t);
poly a = x10yz;
poly b = x8y2z;
poly c = x4z2;
poly d = y5;
poly e = x2y2;
poly f = z2;
equalMultiDeg(leadexp(a), leadexp(b));
equalMultiDeg(leadexp(a), leadexp(c));
equalMultiDeg(leadexp(a), leadexp(d));
equalMultiDeg(leadexp(a), leadexp(e));
equalMultiDeg(leadexp(a), leadexp(f));
equalMultiDeg(leadexp(b), leadexp(c));
equalMultiDeg(leadexp(b), leadexp(d));
equalMultiDeg(leadexp(b), leadexp(e));
equalMultiDeg(leadexp(b), leadexp(f));
equalMultiDeg(leadexp(c), leadexp(d));
equalMultiDeg(leadexp(c), leadexp(e));
equalMultiDeg(leadexp(c), leadexp(f));
equalMultiDeg(leadexp(d), leadexp(e));
equalMultiDeg(leadexp(d), leadexp(f));
equalMultiDeg(leadexp(e), leadexp(f));
tst_status(1);$
