LIB "tst.lib"; tst_init();
LIB "general.lib";
ring r= 0,(x,y,z),dp;
ideal m = maxideal(1);
product(m);
product(m[2..3]);
matrix M[2][3] = 1,x,2,y,3,z;
product(M);
intvec v=2,4,6;
product(M,v);
intvec iv = 1,2,3,4,5,6,7,8,9;
v=1..5,7,9;
product(iv,v);
intmat A[2][3] = 1,1,1,2,2,2;
product(A,3..5);
tst_status(1);$
