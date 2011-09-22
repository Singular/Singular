LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x,y,z),lp;
matrix A[3][3]=
1,4,3,
1,5,7,
0,4,17;
print(inverse(A));"";
matrix B[3][3]=
y+1,  x+y,    y,
z,    z+1,    z,
y+z+2,x+y+z+2,y+z+1;
print(inverse(B));
print(B*inverse(B));
tst_status(1);$
