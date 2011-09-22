LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring R=0,x,dp;
ideal e=ideal(2,3);
intvec s=1,2;
intvec m=1,1;
print(jordanmatrix(list(e,s,m)));
tst_status(1);$
