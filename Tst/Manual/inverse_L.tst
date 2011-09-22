LIB "tst.lib"; tst_init();
LIB "linalg.lib";
ring r=0,(x,y),lp;
matrix A[3][3]=x,y,1,1,x2,y,x,6,0;
print(A);
list Inv=inverse_L(A);
print(Inv[1]);
print(Inv[2]);
print(Inv[1]*A);
tst_status(1);$
