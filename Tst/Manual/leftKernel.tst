LIB "tst.lib"; tst_init();
LIB "control.lib";
ring r= 0,(x,y,z),dp;
matrix M[3][1] = x,y,z;
print(M);
matrix L = leftKernel(M);
print(L);
// check:
print(L*M);
tst_status(1);$
