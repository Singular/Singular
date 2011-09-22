LIB "tst.lib"; tst_init();
LIB "control.lib";
ring r = 0,(x,y,z),dp;
matrix M[1][3] = x,y,z;
print(M);
matrix R = rightKernel(M);
print(R);
// check:
print(M*R);
tst_status(1);$
