LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
printlevel = 3;
ring r = 0,(x,y),dp;
intmat A[1][2]=-1,1;
setBaseMultigrading(A);
isPositive();
intmat B[1][2]=1,1;
setBaseMultigrading(B);
isPositive(B);
tst_status(1);$
