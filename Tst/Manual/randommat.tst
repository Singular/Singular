LIB "tst.lib"; tst_init();
LIB "random.lib";
ring r=0,(x,y,z),dp;
matrix A=randommat(3,3,maxideal(2),9);
print(A);
A=randommat(2,3);
print(A);
tst_status(1);$
