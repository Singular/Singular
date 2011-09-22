LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring r =0,(x,y,z),dp;
intmat grad[2][3] =
1,0,1,
0,1,1;
setBaseMultigrading(grad);
poly f = x2y3-z5+x-3zx;
intmat M = defineHomogeneous(f);
M;
defineHomogeneous(f, grad) == M;
isHomogeneous(f);
setBaseMultigrading(grad, M);
isHomogeneous(f);
tst_status(1);$
