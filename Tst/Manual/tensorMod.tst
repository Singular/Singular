LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring A=0,(x,y,z),dp;
matrix M[3][3]=1,2,3,4,5,6,7,8,9;
matrix N[2][2]=x,y,0,z;
print(M);
print(N);
print(tensorMod(M,N));
tst_status(1);$
