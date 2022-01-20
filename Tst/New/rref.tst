LIB "tst.lib";
tst_init();

ring r;
matrix m[2][2]=1,2,3,4;
system("rref",m);
system("rref",smatrix(m));

ring R=QQ,x,dp;
matrix m[2][2]=1,2,3,4;
system("rref",m);
system("rref",smatrix(m));

tst_status(1);$
