LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring R=0,x,dp;
matrix M[3][3]=3,2,1,0,2,1,0,0,3;
print(M);
print(jordannf(M));
tst_status(1);$
