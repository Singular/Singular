LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r = 0,(x,y,z),dp;
intmat g[2][3] = 1,0,1,0,1,1;
intmat l[2][1] = 3,0;
setBaseMultigrading(g,l);
getLattice();
ideal i = -y5+x4,
y6+xz,
x2y;
gradiator(i);
getLattice();
isHomogeneous(i);
tst_status(1);$
