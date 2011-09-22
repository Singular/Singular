LIB "tst.lib"; tst_init();
LIB "matrix.lib";
intmat A[3][3]=1,2,3,4,5,6,7,8,9;
print(power(A,3));"";
ring r=0,(x,y,z),dp;
matrix B[3][3]=0,x,y,z,0,0,y,z,0;
print(power(B,3));"";
tst_status(1);$
